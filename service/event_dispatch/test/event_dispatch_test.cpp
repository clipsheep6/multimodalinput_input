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
#include "input_event_handler.h"

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
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem itemFirst;
    itemFirst.SetPointerId(0);
    itemFirst.SetPressed(true);
    pointerEvent->UpdatePointerItem(0, itemFirst);
    UDSServer udsServer;
    InputHandler->udsServer_ = &udsServer;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(pointerEvent, fd));
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
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem itemFirst;
    itemFirst.SetPointerId(0);
    itemFirst.SetPressed(true);
    pointerEvent->UpdatePointerItem(0, itemFirst);
    PointerEvent::PointerItem itemSecond;
    itemSecond.SetPointerId(1);
    itemSecond.SetPressed(true);
    pointerEvent->UpdatePointerItem(1, itemSecond);
    UDSServer udsServer;
    InputHandler->udsServer_ = &udsServer;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_NotifyPointerEventToRS_001
 * @tc.desc: Test the function NotifyPointerEventToRS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_NotifyPointerEventToRS_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t action = 1;
    std::string name = "ExampleProgram";
    uint32_t processId = 12345;
    int32_t touchCnt = 0;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.NotifyPointerEventToRS(action, name, processId, touchCnt));
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
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandlePointerEventInner(pointerEvent));
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
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->SetId(0);
    keyEvent->SetKeyCode(0);
    int32_t ret = eventdispatchhandler.DispatchKeyEventPid(udsServer, keyEvent);
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
    pointerItem.SetPointerId(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);
    ASSERT_NO_FATAL_FAILURE(dispatch.HandlePointerEventInner(pointerEvent));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = -1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem itemFirst;
    itemFirst.SetPointerId(0);
    itemFirst.SetPressed(true);
    itemFirst.SetDownTime(5000);
    pointerEvent->UpdatePointerItem(0, itemFirst);
    pointerEvent->SetPointerId(0);
    dispatch.eventTime_ = 1000;
    pointerEvent->SetActionTime(5000);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid
 * @tc.desc: Test Dispatch Key Event Pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid, TestSize.Level1)
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
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_001
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_001, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem itemFirst;
    itemFirst.SetPointerId(0);
    itemFirst.SetPressed(true);
    pointerEvent->UpdatePointerItem(0, itemFirst);
    pointerEvent->SetPointerId(0);
    ASSERT_NO_FATAL_FAILURE(dispatch.HandleMultiWindowPointerEvent(pointerEvent, itemFirst));
}
} // namespace MMI
} // namespace OHOS
