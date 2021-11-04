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
#include "keyboard_event.h"

using namespace testing::ext;

namespace {
class KeyBoardEventsApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};
HWTEST_F(KeyBoardEventsApiTimeTest, Test_EnableIme, TestSize.Level1)
{
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.EnableIme();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_DisableIme, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.DisableIme();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_IsHandledByIme, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.IsHandledByIme();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_IsNoncharacterKeyPressed_One, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    int32_t keycodeOne = 0;
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.IsNoncharacterKeyPressed(keycodeOne);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_IsNoncharacterKeyPressed_Two, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    int32_t keycodeOne = 0;
    int32_t keycodeTwo = 0;
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.IsNoncharacterKeyPressed(keycodeOne, keycodeTwo);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_IsNoncharacterKeyPressed_Three, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    int32_t keycodeOne = 0;
    int32_t keycodeTwo = 0;
    int32_t keycodeThree = 0;
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.IsNoncharacterKeyPressed(keycodeOne, keycodeTwo, keycodeThree);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(KeyBoardEventsApiTimeTest, Test_GetUnicode, TestSize.Level1)
{
    OHOS::Statistics items;
    OHOS::KeyBoardEvent keyBoardEventTest;
    bool isTimeOk = false;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        keyBoardEventTest.GetUnicode();
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
