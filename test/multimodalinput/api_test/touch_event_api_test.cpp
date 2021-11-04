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
#include "touch_event.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS;

class TouchEventApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string &json, const bool ansert, TouchEvent &touchEventTest);
private:
    static StClientHelper st_client_helper_;
};

StClientHelper MultimodalEventsApiTest::st_client_helper_;

void MultimodalEventsApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void MultimodalEventsApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void MultimodalEventsApiTest::SetUp()
{
}

void MultimodalEventsApiTest::TearDown()
{
}

void MultimodalEventsApiTest::Initialize(const std::string &json, const bool ansert, TouchEvent &touchEventTest)
{
    //Initialize("Api_Test_Common_01_0.json",true,mouseEventTest);
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    //struct AutoTestClientKeyEventInitPkt_ KeyEventInitData = GetDataProcess().GetApiTitle("eventKeyboard");
    GetSTHelper().InJectionEvent(json,true,manipulationEventTest);
    /*if (ansert == true and KeyEventInitData.windowId != 0) {
        multimodalEventTest.Initialize(KeyEventInitData.windowId, KeyEventInitData.isPressed, KeyEventInitData.keyCode,
        KeyEventInitData.keyDownDuration, KeyEventInitData.highLevelEvent, KeyEventInitData.uuid,
        KeyEventInitData.sourceType, KeyEventInitData.occurredTime, KeyEventInitData.deviceId,
        KeyEventInitData.inputDeviceId, KeyEventInitData.isHighLevelEvent, KeyEventInitData.deviceUdevTags,
        KeyEventInitData.deviceEventType);
       
    }
    */
    //GetDataProcess().ClearAll();
}


/*
 * Feature: Api_Test_GetOriginEventType_00
 * Function: Api_Test_GetOriginEventType_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(TouchEventApiTest, Api_Test_GetOriginEventType_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetOriginEventType_00.json",true,touchEventTest);
    auto retOriginEventType = touchEventTest.GetOriginEventType();
    EXPECT_EQ(retOriginEventType, 4);
}

/*
 * Feature: Api_Test_GetOriginEventType_01
 * Function: Api_Test_GetOriginEventType_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetOriginEventType_01, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetOriginEventType_01.json",true,touchEventTest);
    auto retOriginEventType = touchEventTest.GetOriginEventType();
    EXPECT_EQ(retOriginEventType,  7);
}

/*
 * Feature: Api_Test_GetOriginEventType_02
 * Function: Api_Test_GetOriginEventType_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 

HWTEST_F(TouchEventApiTest, Api_Test_GetOriginEventType_02, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetOriginEventType_02.json",true,touchEventTest);
    auto retOriginEventType = touchEventTest.GetOriginEventType();
    EXPECT_EQ(retOriginEventType,  8);
}

/*
 * Feature: Api_Test_GetOriginEventType_03
 * Function: Api_Test_GetOriginEventType_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetOriginEventType_03, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetOriginEventType_03.json",true,touchEventTest);
    auto retOriginEventType = touchEventTest.GetOriginEventType();
    EXPECT_EQ(retOriginEventType,  10);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_00
 * Function: Api_Test_GetMultimodalEvent_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetMultimodalEvent_00.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = nullptr;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_01
 * Function: Api_Test_GetMultimodalEvent_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_01, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetMultimodalEvent_01.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = nullptr;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_02
 * Function: Api_Test_GetMultimodalEvent_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_02, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetMultimodalEvent_02.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = nullptr;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_03
 * Function: Api_Test_GetMultimodalEvent_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_03, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetMultimodalEvent_03.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = nullptr;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_04
 * Function: Api_Test_GetMultimodalEvent_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_04, TestSize.Level1)
{
    TouchEvent touchEventTest;
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_GetMultimodalEvent_04.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = &multimodalEventTest;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_05
 * Function: Api_Test_GetMultimodalEvent_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_05, TestSize.Level1)
{
    TouchEvent touchEventTest;
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_GetMultimodalEvent_05.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = &multimodalEventTest;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}

/*
 * Feature: Api_Test_GetMultimodalEvent_06
 * Function: Api_Test_GetMultimodalEvent_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_06, TestSize.Level1)
{
    TouchEvent touchEventTest;
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_GetMultimodalEvent_06.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = &multimodalEventTest;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetMultimodalEvent_07
 * Function: Api_Test_GetMultimodalEvent_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetMultimodalEvent_07, TestSize.Level1)
{
    TouchEvent touchEventTest;
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_GetMultimodalEvent_07.json",true,touchEventTest);
    MultimodalEventPtr deviceEvent = &multimodalEventTest;
    auto retMultimodalEvent = touchEventTest.GetMultimodalEvent();
    EXPECT_EQ(retMultimodalEvent,  deviceEvent);
}


/*
 * Feature: Api_Test_GetAction_00
 * Function: Api_Test_GetAction_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetAction_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetAction_00.json",true,touchEventTest);
    int32_t action = 0;
    auto retAction = touchEventTest.GetAction();
    EXPECT_EQ(retAction, action);
}


/*
 * Feature: Api_Test_GetAction_01
 * Function: Api_Test_GetAction_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetAction_01, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetAction_01.json",true,touchEventTest);
    int32_t action = 1;
    auto retAction = touchEventTest.GetAction();
    EXPECT_EQ(retAction, action);
}


/*
 * Feature: Api_Test_GetAction_02
 * Function: Api_Test_GetAction_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetAction_02, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetAction_02.json",true,touchEventTest);
    int32_t action = 2;
    auto retAction = touchEventTest.GetAction();
    EXPECT_EQ(retAction, action);
}

/*
 * Feature: Api_Test_GetAction_03
 * Function: Api_Test_GetAction_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetAction_03, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetAction_03.json",true,touchEventTest);
    auto retAction = touchEventTest.GetAction();
    EXPECT_EQ(retAction, 0);
}


/*
 * Feature: Api_Test_GetAction_04
 * Function: Api_Test_GetAction_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetAction_04, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetAction_04.json",true,touchEventTest);
    auto retAction = touchEventTest.GetAction();
    EXPECT_EQ(retAction, 0);
}


/*
 * Feature: Api_Test_GetIndex_00
 * Function: Api_Test_GetIndex_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetIndex_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetIndex_00.json",true,touchEventTest);
    auto retIndex = touchEventTest.GetIndex();
    EXPECT_EQ(retIndex, 0);
}

/*
 * Feature: Api_Test_GetIndex_01
 * Function: Api_Test_GetIndex_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetIndex_01, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetIndex_01.json",true,touchEventTest);
    auto retIndex = touchEventTest.GetIndex();
    EXPECT_EQ(retIndex, 1);
}

/*
 * Feature: Api_Test_GetIndex_02
 * Function: Api_Test_GetIndex_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetIndex_02, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetIndex_02.json",true,touchEventTest);
    auto retIndex = touchEventTest.GetIndex();
    EXPECT_EQ(retIndex, 2);
}

/*
 * Feature: Api_Test_GetIndex_03
 * Function: Api_Test_GetIndex_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetIndex_03, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetIndex_03.json",true,touchEventTest);
    auto retIndex = touchEventTest.GetIndex();
    EXPECT_EQ(retIndex, 3);
}


/*
 * Feature: Api_Test_GetForcePrecision_00
 * Function: Api_Test_GetForcePrecision_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
   
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetForcePrecision_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    auto retForcePrecision = touchEventTest.GetForcePrecision();
    EXPECT_FLOAT_EQ(retForcePrecision, 0.0f);
}


/*
 * Feature: Api_Test_GetMaxForce_00
 * Function: Api_Test_GetMaxForce_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
    
 */

