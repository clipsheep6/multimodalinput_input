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

#include "input_manager_impl.h"

#include <cinttypes>

#include "define_multimodal.h"
#include "error_multimodal.h"

#include "bytrace_adapter.h"
#include "mmi_client.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerImpl" };
} // namespace

struct MonitorEventConsumer : public IInputEventConsumer {
    explicit MonitorEventConsumer(const std::function<void(std::shared_ptr<PointerEvent>)> &monitor)
        : monitor_ (monitor) {}

    explicit MonitorEventConsumer(const std::function<void(std::shared_ptr<KeyEvent>)> &monitor)
        : keyMonitor_ (monitor) {}

    void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const
    {
        CHKPV(keyEvent);
        CHKPV(keyMonitor_);
        keyMonitor_(keyEvent);
    }

    void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const
    {
        CHKPV(pointerEvent);
        CHKPV(monitor_);
        monitor_(pointerEvent);
    }

    void OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const
    {
        CHKPV(axisEvent);
        CHKPV(axisMonitor_);
        axisMonitor_(axisEvent);
    }

private:
    std::function<void(std::shared_ptr<PointerEvent>)> monitor_;
    std::function<void(std::shared_ptr<KeyEvent>)> keyMonitor_;
    std::function<void(std::shared_ptr<AxisEvent>)> axisMonitor_;
};

InputManagerImpl::InputManagerImpl() {}
InputManagerImpl::~InputManagerImpl() {}

void Connected(const IMMIClient& client)
{
    CALL_DEBUG_ENTER;
    InputMgrImpl.OnConnected();
}

bool InputManagerImpl::InitClient(EventHandlerPtr eventHandler)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (client_ != nullptr) {
        if (eventHandler != nullptr) {
            client_->MarkIsEventHandlerChanged(eventHandler);
        }
        return true;
    }
    client_ = std::make_shared<MMIClient>();
    CHKPF(client_);
    client_->SetEventHandler(eventHandler);
    client_->RegisterConnectedFunction(&Connected);
    if (!(client_->Start())) {
        client_.reset();
        client_ = nullptr;
        MMI_HILOGE("The client fails to start");
        return false;
    }
    return true;
}

void InputManagerImpl::UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    windowTransfer_.UpdateDisplayInfo(displayGroupInfo);
}

int32_t InputManagerImpl::AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter)
{
    CALL_INFO_TRACE;
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    return inputFilter_.AddInputEventFilter(filter);
#else
    MMI_HILOGW("Pointer and touchscreen device does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
}

void InputManagerImpl::SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CALL_INFO_TRACE;
    CHK_PID_AND_TID();
    CHKPV(inputEventConsumer);
    CHKPV(eventHandler);
    if (!InitClient(eventHandler)) {
        MMI_HILOGE("Client init failed");
        return;
    }
    consumer_ = inputEventConsumer;
    eventHandler_ = eventHandler;
}

int32_t InputManagerImpl::SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
{
    CALL_INFO_TRACE;
    CHK_PID_AND_TID();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CHKPR(keyOption, RET_ERR);
    CHKPR(callback, RET_ERR);
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return keySubscriber_.SubscribeKeyEvent(keyOption, callback);
#else
    MMI_HILOGW("Keyboard device does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

void InputManagerImpl::UnsubscribeKeyEvent(int32_t subscriberId)
{
    CALL_INFO_TRACE;
    CHK_PID_AND_TID();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    keySubscriber_.UnsubscribeKeyEvent(subscriberId);
#else
    MMI_HILOGW("Keyboard device does not support");
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputManagerImpl::OnKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(consumer);
    consumer->OnInputEvent(keyEvent);
    MMI_HILOGD("Key event callback keyCode:%{public}d", keyEvent->GetKeyCode());
}

void InputManagerImpl::OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(keyEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    CHKPV(client_);
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler = nullptr;
    std::shared_ptr<IInputEventConsumer> inputConsumer = nullptr;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        eventHandler = eventHandler_;
        inputConsumer = consumer_;
    }
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_DISPATCH_EVENT);
    if (client_->IsEventHandlerChanged()) {
        if (!eventHandler->PostHighPriorityTask(std::bind(&InputManagerImpl::OnKeyEventTask,
            this, inputConsumer, keyEvent))) {
            MMI_HILOGE("Post task failed");
            return;
        }
    } else {
        inputConsumer->OnInputEvent(keyEvent);
        MMI_HILOGD("Key event report keyCode:%{public}d", keyEvent->GetKeyCode());
    }
    MMI_HILOGD("Key event keyCode:%{public}d", keyEvent->GetKeyCode());
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputManagerImpl::OnPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PID_AND_TID();
    CHKPV(consumer);
    CHKPV(pointerEvent);
    consumer->OnInputEvent(pointerEvent);
    MMI_HILOGD("Pointer event callback pointerId:%{public}d", pointerEvent->GetPointerId());
}

