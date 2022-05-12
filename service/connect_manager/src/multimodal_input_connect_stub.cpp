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
#include <sys/stat.h>
#include <fcntl.h>

#include "string_ex.h"
#include "accesstoken_kit.h"

#include "error_multimodal.h"
#include "multimodal_input_connect_def_parcel.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectStub" };
} // namespace

int32_t MultimodalInputConnectStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    CALL_LOG_ENTER2;
    int32_t pid = GetCallingPid();
    TimeCostChk chk("IPC-OnRemoteRequest", "overtime 300(us)", MAX_OVER_TIME, pid,
        static_cast<int64_t>(code));
    uint64_t tid = GetThisThreadId();
    LOGFMTD("RemoteRequest recv code:%d tid:%" PRId64 " pid:%d", code, tid, pid);
    MMI_HILOGD("RemoteRequest recv code:%{public}d tid:%{public}" PRId64 " pid:%{public}d", code, tid, pid);

    LOGFMTD("step 1 pid:%d", pid);
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IMultimodalInputConnect::GetDescriptor()) {
        LOGFMTE("get unexpect descriptor:%s", Str16ToStr8(descriptor).c_str());
        MMI_HILOGE("get unexpect descriptor:%{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }

    LOGFMTD("step 2 pid:%d", pid);
    switch (code) {
        case IMultimodalInputConnect::ALLOC_SOCKET_FD: {
            return StubHandleAllocSocketFd(data, reply);
        }
        case IMultimodalInputConnect::ADD_INPUT_EVENT_FILTER: {
            return StubAddInputEventFilter(data, reply);
        }
        case IMultimodalInputConnect::SET_POINTER_VISIBLE: {
            return StubSetPointerVisible(data, reply);
        }
        case IMultimodalInputConnect::IS_POINTER_VISIBLE: {
            return StubIsPointerVisible(data, reply);
        }
        default: {
            LOGFMTE("unknown code:%u, go switch defaut", code);
            MMI_HILOGE("unknown code:%{public}u, go switch defaut", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t MultimodalInputConnectStub::StubHandleAllocSocketFd(MessageParcel& data, MessageParcel& reply)
{
    int32_t pid = GetCallingPid();
    if (!IsRunning()) {
        LOGFMTE("service is not running. pid:%d, go switch defaut", pid);
        MMI_HILOGE("service is not running. pid:%{public}d, go switch defaut", pid);
        return MMISERVICE_NOT_RUNNING;
    }
    sptr<ConnectReqParcel> req = data.ReadParcelable<ConnectReqParcel>();
    CHKPR(req, ERR_NULL_OBJECT);
    MMI_HILOGI("clientName:%{public}s,moduleId:%{public}d", req->data.clientName.c_str(), req->data.moduleId);
    
    int32_t clientFd = INVALID_SOCKET_FD;
    int32_t uid = GetCallingUid();
    int32_t ret = entrustTasks_.PostAsyncTask(pid, std::bind(&IMultimodalInputConnect::AllocSocketFd, this,
        req->data.clientName, req->data.moduleId, std::ref(clientFd), pid, uid));
    if (ret != RET_OK) {
        LOGFMTE("post task AllocSocketFd failed pid:%d, go switch defaut", pid);
        MMI_HILOGE("post task AllocSocketFd failed pid:%{public}d, go switch defaut", pid);
        reply.WriteInt32(RET_ERR);
        return ret;
    }
    MMI_HILOGI("call AllocSocketFd success");

    reply.WriteInt32(RET_OK);
    reply.WriteFileDescriptor(clientFd);

    MMI_HILOGI("send clientFd to client, clientFd = %d", clientFd);
    close(clientFd);
    return RET_OK;
}

int32_t MultimodalInputConnectStub::StubAddInputEventFilter(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    const int32_t uid = GetCallingUid();
    if (uid != SYSTEM_UID && uid != ROOT_UID) {
        MMI_HILOGE("check failed, uid is not root or system");
        return SASERVICE_PERMISSION_FAIL;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGE("mouse client is nullptr");
        return ERR_INVALID_VALUE;
    }

    sptr<IEventFilter> filter = iface_cast<IEventFilter>(client);
    if (filter == nullptr) {
        MMI_HILOGE("filter is nullptr");
        return ERROR_NULL_POINTER;
    }

    MMI_HILOGD("filter iface_cast succeeded");
    int32_t ret = entrustTasks_.PostSyncTask(GetCallingPid(),
        std::bind(&IMultimodalInputConnect::AddInputEventFilter, this, filter));
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGE("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("ret:%{public}d", ret);
    return RET_OK;
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
        if (findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_BASIC ||
            findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            MMI_HILOGI("check hap permisson success");
            return true;
        }
        MMI_HILOGE("check hap permisson failed");
        return false;
    }
    if (tokenType == OHOS::Security::AccessToken::TOKEN_NATIVE) {
        OHOS::Security::AccessToken::NativeTokenInfo findInfo;
        if (OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, findInfo) != 0) {
            MMI_HILOGE("GetNativeTokenInfo failed");
            return false;
        }
        if (findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_BASIC ||
            findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            MMI_HILOGI("check native permisson success");
            return true;
        }
        MMI_HILOGE("check native permisson failed");
        return false;
    }
    
    MMI_HILOGE("unsupported token type:%{public}d", tokenType);
    return false;
}

int32_t MultimodalInputConnectStub::StubSetPointerVisible(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    if (!CheckPermission()) {
        MMI_HILOGE("permission check fail");
        return CHECK_PERMISSION_FAIL;
    }

    bool visible = false;
    if (!data.ReadBool(visible)) {
        MMI_HILOGE("data ReadBool fail");
        return IPC_PROXY_DEAD_OBJECT_ERR;
    }
    int32_t ret = entrustTasks_.PostSyncTask(GetCallingPid(),
        std::bind(&IMultimodalInputConnect::SetPointerVisible, this, visible));
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGE("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectStub::StubIsPointerVisible(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    if (!CheckPermission()) {
        MMI_HILOGE("permission check fail");
        return CHECK_PERMISSION_FAIL;
    }

    bool visible = false;
    int32_t ret = entrustTasks_.PostSyncTask(GetCallingPid(),
        std::bind(&IMultimodalInputConnect::IsPointerVisible, this, std::ref(visible)));
    if (!reply.WriteBool(visible)) {
        MMI_HILOGE("WriteBool:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ret;
}
} // namespace MMI
} // namespace OHOS