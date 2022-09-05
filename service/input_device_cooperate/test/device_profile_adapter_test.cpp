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
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

#include "device_profile_adapter.h"

namespace OHOS {
namespace MMI {
using namespace testing::ext;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DeviceProfileAdapterTest" };
} // namespace
class DPAdapterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

HWTEST_F(DPAdapterTest, test001, TestSize.Level1)
{
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true);
    MMI_HILOGD("Update crossing switch state :%{public}d", rest);
    EXPECT_NE(rest, 0);
}

HWTEST_F(DPAdapterTest, test002, TestSize.Level1)
{
    std::vector<std::string> deviceIds = {"123"};
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true, deviceIds);
    MMI_HILOGD("Update crossing switch state :%{public}d", rest);
    EXPECT_NE(rest, 0);
}

HWTEST_F(DPAdapterTest, test003, TestSize.Level1)
{
    std::string deviceId = "123";
    bool state = false;
    state = DProfileAdapter->GetCrossingSwitchState(deviceId);
    MMI_HILOGD("Get crossing switch state :%{public}d", state);
    EXPECT_NE(state, true);
}

HWTEST_F(DPAdapterTest, test004, TestSize.Level1)
{
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    ProfileEventCallback callback ;
    int32_t regisRet = DProfileAdapter->RegisterCrossingStateListener("123", callback);
    EXPECT_NE(regisRet, 0);
}

HWTEST_F(DPAdapterTest, test005, TestSize.Level1)
{
    int32_t regisRet = DProfileAdapter->UnregisterCrossingStateListener("123");
    EXPECT_EQ(regisRet, 0);
}
} // namespace MMI
} // namespace OHOS
