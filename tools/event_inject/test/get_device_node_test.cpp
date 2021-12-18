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
#include "msg_head.h"
#include "proto.h"
#define private public
#include "get_device_node.h"
#undef private

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace std;

class GetDeviceNodeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(GetDeviceNodeTest, Test_GetDeviceNodeTest, TestSize.Level1)
{
    GetDeviceNode getDeviceNode;
    const string cmd = "";
    StringList cmdResult;
    auto ret = getDeviceNode.ExecuteCmd(cmd, cmdResult);
    EXPECT_EQ(ret, RET_ERR);
}

HWTEST_F(GetDeviceNodeTest, Test_GetDeviceNodeTestCmdError, TestSize.Level1)
{
    GetDeviceNode getDeviceNode;
    const string cmd = "temp";
    StringList cmdResult;
    auto ret = getDeviceNode.ExecuteCmd(cmd, cmdResult);
    EXPECT_GT(ret, 0);
}
}