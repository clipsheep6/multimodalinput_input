/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "msg_head.h"
#include "proto.h"
#define private public
#include "manage_inject_device.h"
#undef private

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
} // namespace
class ManageInjectDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name:Test_TransformJsonDataCheckFileIsEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ManageInjectDeviceTest, Test_TransformJsonDataCheckFileIsEmpty, TestSize.Level1)
{
    DeviceItems inputEventArrays;
    inputEventArrays.clear();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name:Test_TransformJsonDataCheckFileNotEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ManageInjectDeviceTest, Test_TransformJsonDataCheckFileNotEmpty, TestSize.Level1)
{
#ifdef OHOS_BUILD
    const std::string path = "/data/json/Test_TransformJsonDataCheckFileNotEmpty.json";
    std::string startDeviceCmd = "mmi-virtual-device-manager start all & ";
    std::string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
    const std::string path = "temp/Test_TransformJsonDataCheckFileNotEmpty.json";
    std::string startDeviceCmd = "./mmi-virtual-deviced.out start all &";
    std::string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
    FILE* startDevice = popen(startDeviceCmd.c_str(), "rw");
    if (!startDevice) {
        ASSERT_TRUE(false) << "start device failed";
    }
    pclose(startDevice);
    std::string jsonBuf = ReadJsonFile(path);
    if (jsonBuf.empty()) {
        ASSERT_TRUE(false) << "read file failed" << path;
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(DataInit(jsonBuf, false));
    FILE* closeDevice = popen(closeDeviceCmd.c_str(), "rw");
    if (!closeDevice) {
        ASSERT_TRUE(false) << "close device failed";
    }
    pclose(closeDevice);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name:Test_TransformJsonDataGetDeviceNodeError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ManageInjectDeviceTest, Test_TransformJsonDataGetDeviceNodeError, TestSize.Level1)
{
#ifdef OHOS_BUILD
    const std::string path = "/data/json/Test_TransformJsonDataGetDeviceNodeError.json";
    std::string startDeviceCmd = "mmi-virtual-device-manager start all & ";
    std::string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
    const std::string path = "temp/Test_TransformJsonDataGetDeviceNodeError.json";
    std::string startDeviceCmd = "./mmi-virtual-deviced.out start all &";
    std::string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
    FILE* startDevice = popen(startDeviceCmd.c_str(), "rw");
    if (!startDevice) {
        ASSERT_TRUE(false) << "start device failed";
    }
    pclose(startDevice);
    std::string jsonBuf = ReadJsonFile(path);
    if (jsonBuf.empty()) {
        ASSERT_TRUE(false) << "read file failed" << path;
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(DataInit(jsonBuf, false));
    FILE* closeDevice = popen(closeDeviceCmd.c_str(), "rw");
    if (!closeDevice) {
        ASSERT_TRUE(false) << "close device failed";
    }
    pclose(closeDevice);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name:Test_SendEventToDeviveNodeError
 * @tc.desc:Verify ManageInjectDevice function SendEventToDeviveNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ManageInjectDeviceTest, Test_SendEventToDeviveNodeError, TestSize.Level1)
{
    ManageInjectDevice manageInjectDevice;
    InputEventArray inputEventArray = {};
    inputEventArray.target = "";
    auto ret = manageInjectDevice.SendEventToDeviveNode(inputEventArray);
    EXPECT_EQ(ret, RET_ERR);
}
} // namespace MMI
} // namespace OHOS