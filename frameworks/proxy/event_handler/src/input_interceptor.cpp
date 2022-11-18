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

#include "input_interceptor.h"

#include "bytrace_adapter.h"
#include "input_event_data_transformation.h"
#include "input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputInterceptor" };
} // namespace

int32_t InputInterceptor::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor,
    HandleEventType eventType)
{
    CHKPR(interceptor, INVALID_HANDLER_ID);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    if (inputHandlers_.size() >= MAX_N_INPUT_HANDLERS) {
        MMI_HILOGE("The number of handlers exceeds the maximum");
        return ERROR_EXCEED_MAX_COUNT;
    }
    int32_t handlerId = GetNextId();
    if (handlerId == INVALID_HANDLER_ID) {
        MMI_HILOGE("Exceeded limit of 32-bit maximum number of integers");
        return INVALID_HANDLER_ID;
    }
    if (eventType == HANDLE_EVENT_TYPE_NONE) {
        MMI_HILOGE("Invalid event type");
        return INVALID_HANDLER_ID;
    }
    const HandleEventType currentType = GetEventType();
    MMI_HILOGD("Register new handler:%{public}d", handlerId);
    if (RET_OK == AddLocal(handlerId, eventType, interceptor)) {
        MMI_HILOGD("New handler successfully registered, report to server");
        const HandleEventType newType = GetEventType();
        if (currentType != newType) {
            int32_t ret = AddToServer(newType);
            if (ret != RET_OK) {
                MMI_HILOGD("Handler:%{public}d permissions failed, remove the interceptor", handlerId);
                RemoveLocal(handlerId);
                return ret;
            }
        }
    } else {
        handlerId = INVALID_HANDLER_ID;
    }
    return handlerId;
}

int32_t InputInterceptor::AddLocal(int32_t handlerId, HandleEventType eventType,
    std::shared_ptr<IInputEventConsumer> interceptor)
{
    InputInterceptor::InterceptorHandler handler {
        .handlerId_ = handlerId,
        .eventType_ = eventType,
        .consumer_ = interceptor,
    };
    auto ret = inputHandlers_.emplace(handler.handlerId_, handler);
    if (!ret.second) {
        MMI_HILOGE("Duplicate handler:%{public}d", handler.handlerId_);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputInterceptor::AddToServer(HandleEventType eventType)
{
    int32_t ret = MultimodalInputConnMgr->AddInterceptorHandler(eventType);
    if (ret != RET_OK) {
        MMI_HILOGE("Add interceptor failed, ret:%{public}d", ret);
    }
    return ret;
}

void InputInterceptor::RemoveInterceptor(int32_t interceptorId)
{
    CALL_INFO_TRACE;
    MMI_HILOGD("Unregister handler:%{public}d", interceptorId);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    const HandleEventType currentType = GetEventType();
    if (RET_OK == RemoveLocal(interceptorId)) {
        MMI_HILOGD("Handler:%{public}d unregistered, report to server", interceptorId);
        const HandleEventType newType = GetEventType();
        if (currentType != newType) {
            RemoveFromServer(newType);
        }
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputInterceptor::OnInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_INTERCEPT_EVENT);
    for (const auto &handler : inputHandlers_) {
        if ((handler.second.eventType_ & HANDLE_EVENT_TYPE_KEY) != HANDLE_EVENT_TYPE_KEY) {
            continue;
        }
        auto consumer = handler.second.consumer_;
        CHKPV(consumer);
        consumer->OnInputEvent(keyEvent);
        MMI_HILOGD("Key event id:%{public}d keyCode:%{public}d", handler.first, keyEvent->GetKeyCode());
    }
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputInterceptor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PID_AND_TID();
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_INTERCEPT_EVENT);
    for (const auto &iter : inputHandlers_) {
        if ((iter.second.eventType_ & HANDLE_EVENT_TYPE_POINTER) != HANDLE_EVENT_TYPE_POINTER) {
            continue;
        }
        auto consumer = iter.second.consumer_;
        CHKPV(consumer);
        consumer->OnInputEvent(pointerEvent);
        MMI_HILOGD("Pointer event id:%{public}d pointerId:%{public}d", iter.first, pointerEvent->GetPointerId());
    }
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

int32_t InputInterceptor::RemoveLocal(int32_t handlerId)
{
    auto iter = inputHandlers_.find(handlerId);
    if (iter == inputHandlers_.end()) {
        MMI_HILOGE("No handler with specified");
        return RET_ERR;
    }
    inputHandlers_.erase(iter);
    return RET_OK;
}

void InputInterceptor::RemoveFromServer(HandleEventType eventType)
{
    int32_t ret = MultimodalInputConnMgr->RemoveInterceptorHandler(eventType);
    if (ret != 0) {
        MMI_HILOGE("Remove interceptor failed, ret:%{public}d", ret);
    }
}

int32_t InputInterceptor::GetNextId()
{
    if (nextId_ == std::numeric_limits<int32_t>::max()) {
        MMI_HILOGE("Exceeded limit of 32-bit maximum number of integers");
        return INVALID_HANDLER_ID;
    }
    return nextId_++;
}

void InputInterceptor::OnConnected()
{
    CALL_DEBUG_ENTER;
    HandleEventType eventType = GetEventType();
    if (eventType != HANDLE_EVENT_TYPE_NONE) {
        AddToServer(eventType);
    }
}

HandleEventType InputInterceptor::GetEventType() const
{
    if (inputHandlers_.empty()) {
        MMI_HILOGD("InterceptorHandlers is empty");
        return HANDLE_EVENT_TYPE_NONE;
    }
    HandleEventType eventType { HANDLE_EVENT_TYPE_NONE };
    for (const auto &inputHandler : inputHandlers_) {
        eventType |= inputHandler.second.eventType_;
    }
    return eventType;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputInterceptor::ReportInterceptorKey(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    auto keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize key event.");
        return RET_ERR;
    }
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_INTERCEPT_EVENT);
    OnInputEvent(keyEvent);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t InputInterceptor::ReportInterceptorPointer(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize pointer event");
        return RET_ERR;
    }
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START, BytraceAdapter::POINT_INTERCEPT_EVENT);
    OnInputEvent(pointerEvent);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS
