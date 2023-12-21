/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "touchpad_transform_processor.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class TouchPadTransformProcessorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

private:
    TouchPadTransformProcessor g_processor_ { 0 };
    bool prePinchSwitch_ { true };
    bool preSwipeSwitch_ { true };
};

void TouchPadTransformProcessorTest::SetUpTestCase(void)
{
}

void TouchPadTransformProcessorTest::TearDownTestCase(void)
{
}

void TouchPadTransformProcessorTest::SetUp()
{
    g_processor_.GetTouchpadPinchSwitch(prePinchSwitch_);
    g_processor_.GetTouchpadSwipeSwitch(preSwipeSwitch_);
}

void TouchPadTransformProcessorTest::TearDown()
{
    g_processor_.SetTouchpadPinchSwitch(prePinchSwitch_);
    g_processor_.SetTouchpadSwipeSwitch(preSwipeSwitch_);
}

/**
 * @tc.name: TouchPadTransformProcessorTest_SetTouchpadPinchSwitch_01
 * @tc.desc: Test SetTouchpadPinchSwitch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TouchPadTransformProcessorTest, TouchPadTransformProcessorTest_SetTouchpadPinchSwitch_01, TestSize.Level1)
{
    int32_t deviceId = 6;
    TouchPadTransformProcessor processor(deviceId);
    bool flag = false;
    ASSERT_TRUE(processor.SetTouchpadPinchSwitch(flag) == RET_OK);
}

/**
 * @tc.name: TouchPadTransformProcessorTest_GetTouchpadPinchSwitch_02
 * @tc.desc: Test GetTouchpadPinchSwitch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TouchPadTransformProcessorTest, TouchPadTransformProcessorTest_GetTouchpadPinchSwitch_02, TestSize.Level1)
{
    int32_t deviceId = 6;
    TouchPadTransformProcessor processor(deviceId);
    bool flag = false;
    processor.SetTouchpadPinchSwitch(flag);
    bool newFlag = false;
    ASSERT_TRUE(processor.GetTouchpadPinchSwitch(flag) == RET_OK);
    ASSERT_TRUE(flag == newFlag);
}

/**
 * @tc.name: TouchPadTransformProcessorTest_SetTouchpadSwipeSwitch_03
 * @tc.desc: Test SetTouchpadSwipeSwitch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TouchPadTransformProcessorTest, TouchPadTransformProcessorTest_SetTouchpadSwipeSwitch_03, TestSize.Level1)
{
    int32_t deviceId = 6;
    TouchPadTransformProcessor processor(deviceId);
    bool flag = false;
    ASSERT_TRUE(processor.SetTouchpadSwipeSwitch(flag) == RET_OK);
}

/**
 * @tc.name: TouchPadTransformProcessorTest_GetTouchpadSwipeSwitch_04
 * @tc.desc: Test GetTouchpadSwipeSwitch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TouchPadTransformProcessorTest, TouchPadTransformProcessorTest_GetTouchpadSwipeSwitch_04, TestSize.Level1)
{
    int32_t deviceId = 6;
    TouchPadTransformProcessor processor(deviceId);
    bool flag = false;
    processor.SetTouchpadSwipeSwitch(flag);
    bool newFlag = false;
    ASSERT_TRUE(processor.GetTouchpadSwipeSwitch(flag) == RET_OK);
    ASSERT_TRUE(flag == newFlag);
}
}
}