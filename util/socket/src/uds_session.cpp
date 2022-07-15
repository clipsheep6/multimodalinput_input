/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uds_session.h"

#include <cinttypes>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "uds_socket.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int64_t INPUT_UI_TIMEOUT_TIME = 5 * 1000000;
const std::string FOUNDATION = "foundation";
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UDSSession" };
} // namespace

UDSSession::UDSSession(const std::string& programName, const int32_t moduleType, const int32_t fd,
    const int32_t uid, const int32_t pid)
    : programName_(programName),
      moduleType_(moduleType),
      fd_(fd),
      uid_(uid),
      pid_(pid)
{
    UpdateDescript();
}

bool UDSSession::SendMsg(const char *buf, size_t size) const
{
    CHKPF(buf);
    if ((size == 0) || (size > MAX_PACKET_BUF_SIZE)) {
        MMI_HILOGE("buf size:%{public}zu", size);
        return false;
    }
    if (fd_ < 0) {
        MMI_HILOGE("fd_ is less than 0");
        return false;
    }

    int32_t idx = 0;
    int32_t retryCount = 0;
    const int32_t bufSize = static_cast<int32_t>(size);
    int32_t remSize = bufSize;
    while (remSize > 0 && retryCount < SEND_RETRY_LIMIT) {
        retryCount += 1;
        auto count = send(fd_, &buf[idx], remSize, MSG_DONTWAIT | MSG_NOSIGNAL);
        if (count < 0) {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
                MMI_HILOGW("continue for errno EAGAIN|EINTR|EWOULDBLOCK, errno:%{public}d", errno);
                usleep(SEND_RETRY_SLEEP_TIME);
                continue;
            }
            MMI_HILOGE("Send return failed,error:%{public}d fd:%{public}d", errno, fd_);
            return false;
        }
        idx += count;
        remSize -= count;
        if (remSize > 0) {
            usleep(SEND_RETRY_SLEEP_TIME);
        }
    }
    if (retryCount >= SEND_RETRY_LIMIT || remSize != 0) {
        MMI_HILOGE("Send too many times:%{public}d/%{public}d,size:%{public}d/%{public}d fd:%{public}d",
            retryCount, SEND_RETRY_LIMIT, idx, bufSize, fd_);
        return false;
    }
    return true;
}

void UDSSession::Close()
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("enter fd_:%{public}d.", fd_);
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
        UpdateDescript();
    }
}

void UDSSession::UpdateDescript()
{
    std::ostringstream oss;
    oss << "fd = " << fd_
        << ", programName = " << programName_
        << ", moduleType = " << moduleType_
        << ((fd_ < 0) ? ", closed" : ", opened")
#ifdef OHOS_BUILD_MMI_DEBUG
        << ", clientFd = " << clientFd_
#endif // OHOS_BUILD_MMI_DEBUG
        << ", uid = " << uid_
        << ", pid = " << pid_
        << std::endl;
    descript_ = oss.str().c_str();
}

bool UDSSession::SendMsg(NetPacket& pkt) const
{
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Read and write status is error");
        return false;
    }
    StreamBuffer buf;
    pkt.MakeData(buf);
    return SendMsg(buf.Data(), buf.Size());
}

void UDSSession::SaveANREvent(int32_t id, int64_t time)
{
    CALL_DEBUG_ENTER;
    if (GetTokenType() == TokenType::TOKEN_NATIVE || GetProgramName() == FOUNDATION) {
        MMI_HILOGD("Is native event");
        return;
    }
    EventTime eventTime = {id, time};
    events_.push_back(eventTime);
}

void UDSSession::DelEvents(int32_t id)
{
    CALL_DEBUG_ENTER;
    int32_t count = 0;
    for (auto &item : events_) {
        ++count;
        if (item.id == id) {
            events_.erase(events_.begin(), events_.begin() + count);
            MMI_HILOGD("Delete events");
            break;
        }
    }
    if (events_.empty()) {
        isANRProcess_ = false;
        return;
    }
    int64_t endTime = 0;
    if (!AddInt64(events_.begin()->eventTime, INPUT_UI_TIMEOUT_TIME, endTime)) {
        MMI_HILOGE("The addition of endTime overflows");
        return;
    }
    auto currentTime = GetSysClockTime();
    if (currentTime < endTime) {
        isANRProcess_ = false;
    }
}

int64_t UDSSession::GetEarliestEventTime() const
{
    CALL_DEBUG_ENTER;
    if (events_.empty()) {
        MMI_HILOGD("events_ is empty");
        return 0;
    }
    return events_.begin()->eventTime;
}

bool UDSSession::IsEventQueueEmpty()
{
    if (events_.empty()) {
        MMI_HILOGD("events_ is empty");
        return true;
    }
    return false;
}
} // namespace MMI
} // namespace OHOS