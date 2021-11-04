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
#include "telephone_event_handler.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;

namespace {
class TelephoneEventHandlerApiTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(TelephoneEventHandlerApiTimeTest, Test_OnAnswer, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    AUTOTEST_MULTIINPUTCOMMON->InitClient("Scene_R");
    sleep(1);
    AUTOTEST_MULTIINPUTCOMMON->InJectionEvent("Multi_HandOut_Keyboard_Reg_App_001-B00.json","");    
    TelephoneEventHandler telephoneEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        telephoneEventHandlerTest.OnAnswer(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);;
}

HWTEST_F(TelephoneEventHandlerApiTimeTest, Test_OnRefuse, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    TelephoneEventHandler telephoneEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        telephoneEventHandlerTest.OnRefuse(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(TelephoneEventHandlerApiTimeTest, Test_OOnHangup, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    TelephoneEventHandler telephoneEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        telephoneEventHandlerTest.OnHangup(event);
    }
    items.time_end = items.GetCurTime();
    auto retCallApiTime = items.GetCallApiTime(items.time_start, items.time_end);
    if (retCallApiTime < CALLAPITIME) {
        isTimeOk = true;
    } else {
    }
    EXPECT_TRUE(true == isTimeOk);
}

HWTEST_F(TelephoneEventHandlerApiTimeTest, Test_OnTelephoneControl, TestSize.Level1)
{
    bool isTimeOk = false;
    OHOS::Statistics items;
    TelephoneEventHandler telephoneEventHandlerTest;
    const MultimodalEvent event;
    items.time_start = items.GetCurTime(); 
    for (auto i = 0; i < CALLAPINUM; i++) {
        telephoneEventHandlerTest.OnTelephoneControl(event);
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
