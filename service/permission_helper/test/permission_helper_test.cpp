/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cinttypes>
#include <cstdio>

#include <gtest/gtest.h>
#include "ipc_skeleton.h"
#include "mmi_log.h"
#include "proto.h"
#include "tokenid_kit.h"
#include "uds_server.h"

#include "define_multimodal.h"
#include "permission_helper.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
const std::string inputMonitor = "ohos.permission.INPUT_MONITORING";
const std::string inputInterceptor = "ohos.permission.INTERCEPT_INPUT_EVENT";
const std::string inputDispatchControl = "ohos.permission.INPUT_CONTROL_DISPATCHING";
} // namespace

class PermissionHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: PermissionHelperTest_CheckMonitorPermission
 * @tc.desc: Test CheckMonitorPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckMonitorPermission, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 1;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, inputMonitor);
    ret = OHOS::Security::AccessToken::PERMISSION_GRANTED;
    bool result = PerHelper->CheckMonitorPermission(tokenId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PermissionHelperTest_CheckInterceptorPermission
 * @tc.desc: Test CheckInterceptorPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckInterceptorPermission, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 1;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, inputInterceptor);
    ret = OHOS::Security::AccessToken::PERMISSION_GRANTED;
    bool result = PerHelper->CheckInterceptorPermission(tokenId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PermissionHelperTest_CheckDispatchControlPermission
 * @tc.desc: Test CheckDispatchControlPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckDispatchControlPermission, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 1;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, inputDispatchControl);
    ret = OHOS::Security::AccessToken::PERMISSION_GRANTED;
    bool result = PerHelper->CheckDispatchControlPermission(tokenId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PermissionHelperTest_GetTokenType
 * @tc.desc: Test GetTokenType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_GetTokenType, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 5;
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    tokenType = OHOS::Security::AccessToken::TOKEN_HAP;
    int32_t result = PerHelper->GetTokenType();
    EXPECT_EQ(result, TokenType::TOKEN_SHELL);
    tokenType = OHOS::Security::AccessToken::TOKEN_NATIVE;
    result = PerHelper->GetTokenType();
    EXPECT_EQ(result, TokenType::TOKEN_SHELL);
    tokenType = OHOS::Security::AccessToken::TOKEN_SHELL;
    result = PerHelper->GetTokenType();
    EXPECT_EQ(result, TokenType::TOKEN_SHELL);
}

/**
 * @tc.name: PermissionHelperTest_CheckDispatchControl
 * @tc.desc: Test CheckDispatchControl
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckDispatchControl, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 2;
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    tokenType = OHOS::Security::AccessToken::TOKEN_HAP;
    bool result = PerHelper->CheckDispatchControl();
    ASSERT_TRUE(result);
    tokenType = OHOS::Security::AccessToken::TOKEN_NATIVE;
    result = PerHelper->CheckDispatchControl();
    ASSERT_TRUE(result);
    tokenType = OHOS::Security::AccessToken::TOKEN_SHELL;
    result = PerHelper->CheckDispatchControl();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: PermissionHelperTest_CheckHapPermission
 * @tc.desc: Test CheckHapPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckHapPermission, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 3;
    std::string permissionCode = "access";
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    tokenType = OHOS::Security::AccessToken::TOKEN_HAP;
    bool result = PerHelper->CheckHapPermission(tokenId, permissionCode);
    ASSERT_FALSE(result);
    tokenType = OHOS::Security::AccessToken::TOKEN_NATIVE;
    result = PerHelper->CheckHapPermission(tokenId, permissionCode);
    ASSERT_FALSE(result);
    tokenType = OHOS::Security::AccessToken::TOKEN_SHELL;
    result = PerHelper->CheckHapPermission(tokenId, permissionCode);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PermissionHelperTest_VerifySystemApp
 * @tc.desc: Test VerifySystemApp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_VerifySystemApp, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t callerToken = 3;
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    tokenType = OHOS::Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE;
    bool result = PerHelper->VerifySystemApp();
    ASSERT_TRUE(result);
    tokenType = OHOS::Security::AccessToken::ATokenTypeEnum::TOKEN_SHELL;
    result = PerHelper->VerifySystemApp();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: PermissionHelperTest_CheckPermission
 * @tc.desc: Test CheckPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionHelperTest, PermissionHelperTest_CheckPermission, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint32_t tokenId = 1;
    uint32_t required = 2;
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    tokenType = OHOS::Security::AccessToken::TOKEN_NATIVE;
    bool result = PerHelper->CheckPermission(required);
    ASSERT_TRUE(result);
    tokenType = OHOS::Security::AccessToken::TOKEN_SHELL;
    result = PerHelper->CheckPermission(required);
    ASSERT_TRUE(result);
}
} // namespace MMI
} // namespace OHOS