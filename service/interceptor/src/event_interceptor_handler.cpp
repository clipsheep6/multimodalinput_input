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

#include "event_interceptor_handler.h"

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "event_dispatch_handler.h"
#include "input_device_manager.h"
#include "input_event_data_transformation.h"
#include "input_event_handler.h"
#include "mmi_log.h"
#include "net_packet.h"
#include "proto.h"
#include "util_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventInterceptorHandler" };
} // namespace

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void EventInterceptorHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPV(keyEvent);
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InputHandler->GetJumpInterceptState() && OnHandleEvent(keyEvent)) {
        MMI_HILOGD("KeyEvent filter find a keyEvent from Original event keyCode:%{public}d",
            keyEvent->GetKeyCode());
        BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_INTERCEPT_EVENT);
        return;
    }
#else
    if (OnHandleEvent(keyEvent)) {
        MMI_HILOGD("KeyEvent filter find a keyEvent from Original event keyCode:%{public}d",
            keyEvent->GetKeyCode());
        BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_INTERCEPT_EVENT);
        return;
    }
#endif // OHOS_BUILD_ENABLE_COOPERATE
    CHKPV(nextHandler_);
    nextHandler_->HandleKeyEvent(keyEvent);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#ifdef OHOS_BUILD_ENABLE_POINTER
void EventInterceptorHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    if (OnHandleEvent(pointerEvent)) {
        BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP);
        MMI_HILOGD("Interception is succeeded");
        return;
    }
    CHKPV(nextHandler_);
    nextHandler_->HandlePointerEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_POINTER

#ifdef OHOS_BUILD_ENABLE_TOUCH
void EventInterceptorHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    if (OnHandleEvent(pointerEvent)) {
        BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP);
        MMI_HILOGD("Interception is succeeded");
        return;
    }
    CHKPV(nextHandler_);
    nextHandler_->HandleTouchEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

int32_t EventInterceptorHandler::AddInputHandler(InputHandlerType handlerType,
    HandleEventType eventType, int32_t priority, uint32_t deviceTags, SessionPtr session)
{
    CALL_INFO_TRACE;
    CHKPR(session, RET_ERR);
    if ((eventType & HANDLE_EVENT_TYPE_ALL) == HANDLE_EVENT_TYPE_NONE) {
        MMI_HILOGE("Invalid event type");
        return RET_ERR;
    }
    InitSessionLostCallback();
    SessionHandler interceptor { handlerType, eventType, priority, deviceTags, session };
    return interceptors_.AddInterceptor(interceptor);
}

void EventInterceptorHandler::RemoveInputHandler(InputHandlerType handlerType,
    HandleEventType eventType, int32_t priority, uint32_t deviceTags, SessionPtr session)
{
    CALL_INFO_TRACE;
    CHKPV(session);
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        SessionHandler interceptor { handlerType, eventType, priority, deviceTags, session };
        interceptors_.RemoveInterceptor(interceptor);
    }
}
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
bool EventInterceptorHandler::OnHandleEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    MMI_HILOGD("Handle KeyEvent");
    CHKPF(keyEvent);
    if (keyEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT)) {
        MMI_HILOGW("This event has been tagged as not to be intercepted");
        return false;
    }
    return interceptors_.HandleEvent(keyEvent);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
bool EventInterceptorHandler::OnHandleEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPF(pointerEvent);
    if (pointerEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT)) {
        MMI_HILOGW("This event has been tagged as not to be intercepted");
        return false;
    }
    return interceptors_.HandleEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

void EventInterceptorHandler::InitSessionLostCallback()
{
    if (sessionLostCallbackInitialized_)  {
        MMI_HILOGE("Init session is failed");
        return;
    }
    auto udsServerPtr = InputHandler->GetUDSServer();
    CHKPV(udsServerPtr);
    udsServerPtr->AddSessionDeletedCallback(std::bind(
        &EventInterceptorHandler::OnSessionLost, this, std::placeholders::_1));
    sessionLostCallbackInitialized_ = true;
    MMI_HILOGD("The callback on session deleted is registered successfully");
}

void EventInterceptorHandler::OnSessionLost(SessionPtr session)
{
    interceptors_.OnSessionLost(session);
}

void EventInterceptorHandler::SessionHandler::SendToClient(std::shared_ptr<KeyEvent> keyEvent) const
{
    CHKPV(keyEvent);
    NetPacket pkt(MmiMessageId::REPORT_KEY_EVENT);
    pkt << handlerType_ << deviceTags_;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write key event failed");
        return;
    }
    if (InputEventDataTransformation::KeyEventToNetPacket(keyEvent, pkt) != RET_OK) {
        MMI_HILOGE("Packet key event failed");
        return;
    }
    if (!session_->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", INPUT_MSG_SEND_FAIL);
        return;
    }
}

