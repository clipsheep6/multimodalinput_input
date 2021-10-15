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
#include "mmi_client.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class MMIClientTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

class MMIClientUnitTest : public MMIClient {
public:
    void OnDisconnectedUnitTest()
    {
        OnDisconnected();
    }
    void OnConnectedUnitTest()
    {
        OnConnected();
    }
};

MMIClient mmiClient;
ConnectCallback connectFun;

HWTEST_F(MMIClientTest, RegisterConnectedFunction, TestSize.Level1)
{
    mmiClient.RegisterConnectedFunction(connectFun);
}

HWTEST_F(MMIClientTest, RegisterDisconnectedFunction, TestSize.Level1)
{
    mmiClient.RegisterDisconnectedFunction(connectFun);
}

HWTEST_F(MMIClientTest, VirtualKeyIn, TestSize.Level1)
{
    RawInputEvent virtualKeyEvent = {};
    mmiClient.VirtualKeyIn(virtualKeyEvent);
}

HWTEST_F(MMIClientTest, ReplyMessageToServer_001, TestSize.Level1)
{
    ssize_t time = 1;
    uint64_t serverStartTime = 1;
    uint64_t clientEndTime = 1;
    int32_t fd = 1;

    mmiClient.ReplyMessageToServer(4, time, serverStartTime, clientEndTime, fd);
}

HWTEST_F(MMIClientTest, ReplyMessageToServer_002, TestSize.Level1)
{
    ssize_t time = 0;
    uint64_t serverStartTime = 0;
    uint64_t clientEndTime = 0;
    int32_t fd = 0;

    mmiClient.ReplyMessageToServer(3, time, serverStartTime, clientEndTime, fd);
}

HWTEST_F(MMIClientTest, SdkGetMultimodeInputInfo, TestSize.Level1)
{
    mmiClient.SdkGetMultimodeInputInfo();
}

MMIClientUnitTest mmiClientTest;
HWTEST_F(MMIClientTest, Re_RegisterConnectedFunction, TestSize.Level1)
{
    mmiClientTest.RegisterConnectedFunction(connectFun);
}

HWTEST_F(MMIClientTest, Re_RegisterDisconnectedFunction, TestSize.Level1)
{
    mmiClientTest.RegisterDisconnectedFunction(connectFun);
}

HWTEST_F(MMIClientTest, Re_VirtualKeyIn, TestSize.Level1)
{
    RawInputEvent virtualKeyEvent = {};
    mmiClientTest.VirtualKeyIn(virtualKeyEvent);
}

HWTEST_F(MMIClientTest, Re_ReplyMessageToServer_001, TestSize.Level1)
{
    ssize_t timeId = 1;
    uint64_t serverStartTime = 1;
    uint64_t clientEndTime = 1;
    int32_t fd = 1;

    mmiClientTest.ReplyMessageToServer(1, timeId, serverStartTime, clientEndTime, fd);
}

HWTEST_F(MMIClientTest, Re_ReplyMessageToServer_002, TestSize.Level1)
{
    ssize_t timeId = 0;
    uint64_t serverStartTime = 0;
    uint64_t clientEndTime = 0;
    int32_t fileId = 0;

    mmiClientTest.ReplyMessageToServer(2, timeId, serverStartTime, clientEndTime, fileId);
}

HWTEST_F(MMIClientTest, Re_SdkGetMultimodeInputInfo, TestSize.Level1)
{
    mmiClientTest.SdkGetMultimodeInputInfo();
}

HWTEST_F(MMIClientTest, Re_OnConnected, TestSize.Level1)
{
    mmiClientTest.OnConnectedUnitTest();
}
HWTEST_F(MMIClientTest, Re_OnConnected_002, TestSize.Level1)
{
    ConnectCallback funTmp;
    mmiClientTest.RegisterConnectedFunction(funTmp);
    mmiClientTest.OnConnectedUnitTest();
}

HWTEST_F(MMIClientTest, Re_OnDisconnected, TestSize.Level1)
{
    mmiClientTest.OnDisconnectedUnitTest();
}

HWTEST_F(MMIClientTest, Re_OnDisconnected_002, TestSize.Level1)
{
    ConnectCallback funTmp;
    mmiClientTest.RegisterDisconnectedFunction(funTmp);
    mmiClientTest.OnDisconnectedUnitTest();
}
} // namespace
