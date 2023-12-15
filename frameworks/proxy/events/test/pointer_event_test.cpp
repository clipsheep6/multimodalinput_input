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

#include "define_multimodal.h"
#include "event_util_test.h"
#include "proto.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
#ifdef OHOS_BUILD_ENABLE_POINTER
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "PointerEventTest" };
#endif // OHOS_BUILD_ENABLE_POINTER
using namespace testing::ext;
} // namespace
class PointerEventTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {}
    static std::shared_ptr<PointerEvent> CreatePointEvent();
};

#ifdef OHOS_BUILD_ENABLE_POINTER
std::shared_ptr<PointerEvent> PointerEventTest::CreatePointEvent()
{
    auto pointerEvent = PointerEvent::Create();
    CHKPP(pointerEvent);
    int64_t downTime = GetMillisTime();
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDownTime(downTime);
    item.SetPressed(true);

    item.SetDisplayX(623);
    item.SetDisplayY(823);
    item.SetWindowX(600);
    item.SetWindowY(800);

    item.SetWidth(0);
    item.SetHeight(0);
    item.SetPressure(0);
    item.SetDeviceId(0);
    pointerEvent->AddPointerItem(item);
    return pointerEvent;
}

/**
 * @tc.name: PointerEventTest_keyEventAndPointerEvent_001
 * @tc.desc: Verify ctrl(left) + point event
 * @tc.type: FUNC
 * @tc.require: AR000GOACS
 * @tc.author: yangguang
 */
HWTEST_F(PointerEventTest, PointerEventTest_keyEventAndPointerEvent_001, TestSize.Level1)
{
    std::shared_ptr<PointerEvent> pointerEvent = CreatePointEvent();
    ASSERT_NE(pointerEvent, nullptr);
    std::vector<int32_t> pressedKeys { KeyEvent::KEYCODE_CTRL_LEFT };
    pointerEvent->SetPressedKeys(pressedKeys);
    SimulateInputEventUtilTest(pointerEvent);
}

/**
 * @tc.name: PointerEventTest_keyEventAndPointerEvent_002
 * @tc.desc: Verify ctrl(right) + point event
 * @tc.type: FUNC
 * @tc.require: AR000GOACS
 * @tc.author: yangguang
 */
HWTEST_F(PointerEventTest, PointerEventTest_keyEventAndPointerEvent_002, TestSize.Level1)
{
    std::shared_ptr<PointerEvent> pointerEvent = CreatePointEvent();
    ASSERT_TRUE(pointerEvent != nullptr);
    std::vector<int32_t> pressedKeys { KeyEvent::KEYCODE_CTRL_RIGHT };
    pointerEvent->SetPressedKeys(pressedKeys);
    SimulateInputEventUtilTest(pointerEvent);
}

/**
 * @tc.name: PointerEventTest_keyEventAndPointerEvent_003
 * @tc.desc: Verify ctrl(left and right) + point event
 * @tc.type: FUNC
 * @tc.require: AR000GOACS
 * @tc.author: yangguang
 */
