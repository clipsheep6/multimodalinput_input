/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "key_command_handler.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace
class KeyCommandHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: KeyCommandHandlerTest_001
 * @tc.desc: test updateSettingsXml: 0 < delay < 4000 
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_001, TestSize.Level1)
{
    int32_t delay = 100;
    ASSERT_EQ(RET_OK, KeyCommandHandler::UpdateSettingsXml("a", delay));
}

/**
 * @tc.name: KeyCommandHandlerTest_002
 * @tc.desc: test updateSettingsXml: delay = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_002, TestSize.Level1)
{
    int32_t delay = 0;
    ASSERT_EQ(RET_OK, KeyCommandHandler::UpdateSettingsXml("a", delay));
}

/**
 * @tc.name: KeyCommandHandlerTest_003
 * @tc.desc: test updateSettingsXml: delay < 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_003, TestSize.Level1)
{
    int32_t delay = -100;
    ASSERT_EQ(RET_ERR, KeyCommandHandler::UpdateSettingsXml("a", -100));
}

/**
 * @tc.name: KeyCommandHandlerTest_004
 * @tc.desc: test updateSettingsXml: delay > 4000 
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeyCommandHandlerTest, KeyCommandHandlerTest_003, TestSize.Level1)
{
    int32_t delay = -100;
    ASSERT_EQ(RET_ERR, KeyCommandHandler::UpdateSettingsXml("a", -100));
}
} //namespace MMI
} // namespace OHOS