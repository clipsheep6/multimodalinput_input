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
//#include <iostream>
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

HWTEST_F(IDCooperateTest, UpdateCrossingSwitchState001, TestSize.Level1)
{
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true);
    MMI_HILOGI("[jy]Update crossing switch state :%{public}d", rest);
    EXPECT_EQ(rest, DP_GET_SERVICE_SUCCESS);
}

HWTEST_F(IDCooperateTest, UpdateCrossingSwitchState002, TestSize.Level1)
{
    std::vector<std::string> deviceIds = {"123"};
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true, deviceIds);
    MMI_HILOGI("[jy]Update crossing switch state :%{public}d", rest);
    EXPECT_EQ(rest, DP_GET_SERVICE_SUCCESS);
}

HWTEST_F(IDCooperateTest, GetCrossingSwitchState001, TestSize.Level1)
{
    std::string deviceId = "123";
    DeviceProfile::MockDistributedDeviceProfileClient::SetDPState(true);
    bool state  = DProfileAdapter->GetCrossingSwitchState(deviceId);
    MMI_HILOGI("[jy]Get crossing switch state :%{public}d", state);
    EXPECT_EQ(state, true);
}

HWTEST_F(IDCooperateTest, GetCrossingSwitchState002, TestSize.Level1)
{
    std::string deviceId = "123";
    DeviceProfile::MockDistributedDeviceProfileClient::SetDPState(false);
    bool state  = DProfileAdapter->GetCrossingSwitchState(deviceId);
    MMI_HILOGI("[jy]Get crossing switch state :%{public}d", state);
    EXPECT_EQ(state, false);
}

HWTEST_F(IDCooperateTest, RegisterCrossingStateListener001, TestSize.Level1)
{
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    ProfileEventCallback callback ;
    std::string deviceId = "";
    int32_t regisRet = DProfileAdapter->RegisterCrossingStateListener(deviceId, callback);
    EXPECT_EQ(regisRet, RET_ERR);
}

HWTEST_F(IDCooperateTest, RegisterCrossingStateListener002, TestSize.Level1)
{
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    ProfileEventCallback callback;
    callback = [](const std::string &, bool)
    {
        MMI_HILOGI("[jy]callback is ok");
    };
    std::string deviceId = "123";
    int32_t regisRet = DProfileAdapter->RegisterCrossingStateListener(deviceId, callback);
    EXPECT_EQ(regisRet, RET_OK);
}

HWTEST_F(IDCooperateTest, UnregisterCrossingStateListener001, TestSize.Level1)
{
    std::string deviceId = "";
    int32_t regisRet = DProfileAdapter->UnregisterCrossingStateListener(deviceId);
    EXPECT_EQ(regisRet, RET_ERR);
}

HWTEST_F(IDCooperateTest, UnregisterCrossingStateListener002, TestSize.Level1)
{
    std::string deviceId = "123";
    int32_t regisRet = DProfileAdapter->UnregisterCrossingStateListener(deviceId);
    EXPECT_EQ(regisRet, RET_OK);
}

