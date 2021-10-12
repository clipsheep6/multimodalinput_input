/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UDSSession" };
    }
}

OHOS::MMI::UDSSession::UDSSession(int32_t fd, sockaddr_un& addr) : fd_(fd), addr_(addr)
{
}

OHOS::MMI::UDSSession::~UDSSession()
{
}

bool OHOS::MMI::UDSSession::SendMsg(const char *buf, size_t size) const
{
    CHKF(buf, OHOS::NULL_POINTER);
    CHKF(size > 0 && size <= MAX_PACKET_BUF_SIZE, PARAM_INPUT_INVALID);
    CHKF(fd_ >= 0, PARAM_INPUT_INVALID);
    auto ret = write(fd_, static_cast<void *>(const_cast<char *>(buf)), size);
    if (ret < 0) {
        MMI_LOGE("UDSSession::SendMsg write return %{public}d", ret);
        return false;
    }
    return true;
}

void OHOS::MMI::UDSSession::Close()
{
    MMI_LOGT("enter fd_ = %{public}d.", fd_);
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

bool OHOS::MMI::UDSSession::SendMsg(NetPacket& pkt) const
{
    StreamBuffer buf;
    pkt.MakeData(buf);
    return SendMsg(buf.Data(), buf.Size());
}
