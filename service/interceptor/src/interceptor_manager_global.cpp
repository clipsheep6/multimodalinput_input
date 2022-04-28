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

#include "input_event_data_transformation.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InterceptorManagerGlobal" };
constexpr int32_t SOURCETYPE_KEY = 4;
} // namespace

InterceptorManagerGlobal::InterceptorManagerGlobal() {}

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

bool InterceptorManagerGlobal::OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_LOG_ENTER;
    CHKPF(pointerEvent);
    if (interceptors_.empty()) {
        MMI_HILOGW("%{public}s no interceptor to send msg", __func__);
        return false;
    }
    PointerEvent::PointerItem pointer;
    if (!(pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointer))) {
        MMI_HILOGE("The obtained pointer parameter is invalid");
        return false;
    }
    MMI_HILOGD("Interceptor-servereventTouchpad:actionTime:%{public}" PRId64 ","
               "sourceType:%{public}d,pointerAction:%{public}d,"
               "pointer:%{public}d,point.x:%{public}d,point.y:%{public}d,press:%{public}d",
               pointerEvent->GetActionTime(), pointerEvent->GetSourceType(), pointerEvent->GetPointerAction(),
               pointerEvent->GetPointerId(), pointer.GetGlobalX(), pointer.GetGlobalY(), pointer.IsPressed());
    for (const auto &item : interceptors_) {
        NetPacket pkt(MmiMessageId::TOUCHPAD_EVENT_INTERCEPTOR);
        InputEventDataTransformation::Marshalling(pointerEvent, pkt);
        pkt << item.session->GetPid() << item.id;
        MMI_HILOGD("server send the interceptor msg to client, pid:%{public}d", item.session->GetPid());
        item.session->SendMsg(pkt);
    }
    return true;
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
} // namespace MMI
} // namespace OHOS