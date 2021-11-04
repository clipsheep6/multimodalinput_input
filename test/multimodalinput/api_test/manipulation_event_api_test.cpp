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
#include "manipulation_event.h"
#include "st_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ManipulationEventsApiTest" };

class ManipulationEventsApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void Initialize(const std::string& eventValue, const bool ansert, ManipulationEvent &manipulationEventTest);
    
private:
    static StClientHelper st_client_helper_;
};

StClientHelper ManipulationEventsApiTest::st_client_helper_;

void ManipulationEventsApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void ManipulationEventsApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void ManipulationEventsApiTest::SetUp()
{
}

void ManipulationEventsApiTest::TearDown()
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
 
void ManipulationEventsApiTest::Initialize(const std::string &json, const bool ansert, ManipulationEvent &manipulationEventTest)
{
    //Initialize("Api_Test_GetOriginEventType_01_0.json",true,mouseEventTest);
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    //struct AutoTestClientKeyEventInitPkt_ KeyEventInitData = GetDataProcess().GetApiTitle("eventKeyboard");
    GetSTHelper().InJectionEvent(json,true,manipulationEventTest);
    /*if (ansert == true and KeyEventInitData.windowId != 0) {
        manipulationEventTest.Initialize(KeyEventInitData.windowId, KeyEventInitData.isPressed, KeyEventInitData.keyCode,
        KeyEventInitData.keyDownDuration, KeyEventInitData.highLevelEvent, KeyEventInitData.uuid,
        KeyEventInitData.sourceType, KeyEventInitData.occurredTime, KeyEventInitData.deviceId,
        KeyEventInitData.inputDeviceId, KeyEventInitData.isHighLevelEvent, KeyEventInitData.deviceUdevTags,
        KeyEventInitData.deviceEventType);
       
    }
    */
    //GetDataProcess().ClearAll();
}

