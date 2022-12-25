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

#ifndef INPUT_MANAGER_IMPL_H
#define INPUT_MANAGER_IMPL_H

#include "singleton.h"

#include "anr_collecter.h"
#include "event_handler.h"
#include "input_device_impl.h"
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include "input_device_cooperate_impl.h"
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include "input_filter.h"
#include "input_injector.h"
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
#include "input_interceptor.h"
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
#include "input_key_subscriber.h"
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_MONITOR
#include "input_monitor.h"
#endif // OHOS_BUILD_ENABLE_MONITOR
#include "input_mouse_helper.h"
#include "input_window_transfer.h"
#include "i_input_device_cooperate_listener.h"
#include "key_option.h"

namespace OHOS {
namespace MMI {
class InputManagerImpl final {
    DECLARE_SINGLETON(InputManagerImpl);

public:
    DISALLOW_MOVE(InputManagerImpl);

    bool InitClient(EventHandlerPtr eventHandler = nullptr);
    int32_t GetDisplayBindInfo(DisplayBindInfos &infos);
    int32_t SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg);
    void UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo);
    int32_t SubscribeKeyEvent(
        std::shared_ptr<KeyOption> keyOption,
        std::function<void(std::shared_ptr<KeyEvent>)> callback
    );
    void UnsubscribeKeyEvent(int32_t subscriberId);
    int32_t AddInputEventFilter(std::shared_ptr<IInputEventFilter> filter, int32_t priority);
    int32_t RemoveInputEventFilter(int32_t filterId);

    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler);

    int32_t AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor);
    int32_t AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor);
    int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> consumer);

    void RemoveMonitor(int32_t monitorId);
    void MarkConsumed(int32_t monitorId, int32_t eventId);
    void MoveMouse(int32_t offsetX, int32_t offsetY);

    int32_t AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor,
        int32_t priority = DEFUALT_INTERCEPTOR_PRIORITY,
        uint32_t deviceTags = CapabilityToTags(InputDeviceCapability::INPUT_DEV_CAP_MAX));
    int32_t AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor,
        int32_t priority = DEFUALT_INTERCEPTOR_PRIORITY,
        uint32_t deviceTags = CapabilityToTags(InputDeviceCapability::INPUT_DEV_CAP_MAX));
    void RemoveInterceptor(int32_t interceptorId);

    void SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent);
    void SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent);
    void OnConnected();

    int32_t RegisterDevListener(std::string type, std::shared_ptr<IInputDeviceListener> listener);
    int32_t UnregisterDevListener(std::string type, std::shared_ptr<IInputDeviceListener> listener = nullptr);
    int32_t GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback);
    int32_t GetDevice(int32_t deviceId, std::function<void(std::shared_ptr<InputDevice>)> callback);
    int32_t SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes,
        std::function<void(std::vector<bool>&)> callback);
    int32_t GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback);

    int32_t SetPointerVisible(bool visible);
    bool IsPointerVisible();
    int32_t SetPointerStyle(int32_t windowId, int32_t pointerStyle);
    int32_t GetPointerStyle(int32_t windowId, int32_t &pointerStyle);

    int32_t SetPointerSpeed(int32_t speed);
    int32_t GetPointerSpeed(int32_t &speed);

    void SetAnrObserver(std::shared_ptr<IAnrObserver> observer);

    int32_t RegisterCooperateListener(std::shared_ptr<IInputDeviceCooperateListener> listener);
    int32_t UnregisterCooperateListener(std::shared_ptr<IInputDeviceCooperateListener> listener = nullptr);
    int32_t EnableInputDeviceCooperate(bool enabled, std::function<void(std::string, CooperationMessage)> callback);
    int32_t StartInputDeviceCooperate(const std::string &sinkDeviceId, int32_t srcInputDeviceId,
        std::function<void(std::string, CooperationMessage)> callback);
    int32_t StopDeviceCooperate(std::function<void(std::string, CooperationMessage)> callback);
    int32_t GetInputDeviceCooperateState(const std::string &deviceId, std::function<void(bool)> callback);
    int32_t SetInputDevice(const std::string& dhid, const std::string& screenId);
    bool GetFunctionKeyState(int32_t funcKey);
    int32_t SetFunctionKeyState(int32_t funcKey, bool enable);
    void SetPointerLocation(int32_t x, int32_t y);
    int32_t EnterCaptureMode(int32_t windowId);
    int32_t LeaveCaptureMode(int32_t windowId);
    MMIClientPtr GetMMIClient() const;
    AnrCollecter& GetAnrCollecter();
    EventHandlerPtr GetEventHandler() const;

private:
    void InitMsgCallback();
private:
    MMIClientPtr client_ { nullptr };
    std::mutex mtx_;
    AnrCollecter anrCollecter_;
    InputInjector inputInjector_;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    InputDeviceCooperateImpl inputDeviceCooImpl_;
#endif // OHOS_BUILD_ENABLE_COOPERATE
    InputDeviceImpl inputDeviceImpl_;
    InputFilter inputFilter_;
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    InputInterceptor inputInterceptor_;
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
#ifdef OHOS_BUILD_ENABLE_MONITOR
    InputMonitor inputMonitor_;
#endif // OHOS_BUILD_ENABLE_MONITOR
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    InputKeySubscriber keySubscriber_;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    InputMouseHelper mouseHelper_;
    InputWindowTransfer windowTransfer_;
};

#define InputMgrImpl ::OHOS::Singleton<InputManagerImpl>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_MANAGER_IMPL_H