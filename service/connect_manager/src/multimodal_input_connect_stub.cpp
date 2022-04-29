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

#include "multimodal_input_connect_stub.h"

#include <sys/types.h>
#include <sys/socket.h>

#include "string_ex.h"
#include "accesstoken_kit.h"

#include "error_multimodal.h"
#include "mmi_log.h"
#include "multimodal_input_connect_define.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectStub" };
} // namespace

int32_t MultimodalInputConnectStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    CALL_LOG_ENTER;
    int32_t pid = GetCallingPid();
    TimeCostChk chk("IPC-OnRemoteRequest", "overtime 300(us)", MAX_OVER_TIME, pid,
        static_cast<int64_t>(code));
    uint64_t tid = GetThisThreadId();
    LOGFMTD("RemoteRequest recv code:%d tid:%" PRId64 " pid:%d", code, tid, pid);
    MMI_HILOGD("RemoteRequest recv code:%{public}d tid:%{public}" PRId64 " pid:%{public}d", code, tid, pid);

    MMI_HILOGD("step 1 pid:%{public}d", pid);
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IMultimodalInputConnect::GetDescriptor()) {
        MMI_HILOGE("get unexpect descriptor:%{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }
    MMI_HILOGD("step 2 pid:%{public}d", pid);
    if (!CheckPermission(code)) {
        MMI_HILOGE("check permission failed");
        // return CHECK_PERMISSION_FAIL;
        return ERR_INVALID_STATE;
    }
    MMI_HILOGD("step 3 pid:%{public}d", pid);
    if (!IsRunning()) {
        MMI_HILOGE("service is not running. code:%{public}u, go switch defaut", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    MMI_HILOGD("step 4 pid:%{public}d", pid);
    if (!rMsgHandler_.ChkKey(code)) {
        MMI_HILOGE("unknown code:%{public}u ids:(%{public}s), go switch defaut", code,
            rMsgHandler_.GetDebugInfo().c_str());
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    MMI_HILOGD("step 5 pid:%{public}d", pid);
    int32_t uid = GetCallingUid();
    EntrustTasks::Promise promise;
    EntrustTasks::Future future = promise.get_future();
    if (!entrustTasks_.PostSyncTask(pid, promise, future, std::bind(&RemoteMsgHandler::OnMsgHandler, &rMsgHandler_,
        std::placeholders::_1, uid, pid, tid, code, std::ref(data), std::ref(reply)))) {
        MMI_HILOGE("post task failed code:%{public}u, go switch defaut", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    MMI_HILOGD("step 6 pid:%{public}d", pid);
    return NO_ERROR;
}

bool MultimodalInputConnectStub::CheckPermission()
{
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    auto tokenType = OHOS::Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == OHOS::Security::AccessToken::TOKEN_HAP) {
        OHOS::Security::AccessToken::HapTokenInfo findInfo;
        if (OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, findInfo) != 0) {
            MMI_HILOGE("GetHapTokenInfo failed");
            return false;
        }
        if (findInfo.apl != OHOS::Security::AccessToken::APL_SYSTEM_BASIC &&
            findInfo.apl != OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            MMI_HILOGE("check hap permisson failed");
            return false;
        }
        MMI_HILOGD("check hap permisson success");
    } else if (tokenType == OHOS::Security::AccessToken::TOKEN_NATIVE) {
        OHOS::Security::AccessToken::NativeTokenInfo findInfo;
        if (OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, findInfo) != 0) {
            MMI_HILOGE("GetNativeTokenInfo failed");
            return false;
        }
        if (findInfo.apl != OHOS::Security::AccessToken::APL_SYSTEM_BASIC &&
            findInfo.apl != OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            MMI_HILOGE("check native permisson failed");
            return false;
        }
        MMI_HILOGD("check native permisson success");
    } else {
        MMI_HILOGE("unsupported token type:%{public}d", tokenType);
        return false;
    }
    return true;
}

bool MultimodalInputConnectStub::CheckPermission(uint32_t code)
{
    switch (code) {
        case IMultimodalInputConnect::SET_POINTER_VISIBLE:
        {
            return CheckPermission();
        }
    }
    return true;
}
} // namespace MMI
} // namespace OHOS