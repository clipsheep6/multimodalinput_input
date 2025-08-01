/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
constexpr uint32_t TRACK_COLOR_BLUE {0xFF1ED0EE};
constexpr uint32_t TRACK_COLOR_PINK {0xFFFF42D2};
constexpr uint32_t TRACK_COLOR_ORANGE_RED {0xFFFF7B47};
constexpr uint32_t TRACK_COLOR_YELLOW {0xFFFFC628};
constexpr int32_t TRACK_PATH_LENGTH_400 {400};
constexpr int32_t TRACK_PATH_LENGTH_500 {500};
constexpr int32_t TRACK_PATH_LENGTH_1000 {1000};
constexpr int32_t TRACK_PATH_LENGTH_1500 {1500};
constexpr int32_t TRACK_PATH_LENGTH_2000 {2000};
constexpr std::string_view SCREEN_READ_ENABLE { "1" };
constexpr int32_t POINTER_NUMBER_TO_DRAW { 10 };
} // namespace
class KnuckleDrawingManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void)
    {
        OLD::DisplayInfo info;
        info.id = 1;
        info.x = 1;
        info.y = 1;
        info.width = 1;
        info.height = 1;
        int32_t displayDpi = 240;
        info.dpi = displayDpi;
        info.name = "display";
        if (knuckleDrawMgr_ == nullptr) {
            knuckleDrawMgr_ = std::make_shared<KnuckleDrawingManager>();
        }
        knuckleDrawMgr_->UpdateDisplayInfo(info);
        pointerEvent_ = PointerEvent::Create();
    }

    void TearDown(void)
    {
        pointerEvent_ = nullptr;
    }

private:
    std::shared_ptr<KnuckleDrawingManager> knuckleDrawMgr_ {nullptr};
    std::shared_ptr<PointerEvent> pointerEvent_ {nullptr};
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
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    kceDrawMgr.trackCanvasNode_ = nullptr;
    kceDrawMgr.StartTouchDraw(pointerEvent);
    EXPECT_EQ(kceDrawMgr.trackCanvasNode_, nullptr);
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
#else
    kceDrawMgr.canvasNode_ = nullptr;
    kceDrawMgr.StartTouchDraw(pointerEvent);
    EXPECT_EQ(kceDrawMgr.canvasNode_, nullptr);
    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    kceDrawMgr.isActionUp_ = true;
    kceDrawMgr.displayInfo_.width = 200;
    kceDrawMgr.displayInfo_.height = 200;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
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
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    kceDrawMgr.trackCanvasNode_ = nullptr;
    ASSERT_FALSE(kceDrawMgr.IsSingleKnuckle(pointerEvent));

    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
#else
    kceDrawMgr.canvasNode_ = nullptr;
    ASSERT_FALSE(kceDrawMgr.IsSingleKnuckle(pointerEvent));

    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
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
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
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
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
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

    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
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
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent), RET_ERR);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawGraphic_002
 * @tc.desc: Test Overrides DrawGraphic function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawGraphic_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 150;
    pointerInfo.y = 150;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 200;
    pointerInfo.y = 200;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 300;
    pointerInfo.y = 300;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);

    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerId(1);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDisplayX(500);
    item.SetDisplayY(500);
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    kceDrawMgr.isActionUp_ = false;
    pointerEvent->SetActionTime(200001);
    kceDrawMgr.firstDownTime_ = 200000;
    kceDrawMgr.pointerNum_ = POINTER_NUMBER_TO_DRAW;
    auto ret = kceDrawMgr.DrawGraphic(pointerEvent);
    EXPECT_FALSE(kceDrawMgr.isActionUp_);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawGraphic_003
 * @tc.desc: Test Overrides DrawGraphic function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawGraphic_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.pointerInfos_.clear();
    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 150;
    pointerInfo.y = 150;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 200;
    pointerInfo.y = 200;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 300;
    pointerInfo.y = 300;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);

    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerId(1);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDisplayX(500);
    item.SetDisplayY(500);
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    kceDrawMgr.isActionUp_ = false;
    pointerEvent->SetActionTime(200001);
    kceDrawMgr.firstDownTime_ = 100000;
    kceDrawMgr.isNeedInitParticleEmitter_ = false;
    auto ret = kceDrawMgr.DrawGraphic(pointerEvent);
    EXPECT_EQ(ret, RET_ERR);
}
#else
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
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent), RET_ERR);
}
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC

