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
#include "processing_joystick_device.h"
#include "manage_inject_device.h"
#include "msg_head.h"

namespace {
    using namespace testing::ext;
    using namespace OHOS::MMI;
    using namespace std;
    class ProcessingJoyStickDeviceTest : public testing::Test {
    public:
        static void SetUpTestCase(void) {}
        static void TearDownTestCase(void) {}
    };

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJsonDataToInputData, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputData.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_OK);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindEvents, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindEvents.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataEventsIsEmpty.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindKeyValue, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindKeyValue.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindKeyValueInThro, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindKeyValueInThro.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindEventInRocker, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindEventInRocker.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInRocker,
        TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInRocker.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInDirectionKey,
        TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindDirectionInDirectionKey.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataNotFindAnyEventType, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataNotFindAnyEventType.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_OK);
}

HWTEST_F(ProcessingJoyStickDeviceTest, Test_TransformJoyStickJsonDataToInputDataHasBlockTime, TestSize.Level1)
{
    const string path = "temp/Test_TransformJoyStickJsonDataToInputDataHasBlockTime.json";
#ifdef OHOS_BUILD
    string startDeviceCmd = "hosmmi-virtual-device-manager start joystick & ";
    string closeDeviceCmd = "hosmmi-virtual-device-manager close all";
#else
    string startDeviceCmd = "./hosmmi-virtual-deviced.out start joystick &";
    string closeDeviceCmd = "./hosmmi-virtual-deviced.out close all";
#endif
    system(startDeviceCmd.c_str());
    std::ifstream reader(path);
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    ManageInjectDevice manageInjectDevice;
    auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
    // system(closeDeviceCmd.c_str());
    EXPECT_EQ(ret, RET_OK);
}
}