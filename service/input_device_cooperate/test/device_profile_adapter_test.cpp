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

#include "softbus_bus_center.h"

namespace OHOS {
namespace MMI {
using namespace testing::ext;

class DPAdapterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

HWTEST_F(DPAdapterTest, UpdateCrossingSwitchState001, TestSize.Level1)
{
    int32_t rest = DProfileAdapter->UpdateCrossingSwitchState(true);
    std::cout << "rest: " << rest << std::endl;
    EXPECT_EQ(rest, 0);
}

HWTEST_F(DPAdapterTest, GetCrossingSwitchState001, TestSize.Level1)
{
    std::string deviceId = "";
    bool state = false;
    state = DProfileAdapter->GetCrossingSwitchState(deviceId);
    std::cout << "state: " << state << std::endl;
    EXPECT_EQ(state, true);
}

HWTEST_F(DPAdapterTest, RegisterCrossingStateListener001, TestSize.Level1)
{
    NodeBasicInfo *info = NULL;
    int32_t infoNum = 0;
    GetAllNodeDeviceInfo("ohos.multimodalinput.input", &info, &infoNum);
    std::cout << "DeviceInfo::" << std::endl;
    for (int32_t i = 0; i < infoNum; i++) {
        std::cout << "other dev networkId::" << std::endl;
        std::cout << info->networkId << std::endl;
        std::cout << "other dev deviceName::" << std::endl;
        std::cout << info->deviceName << std::endl;
        std::cout << std::endl;
        info++;
    }
    std::string networkId;
    std::vector<std::string> ids;
    std::cout << "NetworkId::enter q to stop";
    while (networkId != "q") {
        std::cin >> networkId;
        ids.emplace_back(networkId);
    }
    int32_t regisRet =
        DProfileAdapter->RegisterCrossingStateListener(networkId, [](const std::string &deviceid, bool state) {
            std::cout << "OnState Changed : " << state << "deviceId: " << deviceid << std::endl;
        });
    EXPECT_EQ(regisRet, 0);
    DProfileAdapter->UpdateCrossingSwitchState(false, ids);
    std::cout << "Make Sure that Get the State Change 0 and press any key to be continue" << std::endl;
    std::string wait;
    std::cin >> wait;
    DProfileAdapter->UpdateCrossingSwitchState(true, ids);
    std::cout << "Make Sure that Get the State Change 1 and press any key to be continue" << std::endl;
    std::cin >> wait;
    (void)wait;
}
} // namespace MMI
} // namespace OHOS