void InputManagerImpl::OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    CHKPV(pointerEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    CHKPV(client_);
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler = nullptr;
    std::shared_ptr<IInputEventConsumer> inputConsumer = nullptr;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        eventHandler = eventHandler_;
        inputConsumer = consumer_;
    }
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_DISPATCH_EVENT);
    if (client_->IsEventHandlerChanged()) {
        if (!eventHandler->PostHighPriorityTask(std::bind(&InputManagerImpl::OnPointerEventTask,
            this, inputConsumer, pointerEvent))) {
            MMI_HILOGE("Post task failed");
            return;
        }
    } else {
        inputConsumer->OnInputEvent(pointerEvent);
        MMI_HILOGD("Pointer event report pointerId:%{public}d", pointerEvent->GetPointerId());
    }
    MMI_HILOGD("Pointer event pointerId:%{public}d", pointerEvent->GetPointerId());
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

int32_t InputManagerImpl::AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor)
{
    CALL_INFO_TRACE;
#if defined(OHOS_BUILD_ENABLE_KEYBOARD) && defined(OHOS_BUILD_ENABLE_MONITOR)
    CHKPR(monitor, INVALID_HANDLER_ID);
    auto consumer = std::make_shared<MonitorEventConsumer>(monitor);
    CHKPR(consumer, INVALID_HANDLER_ID);
    return AddMonitor(consumer);
#else
    MMI_HILOGW("Keyboard device or monitor function does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_KEYBOARD || OHOS_BUILD_ENABLE_MONITOR
}

int32_t InputManagerImpl::AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor)
{
    CALL_INFO_TRACE;
#if (defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)) && defined(OHOS_BUILD_ENABLE_MONITOR)
    CHKPR(monitor, INVALID_HANDLER_ID);
    auto consumer = std::make_shared<MonitorEventConsumer>(monitor);
    CHKPR(consumer, INVALID_HANDLER_ID);
    return AddMonitor(consumer);
#else
    MMI_HILOGW("Pointer/touchscreen device or monitor function does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_MONITOR ||  OHOS_BUILD_ENABLE_TOUCH && OHOS_BUILD_ENABLE_MONITOR
}

int32_t InputManagerImpl::AddMonitor(std::shared_ptr<IInputEventConsumer> consumer)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_MONITOR
    CHKPR(consumer, INVALID_HANDLER_ID);
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return INVALID_HANDLER_ID;
    }
    return inputMonitor_.AddMonitor(consumer);
#else
    MMI_HILOGI("Monitor function does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_MONITOR
}

void InputManagerImpl::RemoveMonitor(int32_t monitorId)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_MONITOR
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    inputMonitor_.RemoveMonitor(monitorId);
#else
    MMI_HILOGI("Monitor function does not support");
#endif // OHOS_BUILD_ENABLE_MONITOR
}

void InputManagerImpl::MarkConsumed(int32_t monitorId, int32_t eventId)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_MONITOR
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    inputMonitor_.MarkConsumed(monitorId, eventId);
#else
    MMI_HILOGI("Monitor function does not support");
#endif // OHOS_BUILD_ENABLE_MONITOR
}

void InputManagerImpl::MoveMouse(int32_t offsetX, int32_t offsetY)
{
#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
    mouseHelper_.MoveMouseEvent(offsetX, offsetY);
#else
    MMI_HILOGW("Pointer device or pointer drawing module does not support");
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING
}

