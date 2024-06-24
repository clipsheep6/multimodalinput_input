/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "define_multimodal.h"
#include "key_map_manager.h"
#include "libinput_mock.h"
#include "input_device_manager.h"
#include "hos_key_event.h"
#include "key_event.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "KeyMapManagerTest"

namespace OHOS {
namespace MMI {
using namespace testing;
using namespace testing::ext;

class KeyMapManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void KeyMapManagerTest::SetUpTestCase(void)
{}

void KeyMapManagerTest::TearDownTestCase(void)
{}

void KeyMapManagerTest::SetUp()
{}

void KeyMapManagerTest::TearDown()
{}

/**
 * @tc.name: KeyMapManagerTest_GetConfigKeyValue_01
 * @tc.desc: GetConfigKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_GetConfigKeyValue_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    struct libinput_device *device = nullptr;
    std::string fileName = KeyMapMgr->GetKeyEventFileName(device);
    EXPECT_TRUE(fileName.empty());
    ASSERT_NO_FATAL_FAILURE(KeyMapMgr->ParseDeviceConfigFile(device));
}

/**
 * @tc.name: KeyMapManagerTest_GetConfigKeyValue_02
 * @tc.desc: GetConfigKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_GetConfigKeyValue_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    libinput_device device {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetDevice).WillRepeatedly(Return(&device));
    std::string fileName = KeyMapMgr->GetKeyEventFileName(&device);
    EXPECT_FALSE(fileName.empty());
    ASSERT_NO_FATAL_FAILURE(KeyMapMgr->ParseDeviceConfigFile(&device));
}

/**
 * @tc.name: KeyMapManagerTest_RemoveKeyValue_01
 * @tc.desc: RemoveKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_RemoveKeyValue_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId;
    struct libinput_device *device = nullptr;
    deviceId = INPUT_DEV_MGR->FindInputDeviceId(device);
    ASSERT_NO_FATAL_FAILURE(KeyMapMgr->RemoveKeyValue(device));
}

/**
 * @tc.name: KeyMapManagerTest_RemoveKeyValue_02
 * @tc.desc: RemoveKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_RemoveKeyValue_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId;
    libinput_device device {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetDevice).WillRepeatedly(Return(&device));
    deviceId = INPUT_DEV_MGR->FindInputDeviceId(&device);
    ASSERT_NO_FATAL_FAILURE(KeyMapMgr->RemoveKeyValue(&device));
}

/**
 * @tc.name: KeyMapManagerTest_TransferDeviceKeyValue_01
 * @tc.desc: TransferDeviceKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_TransferDeviceKeyValue_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t inputKey = 2;
    struct libinput_device *device = nullptr;
    int32_t ret = KeyMapMgr->TransferDeviceKeyValue(device, inputKey);
    EXPECT_EQ(ret, HOS_KEY_1);
}

/**
 * @tc.name: KeyMapManagerTest_TransferDeviceKeyValue_02
 * @tc.desc: TransferDeviceKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_TransferDeviceKeyValue_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t inputKey = 3;
    int32_t deviceId;
    libinput_device device {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetDevice).WillRepeatedly(Return(&device));
    deviceId = INPUT_DEV_MGR->FindInputDeviceId(&device);
    int32_t ret = KeyMapMgr->TransferDeviceKeyValue(&device, inputKey);
    EXPECT_EQ(ret, HOS_KEY_2);
}

/**
 * @tc.name: KeyMapManagerTest_InputTransferKeyValue_01
 * @tc.desc: InputTransferKeyValue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyMapManagerTest, KeyMapManagerTest_InputTransferKeyValue_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 2;
    int32_t keyCode = KeyEvent::KEYCODE_3;
    std::vector<int32_t> sysKey = { 4 };
    auto ret = KeyMapMgr->InputTransferKeyValue(deviceId, keyCode);
    EXPECT_EQ(ret, sysKey);
}
} // namespace MMI
} // namespace OHOS