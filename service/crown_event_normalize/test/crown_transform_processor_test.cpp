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

#include <cstdio>
#include <gtest/gtest.h>

#include "libinput.h"
#include "crown_transform_processor.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class CrownTransformProcessorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

private:
    CrownTransformProcessor g_processor_ { 0 };
};

void CrownTransformProcessorTest::SetUpTestCase(void)
{
}

void CrownTransformProcessorTest::TearDownTestCase(void)
{
}

void CrownTransformProcessorTest::SetUp()
{
}

void CrownTransformProcessorTest::TearDown()
{
}

/**
 * @tc.name: CrownTransformProcessorTest_GetPointerEvent_001
 * @tc.desc: Test GetPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_GetPointerEvent_001, TestSize.Level1)
{
    int32_t deviceId = 0;
    CrownTransformProcessor processor(deviceId);
    auto ret = processor.GetPointerEvent();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: CrownTransformProcessorTest_NormalizeKeyEvent_002
 * @tc.desc: Test NormalizeKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_NormalizeKeyEvent_002, TestSize.Level1)
{
    int32_t deviceId = 0;
    struct libinput_event *event = nullptr;
    CrownTransformProcessor processor(deviceId);
    ASSERT_NE(processor.NormalizeKeyEvent(event), RET_OK);
}

/**
 * @tc.name: CrownTransformProcessorTest_NormalizeRotateEvent_003
 * @tc.desc: Test NormalizeRotateEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_NormalizeRotateEvent_003, TestSize.Level1)
{
    int32_t deviceId = 0;
    struct libinput_event *event = nullptr;
    CrownTransformProcessor processor(deviceId);
    ASSERT_NE(processor.NormalizeRotateEvent(event), RET_OK);
}

/**
 * @tc.name: CrownTransformProcessorTest_HandleCrownRotatePostInner_004
 * @tc.desc: Test HandleCrownRotatePostInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_HandleCrownRotatePostInner_004, TestSize.Level1)
{
    int32_t deviceId = 0;
    double angularVelocity = 0.0;
    double degree = 0.0;
    int32_t action = POINTER_ACTION_CROWN_ROTATE_BEGIN;
    CrownTransformProcessor processor(deviceId);
    ASSERT_NO_FATAL_FAILURE(processor.HandleCrownRotatePostInner(angularVelocity, degree, action));
}

/**
 * @tc.name: CrownTransformProcessorTest_DumpInner_005
 * @tc.desc: Test DumpInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_DumpInner_005, TestSize.Level1)
{
    int32_t deviceId = 0;
    CrownTransformProcessor processor(deviceId);
    ASSERT_NO_FATAL_FAILURE(processor.DumpInner());
}

/**
 * @tc.name: CrownTransformProcessorTest_Dump_005
 * @tc.desc: Test Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownTransformProcessorTest, CrownTransformProcessorTest_Dump_006, TestSize.Level1)
{
    std::vector<std::string> args;
    std::vector<std::string> idNames;
    int32_t deviceId = 0;
    CrownTransformProcessor processor(deviceId);
    int32_t fd = 0;
    processor.Dump(fd, args);
    ASSERT_EQ(args, idNames);
}
}
}