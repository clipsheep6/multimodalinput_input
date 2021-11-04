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
#include "common_event_handler.h"
#include "system_event_handler.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

class SystemEventHandlerApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};


class SystemEventHandleApiTest : public SystemEventHandler {
public:
    SystemEventHandleApiTest() {}
    ~SystemEventHandleApiTest() {}

    virtual bool OnScreenShot(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnScreenSplit(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnStartScreenRecord(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnStopScreenRecord(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnGotoDesktop(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnRecent(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnShowNotification(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnLockScreen(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnSearch(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnClosePage(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnLaunchVoiceAssistant(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnMute(const MultimodalEvent& event) override
    {
        return true;
    }
};


 /*
 * Feature: Api_Test_OnScreenShot_00
 * Function: Api_Test_OnScreenShot_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnScreenShot_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnScreenShot = systemEventHandlerTest.OnScreenShot(event);
    EXPECT_EQ(retOnScreenShot, true);
}


 /*
 * Feature: Api_Test_OnScreenSplit_00
 * Function: Api_Test_OnScreenSplit_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnScreenSplit_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnScreenSplit = systemEventHandlerTest.OnScreenSplit(event);
    EXPECT_EQ(retOnScreenSplit, true);
}


 /*
 * Feature: Api_Test_OnStartScreenRecord_00
 * Function: Api_Test_OnStartScreenRecord_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnStartScreenRecord_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnStartScreenRecord = systemEventHandlerTest.OnStartScreenRecord(event);
    EXPECT_EQ(retOnStartScreenRecord, true);
}


 /*
 * Feature: Api_Test_OnStopScreenRecord_00
 * Function: Api_Test_OnStopScreenRecord_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnStopScreenRecord_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnStopScreenRecord = systemEventHandlerTest.OnStopScreenRecord(event);
    EXPECT_EQ(retOnStopScreenRecord, true);
}


 /*
 * Feature: Api_Test_OnGotoDesktop_00
 * Function: Api_Test_OnGotoDesktop_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnGotoDesktop_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnGotoDesktop = systemEventHandlerTest.OnGotoDesktop(event);
    EXPECT_EQ(retOnGotoDesktop, true);
}


 /*
 * Feature: Api_Test_OnRecent_00
 * Function: Api_Test_OnRecent_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnRecent_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnRecent = systemEventHandlerTest.OnRecent(event);
    EXPECT_EQ(retOnRecent, true);
}


 /*
 * Feature: Api_Test_OnShowNotification_00
 * Function: Api_Test_OnShowNotification_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */

HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnShowNotification_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnShowNotification = systemEventHandlerTest.OnShowNotification(event);
    EXPECT_EQ(retOnShowNotification, true);
}


 /*
 * Feature: Api_Test_OnLockScreen_00
 * Function: Api_Test_OnLockScreen_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnLockScreen_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnLockScreen = systemEventHandlerTest.OnLockScreen(event);
    EXPECT_EQ(retOnLockScreen, true);
}


 /*
 * Feature: Api_Test_OnSearch_00
 * Function: Api_Test_OnSearch_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnSearch_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnSearch = systemEventHandlerTest.OnSearch(event);
    EXPECT_EQ(retOnSearch, true);
}


 /*
 * Feature: Api_Test_OnClosePage_00
 * Function: Api_Test_OnClosePage_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnClosePage_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnClosePage = systemEventHandlerTest.OnClosePage(event);
    EXPECT_EQ(retOnClosePage, true);
}


 /*
 * Feature: Api_Test_OnLaunchVoiceAssistant_00
 * Function: Api_Test_OnLaunchVoiceAssistant_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnLaunchVoiceAssistant_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnLaunchVoiceAssistant = systemEventHandlerTest.OnLaunchVoiceAssistant(event);
    EXPECT_EQ(retOnLaunchVoiceAssistant, true);
}


 /*
 * Feature: Api_Test_OnMute_00
 * Function: Api_Test_OnMute_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(SystemEventHandlerApiTest, Api_Test_OnMute_00, TestSize.Level1)
{
    AppCommonEventHandle systemEventHandlerTest;
    const MultimodalEvent event;
    auto retOnMute = systemEventHandlerTest.OnMute(event);
    EXPECT_EQ(retOnMute, true);
}
} // namespace
