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

#include <memory>

#include <gtest/gtest.h>
#include "cooperate_event_manager.h"
#include "multimodal_input_connect_def_parcel.h"
#include "input_device_cooperate_state_out.h"
#include "input_device_cooperate_state_free.h"
#include "input_device_cooperate_state_in.h"
#define private public
#include "input_device_cooperate_sm.h"
#include "device_profile_adapter.h"
#include "define_multimodal.h"
#include "mock_distributed_device_profile_client.h"

namespace OHOS {
namespace MMI {
using namespace testing::ext;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateTest" };
constexpr const int32_t DP_GET_SERVICE_SUCCESS = 98566148;
} // namespace
class IDCooperateTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

/**
 * @tc.number: UpdateCrossingSwitchState001
 * @tc.name: UpdateCrossingSwitchState
 * @tc.desc: Verify that the status of the crossing switch is updated.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, UpdateCrossingSwitchState001, TestSize.Level1)
{
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true);
    MMI_HILOGD("Update crossing switch state :%{public}d", rest);
    EXPECT_EQ(rest, DP_GET_SERVICE_SUCCESS);
}

/**
 * @tc.number: UpdateCrossingSwitchState002
 * @tc.name: UpdateCrossingSwitchState
 * @tc.desc: Verify that the status of the crossing switch is updated.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, UpdateCrossingSwitchState002, TestSize.Level1)
{
    std::vector<std::string> deviceIds = {"123"};
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true, deviceIds);
    MMI_HILOGD("Update crossing switch state :%{public}d", rest);
    EXPECT_EQ(rest, DP_GET_SERVICE_SUCCESS);
}

/**
 * @tc.number: GetCrossingSwitchState001
 * @tc.name: GetCrossingSwitchState
 * @tc.desc: Verify that the cross switch status is obtained.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, GetCrossingSwitchState001, TestSize.Level1)
{
    std::string deviceId = "123";
    DeviceProfile::MockDistributedDeviceProfileClient::SetDPState(true);
    bool state  = DProfileAdapter->GetCrossingSwitchState(deviceId);
    MMI_HILOGD("Get crossing switch state :%{public}d", state);
    EXPECT_EQ(state, true);
}

/**
 * @tc.number: GetCrossingSwitchState002
 * @tc.name: GetCrossingSwitchState
 * @tc.desc: Verify that the cross switch status is obtained.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, GetCrossingSwitchState002, TestSize.Level1)
{
    std::string deviceId = "123";
    DeviceProfile::MockDistributedDeviceProfileClient::SetDPState(false);
    bool state  = DProfileAdapter->GetCrossingSwitchState(deviceId);
    MMI_HILOGD("Get crossing switch state :%{public}d", state);
    EXPECT_EQ(state, false);
}

/**
 * @tc.number: RegisterCrossingStateListener001
 * @tc.name: RegisterCrossingStateListener
 * @tc.desc: Verify register cross state listener.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, RegisterCrossingStateListener001, TestSize.Level1)
{
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    ProfileEventCallback callback ;
    std::string deviceId = "";
    int32_t regisRet = DProfileAdapter->RegisterCrossingStateListener(deviceId, callback);
    EXPECT_EQ(regisRet, RET_ERR);
}

/**
 * @tc.number: RegisterCrossingStateListener002
 * @tc.name: RegisterCrossingStateListener
 * @tc.desc: Verify register cross state listener.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, RegisterCrossingStateListener002, TestSize.Level1)
{
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    ProfileEventCallback callback;
    callback = [](const std::string &, bool) {
        MMI_HILOGD("callback is ok");
    };
    std::string deviceId = "123";
    int32_t regisRet = DProfileAdapter->RegisterCrossingStateListener(deviceId, callback);
    EXPECT_EQ(regisRet, RET_OK);
}

/**
 * @tc.number: UnregisterCrossingStateListener001
 * @tc.name: UnregisterCrossingStateListener
 * @tc.desc: Verify that cross state listeners are unregistered.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, UnregisterCrossingStateListener001, TestSize.Level1)
{
    std::string deviceId = "";
    int32_t regisRet = DProfileAdapter->UnregisterCrossingStateListener(deviceId);
    EXPECT_EQ(regisRet, RET_ERR);
}

/**
 * @tc.number: UnregisterCrossingStateListener002
 * @tc.name: UnregisterCrossingStateListener
 * @tc.desc: Verify that cross state listeners are unregistered.
 * @tc.require: I5P6WL I5P6ZR
 */
HWTEST_F(IDCooperateTest, UnregisterCrossingStateListener002, TestSize.Level1)
{
    std::string deviceId = "123";
    int32_t regisRet = DProfileAdapter->UnregisterCrossingStateListener(deviceId);
    EXPECT_EQ(regisRet, RET_OK);
}

/**
 * @tc.number: StartInputDeviceCooperate001
 * @tc.name: StartInputDeviceCooperate
 * @tc.desc: Verify that the input device is started for cooperation.
 * @tc.require: I5P6WL I5P772 I5P76M
 */
HWTEST_F(IDCooperateTest, StartInputDeviceCooperate001, TestSize.Level1)
{
    std::string remote = "";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->Reset();
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGD("Start inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
    sleep(300);
    EXPECT_EQ(CooperateState::STATE_FREE, InputDevCooSM->cooperateState_);
}

/**
 * @tc.number: StartInputDeviceCooperate002
 * @tc.name: StartInputDeviceCooperate
 * @tc.desc: Verify that the input device is started for cooperation.
 * @tc.require: I5P6WL I5P772 I5P76M
 */
HWTEST_F(IDCooperateTest, StartInputDeviceCooperate002, TestSize.Level1)
{
    std::string remote = "123";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateIn>(remote);
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGD("Start inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
    sleep(300);
    EXPECT_EQ(CooperateState::STATE_IN, InputDevCooSM->cooperateState_);
}

/**
 * @tc.number: StartInputDeviceCooperate002
 * @tc.name: StartInputDeviceCooperate
 * @tc.desc: Verify that the input device is started for cooperation.
 * @tc.require: I5P6WL I5P772 I5P76M
 */
HWTEST_F(IDCooperateTest, StartInputDeviceCooperate003, TestSize.Level1)//a
{
    std::string remote = "123";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->Reset();
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGD("Start inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
    sleep(300);
    EXPECT_EQ(CooperateState::STATE_FREE, InputDevCooSM->cooperateState_);
}

/**
 * @tc.number: StopInputDeviceCooperate001
 * @tc.name: StopInputDeviceCooperate
 * @tc.desc: Verify that input device cooperation is stopped.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopInputDeviceCooperate001, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateFree>();
    InputDevCooSM->cooperateState_ = CooperateState::STATE_FREE;
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGD("Stop inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
    sleep(300);
    EXPECT_EQ(CooperateState::STATE_FREE, InputDevCooSM->cooperateState_);

}

/**
 * @tc.number: StopInputDeviceCooperate002
 * @tc.name: StopInputDeviceCooperate
 * @tc.desc: Verify that input device cooperation is stopped.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopInputDeviceCooperate002, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateIn>(startDhid);
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGD("Stop inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StopInputDeviceCooperate003
 * @tc.name: StopInputDeviceCooperate
 * @tc.desc: Verify that input device cooperation is stopped.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopInputDeviceCooperate003, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->srcNetworkId_ = "123";
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateOut>(startDhid);
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGD("Stop inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
    sleep(100);
    EXPECT_EQ(CooperateState::STATE_OUT, InputDevCooSM->cooperateState_);
}

/**
 * @tc.number: StartRemoteCooperateResult001
 * @tc.name: StartRemoteCooperateResult
 * @tc.desc: Verify remote cooperation results are started.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteCooperateResult001, TestSize.Level1)
{
    std::string startDhid = "123";
    int32_t xPercent = 10;
    int32_t yPercent = 20; 
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    InputDevCooSM->StartRemoteCooperateResult(false, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

/**
 * @tc.number: StartRemoteCooperateResult002
 * @tc.name: StartRemoteCooperateResult
 * @tc.desc: Verify remote cooperation results are started.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteCooperateResult002, TestSize.Level1)
{
    std::string startDhid = "123";
    int32_t xPercent = 10;
    int32_t yPercent = 20; 
    InputDevCooSM->cooperateState_ = CooperateState::STATE_FREE;
    InputDevCooSM->StartRemoteCooperateResult(true, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

/**
 * @tc.number: StopRemoteCooperate
 * @tc.name: StopRemoteCooperate
 * @tc.desc: Verify Stop Remote cooperation.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteCooperate, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperate();
    EXPECT_EQ(InputDevCooSM->isStopping_, true);
}

/**
 * @tc.number: StopRemoteCooperateResult001
 * @tc.name: StopRemoteCooperateResult
 * @tc.desc: Verify whether to stop remote cooperation results.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteCooperateResult001, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperateResult(true);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

/**
 * @tc.number: StopRemoteCooperateResult002
 * @tc.name: StopRemoteCooperateResult
 * @tc.desc: Verify whether to stop remote cooperation results.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteCooperateResult002, TestSize.Level1) 
{
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    InputDevCooSM->StopRemoteCooperateResult(false);
    MMI_HILOGD("Stop inputdevice cooperate state :%{public}d", InputDevCooSM->cooperateState_);
    EXPECT_NE(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

/**
 * @tc.number: StartCooperateOtherResult
 * @tc.name: StartCooperateOtherResult
 * @tc.desc: Verify start working with other results.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartCooperateOtherResult, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    InputDevCooSM->StartCooperateOtherResult(srcNetworkId);
    EXPECT_EQ(InputDevCooSM->srcNetworkId_, srcNetworkId);
}

/**
 * @tc.number: UpdateState001
 * @tc.name: UpdateState
 * @tc.desc: Verify whether the status is updated.
 * @tc.require: I5P6UW I5P6TG
 */
HWTEST_F(IDCooperateTest, UpdateState001, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

/**
 * @tc.number: UpdateState002
 * @tc.name: UpdateState
 * @tc.desc: Verify whether the status is updated.
 * @tc.require: I5P6UW I5P6TG
 */
HWTEST_F(IDCooperateTest, UpdateState002, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_IN);
    auto state = InputDevCooSM->GetCurrentCooperateState();
    EXPECT_EQ(state, CooperateState::STATE_IN);
}

/**
 * @tc.number: UpdateState003
 * @tc.name: UpdateState
 * @tc.desc: Verify whether the status is updated.
 * @tc.require: I5P6UW I5P6TG
 */
HWTEST_F(IDCooperateTest, UpdateState003, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_OUT);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

/**
 * @tc.number: UpdatePreparedDevices
 * @tc.name: UpdatePreparedDevices
 * @tc.desc: Verify that the prepared device is updated.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, UpdatePreparedDevices, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    std::string sinkNetworkId = "456";
    InputDevCooSM->UpdatePreparedDevices(srcNetworkId, sinkNetworkId);
    EXPECT_EQ(InputDevCooSM->preparedNetworkId_, std::make_pair(srcNetworkId, sinkNetworkId));
}

/**
 * @tc.number: GetPreparedDevices
 * @tc.name: GetPreparedDevices
 * @tc.desc: Verify to obtain the prepared networkId.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, GetPreparedDevices, TestSize.Level1)
{
    std::pair<std::string, std::string> pair {"123", "456"};
    std::pair<std::string, std::string> state = InputDevCooSM->GetPreparedDevices();
    EXPECT_EQ(state, pair);
}

/**
 * @tc.number: GetCurrentCooperateState
 * @tc.name: GetCurrentCooperateState
 * @tc.desc: Verify whether the current cooperation status is obtained.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, GetCurrentCooperateState, TestSize.Level1)
{
    InputDevCooSM->Reset();
    CooperateState  cooperateState = InputDevCooSM->GetCurrentCooperateState();
    MMI_HILOGD("Get current cooperatestate :%{public}d", cooperateState);
    EXPECT_EQ(cooperateState, CooperateState::STATE_FREE);
}

/**
 * @tc.number: InitDeviceManager
 * @tc.name: InitDeviceManager
 * @tc.desc: Verify that Device Manager is initialized.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, InitDeviceManager, TestSize.Level1)
{
    bool state = false;
    state = InputDevCooSM->InitDeviceManager();
    MMI_HILOGD("Get src networkId :%{public}d", state);
    EXPECT_EQ(state, true);
}

/**
 * @tc.number: OnDeviceOnline
 * @tc.name: OnDeviceOnline
 * @tc.desc: Verify online devices.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, OnDeviceOnline, TestSize.Level1) 
{
    std::string networkId = "123";
    InputDevCooSM->OnDeviceOnline(networkId);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        EXPECT_EQ(deviceId, networkId);
    });
}

/**
 * @tc.number: OnDeviceOffline001
 * @tc.name: OnDeviceOffline
 * @tc.desc: Verify that the device is offline.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, OnDeviceOffline001, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    InputDevCooSM->srcNetworkId_ = "456";
    InputDevCooSM->OnDeviceOffline(networkId);
    if (!InputDevCooSM->onlineDevice_.empty()) {
        MMI_HILOGD("onlineDevice is not nullptr");
        std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
        {
            EXPECT_NE(deviceId, networkId);
        });
    }
}

/**
 * @tc.number: GetCrossingSwitchState002
 * @tc.name: UpdateCrossingSwitchState
 * @tc.desc: Verify that the device is offline.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, OnDeviceOffline002, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    InputDevCooSM->srcNetworkId_ = "456";
    InputDevCooSM->OnDeviceOffline(networkId);
    if (!InputDevCooSM->onlineDevice_.empty()) {
        MMI_HILOGD("InputDevCooSM onlineDevice is not nullptr");
        std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
        {
            EXPECT_NE(deviceId, networkId);
        });
    }
}

/**
 * @tc.number: IsStarting001
 * @tc.name: IsStarting
 * @tc.desc: Verify whether to start.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, IsStarting001, TestSize.Level1)
{
    bool isSuccess = false;
    InputDevCooSM->isStarting_ = true;
    isSuccess = InputDevCooSM->IsStarting();
    MMI_HILOGD("The current starting status is:%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, true);
}

/**
 * @tc.number: IsStarting002
 * @tc.name: IsStarting
 * @tc.desc: Verify whether to start.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, IsStarting002, TestSize.Level1)
{
    bool isSuccess = true;
    InputDevCooSM->isStarting_ = false;
    isSuccess = InputDevCooSM->IsStarting();
    MMI_HILOGD("Is starting :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, false);
}

/**
 * @tc.number: IsStopping001
 * @tc.name: IsStopping
 * @tc.desc: Verify whether to stop.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, IsStopping001, TestSize.Level1)
{
    bool isSuccess = false;
    InputDevCooSM->isStopping_= true;
    isSuccess = InputDevCooSM->IsStopping();
    MMI_HILOGD("Is stopping :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, true);
}

/**
 * @tc.number: IsStopping002
 * @tc.name: IsStopping
 * @tc.desc: Verify whether to stop.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, IsStopping002, TestSize.Level1)
{
    bool isSuccess = true;
    InputDevCooSM->isStopping_= false;
    isSuccess = InputDevCooSM->IsStopping();
    MMI_HILOGD("Is stopping :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, false);
}

/**
 * @tc.number: IsNeedFilterOut001
 * @tc.name: IsNeedFilterOut
 * @tc.desc: Verify whether filtering is required.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, IsNeedFilterOut001, TestSize.Level1)
{
    std::string deviceId = "";
    DistributedHardware::DistributedInput::BusinessEvent event;
    bool state = true;
    state = DistributedAdapter->IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(state, false);
}

/**
 * @tc.number: IsNeedFilterOut002
 * @tc.name: IsNeedFilterOut
 * @tc.desc: Verify whether filtering is required.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, IsNeedFilterOut002, TestSize.Level1)
{
    std::string deviceId = "123";
    DistributedHardware::DistributedInput::BusinessEvent event;
    bool state = false;
    state = DistributedAdapter->IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(state, true);
}

/**
 * @tc.number: StartRemoteInput001
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>;
    DICallback callback{nullptr};
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StartRemoteInput002
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>; 
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StopRemoteInput001
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StopRemoteInput002
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StartRemoteInput003
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput003, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StartRemoteInput004
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput004, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StopRemoteInput003
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput003, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StopRemoteInput004
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput004, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StartRemoteInput005
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput005, TestSize.Level1)
{
    std::string deviceId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StartRemoteInput006
 * @tc.name: StartRemoteInput
 * @tc.desc: Verify start remote input.
 * @tc.require: I5P6WL
 */
HWTEST_F(IDCooperateTest, StartRemoteInput006, TestSize.Level1)
{
    std::string deviceId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: StopRemoteInput005
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput005, TestSize.Level1)
{
    std::string deviceId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: StopRemoteInput006
 * @tc.name: StopRemoteInput
 * @tc.desc: Verify stop remote input.
 * @tc.require: I5P705
 */
HWTEST_F(IDCooperateTest, StopRemoteInput006, TestSize.Level1)
{
    std::string deviceId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: PrepareRemoteInput001
 * @tc.name: PrepareRemoteInput
 * @tc.desc: Verify prepare remote input.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, PrepareRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: PrepareRemoteInput002
 * @tc.name: PrepareRemoteInput
 * @tc.desc: Verify prepare remote input.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, PrepareRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: UnPrepareRemoteInput001
 * @tc.name: UnPrepareRemoteInput
 * @tc.desc: Verify that remote input is not prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, UnPrepareRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: UnPrepareRemoteInput002
 * @tc.name: UnPrepareRemoteInput
 * @tc.desc: Verify that remote input is not prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, UnPrepareRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: PrepareRemoteInput003
 * @tc.name: PrepareRemoteInput
 * @tc.desc: Verify that remote input is prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, PrepareRemoteInput003, TestSize.Level1)
{
    std::string deviceId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: PrepareRemoteInput004
 * @tc.name: PrepareRemoteInput
 * @tc.desc: Verify that remote input is prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, PrepareRemoteInput004, TestSize.Level1)
{
    std::string deviceId = "123";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: UnPrepareRemoteInput003
 * @tc.name: UnPrepareRemoteInput
 * @tc.desc: Verify that remote input is not prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, UnPrepareRemoteInput003, TestSize.Level1)
{
    std::string deviceId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: UnPrepareRemoteInput004
 * @tc.name: UnPrepareRemoteInput
 * @tc.desc: Verify that remote input is not prepared.
 * @tc.require:
 */
HWTEST_F(IDCooperateTest, UnPrepareRemoteInput004, TestSize.Level1)
{
    std::string deviceId = "123";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: RegisterEventCallback001
 * @tc.name: RegisterEventCallback
 * @tc.desc: Verify registration event callback.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, RegisterEventCallback001, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback{nullptr};
    int32_t state = DistributedAdapter->RegisterEventCallback(callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: RegisterEventCallback002
 * @tc.name: RegisterEventCallback
 * @tc.desc: Verify registration event callback.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, RegisterEventCallback002, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback = [](uint32_t type, uint32_t code, int32_t value) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->RegisterEventCallback(callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.number: UnregisterEventCallback001
 * @tc.name: UnregisterEventCallback
 * @tc.desc: Verify unregister event callback.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, UnregisterEventCallback001, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback{nullptr};
    int32_t state = DistributedAdapter->UnregisterEventCallback(callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.number: UnregisterEventCallback002
 * @tc.name: UnregisterEventCallback
 * @tc.desc: Verify unregister event callback.
 * @tc.require: I5P6UW
 */
HWTEST_F(IDCooperateTest, UnregisterEventCallback002, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback = [](uint32_t type, uint32_t code, int32_t value) {
        MMI_HILOGD("Callback succeeded");
    };
    int32_t state = DistributedAdapter->UnregisterEventCallback(callback);
    EXPECT_EQ(state, RET_OK);
}
} // namespace MMI
} // namespace OHOS
