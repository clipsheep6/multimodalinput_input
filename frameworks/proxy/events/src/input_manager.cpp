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

#include "input_manager.h"

#include "error_multimodal.h"
#include "input_manager_impl.h"
#include "define_multimodal.h"
#include "multimodal_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManager" };
} // namespace

InputManager *InputManager::instance_ = new (std::nothrow) InputManager();
InputManager *InputManager::GetInstance()
{
    return instance_;
}

int32_t InputManager::GetDisplayBindInfo(DisplayBindInfos &infos)
{
    return InputMgrImpl.GetDisplayBindInfo(infos);
}

int32_t InputManager::SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg)
{
    return InputMgrImpl.SetDisplayBind(deviceId, displayId, msg);
}

int32_t InputManager::GetWindowPid(int32_t windowId)
{
    return InputMgrImpl.GetWindowPid(windowId);
}

void InputManager::UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
{
    InputMgrImpl.UpdateDisplayInfo(displayGroupInfo);
}

int32_t InputManager::AddInputEventFilter(std::shared_ptr<IInputEventFilter> filter, int32_t priority,
    uint32_t deviceTags)
{
    return InputMgrImpl.AddInputEventFilter(filter, priority, deviceTags);
}

int32_t InputManager::RemoveInputEventFilter(int32_t filterId)
{
    return InputMgrImpl.RemoveInputEventFilter(filterId);
}

void InputManager::SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer)
{
    InputMgrImpl.SetWindowInputEventConsumer(inputEventConsumer, nullptr);
}

void InputManager::SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CHKPV(eventHandler);
    InputMgrImpl.SetWindowInputEventConsumer(inputEventConsumer, eventHandler);
}

int32_t InputManager::SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
{
    return InputMgrImpl.SubscribeKeyEvent(keyOption, callback);
}

void InputManager::UnsubscribeKeyEvent(int32_t subscriberId)
{
    InputMgrImpl.UnsubscribeKeyEvent(subscriberId);
}

int32_t InputManager::SubscribeSwitchEvent(std::function<void(std::shared_ptr<SwitchEvent>)> callback)
{
    return InputMgrImpl.SubscribeSwitchEvent(callback);
}

void InputManager::UnsubscribeSwitchEvent(int32_t subscriberId)
{
    InputMgrImpl.UnsubscribeSwitchEvent(subscriberId);
}

int32_t InputManager::AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor)
{
    return InputMgrImpl.AddMonitor(monitor);
}

int32_t InputManager::AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor)
{
    return InputMgrImpl.AddMonitor(monitor);
}

int32_t InputManager::AddMonitor(std::shared_ptr<IInputEventConsumer> monitor)
{
    return InputMgrImpl.AddMonitor(monitor);
}

void InputManager::RemoveMonitor(int32_t monitorId)
{
    InputMgrImpl.RemoveMonitor(monitorId);
}

void InputManager::MarkConsumed(int32_t monitorId, int32_t eventId)
{
    InputMgrImpl.MarkConsumed(monitorId, eventId);
}

void InputManager::MoveMouse(int32_t offsetX, int32_t offsetY)
{
    InputMgrImpl.MoveMouse(offsetX, offsetY);
}

int32_t InputManager::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor)
{
    return InputMgrImpl.AddInterceptor(interceptor);
}

int32_t InputManager::AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor)
{
    return InputMgrImpl.AddInterceptor(interceptor);
}

int32_t InputManager::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor, int32_t priority,
    uint32_t deviceTags)
{
    return InputMgrImpl.AddInterceptor(interceptor, priority, deviceTags);
}

void InputManager::RemoveInterceptor(int32_t interceptorId)
{
    InputMgrImpl.RemoveInterceptor(interceptorId);
}

void InputManager::SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    InputMgrImpl.SimulateInputEvent(keyEvent);
}

void InputManager::SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    InputMgrImpl.SimulateInputEvent(pointerEvent);
}

int32_t InputManager::RegisterDevListener(std::string type, std::shared_ptr<IInputDeviceListener> listener)
{
    return InputMgrImpl.RegisterDevListener(type, listener);
}

int32_t InputManager::UnregisterDevListener(std::string type, std::shared_ptr<IInputDeviceListener> listener)
{
    return InputMgrImpl.UnregisterDevListener(type, listener);
}

int32_t InputManager::GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback)
{
    return InputMgrImpl.GetDeviceIds(callback);
}

int32_t InputManager::GetDevice(int32_t deviceId,
    std::function<void(std::shared_ptr<InputDevice>)> callback)
{
    return InputMgrImpl.GetDevice(deviceId, callback);
}

int32_t InputManager::SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes,
    std::function<void(std::vector<bool>&)> callback)
{
    return InputMgrImpl.SupportKeys(deviceId, keyCodes, callback);
}

int32_t InputManager::SetPointerVisible(bool visible)
{
    return InputMgrImpl.SetPointerVisible(visible);
}

bool InputManager::IsPointerVisible()
{
    return InputMgrImpl.IsPointerVisible();
}

int32_t InputManager::SetPointerSpeed(int32_t speed)
{
    return InputMgrImpl.SetPointerSpeed(speed);
}

int32_t InputManager::GetPointerSpeed(int32_t &speed)
{
    return InputMgrImpl.GetPointerSpeed(speed);
}

int32_t InputManager::GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback)
{
    return InputMgrImpl.GetKeyboardType(deviceId, callback);
}

void InputManager::SetAnrObserver(std::shared_ptr<IAnrObserver> observer)
{
    InputMgrImpl.SetAnrObserver(observer);
}

int32_t InputManager::SetPointerStyle(int32_t windowId, PointerStyle pointerStyle)
{
    return InputMgrImpl.SetPointerStyle(windowId, pointerStyle);
}

int32_t InputManager::GetPointerStyle(int32_t windowId, PointerStyle &pointerStyle)
{
    return InputMgrImpl.GetPointerStyle(windowId, pointerStyle);
}

int32_t InputManager::SetInputDevice(const std::string& dhid, const std::string& screenId)
{
    return InputMgrImpl.SetInputDevice(dhid, screenId);
}

bool InputManager::GetFunctionKeyState(int32_t funcKey)
{
    return InputMgrImpl.GetFunctionKeyState(funcKey);
}

int32_t InputManager::SetFunctionKeyState(int32_t funcKey, bool enable)
{
    return InputMgrImpl.SetFunctionKeyState(funcKey, enable);
}

void InputManager::SetPointerLocation(int32_t x, int32_t y)
{
    InputMgrImpl.SetPointerLocation(x, y);
}

int32_t InputManager::EnterCaptureMode(int32_t windowId)
{
    return InputMgrImpl.EnterCaptureMode(windowId);
}

int32_t InputManager::LeaveCaptureMode(int32_t windowId)
{
    return InputMgrImpl.LeaveCaptureMode(windowId);
}

void InputManager::AppendExtraData(const ExtraData& extraData)
{
    InputMgrImpl.AppendExtraData(extraData);
}
} // namespace MMI
} // namespace OHOS