HWTEST_F(TouchEventApiTest, Api_Test_GetMaxForce_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    auto retMaxForce = touchEventTest.GetMaxForce();
    EXPECT_FLOAT_EQ(retMaxForce, 0.0f);
}


/*
 * Feature: Api_Test_GetTapCount_00
 * Function: Api_Test_GetTapCount_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetTapCount_00, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetTapCount_00.json",true,touchEventTest);
    auto retTapCount = touchEventTest.GetTapCount();
    EXPECT_FLOAT_EQ(retTapCount, 0.0f);
}


/*
 * Feature: Api_Test_GetTapCount_01
 * Function: Api_Test_GetTapCount_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetTapCount_01, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetTapCount_01.json",true,touchEventTest);
    float tapCount = 0.0f;
    auto retTapCount = touchEventTest.GetTapCount();
    EXPECT_FLOAT_EQ(retTapCount, tapCount);
}

/*
 * Feature: Api_Test_GetTapCount_02
 * Function: Api_Test_GetTapCount_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(TouchEventApiTest, Api_Test_GetTapCount_02, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetTapCount_02.json",true,touchEventTest);
    float tapCount = 1.0f;
    auto retTapCount = touchEventTest.GetTapCount();
    EXPECT_FLOAT_EQ(retTapCount, tapCount);
}

/*
 * Feature: Api_Test_GetTapCount_03
 * Function: Api_Test_GetTapCount_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
 
HWTEST_F(TouchEventApiTest, Api_Test_GetTapCount_03, TestSize.Level1)
{
    TouchEvent touchEventTest;
    Initialize("Api_Test_GetTapCount_03.json",true,touchEventTest);
    float tapCount = 2.0f;
    auto retTapCount = touchEventTest.GetTapCount();
    EXPECT_FLOAT_EQ(retTapCount, tapCount);
}

} // namespace
