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
#include "keyboard_event.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyBoardEventsApiTest" };

class KeyBoardEventsApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string& eventValue, const bool ansert, KeyBoardEvent &keyBoardEventTest);
    std::string ToStrings(const std::string& px, const std::string& py, const std::string& pz) const;
    
private:
    static StClientHelper st_client_helper_;
};

StClientHelper KeyBoardEventsApiTest::st_client_helper_;

void KeyBoardEventsApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void KeyBoardEventsApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void KeyBoardEventsApiTest::SetUp()
{
}

void KeyBoardEventsApiTest::TearDown()
{
}

void KeyEventApiTest::Initialize(const std::string& eventValue, const std::string& deviceMsg, const bool ansert, KeyBoardEvent &keyBoardEventTest)
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
 * Feature: Api_Test_IsHandledByIme_00
 * Function: Api_Test_IsHandledByIme_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_00, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_00.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
}


/*
 * Feature: Api_Test_IsHandledByIme_01
 * Function: Api_Test_IsHandledByIme_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_01_0.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
    Initialize("Api_Test_IsHandledByIme_01_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsHandledByIme_02
 * Function: Api_Test_IsHandledByIme_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_02.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
}


/*
 * Feature: Api_Test_IsHandledByIme_03
 * Function: Api_Test_IsHandledByIme_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_03, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_03.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
}


/*
 * Feature: Api_Test_IsHandledByIme_04
 * Function: Api_Test_IsHandledByIme_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_04, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_04_0.json",true,keyBoardEventTest)
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
    Initialize("Api_Test_IsHandledByIme_04_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsHandledByIme_05
 * Function: Api_Test_IsHandledByIme_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_05, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_05_0.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
    Initialize("Api_Test_IsHandledByIme_05_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsHandledByIme_06
 * Function: Api_Test_IsHandledByIme_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_06, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_06_0.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
    Initialize("Api_Test_IsHandledByIme_06_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsHandledByIme_07
 * Function: Api_Test_IsHandledByIme_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsHandledByIme_07, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsHandledByIme_07_0.json",true,keyBoardEventTest);
    auto rethandledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(rethandledByIme == 0);
    Initialize("Api_Test_IsHandledByIme_07_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_DisableIme_00
 * Function: Api_Test_DisableIme_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_00, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_DisableIme_00_0.json",true,keyBoardEventTest);
    keyBoardEventTest.DisableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == false);
}


/*
* Feature: Api_Test_DisableIme_01
* Function: Api_Test_DisableIme_01
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
json
*/
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_DisableIme_01_0.json",true,keyBoardEventTest);
    keyBoardEventTest.DisableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == false);
     Initialize("Api_Test_DisableIme_01_1.json",false,keyBoardEventTest);
}

/*
 * Feature: Api_Test_DisableIme_02
 * Function: Api_Test_DisableIme_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_DisableIme_02.json",true,keyBoardEventTest);
    keyBoardEventTest.DisableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == false);
}


/*
 * Feature: Api_Test_DisableIme_03
 * Function: Api_Test_DisableIme_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_03, TestSize.Level1)
{
    Initialize("Api_Test_DisableIme_03.json",true,keyBoardEventTest);
    KeyBoardEvent keyBoardEventTest;
    keyBoardEventTest.DisableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == false);
}


/*
 * Feature: Api_Test_DisableIme_04
 * Function: Api_Test_DisableIme_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_04, TestSize.Level1)
{
    Initialize("Api_Test_DisableIme_04_0.json",true,keyBoardEventTest);
    KeyBoardEvent keyBoardEventTest;
    keyBoardEventTest.DisableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == false);
    Initialize("Api_Test_DisableIme_04_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_DisableIme_05
 * Function: Api_Test_DisableIme_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_05, TestSize.Level1)
{
    Initialize("Api_Test_DisableIme_05_0.json",true,keyBoardEventTest);
    KeyBoardEvent keyBoardEventTest;
    keyBoardEventTest.EnableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == true);
    Initialize("Api_Test_DisableIme_05_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_DisableIme_06
 * Function: Api_Test_DisableIme_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_06, TestSize.Level1)
{
    Initialize("Api_Test_DisableIme_06_0.json",true,keyBoardEventTest);
    KeyBoardEvent keyBoardEventTest;
    keyBoardEventTest.EnableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == true);
    Initialize("Api_Test_DisableIme_06_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_DisableIme_07
 * Function: Api_Test_DisableIme_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_DisableIme_07, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_DisableIme_07_0.json",true,keyBoardEventTest);
    keyBoardEventTest.EnableIme();
    auto handledByIme = keyBoardEventTest.IsHandledByIme();
    EXPECT_TRUE(handledByIme == true);
    Initialize("Api_Test_DisableIme_07_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_One_00
 * Function: Api_Test_IsNoncharacterKeyPressed_One_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */


HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_One_00, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_00_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2072);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_00_1.json",false,keyBoardEventTest);
}

/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_One_01
 * Function: Api_Test_IsNoncharacterKeyPressed_One_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_One_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_01_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2073);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_01_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_One_02
 * Function: Api_Test_IsNoncharacterKeyPressed_One_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_One_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_02_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2044);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_02_1.json",false,keyBoardEventTest);
}

/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_One_03
 * Function: Api_Test_IsNoncharacterKeyPressed_One_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_One_03, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_03_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2045);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_One_03_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Two_01
 * Function: Api_Test_IsNoncharacterKeyPressed_Two_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Two_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_01_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2045, 2017);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_01_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Two_02
 * Function: Api_Test_IsNoncharacterKeyPressed_Two_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Two_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_02_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2045, 2019);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_02_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Two_03
 * Function: Api_Test_IsNoncharacterKeyPressed_Two_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Two_03, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_03_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2045, 2072);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_03_1.json",false,keyBoardEventTest);
}

/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Two_04
 * Function: Api_Test_IsNoncharacterKeyPressed_Two_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Two_04, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_04_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2045, 2073);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Two_04_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Three_00
 * Function: Api_Test_IsNoncharacterKeyPressed_Three_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Three_00, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_00_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2072, 2045, 2047);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_00_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Three_01
 * Function: Api_Test_IsNoncharacterKeyPressed_Three_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Three_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_01_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2073, 2046, 2048);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_01_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Three_02
 * Function: Api_Test_IsNoncharacterKeyPressed_Three_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Three_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_02_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2072, 2045, 2017);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_02_1.json",false,keyBoardEventTest);
}


/*
 * Feature: Api_Test_IsNoncharacterKeyPressed_Three_03
 * Function: Api_Test_IsNoncharacterKeyPressed_Three_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_IsNoncharacterKeyPressed_Three_03, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_03_0.json",true,keyBoardEventTest);
    auto noncharacterKeyPressed = keyBoardEventTest.IsNoncharacterKeyPressed(2073, 2046, 2017);
    EXPECT_TRUE(noncharacterKeyPressed == true);
    Initialize("Api_Test_IsNoncharacterKeyPressed_Three_03_1.json",false,keyBoardEventTest);
}

/*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */


HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_00, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_00_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 97);
    Initialize("Api_Test_GetUnicode_00_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_01, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_01_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 100);
    Initialize("Api_Test_GetUnicode_01_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_02, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_02_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 30);
    Initialize("Api_Test_GetUnicode_02_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_03, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_03_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 29);
    Initialize("Api_Test_GetUnicode_03_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_04, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_04_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 56);
    Initialize("Api_Test_GetUnicode_05_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_05, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_05_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 11);
    Initialize("Api_Test_GetUnicode_05_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_06, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_06_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 10);
    Initialize("Api_Test_GetUnicode_06_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_07, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_07_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 522);
    Initialize("Api_Test_GetUnicode_07_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_08, TestSize.Level1)
{

    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_08_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 106);
    Initialize("Api_Test_GetUnicode_08_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_09, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_09_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 353);
    Initialize("Api_Test_GetUnicode_09_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_10, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_10_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 29);
    Initialize("Api_Test_GetUnicode_10_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_11, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_11_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 111);
    Initialize("Api_Test_GetUnicode_11_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_12, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_12_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 1);
    Initialize("Api_Test_GetUnicode_12_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_13, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_13_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 109);
    Initialize("Api_Test_GetUnicode_13_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_14, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_14_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 104);
    Initialize("Api_Test_GetUnicode_14_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_15, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_15_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 139);
    Initialize("Api_Test_GetUnicode_15_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_16, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_16_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 21);
    Initialize("Api_Test_GetUnicode_16_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_17, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_17_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 44);
    Initialize("Api_Test_GetUnicode_17_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_00
 * Function: Api_Test_GetUnicode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_18, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_18_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 51);
    Initialize("Api_Test_GetUnicode_18_1.json",false,keyBoardEventTest);
}


 /*
 * Feature: Api_Test_GetUnicode_19
 * Function: Api_Test_GetUnicode_19
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyBoardEventsApiTest, Api_Test_GetUnicode_19, TestSize.Level1)
{
    KeyBoardEvent keyBoardEventTest;
    Initialize("Api_Test_GetUnicode_19_0.json",true,keyBoardEventTest);
    auto retUnicode = keyBoardEventTest.GetUnicode();
    EXPECT_EQ(retUnicode, 52);
    Initialize("Api_Test_GetUnicode_19_1.json",false,keyBoardEventTest);
}
} // namespace
