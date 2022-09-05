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
#include <iostream>
#include <memory>

#include <gtest/gtest.h>
#include "cooperate_event_manager.h"
#include "multimodal_input_connect_def_parcel.h"
#define private public
#include "input_device_cooperate_sm.h"
#include "device_profile_adapter.h"

namespace OHOS {
namespace MMI {
using namespace testing::ext;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateTest" };
} // namespace
class IDAdapterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

HWTEST_F(IDAdapterTest, test001, TestSize.Level1)
{
    InputDevCooSM->EnableInputDeviceCooperate(true);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        bool state = DProfileAdapter->GetCrossingSwitchState(deviceId);
        EXPECT_EQ(state, true);
    });
    sleep(1);
    InputDevCooSM->EnableInputDeviceCooperate(false);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        bool state = DProfileAdapter->GetCrossingSwitchState(deviceId);
        EXPECT_EQ(state, false);
    });
}

HWTEST_F(IDAdapterTest, test002, TestSize.Level1)
{
    std::string remote = "123";
    int32_t startInputDeviceId = 10;
    int32_t state = InputDevCooSM->StartInputDeviceCooperate(remote, startInputDeviceId);
    MMI_HILOGD("Start inputdevive cooperate state :%{public}d", state);
    EXPECT_EQ(state, -1);
}

HWTEST_F(IDAdapterTest, test003, TestSize.Level1)
{
    int32_t state = InputDevCooSM->StopInputDeviceCooperate();
    MMI_HILOGD("Stop inputdevive cooperate state :%{public}d", state);
    EXPECT_EQ(state, -1);
}

HWTEST_F(IDAdapterTest, test004, TestSize.Level1)
{
    std::string deviceId = "123";
    InputDevCooSM->GetCooperateState(deviceId);
    bool state = false;
    state = DProfileAdapter->GetCrossingSwitchState(deviceId);
    EXPECT_NE(state, true);
}

HWTEST_F(IDAdapterTest, test005, TestSize.Level1)
{
    std::string remote = "123";
    InputDevCooSM->StartRemoteCooperate(remote);
    EXPECT_EQ(InputDevCooSM->isStarting_, true);
}

HWTEST_F(IDAdapterTest, test006, TestSize.Level1)
{
    std::string startDhid = "";
    int32_t xPercent = 10;
    int32_t yPercent = 20; 
    InputDevCooSM->StartRemoteCooperateResult(false, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
    sleep(1);
    InputDevCooSM->StartRemoteCooperateResult(true, startDhid, xPercent, yPercent);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_IN);
}

HWTEST_F(IDAdapterTest, test007, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperate();
    EXPECT_EQ(InputDevCooSM->isStopping_, true);
}

HWTEST_F(IDAdapterTest, test008, TestSize.Level1)
{
    InputDevCooSM->StopRemoteCooperateResult(true);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
    sleep(1);
    InputDevCooSM->StopRemoteCooperateResult(false);
    EXPECT_EQ(InputDevCooSM->isStopping_, false);
}

HWTEST_F(IDAdapterTest, test009, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    InputDevCooSM->StartCooperateOtherResult(srcNetworkId);
    EXPECT_EQ(InputDevCooSM->srcNetworkId_, srcNetworkId);
}

HWTEST_F(IDAdapterTest, test0010, TestSize.Level1)
{
    struct libinput_event *event = nullptr;
    InputDevCooSM->HandleLibinputEvent(event);
    EXPECT_EQ(InputDevCooSM->isStopping_, 0);
}

HWTEST_F(IDAdapterTest, test0011, TestSize.Level1)
{
    InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
    sleep(1);
    InputDevCooSM->UpdateState(CooperateState::STATE_IN);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_IN);
    sleep(1);
    InputDevCooSM->UpdateState(CooperateState::STATE_OUT);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

HWTEST_F(IDAdapterTest, test0012, TestSize.Level1)
{
    std::string srcNetworkId = "123";
    std::string sinkNetworkId = "456";
    InputDevCooSM->UpdatePreparedDevices(srcNetworkId, sinkNetworkId);
    EXPECT_EQ(InputDevCooSM->preparedNetworkId_, std::make_pair(srcNetworkId, sinkNetworkId));
}

HWTEST_F(IDAdapterTest, test0013, TestSize.Level1)
{
    std::pair<std::string, std::string> pair {"123", "456"};
    std::pair<std::string, std::string> state = InputDevCooSM->GetPreparedDevices();
    EXPECT_EQ(state, pair);
}

