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
    HandleEventType eventType, int32_t priority, uint32_t deviceTags)
{
    CHKPR(interceptor, INVALID_HANDLER_ID);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    if (interHandlers_.size() >= MAX_N_INPUT_HANDLERS) {
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
    if (RET_OK == AddLocal(handlerId, eventType, priority, deviceTags, interceptor)) {
        MMI_HILOGD("New handler successfully registered, report to server");
        const HandleEventType newType = GetEventType();
        if (currentType != newType) {
            int32_t ret = AddToServer(newType, priority, deviceTags);
            if (ret != RET_OK) {
                MMI_HILOGD("Handler:%{public}d permissions failed, remove the monitor", handlerId);
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
    int32_t priority, uint32_t deviceTags, std::shared_ptr<IInputEventConsumer> interceptor)
{
    InputInterceptor::InterceptorHandler handler {
        .handlerId_ = handlerId,
        .eventType_ = eventType,
        .priority_ = priority,
        .deviceTags_ = deviceTags,
        .consumer_ = interceptor,
    };
    auto iterIndex = interHandlers_.begin();
    for (; iterIndex != interHandlers_.end(); ++iterIndex) {
        if (handler.priority_ < iterIndex->priority_) {
            break;
        }
    }
    auto iter = interHandlers_.emplace(iterIndex, handler);
    if (iter == interHandlers_.end()) {
        MMI_HILOGE("Add new interceptor failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputInterceptor::AddToServer(HandleEventType eventType, int32_t priority, uint32_t deviceTags)
{
    int32_t ret = MultimodalInputConnMgr->AddInterceptorHandler(eventType, priority, deviceTags);
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
            const int32_t newLevel = GetPriority();
            const uint64_t newTags = GetDeviceTags();
            RemoveFromServer(newType, newLevel, newTags);
        }
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputInterceptor::OnInputEvent(std::shared_ptr<KeyEvent> keyEvent, uint32_t deviceTags)
{
    CHK_PID_AND_TID();
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_INTERCEPT_EVENT);
    for (const auto &item : interHandlers_) {
        if ((item.deviceTags_ !=  deviceTags) &&
            ((item.eventType_ & HANDLE_EVENT_TYPE_KEY) != HANDLE_EVENT_TYPE_KEY)) {
            continue;
        }
        auto consumer = item.consumer_;
        CHKPV(consumer);
        consumer->OnInputEvent(keyEvent);
        MMI_HILOGD("Key event id:%{public}d keyCode:%{public}d", item.handlerId_, keyEvent->GetKeyCode());
        break;
    }
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputInterceptor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent, uint32_t deviceTags)
{
    CHK_PID_AND_TID();
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_INTERCEPT_EVENT);
    for (const auto &item : interHandlers_) {
        if ((item.deviceTags_ !=  deviceTags) &&
            ((item.eventType_ & HANDLE_EVENT_TYPE_POINTER) != HANDLE_EVENT_TYPE_POINTER)) {
            continue;
        }
        auto consumer = item.consumer_;
        CHKPV(consumer);
        consumer->OnInputEvent(pointerEvent);
        MMI_HILOGD("Pointer event id:%{public}d pointerId:%{public}d", item.handlerId_, pointerEvent->GetPointerId());
        break;
    }
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

int32_t InputInterceptor::RemoveLocal(int32_t handlerId)
{
    for (auto it = interHandlers_.begin(); it != interHandlers_.end(); ++it) {
        if (handlerId == it->handlerId_) {
            interHandlers_.erase(it);
            break;
        }
    }
    return RET_OK;
}

void InputInterceptor::RemoveFromServer(HandleEventType eventType, int32_t priority, uint32_t deviceTags)
{
    int32_t ret = MultimodalInputConnMgr->RemoveInterceptorHandler(eventType, priority, deviceTags);
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
        int32_t priority = GetPriority();
        uint32_t deviceTags = GetDeviceTags();
        AddToServer(eventType, priority, deviceTags);
    }
}

HandleEventType InputInterceptor::GetEventType() const
{
    if (interHandlers_.empty()) {
        MMI_HILOGD("interHandlers_ is empty");
        return HANDLE_EVENT_TYPE_NONE;
    }
    HandleEventType eventType { HANDLE_EVENT_TYPE_NONE };
    eventType |= interHandlers_.front().eventType_;
    return eventType;
}

int32_t InputInterceptor::GetPriority() const
{
    if (interHandlers_.empty()) {
        MMI_HILOGD("InputHandlers is empty");
        return DEFUALT_INTERCEPTOR_PRIORITY;
    }
    int32_t priority { DEFUALT_INTERCEPTOR_PRIORITY };
    priority = interHandlers_.front().priority_;
    return priority;
}

uint32_t InputInterceptor::GetDeviceTags() const
{
    if (interHandlers_.empty()) {
        MMI_HILOGD("InputHandlers is empty");
        return DEFUALT_INTERCEPTOR_PRIORITY;
    }
    uint32_t deviceTags { CapabilityToTags(InputDeviceCapability::INPUT_DEV_CAP_MAX) };
    deviceTags = interHandlers_.front().deviceTags_;
    return deviceTags;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputInterceptor::ReportInterceptorKey(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    uint32_t deviceTags;
    pkt >> deviceTags;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read handler failed");
        return RET_ERR;
    }
    auto keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize key event.");
        return RET_ERR;
    }
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_INTERCEPT_EVENT);
    OnInputEvent(keyEvent, deviceTags);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t InputInterceptor::ReportInterceptorPointer(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    uint32_t deviceTags;
    pkt >> deviceTags;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read Pointer data failed");
        return RET_ERR;
    }
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize pointer event");
        return RET_ERR;
    }
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START, BytraceAdapter::POINT_INTERCEPT_EVENT);
    OnInputEvent(pointerEvent, deviceTags);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS
