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

#ifndef INPUT_DEVICE_COOPERATE_SM_H
#define INPUT_DEVICE_COOPERATE_SM_H

#include "bytrace_adapter.h"
#include "singleton.h"

#include "device_manager_callback.h"
#include "distributed_input_adapter.h"
#include "dm_device_info.h"
#include "event_package.h"
#include "i_input_device_cooperate_state.h"
#include "i_input_event_handler.h"

namespace OHOS {
namespace MMI {
enum class CooperateState {
    STATE_FREE = 0,
    STATE_IN = 1,
    STATE_OUT = 2,
};

enum class CooperateMsg {
    COOPERATE_ON_SUCESS = 0,
    COOPERATE_ON_FAIL = 1,
    COOPERATE_OFF_SUCESS = 2,
    COOPERATE_OFF_FAIL = 3,
    COOPERATE_START = 4,
    COOPERATE_START_SUCESS = 5,
    COOPERATE_START_FAIL = 6,
    COOPERATE_STOP = 7,
    COOPERATE_STOP_SUCESS = 8,
    COOPERATE_STOP_FIAL = 9,
    COOPERATE_NULL = 10,
};

class InputDeviceCooperateSM : public DelayedSingleton<InputDeviceCooperateSM>, public IInputEventHandler {
    class DeviceInitCallBack : public DistributedHardware::DmInitCallback {
        void OnRemoteDied() override;
    };

    class MmiDeviceStateCallback : public DistributedHardware::DeviceStateCallback {
        void OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
        void OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
        void OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
        void OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    };
public:
    InputDeviceCooperateSM() = default;
    DISALLOW_COPY_AND_MOVE(InputDeviceCooperateSM);
    void Init();
    void EnableInputDeviceCooperate(bool enabled);
    int32_t StartInputDeviceCooperate(const std::string &remoteNetworkId, int32_t startInputDeviceId);
    int32_t StopInputDeviceCooperate();
    void GetCooperateState(const std::string &deviceId);
    void StartRemoteCooperate(const std::string &remoteNetworkId);
    void StartRemoteCooperateResult(bool isSuccess, const std::string &startDhid, int32_t xPercent, int32_t yPercent);
    void StopRemoteCooperate();
    void StopRemoteCooperateResult(bool isSuccess);
    void StartCooperateOtherResult(const std::string &srcNetworkId);
    void HandleLibinputEvent(struct libinput_event *event) override;
    void UpdateState(CooperateState state);
    void UpdatePreparedDevices(const std::string &srcNetworkId, const std::string &sinkNetworkId);
    std::pair<std::string, std::string> GetPreparedDevices() const;
    std::shared_ptr<IInputDeviceCooperateState> GetCurrentState() const;
    CooperateState GetCurrentCooperateState() const;
    const std::string &GetSrcNetworkId() const;
    void OnCooperateChanged(const std::string &networkId, bool isOpen);
    void OnKeyboardOnline(const std::string &dhid);
    void OnPointerOffline(const std::string &dhid, const std::string &sinkNetworkId,
        const std::vector<std::string> &keyboards);
    bool InitDeviceManager();
    void OnDeviceOnline(const std::string &networkId);
    void OnDeviceOffline(const std::string &networkId);
    void OnStartFinish(bool isSuccess, const std::string &remoteNetworkId, int32_t startInputDeviceId);
    void OnStopFinish(bool isSuccess, const std::string &remoteNetworkId);
    void Dump(int32_t fd, const std::vector<std::string> &args);

private:
    void Reset();
    void CheckPointerEvent(struct libinput_event *event);
    void OnCloseCooperation(const std::string &networkId, bool isLocal);
    void NotifyRemoteStartFail(const std::string &remoteNetworkId);
    void NotifyRemoteStartSucess(const std::string &remoteNetworkId, const std::string &startDhid);
    void NotifyRemoteStopFinish(bool isSuccess, const std::string &remoteNetworkId);
    bool UpdateMouseLocation();

    std::shared_ptr<IInputDeviceCooperateState> currentStateSM_ { nullptr };
    std::pair<std::string, std::string> preparedNetworkId_;
    std::string startDhid_ ;
    std::string srcNetworkId_;
    CooperateState cooperateState_ { CooperateState::STATE_FREE };
    std::shared_ptr<DistributedHardware::DmInitCallback> initCallback_ { nullptr };
    std::shared_ptr<DistributedHardware::DeviceStateCallback> stateCallback_ { nullptr };
    std::vector<std::string> onlineDevice_;
    mutable std::mutex mutex_;
    std::atomic<bool> isStarting_ { false };
    std::atomic<bool> isStopping_ { false };
    std::pair<int32_t, int32_t> mouseLocation_ { std::make_pair(0, 0) };
};
#define InputDevCooSM InputDeviceCooperateSM::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_COOPERATE_SM_H