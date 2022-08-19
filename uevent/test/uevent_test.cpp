/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <cctype>
#include <gtest/gtest.h>
#include <thread>
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
using namespace testing::ext;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventTest" };
} // namespace

class UeventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
};

void UeventTest::SetUpTestCase(void)
{}

void UeventTest::TearDownTestCase(void)
{}

HWTEST_F(UeventTest, TestFunction, TestSize.Level1)
{
    MMI_HILOGD("test");
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
