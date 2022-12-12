/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "event_queue.h"

#include <errors.h>

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventQueue" };
int32_t g_hdfAdapterWriteFd { -1 };
int32_t g_mmiServiceReadFd { -1 };
} // namespace

EventQueue::EventQueue(int32_t id) : IEventQueue(id)
{}

int32_t EventQueue::Init()
{
    CALL_DEBUG_ENTER;
    do {
        int32_t fds[2] = {-1, -1};
        auto ret = pipe(fds);
        if (ret != 0) {
            int saveErrno = errno;
            MMI_HILOGE("Create pipe error, errno: %{public}d, %{public}s", saveErrno, strerror(saveErrno));
            break;
        }
        g_mmiServiceReadFd = fds[0];
        g_hdfAdapterWriteFd = fds[1];
        MMI_HILOGI("Connect hdf init, fds:(read:%{public}d, write:%{public}d)", fds[0], fds[1]);
        return RET_OK;
    }while(0);
    ReleasePipe();
    return RET_ERR;
}

int32_t EventQueue::RegisterEventHandler(std::shared_ptr<IEventHandler> handler)
{
    handler_ = handler;
    return RET_OK; 
}

int32_t EventQueue::UnregisterEventHandler()
{
    handler_ = nullptr;
    return RET_OK;
}

int32_t EventQueue::SendEvent(EventData &event)
{
    if (event.type  == 0) {
        event.handler = handler_;
    }
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(event));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write failed, errno: %{public}d, %{public}s", saveErrno, strerror(saveErrno));
        return RET_ERR;
    }
    return RET_OK;
}

int32_t EventQueue::SetDefaultHandler(std::function<void(int32_t, void *, size_t)>)
{
    return -1;
}

void EventQueue::ReleasePipe()
{
    CALL_DEBUG_ENTER;
    int32_t ret = 0;
    if (g_hdfAdapterWriteFd != -1) {
        ret = close(g_hdfAdapterWriteFd);
        if (ret != RET_OK) {
            MMI_HILOGE("Close fd failed, write fd:%{public}d, ret:%{public}d, errno:%{public}d", g_hdfAdapterWriteFd, ret, errno); 
        }
        g_hdfAdapterWriteFd = -1;
    }
    if (g_mmiServiceReadFd != -1) {
        ret = close(g_mmiServiceReadFd);
        if (ret != RET_OK) {
            MMI_HILOGE("Close fd failed, read fd:%{public}d, ret:%{public}d, errno:%{public}d", g_mmiServiceReadFd, ret, errno);
        }
        g_mmiServiceReadFd = -1;
    }
}

int32_t EventQueue::GetInputFd() const
{
    return g_mmiServiceReadFd;
}
} // namespace MMI
} // namespace OHOS