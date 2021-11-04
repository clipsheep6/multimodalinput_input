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
#include "common_event_handler.h"
#include "st_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

class CommonEventHandlerApiTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};


class AppCommonEventHandle : public CommonEventHandler {
public:
    AppCommonEventHandle() {}
    ~AppCommonEventHandle() {}

    virtual bool OnShowMenu(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnSend(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnCopy(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnPaste(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnCut(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnUndo(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnRefresh(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnStartDrag(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnCancel(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnEnter(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnPrevious(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnNext(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnBack(const MultimodalEvent& event) override
    {
        return true;
    }

    virtual bool OnPrint(const MultimodalEvent& event) override
    {
        return true;
    }
};


 /*
 * Feature: Api_Test_OnShowMenu_00
 * Function: Api_Test_OnShowMenu_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnShowMenu_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retShowMenu = commonEventHandlerTest.OnShowMenu(event);
    EXPECT_EQ(retShowMenu, true);
}


 /*
 * Feature: Api_Test_OnSend_00
 * Function: Api_Test_OnSend_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnSend_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnSend = commonEventHandlerTest.OnSend(event);
    EXPECT_EQ(retOnSend, true);
}


 /*
 * Feature: Api_Test_OnCopy_00
 * Function: Api_Test_OnCopy_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnCopy_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnCopy = commonEventHandlerTest.OnCopy(event);
    EXPECT_EQ(retOnCopy, true);
}


 /*
 * Feature: Api_Test_OnPaste_00
 * Function: Api_Test_OnPaste_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnPaste_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnPaste = commonEventHandlerTest.OnPaste(event);
    EXPECT_EQ(retOnPaste, true);
}


 /*
 * Feature: Api_Test_OnCut_00
 * Function: Api_Test_OnCut_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnCut_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnCut = commonEventHandlerTest.OnCut(event);
    EXPECT_EQ(retOnCut, true);
}


 /*
 * Feature: Api_Test_OnUndo_00
 * Function: Api_Test_OnUndo_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnUndo_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnUndo = commonEventHandlerTest.OnUndo(event);
    EXPECT_EQ(retOnUndo, true);
}


 /*
 * Feature: Api_Test_OnRefresh_00
 * Function: Api_Test_OnRefresh_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnRefresh_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnRefresh = commonEventHandlerTest.OnRefresh(event);
    EXPECT_EQ(retOnRefresh, true);
}


 /*
 * Feature: Api_Test_OnStartDrag_00
 * Function: Api_Test_OnStartDrag_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnStartDrag_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnStartDrag = commonEventHandlerTest.OnStartDrag(event);
    EXPECT_EQ(retOnStartDrag, true);
}


 /*
 * Feature: Api_Test_OnCancel_00
 * Function: Api_Test_OnCancel_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnCancel_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnCancel = commonEventHandlerTest.OnCancel(event);
    EXPECT_EQ(retOnCancel, true);
}


 /*
 * Feature: Api_Test_OnEnter_00
 * Function: Api_Test_OnEnter_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnEnter_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnEnter = commonEventHandlerTest.OnEnter(event);
    EXPECT_EQ(retOnEnter, true);
}


 /*
 * Feature: Api_Test_OnPrevious_00
 * Function: Api_Test_OnPrevious_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnPrevious_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnPrevious = commonEventHandlerTest.OnPrevious(event);
    EXPECT_EQ(retOnPrevious, true);
}


 /*
 * Feature: Api_Test_OnNext_00
 * Function: Api_Test_OnNext_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnNext_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnNext = commonEventHandlerTest.OnNext(event);
    EXPECT_EQ(retOnNext, true);
}


 /*
 * Feature: Api_Test_OnBack_00
 * Function: Api_Test_OnBack_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnBack_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnBack = commonEventHandlerTest.OnBack(event);
    EXPECT_EQ(retOnBack, true);
}


 /*
 * Feature: Api_Test_OnPrint_00
 * Function: Api_Test_OnPrint_00
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 * 
 */
HWTEST_F(CommonEventHandlerApiTest, Api_Test_OnPrint_00, TestSize.Level1)
{
    AppCommonEventHandle commonEventHandlerTest;
    const MultimodalEvent event;
    auto retOnPrint = commonEventHandlerTest.OnPrint(event);
    EXPECT_EQ(retOnPrint, true);
}
} // namespace