HWTEST_F(IDAdapterTest, test0014, TestSize.Level1)
{
    std::shared_ptr<IInputDeviceCooperateState> pair{ nullptr };
    std::shared_ptr<IInputDeviceCooperateState> currentState= InputDevCooSM->GetCurrentState();
    EXPECT_NE(currentState, pair);
}

HWTEST_F(IDAdapterTest, test0015, TestSize.Level1)
{
    CooperateState pair { CooperateState::STATE_OUT };
    CooperateState  cooperateState = InputDevCooSM->GetCurrentCooperateState();
    MMI_HILOGD("Get current cooperatestate :%{public}d", cooperateState);
    EXPECT_EQ(cooperateState, pair);
}

HWTEST_F(IDAdapterTest, test0016, TestSize.Level1)
{
    std::string state = "123";
    InputDevCooSM->srcNetworkId_ = "123";
    std::string  srcNetworkId = InputDevCooSM->GetSrcNetworkId();
    MMI_HILOGD("Get srcNetworkId :%{public}s", srcNetworkId.c_str());
    EXPECT_EQ(srcNetworkId, state);
}

HWTEST_F(IDAdapterTest, test0017, TestSize.Level1)
{
    std::string networkId = "123456";
    std::string srcNetworkId = "";
    std::string sinkNetworkId = "";
    InputDevCooSM->UpdatePreparedDevices("123", "456");
    InputDevCooSM->OnCooperateChanged(networkId, false);
    EXPECT_EQ(InputDevCooSM->preparedNetworkId_, std::make_pair(srcNetworkId,sinkNetworkId));

}

HWTEST_F(IDAdapterTest, test0018, TestSize.Level1)
{
    std::string dhid = "123";
    std::string srcNetworkId = "456";
    std::string sinkNetworkId = "789";
    InputDevCooSM->UpdatePreparedDevices("456", "789");
    InputDevCooSM->OnKeyboardOnline(dhid);
    EXPECT_EQ(InputDevCooSM->GetPreparedDevices(), std::make_pair(srcNetworkId,sinkNetworkId));
}

HWTEST_F(IDAdapterTest, test0019, TestSize.Level1)
{
    std::string dhid = "123";
    std::string sinkNetworkId = "456";
    std::vector<std::string>  keyboards;
    InputDevCooSM->cooperateState_ = CooperateState::STATE_IN;
    sleep(1);
    InputDevCooSM->OnPointerOffline(dhid, sinkNetworkId, keyboards);
    EXPECT_EQ(InputDevCooSM->startDhid_, "");
}

HWTEST_F(IDAdapterTest, test0020, TestSize.Level1)
{
    bool state = false;
    state = InputDevCooSM->InitDeviceManager();
    MMI_HILOGD("Get src networkId :%{public}d", state);
    EXPECT_EQ(state, true);
}

HWTEST_F(IDAdapterTest, test0021, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->OnDeviceOnline(networkId);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        EXPECT_EQ(deviceId, networkId);
    });
}

HWTEST_F(IDAdapterTest, test0022, TestSize.Level1)
{
    std::string networkId = "123";
    InputDevCooSM->OnDeviceOffline(networkId);
    std::for_each(InputDevCooSM->onlineDevice_.begin(), InputDevCooSM->onlineDevice_.end(),[&, this](std::string &deviceId)
    {
        EXPECT_EQ(deviceId, "");
    });
}

HWTEST_F(IDAdapterTest, test0023, TestSize.Level1)
{
    bool isSucess = true;
    std::string remote = "123";
    int32_t startInputDeviceId = 10;
    InputDevCooSM->cooperateState_ = CooperateState::STATE_FREE;
    InputDevCooSM->StartFinish(isSucess, remote, startInputDeviceId);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_OUT);
}

HWTEST_F(IDAdapterTest, test0024, TestSize.Level1)
{
    bool isSucess = true;
    std::string remote = "123";
    InputDevCooSM->StopFinish(isSucess, remote);
    EXPECT_EQ(InputDevCooSM->cooperateState_, CooperateState::STATE_FREE);
}

HWTEST_F(IDAdapterTest, test0025, TestSize.Level1)
{
    bool isSucess = false;
    InputDevCooSM->isStarting_ = true;
    isSucess = InputDevCooSM->IsStarting();
    MMI_HILOGD("Is starting :%{public}d", isSucess);
    EXPECT_EQ(isSucess, true);
}

HWTEST_F(IDAdapterTest, test0026, TestSize.Level1)
{
    bool isSucess = false;
    InputDevCooSM->isStopping_= true;
    isSucess = InputDevCooSM->IsStopping();
    MMI_HILOGD("Is stopping :%{public}d", isSucess);
    EXPECT_EQ(isSucess, true);
}
} // namespace MMI
} // namespace OHOS
