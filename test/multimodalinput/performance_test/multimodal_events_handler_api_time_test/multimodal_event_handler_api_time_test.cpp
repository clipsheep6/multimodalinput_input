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
#include "api_time.h"
#include "libmmi_util.h"
#include "MultiInputCommon.h"
#include "error_multimodal.h"
#include "multimodal_event_handler.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;

namespace {
class MultimodalEventHandlerApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(MultimodalEventHandlerApiTimeTest, Test_GetAbilityInfoVec, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");
    MultimodalEventHandler multimodalEventHandlerTest;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventHandlerTest.GetAbilityInfoVec();
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventHandlerApiTimeTest, Test_RegisterStandardizedEventHandle, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MultimodalEventHandler multimodalEventHandlerTest;
    sptr<IRemoteObject> token = nullptr;
    int32_t windowId = 1;
    StandEventPtr standardizedEventHandle = nullptr;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventHandlerTest.RegisterStandardizedEventHandle(token, windowId, standardizedEventHandle);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(MultimodalEventHandlerApiTimeTest, Test_UnRegisterStandardizedEventHandle, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    MultimodalEventHandler multimodalEventHandlerTest;
    sptr<IRemoteObject> token = nullptr;
    int32_t windowId = 1;
    StandEventPtr standardizedEventHandle = nullptr;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        multimodalEventHandlerTest.UnregisterStandardizedEventHandle(token, windowId, standardizedEventHandle);
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
