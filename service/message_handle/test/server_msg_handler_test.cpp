/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <gtest/gtest.h>

#include "libinput.h"
#include "define_multimodal.h"
#include "server_msg_handler.h"
#ifdef OHOS_BUILD_ENABLE_SECURITY_COMPONENT
#include "sec_comp_enhance_kit.h"
#endif // OHOS_BUILD_ENABLE_SECURITY_COMPONENT

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class ServerMsgHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDoen() {}
};

/**
 * @tc.name: ServerMsgHandlerTest_SetShieldStatus_01
 * @tc.desc: Test SetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_SetShieldStatus_01, TestSize.Level1)
{
    bool isShield = true;
    int32_t shieldMode = 1;
    int32_t result = KeyEventHdr->SetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(result, RET_OK);
}

/**
 * @tc.name: ServerMsgHandlerTest_SetShieldStatus_02
 * @tc.desc: Test SetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_SetShieldStatus_02, TestSize.Level1)
{
    bool isShield = true;
    int32_t shieldMode = -1;
    int32_t result = KeyEventHdr->SetShieldStatus(shieldMode, isShield);
    EXPECT_NE(result, RET_ERR);
}

/**
 * @tc.name: ServerMsgHandlerTest_SetShieldStatus_03
 * @tc.desc: Test SetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_SetShieldStatus_03, TestSize.Level1)
{
    bool isShield = false;
    int32_t shieldMode = 1;
    int32_t result = KeyEventHdr->SetShieldStatus(shieldMode, isShield);
    EXPECT_NE(result, RET_ERR);
}

/**
 * @tc.name: ServerMsgHandlerTest_GetShieldStatus_01
 * @tc.desc: Test GetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_GetShieldStatus_01, TestSize.Level1)
{
    bool isShield = false;
    int32_t shieldMode = 0;
    int32_t result = KeyEventHdr->GetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(result, RET_OK);
}

/**
 * @tc.name: ServerMsgHandlerTest_GetShieldStatus_02
 * @tc.desc: Test GetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_GetShieldStatus_02, TestSize.Level1)
{
    bool isShield = true;
    int32_t shieldMode = 0;
    int32_t result = KeyEventHdr->GetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(result, RET_OK);
}

/**
 * @tc.name: ServerMsgHandlerTest_GetShieldStatus_03
 * @tc.desc: Test GetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_GetShieldStatus_03, TestSize.Level1)
{
    bool isShield = false;
    int32_t shieldMode = 1;
    int32_t result = KeyEventHdr->GetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(result, RET_ERR);
}

/**
 * @tc.name: ServerMsgHandlerTest_GetShieldStatus_04
 * @tc.desc: Test GetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ServerMsgHandlerTest, ServerMsgHandlerTest_GetShieldStatus_04, TestSize.Level1)
{
    bool isShield = true;
    int32_t shieldMode = 1;
    int32_t result = KeyEventHdr->GetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(result, RET_ERR);
}
}
}