/**
 * @tc.name: KnuckleDrawingManagerTest_DestoryWindow_001
 * @tc.desc: Test Overrides DestoryWindow function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DestoryWindow_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    kceDrawMgr.trackCanvasNode_ = nullptr;
#else
    kceDrawMgr.canvasNode_ = nullptr;
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    ASSERT_EQ(kceDrawMgr.DestoryWindow(), RET_ERR);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DestoryWindow_002
 * @tc.desc: Test Overrides DestoryWindow function branches
 * @tc.type: Function
 * @tc.require:
 */
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DestoryWindow_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.trackCanvasNode_ = nullptr;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    ASSERT_EQ(kceDrawMgr.DestoryWindow(), RET_OK);
}
#else
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DestoryWindow_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.canvasNode_ = nullptr;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
    ASSERT_EQ(kceDrawMgr.DestoryWindow(), RET_OK);
}
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_007
 * @tc.desc: Test Overrides KnuckleDrawHandler function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_007, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    std::shared_ptr<PointerEvent> touchEvent = PointerEvent::Create();
    ASSERT_NE(touchEvent, nullptr);
    touchEvent->SetPointerId(1);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDisplayX(150);
    item.SetDisplayY(150);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    touchEvent->AddPointerItem(item);
    kceDrawMgr.isRotate_ = false;
    kceDrawMgr.lastUpTime_ = 1000;
    kceDrawMgr.lastDownPointer_.x = 50;
    kceDrawMgr.lastDownPointer_.y = 50;
    touchEvent->SetTargetDisplayId(10);
    touchEvent->SetActionTime(100);
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.KnuckleDrawHandler(touchEvent));
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UNKNOWN);
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.KnuckleDrawHandler(touchEvent));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_KnuckleDrawHandler_008
 * @tc.desc: Test Overrides KnuckleDrawHandler function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_KnuckleDrawHandler_008, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    std::shared_ptr<PointerEvent> touchEvent = PointerEvent::Create();
    ASSERT_NE(touchEvent, nullptr);
    touchEvent->SetPointerId(1);
    touchEvent->SetActionTime(10);
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDisplayX(150);
    item.SetDisplayY(150);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    touchEvent->AddPointerItem(item);
    item.SetPointerId(2);
    touchEvent->AddPointerItem(item);
    kceDrawMgr.isRotate_ = true;
    constexpr int32_t rsId = 1;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.KnuckleDrawHandler(touchEvent, rsId));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsSingleKnuckleDoubleClick
 * @tc.desc: Test Overrides IsSingleKnuckleDoubleClick function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsSingleKnuckleDoubleClick, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.lastUpTime_ = 100;
    std::shared_ptr<PointerEvent> touchEvent = PointerEvent::Create();
    ASSERT_NE(touchEvent, nullptr);
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    touchEvent->SetActionTime(200);
    touchEvent->SetPointerId(1);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDisplayX(50);
    item.SetDisplayY(50);
    kceDrawMgr.lastDownPointer_.x = 60;
    kceDrawMgr.lastDownPointer_.y = 60;
    EXPECT_FALSE(kceDrawMgr.IsSingleKnuckleDoubleClick(touchEvent));
    kceDrawMgr.lastUpTime_ = 500;
    EXPECT_TRUE(kceDrawMgr.IsSingleKnuckleDoubleClick(touchEvent));
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    EXPECT_TRUE(kceDrawMgr.IsSingleKnuckleDoubleClick(touchEvent));
    touchEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    EXPECT_TRUE(kceDrawMgr.IsSingleKnuckleDoubleClick(touchEvent));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsValidAction_001
 * @tc.desc: Test Overrides IsValidAction function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsValidAction_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    int32_t action = PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_PULL_DOWN;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_MOVE;
    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_PULL_MOVE;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_UP;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_PULL_UP;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
    action = PointerEvent::POINTER_ACTION_UNKNOWN;
    EXPECT_FALSE(kceDrawMgr.IsValidAction(action));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsValidAction_003
 * @tc.desc: Test Overrides IsValidAction function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsValidAction_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    int32_t action = PointerEvent::POINTER_ACTION_DOWN;
    kceDrawMgr.screenReadState_.state = SCREEN_READ_ENABLE;
    EXPECT_TRUE(kceDrawMgr.IsValidAction(action));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_CreateTouchWindow_001
 * @tc.desc: Test Overrides CreateTouchWindow function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_CreateTouchWindow_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t displayId = 10;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    knuckleDrawMgr_->surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    knuckleDrawMgr_->displayInfo_.displayMode = DisplayMode::MAIN;
    EXPECT_NO_FATAL_FAILURE(knuckleDrawMgr_->CreateTouchWindow(displayId));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_CreateTouchWindow_002
 * @tc.desc: Test Overrides CreateTouchWindow function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_CreateTouchWindow_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t displayId = 10;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    knuckleDrawMgr_->surfaceNode_ = nullptr;
    knuckleDrawMgr_->displayInfo_.displayMode = DisplayMode::MAIN;
    knuckleDrawMgr_->SetMultiWindowScreenId(10, 0);
    knuckleDrawMgr_->CreateTouchWindow(displayId);
    EXPECT_EQ(knuckleDrawMgr_->screenId_, 0);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsSingleKnuckle_001
 * @tc.desc: Test Overrides IsSingleKnuckle function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsSingleKnuckle_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    auto touchEvent = PointerEvent::Create();
    ASSERT_NE(touchEvent, nullptr);
    touchEvent->SetPointerId(1);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetToolType(PointerEvent::TOOL_TYPE_TOUCHPAD);
    touchEvent->AddPointerItem(item);
    kceDrawMgr.isRotate_ = true;
    EXPECT_FALSE(kceDrawMgr.IsSingleKnuckle(touchEvent));

    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    touchEvent->UpdatePointerItem(1, item);
    kceDrawMgr.isRotate_ = true;
    EXPECT_TRUE(kceDrawMgr.IsSingleKnuckle(touchEvent));

    touchEvent->SetPointerId(2);
    item.SetPointerId(2);
    item.SetToolType(PointerEvent::TOOL_TYPE_KNUCKLE);
    kceDrawMgr.isRotate_ = false;
    PointerInfo pointerInfo;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
