/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "permission_manager.h"
#include "ipc_skeleton.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "PermissionManager"};
} // namespace

bool PermissionManager::CheckPermission(int32_t required)
{
    CALL_LOG_ENTER;
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == OHOS::Security::AccessToken::TOKEN_HAP) {
        return CheckHapPermission(tokenId, required);
    }
    if (tokenType == OHOS::Security::AccessToken::TOKEN_NATIVE) {
        return CheckNativePermission(tokenId, required);
    }
    
    MMI_HILOGE("unsupported token type:%{public}d", tokenType);
    return false;
}

bool PermissionManager::CheckHapPermission(uint32_t tokenId, int32_t required)
{
    OHOS::Security::AccessToken::HapTokenInfo findInfo;
    if (OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, findInfo) != 0) {
        MMI_HILOGE("GetHapTokenInfo failed");
        return false;
    }
    if ((1 << findInfo.apl) & required) {
        MMI_HILOGI("check hap permisson success");
        return true;
    }
    MMI_HILOGE("check hap permisson failed");
    return false;
}

bool PermissionManager::CheckNativePermission(uint32_t tokenId, int32_t required)
{
    OHOS::Security::AccessToken::NativeTokenInfo findInfo;
    if (OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, findInfo) != 0) {
        MMI_HILOGE("GetNativeTokenInfo failed");
        return false;
    }
    if ((1 << findInfo.apl) & required) {
        MMI_HILOGI("check native permisson success");
        return true;
    }
    MMI_HILOGE("check native permisson failed");
    return false;
}
} // namespace MMI
} // namespace OHOS