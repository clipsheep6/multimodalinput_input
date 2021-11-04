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
#include "multimodal_event.h"
#include "struct_multimodal.h"

using namespace testing::ext;
using namespace OHOS;

namespace {
class MultimodalEventsApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(MultimodalEventsApiTimeTest, Test_IsSameEvent, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    MultimodalEvent multimodalEventTest;
    const std::string id = "bEvent";
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.IsSameEvent(id);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_IsHighLevelInput, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MultimodalEvent multimodalEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.IsHighLevelInput();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetHighLevelEvent, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetHighLevelEvent();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetSourceDevice, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetSourceDevice();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetDeviceId, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetDeviceId();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetInputDeviceId, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetInputDeviceId();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetOccurredTime, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetOccurredTime();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetEventType, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetEventType();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetWindowID, TestSize.Level1)
{
    MultimodalEvent multimodalEventTest;
    bool isTimeOk = false;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventTest.GetWindowID();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventsApiTimeTest, Test_GetUuid, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MultimodalEvent multimodalEventTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
            auto retUuid = multimodalEventTest.GetUuid();
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
