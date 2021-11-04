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
#include "manipulation_event.h"

using namespace testing::ext;
using namespace OHOS;
namespace {
class ManipulationEventsApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetStartTime, TestSize.Level1)
{
    bool isTimeOk = false;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    OHOS::Statistics items;
    OHOS::ManipulationEvent manipulationEventTest;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetStartTime();;
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetPhase, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetPhase();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetPointerPosition, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    int32_t index = 1;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetPointerPosition(index);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_SetScreenOffset, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    float offsetX = 1.1f;
    float offsetY = 1.1f;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.SetScreenOffset(offsetX, offsetY);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_SetScreenOffset_GetFingersInfos, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetFingersInfos();;
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetPointerCount, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetPointerCount();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetPointerScreenPosition, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    int32_t index = 2;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetPointerScreenPosition(index);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}
HWTEST_F(ManipulationEventsApiTimeTest, Test_GetPointerId, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    int32_t index = 2;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetPointerId(index);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(ManipulationEventsApiTimeTest, Test_GetForce, TestSize.Level1)
{   
    bool isTimeOk = false;
    int32_t index = 2;
    OHOS::ManipulationEvent manipulationEventTest;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetForce(index);;
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(ManipulationEventsApiTimeTest, Test_GetRadius, TestSize.Level1)
{   
    bool isTimeOk = false;
    OHOS::ManipulationEvent manipulationEventTest;
    int32_t index = 2;
    OHOS::Statistics items;
    items.time_start = items.GetCurTime();
    for (auto i = 0; i < CALLAPINUM; i++) {
        manipulationEventTest.GetRadius(index);
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
