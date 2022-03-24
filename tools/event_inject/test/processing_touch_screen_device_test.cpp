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

#include "manage_inject_device.h"
#include "msg_head.h"
#include "processing_touch_screen_device.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
} // namespace

class ProcessingTouchScreenDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name:Test_TransformJsonDataToInputData
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputData, TestSize.Level1)
{
#ifdef OHOS_BUILD
    const std::string path = "/data/json/Test_TransformTouchScreenJsonDataToInputData.json";
    std::string startDeviceCmd = "mmi-virtual-device-manager start touchscreen & ";
    std::string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
    const std::string path = "temp/Test_TransformTouchScreenJsonDataToInputData.json";
    std::string startDeviceCmd = "./mmi-virtual-deviced.out start touchscreen &";
    std::string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    FILE* fp = fopen(path.c_str(), "r");
    if (fp == nullptr) {
        ASSERT_TRUE(false) << "can not open " << path;
    }
    char buf[256] = {};
    std::string jsonBuf;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        jsonBuf = jsonBuf + buf;
    }
    if (fclose(fp) < 0) {
        ASSERT_TRUE(false) << "close file error";
    }
    cJSON* inputEventArrays = cJSON_Parse(jsonBuf.c_str());
    if (inputEventArrays == nullptr) {
        ASSERT_TRUE(false) << "inputEventArrays is null";
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    cJSON_Delete(inputEventArrays);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name:Test_TransformJsonDataToInputDataEventsIsEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
#ifdef OHOS_BUILD
    const std::string path = "/data/json/Test_TransformJsonDataToInputDataEventsIsEmpty.json";
    std::string startDeviceCmd = "mmi-virtual-device-manager start touchscreen & ";
    std::string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
    const std::string path = "temp/Test_TransformJsonDataToInputDataEventsIsEmpty.json";
    std::string startDeviceCmd = "./mmi-virtual-deviced.out start touchscreen &";
    std::string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    FILE* fp = fopen(path.c_str(), "r");
    if (fp == nullptr) {
        ASSERT_TRUE(false) << "can not open " << path;
    }
    char buf[256] = {};
    std::string jsonBuf;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        jsonBuf = jsonBuf + buf;
    }
    if (fclose(fp) < 0) {
        ASSERT_TRUE(false) << "close file error";
    }
    cJSON* inputEventArrays = cJSON_Parse(jsonBuf.c_str());
    if (inputEventArrays == nullptr) {
        ASSERT_TRUE(false) << "inputEventArrays is null";
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    cJSON_Delete(inputEventArrays);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name:Test_TransformJsonDataToInputDataSingleEventsIsEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputDataSingleEventsIsEmpty, TestSize.Level1)
{
#ifdef OHOS_BUILD
    const std::string path = "/data/json/Test_TransformJsonDataToInputDataSingleEventsIsEmpty.json";
    std::string startDeviceCmd = "mmi-virtual-device-manager start touchscreen & ";
    std::string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
    const std::string path = "temp/Test_TransformJsonDataToInputDataSingleEventsIsEmpty.json";
    std::string startDeviceCmd = "./mmi-virtual-deviced.out start touchscreen &";
    std::string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    FILE* fp = fopen(path.c_str(), "r");
    if (fp == nullptr) {
        ASSERT_TRUE(false) << "can not open " << path;
    }
    char buf[256] = {};
    std::string jsonBuf;
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        jsonBuf = jsonBuf + buf;
    }
    if (fclose(fp) < 0) {
        ASSERT_TRUE(false) << "close file error";
    }
    cJSON* inputEventArrays = cJSON_Parse(jsonBuf.c_str());
    if (inputEventArrays == nullptr) {
        ASSERT_TRUE(false) << "inputEventArrays is null";
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    cJSON_Delete(inputEventArrays);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}
} // namespace MMI
} // namespace OHOS