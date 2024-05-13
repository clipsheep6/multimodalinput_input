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

#include <fstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mmi_log.h"
#include "uds_server.h"
#include "uds_session.h"

#include "input_device_manager.h"
#include "libinput-private.h"


namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class InputDeviceManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

class MockUDSSession: public UDSSession {
public:
    MOCK_METHOD1(SendMsg, int32_t(NetPacket &));
    MockUDSSession() : UDSSession("mockProgram", 1, 2, 3, 4) {}
};


/**
 * @tc.name: GetInputDevice_Test_001
 * @tc.desc: Test the function GetInputDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetInputDevice_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    std::shared_ptr<InputDevice> inputDeviceManager{nullptr};
    int32_t id = 1;
    bool checked = true;
    inputDeviceManager = inputDevice.GetInputDevice(id, checked);
    EXPECT_EQ(inputDeviceManager, nullptr);
}

/**
 * @tc.name: GetInputDeviceIds_Test_001
 * @tc.desc: Test the function GetInputDeviceIds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetInputDeviceIds_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    ASSERT_NO_FATAL_FAILURE(inputDevice.GetInputDeviceIds());
}

/**
 * @tc.name: SupportKeys_Test_001
 * @tc.desc: Test the function SupportKeys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, SupportKeys_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t deviceId = 1;
    std::vector<int32_t> keyCodes{12};
    std::vector<bool> keystroke{true};
    int32_t returnCode = 401;
    int32_t ret = inputDevice.SupportKeys(deviceId, keyCodes, keystroke);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: GetDeviceConfig_Test_001
 * @tc.desc: Test the function GetDeviceConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetDeviceConfig_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t deviceId = 1;
    int32_t keyboardType = 1;
    bool ret = inputDevice.GetDeviceConfig(deviceId, keyboardType);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetDeviceSupportKey_Test_001
 * @tc.desc: Test the function GetDeviceSupportKey
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetDeviceSupportKey_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t deviceId = 1;
    int32_t keyboardType = 1;
    int32_t returnCode = 401;
    int32_t ret = inputDevice.GetDeviceSupportKey(deviceId, keyboardType);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: GetKeyboardType_Test_001
 * @tc.desc: Test the function GetKeyboardType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetKeyboardType_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t deviceId = 1;
    int32_t keyboardType = 1;
    int32_t returnCode = 401;
    int32_t ret = inputDevice.GetKeyboardType(deviceId, keyboardType);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: HasTouchDevice_Test_001
 * @tc.desc: Test the function HasTouchDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, HasTouchDevice_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    bool ret = inputDevice.HasTouchDevice();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ScanPointerDevice_Test_001
 * @tc.desc: Test the function ScanPointerDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, ScanPointerDevice_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    ASSERT_NO_FATAL_FAILURE(inputDevice.ScanPointerDevice());
}

/**
 * @tc.name: Dump_Test_001
 * @tc.desc: Test the function Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, Dump_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t fd = 1;
    std::vector<std::string> args{"test"};
    ASSERT_NO_FATAL_FAILURE(inputDevice.Dump(fd, args));
}

/**
 * @tc.name: DumpDeviceList_Test_001
 * @tc.desc: Test the function DumpDeviceList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, DumpDeviceList_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t fd = 1;
    std::vector<std::string> args{"test"};
    ASSERT_NO_FATAL_FAILURE(inputDevice.DumpDeviceList(fd, args));
}

/**
 * @tc.name: GetVendorConfig_Test_001
 * @tc.desc: Test the function GetVendorConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, GetVendorConfig_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    int32_t deviceId = 1;
    ASSERT_NO_FATAL_FAILURE(inputDevice.GetVendorConfig(deviceId));
}

/**
 * @tc.name: OnEnableInputDevice_Test_001
 * @tc.desc: Test the function OnEnableInputDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, OnEnableInputDevice_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    bool enable = true;
    int32_t ret = inputDevice.OnEnableInputDevice(enable);
    EXPECT_EQ(ret, RET_OK);
    enable = false;
    ret = inputDevice.OnEnableInputDevice(enable);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InitSessionLostCallback_Test_001
 * @tc.desc: Test the function InitSessionLostCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, InitSessionLostCallback_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    ASSERT_NO_FATAL_FAILURE(inputDevice.InitSessionLostCallback());
}

/**
 * @tc.name: InitSessionLostCallback_Test_002
 * @tc.desc: Test the function InitSessionLostCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, InitSessionLostCallback_Test_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    inputDevice.sessionLostCallbackInitialized_ = true;
    ASSERT_NO_FATAL_FAILURE(inputDevice.InitSessionLostCallback());
}

/**
 * @tc.name: InitSessionLostCallback_Test_003
 * @tc.desc: Test the function InitSessionLostCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, InitSessionLostCallback_Test_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    inputDevice.sessionLostCallbackInitialized_ = false;
    ASSERT_NO_FATAL_FAILURE(inputDevice.InitSessionLostCallback());
    EXPECT_FALSE(inputDevice.sessionLostCallbackInitialized_);
}

/**
 * @tc.name: OnSessionLost_Test_001
 * @tc.desc: Test the function OnSessionLost
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, OnSessionLost_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    SessionPtr session = std::make_shared<MockUDSSession>();
    ASSERT_NE(session, nullptr);
    ASSERT_NO_FATAL_FAILURE(inputDevice.OnSessionLost(session));
    session = nullptr;
    ASSERT_NO_FATAL_FAILURE(inputDevice.OnSessionLost(session));
}


/**
 * @tc.name: NotifyMessage_Test_001
 * @tc.desc: Test the function NotifyMessage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, NotifyMessage_Test_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    std::shared_ptr<MockUDSSession> mockSession = std::make_shared<MockUDSSession>();
    EXPECT_CALL(*mockSession, SendMsg(testing::_)).WillRepeatedly(testing::Return(true));
    int32_t result = inputDevice.NotifyMessage(mockSession, 1, "type");
    EXPECT_EQ(result, RET_OK);
}

/**
 * @tc.name: NotifyMessage_Test_002
 * @tc.desc: Test the function NotifyMessage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, NotifyMessage_Test_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    std::shared_ptr<MockUDSSession> mockSession = std::make_shared<MockUDSSession>();
    EXPECT_CALL(*mockSession, SendMsg(testing::_)).WillRepeatedly(testing::Return(false));
    int32_t result = inputDevice.NotifyMessage(mockSession, 1, "type");
    EXPECT_EQ(result, RET_OK);
}

/**
 * @tc.name: NotifyMessage_Test_003
 * @tc.desc: Test the function NotifyMessage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputDeviceManagerTest, NotifyMessage_Test_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputDeviceManager inputDevice;
    std::shared_ptr<MockUDSSession> mockSession = std::make_shared<MockUDSSession>();
    EXPECT_CALL(*mockSession, SendMsg(testing::_)).WillRepeatedly(testing::Return(false));
    SessionPtr nullSession = nullptr;
    int32_t result = inputDevice.NotifyMessage(nullSession, 1, "type");
    EXPECT_NE(result, RET_OK);
}
} // namespace MMI
} // namespace OHOS
