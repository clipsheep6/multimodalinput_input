/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "msg_head.h"
#include "proto.h"
#include "manage_inject_device.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
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
    const std::string path = "/data/json/Test_TransformJsonDataCheckFileNotEmpty.json";
    std::string beginDeviceCmd = "vuinput open all & ";
    std::string afterDeviceCmd = "vuinput is closed";
    FILE* beginDevice = popen(beginDeviceCmd.c_str(), "rw");
    if (!beginDevice) {
        ASSERT_TRUE(false) << "open device failed";
    }
    pclose(beginDevice);
    std::string jsonBuffer = ReadJsonFile(path);
    if (jsonBuffer.empty()) {
        ASSERT_TRUE(false) << "Open file failed" << path;
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(DataInit(jsonBuffer, false));
    FILE* closeDevice = popen(afterDeviceCmd.c_str(), "rw");
    if (!closeDevice) {
        ASSERT_TRUE(false) << "Close local device failed";
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
    const std::string filePath = "/data/json/Test_TransformJsonDataGetDeviceNodeError.json";
    std::string startDeviceMsg = "vuinput start all & ";
    std::string closeDeviceMsg = "vuinput close all";
    FILE* startDevice = popen(startDeviceMsg.c_str(), "rw");
    if (!startDevice) {
        ASSERT_TRUE(false) << "Start device failed";
    }
    pclose(startDevice);
    std::string jsonBuf = ReadJsonFile(filePath);
    if (jsonBuf.empty()) {
        ASSERT_TRUE(false) << "Read file failed" << filePath;
    }
    ManageInjectDevice openInjectDevice;
    auto ret = openInjectDevice.TransformJsonData(DataInit(jsonBuf, false));
    FILE* closeDevice = popen(closeDeviceMsg.c_str(), "rw");
    if (!closeDevice) {
        ASSERT_TRUE(false) << "Close device failed";
    }
    pclose(closeDevice);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name:Test_SendEventToDeviceNodeError
 * @tc.desc:Verify ManageInjectDevice function SendEventToDeviceNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ManageInjectDeviceTest, Test_SendEventToDeviceNodeError, TestSize.Level1)
{
    ManageInjectDevice manageInjectDevice;
    InputEventArray inputEventArray = {};
    inputEventArray.target = "";
    auto ret = manageInjectDevice.SendEventToDeviceNode(inputEventArray);
    EXPECT_EQ(ret, RET_ERR);
}
} // namespace MMI
} // namespace OHOS