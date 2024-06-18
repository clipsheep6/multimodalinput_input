/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "define_multimodal.h"
#include "event_dispatch_handler.h"
#include "i_input_windows_manager.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class EventDispatchTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: EventDispatchTest_HandleTouchEvent_001
 * @tc.desc: Test the function HandleTouchEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleTouchEvent_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleTouchEvent(sharedPointerEvent));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_001
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_002
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(sharedPointerEvent, nullptr);

    std::vector<int32_t> pointerIdList;
    pointerIdList.push_back(1);
    pointerIdList.push_back(2);
    EXPECT_TRUE(pointerIdList.size() > 1);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_003
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_003, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(sharedPointerEvent, nullptr);

    std::vector<int32_t> pointerIdList;
    pointerIdList.push_back(1);
    pointerIdList.push_back(2);
    pointerIdList.push_back(3);
    EXPECT_TRUE(pointerIdList.size() > 1);

    int32_t itemPid = 5;
    EXPECT_TRUE(itemPid >= 0);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_001
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(10);
    pointerItem.SetWindowY(20);
    pointerItem.SetTargetWindowId(2);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    EXPECT_TRUE(windowInfo == std::nullopt);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_002
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 2;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(20);
    pointerItem.SetWindowY(30);
    pointerItem.SetTargetWindowId(3);

    std::optional<WindowInfo> windowInfo;
    EXPECT_TRUE(windowInfo->transform.empty());
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner_001
 * @tc.desc: Test the function HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    PointerEvent* pointerEvent = new PointerEvent(eventType);
    std::shared_ptr<PointerEvent> sharedPointerEvent(pointerEvent);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandlePointerEventInner(sharedPointerEvent));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner_002
 * @tc.desc: Test the function HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    EXPECT_FALSE(windowIds.empty());
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandlePointerEventInner(point));
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_001
 * @tc.desc: Test the function DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    UDSServer udsServer;
    int32_t keyevent = 3;
    KeyEvent* keyEvent = new KeyEvent(keyevent);
    std::shared_ptr<KeyEvent> sharedKeyEvent(keyEvent);
    int32_t ret = eventdispatchhandler.DispatchKeyEventPid(udsServer, sharedKeyEvent);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: EventDispatchTest_AcquireEnableMark
 * @tc.desc: Test Acquire Enable Mark
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_AcquireEnableMark, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> event = PointerEvent::Create();
    ASSERT_NE(event, nullptr);
    event->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_FALSE(dispatch.AcquireEnableMark(event));
    event->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_TRUE(dispatch.AcquireEnableMark(event));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner
 * @tc.desc: Test HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem pointerItem;
    pointerEvent->SetPointerId(5);
    pointerItem.SetPointerId(5);
    pointerEvent->AddPointerItem(pointerItem);
    ASSERT_NO_FATAL_FAILURE(dispatch.HandlePointerEventInner(pointerEvent));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_001
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_001, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = -1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    dispatch.eventTime_ = 1000;
    pointerEvent->SetActionTime(5000);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_002
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_002, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = -1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    int32_t currentTime = dispatch.currentTime_;
    int32_t eventTime = dispatch.eventTime_;
    int32_t INTERVAL_TIME = 3000;
    currentTime = 6000;
    eventTime = 1000;
    EXPECT_TRUE(currentTime - eventTime > INTERVAL_TIME);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_003
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_003, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = 1;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    int32_t pointerAction;
    pointerAction = PointerEvent::POINTER_ACTION_DOWN;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(point, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_002
 * @tc.desc: Test Dispatch Key Event Pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_002, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> KeyEvent = KeyEvent::Create();
    ASSERT_NE(KeyEvent, nullptr);
    dispatch.eventTime_ = 1000;
    KeyEvent->SetActionTime(5000);
    ASSERT_EQ(dispatch.DispatchKeyEventPid(udsServer, KeyEvent), RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_003
 * @tc.desc: Test DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_003, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    int32_t currentTime = dispatch.currentTime_;
    int32_t eventTime = dispatch.eventTime_;
    int32_t INTERVAL_TIME = 3000;
    currentTime = 6000;
    eventTime = 1000;
    EXPECT_TRUE(currentTime - eventTime > INTERVAL_TIME);
    int32_t ret = dispatch.DispatchKeyEventPid(udsServer, keyEvent);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_004
 * @tc.desc: Test DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_004, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);

    NetPacket pkt(MmiMessageId::INVALID);
    EXPECT_FALSE(pkt.ChkRWError());
    int32_t ret = dispatch.DispatchKeyEventPid(udsServer, keyEvent);
    EXPECT_EQ(ret, RET_ERR);
}
} // namespace MMI
} // namespace OHOS
