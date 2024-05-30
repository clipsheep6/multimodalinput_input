/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <cinttypes>

#include "event_util_test.h"
#include "input_manager_util.h"
#include "input_manager.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "VirtualInputDeviceTest"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t TIME_WAIT_FOR_OP = 100;

} // namespace

class VirtualInputDeviceTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    std::string GetEventDump();
};


void VirtualInputDeviceTest::SetUpTestCase()
{
    ASSERT_TRUE(TestUtil->Init());
}

void VirtualInputDeviceTest::SetUp()
{
    TestUtil->SetRecvFlag(RECV_FLAG::RECV_FOCUS);
}

void VirtualInputDeviceTest::TearDown()
{
    TestUtil->AddEventDump("");
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
}

std::string VirtualInputDeviceTest::GetEventDump()
{
    return TestUtil->GetEventDump();
}

/**
 * @tc.name: VirtualInputDeviceTest_AddVirtualInputDevice_001
 * @tc.desc: Add virtual input device
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VirtualInputDeviceTest, VirtualInputDeviceTest_AddVirtualInputDevice_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto cmpInputDevice = [] (std::shared_ptr<InputDevice> one, std::shared_ptr<InputDevice> other) -> bool {
        return one->GetName() == other->GetName() && one->GetType() == other->GetType() &&
            one->GetBus() == other->GetBus() && one->GetVersion() == other->GetVersion() &&
            one->GetProduct() == other->GetProduct() && one->GetVendor() == other->GetVendor() &&
            one->GetPhys() == other->GetPhys() && one->GetUniq() == other->GetUniq() &&
            one->HasCapability(InputDeviceCapability::INPUT_DEV_CAP_KEYBOARD) ==
            other->HasCapability(InputDeviceCapability::INPUT_DEV_CAP_KEYBOARD) &&
            one->HasCapability(InputDeviceCapability::INPUT_DEV_CAP_POINTER) ==
            other->HasCapability(InputDeviceCapability::INPUT_DEV_CAP_POINTER);
    };

    auto inputDeviceAdd = std::make_shared<InputDevice>();
    inputDeviceAdd->SetName("VirtualDeviceName");
    inputDeviceAdd->SetType(-1);
    inputDeviceAdd->SetBus(-1);
    inputDeviceAdd->SetVersion(-1);
    inputDeviceAdd->SetProduct(-1);
    inputDeviceAdd->SetVendor(-1);
    inputDeviceAdd->SetPhys("Phys");
    inputDeviceAdd->SetUniq("unique");
    inputDeviceAdd->AddCapability(InputDeviceCapability::INPUT_DEV_CAP_KEYBOARD);
    int32_t deviceId { -1 };
    InputManager::GetInstance()->AddVirtualInputDevice(inputDeviceAdd, deviceId);
    auto inputDeviceGet = std::make_shared<InputDevice>();
    inputDeviceGet = nullptr;
    auto inputDevFun = [] (std::shared_ptr<InputDevice>) {
        MMI_HILOGI("GetDevice Executed");
    };
    ASSERT_TRUE(InputManager::GetInstance()->GetDevice(deviceId, inputDevFun) == RET_OK);
    ASSERT_TRUE(cmpInputDevice(inputDeviceAdd, inputDeviceGet));
}

/**
 * @tc.name: VirtualInputDeviceTest_RemoveVirtualInputDevice_001
 * @tc.desc: Remove virtual input device
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VirtualInputDeviceTest, VirtualInputDeviceTest_RemoveVirtualInputDevice_001, TestSize.Level1)
{
    auto inputDeviceAdd = std::make_shared<InputDevice>();
    int32_t deviceId { -1 };
    InputManager::GetInstance()->AddVirtualInputDevice(inputDeviceAdd, deviceId);
    auto inputDeviceGet = std::make_shared<InputDevice>();
    inputDeviceGet = nullptr;
    auto inputDevFun = [] (std::shared_ptr<InputDevice>) {
        MMI_HILOGI("GetDevice Executed");
    };
    ASSERT_TRUE(InputManager::GetInstance()->GetDevice(deviceId, inputDevFun) == RET_OK);
    ASSERT_TRUE(inputDeviceAdd == inputDeviceGet);
    InputManager::GetInstance()->RemoveVirtualInputDevice(deviceId);
    ASSERT_TRUE(InputManager::GetInstance()->GetDevice(deviceId, inputDevFun) != RET_OK);
}
} // namespace MMI
} // namespace OHOS
