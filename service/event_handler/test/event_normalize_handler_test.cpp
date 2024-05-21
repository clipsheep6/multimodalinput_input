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

#include <gtest/gtest.h>
#include <libinput.h>

#include "dfx_hisysevent.h"
#include "event_normalize_handler.h"

#include "libinput-private.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class EventNormalizeHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: EventNormalizeHandlerTest_HandleEvent_001
 * @tc.desc: Test the function HandleEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleEvent_001, TestSize.Level1)
{
    EventNormalizeHandler handler;
    int64_t frameTime = 10000;
    libinput_event* event = nullptr;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.ProcessNullEvent(event, frameTime));
    event = new (std::nothrow) libinput_event;
    ASSERT_NE(event, nullptr);
    event->type = LIBINPUT_EVENT_TOUCH_CANCEL;
    ASSERT_NO_FATAL_FAILURE(handler.HandleEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_TOUCH_FRAME;
    ASSERT_NO_FATAL_FAILURE(handler.HandleEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_DEVICE_ADDED;
    ASSERT_NO_FATAL_FAILURE(handler.HandleEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_DEVICE_REMOVED;
    ASSERT_NO_FATAL_FAILURE(handler.HandleEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_KEYBOARD_KEY;
    ASSERT_NO_FATAL_FAILURE(handler.HandleEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_POINTER_MOTION;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_BUTTON;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_AXIS;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_TAP;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_POINTER_MOTION_TOUCHPAD;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleMouseEvent(event));
    event->type = LIBINPUT_EVENT_TOUCHPAD_DOWN;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchPadEvent(event));
    event->type = LIBINPUT_EVENT_TOUCHPAD_UP;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchPadEvent(event));
    event->type = LIBINPUT_EVENT_TOUCHPAD_MOTION;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchPadEvent(event));
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleEvent_002
 * @tc.desc: Test the function HandleEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleEvent_002, TestSize.Level1)
{
    EventNormalizeHandler handler;
    int64_t frameTime = 10000;
    libinput_event* event = new (std::nothrow) libinput_event;
    ASSERT_NE(event, nullptr);
    event->type = LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleGestureEvent(event));
    event->type = LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleGestureEvent(event));
    event->type = LIBINPUT_EVENT_GESTURE_SWIPE_END;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleGestureEvent(event));
    event->type = LIBINPUT_EVENT_GESTURE_PINCH_UPDATE;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleGestureEvent(event));
    event->type = LIBINPUT_EVENT_GESTURE_PINCH_END;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleGestureEvent(event));
    event->type = LIBINPUT_EVENT_TOUCH_DOWN;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_TOUCH_UP;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_TOUCH_MOTION;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchEvent(event, frameTime));
    event->type = LIBINPUT_EVENT_TABLET_TOOL_AXIS;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTableToolEvent(event));
    event->type = LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTableToolEvent(event));
    event->type = LIBINPUT_EVENT_TABLET_TOOL_TIP;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleTableToolEvent(event));
    event->type = LIBINPUT_EVENT_JOYSTICK_BUTTON;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleJoystickEvent(event));
    event->type = LIBINPUT_EVENT_JOYSTICK_AXIS;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleJoystickEvent(event));
    event->type = LIBINPUT_EVENT_SWITCH_TOGGLE;
    handler.HandleEvent(event, frameTime);
    ASSERT_NO_FATAL_FAILURE(handler.HandleSwitchInputEvent(event));
}

