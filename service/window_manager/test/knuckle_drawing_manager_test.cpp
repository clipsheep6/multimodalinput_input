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

#include "knuckle_drawing_manager.h"
#include "mmi_log.h"
#include "pointer_event.h"
#include "window_info.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "KnuckleDrawingManagerTest"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace
class KnuckleDrawingManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void)
    {
        DisplayInfo info;
        info.id = 1;
        info.x =1;
        info.y = 1;
        info.width = 1;
        info.height = 1;
        int32_t displayDpi = 240;
        info.dpi = displayDpi;
        info.name = "display";
        info.uniq = "xx";
        if (knuckleDrawMgr_ == nullptr) {
            knuckleDrawMgr_ = std::make_shared<KnuckleDrawingManager>();
        }
        knuckleDrawMgr_->UpdateDisplayInfo(info);
    }
private:
    std::shared_ptr<KnuckleDrawingManager> knuckleDrawMgr_ { nullptr };
};

/**
 * @tc.name: KnuckleDrawingManagerTest_StartTouchDraw
 * @tc.desc: Test Overrides StartTouchDraw function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_StartTouchDraw, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    kceDrawMgr.canvasNode_ = nullptr;
    kceDrawMgr.StartTouchDraw(pointerEvent);
    EXPECT_EQ(kceDrawMgr.canvasNode_, nullptr);

    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    kceDrawMgr.isActionUp_ = true;
    kceDrawMgr.displayInfo_.width = 200;
    kceDrawMgr.displayInfo_.height = 200;
    kceDrawMgr.StartTouchDraw(pointerEvent);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_CreateTouchWindow
 * @tc.desc: Test Overrides CreateTouchWindow function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_CreateTouchWindow, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    int32_t displayId = 10;
    kceDrawMgr.surfaceNode_ = nullptr;
    kceDrawMgr.displayInfo_.width = 200;
    kceDrawMgr.displayInfo_.height = 200;
    kceDrawMgr.CreateTouchWindow(displayId);

    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "touch window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.CreateTouchWindow(displayId);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_001
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_001, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_002
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_002, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_003
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_003, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_004
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_004, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_005
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_005, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_006
 * @tc.desc: Test KnuckleDrawHandler
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_006, TestSize.Level1)
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
    knuckleDrawMgr_->KnuckleDrawHandler(pointerEvent);
    EXPECT_FALSE(knuckleDrawMgr_->isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsValidAction
 * @tc.desc: Test Overrides IsValidAction function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsValidAction, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    int32_t action = PointerEvent::POINTER_ACTION_DOWN;
    ASSERT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_UP;
    ASSERT_TRUE(kceDrawMgr.IsValidAction(action));

    action = PointerEvent::POINTER_ACTION_MOVE;
    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    ASSERT_TRUE(kceDrawMgr.IsValidAction(action));

    action = PointerEvent::POINTER_ACTION_UNKNOWN;
    kceDrawMgr.pointerInfos_.clear();
    ASSERT_FALSE(kceDrawMgr.IsValidAction(action));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateDisplayInfo
 * @tc.desc: Test Overrides UpdateDisplayInfo function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateDisplayInfo, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    DisplayInfo displayInfo;
    displayInfo.direction = Direction::DIRECTION0;
    kceDrawMgr.displayInfo_.direction = Direction::DIRECTION0;
    kceDrawMgr.UpdateDisplayInfo(displayInfo);
    EXPECT_FALSE(kceDrawMgr.isRotate_);
    kceDrawMgr.displayInfo_.direction = Direction::DIRECTION90;
    kceDrawMgr.UpdateDisplayInfo(displayInfo);
    EXPECT_TRUE(kceDrawMgr.isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsSingleKnuckle
 * @tc.desc: Test Overrides IsSingleKnuckle function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsSingleKnuckle, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    auto pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    ASSERT_TRUE(kceDrawMgr.IsSingleKnuckle(pointerEvent));

    item.SetPointerId(1);
    item.SetToolType(PointerEvent::TOOL_TYPE_TOUCHPAD);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    kceDrawMgr.canvasNode_ = nullptr;
    ASSERT_FALSE(kceDrawMgr.IsSingleKnuckle(pointerEvent));

    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
    ASSERT_FALSE(kceDrawMgr.IsSingleKnuckle(pointerEvent));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_GetPointerPos
 * @tc.desc: Test Overrides GetPointerPos function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_GetPointerPos, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(kceDrawMgr.GetPointerPos(pointerEvent), RET_OK);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerId(1);
    pointerEvent->AddPointerItem(item);
    ASSERT_EQ(kceDrawMgr.GetPointerPos(pointerEvent), RET_ERR);

    pointerEvent->SetPointerId(0);
    ASSERT_EQ(kceDrawMgr.GetPointerPos(pointerEvent), RET_ERR);

    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 150;
    pointerInfo.y = 150;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 300;
    pointerInfo.y = 300;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    ASSERT_EQ(kceDrawMgr.GetPointerPos(pointerEvent), RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawGraphic
 * @tc.desc: Test Overrides DrawGraphic function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawGraphic, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(500);
    item.SetDisplayY(500);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent), RET_ERR);

    kceDrawMgr.displayInfo_.width = 200;
    kceDrawMgr.displayInfo_.height = 200;
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent), RET_OK);

    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerInfo.x = 150;
    pointerInfo.y = 150;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 200;
    pointerInfo.y = 200;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 300;
    pointerInfo.y = 300;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent), RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_RotationCanvasNode
 * @tc.desc: Test Overrides RotationCanvasNode function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_RotationCanvasNode, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(canvasNode, nullptr);
    DisplayInfo displayInfo;
    displayInfo.direction = Direction::DIRECTION0;
    EXPECT_NO_FATAL_FAILURE(KnuckleDrawingManager::RotationCanvasNode(canvasNode, displayInfo));
    displayInfo.direction = Direction::DIRECTION90;
    EXPECT_NO_FATAL_FAILURE(KnuckleDrawingManager::RotationCanvasNode(canvasNode, displayInfo));
    displayInfo.direction = Direction::DIRECTION180;
    EXPECT_NO_FATAL_FAILURE(KnuckleDrawingManager::RotationCanvasNode(canvasNode, displayInfo));
    displayInfo.direction = Direction::DIRECTION270;
    EXPECT_NO_FATAL_FAILURE(KnuckleDrawingManager::RotationCanvasNode(canvasNode, displayInfo));
}
} // namespace MMI
} // namespace OHOS