int32_t InputManagerImpl::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    CHKPR(interceptor, INVALID_HANDLER_ID);
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return INVALID_HANDLER_ID;
    }
    return inputInterceptor_.AddInterceptor(interceptor, HANDLE_EVENT_TYPE_ALL);
#else
    MMI_HILOGW("Interceptor function does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
}

int32_t InputManagerImpl::AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor)
{
    CALL_INFO_TRACE;
#if defined(OHOS_BUILD_ENABLE_KEYBOARD) && defined(OHOS_BUILD_ENABLE_INTERCEPTOR)
    CHKPR(interceptor, INVALID_HANDLER_ID);
    auto consumer = std::make_shared<MonitorEventConsumer>(interceptor);
    CHKPR(consumer, INVALID_HANDLER_ID);
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return INVALID_HANDLER_ID;
    }
    return inputInterceptor_.AddInterceptor(consumer, HANDLE_EVENT_TYPE_KEY);
#else
    MMI_HILOGW("Keyboard device or interceptor function does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_KEYBOARD && OHOS_BUILD_ENABLE_INTERCEPTOR
}

void InputManagerImpl::RemoveInterceptor(int32_t interceptorId)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    inputInterceptor_.RemoveInterceptor(interceptorId);
#else
    MMI_HILOGW("Interceptor function does not support");
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
}

void InputManagerImpl::SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_INFO_TRACE;
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CHKPV(keyEvent);
    inputInjector_.InjectKeyEvent(keyEvent);
#else
    MMI_HILOGW("Keyboard device does not support");
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

void InputManagerImpl::SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_INFO_TRACE;
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    CHKPV(pointerEvent);
    inputInjector_.InjectPointerEvent(pointerEvent);
#else
    MMI_HILOGW("Pointer and touchscreen device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
}

int32_t InputManagerImpl::SetPointerVisible(bool visible)
{
    CALL_DEBUG_ENTER;
#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
    return mouseHelper_.SetPointerVisible(visible);
#else
    MMI_HILOGW("Pointer device or pointer drawing module does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING
}

bool InputManagerImpl::IsPointerVisible()
{
    CALL_DEBUG_ENTER;
#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
    return mouseHelper_.IsPointerVisible();
#else
    MMI_HILOGW("Pointer device or pointer drawing module does not support");
    return false;
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
}

int32_t InputManagerImpl::SetPointerSpeed(int32_t speed)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_POINTER
    return mouseHelper_.SetPointerSpeed(speed);
#else
    MMI_HILOGW("Pointer device does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t InputManagerImpl::GetPointerSpeed(int32_t &speed)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_POINTER
    return mouseHelper_.GetPointerSpeed(speed);
#else
    return ERROR_UNSUPPORT;
    MMI_HILOGW("Pointer device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t InputManagerImpl::SetPointerStyle(int32_t windowId, int32_t pointerStyle)
{
    CALL_DEBUG_ENTER;
    return mouseHelper_.SetPointerStyle(windowId, pointerStyle);
}

int32_t InputManagerImpl::GetPointerStyle(int32_t windowId, int32_t &pointerStyle)
{
    CALL_DEBUG_ENTER;
    return mouseHelper_.GetPointerStyle(windowId, pointerStyle);
}

void InputManagerImpl::OnConnected()
{
    CALL_DEBUG_ENTER;
    windowTransfer_.OnConnected();
    anrCollecter_.OnConnected();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    keySubscriber_.OnConnected();
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_MONITOR
    inputMonitor_.OnConnected();
#endif // OHOS_BUILD_ENABLE_MONITOR
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    inputInterceptor_.OnConnected();
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
}

int32_t InputManagerImpl::RegisterDevListener(std::string type, std::shared_ptr<IInputDeviceListener> listener)
{
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.RegisterDevListener(type, listener);
}

int32_t InputManagerImpl::UnregisterDevListener(std::string type,
    std::shared_ptr<IInputDeviceListener> listener)
{
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.UnregisterDevListener(type, listener);
}

int32_t InputManagerImpl::GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback)
{
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.GetInputDeviceIdsAsync(callback);
}

int32_t InputManagerImpl::GetDevice(int32_t deviceId,
    std::function<void(std::shared_ptr<InputDevice>)> callback)
{
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.GetInputDeviceAsync(deviceId, callback);
}

int32_t InputManagerImpl::SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes,
    std::function<void(std::vector<bool>&)> callback)
{
    CALL_DEBUG_ENTER;
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.SupportKeys(deviceId, keyCodes, callback);
}

int32_t InputManagerImpl::GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback)
{
    CALL_DEBUG_ENTER;
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return RET_ERR;
    }
    return inputDeviceImpl_.GetKeyboardType(deviceId, callback);
}

