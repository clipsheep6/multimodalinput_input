/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>

#include "libinput.h"
#include "define_multimodal.h"
#include "joystick_transform_processor.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class JoystickTransformProcessorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

HWTEST_F(JoystickTransformProcessorTest, OnEventJoystickButtonTest, TestSize.Level1)
{
    int32_t deviceID = 123;
    JoystickTransformProcessor processor(deviceID);

    libinput_event* event = nullptr;
    auto data = libinput_event_get_joystick_button_event(event);

    EXPECT_FALSE(processor.OnEvent(nullptr));

    EXPECT_FALSE(processor.OnEvent(event));

    auto state = libinput_event_joystick_button_get_key_state(data);
    state = LIBINPUT_BUTTON_STATE_RELEASED;
    EXPECT_TRUE(processor.OnEvent(event));
    
    state = LIBINPUT_BUTTON_STATE_PRESSED;
    EXPECT_TRUE(processor.OnEvent(event));

}

HWTEST_F(JoystickTransformProcessorTest, OnEventJoystickAxisTest, TestSize.Level1)
{
    int32_t deviceID = 123;
    JoystickTransformProcessor processor(deviceID);
    struct libinput_event* event = new libinput_event();
    struct libinput_event_get_joystick_axis_event* axisEvent = new libinput_event_get_joystick_axis_event();
    axisEvent->value = 100;
    event->libinput_event_get_joystick_axis_event = axisEvent;

    bool result = jtProcessor.OnEventJoystickAxis(event);
    EXPECT_TRUE(result);

    EXPECT_EQ(event->GetActionTime(), GetSysClockTime());
    EXPECT_EQ(event->GetActionStartTime(), GetSysClockTime());
    EXPECT_EQ(event->GetDeviceId(), jtProcessor.deviceId_);
    EXPECT_EQ(event->GetPointerAction(), PointerEvent::POINTER_ACTION_AXIS_UPDATE);
    EXPECT_EQ(event->GetSourceType(), PointerEvent::SOURCE_TYPE_JOYSTICK);
    EXPECT_EQ(event->GetAxisValue(item.second), axisEvent->value);

    delete event;
    delete axisEvent;
}
}
}