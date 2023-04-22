/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
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

#include "event_log_helper.h"
#include "input_handler_type.h"
#include "key_command_handler.h"
#include "input_event_handler.h"
#include "mmi_log.h"
#include "multimodal_event_handler.h"
#include "system_info.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyCommandHandlerTest" };
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t SEC_TO_NANOSEC = 1000000000;
} // namespace
class KeyCommandHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    std::shared_ptr<PointerEvent> SetupPointerEvent001();
    std::shared_ptr<PointerEvent> SetupPointerEvent002();
    std::shared_ptr<KeyEvent> SetupKeyEvent001();
};

int64_t GetNanoTime()
{
    struct timespec time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);
    return static_cast<int64_t>(time.tv_sec) * SEC_TO_NANOSEC + time.tv_nsec;
}

std::shared_ptr<PointerEvent> KeyCommandHandlerTest::SetupPointerEvent001()
{
    auto pointerEvent = PointerEvent::Create();
    CHKPP(pointerEvent);
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerItem.SetDisplayX(523);
    pointerItem.SetDisplayY(723);
    pointerItem.SetPressure(5);
    pointerItem.SetDeviceId(1);
    pointerEvent->AddPointerItem(pointerItem);

    pointerItem.SetPointerId(1);
    pointerItem.SetDisplayX(610);
    pointerItem.SetDisplayY(910);
    pointerItem.SetPressure(7);
    pointerItem.SetDeviceId(1);
    pointerEvent->AddPointerItem(pointerItem);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

std::shared_ptr<PointerEvent> KeyCommandHandlerTest::SetupPointerEvent002()
{
    auto pointerEvent = PointerEvent::Create();
    CHKPP(pointerEvent);
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerItem.SetDisplayX(523);
    pointerItem.SetDisplayY(723);
    pointerItem.SetPressure(5);
    pointerItem.SetDeviceId(1);
    pointerEvent->AddPointerItem(pointerItem);

    pointerItem.SetPointerId(1);
    pointerItem.SetDisplayX(610);
    pointerItem.SetDisplayY(910);
    pointerItem.SetPressure(7);
    pointerItem.SetDeviceId(1);
    pointerEvent->AddPointerItem(pointerItem);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

std::shared_ptr<KeyEvent> KeyCommandHandlerTest::SetupKeyEvent001()
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    CHKPP(keyEvent);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_HOME);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    keyEvent->SetKeyCode(KeyEvent::KEYCODE_HOME);
    keyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    keyEvent->AddPressedKeyItems(kitDown);

    return keyEvent;
}

/**
 * @tc.name: KeyCommandHandlerTest_001
 * @tc.desc: Test update key down duration 0, 100, 4000
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_001, TestSize.Level1)
{
    KeyCommandHandler eventKeyCommandHandler;
    std::string businessId = "aaa";
    int32_t delay = 0;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
    delay = 100;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
    delay = 4000;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
}

/**
 * @tc.name: KeyCommandHandlerTest_002
 * @tc.desc: Test update key down duration -1 and 4001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_002, TestSize.Level1)
{
    KeyCommandHandler eventKeyCommandHandler;
    std::string businessId = "com.ohos.camera";
    int32_t delay = -1;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
    delay = 4001;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
}

/**
 * @tc.name: KeyCommandHandlerTest_003
 * @tc.desc: Test businessId is ""
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_003, TestSize.Level1)
{
    KeyCommandHandler eventKeyCommandHandler;
    std::string businessId = "";
    int32_t delay = 100;
    ASSERT_EQ(RET_ERR, eventKeyCommandHandler.UpdateSettingsXml(businessId, delay));
}

/**
 * @tc.name: KeyCommandHandlerTest_004
 * @tc.desc: Test key event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = SetupKeyEvent001();
    ASSERT_TRUE(keyEvent != nullptr);
    KeyCommandHandler eventKeyCommandHandler;
    ASSERT_FALSE(eventKeyCommandHandler.OnHandleEvent(keyEvent));
}

#ifdef OHOS_BUILD_ENABLE_TOUCH
/**
 * @tc.name: KeyCommandHandlerTest_005
 * @tc.desc: Test two fingers touchDown event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<PointerEvent> pointerEvent { SetupPointerEvent001() };
    ASSERT_TRUE(pointerEvent != nullptr);
    KeyCommandHandler eventKeyCommandHandler;
    eventKeyCommandHandler.HandleTouchEvent(pointerEvent);
}

/**
 * @tc.name: KeyCommandHandlerTest_006
 * @tc.desc: Test two fingers move event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_006, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<PointerEvent> pointerEvent { SetupPointerEvent002() };
    ASSERT_TRUE(pointerEvent != nullptr);
    KeyCommandHandler eventKeyCommandHandler;
    eventKeyCommandHandler.HandleTouchEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

} // namespace MMI
} // namespace OHOS