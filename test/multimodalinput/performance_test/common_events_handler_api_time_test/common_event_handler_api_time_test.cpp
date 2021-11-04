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
#include "common_event_handler.h"

using namespace testing::ext;
namespace {
class CommonEventHandlerApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnShowMenu, TestSize.Level1)
{
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnShowMenu(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnSend, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnSend(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnCopy, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;

    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnCopy(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnPaste, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;

    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnPaste(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnCut, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;

    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnCut(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnUndo, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnUndo(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnRefresh, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnRefresh(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnStartDrag, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnStartDrag(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnCancel, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnCancel(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnEnter, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnEnter(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnPrevious, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnPrevious(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnNext, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnNext(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnBack, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnBack(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(CommonEventHandlerApiTimeTest, Test_OnPrint, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::MMI::CommonEventHandler commonEventHandlerTest;
    bool isTimeOk = false;
    const OHOS::MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        commonEventHandlerTest.OnPrint(event);
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
