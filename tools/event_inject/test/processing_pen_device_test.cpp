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

#include "processing_pen_device.h"
#include <gtest/gtest.h>
#include "manage_inject_device.h"
#include "msg_head.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace std;
} // namespace

class ProcessingPenDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputData, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputData.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputData.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_OK);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataNotfindEvents, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataNotfindEvents.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataNotfindEvents.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataEventsIsEmpty.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataEventsIsEmpty.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataApprochEventError, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataApprochEventError.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataApprochEventError.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataSlideEventError, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataSlideEventError.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataSlideEventError.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataLeaveEventError, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataLeaveEventError.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataLeaveEventError.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataApprochEventEventTypeError, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataApprochEventEventTypeError.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataApprochEventEventTypeError.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }

    HWTEST_F(ProcessingPenDeviceTest, Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError, TestSize.Level1)
    {
#ifdef OHOS_BUILD
        const string path = "/data/json/Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError.json";
        string startDeviceCmd = "mmi-virtual-device-manager start touchpad & ";
        string closeDeviceCmd = "mmi-virtual-device-manager close all";
#else
        const string path = "temp/Test_TransformPenJsonDataToInputDataLeaveEventEventTypeError.json";
        string startDeviceCmd = "./mmi-virtual-deviced.out start touchpad &";
        string closeDeviceCmd = "./mmi-virtual-deviced.out close all";
#endif
        system(startDeviceCmd.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::ifstream reader(path);
        if (!reader.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            system(closeDeviceCmd.c_str());
            ASSERT_TRUE(false) << "can not open " << path;
        }
        Json inputEventArrays;
        reader >> inputEventArrays;
        reader.close();
        ManageInjectDevice manageInjectDevice;
        auto ret = manageInjectDevice.TransformJsonData(inputEventArrays);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        system(closeDeviceCmd.c_str());
        EXPECT_EQ(ret, RET_ERR);
    }
} // namespace MMI
} // namespace OHOS