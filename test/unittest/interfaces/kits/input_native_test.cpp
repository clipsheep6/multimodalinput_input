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

#include <gtest/gtest.h>

#include "oh_input_manager.h"
#include "oh_key_code.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class InputNativeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: InputNativeTest_CreateKeyState_001
 * @tc.desc: Verify create key states
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputNativeTest, InputNativeTest_CreateKeyState_001, TestSize.Level1)
{
    // auto keyState = OH_Input_CreateKeyState();
    // ASSERT_NE(keyState, nullptr);
}
} // namespace MMI
} // namespace OHOS
