/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "proto.h"
#include "processing_game_pad_device.h"
#include "manage_inject_device.h"
#include "msg_head.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace std;
class ProcessingGamePadDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputData, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputData.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_OK);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataNotFindEvents, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataNotFindEvents.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataEventsIsEmpty.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataNotFindKeyValueInPress, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataNotFindKeyValueInPress.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataNotFindEventInRocker, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataNotFindEventInRocker.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRocker, TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRocker.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingGamePadDeviceTest, Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRockerDirectionKey,
        TestSize.Level1)
{
    const string path = "temp/Test_TransformGamePadJsonDataToInputDataNotFindDirectionInRockerDirectionKey.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start gamepad & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start gamepad &";
#endif
    system(cmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    EXPECT_EQ(ret, RET_ERR);
}
}