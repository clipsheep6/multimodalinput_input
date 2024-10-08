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

#include "manage_inject_device.h"
#include "msg_head.h"
#include "processing_pen_device.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class ProcessingPenDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    static void CheckJsonData(const std::string path)
    {
        std::string startDeviceCmd = "vuinput start touchpad & ";
        std::string closeDeviceCmd = "vuinput close all";
        FILE* startDevice = popen(startDeviceCmd.c_str(), "rw");
        if (!startDevice) {
            ASSERT_TRUE(false) << "Start device failed";
        }
        pclose(startDevice);
        std::string jsonBuf = ReadJsonFile(path);
        if (jsonBuf.empty()) {
            ASSERT_TRUE(false) << "Read file failed" << path;
        }
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(DataInit(jsonBuf, false));
        FILE* closeDevice = popen(closeDeviceCmd.c_str(), "rw");
        if (!closeDevice) {
            ASSERT_TRUE(false) << "Close device failed";
        }
        pclose(closeDevice);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        EXPECT_EQ(ret, RET_ERR);
    }
};

/**
 * @tc.name:Test_TransformPenJsonDataToInputData
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputData, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputData.json";
    std::string startDeviceCmd = "vuinput start touchpad & ";
    std::string closeDeviceCmd = "vuinput close all";
    FILE* startDevice = popen(startDeviceCmd.c_str(), "rw");
    if (!startDevice) {
        ASSERT_TRUE(false) << "Start device failed";
    }
    pclose(startDevice);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string jsonBuf = ReadJsonFile(path);
    if (jsonBuf.empty()) {
        ASSERT_TRUE(false) << "Read file failed" << path;
    }
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(DataInit(jsonBuf, false));
    FILE* closeDevice = popen(closeDeviceCmd.c_str(), "rw");
    if (!closeDevice) {
        ASSERT_TRUE(false) << "Close device failed";
    }
    pclose(closeDevice);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataNotfindEvents
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataNotfindEvents, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataNotfindEvents.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataEventsIsEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataEventsIsEmpty.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataApproachEventError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataApproachEventError, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataApprochEventError.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataSlideEventError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataSlideEventError, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataSlideEventError.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataLeaveEventError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataLeaveEventError, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataLeaveEventError.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataApproachEventEventTypeError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataApproachEventEventTypeError, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataApprochEventEventTypeError.json";
    CheckJsonData(path);
}

/**
 * @tc.name:Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError.json";
    CheckJsonData(path);
}
} // namespace MMI
} // namespace OHOS