HWTEST_F(PointerEventTest, PointerEventTest_keyEventAndPointerEvent_003, TestSize.Level1)
{
    std::shared_ptr<PointerEvent> pointerEvent = CreatePointEvent();
    ASSERT_TRUE(pointerEvent != nullptr);
    std::vector<int32_t> pressedKeys { KeyEvent::KEYCODE_CTRL_LEFT, KeyEvent::KEYCODE_CTRL_RIGHT };
    pointerEvent->SetPressedKeys(pressedKeys);
    SimulateInputEventUtilTest(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_POINTER

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_001
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_UNKNOWN);
    ASSERT_TRUE(!pointerEvent->IsValid());

    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(-1);
    ASSERT_TRUE(!pointerEvent->IsValid());

    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(0);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    pointerEvent->AddPointerItem(item1);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    pointerEvent->AddPointerItem(item2);
    ASSERT_TRUE(!pointerEvent->IsValid());
    PointerEvent::PointerItem item3;
    item3.SetPointerId(0);
    pointerEvent->AddPointerItem(item3);
    PointerEvent::PointerItem item4;
    item4.SetPointerId(0);
    pointerEvent->AddPointerItem(item4);
    PointerEvent::PointerItem item5;
    item5.SetPointerId(0);
    pointerEvent->AddPointerItem(item5);
    PointerEvent::PointerItem item6;
    item6.SetPointerId(0);
    pointerEvent->AddPointerItem(item6);

    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetButtonPressed(PointerEvent::BUTTON_NONE);
    pointerEvent1->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent1->SetButtonPressed(PointerEvent::MOUSE_BUTTON_RIGHT);
    pointerEvent1->SetButtonPressed(PointerEvent::MOUSE_BUTTON_MIDDLE);
    item1.SetPointerId(0);
    pointerEvent1->AddPointerItem(item1);
    ASSERT_TRUE(!pointerEvent1->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_002
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_002, TestSize.Level1)
{
    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetButtonPressed(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    pointerEvent1->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_UNKNOWN);
    item.SetPointerId(0);
    pointerEvent2->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent2->IsValid());

    auto pointerEvent3 = PointerEvent::Create();
    ASSERT_NE(pointerEvent3, nullptr);
    pointerEvent3->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent3->SetPointerId(0);
    pointerEvent3->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent3->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    pointerEvent3->SetButtonId(PointerEvent::BUTTON_NONE);
    item.SetPointerId(0);
    pointerEvent3->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent3->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_003
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_003, TestSize.Level1)
{
    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent1->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent1->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    pointerEvent1->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent2->SetButtonId(PointerEvent::BUTTON_NONE);
    item.SetPointerId(-1);
    pointerEvent2->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent2->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_004
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_004, TestSize.Level1)
{
    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent1->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent1->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(2);
    pointerEvent1->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent2->SetButtonId(PointerEvent::BUTTON_NONE);
    item.SetPointerId(0);
    item.SetDownTime(10010);
    pointerEvent2->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent2->IsValid());

    auto pointerEvent3 = PointerEvent::Create();
    ASSERT_NE(pointerEvent3, nullptr);
    pointerEvent3->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent3->SetPointerId(0);
    pointerEvent3->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent3->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent3->SetButtonId(PointerEvent::BUTTON_NONE);
    item.SetPointerId(0);
    item.SetDownTime(0);
    item.SetPressed(true);
    pointerEvent3->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent3->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_005
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_005, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(0);
    item.SetPressed(false);
    pointerEvent->AddPointerItem(item);
    ASSERT_TRUE(pointerEvent->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckMousePointEvent_006
 * @tc.desc: Verify mouse point event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckMousePointEvent_006, TestSize.Level1)
{
    auto inputEvent = InputEvent::Create();
    ASSERT_NE(inputEvent, nullptr);
    inputEvent->SetDeviceId(1);
    inputEvent->SetTargetWindowId(1);
    inputEvent->SetAgentWindowId(1);
    auto event = PointerEvent::from(inputEvent);
    ASSERT_EQ(event, nullptr);

    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->Reset();
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetDeviceId(inputEvent->GetDeviceId());
    pointerEvent->SetTargetWindowId(inputEvent->GetTargetWindowId());
    pointerEvent->SetAgentWindowId(inputEvent->GetAgentWindowId());
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDownTime(0);
    item1.SetPressed(true);
    item1.SetWindowX(10);
    item1.SetWindowY(10);
    item1.SetDeviceId(inputEvent->GetDeviceId());
    item1.SetRawDx(60);
    item1.SetRawDy(60);
    pointerEvent->AddPointerItem(item1);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(1);
    item2.SetDownTime(0);
    item2.SetPressed(false);
    item2.SetWindowX(item1.GetWindowX());
    item2.SetWindowY(item1.GetWindowY());
    item2.SetDeviceId(inputEvent->GetDeviceId());
    item2.SetRawDx(100);
    item2.SetRawDy(100);
    pointerEvent->AddPointerItem(item2);
    ASSERT_TRUE(pointerEvent != nullptr);
#ifdef OHOS_BUILD_ENABLE_POINTER
    SimulateInputEventUtilTest(pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_001
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(-1);
    ASSERT_TRUE(!pointerEvent->IsValid());

    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_TRUE(!pointerEvent->IsValid());

    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetPointerAction(PointerEvent::POINTER_ACTION_UNKNOWN);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent2->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_TRUE(!pointerEvent2->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_002
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_002, TestSize.Level1)
{
    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent1->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(-1);
    pointerEvent1->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent2->SetButtonId(PointerEvent::BUTTON_NONE);
    item.SetPointerId(0);
    item.SetDownTime(0);
    item.SetPressed(false);
    pointerEvent2->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent2->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_003
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_003, TestSize.Level1)
{
    auto pointerEvent1 = PointerEvent::Create();
    ASSERT_NE(pointerEvent1, nullptr);
    pointerEvent1->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent1->SetPointerId(0);
    pointerEvent1->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent1->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(100);
    item.SetPressed(true);
    pointerEvent1->AddPointerItem(item);
    ASSERT_TRUE(!pointerEvent1->IsValid());

    auto pointerEvent2 = PointerEvent::Create();
    ASSERT_NE(pointerEvent2, nullptr);
    pointerEvent2->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent2->SetPointerId(0);
    pointerEvent2->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent2->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDownTime(100);
    item1.SetPressed(false);
    pointerEvent2->AddPointerItem(item1);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(0);
    item2.SetDownTime(100);
    item2.SetPressed(false);
    pointerEvent2->AddPointerItem(item2);
    ASSERT_TRUE(!pointerEvent2->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_004
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_004, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDownTime(100);
    item1.SetPressed(false);
    pointerEvent->AddPointerItem(item1);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(2);
    item2.SetDownTime(100);
    item2.SetPressed(false);
    pointerEvent->AddPointerItem(item2);
    ASSERT_TRUE(!pointerEvent->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_005
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_005, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item1;
    item1.SetPointerId(0);
    item1.SetDownTime(100);
    item1.SetPressed(false);
    pointerEvent->AddPointerItem(item1);
    PointerEvent::PointerItem item2;
    item2.SetPointerId(1);
    item2.SetDownTime(100);
    item2.SetPressed(false);
    pointerEvent->AddPointerItem(item2);
    ASSERT_TRUE(pointerEvent->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchPointEvent_006
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require: I5QSN3
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchPointEvent_006, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(100);
    item.SetToolDisplayX(90);
    item.SetToolDisplayY(90);
    item.SetToolWindowX(50);
    item.SetToolWindowY(50);
    item.SetToolWidth(30);
    item.SetToolHeight(30);
    item.SetLongAxis(100);
    item.SetShortAxis(20);
    item.SetToolType(2);
    item.SetTargetWindowId(0);
    pointerEvent->AddPointerItem(item);
    ASSERT_TRUE(pointerEvent->IsValid());
    DumpWindowData(pointerEvent);
    pointerEvent->RemovePointerItem(0);
    pointerEvent->IsButtonPressed(0);
    pointerEvent->ClearButtonPressed();
    pointerEvent->ClearAxisValue();
    pointerEvent->DeleteReleaseButton(PointerEvent::BUTTON_NONE);
    ASSERT_FALSE(pointerEvent->IsValid());
}

/**
 * @tc.name: PointerEventTest_CheckTouchInputEvent_001
 * @tc.desc: Verify touch screen event
 * @tc.type: FUNC
 * @tc.require: I5QSN3
 */
HWTEST_F(PointerEventTest, PointerEventTest_CheckTouchInputEvent_001, TestSize.Level1)
{
    auto inputEvent = InputEvent::Create();
    ASSERT_NE(inputEvent, nullptr);
    inputEvent->SetTargetDisplayId(0);
    inputEvent->SetDeviceId(0);
    inputEvent->EventTypeToString(InputEvent::EVENT_TYPE_POINTER);
    inputEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);
    inputEvent->ClearFlag();
}

/**
 * @tc.name: PointerEventTest_SetEnhanceData_001
 * @tc.desc: Set the enhance data.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetEnhanceData_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(-1);
    pointerEvent->AddPointerItem(item);
    uint32_t enHanceDataLen = 3;
    uint8_t enhanceDataBuf[enHanceDataLen];
    std::vector<uint8_t> enhanceData;
    for (uint32_t i = 0; i < enHanceDataLen; i++) {
        enhanceData.push_back(enhanceDataBuf[i]);
    }
    #ifdef OHOS_BUILD_ENABLE_SECURITY_COMPONENT
    ASSERT_NO_FATAL_FAILURE(pointerEvent->SetEnhanceData(enhanceData));
    ASSERT_EQ(pointerEvent->GetEnhanceData(), enhanceData);
    #endif // OHOS_BUILD_ENABLE_SECURITY_COMPONENT
}

/**
 * @tc.name: PointerEventTest_SetToolDisplayX_001
 * @tc.desc: Set Tool Display Coordinates.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetToolDisplayX_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    int32_t displayX = 90;
    PointerEvent::PointerItem item;
    item.SetPointerId(1);
    item.SetDownTime(0);
    ASSERT_NO_FATAL_FAILURE(item.SetToolDisplayX(displayX));
    ASSERT_EQ(item.GetToolDisplayX(), displayX);
    pointerEvent->AddPointerItem(item);
}

/**
 * @tc.name: PointerEventTest_SetToolDisplayY_001
 * @tc.desc: Set Tool Display Coordinates.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetToolDisplayY_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    int32_t displayY = 70;
    PointerEvent::PointerItem item;
    item.SetPointerId(2);
    item.SetDownTime(1);
    ASSERT_NO_FATAL_FAILURE(item.SetToolDisplayY(displayY));
    ASSERT_EQ(item.GetToolDisplayY(), displayY);
    pointerEvent->AddPointerItem(item);
}

/**
 * @tc.name: PointerEventTest_SetToolWidth_001
 * @tc.desc: Set Tool Display Width.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetToolWidth_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t toolWidth = 30;
    PointerEvent::PointerItem item;
    item.SetPointerId(3);
    item.SetDownTime(0);
    ASSERT_NO_FATAL_FAILURE(item.SetToolWidth(toolWidth));
    ASSERT_EQ(item.GetToolWidth(), toolWidth);
}

/**
 * @tc.name: PointerEventTest_SetToolHeight_001
 * @tc.desc: Set Tool Display Height.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetToolHeight_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t toolHeight = 40;
    PointerEvent::PointerItem item;
    item.SetPointerId(4);
    item.SetDownTime(1);
    ASSERT_NO_FATAL_FAILURE(item.SetToolHeight(toolHeight));
    ASSERT_EQ(item.GetToolHeight(), toolHeight);
}

/**
 * @tc.name: PointerEventTest_SetLongAxis_001
 * @tc.desc: Sets the long axis of the touch point area.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetLongAxis_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t longAxis = 50;
    PointerEvent::PointerItem item;
    item.SetPointerId(5);
    item.SetDownTime(0);
    ASSERT_NO_FATAL_FAILURE(item.SetLongAxis(longAxis));
    ASSERT_EQ(item.GetLongAxis(), longAxis);
}

/**
 * @tc.name: PointerEventTest_SetShortAxis_001
 * @tc.desc: Sets the short axis of the touch point area.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetShortAxis_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t shortAxis = 45;
    PointerEvent::PointerItem item;
    item.SetPointerId(6);
    item.SetDownTime(1);
    ASSERT_NO_FATAL_FAILURE(item.SetShortAxis(shortAxis));
    ASSERT_EQ(item.GetShortAxis(), shortAxis);
}

/**
 * @tc.name: PointerEventTest_SetRawDx_001
 * @tc.desc: Sets the raw X coordinate.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetRawDx_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t rawDx = 55;
    PointerEvent::PointerItem item;
    item.SetPointerId(7);
    item.SetDownTime(0);
    ASSERT_NO_FATAL_FAILURE(item.SetRawDx(rawDx));
    ASSERT_EQ(item.GetRawDx(), rawDx);
}

/**
 * @tc.name: PointerEventTest_SetRawDy_001
 * @tc.desc: Sets the raw Y coordinate.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetRawDy_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t rawDy = 60;
    PointerEvent::PointerItem item;
    item.SetPointerId(8);
    item.SetDownTime(1);
    ASSERT_NO_FATAL_FAILURE(item.SetRawDy(rawDy));
    ASSERT_EQ(item.GetRawDy(), rawDy);
}

/**
 * @tc.name: PointerEventTest_ClearFlag_001
 * @tc.desc: Clears all flags of an input event.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_ClearFlag_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto inputEvent = InputEvent::Create();
    ASSERT_NE(inputEvent, nullptr);
    inputEvent->SetTargetDisplayId(0);
    inputEvent->SetDeviceId(0);
    inputEvent->EventTypeToString(InputEvent::EVENT_TYPE_POINTER);
    inputEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);
    ASSERT_NO_FATAL_FAILURE(inputEvent->ClearFlag());
    ASSERT_EQ(inputEvent->GetFlag(), InputEvent::EVENT_FLAG_NONE);
}

/**
 * @tc.name: PointerEventTest_From_001
 * @tc.desc: Convert InputEvent to nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_From_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto inputEvent = InputEvent::Create();
    ASSERT_NE(inputEvent, nullptr);
    inputEvent->SetDeviceId(2);
    inputEvent->SetTargetWindowId(2);
    inputEvent->SetAgentWindowId(2);
    auto event = PointerEvent::from(inputEvent);
    ASSERT_EQ(event, nullptr);
}

/**
 * @tc.name: PointerEventTest_Reset_001
 * @tc.desc: Reset pointer event.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_Reset_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_NO_FATAL_FAILURE(pointerEvent->Reset());
    ASSERT_EQ(pointerEvent->GetSourceType(), PointerEvent::SOURCE_TYPE_UNKNOWN);
    ASSERT_EQ(pointerEvent->GetPointerId(), -1);
    ASSERT_EQ(pointerEvent->GetPointerAction(), PointerEvent::POINTER_ACTION_UNKNOWN);
}

/**
 * @tc.name: PointerEventTest_IsButtonPressed_001
 * @tc.desc: Determine whether the button is pressed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_IsButtonPressed_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    pointerEvent->SetButtonPressed(0);
    ASSERT_TRUE(pointerEvent->IsButtonPressed(0));
}

/**
 * @tc.name: PointerEventTest_DeleteReleaseButton_001
 * @tc.desc: Deletes a released button.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_DeleteReleaseButton_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    pointerEvent->SetButtonPressed(0);
    ASSERT_NO_FATAL_FAILURE(pointerEvent->DeleteReleaseButton(0));
    std::set<int32_t> pressButtons = pointerEvent->GetPressedButtons();
    ASSERT_EQ(pressButtons.size(), 0);
}

/**
 * @tc.name: PointerEventTest_ClearButtonPressed_001
 * @tc.desc: Clears the button in the pressed state.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_ClearButtonPressed_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    pointerEvent->SetButtonPressed(0);
    ASSERT_NO_FATAL_FAILURE(pointerEvent->ClearButtonPressed());
    std::set<int32_t> pressButtons = pointerEvent->GetPressedButtons();
    ASSERT_EQ(pressButtons.size(), 0);
}

/**
 * @tc.name: PointerEventTest_ClearAxisValue_001
 * @tc.desc: Clears the button in the pressed state.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_ClearAxisValue_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL, 30.0);
    double axisValue = pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL);
    ASSERT_EQ(axisValue, 30.0);
    ASSERT_NO_FATAL_FAILURE(pointerEvent->ClearAxisValue());
    ASSERT_EQ(pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL), 0);
}

/**
 * @tc.name: PointerEventTest_SetZorderValue_001
 * @tc.desc: Sets the zOrder for this event, inject to windows whose zOrder less than the target zOrder.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_SetZorderValue_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetZOrder(30.0);
    float zOrder = pointerEvent->GetZOrder();
    ASSERT_EQ(zOrder, 30.0);
}

/**
 * @tc.name: PointerEventTest_IsValid_001
 * @tc.desc: Checks whether this input event is valid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PointerEventTest, PointerEventTest_IsValid_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    ASSERT_FALSE(pointerEvent->IsValid());
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    ASSERT_FALSE(pointerEvent->IsValid());
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetButtonId(PointerEvent::BUTTON_NONE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(0);
    item.SetPressed(false);
    pointerEvent->AddPointerItem(item);
    ASSERT_TRUE(pointerEvent->IsValid());
}
} // namespace MMI
} // namespace OHOS