void InputManagerImpl::SetAnrObserver(std::shared_ptr<IAnrObserver> observer)
{
    CALL_DEBUG_ENTER;
    if (!InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    anrCollecter_.SetAnrObserver(observer);
}

void InputManagerImpl::OnAnr(int32_t pid)
{
    CALL_DEBUG_ENTER;
    anrCollecter_.OnAnr(pid);
}

int32_t InputManagerImpl::SetInputDevice(const std::string &dhid, const std::string &screenId)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    return inputDeviceCooImpl_.SetInputDevice(dhid, screenId);
#else
    (void)(dhid);
    (void)(screenId);
    MMI_HILOGW("Enable input device cooperate does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::RegisterCooperateListener(std::shared_ptr<IInputDeviceCooperateListener> listener)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.RegisterCooperateListener(listener);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(listener);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::UnregisterCooperateListener(std::shared_ptr<IInputDeviceCooperateListener> listener)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.UnregisterCooperateListener(listener);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(listener);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::EnableInputDeviceCooperate(bool enabled,
    std::function<void(std::string, CooperationMessage)> callback)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.EnableInputDeviceCooperate(enabled, callback);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(enabled);
    (void)(callback);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::StartInputDeviceCooperate(const std::string &sinkDeviceId, int32_t srcInputDeviceId,
    std::function<void(std::string, CooperationMessage)> callback)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.StartInputDeviceCooperate(sinkDeviceId, srcInputDeviceId, callback);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(sinkDeviceId);
    (void)(srcInputDeviceId);
    (void)(callback);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::StopDeviceCooperate(std::function<void(std::string, CooperationMessage)> callback)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.StopDeviceCooperate(callback);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(callback);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputManagerImpl::GetInputDeviceCooperateState(const std::string &deviceId, std::function<void(bool)> callback)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (!InitClient()) {
        MMI_HILOGE("client init failed");
        return RET_ERR;
    }
    return inputDeviceCooImpl_.GetInputDeviceCooperateState(deviceId, callback);
#else
    MMI_HILOGW("Cooperate does not support");
    (void)(deviceId);
    (void)(callback);
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

bool InputManagerImpl::GetFunctionKeyState(int32_t funcKey)
{
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CALL_DEBUG_ENTER;
    return keySubscriber_.GetFunctionKeyState(funcKey);
#else
    MMI_HILOGW("Keyboard device does not support");
    return false;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

int32_t InputManagerImpl::SetFunctionKeyState(int32_t funcKey, bool enable)
{
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CALL_DEBUG_ENTER;
    return keySubscriber_.SetFunctionKeyState(funcKey, enable);
#else
    MMI_HILOGW("Keyboard device does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

MMIClientPtr InputManagerImpl::GetMMIClient()
{
    CHKPP(client_);
    return client_->GetSharedPtr();
}

InputDeviceCooperateImpl& InputManagerImpl::GetInputDeviceCooImpl()
{
    return inputDeviceCooImpl_;
}

InputDeviceImpl& InputManagerImpl::GetInputDeviceImpl()
{
    return inputDeviceImpl_;
}

InputMonitor& InputManagerImpl::GetInputMonitor()
{
    return inputMonitor_;
}

InputInterceptor& InputManagerImpl::GetInputInterceptor()
{
    return inputInterceptor_;
}

InputKeySubscriber& InputManagerImpl::GetKeySubscriber()
{
    return keySubscriber_;
}
} // namespace MMI
} // namespace OHOS
