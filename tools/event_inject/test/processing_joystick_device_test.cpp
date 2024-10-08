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
#include "processing_joystick_device.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class ProcessingJoyStickDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    static void CheckTransformJson(const std::string path)
    {
        std::string startDeviceCmd = "vuinput start joystick & ";
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
    static void CheckJoyStickJson(const std::string path)
    {
        std::string startDeviceCmd = "vuinput start joystick & ";
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
 * @tc.name:Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRockerDirectionKey
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJsonDataToInputData, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputData.json";
    CheckTransformJson(path);
}

/**
 * @tc.name:Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRockerDirectionKey
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindEvents, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindEvents.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataEventsIsEmpty
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataEventsIsEmpty.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindKeyValue
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindKeyValue, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindKeyValue.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindKeyValueInThro
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindKeyValueInThro, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindKeyValueInThro.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindEventInRocker
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindEventInRocker, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindEventInRocker.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInRocker
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInRocker,
         TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInRocker.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInDirectionKey
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInDirectionKey,
         TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInDirectionKey.json";
    CheckJoyStickJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataNotFindAnyEventType
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindAnyEventType, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataNotFindAnyEventType.json";
    CheckTransformJson(path);
}

/**
 * @tc.name:Test_TransformJoyStickJsonDataToInputDataHasBlockTime
 * @tc.desc:Verify ManageInjectDevice function TransformJsonData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataHasBlockTime, TestSize.Level1)
{
    const std::string path = "/data/json/Test_TransformJoyStickJsonDataToInputDataHasBlockTime.json";
    CheckTransformJson(path);
}
} // namespace MMI
} // namespace OHOS