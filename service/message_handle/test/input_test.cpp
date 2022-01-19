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
#include "mmi_server.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class InputTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
};

void InputTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void InputTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

#ifdef OHOS_BUILD_AI
HWTEST_F(InputTest, Init_001, TestSize.Level1)
{
    UDSServer udsServer;
    ServerMsgHandler serverMsgHandler;
    SeniorInputFuncProcBase seniorInputFuncProc;
    serverMsgHandler.Init(udsServer);
    serverMsgHandler.SetSeniorInputHandle(seniorInputFuncProc);
}
#endif // OHOS_BUILD_AI

HWTEST_F(InputTest, OnMsgHandler, TestSize.Level1)
{
    SessionPtr sess;
    NetPacket netPacket(10);
    ServerMsgHandler serverMsgHandler;
    serverMsgHandler.OnMsgHandler(sess, netPacket);
}

HWTEST_F(InputTest, OnMsgHandler_01, TestSize.Level1)
{
    SessionPtr sess;
    NetPacket netPacket(MmiMessageId::INVALID_MSG_ID);
    ServerMsgHandler serverMsgHandler;
    serverMsgHandler.OnMsgHandler(sess, netPacket);
}

HWTEST_F(InputTest, OnMsgHandler_02, TestSize.Level1)
{
    SessionPtr sess;
    NetPacket netPacket(-10);
    ServerMsgHandler serverMsgHandler;
    serverMsgHandler.OnMsgHandler(sess, netPacket);
}
} // namespace
