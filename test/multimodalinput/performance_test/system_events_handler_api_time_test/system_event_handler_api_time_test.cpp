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

#include "api_time.h"
#include "libmmi_util.h"
#include "MultiInputCommon.h"
#include <gtest/gtest.h>
#include "system_event_handler.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;

namespace {
class SystemEventHandlerApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnScreenShot, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnScreenShot(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnScreenSplit, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnScreenSplit(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnStartScreenRecord, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnStartScreenRecord(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnStopScreenRecord, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnStopScreenRecord(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnGotoDesktop, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnGotoDesktop(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnRecent, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnRecent(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnShowNotification, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnShowNotification(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnLockScreen, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnLockScreen(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnSearch, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnSearch(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnClosePage, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnClosePage(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnLaunchVoiceAssistant, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnLaunchVoiceAssistant(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(SystemEventHandlerApiTimeTest, Test_OnMute, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    SystemEventHandler systemEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        systemEventHandlerTest.OnMute(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
} // namespace
