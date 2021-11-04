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
#include "multimodal_event.h"
#include "struct_multimodal.h"
#include "st_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalEventsApiTest" };


class MultimodalEventsApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string &json, const bool ansert, MultimodalEvent &multimodalEventTest);
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

void MultimodalEventsApiTest::Initialize(const std::string &json, const bool ansert, MultimodalEvent &multimodalEventTest)
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
* Feature: Api_Test_IsSameEvent_00
* Function: Api_Test_IsSameEvent_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_IsSameEvent_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
}

/*
* Feature: Api_Test_IsSameEvent_01
* Function: Api_Test_IsSameEvent_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsSameEvent_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_01_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_IsSameEvent_02
* Function: Api_Test_IsSameEvent_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsSameEvent_02, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_02_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_IsSameEvent_03
* Function: Api_Test_IsSameEvent_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsSameEvent_03, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_IsSameEvent_04
* Function: Api_Test_IsSameEvent_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsSameEvent_04, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}


/*
* Feature: Api_Test_IsSameEvent_05
* Function: Api_Test_IsSameEvent_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsSameEvent_05, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto retUuid = multimodalEventTest.GetUuid();
    auto isTrue = multimodalEventTest.IsSameEvent(retUuid);
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_IsHighLevelInput_00
* Function: Api_Test_IsHighLevelInput_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_IsHighLevelInput_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,manipulationEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == true);
}

/*
* Feature: Api_Test_IsHighLevelInput_01
* Function: Api_Test_IsHighLevelInput_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsHighLevelInput_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0",true,manipulationEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == true);
    Initialize("Api_Test_Common_01_1",false,manipulationEventTest);
}

/*
* Feature: Api_Test_IsHighLevelInput_02
* Function: Api_Test_IsHighLevelInput_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsHighLevelInput_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0",true,manipulationEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == false);
    Initialize("Api_Test_Common_02_1",true,manipulationEventTest);
}

 /*
 * Feature: Api_Test_IsHighLevelInput_03
 * Function: Api_Test_IsHighLevelInput_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsHighLevelInput_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == false);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_IsHighLevelInput_04
* Function: Api_Test_IsHighLevelInput_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsHighLevelInput_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == false);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_IsHighLevelInput_05
* Function: Api_Test_IsHighLevelInput_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsHighLevelInput_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto isTrue = multimodalEventTest.IsHighLevelInput();
    EXPECT_TRUE(isTrue == false);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetSourceDevice_00
* Function: Api_Test_GetSourceDevice_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetSourceDevice_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1",true,manipulationEventTest);
}

/*
* Feature: Api_Test_GetSourceDevice_01
* Function: Api_Test_GetSourceDevice_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetSourceDevice_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1",false,manipulationEventTest);
}

/*
* Feature: Api_Test_GetSourceDevice_02
* Function: Api_Test_GetSourceDevice_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetSourceDevice_02, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_02_1",false,manipulationEventTest);
}

/*
* Feature: Api_Test_GetSourceDevice_03
* Function: Api_Test_GetSourceDevice_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetSourceDevice_03, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetSourceDevice_04
* Function: Api_Test_GetSourceDevice_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetSourceDevice_04, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0 );
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetSourceDevice_05
* Function: Api_Test_GetSourceDevice_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetSourceDevice_05, TestSize.Level1)
{
    KeyEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetSourceDevice();
    EXPECT_TRUE(ret != 0 );
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetDeviceId_00
* Function: Api_Test_GetDeviceId_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetDeviceId_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
}

/*
* Feature: Api_Test_GetDeviceId_01
* Function: Api_Test_GetDeviceId_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetDeviceId_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
    Initialize("Api_Test_Common_01_1",false,manipulationEventTest);
}

/*
* Feature: Api_Test_GetDeviceId_02
* Function: Api_Test_GetDeviceId_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetDeviceId_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0",true,manipulationEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
    Initialize("Api_Test_Common_02_1",false,manipulationEventTest);
}

/*
* Feature: Api_Test_GetDeviceId_03
* Function: Api_Test_GetDeviceId_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetDeviceId_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetDeviceId_04
* Function: Api_Test_GetDeviceId_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetDeviceId_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetDeviceId_05
* Function: Api_Test_GetDeviceId_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetDeviceId_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetDeviceId();
    EXPECT_TRUE(ret != "");
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetInputDeviceId_00
* Function: Api_Test_GetInputDeviceId_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetInputDeviceId_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
}

/*
* Feature: Api_Test_GetInputDeviceId_01
* Function: Api_Test_GetInputDeviceId_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetInputDeviceId_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetInputDeviceId_02
* Function: Api_Test_GetInputDeviceId_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetInputDeviceId_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_02_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetInputDeviceId_03
* Function: Api_Test_GetInputDeviceId_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetInputDeviceId_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetInputDeviceId_04
* Function: Api_Test_GetInputDeviceId_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetInputDeviceId_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetInputDeviceId_05
* Function: Api_Test_GetInputDeviceId_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetInputDeviceId_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetInputDeviceId();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetOccurredTime_00
* Function: Api_Test_GetOccurredTime_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetOccurredTime_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
}

/*
* Feature: Api_Test_GetOccurredTime_01
* Function: Api_Test_GetOccurredTime_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetOccurredTime_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetOccurredTime_02
* Function: Api_Test_GetOccurredTime_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetOccurredTime_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_02_1.json",false,multimodalEventTest);
}

 /*
 * Feature: Api_Test_GetOccurredTime_03
 * Function: Api_Test_GetOccurredTime_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetOccurredTime_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetOccurredTime_04
* Function: Api_Test_GetOccurredTime_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetOccurredTime_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetOccurredTime_05
* Function: Api_Test_GetOccurredTime_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetOccurredTime_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetOccurredTime();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetWindowID_00
* Function: Api_Test_GetWindowID_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetWindowID_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
}

/*
* Feature: Api_Test_GetWindowID_01
* Function: Api_Test_GetWindowID_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetWindowID_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetWindowID_02
* Function: Api_Test_GetWindowID_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetWindowID_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_02_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetWindowID_03
* Function: Api_Test_GetWindowID_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetWindowID_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetWindowID_04
* Function: Api_Test_GetWindowID_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetWindowID_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetWindowID_05
* Function: Api_Test_GetWindowID_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetWindowID_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetWindowID();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetUuid_00
* Function: Api_Test_GetUuid_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(MultimodalEventsApiTest, Api_Test_GetUuid_00, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_00.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
}

/*
* Feature: Api_Test_GetUuid_01
* Function: Api_Test_GetUuid_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetUuid_01, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_01_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_01_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetUuid_02
* Function: Api_Test_GetUuid_02
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetUuid_02, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_02_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_02_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetUuid_03
* Function: Api_Test_GetUuid_03
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetUuid_03, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_03_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_03_1.json",false,multimodalEventTest);
}

/*
* Feature: Api_Test_GetUuid_04
* Function: Api_Test_GetUuid_04
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA  
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetUuid_04, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_04_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_04_1.json",false,multimodalEventTest);

}

/*
* Feature: Api_Test_GetUuid_05
* Function: Api_Test_GetUuid_05
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA 
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetUuid_05, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    Initialize("Api_Test_Common_05_0.json",true,multimodalEventTest);
    auto ret = multimodalEventTest.GetUuid();
    EXPECT_TRUE(ret != 0);
    Initialize("Api_Test_Common_05_1.json",false,multimodalEventTest);
}
}
