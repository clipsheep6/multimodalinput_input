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
#include "mmi_point.h"
#include "st_helper.h"
#include "multimodal_event.h"
#include "multimodal_event_handler.h"
#include "st_data_process.h"
#include "st_client_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MmiPointApiTest" };

class MmiPointApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void Initialize(const std::string& eventValue, const std::string& deviceMsg, const bool ansert, MmiPoint &mmiPointTest);
    std::string ToStrings(const std::string& px, const std::string& py, const std::string& pz) const
    
    
private:
    static StClientHelper st_client_helper_;
};

StClientHelper MmiPointApiTest::st_client_helper_;

void MmiPointApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void MmiPointApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void MmiPointApiTest::SetUp()
{
}

void MmiPointApiTest::TearDown()
{
}

std::string MmiPointApiTest::ToStrings(const std::string& px, const std::string& py, const std::string& pz) const
{
    return "MmiPoint{ px =" + std::to_string(px)
        + ", py = " + std::to_string(py)
        + ", pz = " + std::to_string(pz) + "}";
}

/*
 * Feature: Api_Test_GetAction_0
 * Function: Api_Test_GetAction_0
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
void MmiPointApiTest::Initialize(const std::string& eventValue, const std::string& deviceMsg, const bool ansert, MmiPoint &mmiPointTest)
{
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    auto KeyEventInitData = GetDataProcess().GetOnKeyData();
    GetSTHelper().InJectionEvent(eventValue, deviceMsg);
    if (ansert == true and KeyEventInitData.windowId != 0) {
        keyEventTest.Initialize(KeyEventInitData.windowId, KeyEventInitData.isPressed, KeyEventInitData.keyCode,
        KeyEventInitData.keyDownDuration, KeyEventInitData.highLevelEvent, KeyEventInitData.uuid,
        KeyEventInitData.sourceType, KeyEventInitData.occurredTime, KeyEventInitData.deviceId,
        KeyEventInitData.inputDeviceId, KeyEventInitData.isHighLevelEvent, KeyEventInitData.deviceUdevTags,
        KeyEventInitData.deviceEventType);
       
    }
    GetDataProcess().ClearAll();
}

/*
* Feature: Api_Test_GetX_0
* Function: Api_Test_GetX_0
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_GetX_0, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("ABS_VOLUME_press.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetX();
    Initialize("ABS_VOLUME_release.json",false,mmiPointTest);
    EXPECT_EQ(ret, 0);
}

/*
 * Feature: Api_Test_GetX_1
 * Function: Api_Test_GetX_1
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetX_1, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("keyboard_KEY_A_press.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetX();
    EXPECT_EQ(ret, 0);
    Initialize("keyboard_KEY_A_release.json",false,mmiPointTest);
}


/*
 * Feature: Api_Test_GetX_2
 * Function: Api_Test_GetX_2
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetX_2, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("mouse_move_1.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetX();
    EXPECT_TRUE(ret != 0);
}


/*
 * Feature: Api_Test_GetX_3
 * Function: Api_Test_GetX_3
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 "xPos":125,"yPos":237,
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetX_3, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("touch_singlefinger_down_move_1.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetX();
    EXPECT_TRUE(ret == 125);
}


/*
 * Feature: Api_Test_GetY_00
 * Function: Api_Test_GetY_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetY_00, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("gamepad_key_172_press.json",true,mmiPointTest);
    auto retActionButton = mmiPointTest.GetY();
    Initialize("gamepad_key_172_release.json",false,mmiPointTest);
    EXPECT_EQ(retActionButton, 0);
}


/*
 * Feature: Api_Test_GetY_01
 * Function: Api_Test_GetY_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetY_01, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("keyboard_KEY_2_press.json",true,mmiPointTest);
    auto retActionButton = mmiPointTest.GetY();
    Initialize("keyboard_KEY_2_release.json",false,mmiPointTest);
    EXPECT_EQ(retActionButton, 0);
}


/*
 * Feature: Api_Test_GetY_02
 * Function: Api_Test_GetY_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetY_02, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("mouse_move_2.json",true,mmiPointTest);
    auto retActionButton = mmiPointTest.GetY();
    EXPECT_TRUE(ret != 0);
}


/*
 * Feature: Api_Test_GetY_03
 * Function: Api_Test_GetY_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NABACK--278
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetY_03, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("touch_singlefinger_down_move_2.json",true,mmiPointTest);
    auto retActionButton = mmiPointTest.GetY();
    EXPECT_TRUE(ret == 237);
}

/*
 * Feature: Api_Test_GetZ_00
 * Function: Api_Test_GetZ_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetZ_00, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("ABS_VOLUME_press.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetZ();
    Initialize("ABS_VOLUME_release.json",false,mmiPointTest);
    EXPECT_EQ(ret, 0);
}

/*
 * Feature: Api_Test_GetZ_01
 * Function: Api_Test_GetZ_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetZ_01, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("keyboard_KEY_A_press.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetZ();
    EXPECT_EQ(ret, 0);
    Initialize("keyboard_KEY_A_release.json",false,mmiPointTest);
    EXPECT_EQ(ret, 0);
}

/*
 * Feature: Api_Test_GetZ_02
 * Function: Api_Test_GetZ_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MmiPointApiTest, Api_Test_GetZ_02, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("mouse_move_1.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetZ();
    EXPECT_EQ(ret, 0);
}

/*
* Feature: Api_Test_GetZ_03
* Function: Api_Test_GetZ_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_GetZ_03, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("touch_singlefinger_down_move_1.json",true,mmiPointTest);
    auto ret = mmiPointTest.GetZ();
    EXPECT_EQ(ret, 0);
}



/*
* Feature: Api_Test_ToString_00
* Function: Api_Test_ToString_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_ToString_00, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("ABS_VOLUME_press.json",true,mmiPointTest);
    auto retX = mmiPointTest.GetX();
    auto retY = mmiPointTest.GetY();
    auto retZ = mmiPointTest.GetZ();
    Initialize("ABS_VOLUME_release.json",false,mmiPointTest);
    ABS_VOLUME_release
    EXPECT_EQ(ToString, ToStrings(retX,retY,retZ));
    EXPECT_TRUE(retX == 0);
    EXPECT_TRUE(retY == 0);
    EXPECT_TRUE(retZ == 0);
    
}

/*
* Feature: Api_Test_ToString_01
* Function: Api_Test_ToString_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_ToString_01, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("keyboard_KEY_A_press.json",true,mmiPointTest);;
    auto retX = mmiPointTest.GetX();
    auto retY = mmiPointTest.GetY();
    auto retZ = mmiPointTest.GetZ();
    Initialize("keyboard_KEY_A_release.json",false,mmiPointTest);
    EXPECT_EQ(ToString, ToStrings(retX,retY,retZ));
    EXPECT_TRUE(retX == 0);
    EXPECT_TRUE(retY == 0);
    EXPECT_TRUE(retZ == 0);
    
}


/*
* Feature: Api_Test_ToString_02
* Function: Api_Test_ToString_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_ToString_02, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("mouse_move_1.json",true,mmiPointTest);
    auto retX = mmiPointTest.GetX();
    auto retY = mmiPointTest.GetY();
    auto retZ = mmiPointTest.GetZ();
    EXPECT_EQ(ToString, ToStrings(retX,retY,retZ));
    EXPECT_TRUE(retX != 0);
    EXPECT_TRUE(retY != 0);
    EXPECT_TRUE(retZ == 0);
    
}

/*
* Feature: Api_Test_ToString_03
* Function: Api_Test_ToString_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(MmiPointApiTest, Api_Test_ToString_03, TestSize.Level1)
{
    MmiPoint mmiPointTest;
    Initialize("touch_singlefinger_down_move_1.json",true,mmiPointTest);
    auto retX = mmiPointTest.GetX();
    auto retY = mmiPointTest.GetY();
    auto retZ = mmiPointTest.GetZ();
    EXPECT_EQ(ToString, ToStrings(retX,retY,retZ));
    EXPECT_TRUE(retX == 125);
    EXPECT_TRUE(retY == 237);
    EXPECT_TRUE(retZ == 0);
    
}
} // namespace
