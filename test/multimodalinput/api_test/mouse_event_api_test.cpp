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
#include "mouse_event.h"
#include "st_helper.h"
#include "multimodal_event.h"
#include "multimodal_event_handler.h"
#include "st_data_process.h"
#include "st_client_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MouseEventApiTest" };

class MouseEventApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void Initialize(const std::string& eventValue, const bool ansert, MouseEvent &mouseEventTest);
    
private:
    static StClientHelper st_client_helper_;
};

StClientHelper MouseEventApiTest::st_client_helper_;

void MouseEventApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void MouseEventApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void MouseEventApiTest::SetUp()
{
}

void MouseEventApiTest::TearDown()
{
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
 
void MouseEventApiTest::Initialize(const std::string &json, const bool ansert, MouseEvent &mouseEventTest)
{
    //Initialize("Api_Test_GetOriginEventType_01_0.json",true,mouseEventTest);
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    //struct AutoTestClientKeyEventInitPkt_ KeyEventInitData = GetDataProcess().GetApiTitle("eventKeyboard");
    GetSTHelper().InJectionEvent(json,true,manipulationEventTest);
    /*if (ansert == true and KeyEventInitData.windowId != 0) {
        keyEventTest.Initialize(KeyEventInitData.windowId, KeyEventInitData.isPressed, KeyEventInitData.keyCode,
        KeyEventInitData.keyDownDuration, KeyEventInitData.highLevelEvent, KeyEventInitData.uuid,
        KeyEventInitData.sourceType, KeyEventInitData.occurredTime, KeyEventInitData.deviceId,
        KeyEventInitData.inputDeviceId, KeyEventInitData.isHighLevelEvent, KeyEventInitData.deviceUdevTags,
        KeyEventInitData.deviceEventType);
       
    }
    */
    //GetDataProcess().ClearAll();
}

HWTEST_F(MouseEventApiTest, Api_Test_GetAction_0, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetAction_0.json",true,mouseEventTest);
    auto retAction = mouseEventTest.GetAction();
    EXPECT_EQ(retAction, 0);
}

/*
 * Feature: Api_Test_GetAction_1
 * Function: Api_Test_GetAction_1
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetAction_1, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetAction_1.json",true,mouseEventTest);
    auto retAction = mouseEventTest.GetAction();
    EXPECT_EQ(retAction, 3);
}


/*
 * Feature: Api_Test_GetAction_2
 * Function: Api_Test_GetAction_2
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetAction_2, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetAction_2.json",true,mouseEventTest);
    auto retAction = mouseEventTest.GetAction();
    EXPECT_EQ(retAction, 1);
}


/*
 * Feature: Api_Test_GetAction_3
 * Function: Api_Test_GetAction_3
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetAction_3, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetAction_3.json",true,mouseEventTest);
    auto retAction = mouseEventTest.GetAction();
    EXPECT_EQ(retAction, 2);
}


/*
 * Feature: Api_Test_GetActionButton_00
 * Function: Api_Test_GetActionButton_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_00.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 1);
}


/*
 * Feature: Api_Test_GetActionButton_01
 * Function: Api_Test_GetActionButton_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_01, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_01.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 2);
}


/*
 * Feature: Api_Test_GetActionButton_02
 * Function: Api_Test_GetActionButton_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_02, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_02.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 4);
}


/*
 * Feature: Api_Test_GetActionButton_03
 * Function: Api_Test_GetActionButton_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NABACK--278
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_03, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_03.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 8);
}


/*
 * Feature: Api_Test_GetActionButton_04
 * Function: Api_Test_GetActionButton_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NAFORWARD--277
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_04, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_04.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 16);
}


/*
 * Feature: Api_Test_GetActionButton_05
 * Function: Api_Test_GetActionButton_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NAEXTRA--276
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_05, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_05.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 64);
}


/*
 * Feature: Api_Test_GetActionButton_06
 * Function: Api_Test_GetActionButton_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA_SIDE--275
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetActionButton_06, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetActionButton_06.json",true,mouseEventTest);
    auto retActionButton = mouseEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 32);
}


/*
 * Feature: Api_Test_GetPressedButtons_00
 * Function: Api_Test_GetPressedButtons_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetPressedButtons_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetPressedButtons_00.json",true,mouseEventTest);
    auto retPressedButtons = mouseEventTest.GetPressedButtons();
    EXPECT_EQ(retPressedButtons, 0);
}

/*
 * Feature: Api_Test_GetPressedButtons_01
 * Function: Api_Test_GetPressedButtons_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetPressedButtons_01, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetPressedButtons_01.json",true,mouseEventTest);
    auto retPressedButtons = mouseEventTest.GetPressedButtons();
    EXPECT_EQ(retPressedButtons, 1);
}

/*
 * Feature: Api_Test_GetPressedButtons_02
 * Function: Api_Test_GetPressedButtons_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetPressedButtons_02, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetPressedButtons_02.json",true,mouseEventTest);
    auto retPressedButtons = mouseEventTest.GetPressedButtons();
    EXPECT_EQ(retPressedButtons, 0);
}

/*
 * Feature: Api_Test_GetPressedButtons_03
 * Function: Api_Test_GetPressedButtons_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetPressedButtons_03, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetPressedButtons_03.json",true,mouseEventTest);
    auto retPressedButtons = mouseEventTest.GetPressedButtons();
    EXPECT_EQ(retPressedButtons, 1);
}

/*
 * Feature: Api_Test_GetPressedButtons_04
 * Function: Api_Test_GetPressedButtons_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetPressedButtons_04, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetPressedButtons_04.json",true,mouseEventTest);
    auto retPressedButtons = mouseEventTest.GetPressedButtons();
    EXPECT_EQ(retPressedButtons, 0);
}


/*
 * Feature: Api_Test_GetCursor_00
 * Function: Api_Test_GetCursor_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetCursor_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetCursor_00.json",true,mouseEventTest);
    auto retCursor = mouseEventTest.GetCursor();
    float px = 0.56f;
    float py = 15.548f;
    retCursor.Setxy(px, py);
    auto retX = retCursor.GetX();
    EXPECT_FLOAT_EQ(retX, px);
}


/*
 * Feature: Api_Test_SetCursorOffset_GetXOffset_00
 * Function: Api_Test_SetCursorOffset_GetXOffset_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_SetCursorOffset_GetXOffset_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_SetCursorOffset_GetXOffset_00.json",true,mouseEventTest);
    float offsetX = 48.185f;
    float offsetY = 3.545f;
    mouseEventTest.SetCursorOffset(offsetX, offsetY);
    auto retXOffset = mouseEventTest.GetXOffset();
    EXPECT_FLOAT_EQ(retXOffset, offsetX);
}


/*
 * Feature: Api_Test_GetAxisValue_00
 * Function: Api_Test_GetAxisValue_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetAxisValue_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetAxisValue_00.json",true,mouseEventTest);
    int32_t axis = 100;
    auto retAxisValue = mouseEventTest.GetAxisValue(axis);
    EXPECT_FLOAT_EQ(retAxisValue, 0.0f);
}


/*
 * Feature: Api_Test_GetCursorDelta_00
 * Function: Api_Test_GetCursorDelta_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetCursorDelta_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetCursorDelta_00.json",true,mouseEventTest);
    auto retCursorDelta = mouseEventTest.GetCursorDelta(0);
    EXPECT_FLOAT_EQ(retCursorDelta, 0);
}

/*
 * Feature: Api_Test_GetCursorDelta_01
 * Function: Api_Test_GetCursorDelta_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA0
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetCursorDelta_01, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetCursorDelta_01.json",true,mouseEventTest);
    auto retCursorDelta = mouseEventTest.GetCursorDelta(1);
    EXPECT_FLOAT_EQ(retCursorDelta, 0);
}

/*
 * Feature: Api_Test_GetCursorDelta_02
 * Function: Api_Test_GetCursorDelta_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetCursorDelta_02, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetCursorDelta_02.json",true,mouseEventTest);
    auto retCursorDelta = mouseEventTest.GetCursorDelta(1);
    EXPECT_FLOAT_EQ(retCursorDelta, 0);
}

/*
 * Feature: Api_Test_GetScrollingDelta_00
 * Function: Api_Test_GetScrollingDelta_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetScrollingDelta_00, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetScrollingDelta_00.json",true,mouseEventTest);
    int32_t axis = 100;
    auto retScrollingDelta = mouseEventTest.GetScrollingDelta(axis);
    EXPECT_FLOAT_EQ(retScrollingDelta, 0.0f);
}

/*
 * Feature: Api_Test_GetScrollingDelta_01
 * Function: Api_Test_GetScrollingDelta_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MouseEventApiTest, Api_Test_GetScrollingDelta_01, TestSize.Level1)
{
    MouseEvent mouseEventTest;
    Initialize("Api_Test_GetScrollingDelta_01.json",true,mouseEventTest);
    auto retScrollingDelta = mouseEventTest.GetScrollingDelta(1);
    EXPECT_FLOAT_EQ(retScrollingDelta, 0.0f);
}
} // namespace