HWTEST_F(IDCooperateTest, StartInputDeviceCooperate001, TestSize.Level1)
{
    std::string remote = "";
    std::string startDhid = "123";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->Reset();
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGI("[jy] StartInputDeviceCooperate]Start inputdevive cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
}


HWTEST_F(IDCooperateTest, StartInputDeviceCooperate002, TestSize.Level1)
{
    std::string remote = "123";
    std::string startDhid = "456";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->Reset();
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGI("[jy] StartInputDeviceCooperate002]Start inputdevive cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StopInputDeviceCooperate001, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateFree>();
    InputDevCooSM->cooperateState_ = CooperateState::STATE_FREE;
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGI("[jy StopInputDeviceCooperate]Stop inputdevive cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StopInputDeviceCooperate002, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateIn>(startDhid);
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGI("[jy StopInputDeviceCooperate]Stop inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StopInputDeviceCooperate003, TestSize.Level1)
{
    std::string startDhid = "123";
    InputDevCooSM->isStopping_ = false;
    InputDevCooSM->isStarting_ = false;
    InputDevCooSM->srcNetworkId_ = "123";
    InputDevCooSM->currentStateSM_ = std::make_shared<InputDeviceCooperateStateOut>(startDhid);
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    MMI_HILOGI("[jy 1]");
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGI("[jy StopInputDeviceCooperate]Stop inputdevice cooperate state :%{public}d", state);
    EXPECT_EQ(state, RET_ERR);
}


HWTEST_F(IDCooperateTest, StartRemoteCooperateResult001, TestSize.Level1)
{
    std::string startDhid = "123";
    int32_t xPercent = 10;
    int32_t yPercent = 20; 
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    InputDevCooSM->StartRemoteCooperateResult(false, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

HWTEST_F(IDCooperateTest, StartRemoteCooperateResult002, TestSize.Level1)
{
    std::string startDhid = "123";
    int32_t xPercent = 10;
    int32_t yPercent = 20; 
    InputDevCooSM->cooperateState_ = CooperateState::STATE_FREE;
    InputDevCooSM->StartRemoteCooperateResult(true, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

HWTEST_F(IDCooperateTest, StopRemoteCooperate, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperate();
    EXPECT_EQ(InputDevCooSM->isStopping_, true);
}

HWTEST_F(IDCooperateTest, StopRemoteCooperateResult001, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperateResult(true);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

HWTEST_F(IDCooperateTest, StopRemoteCooperateResult002, TestSize.Level1) 
{
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    InputDevCooSM->StopRemoteCooperateResult(false);
    MMI_HILOGI("[jy StopInputDeviceCooperate]Stop inputdevive cooperate state :%{public}d", InputDevCooSM->cooperateState_);
    EXPECT_NE(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

HWTEST_F(IDCooperateTest, StartCooperateOtherResult, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    InputDevCooSM->StartCooperateOtherResult(srcNetworkId);
    EXPECT_EQ(InputDevCooSM->srcNetworkId_, srcNetworkId);
}

HWTEST_F(IDCooperateTest, UpdateState001, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

HWTEST_F(IDCooperateTest, UpdateState002, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_IN);
    auto state = InputDevCooSM->GetCurrentCooperateState();
    EXPECT_EQ(state, CooperateState::STATE_IN);
}

HWTEST_F(IDCooperateTest, UpdateState003, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_OUT);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

HWTEST_F(IDCooperateTest, UpdatePreparedDevices, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    std::string sinkNetworkId = "456";
    InputDevCooSM->UpdatePreparedDevices(srcNetworkId, sinkNetworkId);
    EXPECT_EQ(InputDevCooSM->preparedNetworkId_, std::make_pair(srcNetworkId, sinkNetworkId));
}

HWTEST_F(IDCooperateTest, GetPreparedDevices, TestSize.Level1)
{
    std::pair<std::string, std::string> pair {"123", "456"};
    std::pair<std::string, std::string> state = InputDevCooSM->GetPreparedDevices();
    EXPECT_EQ(state, pair);
}

HWTEST_F(IDCooperateTest, GetCurrentCooperateState, TestSize.Level1)
{
    InputDevCooSM->Reset();
    CooperateState  cooperateState = InputDevCooSM->GetCurrentCooperateState();
    MMI_HILOGI("[jy]Get current cooperatestate :%{public}d", cooperateState);
    EXPECT_EQ(cooperateState, CooperateState::STATE_FREE);
}

HWTEST_F(IDCooperateTest, InitDeviceManager, TestSize.Level1)
{
    bool state = false;
    state = InputDevCooSM->InitDeviceManager();
    MMI_HILOGI("[jy]Get src networkId :%{public}d", state);
    EXPECT_EQ(state, true);
}

HWTEST_F(IDCooperateTest, OnDeviceOnline, TestSize.Level1) 
{
    std::string networkId = "123";
    InputDevCooSM->OnDeviceOnline(networkId);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        EXPECT_EQ(deviceId, networkId);
    });
}

HWTEST_F(IDCooperateTest, OnDeviceOffline001, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->cooperateState_ = CooperateState::STATE_OUT;
    InputDevCooSM->srcNetworkId_ = "456";
    InputDevCooSM->OnDeviceOffline(networkId);
    if (!InputDevCooSM->onlineDevice_.empty()) {
        MMI_HILOGI("[jy]InputDevCooSM->onlineDevice_ is not nullptr");
        std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
        {
            EXPECT_NE(deviceId, networkId);
        });
    }
}

HWTEST_F(IDCooperateTest, OnDeviceOffline002, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    InputDevCooSM->srcNetworkId_ = "456";
    InputDevCooSM->OnDeviceOffline(networkId);
    if (!InputDevCooSM->onlineDevice_.empty()) {
        MMI_HILOGI("[jy]InputDevCooSM->onlineDevice_ is not nullptr");
        std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
        {
            EXPECT_NE(deviceId, networkId);
        });
    }
}

HWTEST_F(IDCooperateTest, IsStarting001, TestSize.Level1)
{
    bool isSuccess = false;
    InputDevCooSM->isStarting_ = true;
    isSuccess = InputDevCooSM->IsStarting();
    MMI_HILOGI("[jy]Is starting :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, true);
}

HWTEST_F(IDCooperateTest, IsStarting002, TestSize.Level1)
{
    bool isSuccess = true;
    InputDevCooSM->isStarting_ = false;
    isSuccess = InputDevCooSM->IsStarting();
    MMI_HILOGI("[jy]Is starting :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, false);
}

HWTEST_F(IDCooperateTest, IsStopping001, TestSize.Level1)
{
    bool isSuccess = false;
    InputDevCooSM->isStopping_= true;
    isSuccess = InputDevCooSM->IsStopping();
    MMI_HILOGI("[jy]Is stopping :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, true);
}

HWTEST_F(IDCooperateTest, IsStopping002, TestSize.Level1)
{
    bool isSuccess = true;
    InputDevCooSM->isStopping_= false;
    isSuccess = InputDevCooSM->IsStopping();
    MMI_HILOGI("[jy]Is stopping :%{public}d", isSuccess);
    EXPECT_EQ(isSuccess, false);
}

HWTEST_F(IDCooperateTest, IsNeedFilterOut001, TestSize.Level1)
{
    std::string deviceId = "";
    DistributedHardware::DistributedInput::BusinessEvent event;
    bool state = true;
    state = DistributedAdapter->IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(state, false);
}

HWTEST_F(IDCooperateTest, IsNeedFilterOut002, TestSize.Level1)
{
    std::string deviceId = "123";
    DistributedHardware::DistributedInput::BusinessEvent event;
    bool state = false;
    state = DistributedAdapter->IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(state, true);
}

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

HWTEST_F(IDCooperateTest, StartRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>; 
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StopRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StopRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    uint32_t  inputTypes = 1;
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StartRemoteInput003, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}
HWTEST_F(IDCooperateTest, StartRemoteInput004, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StopRemoteInput003, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StopRemoteInput004, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StartRemoteInput005, TestSize.Level1)
{
    std::string deviceId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    MMI_HILOGI("[jy] StartRemoteInput005");
    int32_t state = DistributedAdapter->StartRemoteInput(deviceId, dhIds, callback);
    MMI_HILOGI("[jy] StartRemoteInput005_1");
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StartRemoteInput006, TestSize.Level1)
{
    std::string deviceId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){   
        MMI_HILOGI("[jy] callback success");
    };
    MMI_HILOGI("[jy] StartRemoteInput006");
    int32_t state = DistributedAdapter->StartRemoteInput(deviceId, dhIds, callback);
    MMI_HILOGI("[jy] StartRemoteInput006_1");
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, StopRemoteInput005, TestSize.Level1)
{
    std::string deviceId = "";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, StopRemoteInput006, TestSize.Level1)
{
    std::string deviceId = "123";
    std::vector<std::string> dhIds{"123"};
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->StopRemoteInput(deviceId, dhIds, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, PrepareRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, PrepareRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, UnPrepareRemoteInput001, TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, UnPrepareRemoteInput002, TestSize.Level1)
{
    std::string srcId = "123";
    std::string sinkId = "456";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, PrepareRemoteInput003, TestSize.Level1)
{
    std::string deviceId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, PrepareRemoteInput004, TestSize.Level1)
{
    std::string deviceId = "123";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, UnPrepareRemoteInput003, TestSize.Level1)
{
    std::string deviceId = "";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, UnPrepareRemoteInput004, TestSize.Level1)
{
    std::string deviceId = "123";
    using DICallback = std::function<void(bool)>;
    DICallback callback = [](bool state){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->UnPrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, RegisterEventCallback001, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback{nullptr};
    int32_t state = DistributedAdapter->RegisterEventCallback(callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, RegisterEventCallback002, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback = [](uint32_t type, uint32_t code, int32_t value){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->RegisterEventCallback(callback);
    EXPECT_EQ(state, RET_OK);
}

HWTEST_F(IDCooperateTest, UnregisterEventCallback001, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback{nullptr};
    int32_t state = DistributedAdapter->UnregisterEventCallback(callback);
    EXPECT_EQ(state, RET_ERR);
}

HWTEST_F(IDCooperateTest, UnregisterEventCallback002, TestSize.Level1)
{
    using MouseStateChangeCallback = std::function<void(uint32_t type, uint32_t code, int32_t value)>;
    MouseStateChangeCallback callback = [](uint32_t type, uint32_t code, int32_t value){
        MMI_HILOGI("[jy] callback success");
    };
    int32_t state = DistributedAdapter->UnregisterEventCallback(callback);
    EXPECT_EQ(state, RET_OK);
}

} // namespace MMI
} // namespace OHOS
