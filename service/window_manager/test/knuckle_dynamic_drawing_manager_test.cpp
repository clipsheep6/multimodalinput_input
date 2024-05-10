/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdio>
#include <fstream>

#include <gtest/gtest.h>

#include "mmi_log.h"
#include "pointer_event.h"
#include "knuckle_dynamic_drawing_manager.h"
#include "window_info.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "KnuckleDynamicDrawingManagerTest"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace
class KnuckleDynamicDrawingManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void)
    {
        DisplayInfo displayInfo = { .id = 1, .x = 1, .y = 1, .width = 1, .height = 1,
            .dpi = 240, .name = "display", .uniq = "xx" };
        if (knuckleDynamicDrawingMgr == nullptr) {
            knuckleDynamicDrawingMgr = std::make_shared<KnuckleDynamicDrawingManager>();
        }
        knuckleDynamicDrawingMgr->UpdateDisplayInfo(displayInfo);
    }
private:
    std::shared_ptr<KnuckleDynamicDrawingManager> knuckleDynamicDrawingMgr { nullptr };
};

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_001
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_001,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    int32_t displayX = 100;
    int32_t displayY = 100;
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetToolType(PointerEvent::TOOL_TYPE_FINGER);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_002
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_002,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    int32_t displayX = 200;
    int32_t displayY = 200;
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_003
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_003,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    int32_t displayX = 100;
    int32_t displayY = 200;
    item1.SetDisplayX(displayX);
    item1.SetDisplayY(displayY);
    item1.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item1);

    PointerEvent::PointerItem item2;
    item2.SetPointerId(1);
    displayX = 200;
    displayY = 200;
    item2.SetDisplayX(displayX);
    item2.SetDisplayY(displayY);
    item2.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->AddPointerItem(item2);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_004
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_004,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    int32_t displayX = 200;
    int32_t displayY = 200;
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_005
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_005,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    int32_t displayX = 200;
    int32_t displayY = 200;
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_006
 * @tc.desc: Test KnuckleDynamicDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_KnuckleDynamicDrawHandler_006,
    TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    int32_t displayX = 200;
    int32_t displayY = 200;
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetTargetDisplayId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->KnuckleDynamicDrawHandler(pointerEvent));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_UpdateDisplayInfo_001
 * @tc.desc: Test UpdateDisplayInfo
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_UpdateDisplayInfo_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayInfo displayInfo = { .id = 1, .x = 1, .y = 1, .width = 1, .height = 1,
        .dpi = 240, .name = "display", .uniq = "xx" };
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->UpdateDisplayInfo(displayInfo));
}

/**
 * @tc.name: KnuckleDynamicDrawingManagerTest_UpdateDisplayInfo_002
 * @tc.desc: Test UpdateDisplayInfo
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDynamicDrawingManagerTest, KnuckleDynamicDrawingManagerTest_UpdateDisplayInfo_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayInfo displayInfo;
    EXPECT_NO_FATAL_FAILURE(knuckleDynamicDrawingMgr->UpdateDisplayInfo(displayInfo));
}
} // namespace MMI
} // namespace OHOS