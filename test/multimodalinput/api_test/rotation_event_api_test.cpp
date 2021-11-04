/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "rotation_event.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "RotationEventApiTest" };


class RotationEventApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string &json, const bool ansert, RotationEvent &rotationEventTest);
private:
    static StClientHelper st_client_helper_;
};

StClientHelper RotationEventApiTest::st_client_helper_;

void RotationEventApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void RotationEventApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void RotationEventApiTest::SetUp()
{
}

void RotationEventApiTest::TearDown()
{
}

void RotationEventApiTest::Initialize(const std::string &json, const bool ansert, RotationEvent &rotationEventTest)
{
    //Initialize("Api_Test_GetOriginEventType_01_0.json",true,mouseEventTest);
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    //struct AutoTestClientKeyEventInitPkt_ KeyEventInitData = GetDataProcess().GetApiTitle("eventKeyboard");
    GetSTHelper().InJectionEvent(json,true,manipulationEventTest);
    /*if (ansert == true and KeyEventInitData.windowId != 0) {
        rotationEventTest.Initialize(KeyEventInitData.windowId, KeyEventInitData.isPressed, KeyEventInitData.keyCode,
        KeyEventInitData.keyDownDuration, KeyEventInitData.highLevelEvent, KeyEventInitData.uuid,
        KeyEventInitData.sourceType, KeyEventInitData.occurredTime, KeyEventInitData.deviceId,
        KeyEventInitData.inputDeviceId, KeyEventInitData.isHighLevelEvent, KeyEventInitData.deviceUdevTags,
        KeyEventInitData.deviceEventType);
       
    }
    */
    //GetDataProcess().ClearAll();
}
/*
 * Feature: Api_Test_GetRotationValue_00
 * Function: Api_Test_GetRotationValue_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为旋钮左旋动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_00, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("Api_Test_GetRotationValue_00.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue != 0);
}


/*
 * Feature: Api_Test_GetRotationValue_01
 * Function: Api_Test_GetRotationValue_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为旋钮右旋动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_01, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("Api_Test_GetRotationValue_01.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
}


/*
 * Feature: Api_Test_GetRotationValue_02
 * Function: Api_Test_GetRotationValue_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为鼠标按键动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_02, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("Api_Test_GetRotationValue_02_0.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("Api_Test_GetRotationValue_02_1.json",false,rotationEventTest);
}


/*
 * Feature: Api_Test_GetRotationValue_03
 * Function: Api_Test_GetRotationValue_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为摇杆按键动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_03, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("rock_key_295_press.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("rock_key_295_release.json",false,rotationEventTest);
}

/*
 * Feature: Api_Test_GetRotationValue_04
 * Function: Api_Test_GetRotationValue_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为手柄按键动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_04, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("gamepad_key_172_press.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("gamepad_key_172_release.json",false,rotationEventTest);
}




/*
 * Feature: Api_Test_GetRotationValue_05
 * Function: Api_Test_GetRotationValue_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为touch-pad-touch动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_05, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("Api_Test_GetRotationValue_05_00.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("Api_Test_GetRotationValue_05_01.json",true,rotationEventTest);
}


/*
 * Feature: Api_Test_GetRotationValue_08
 * Function: Api_Test_GetRotationValue_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为轨迹球按键动作
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_08, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("trackball_leftbtn_press.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("trackball_leftbtn_release.json",false,rotationEventTest);
}

/*
 * Feature: Api_Test_GetRotationValue_09
 * Function: Api_Test_GetRotationValue_09
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为旋钮按键
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_09, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("knob_model1_press.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("knob_model1_release.json",false,rotationEventTest);
}

/*
 * Feature: Api_Test_GetRotationValue_08
 * Function: Api_Test_GetRotationValue_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   json动作为键盘按键
 */
HWTEST_F(RotationEventApiTest, Api_Test_GetRotationValue_08, TestSize.Level1)
{
    RotationEvent rotationEventTest;
    Initialize("keyboard_KEY_U_press.json",true,rotationEventTest);
    auto retRotationValue = rotationEventTest.GetRotationValue();
    EXPECT_TRUE(retRotationValue == 0);
    Initialize("keyboard_KEY_U_release.json",false,rotationEventTest);
}
} // namespace