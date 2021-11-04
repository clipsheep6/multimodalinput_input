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
#include "mmi_token.h"
#include "mmi_client.h"
#include "multimodal_event.h"
#include "multimodal_event_handler.h"
#include "key_event_handler.h"
#include "key_event.h"
#include "st_helper.h"
#include "st_data_process.h"
#include "st_client_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyEventApiTest" };

class KeyEventApiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void Initialize(const std::string& eventValue, const std::string& deviceMsg, const bool ansert, KeyEvent &keyEventTest);
    std::string ToStrings(const std::string& px, const std::string& py, const std::string& pz) const;
    
private:
    static StClientHelper st_client_helper_;
};

StClientHelper KeyEventApiTest::st_client_helper_;

void KeyEventApiTest::SetUpTestCase()
{
    st_client_helper_.StartStClient();
    sleep(2);
}

void KeyEventApiTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void KeyEventApiTest::SetUp()
{
}

void KeyEventApiTest::TearDown()
{
}

void KeyEventApiTest::Initialize(const std::string& eventValue, const std::string& deviceMsg, const bool ansert, KeyEvent &keyEventTest)
{
    sleep(2);
    MMI_LOGI("******************zhuru******************* \n");
    GetSTHelper().InJectionEvent(eventValue, deviceMsg);
    auto KeyEventInitData = GetDataProcess().GetOnKeyData();
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
 * Feature: Api_Test_GetOriginEventType_00
 * Function: Api_Test_GetOriginEventType_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_00, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_00.json","",false,keyEventTest);
    auto originEventType = keyEventTest.GetOriginEventType();
    EXPECT_EQ(originEventType, 0);

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
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_01, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_01_0.json","",true,keyEventTest);
    MMI_LOGI("Injected Sucess!");
    auto originEventType = keyEventTest.GetOriginEventType();
    MMI_LOGI("Get Api Sucess!");
    Initialize("Api_Test_GetOriginEventType_01_1.json","",false,keyEventTest);
    EXPECT_EQ(originEventType, 2);
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
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_02, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_02_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetOriginEventType();
    EXPECT_EQ(originEventType, 4);
    Initialize("Api_Test_GetOriginEventType_02_1.json","",false,keyEventTest);
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
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_03, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_03_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetOriginEventType();
    EXPECT_EQ(originEventType, 8);
    Initialize("Api_Test_GetOriginEventType_03_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetOriginEventType_04
 * Function: Api_Test_GetOriginEventType_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_04, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_04_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetOriginEventType();
    EXPECT_EQ(originEventType, 9);
    Initialize("Api_Test_GetOriginEventType_04_1.json","",false,keyEventTest);

}


 /*
 * Feature: Api_Test_GetOriginEventType_05
 * Function: Api_Test_GetOriginEventType_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA 
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetOriginEventType_05, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetOriginEventType_05_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetOriginEventType();
    EXPECT_EQ(originEventType, 10);
    Initialize("Api_Test_GetOriginEventType_05_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetOriginEventType_06
 * Function: Api_Test_GetOriginEventType_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_00, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_00_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_00_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_01
 * Function: Api_Test_GetMaxKeyCode_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_01, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_01.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    GetDataProcess().ClearAll();
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_02
 * Function: Api_Test_GetMaxKeyCode_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_02, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_02_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_02_1.json","",false,keyEventTest);;
    GetDataProcess().ClearAll();
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_03
 * Function: Api_Test_GetMaxKeyCode_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_03, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_03_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_03_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_04
 * Function: Api_Test_GetMaxKeyCode_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_04, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_04_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_04_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_05
 * Function: Api_Test_GetMaxKeyCode_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_05, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_05_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_05_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_06
 * Function: Api_Test_GetMaxKeyCode_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_06, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_06_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_06_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_07
 * Function: Api_Test_GetMaxKeyCode_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_07, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_07_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_07_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetMaxKeyCode_08
 * Function: Api_Test_GetMaxKeyCode_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_08, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetMaxKeyCode_08_0.json","",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
    Initialize("Api_Test_GetMaxKeyCode_08_1.json","",false,keyEventTest);
}


/*
* Feature: Api_Test_GetMaxKeyCode_09
* Function: Api_Test_GetMaxKeyCode_09
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
}; 
*/ 
HWTEST_F(KeyEventApiTest, Api_Test_GetMaxKeyCode_09, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("4001","Voice",true,keyEventTest);
    auto originEventType = keyEventTest.GetMaxKeyCode();
    EXPECT_TRUE(originEventType == 10008);
}

/*
* Feature: Api_Test_IsKeyDown_00
* Function: Api_Test_IsKeyDown_00
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
};  
*/
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_00, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_00_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_00_1.json","",true,keyEventTest);
    auto keyDown1 = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown1, 0);

}


 /*
 * Feature: Api_Test_IsKeyDown_01
 * Function: Api_Test_IsKeyDown_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_01, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_01_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_01_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_02
 * Function: Api_Test_IsKeyDown_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_03, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_03_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_03_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_02
 * Function: Api_Test_IsKeyDown_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_04, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_04_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_04_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_05
 * Function: Api_Test_IsKeyDown_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_05, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_05_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_05_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_06
 * Function: Api_Test_IsKeyDown_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_06, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_06_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_06_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_07
 * Function: Api_Test_IsKeyDown_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
};
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_07, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_07.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 0);
}


 /*
 * Feature: Api_Test_IsKeyDown_08
 * Function: Api_Test_IsKeyDown_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
};  
 */
