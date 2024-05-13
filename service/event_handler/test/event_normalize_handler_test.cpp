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

#include "event_normalize_handler.h"

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
 * @tc.name: EventNormalizeHandlerTest_HandleTouchEvent_001
 * @tc.desc: Handle touch event
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_HandleTouchEvent_001, TestSize.Level1)
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
 * @tc.name: EventNormalizeHandlerTest_ProcessNullEvent_001
 * @tc.desc: Process nullEvent
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(EventNormalizeHandlerTest, EventNormalizeHandlerTest_ProcessNullEvent_001, TestSize.Level1)
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