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

#include "interceptor_manager_global.h"

#include <cinttypes>

#include "bytrace_adapter.h"
#include "input_event_data_transformation.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InterceptorManagerGlobal" };
constexpr int32_t SOURCETYPE_KEY = 4;
} // namespace

InterceptorManagerGlobal::InterceptorManagerGlobal(int32_t priority) : IInterceptorManagerGlobal(priority) {}

int32_t InterceptorManagerGlobal::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    if (!keyEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT)) {
        if (OnKeyEvent(keyEvent)) {
            MMI_HILOGD("keyEvent filter find a keyEvent from Original event keyCode: %{puiblic}d",
                keyEvent->GetKeyCode());
            BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_INTERCEPT_EVENT);
            return RET_OK;
        }
    }
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandleKeyEvent(keyEvent);
}

void InterceptorManagerGlobal::OnAddInterceptor(int32_t sourceType, int32_t id, SessionPtr session)
{
    CALL_LOG_ENTER;
    CHKPV(session);
    InterceptorItem interceptorItem = {};
    interceptorItem.sourceType = sourceType;
    interceptorItem.id = id;
    interceptorItem.session = session;
    auto iter = std::find(interceptors_.begin(), interceptors_.end(), interceptorItem);
    if (iter != interceptors_.end()) {
        MMI_HILOGE("touchpad event repeate register");
        return;
    }
    iter = interceptors_.insert(iter, interceptorItem);
    MMI_HILOGD("sourceType:%{public}d,fd:%{public}d register in server", sourceType, session->GetFd());
}

void InterceptorManagerGlobal::OnRemoveInterceptor(int32_t id)
{
    CALL_LOG_ENTER;
    InterceptorItem interceptorItem = {};
    interceptorItem.id = id;
    auto iter = std::find(interceptors_.begin(), interceptors_.end(), interceptorItem);
    if (iter == interceptors_.end()) {
        MMI_HILOGE("interceptorItem does not exist");
        return;
    }
    MMI_HILOGD("sourceType:%{public}d,fd:%{public}d remove from server", iter->sourceType,
               iter->session->GetFd());
    interceptors_.erase(iter);
}

bool InterceptorManagerGlobal::OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    if (interceptors_.empty()) {
        MMI_HILOGE("%{public}s no interceptor to send msg", __func__);
        return false;
    }
    for (const auto &item : interceptors_) {
        if (item.sourceType == SOURCETYPE_KEY) {
            NetPacket pkt(MmiMessageId::KEYBOARD_EVENT_INTERCEPTOR);
            InputEventDataTransformation::KeyEventToNetPacket(keyEvent, pkt);
            pkt << item.session->GetPid();
            MMI_HILOGD("server send the interceptor msg to client, pid:%{public}d", item.session->GetPid());
            item.session->SendMsg(pkt);
        }
    }
    return true;
}

std::shared_ptr<IInterceptorManagerGlobal> IInterceptorManagerGlobal::CreateInstance(int32_t priority)
{
    return std::make_shared<InterceptorManagerGlobal>(priority);
}
} // namespace MMI
} // namespace OHOS