HWTEST_F(KeyEventApiTest, Api_Test_IsKeyDown_08, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_IsKeyDown_08_0.json","",true,keyEventTest);
    sleep(2);
    auto keyDown = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyDown, 1);
    Initialize("Api_Test_IsKeyDown_08_1.json","",true,keyEventTest);
    sleep(2);
    auto keyUp = keyEventTest.IsKeyDown();
    EXPECT_EQ(keyUp, 0);
}


/*
 * Feature: Api_Test_GetKeyCode_00
 * Function: Api_Test_GetKeyCode_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_00, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_00_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2073);
    Initialize("Api_Test_GetKeyCode_00_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_01
 * Function: Api_Test_GetKeyCode_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_01, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_01_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2046);
    Initialize("Api_Test_GetKeyCode_01_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_02
 * Function: Api_Test_GetKeyCode_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_02, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_02_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2017);
    Initialize("Api_Test_GetKeyCode_02_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_03
 * Function: Api_Test_GetKeyCode_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_03, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_03_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2072);
    Initialize("Api_Test_GetKeyCode_03_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_04
 * Function: Api_Test_GetKeyCode_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_04, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_04_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2045);
    Initialize("Api_Test_GetKeyCode_04_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_05
 * Function: Api_Test_GetKeyCode_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_05, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_05_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2000);
    Initialize("Api_Test_GetKeyCode_05_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_06
 * Function: Api_Test_GetKeyCode_06
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_06, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_06_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2009);
    Initialize("Api_Test_GetKeyCode_06_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_07
 * Function: Api_Test_GetKeyCode_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_07, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_07_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2010);
    Initialize("Api_Test_GetKeyCode_07_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_08
 * Function: Api_Test_GetKeyCode_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_08, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_08_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2015);
    Initialize("Api_Test_GetKeyCode_08_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_09
 * Function: Api_Test_GetKeyCode_09
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_09, TestSize.Level1)
{
    KeyEvent keyEventTest;
    sleep(2);
    Initialize("Api_Test_GetKeyCode_09_0.json","",true,keyEventTest);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2016);
    Initialize("Api_Test_GetKeyCode_09_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_10
 * Function: Api_Test_GetKeyCode_10
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_10, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_10_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2072);
    Initialize("Api_Test_GetKeyCode_10_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_11
 * Function: Api_Test_GetKeyCode_11
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_11, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_11_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2071);
    Initialize("Api_Test_GetKeyCode_11_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_12
 * Function: Api_Test_GetKeyCode_12
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_12, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_12_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2070);
    Initialize("Api_Test_GetKeyCode_12_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_13
 * Function: Api_Test_GetKeyCode_13
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_13, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_13_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2069);
    Initialize("Api_Test_GetKeyCode_13_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_14
 * Function: Api_Test_GetKeyCode_14
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_14, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_14_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2068);
    Initialize("Api_Test_GetKeyCode_14_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_15
 * Function: Api_Test_GetKeyCode_15
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_15, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_15_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2067);
    Initialize("Api_Test_GetKeyCode_15_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_16
 * Function: Api_Test_GetKeyCode_16
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_16, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_16_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2041);
    Initialize("Api_Test_GetKeyCode_16_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_17
 * Function: Api_Test_GetKeyCode_17
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_17, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_17_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2042);
    Initialize("Api_Test_GetKeyCode_17_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_18
 * Function: Api_Test_GetKeyCode_18
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_18, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_18_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2043);
    Initialize("Api_Test_GetKeyCode_18_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyCode_19
 * Function: Api_Test_GetKeyCode_19
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyCode_19, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyCode_19_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyCode = keyEventTest.GetKeyCode();
    EXPECT_EQ(retKeyCode, 2044);
    Initialize("Api_Test_GetKeyCode_19_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_00
 * Function: Api_Test_GetKeyDownDuration_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_00, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_00_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_00_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_01
 * Function: Api_Test_GetKeyDownDuration_01
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_01, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_01_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_01_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_02
 * Function: Api_Test_GetKeyDownDuration_02
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_02, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_02_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_02_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_03
 * Function: Api_Test_GetKeyDownDuration_03
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_03, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_03_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_03_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_04
 * Function: Api_Test_GetKeyDownDuration_04
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_04, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_04_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_04_1.json","",false,keyEventTest);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_05
 * Function: Api_Test_GetKeyDownDuration_05
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_05, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_05_0.json","",true,keyEventTest);
    sleep(2);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_05_1.json","",false,keyEventTest);
}


/*
* Feature: Api_Test_GetKeyDownDuration_06
* Function: Api_Test_GetKeyDownDuration_06
* SubFunction: NA
* FunctionPoints: NA
* EnvConditions: NA
* CaseDescription: set log print level
* Description: NA
*/
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_06, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("4002","Voice",true,keyEventTest);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_07
 * Function: Api_Test_GetKeyDownDuration_07
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_07, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_07.json","",true,keyEventTest);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_08
 * Function: Api_Test_GetKeyDownDuration_08
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_08, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_08.json","",true,keyEventTest);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(3);
    EXPECT_EQ(retKeyDownDuration, 0);
}


 /*
 * Feature: Api_Test_GetKeyDownDuration_09
 * Function: Api_Test_GetKeyDownDuration_09
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 json
 */
HWTEST_F(KeyEventApiTest, Api_Test_GetKeyDownDuration_09, TestSize.Level1)
{
    KeyEvent keyEventTest;
    Initialize("Api_Test_GetKeyDownDuration_09_0.json","",true,keyEventTest);
    auto retKeyDownDuration = keyEventTest.GetKeyDownDuration();
    sleep(10);
    EXPECT_EQ(retKeyDownDuration, 0);
    Initialize("Api_Test_GetKeyDownDuration_09_1.json","",false,keyEventTest);
}
} // namespace
