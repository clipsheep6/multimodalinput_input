/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
#include "dinput_manager.h"
#include "display_info.h"
#include "i_input_event_consumer.h"
#include "pointer_event.h"
#include "if_mmi_client.h"
#include "net_packet.h"
#include "if_client_msg_handler.h"
#include "event_filter_service.h"
#include "input_device_impl.h"
#include "input_monitor_manager.h"
#include "input_interceptor_manager.h"

namespace OHOS {
namespace MMI {
class InputManagerImpl : public DelayedSingleton<InputManagerImpl> {
public:
    virtual ~InputManagerImpl() = default;
    InputManagerImpl() = default;

    void UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
        const std::vector<LogicalDisplayInfo> &logicalDisplays);                         // 建议本地调用，可IPC
    int32_t AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter);

    void SetWindowInputEventConsumer(std::shared_ptr<OHOS::MMI::IInputEventConsumer> inputEventConsumer);

    void OnKeyEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent);
    void OnPointerEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent);
    int32_t PackDisplayData(NetPacket &pkt);

    int32_t AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor);
    int32_t AddMontior(std::function<void(std::shared_ptr<PointerEvent>)> monitor);
    int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> consumer);
    void RemoveMonitor(int32_t monitorId);
    void MarkConsumed(int32_t monitorId, int32_t eventId);

    int32_t AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor);
    int32_t AddInterceptor(int32_t sourceType, std::function<void(std::shared_ptr<PointerEvent>)> interceptor);
    int32_t AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor);
    void RemoveInterceptor(int32_t interceptorId);

    void SimulateInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent);
    void SimulateInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent);
    void OnConnected();

    void GetVirtualDeviceIdListAsync(std::function<void(std::vector<int32_t>)> callback);
    void OnVirtualDeviceIds(int32_t taskId, std::vector<int32_t> ids);
    void GetVirtualDeviceAsync(int32_t deviceId, std::function<void(std::shared_ptr<InputDeviceImpl::InputDeviceInfo>)> callback);
    void OnVirtualDevice(int32_t taskId, int32_t id, std::string name, int32_t deviceType);
    void GetAllNodeDeviceInfo(std::function<void(std::vector<std::string>)> callback);
    void OnGetAllNodeDeviceInfo(int32_t taskId, std::vector<std::string> ids);
    void HideMouse(std::function<void(bool)> callback);
    void ShowMouse(std::function<void(bool)> callback);
    void GetMouseLocation(std::function<void(std::shared_ptr<DMouseLocation>)> callback );
    void OnMouseLocation(int32_t taskId, std::shared_ptr<DMouseLocation> mouseLocation);
    void PrepareRemoteInput(const std::string& deviceId ,std::function<void(int32_t)> callback);
    void OnPrepareRemoteInput(int32_t taskId, int32_t status);
    void UnprepareRemoteInput(const std::string& deviceId ,std::function<void(int32_t)> callback);
    void OnUnprepareRemoteInput(int32_t taskId, int32_t status);
    void StartRemoteInput(const std::string& deviceId ,std::function<void(int32_t)> callback);
    void OnStartRemoteInput(int32_t taskId, int32_t status);
    void StopRemoteInput(const std::string& deviceId ,std::function<void(int32_t)> callback);
    void OnStopRemoteInput(int32_t taskId, int32_t status);
    void SimulateCrossLocation(int32_t x,int32_t y,std::function<void(int32_t)> callback);
    void OnCrossLocation(int32_t taskId, int32_t status);
    

private:
    int32_t PackPhysicalDisplay(NetPacket &pkt);
    int32_t PackLogicalDisplay(NetPacket &pkt);
    void PrintDisplayDebugInfo();
    void SendDisplayInfo();

private:
    sptr<EventFilterService> eventFilterService_ {nullptr};
    std::shared_ptr<OHOS::MMI::IInputEventConsumer> consumer_ = nullptr;
    std::vector<PhysicalDisplayInfo> physicalDisplays_;
    std::vector<LogicalDisplayInfo> logicalDisplays_;
    InputMonitorManager monitorManager_;
    InputInterceptorManager interceptorManager_;

    std::mutex lk_;
    std::map<int32_t, std::function<void(std::shared_ptr<DMouseLocation>)>> mouseLocationRequests_;
    int32_t mouseLocationTaskId_ {1};
    std::map<int32_t, std::function<void(int32_t)>> remoteInputStateRequests_;
    int32_t remoteInputStateTaskId_ {1};
    std::map<int32_t, std::function<void(int32_t)>> simulateCrossLocationRequests_;
    int32_t simulateCrossLocationTaskId_ {1};
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_MANAGER_IMPL_H
