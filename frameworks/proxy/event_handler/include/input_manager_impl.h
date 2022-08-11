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

#include <vector>

#include "nocopyable.h"
#include "singleton.h"

#include "net_packet.h"

#include "display_info.h"
#include "event_filter_service.h"

#include "if_mmi_client.h"
#include "input_device_impl.h"
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
#include "input_interceptor_manager.h"
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
#ifdef OHOS_BUILD_ENABLE_MONITOR
#include "input_monitor_manager.h"
#endif // OHOS_BUILD_ENABLE_MONITOR
#include "i_anr_observer.h"
#include "i_input_event_consumer.h"
#include "key_option.h"
#include "mmi_event_handler.h"
#include "pointer_event.h"
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
#include "call_dinput_service.h"
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

namespace OHOS {
namespace MMI {
class InputManagerImpl : public DelayedSingleton<InputManagerImpl> {
public:
    virtual ~InputManagerImpl() = default;
    DISALLOW_COPY_AND_MOVE(InputManagerImpl);
    InputManagerImpl() = default;

    bool InitEventHandler();
    MMIEventHandlerPtr GetEventHandler() const;
    EventHandlerPtr GetCurrentEventHandler() const;

    void UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo);
    int32_t SubscribeKeyEvent(
        std::shared_ptr<KeyOption> keyOption,
        std::function<void(std::shared_ptr<KeyEvent>)> callback
    );
    void UnsubscribeKeyEvent(int32_t subscriberId);
    int32_t AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter);

    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler);

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
    int32_t PackDisplayData(NetPacket &pkt);

    int32_t AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor);
    int32_t AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor);
    int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> consumer);

    void RemoveMonitor(int32_t monitorId);
    void MarkConsumed(int32_t monitorId, int32_t eventId);
    void MoveMouse(int32_t offsetX, int32_t offsetY);

    int32_t AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor);
    int32_t AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor);
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

    int32_t SetPointerSpeed(int32_t speed);
    int32_t GetPointerSpeed();

    void SetAnrObserver(std::shared_ptr<IAnrObserver> observer);
    void OnAnr(int32_t pid);
    int32_t SetPointerLocation(int32_t x, int32_t y);
    using DeviceUniqId = std::tuple<int32_t, int32_t, int32_t, int32_t, int32_t, std::string>;
    int32_t SetInputDeviceSeatName(const std::string& seatName, DeviceUniqId& deviceUniqId);
    
    int32_t GetRemoteInputAbility(std::string deviceId, std::function<void(std::set<int32_t>)> remoteTypes);
    int32_t PrepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);
    int32_t UnprepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);
    int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);
    int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);

private:
    int32_t PackWindowInfo(NetPacket &pkt);
    int32_t PackDisplayInfo(NetPacket &pkt);
    void PrintDisplayInfo();
    void SendDisplayInfo();

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void OnKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void OnPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
    void OnAnrTask(std::vector<std::shared_ptr<IAnrObserver>> observers, int32_t pid);
    void OnThread();

private:
    sptr<EventFilterService> eventFilterService_ {nullptr};
    std::shared_ptr<IInputEventConsumer> consumer_ = nullptr;
    std::vector<std::shared_ptr<IAnrObserver>> anrObservers_;

    DisplayGroupInfo displayGroupInfo_;
    std::mutex mtx_;
    std::mutex handleMtx_;
    std::condition_variable cv_;
    std::thread ehThread_;
    EventHandlerPtr eventHandler_  = nullptr;
    MMIEventHandlerPtr mmiEventHandler_ = nullptr;
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    sptr<CallDinputService> callDinputService_ = nullptr;
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
};
} // namespace MMI
} // namespace OHOS
#define InputMgrImpl OHOS::MMI::InputManagerImpl::GetInstance()
#endif // INPUT_MANAGER_IMPL_H