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
#include "input_monitor_manager.h"
#include "i_input_event_consumer.h"
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
    
    void UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
        const std::vector<LogicalDisplayInfo> &logicalDisplays);
    int32_t AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter);

    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler);

    void OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent);
    void OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent);
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

    void SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes,
        std::function<void(std::vector<bool>&)> callback);

    int32_t SetPointerVisible(bool visible);
    bool IsPointerVisible();
#ifdef OHOS_BUILD_KEY_MOUSE
    int32_t SetPointerLocation(int32_t x, int32_t y);
#endif
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    int32_t GetRemoteInputAbility(std::string deviceId, std::function<void(std::set<int32_t>)> remoteTypes);
    int32_t PrepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);
    int32_t UnprepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);
    int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);
    int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
private:
    int32_t PackPhysicalDisplay(NetPacket &pkt);
    int32_t PackLogicalDisplay(NetPacket &pkt);
    void PrintDisplayInfo();
    void SendDisplayInfo();

    void OnKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<KeyEvent> keyEvent);
    void OnPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<PointerEvent> pointerEvent);
    void OnThread();

private:
    sptr<EventFilterService> eventFilterService_ {nullptr};
    std::shared_ptr<IInputEventConsumer> consumer_ = nullptr;

    std::vector<PhysicalDisplayInfo> physicalDisplays_;
    std::vector<LogicalDisplayInfo> logicalDisplays_;
    InputMonitorManager monitorManager_;

    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread ehThread_;
    EventHandlerPtr eventHandler_  = nullptr;
    MMIEventHandlerPtr mmiEventHandler_ = nullptr;
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    sptr<CallDinputService> callDinputService_ {nullptr};
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
};
} // namespace MMI
} // namespace OHOS
#define InputMgrImpl OHOS::MMI::InputManagerImpl::GetInstance()
#endif // INPUT_MANAGER_IMPL_H