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

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectStub" };
} // namespace

int32_t MultimodalInputConnectStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    CALL_LOG_ENTER;
    MMI_HILOGD("request code:%{public}d", code);

    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IMultimodalInputConnect::GetDescriptor()) {
        MMI_HILOGE("get unexpect descriptor:%{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }

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
        case IMultimodalInputConnect::SIMULATE_CROSS_LOCATION:
            return StubSetPointerLocation(data, reply);
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
        case IMultimodalInputConnect::GET_REMOTE_ABILITY:
            return StubGetRemoteAbility(data, reply);
        case IMultimodalInputConnect::PREPARE_DINPUT:
            return StubPrepareRemoteInput(data, reply);
        case IMultimodalInputConnect::UNPREPARE_DINPUT:
            return StubUnprepareRemoteInput(data, reply);
        case IMultimodalInputConnect::START_DINPUT:
            return StubStartRemoteInput(data, reply);
        case IMultimodalInputConnect::STOP_DINPUT:
            return StubStopRemoteInput(data, reply);
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
        default: {
            MMI_HILOGE("unknown code:%{public}u, go switch defaut", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t MultimodalInputConnectStub::StubAddInputEventFilter(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    int32_t ret = RET_OK;

    do {
        const int32_t uid = GetCallingUid();
        if (uid != SYSTEM_UID && uid != ROOT_UID) {
            MMI_HILOGE("check failed, uid is not root or system");
            ret = SASERVICE_PERMISSION_FAIL;
            break;
        }

        sptr<IRemoteObject> client = data.ReadRemoteObject();
        if (client == nullptr) {
            MMI_HILOGE("mouse client is nullptr");
            ret = ERR_INVALID_VALUE;
            break;
        }

        sptr<IEventFilter> filter = iface_cast<IEventFilter>(client);
        if (filter == nullptr) {
            MMI_HILOGE("filter is nullptr");
            ret = ERROR_NULL_POINTER;
            break;
        }

        MMI_HILOGD("filter iface_cast succeeded");

        ret = AddInputEventFilter(filter);
    } while (0);
    
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
    int32_t ret;
    bool visible;
    if (!data.ReadBool(visible)) {
        MMI_HILOGE("data ReadBool fail");
        return IPC_PROXY_DEAD_OBJECT_ERR;
    }
    ret = SetPointerVisible(visible);
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

    int32_t ret;
    bool visible;
    ret = IsPointerVisible(visible);
    if (!reply.WriteBool(visible)) {
        MMI_HILOGE("WriteBool:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    return ret;
}

int32_t MultimodalInputConnectStub::StubSetPointerLocation(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    if (!CheckPermission()) {
        MMI_HILOGE("permission check fail");
        return CHECK_PERMISSION_FAIL;
    }
    int32_t ret = RET_OK;

    int32_t x = data.ReadInt32();
    int32_t y = data.ReadInt32();
    ret = SetPointerLocation(x, y);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
int32_t MultimodalInputConnectStub::StubGetRemoteAbility(MessageParcel& data, MessageParcel& reply)
{
    int32_t ret = RET_OK;

    std::string networkId = data.ReadString();
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGW("the client value is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<ICallDinput> remoteAbility = iface_cast<ICallDinput>(client);
    if (remoteAbility == nullptr) {
        MMI_HILOGW("remoteAbility is nullptr");
        return ERROR_NULL_POINTER;
    }
    MMI_HILOGD("remoteAbility iface_cast succeeded");
    ret = GetRemoteInputAbility(networkId, remoteAbility);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}
int32_t MultimodalInputConnectStub::StubPrepareRemoteInput(MessageParcel& data, MessageParcel& reply)
{
    int32_t ret = RET_OK;
    std::string networkId = data.ReadString();
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGW("the client value is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<ICallDinput> prepareDinput = iface_cast<ICallDinput>(client);
    if (prepareDinput == nullptr) {
        MMI_HILOGW("prepareDinput is nullptr");
        return ERROR_NULL_POINTER;
    }
    MMI_HILOGD("prepareDinput iface_cast succeeded");
    ret = PrepareRemoteInput(networkId, prepareDinput);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}
int32_t MultimodalInputConnectStub::StubUnprepareRemoteInput(MessageParcel& data, MessageParcel& reply)
{
    int32_t ret = RET_OK;
    std::string networkId = data.ReadString();
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGW("the client value is nullptr");
        return ERROR_NULL_POINTER;
    }
    sptr<ICallDinput> unprepareDinput = iface_cast<ICallDinput>(client);
    if (unprepareDinput == nullptr) {
        MMI_HILOGW("unprepareDinput is nullptr");
        return ERROR_NULL_POINTER;
    }
    MMI_HILOGD("unprepareDinput iface_cast succeeded");
    ret = UnprepareRemoteInput(networkId, unprepareDinput);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}
int32_t MultimodalInputConnectStub::StubStartRemoteInput(MessageParcel& data, MessageParcel& reply)
{
    int32_t ret = RET_OK;

    std::string networkId = data.ReadString();
    int32_t inputAbility = data.ReadInt32();
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGW("the client value is nullptr");
        return ERROR_NULL_POINTER;
    }
    sptr<ICallDinput> startDinput = iface_cast<ICallDinput>(client);
    if (startDinput == nullptr) {
        MMI_HILOGW("startDinput is nullptr");
        return ERROR_NULL_POINTER;
    }
    MMI_HILOGD("startDinput iface_cast succeeded");
    ret = StartRemoteInput(networkId, inputAbility, startDinput);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}

int32_t MultimodalInputConnectStub::StubStopRemoteInput(MessageParcel& data, MessageParcel& reply)
{
    int32_t ret = RET_OK;
    std::string networkId = data.ReadString();
    int32_t inputAbility = data.ReadInt32();
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGW("the client value is nullptr");
        return ERROR_NULL_POINTER;
    }
    sptr<ICallDinput> stopDinput = iface_cast<ICallDinput>(client);
    if (stopDinput == nullptr) {
        MMI_HILOGW("stopDinput is nullptr");
        return ERROR_NULL_POINTER;
    }
    MMI_HILOGD("stopDinput iface_cast succeeded");
    ret = StopRemoteInput(networkId, inputAbility, stopDinput);
    
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGW("WriteInt32:%{public}d fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_HILOGD("leave, ret:%{public}d", ret);
    return RET_OK;
}
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
} // namespace MMI
} // namespace OHOS
