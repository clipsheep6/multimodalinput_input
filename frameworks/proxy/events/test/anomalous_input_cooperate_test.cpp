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

#include <gtest/gtest.h>

#include "define_multimodal.h"
#include "mmi_log.h"
#define private public
#include "multimodal_event_handler.h"
#include "input_manager.h"
#include "multimodal_input_connect_manager.h"
#include "input_device_cooperate_impl.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AnomalousInputCooperateTest" };
} // namespace
#define InputMG ::OHOS::DelayedSingleton<InputManager>::GetInstance()
class InputCooperateTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};
class InputDeviceCooperateListenerTest : public IInputDeviceCooperateListener {
public:
    void OnCooperateMessage(const std::string &deviceId, CooperationMessage msg)  override
    {
        MMI_HILOGD("InputDeviceCooperateListenerTest");
    };
};

/**
 * @tc.name: InputCooperateTest_RegisterCooperateListener_001
 * @tc.desc: Verify whether the cooperation listener is registered
 * @tc.type: FUNC
 * @tc.require: I5P6WL
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_RegisterCooperateListener_001, TestSize.Level1)
{
    MMI_HILOGD("RegisterCooperateListener001");
    std::shared_ptr<InputDeviceCooperateListenerTest> listener = nullptr;
    int state = InputMG->RegisterCooperateListener(listener);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.name: InputCooperateTest_RegisterCooperateListener_002
 * @tc.desc: Verify whether the cooperation listener is registered
 * @tc.type: FUNC
 * @tc.require: I5P6WL
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_RegisterCooperateListener_002, TestSize.Level1)
{
    MMI_HILOGD("RegisterCooperateListener002");
    std::shared_ptr<InputDeviceCooperateListenerTest> listener = std::make_shared<InputDeviceCooperateListenerTest>();
    int state = InputMG->RegisterCooperateListener(listener);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.name: InputCooperateTest_UnregisterCooperateListener001
 * @tc.desc: Verify that the cooperation listener is unregistered
 * @tc.type: FUNC
 * @tc.require: I5P6ZR
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_UnregisterCooperateListener001, TestSize.Level1)
{
    MMI_HILOGD("UnregisterCooperateListener001");
    std::shared_ptr<InputDeviceCooperateListenerTest> listener = nullptr;
    int state = InputMG->UnregisterCooperateListener(listener);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.name: InputCooperateTest_UnregisterCooperateListener002
 * @tc.desc: Verify that the cooperation listener is unregistered
 * @tc.type: FUNC
 * @tc.require: I5P6ZR
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_UnregisterCooperateListener002, TestSize.Level1)
{
    MMI_HILOGD("UnregisterCooperateListener002");
    std::shared_ptr<InputDeviceCooperateListenerTest> listener = std::make_shared<InputDeviceCooperateListenerTest>();
    int state = InputMG->UnregisterCooperateListener(listener);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.name: InputCooperateTest_EnableInputDeviceCooperate_001
 * @tc.desc: Verify whether enable input device cooperation
 * @tc.type: FUNC
 * @tc.require: I5P6VG
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_EnableInputDeviceCooperate_001, TestSize.Level1)
{
    bool enabled = true;
    MMI_HILOGD("EnableInputDeviceCooperate001");
    using CooperationCallback = std::function<void(std::string, CooperationMessage)>;
    CooperationCallback callback;
    callback = [](const std::string &, CooperationMessage) {
        MMI_HILOGD("callback is ok");
    };
    InputDevCooperateImpl.userData_ = 2147483647;
    int state = InputMG->EnableInputDeviceCooperate(enabled, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.name: InputCooperateTest_EnableInputDeviceCooperate_002
 * @tc.desc: Verify whether enable input device cooperation
 * @tc.type: FUNC
 * @tc.require: I5P6VG
 */
HWTEST_F(InputCooperateTest, InputCooperateTest_EnableInputDeviceCooperate_002, TestSize.Level1)
{
    bool enabled = true;
    InputDevCooperateImpl.userData_ = 0;
    MMI_HILOGD("EnableInputDeviceCooperate002");
    using CooperationCallback = std::function<void(std::string, CooperationMessage)>;
    CooperationCallback callback;
    callback = [](const std::string &, CooperationMessage) {
        MMI_HILOGD("callback is ok");
    };
    int state = InputMG->EnableInputDeviceCooperate(enabled, callback);
    EXPECT_EQ(state, RET_OK);
}

/**
 * @tc.name: Anomalous_InputCooperateTest_StartInputDeviceCooperate
 * @tc.desc: Verify that the input device is started for cooperation.This is an exception
 * @tc.type: FUNC
 * @tc.require: I5P6WL I5P772 I5P76M
 */

HWTEST_F(InputCooperateTest, Anomalous_InputCooperateTest_StartInputDeviceCooperate, TestSize.Level1)
{
    MMI_HILOGD("StartInputDeviceCooperate");
    std::string sinkDeviceId = "123";
    int32_t srcInputDeviceId = 10;
    InputDevCooperateImpl.userData_ = 0;
    using CooperationCallback = std::function<void(std::string, CooperationMessage)>;
    CooperationCallback callback;
    callback = [](const std::string &, CooperationMessage) {
        MMI_HILOGD("callback is ok");
    };
    int state = InputMG->StartInputDeviceCooperate(sinkDeviceId, srcInputDeviceId, callback);
    EXPECT_EQ(state, RET_ERR);
}

/**
 * @tc.name: Anomalous_IInputCooperateTest_StopDeviceCooperate
 * @tc.desc: Verify whether device cooperation is stopped.This is an exception
 * @tc.type: FUNC
 * @tc.require: I5P6ZR
 */
HWTEST_F(InputCooperateTest, Anomalous_IInputCooperateTest_StopDeviceCooperate, TestSize.Level1)
{
    MMI_HILOGD("StopDeviceCooperate");
    int32_t listen = 1;
    InputDevCooperateImpl.userData_ = 0;
    using CooperationCallback = std::function<void(std::string, CooperationMessage)>;
    CooperationCallback callback;
    callback = [](const std::string &, CooperationMessage) {
        MMI_HILOGD("callback is ok");
    };
    int state = InputMG->StopDeviceCooperate(listen, callback);
    EXPECT_EQ(state, RET_ERR);
}
} // namespace MMI
} // namespace OHOS