#else
    kceDrawMgr.canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.canvasNode_, nullptr);
#endif // OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    EXPECT_FALSE(kceDrawMgr.IsSingleKnuckle(touchEvent));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_RotationCanvasNode_001
 * @tc.desc: Test Overrides RotationCanvasNode function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_RotationCanvasNode_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    OLD::DisplayInfo displayInfo;
    displayInfo.direction = Direction::DIRECTION90;
    displayInfo.displayDirection = Direction::DIRECTION0;
    auto canvasNode = Rosen::RSCanvasNode::Create();
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.RotationCanvasNode(canvasNode, displayInfo));

    displayInfo.direction = Direction::DIRECTION270;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.RotationCanvasNode(canvasNode, displayInfo));

    displayInfo.direction = Direction::DIRECTION180;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.RotationCanvasNode(canvasNode, displayInfo));

    displayInfo.direction = Direction::DIRECTION0;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.RotationCanvasNode(canvasNode, displayInfo));
}

#ifdef OHOS_BUILD_ENABLE_NEW_KNUCKLE_DYNAMIC
/**
 * @tc.name: KnuckleDrawingManagerTest_GetDeltaColor_001
 * @tc.desc: Test Overrides GetDeltaColor function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_GetDeltaColor_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t deltaSource = 1;
    uint32_t deltaTarget = 2;
    KnuckleDrawingManager kceDrawMgr;
    auto ret = kceDrawMgr.GetDeltaColor(deltaSource, deltaTarget);
    EXPECT_EQ(ret, 0);

    deltaTarget = 0;
    ret = kceDrawMgr.GetDeltaColor(deltaSource, deltaTarget);
    EXPECT_EQ(ret, (deltaSource - deltaTarget));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawTrackColorBlue_001
 * @tc.desc: Test Overrides DrawTrackColorBlue function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawTrackColorBlue_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t pathValue = 0;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.pathLength_ = TRACK_PATH_LENGTH_2000;
    auto ret = kceDrawMgr.DrawTrackColorBlue(pathValue);
    EXPECT_NE(ret, TRACK_COLOR_BLUE);

    pathValue = TRACK_PATH_LENGTH_2000;
    ret = kceDrawMgr.DrawTrackColorBlue(pathValue);
    EXPECT_EQ(ret, TRACK_COLOR_BLUE);

    kceDrawMgr.pathLength_ = TRACK_PATH_LENGTH_400;
    ret = kceDrawMgr.DrawTrackColorBlue(pathValue);
    EXPECT_EQ(ret, TRACK_COLOR_BLUE);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawTrackColorPink_001
 * @tc.desc: Test Overrides DrawTrackColorPink function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawTrackColorPink_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t pathValue = 0;
    KnuckleDrawingManager kceDrawMgr;
    auto ret = kceDrawMgr.DrawTrackColorPink(pathValue);
    EXPECT_NE(ret, TRACK_COLOR_PINK);

    pathValue = TRACK_PATH_LENGTH_2000;
    ret = kceDrawMgr.DrawTrackColorPink(pathValue);
    EXPECT_EQ(ret, TRACK_COLOR_PINK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawTrackColorYellow_001
 * @tc.desc: Test Overrides DrawTrackColorYellow function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawTrackColorYellow_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t pathValue = 0;
    KnuckleDrawingManager kceDrawMgr;
    auto ret = kceDrawMgr.DrawTrackColorYellow(pathValue);
    EXPECT_NE(ret, TRACK_COLOR_YELLOW);

    pathValue = TRACK_PATH_LENGTH_2000;
    ret = kceDrawMgr.DrawTrackColorYellow(pathValue);
    EXPECT_EQ(ret, TRACK_COLOR_YELLOW);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawTrackColorOrangeRed_001
 * @tc.desc: Test Overrides DrawTrackColorOrangeRed function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawTrackColorOrangeRed_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t pathValue = 0;
    KnuckleDrawingManager kceDrawMgr;
    auto ret = kceDrawMgr.DrawTrackColorOrangeRed(pathValue);
    EXPECT_NE(ret, TRACK_COLOR_ORANGE_RED);

    pathValue = TRACK_PATH_LENGTH_2000;
    ret = kceDrawMgr.DrawTrackColorOrangeRed(pathValue);
    EXPECT_EQ(ret, TRACK_COLOR_ORANGE_RED);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawTrackCanvas_001
 * @tc.desc: Test Overrides DrawTrackCanvas function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawTrackCanvas_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.CreateTrackCanvasNode();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);

    kceDrawMgr.pathLength_ = TRACK_PATH_LENGTH_500;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawTrackCanvas());

    kceDrawMgr.pathLength_ = TRACK_PATH_LENGTH_1000;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawTrackCanvas());

    kceDrawMgr.pathLength_ = TRACK_PATH_LENGTH_1500;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawTrackCanvas());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_ProcessUpEvent_001
 * @tc.desc: Test Overrides ProcessUpEvent function branches when isNeedUpAnimation is false
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_ProcessUpEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    bool isNeedUpAnimation = false;
    auto ret = kceDrawMgr.ProcessUpEvent(isNeedUpAnimation);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_ProcessUpEvent_002
 * @tc.desc: Test Overrides ProcessUpEvent function branches when isNeedUpAnimation is true and
 * destroyTimerId_ greater than 0
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_ProcessUpEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    bool isNeedUpAnimation = true;
    kceDrawMgr.destroyTimerId_ = 1;
    auto ret = kceDrawMgr.ProcessUpEvent(isNeedUpAnimation);
    kceDrawMgr.destroyTimerId_ = -1;
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_ProcessUpEvent_003
 * @tc.desc: Test Overrides ProcessUpEvent function branches when isNeedUpAnimation is true,
 * destroyTimerId_ small than 0 and addTimerFunc_ is invalid
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_ProcessUpEvent_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    bool isNeedUpAnimation = true;
    kceDrawMgr.destroyTimerId_ = -1;
    auto ret = kceDrawMgr.ProcessUpEvent(isNeedUpAnimation);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_ProcessUpEvent_004
 * @tc.desc: Test Overrides ProcessUpEvent function branches when isNeedUpAnimation is true,
 * destroyTimerId_ small than 0, addTimerFunc_ is valid and addTimerFunc_ calling fail
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_ProcessUpEvent_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    bool isNeedUpAnimation = true;
    kceDrawMgr.destroyTimerId_ = -1;

    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);

    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);

    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);

    kceDrawMgr.addTimerFunc_ = [] (int32_t intervalMs, int32_t repeatCount, std::function<void()> callback,
        const std::string &name) -> int32_t {
        (void)intervalMs;
        (void)repeatCount;
        (void)callback;
        (void)name;
        return -1;
    };
    auto ret = kceDrawMgr.ProcessUpEvent(isNeedUpAnimation);
    EXPECT_EQ(ret, RET_OK);
    EXPECT_EQ(kceDrawMgr.destroyTimerId_, -1);
    ASSERT_EQ(kceDrawMgr.brushCanvasNode_, nullptr);
    ASSERT_EQ(kceDrawMgr.trackCanvasNode_, nullptr);
    ASSERT_EQ(kceDrawMgr.surfaceNode_, nullptr);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_ProcessUpEvent_005
 * @tc.desc: Test Overrides ProcessUpEvent function branches when isNeedUpAnimation is true,
 * destroyTimerId_ small than 0, addTimerFunc_ is valid and addTimerFunc_ call success
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_ProcessUpEvent_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    bool isNeedUpAnimation = true;
    kceDrawMgr.destroyTimerId_ = -1;

    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);

    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);

    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);

    kceDrawMgr.addTimerFunc_ = [] (int32_t intervalMs, int32_t repeatCount, std::function<void()> callback,
        const std::string &name) -> int32_t {
        (void)intervalMs;
        (void)repeatCount;
        (void)callback;
        (void)name;
        return 1;
    };
    auto ret = kceDrawMgr.ProcessUpEvent(isNeedUpAnimation);
    EXPECT_EQ(ret, RET_OK);
    EXPECT_EQ(kceDrawMgr.destroyTimerId_, 1);
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateEmitter_001
 * @tc.desc: Test Overrides UpdateEmitter function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateEmitter_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.UpdateEmitter());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateEmitter_002
 * @tc.desc: Test Overrides UpdateEmitter function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateEmitter_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);

    PointerInfo pointerInfo;
    pointerInfo.x = 100;
    pointerInfo.y = 100;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 150;
    pointerInfo.y = 150;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 200;
    pointerInfo.y = 200;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerInfo.x = 300;
    pointerInfo.y = 300;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);

    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.UpdateEmitter());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawBrushCanvas_001
 * @tc.desc: Test Overrides DrawBrushCanvas function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawBrushCanvas_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawBrushCanvas());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawBrushCanvas_002
 * @tc.desc: Test Overrides DrawBrushCanvas function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawBrushCanvas_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    kceDrawMgr.DrawBrushCanvas();
    EXPECT_EQ(kceDrawMgr.pathInfos_.size(), 8);
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawBrushCanvas());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_SetMultiWindowScreenId_001
 * @tc.desc: Test Overrides SetMultiWindowScreenId function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_SetMultiWindowScreenId_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    uint64_t screenId = 1;
    uint64_t displayNodeScreenId = 2;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.SetMultiWindowScreenId(screenId, displayNodeScreenId));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_InitParticleEmitter_001
 * @tc.desc: Test Overrides InitParticleEmitter function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_InitParticleEmitter_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.InitParticleEmitter());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_InitParticleEmitter_002
 * @tc.desc: Test Overrides InitParticleEmitter function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_InitParticleEmitter_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.InitParticleEmitter());
    EXPECT_FALSE(kceDrawMgr.isNeedInitParticleEmitter_);
}
#endif

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateDisplayInfo_002
 * @tc.desc: Test UpdateDisplayInfo
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateDisplayInfo_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    OLD::DisplayInfo displayInfo;
    kceDrawMgr.isRotate_ = false;
    displayInfo.direction = Direction::DIRECTION90;
    kceDrawMgr.displayInfo_.direction = Direction::DIRECTION90;
    kceDrawMgr.UpdateDisplayInfo(displayInfo);
    EXPECT_FALSE(kceDrawMgr.isRotate_);

    displayInfo.direction = Direction::DIRECTION90;
    kceDrawMgr.displayInfo_.direction = Direction::DIRECTION0;
    kceDrawMgr.UpdateDisplayInfo(displayInfo);
    EXPECT_TRUE(kceDrawMgr.isRotate_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_RegisterAddTimer_001
 * @tc.desc: Test Overrides RegisterAddTimer function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_RegisterAddTimer_001, TestSize.Level1)
{
    knuckleDrawMgr_->RegisterAddTimer([] (int32_t intervalMs, int32_t repeatCount, std::function<void()> callback,
        const std::string &name) -> int32_t {
        (void)intervalMs;
        (void)repeatCount;
        (void)callback;
        (void)name;
        return 1;
    });
    ASSERT_TRUE(knuckleDrawMgr_->addTimerFunc_);
}

/**
 * @tc.name: KnuckleDrawingManagerTest_IsValidAction_002
 * @tc.desc: Test IsValidAction
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_IsValidAction_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    ASSERT_NE(knuckleDrawMgr_, nullptr);
    ASSERT_NE(pointerEvent_, nullptr);
    kceDrawMgr.screenReadState_.state = "1";
    int32_t action = PointerEvent::POINTER_ACTION_DOWN;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.IsValidAction(action));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateDisplayInfo_001
 * @tc.desc: Test UpdateDisplayInfo
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateDisplayInfo_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_NE(knuckleDrawMgr_, nullptr);
    ASSERT_NE(pointerEvent_, nullptr);
    OLD::DisplayInfo info;
    info.id = 10;
    info.x = 10;
    info.y = 5;
    info.width = 5;
    info.height = 5;
    int32_t displayDpi = 220;
    info.dpi = displayDpi;
    info.name = "display";
    info.direction = Direction::DIRECTION90;
    if (knuckleDrawMgr_ == nullptr)
    {
        knuckleDrawMgr_ = std::make_shared<KnuckleDrawingManager>();
    }
    EXPECT_NO_FATAL_FAILURE(knuckleDrawMgr_->UpdateDisplayInfo(info));
}

/**
 * @tc.name: KnuckleDrawingManagerTest_UpdateEmitter001
 * @tc.desc: Test Overrides UpdateEmitter function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_UpdateEmitter001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    ASSERT_NE(pointerEvent_, nullptr);
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
    EXPECT_NO_FATAL_FAILURE(knuckleDrawMgr_->UpdateEmitter());
}

/**
 * @tc.name: KnuckleDrawingManagerTest_DrawGraphic002
 * @tc.desc: Test Overrides DrawGraphic function branches
 * @tc.type: Function
 * @tc.require:
 */
