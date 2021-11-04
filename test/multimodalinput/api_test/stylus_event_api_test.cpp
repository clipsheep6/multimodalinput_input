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
#include "stylus_event.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "StylusEventApiTest" };

class StylusEventApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string &json, const bool ansert, StylusEvent &stylusEventTest);
    void Initialize1(const std::string &json);
private:
    static StClientHelper st_client_helper_;
};

StClientHelper StylusEventApiTest::st_client_helper_;

void StylusEventApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void StylusEventApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void StylusEventApiTest::SetUp()
{
}

void StylusEventApiTest::TearDown()
{
}

void StylusEventApiTest::Initialize(const std::string &json, const bool ansert, StylusEvent &stylusEventTest)
{
    //Initialize("Api_Test_Common_01_0.json",true,mouseEventTest);
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



/*
 * Feature: Api_Test_GetAction_000
 * Function: Api_Test_GetAction_000
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetAction_000, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetAction_000.json",true,stylusEventTest);
    auto retAction = stylusEventTest.GetAction();
    EXPECT_EQ(retAction, 0);
}

/*
 * Feature: Api_Test_GetAction_001
 * Function: Api_Test_GetAction_001
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetAction_001, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetAction_001_0.json",true,stylusEventTest);
    auto retAction = stylusEventTest.GetAction();
    EXPECT_EQ(retAction, 3);
    Initialize("Api_Test_GetAction_001_1.json",false,stylusEventTest);
}


/*
 * Feature: Api_Test_GetAction_002
 * Function: Api_Test_GetAction_002
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetAction_002, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetAction_002.json",true,stylusEventTest);
    auto retAction = stylusEventTest.GetAction();
    EXPECT_EQ(retAction, 1);
}


/*
 * Feature: Api_Test_GetAction_003
 * Function: Api_Test_GetAction_003
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetAction_003, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetAction_003.json",true,stylusEventTest);
    auto retAction = stylusEventTest.GetAction();
    EXPECT_EQ(retAction, 2);
}


/*
 * Feature: Api_Test_GetButtons_00
 * Function: Api_Test_GetButtons_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetButtons_00, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetOriginEventType_00.json",true,stylusEventTest);
    auto retButtons = stylusEventTest.GetButtons();
    EXPECT_EQ(retButtons, 0);
}


/*
 * Feature: Api_Test_GetButtons_01
 * Function: Api_Test_GetButtons_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA331 (BTN_STYLUS)
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetButtons_01, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetButtons_01.json",true,stylusEventTest);
    auto retButtons = stylusEventTest.GetButtons();
    EXPECT_EQ(retButtons, 1);
}


/*
 * Feature: Api_Test_GetButtons_02
 * Function: Api_Test_GetButtons_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA332 (BTN_STYLUS2)
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetButtons_02, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetButtons_02.json",true,stylusEventTest);
    auto retButtons = stylusEventTest.GetButtons();
    EXPECT_EQ(retButtons, 2);
}


/*
 * Feature: Api_Test_GetActionButton_000
 * Function: Api_Test_GetActionButton_000
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA331 (BTN_STYLUS)
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetActionButton_000, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetActionButton_000.json",true,stylusEventTest);
    auto retActionButton = stylusEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 0);
}


/*
 * Feature: Api_Test_GetActionButton_001
 * Function: Api_Test_GetActionButton_001
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA332 (BTN_STYLUS)
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetActionButton_001, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetActionButton_001.json",true,stylusEventTest);
    auto retActionButton = stylusEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 1);
}



/*
 * Feature: Api_Test_GetActionButton_002
 * Function: Api_Test_GetActionButton_002
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA (BTN_STYLUS)
 */
HWTEST_F(StylusEventApiTest, Api_Test_GetActionButton_002, TestSize.Level1)
{
    StylusEvent stylusEventTest;
    Initialize("Api_Test_GetActionButton_002.json",true,stylusEventTest);
    auto retActionButton = stylusEventTest.GetActionButton();
    EXPECT_EQ(retActionButton, 0);
}

} // namespace
