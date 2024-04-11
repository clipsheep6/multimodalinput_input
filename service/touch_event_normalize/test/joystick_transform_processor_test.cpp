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

void JoystickTransformProcessorTest::SetUpTestCase(void)
{
}

void JoystickTransformProcessorTest::TearDownTestCase(void)
{
}

void JoystickTransformProcessorTest::SetUp()
{
}

void JoystickTransformProcessorTest::TearDown()
{
}

/**
 * @tc.name: JoystickTransformProcessorTest_ButtonEvent
 * @tc.desc: Test ButtonEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(JoystickTransformProcessorTest, OnEvent_ButtonEvent, TestSize.Level1)
{
    int32_t deviceID = 1;
    JoystickTransformProcessor processor(deviceID);
    libinput_event* event = nullptr;
    auto type = libinput_event_get_type(event);
    type = LIBINPUT_EVENT_JOYSTICK_BUTTON;
    auto result = processor.OnEvent(event);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: JoystickTransformProcessorTest_AxisEvent
 * @tc.desc: Test AxisEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(JoystickTransformProcessorTest, OnEvent_AxisEvent, TestSize.Level1)
{
    int32_t deviceID = 1;
    JoystickTransformProcessor processor(deviceID);
    libinput_event* event = nullptr;
    auto type = libinput_event_get_type(event);
    type = LIBINPUT_EVENT_JOYSTICK_AXIS;
    auto result = processor.OnEvent(event);
    ASSERT_EQ(result, nullptr);
}
}
}