HWTEST_F(KnuckleDrawingManagerTest, KnuckleDrawingManagerTest_DrawGraphic002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    KnuckleDrawingManager kceDrawMgr;
    ASSERT_NE(pointerEvent_, nullptr);
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "knuckle window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    kceDrawMgr.surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    ASSERT_NE(kceDrawMgr.surfaceNode_, nullptr);
    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    kceDrawMgr.brushCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.brushCanvasNode_, nullptr);
    PointerEvent::PointerItem item;
    item.SetPointerId(10);
    item.SetDisplayX(50);
    item.SetDisplayY(50);
    pointerEvent_->AddPointerItem(item);
    pointerEvent_->SetPointerId(1);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent_), RET_ERR);

    kceDrawMgr.displayInfo_.width = 20;
    kceDrawMgr.displayInfo_.height = 20;
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent_), RET_OK);

    kceDrawMgr.trackCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    ASSERT_NE(kceDrawMgr.trackCanvasNode_, nullptr);
    PointerInfo pointerInfo;
    pointerInfo.x = 10;
    pointerInfo.y = 10;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    pointerEvent_->SetPointerId(0);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerInfo.x = 50;
    pointerInfo.y = 50;
    kceDrawMgr.pointerInfos_.push_back(pointerInfo);
    ASSERT_EQ(kceDrawMgr.DrawGraphic(pointerEvent_), RET_ERR);
    kceDrawMgr.isActionUp_ = false;
    EXPECT_NO_FATAL_FAILURE(kceDrawMgr.DrawGraphic(pointerEvent_));
}
} // namespace MMI
} // namespace OHOS