void EventInterceptorHandler::SessionHandler::SendToClient(std::shared_ptr<PointerEvent> pointerEvent) const
{
    CHKPV(pointerEvent);
    NetPacket pkt(MmiMessageId::REPORT_POINTER_EVENT);
    MMI_HILOGD("Service send to client InputHandlerType:%{public}d", handlerType_);
    pkt << handlerType_ << deviceTags_;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write pointer event failed");
        return;
    }
    if (InputEventDataTransformation::Marshalling(pointerEvent, pkt) != RET_OK) {
        MMI_HILOGE("Marshalling pointer event failed");
        return;
    }
    if (!session_->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed");
        return;
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
bool EventInterceptorHandler::InterceptorCollection::HandleEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPF(keyEvent);
    if (interceptors_.empty()) {
        MMI_HILOGW("Key interceptors is empty");
        return false;
    }
    MMI_HILOGD("There are currently:%{public}zu interceptors", interceptors_.size());
    bool isInterceptor = false;
    std::vector<KeyEvent::KeyItem> keyItems = keyEvent->GetKeyItems();
    if (keyItems.empty()) {
        MMI_HILOGE("keyItems is empty");
        return false;
    }
    std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetInputDevice(keyItems.front().GetDeviceId());
    CHKPF(inputDevice);
    for (const auto &interceptor : interceptors_) {
        if (!inputDevice->HasCapability(interceptor.deviceTags_)) {
            continue;
        }
        if ((interceptor.eventType_ & HANDLE_EVENT_TYPE_KEY) == HANDLE_EVENT_TYPE_KEY) {
            interceptor.SendToClient(keyEvent);
            MMI_HILOGD("Key event was intercepted");
            isInterceptor = true;
            break;
        }
    }
    return isInterceptor;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
bool EventInterceptorHandler::InterceptorCollection::HandleEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPF(pointerEvent);
    if (interceptors_.empty()) {
        MMI_HILOGI("Interceptors are empty");
        return false;
    }
    MMI_HILOGD("There are currently:%{public}zu interceptors", interceptors_.size());
    bool isInterceptor = false;
    PointerEvent::PointerItem pointerItem;
    int32_t pointerId = pointerEvent->GetPointerId();
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("GetPointerItem:%{public}d fail", pointerId);
        return false;
    }
    std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetInputDevice(pointerItem.GetDeviceId());
    CHKPF(inputDevice);
    for (const auto &interceptor : interceptors_) {
        if (!inputDevice->HasCapability(interceptor.deviceTags_)) {
            continue;
        }
        if ((interceptor.eventType_ & HANDLE_EVENT_TYPE_POINTER) == HANDLE_EVENT_TYPE_POINTER) {
            interceptor.SendToClient(pointerEvent);
            MMI_HILOGD("Pointer event was intercepted");
            isInterceptor = true;
            break;
        }
    }
    return isInterceptor;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

int32_t EventInterceptorHandler::InterceptorCollection::AddInterceptor(const SessionHandler& interceptor)
{
    for (auto iter = interceptors_.begin(); iter != interceptors_.end(); ++iter) {
        if (iter->session_ == interceptor.session_) {
            interceptors_.erase(iter);
            break;
        }
    }

    if (interceptors_.size() >= MAX_N_INPUT_INTERCEPTORS) {
        MMI_HILOGE("The number of interceptors exceeds limit");
        return RET_ERR;
    }

    auto iterIndex = interceptors_.cbegin();
    for (; iterIndex != interceptors_.cend(); ++iterIndex) {
        if (interceptor.priority_ < iterIndex->priority_) {
            break;
        }
    }
    auto sIter = interceptors_.emplace(iterIndex, interceptor);
    if (sIter == interceptors_.end()) {
        MMI_HILOGE("Failed to add interceptor");
        RET_ERR;
    }
    return RET_OK;
}

void EventInterceptorHandler::InterceptorCollection::RemoveInterceptor(const SessionHandler& interceptor)
{
    for (auto iter = interceptors_.begin(); iter != interceptors_.end(); ++iter) {
        if (iter->session_ == interceptor.session_) {
            interceptors_.erase(iter);
            break;
        }
    }
    if (interceptor.eventType_ == HANDLE_EVENT_TYPE_NONE) {
        MMI_HILOGD("Unregister interceptor successfully");
        return;
    }

    auto iterIndex = interceptors_.cbegin();
    for (; iterIndex != interceptors_.cend(); ++iterIndex) {
        if (interceptor.priority_ < iterIndex->priority_) {
            break;
        }
    }
    auto sIter = interceptors_.emplace(iterIndex, interceptor);
    if (sIter == interceptors_.end()) {
        MMI_HILOGE("Internal error, interceptor has been removed");
        RET_ERR;
    }
    MMI_HILOGD("Event type is updated:%{public}u", interceptor.eventType_);
}

void EventInterceptorHandler::InterceptorCollection::OnSessionLost(SessionPtr session)
{
    CALL_INFO_TRACE;
    for (auto iter = interceptors_.begin(); iter != interceptors_.end(); ++iter) {
        if (iter->session_ == session) {
            interceptors_.erase(iter);
        }
    }
}
void EventInterceptorHandler::Dump(int32_t fd, const std::vector<std::string> &args)
{
    return interceptors_.Dump(fd, args);
}

void EventInterceptorHandler::InterceptorCollection::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    mprintf(fd, "Interceptor information:\t");
    mprintf(fd, "interceptors: count=%d", interceptors_.size());
    for (const auto &item : interceptors_) {
        SessionPtr session = item.session_;
        CHKPV(session);
        mprintf(fd,
                "handlerType:%d | eventType:%d | Pid:%d | Uid:%d | Fd:%d "
                "| EarliestEventTime:%" PRId64 " | Descript:%s \t",
                item.handlerType_, item.eventType_,
                session->GetPid(), session->GetUid(),
                session->GetFd(),
                session->GetEarliestEventTime(), session->GetDescript().c_str());
    }
}
} // namespace MMI
} // namespace OHOS