/**
 * @tc.name: EventNormalizeHandlerTest_ProcessNullEvent_001
 * @tc.desc: Test the function ProcessNullEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_ProcessNullEvent_001, TestSize.Level1)
{
    EventNormalizeHandler handler;
    int64_t frameTime = 10000;
    libinput_event* event = nullptr;
    bool ret = handler.ProcessNullEvent(event, frameTime);
    ASSERT_FALSE(ret);
    event = new (std::nothrow) libinput_event;
    ASSERT_NE(event, nullptr);
    event->type = LIBINPUT_EVENT_NONE;
    ret = handler.ProcessNullEvent(event, frameTime);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleKeyEvent_001
 * @tc.desc: Test the function HandleKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleKeyEvent_001, TestSize.Level1)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->SetRepeat(true);
    EventNormalizeHandler handler;
    ASSERT_NO_FATAL_FAILURE(handler.HandleKeyEvent(keyEvent));
    keyEvent->SetRepeat(false);
    ASSERT_NO_FATAL_FAILURE(handler.HandleKeyEvent(keyEvent));
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandlePointerEvent_001
 * @tc.desc: Test the function HandlePointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandlePointerEvent_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    EventNormalizeHandler handler;
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_NO_FATAL_FAILURE(handler.HandlePointerEvent(pointerEvent));
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_END);
    ASSERT_NO_FATAL_FAILURE(handler.HandlePointerEvent(pointerEvent));
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEvent_001
 * @tc.desc: Test the function HandleTouchEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEvent_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    EventNormalizeHandler handler;
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchEvent(pointerEvent));
    pointerEvent = nullptr;
    ASSERT_NO_FATAL_FAILURE(handler.HandleTouchEvent(pointerEvent));
}

/**
 * @tc.name: EventNormalizeHandlerTest_UpdateKeyEventHandlerChain_001
 * @tc.desc: Test the function UpdateKeyEventHandlerChain
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_UpdateKeyEventHandlerChain_001, TestSize.Level1)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    EventNormalizeHandler handler;
    ASSERT_NO_FATAL_FAILURE(handler.UpdateKeyEventHandlerChain(keyEvent));
    keyEvent = nullptr;
    ASSERT_NO_FATAL_FAILURE(handler.UpdateKeyEventHandlerChain(keyEvent));
}

/**
 * @tc.name: EventNormalizeHandlerTest_SetOriginPointerId_001
 * @tc.desc: Test the function SetOriginPointerId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_SetOriginPointerId_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    EventNormalizeHandler handler;
    ASSERT_NO_FATAL_FAILURE(handler.SetOriginPointerId(pointerEvent));
    pointerEvent = nullptr;
    int32_t ret = handler.SetOriginPointerId(pointerEvent);
    pointerEvent = nullptr;
    ASSERT_EQ(ret, ERROR_NULL_POINTER);
}

#ifdef OHOS_BUILD_ENABLE_TEST
/**
 * @tc.name: EventNormalizeHandlerTest_SetMoveEventFilters_001
 * @tc.desc: Set moveEventFilterFlag_
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_SetMoveEventFilters_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    bool flag = true;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: EventNormalizeHandlerTest_SetMoveEventFilters_002
 * @tc.desc: Set moveEventFilterFlag_
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_SetMoveEventFilters_002, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    bool flag = false;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_001
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = true;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(0);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_TRUE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_002
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_002, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = true;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(1);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_003
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_003, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = true;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(2);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_004
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_004, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = false;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(0);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_005
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_005, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = false;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(1);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEventWithFlag_006
 * @tc.desc: Handle Touch Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEventWithFlag_006, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    bool flag = false;
    int32_t ret = eventNormalizeHandler.SetMoveEventFilters(flag);
    ASSERT_EQ(ret, RET_OK);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item1);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);

    pointerEvent->RemovePointerItem(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(2);
    pointerEvent->AddPointerItem(item2);
    flag = eventNormalizeHandler.HandleTouchEventWithFlag(pointerEvent);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: EventNormalizeHandlerTest_AddHandleTimer_001
 * @tc.desc: Add handlerTimer
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_AddHandleTimer_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    const int32_t timeOut = 400;
    int32_t timeId = eventNormalizeHandler.AddHandleTimer(timeOut);
    ASSERT_NE(timeId, -1);
}

/**
 * @tc.name: EventNormalizeHandlerTest_CalcTouchOffset_001
 * @tc.desc: Determine whether the touch produces displacement
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_CalcTouchOffset_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    double offSet = eventNormalizeHandler.CalcTouchOffset(pointerEvent);
    ASSERT_EQ(offSet, 0.f);
}

/**
 * @tc.name: EventNormalizeHandlerTest_CalcTouchOffset_002
 * @tc.desc: Determine whether the touch produces displacement
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_CalcTouchOffset_002, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDisplayX(0);
    item1.SetDisplayY(0);
    eventNormalizeHandler.lastTouchDownItems_.push_back(item1);
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDisplayX(0);
    item2.SetDisplayY(1);
    pointerEvent->AddPointerItem(item2);
    double offSet = eventNormalizeHandler.CalcTouchOffset(pointerEvent);
    ASSERT_EQ(offSet, 1.f);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleSwitchInputEvent_001
 * @tc.desc: Handle Switch Input Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleSwitchInputEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleSwitchInputEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleJoystickEvent_001
 * @tc.desc: Handle Joystick Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleJoystickEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleJoystickEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTableToolEvent_001
 * @tc.desc: Handle TableTool Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTableToolEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleTableToolEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEvent_002
 * @tc.desc: Handle touch event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEvent_002, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int64_t frameTime = 50;
    int32_t ret = eventNormalizeHandler.HandleTouchEvent(event, frameTime);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleGestureEvent_001
 * @tc.desc: Handle Gesture Event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleGestureEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleGestureEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_GestureIdentify_001
 * @tc.desc: Gesture Identify
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_GestureIdentify_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.GestureIdentify(event);
    ASSERT_EQ(ret, ERROR_NULL_POINTER);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleTouchPadEvent_001
 * @tc.desc: Handle TouchPadEvent
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchPadEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleTouchPadEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_HandleMouseEvent_001
 * @tc.desc: Handle mouseEvent
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleMouseEvent_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.HandleMouseEvent(event);
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
}

/**
 * @tc.name: EventNormalizeHandlerTest_OnEventDeviceRemoved_001
 * @tc.desc: OnEvent device removed
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_OnEventDeviceRemoved_001, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int32_t ret = eventNormalizeHandler.OnEventDeviceRemoved(event);
    ASSERT_EQ(ret, ERROR_NULL_POINTER);
}

/**
 * @tc.name: EventNormalizeHandlerTest_ProcessNullEvent_002
 * @tc.desc: Process nullEvent
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_ProcessNullEvent_002, TestSize.Level1)
{
    EventNormalizeHandler eventNormalizeHandler;
    libinput_event *event = nullptr;
    int64_t frametime = 30;
    bool flag = eventNormalizeHandler.ProcessNullEvent(event, frametime);
    ASSERT_FALSE(flag);
}
#endif // OHOS_BUILD_ENABLE_TEST
} // namespace MMI
} // namespace OHOS