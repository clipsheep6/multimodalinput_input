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

#include "input_monitor.h"

#include "bytrace_adapter.h"
#include "input_event_data_transformation.h"
#include "input_connect_manager.h"
#include "input_manager_impl.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputMonitor" };
} // namespace

InputMonitor::InputMonitor()
{
    monitorCallback_ = std::bind(&InputMonitor::OnDispatchEventProcessed, this, std::placeholders::_1,
        std::placeholders::_2);
}

int32_t InputMonitor::AddMonitor(std::shared_ptr<IInputEventConsumer> monitor,
    HandleEventType eventType)
{
    CHKPR(monitor, INVALID_HANDLER_ID);
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
    if (RET_OK == AddLocal(handlerId, eventType, monitor)) {
        MMI_HILOGD("New handler successfully registered, report to server");
        const HandleEventType newType = GetEventType();
        if (currentType != newType) {
            int32_t ret = AddToServer(newType);
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

int32_t InputMonitor::AddLocal(int32_t handlerId, HandleEventType eventType,
    std::shared_ptr<IInputEventConsumer> monitor)
{
    InputMonitor::MonitorHandler handler {
        .handlerId_ = handlerId,
        .eventType_ = eventType,
        .consumer_ = monitor,
    };
    auto ret = inputHandlers_.emplace(handler.handlerId_, handler);
    if (!ret.second) {
        MMI_HILOGE("Duplicate handler:%{public}d", handler.handlerId_);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputMonitor::AddToServer(HandleEventType eventType)
{
    int32_t ret = MultimodalInputConnMgr->AddMonitorHandler(eventType);
    if (ret != RET_OK) {
        MMI_HILOGE("Add monitor failed, ret:%{public}d", ret);
    }
    return ret;
}

void InputMonitor::RemoveMonitor(int32_t monitorId)
{
    CALL_INFO_TRACE;
    MMI_HILOGD("Unregister handler:%{public}d", monitorId);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    const HandleEventType currentType = GetEventType();
    if (RET_OK == RemoveLocal(monitorId)) {
        MMI_HILOGD("Handler:%{public}d unregistered, report to server", monitorId);
        const HandleEventType newType = GetEventType();
        if (currentType != newType) {
            RemoveFromServer(newType);
        }
    }
}

void InputMonitor::MarkConsumed(int32_t monitorId, int32_t eventId)
{
    MMI_HILOGD("Mark consumed state, monitor:%{public}d,event:%{public}d", monitorId, eventId);
    if (!HasHandler(monitorId)) {
        MMI_HILOGW("Failed to find the monitorId");
        return;
    }
    int32_t ret = MultimodalInputConnMgr->MarkEventConsumed(eventId);
    if (ret != RET_OK) {
        MMI_HILOGE("Send to server failed, ret:%{public}d", ret);
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputMonitor::OnInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_INTERCEPT_EVENT);
    for (const auto &handler : inputHandlers_) {
        if ((handler.second.eventType_ & HANDLE_EVENT_TYPE_KEY) != HANDLE_EVENT_TYPE_KEY) {
            continue;
        }
        int32_t handlerId = handler.first;
        auto consumer = handler.second.consumer_;
        CHKPV(consumer);
        consumer->OnInputEvent(keyEvent);
        MMI_HILOGD("Key event id:%{public}d keyCode:%{public}d", handlerId, keyEvent->GetKeyCode());
    }
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputMonitor::GetConsumerInfos(std::shared_ptr<PointerEvent> pointerEvent,
    std::map<int32_t, std::shared_ptr<IInputEventConsumer>> &consumerInfos)
{
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    int32_t consumerCount = 0;
    for (const auto &iter : inputHandlers_) {
        if ((iter.second.eventType_ & HANDLE_EVENT_TYPE_POINTER) != HANDLE_EVENT_TYPE_POINTER) {
            continue;
        }
        int32_t handlerId = iter.first;
        auto consumer = iter.second.consumer_;
        CHKPV(consumer);
        auto ret = consumerInfos.emplace(handlerId, consumer);
        if (!ret.second) {
            MMI_HILOGI("Duplicate handler:%{public}d", handlerId);
            continue;
        }
        consumerCount++;
    }
    if (consumerCount == 0) {
        MMI_HILOGE("All task post failed");
        return;
    }
    int32_t tokenType = MultimodalInputConnMgr->GetTokenType();
    if (tokenType != TokenType::TOKEN_HAP) {
        return;
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
        mouseEventIds_.emplace(pointerEvent->GetId());
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        processedEvents_.emplace(pointerEvent->GetId(), consumerCount);
    }
}

void InputMonitor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PID_AND_TID();
    CHKPV(pointerEvent);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_INTERCEPT_EVENT);
    std::map<int32_t, std::shared_ptr<IInputEventConsumer>> consumerInfos;
    GetConsumerInfos(pointerEvent, consumerInfos);
    for (const auto &iter : consumerInfos) {
        auto tempEvent = std::make_shared<PointerEvent>(*pointerEvent);
        tempEvent->SetProcessedCallback(monitorCallback_);
        CHKPV(iter.second);
        auto consumer = iter.second;
        consumer->OnInputEvent(tempEvent);
        MMI_HILOGD("Pointer event id:%{public}d pointerId:%{public}d", iter.first, pointerEvent->GetPointerId());
    }
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

int32_t InputMonitor::RemoveLocal(int32_t handlerId)
{
    auto iter = inputHandlers_.find(handlerId);
    if (iter == inputHandlers_.end()) {
        MMI_HILOGE("No handler with specified");
        return RET_ERR;
    }
    inputHandlers_.erase(iter);
    return RET_OK;
}

void InputMonitor::RemoveFromServer(HandleEventType eventType)
{
    int32_t ret = MultimodalInputConnMgr->RemoveMonitorHandler(eventType);
    if (ret != 0) {
        MMI_HILOGE("Remove monitor failed, ret:%{public}d", ret);
    }
}

int32_t InputMonitor::GetNextId()
{
    if (nextId_ == std::numeric_limits<int32_t>::max()) {
        MMI_HILOGE("Exceeded limit of 32-bit maximum number of integers");
        return INVALID_HANDLER_ID;
    }
    return nextId_++;
}

void InputMonitor::OnConnected()
{
    CALL_DEBUG_ENTER;
    HandleEventType eventType = GetEventType();
    if (eventType != HANDLE_EVENT_TYPE_NONE) {
        AddToServer(eventType);
    }
}

bool InputMonitor::HasHandler(int32_t handlerId)
{
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    auto iter = inputHandlers_.find(handlerId);
    return (iter != inputHandlers_.end());
}

HandleEventType InputMonitor::GetEventType() const
{
    if (inputHandlers_.empty()) {
        MMI_HILOGD("MonitorHandlers is empty");
        return HANDLE_EVENT_TYPE_NONE;
    }
    HandleEventType eventType { HANDLE_EVENT_TYPE_NONE };
    for (const auto &inputHandler : inputHandlers_) {
        eventType |= inputHandler.second.eventType_;
    }
    return eventType;
}

void InputMonitor::OnDispatchEventProcessed(int32_t eventId, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtxHandlers_);
    if (mouseEventIds_.find(eventId) != mouseEventIds_.end()) {
        mouseEventIds_.erase(eventId);
        return;
    }
    auto iter = processedEvents_.find(eventId);
    if (iter == processedEvents_.end()) {
        MMI_HILOGE("EventId not in processedEvents_");
        return;
    }
    int32_t count = iter->second;
    processedEvents_.erase(iter);
    count--;
    if (count > 0) {
        processedEvents_.emplace(eventId, count);
        return;
    }
    InputMgrImpl.GetAnrCollecter().SetLastProcessedEventId(ANR_MONITOR, eventId, actionTime);
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputMonitor::ReportMonitorKey(NetPacket& pkt)
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
int32_t InputMonitor::ReportMonitorPointer(NetPacket& pkt)
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
