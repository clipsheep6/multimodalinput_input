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
#include "crown_event_normalize.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class CrownEventNormalizeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

private:
};

void CrownEventNormalizeTest::SetUpTestCase(void)
{
}

void CrownEventNormalizeTest::TearDownTestCase(void)
{
}

void CrownEventNormalizeTest::SetUp()
{
}

void CrownEventNormalizeTest::TearDown()
{
}

/**
 * @tc.name: CrownEventNormalizeTest_IsCrownEvent_001
 * @tc.desc: Test IsCrownEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownEventNormalizeTest, CrownEventNormalizeTest_IsCrownEvent_001, TestSize.Level1)
{
    struct libinput_event *event = nullptr;
    ASSERT_NE(CROWNEVENTHDR->IsCrownEvent(event), false);
}

/**
 * @tc.name: CrownEventNormalizeTest_NormalizeKeyEvent_002
 * @tc.desc: Test NormalizeKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownEventNormalizeTest, CrownEventNormalizeTest_NormalizeKeyEvent_002, TestSize.Level1)
{
    struct libinput_event *event = nullptr;
    ASSERT_NE(CROWNEVENTHDR->NormalizeKeyEvent(event), RET_OK);
}

/**
 * @tc.name: CrownEventNormalizeTest_NormalizeRotateEvent_003
 * @tc.desc: Test NormalizeRotateEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownEventNormalizeTest, CrownEventNormalizeTest_NormalizeRotateEvent_003, TestSize.Level1)
{
    struct libinput_event *event = nullptr;
    ASSERT_NE(CROWNEVENTHDR->NormalizeRotateEvent(event), RET_OK);
}

/**
 * @tc.name: CrownEventNormalizeTest_Dump_004
 * @tc.desc: Test Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CrownEventNormalizeTest, CrownEventNormalizeTest_Dump_004, TestSize.Level1)
{
    std::vector<std::string> args;
    std::vector<std::string> idNames;
    int32_t fd = 0;
    CROWNEVENTHDR->Dump(fd, args);
    ASSERT_EQ(args, idNames);
}
}
}