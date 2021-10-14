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
#include "processing_touch_screen_device.h"
#include "manage_inject_device.h"
#include "msg_head.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace std;
class ProcessingTouchScreenDeviceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputData, TestSize.Level1)
{
    const string path = "temp/Test_TransformTouchScreenJsonDataToInputData.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start touchscreen & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start touchscreen &";
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

HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputDataEventsIsEmpty, TestSize.Level1)
{
    const string path = "temp/Test_TransformJsonDataToInputDataEventsIsEmpty.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start touchscreen & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start touchscreen &";
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

HWTEST_F(ProcessingTouchScreenDeviceTest, Test_TransformJsonDataToInputDataSingleEventsIsEmpty, TestSize.Level1)
{
    const string path = "temp/Test_TransformJsonDataToInputDataSingleEventsIsEmpty.json";
#ifdef OHOS_BUILD
    string cmd = "hosmmi-virtual-device-manager start touchscreen & ";
#else
    string cmd = "./hosmmi-virtual-deviced.out start touchscreen &";
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