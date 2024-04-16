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
#include "libinput.h"

#include "define_multimodal.h"
#include "gesture_transform_processor.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class GestureTransformProcessorTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDoen() {}
};

/**
 * @tc.name: GestureTransformProcessorTest
 * @tc.desc: Test GESTURE_SWIPE_BEGIN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GestureTransformProcessorTest, GESTURE_SWIPE_BEGIN, TestSize.Level1)
{
    int32_t deviceID = 1;
    GestureTransformProcessor processor(deviceID);
    libinput_event* event = nullptr;
    auto type = libinput_event_get_type(event);
    type = LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN;
    auto result = processor.OnEvent(event);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: GestureTransformProcessorTest
 * @tc.desc: Test GESTURE_SWIPE_UPDATE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GestureTransformProcessorTest, GESTURE_SWIPE_UPDATE, TestSize.Level1)
{
    int32_t deviceID = 1;
    GestureTransformProcessor processor(deviceID);
    libinput_event* event = nullptr;
    auto type = libinput_event_get_type(event);
    type = LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE;
    auto result = processor.OnEvent(event);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: GestureTransformProcessorTest
 * @tc.desc: Test GESTURE_SWIPE_END
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GestureTransformProcessorTest, GESTURE_SWIPE_END, TestSize.Level1)
{
    int32_t deviceID = 1;
    GestureTransformProcessor processor(deviceID);
    libinput_event* event = nullptr;
    auto type = libinput_event_get_type(event);
    type = LIBINPUT_EVENT_GESTURE_SWIPE_END;
    auto result = processor.OnEvent(event);
    ASSERT_EQ(result, nullptr);
}
}
}