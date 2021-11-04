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
#include "media_event_handler.h"
#include "proto.h"
#include "st_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

class MediaEventHandlerApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};


class AppMediaEventHandle : public MediaEventHandler {
public:
    AppMediaEventHandle() {}
    ~AppMediaEventHandle() {}

    virtual bool OnPlay(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnPause(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnMediaControl(const MultimodalEvent& event) override
    {
        return true;
    }
};


 /*
 * Feature: Api_Test_OnPlay_00
 * Function: Api_Test_OnPlay_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MediaEventHandlerApiTest, Api_Test_OnPlay_00, TestSize.Level1)
{
    AppMediaEventHandle mediaEventHandlerTest;
    const MultimodalEvent event;
    auto retOnPlay = mediaEventHandlerTest.OnPlay(event);
    EXPECT_EQ(retOnPlay, true);
}


 /*
 * Feature: Api_Test_OnPause_00
 * Function: Api_Test_OnPause_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MediaEventHandlerApiTest, Api_Test_OnPause_00, TestSize.Level1)
{
    AppMediaEventHandle mediaEventHandlerTest;
    const MultimodalEvent event;
    auto retOnPause = mediaEventHandlerTest.OnPause(event);
    EXPECT_EQ(retOnPause, true);
}


 /*
 * Feature: Api_Test_OnMediaControl_00
 * Function: Api_Test_OnMediaControl_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * Description: NA
 */
HWTEST_F(MediaEventHandlerApiTest, Api_Test_OnMediaControl_00, TestSize.Level1)
{
    AppMediaEventHandle mediaEventHandlerTest;
    const MultimodalEvent event;
    auto retOnMediaControl = mediaEventHandlerTest.OnMediaControl(event);
    EXPECT_EQ(retOnMediaControl, true);
}
} // namespace
