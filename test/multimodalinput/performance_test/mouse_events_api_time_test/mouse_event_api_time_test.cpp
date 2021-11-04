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
#include "mouse_event.h"

using namespace testing::ext;
using namespace OHOS;

namespace {
class MouseEventApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(MouseEventApiTimeTest, Test_GetAction, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetAction();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetActionButton, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetActionButton();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetPressedButtons, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetPressedButtons();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetCursor, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetCursor();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_SetCursorOffset, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    float offsetX = 48.185f;
    float offsetY = 3.545f;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.SetCursorOffset(offsetX, offsetY);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetXOffset, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetXOffset();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetYOffset, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetYOffset();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetAxisValue, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    int32_t axis = AXIS_RX;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetAxisValue(axis);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}



HWTEST_F(MouseEventApiTimeTest, Test_GetCursorDelta, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    int32_t axis = -1;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetCursorDelta(axis);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MouseEventApiTimeTest, Test_GetScrollingDelta_Normal, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MouseEvent mouseEventTest;
    int32_t axis = AXIS_RX;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        mouseEventTest.GetScrollingDelta(axis);
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