/*
 * Feature: Api_Test_GetStartTime_00
 * Function: Api_Test_GetStartTime_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_00.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_01
 * Function: Api_Test_GetStartTime_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_01.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_02
 * Function: Api_Test_GetStartTime_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_02, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_02.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_03
 * Function: Api_Test_GetStartTime_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_03, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_03.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_04
 * Function: Api_Test_GetStartTime_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_04, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_04.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_05
 * Function: Api_Test_GetStartTime_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_05, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_04.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_06
 * Function: Api_Test_GetStartTime_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_06, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_06.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetStartTime_07
 * Function: Api_Test_GetStartTime_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetStartTime_07, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetStartTime_07.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetStartTime();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_00
 * Function: Api_Test_GetPhase_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_00, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_00.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_01
 * Function: Api_Test_GetPhase_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_01, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_01.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_02
 * Function: Api_Test_GetPhase_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_02, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_02.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_03
 * Function: Api_Test_GetPhase_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_03, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_03.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_04
 * Function: Api_Test_GetPhase_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_04, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_04.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_05
 * Function: Api_Test_GetPhase_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_05, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_05.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_06
 * Function: Api_Test_GetPhase_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_06, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_06.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPhase_07
 * Function: Api_Test_GetPhase_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
 
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPhase_07, TestSize.Level1)
{
    
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPhase_07.json",true,manipulationEventTest);
    auto retStartTime =  manipulationEventTest.GetPhase();
    EXPECT_EQ(retStartTime, 0);
}


/*
 * Feature: Api_Test_GetPointerPosition_00
 * Function: Api_Test_GetPointerPosition_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_00.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(-1);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}


/*
 * Feature: Api_Test_GetPointerPosition_01
 * Function: Api_Test_GetPointerPosition_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_01.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(10);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}


/*
 * Feature: Api_Test_GetPointerPosition_02
 * Function: Api_Test_GetPointerPosition_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_02, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_02.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(11);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}

/*
 * Feature: Api_Test_GetPointerPosition_03
 * Function: Api_Test_GetPointerPosition_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_03, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_03.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(0);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}


/*
 * Feature: Api_Test_GetPointerPosition_04
 * Function: Api_Test_GetPointerPosition_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_04, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_04.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(0);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}


/*
 * Feature: Api_Test_GetPointerPosition_05
 * Function: Api_Test_GetPointerPosition_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_05, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerPosition_05.json",true,manipulationEventTest);
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(0);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_Fail, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 2;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[1].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 2;
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(index);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0.0f);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_Abnormal, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 2;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[1].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 0;
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(index);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0.0f);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerPosition_Abnormal_Parameter, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 2;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[1].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 0xFFFFFFFF;
    auto retPointerPosition = manipulationEventTest.GetPointerPosition(index);
    auto retX = retPointerPosition.GetX();
    EXPECT_FLOAT_EQ(retX, 0.0f);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_SetScreenOffset_GetFingersInfos_Normal, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 2;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
                                     highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
                                     isHighLevelEvent, deviceUdevTags);
    float offsetX = 1.1f;
    float offsetY = 1.1f;
    manipulationEventTest.SetScreenOffset(offsetX, offsetY);
    auto FingersInfos = manipulationEventTest.GetFingersInfos();
    auto retY = FingersInfos->mMp.GetY();
    EXPECT_FLOAT_EQ(retY, offsetY);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_SetScreenOffset_GetFingersInfos_Abnormal, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 2;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    float offsetX = 1.1f;
    auto offsetY = static_cast<float>(0xFFFFFFFFFFFFFFFF);
    manipulationEventTest.SetScreenOffset(offsetX, offsetY);
    auto FingersInfos = manipulationEventTest.GetFingersInfos();
    auto retY = FingersInfos->mMp.GetY();
    EXPECT_FLOAT_EQ(retY, offsetY);
}


/*
 * Feature: Api_Test_GetPointerCount_00
 * Function: Api_Test_GetPointerCount_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_00.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 1);
}


/*
 * Feature: Api_Test_GetPointerCount_01
 * Function: Api_Test_GetPointerCount_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_01.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_02
 * Function: Api_Test_GetPointerCount_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_02, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_02.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_03
 * Function: Api_Test_GetPointerCount_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_03, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_03_0.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    Initialize("Api_Test_GetPointerCount_03_1.json",false,manipulationEventTest);
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_04
 * Function: Api_Test_GetPointerCount_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_04, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_04.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_05
 * Function: Api_Test_GetPointerCount_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_05, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_05.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_06
 * Function: Api_Test_GetPointerCount_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_06, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_06.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_07
 * Function: Api_Test_GetPointerCount_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_07, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_07.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_08
 * Function: Api_Test_GetPointerCount_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_08, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_08.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}



/*
 * Feature: Api_Test_GetPointerCount_009
 * Function: Api_Test_GetPointerCount_009
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerCount_009, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerCount_009.json",true,manipulationEventTest);
    auto retPointerCount = manipulationEventTest.GetPointerCount();
    EXPECT_EQ(retPointerCount, 0);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerScreenPosition_Normal, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 3;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[2].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 2;
    auto retPointerPosition = manipulationEventTest.GetPointerScreenPosition(index);
    auto retY = retPointerPosition.GetY();
    EXPECT_FLOAT_EQ(retY, y);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerScreenPosition_Abnormal, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 3;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[2].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 1;
    auto retPointerPosition = manipulationEventTest.GetPointerScreenPosition(index);
    auto retY = retPointerPosition.GetY();
    EXPECT_FLOAT_EQ(retY, 0.0f);
}

HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerScreenPosition_Abnormal_Parameter, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 3;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    float x = 0.12f;
    float y = 0.21f;
    fingersInfos[2].mMp.Setxy(x, y);
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos,
        highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
        isHighLevelEvent, deviceUdevTags);
    int32_t index = 0xFFFFFFFF;
    auto retPointerPosition = manipulationEventTest.GetPointerScreenPosition(index);
    auto retY = retPointerPosition.GetY();
    EXPECT_FLOAT_EQ(retY, 0.0f);
}


/*
 * Feature: Api_Test_GetPointerId_00
 * Function: Api_Test_GetPointerId_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerId_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerId_00.json",true,manipulationEventTest);
    auto retPointerId = manipulationEventTest.GetPointerId(0);
    EXPECT_EQ(retPointerId, -1);
}


/*
 * Feature: Api_Test_GetPointerId_01
 * Function: Api_Test_GetPointerId_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetPointerId_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetPointerId_01.json",true,manipulationEventTest);
    auto retPointerId = manipulationEventTest.GetPointerId(0);
    EXPECT_EQ(retPointerId, 0);
}


/*
 * Feature: Api_Test_GetForce_00
 * Function: Api_Test_GetForce_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_00.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_01
 * Function: Api_Test_GetForce_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_01.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_02
 * Function: Api_Test_GetForce_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_02, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_02.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_03
 * Function: Api_Test_GetForce_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_03, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_03.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_04
 * Function: Api_Test_GetForce_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_04, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_04.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_05
 * Function: Api_Test_GetForce_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_05, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_05.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_06
 * Function: Api_Test_GetForce_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_06, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_06.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
}


/*
 * Feature: Api_Test_GetForce_07
 * Function: Api_Test_GetForce_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetForce_07, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetForce_07_0.json",true,manipulationEventTest);
    auto retForce = manipulationEventTest.GetForce(1);
    EXPECT_FLOAT_EQ(retForce, 0);
    Initialize("Api_Test_GetForce_07_1.json",false,manipulationEventTest);
}


/*
 * Feature: Api_Test_GetRadius_00
 * Function: Api_Test_GetRadius_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_00, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_00.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_01
 * Function: Api_Test_GetRadius_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_01, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_01.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_02
 * Function: Api_Test_GetRadius_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_02, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_02.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_03
 * Function: Api_Test_GetRadius_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_03, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_03.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_04
 * Function: Api_Test_GetRadius_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_04, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_04.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_05
 * Function: Api_Test_GetRadius_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_05, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_05.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_06
 * Function: Api_Test_GetRadius_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_06, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_06.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
}


/*
 * Feature: Api_Test_GetRadius_07
 * Function: Api_Test_GetRadius_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(ManipulationEventsApiTest, Api_Test_GetRadius_07, TestSize.Level1)
{
    ManipulationEvent manipulationEventTest;
    Initialize("Api_Test_GetRadius_07_0.json",true,manipulationEventTest);
    auto retRadius = manipulationEventTest.GetRadius(0);
    EXPECT_FLOAT_EQ(retRadius, 0.0);
    Initialize("Api_Test_GetRadius_07_1.json",false,manipulationEventTest);
}

} // namespace
