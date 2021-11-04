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
#include "telephone_event_handler.h"
#include "proto.h"
#include "st_helper.h"


namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

class TelephoneEventHandlerApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};


class AppTelephoneEventHandle : public TelephoneEventHandler {
public:
    AppTelephoneEventHandle() {}
    ~AppTelephoneEventHandle() {}

    virtual bool OnAnswer(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnRefuse(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnHangup(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnTelephoneControl(const MultimodalEvent& event) override
    {
        return true;
    }
};


 /*
 * Feature: Api_Test_OnAnswer_00
 * Function: Api_Test_OnAnswer_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(TelephoneEventHandlerApiTest, Api_Test_OnAnswer_00, TestSize.Level1)
{
    AppTelephoneEventHandle telephoneEventHandlerTest;
    const MultimodalEvent event;
    auto retOnAnswer = telephoneEventHandlerTest.OnAnswer(event);
    EXPECT_EQ(retOnAnswer, true);
}


 /*
 * Feature: Api_Test_OnRefuse_00
 * Function: Api_Test_OnRefuse_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(TelephoneEventHandlerApiTest, Api_Test_OnRefuse_00, TestSize.Level1)
{
    AppTelephoneEventHandle telephoneEventHandlerTest;
    const MultimodalEvent event;
    auto retOnRefuse = telephoneEventHandlerTest.OnRefuse(event);
    EXPECT_EQ(retOnRefuse, true);
}


 /*
 * Feature: Api_Test_OOnHangup_00
 * Function: Api_Test_OOnHangup_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(TelephoneEventHandlerApiTest, Api_Test_OOnHangup_00, TestSize.Level1)
{
    AppTelephoneEventHandle telephoneEventHandlerTest;
    const MultimodalEvent event;
    auto retOnHangup = telephoneEventHandlerTest.OnHangup(event);
    EXPECT_EQ(retOnHangup, true);
}


 /*
 * Feature: Api_Test_OnTelephoneControl_00
 * Function: Api_Test_OnTelephoneControl_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(TelephoneEventHandlerApiTest, Api_Test_OnTelephoneControl_00, TestSize.Level1)
{
    AppTelephoneEventHandle telephoneEventHandlerTest;
    const MultimodalEvent event;
    auto retOnTelephoneControl = telephoneEventHandlerTest.OnTelephoneControl(event);
    EXPECT_EQ(retOnTelephoneControl, true);
}
} // namespace
