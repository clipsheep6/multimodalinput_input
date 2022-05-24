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

#include "multimodal_input_connect_proxy.h"

#include "message_option.h"
#include "mmi_log.h"
#include "multimodal_input_connect_def_parcel.h"
#include "multimodal_input_connect_define.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectProxy" };
} // namespace

MultimodalInputConnectProxy::MultimodalInputConnectProxy(const sptr<IRemoteObject> &impl) :
    IRemoteProxy<IMultimodalInputConnect>(impl)
{
    MMI_HILOGI("enter MultimodalInputConnectProxy");
}

MultimodalInputConnectProxy::~MultimodalInputConnectProxy()
{
    MMI_HILOGI("enter ~MultimodalInputConnectProxy");
}

int32_t MultimodalInputConnectProxy::AllocSocketFd(const std::string &programName,
    const int32_t moduleType, int32_t &socketFd)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    ConnectReqParcel req;
    req.data.moduleId = moduleType;
    req.data.clientName = programName;
    if (!data.WriteParcelable(&req)) {
        MMI_HILOGE("Failed to write programName");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t requestResult = Remote()->SendRequest(ALLOC_SOCKET_FD, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGE("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }

    MMI_HILOGD("recieved message from server");

    int32_t result = reply.ReadInt32();
    MMI_HILOGD("result:%{public}d", result);
    if (result != RET_OK) {
        MMI_HILOGE("responce return error:%{public}d", result);
        return RET_ERR;
    }
    socketFd = reply.ReadFileDescriptor();
    MMI_HILOGD("socketFd:%{public}d", socketFd);
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputEventFilter(sptr<IEventFilter> filter)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteRemoteObject(filter->AsObject().GetRefPtr())) {
        MMI_HILOGE("Failed to write filter");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t requestResult = Remote()->SendRequest(ADD_INPUT_EVENT_FILTER, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGE("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }

    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGE("reply readint32 error:%{public}d", result);
    }
    return result;
}

int32_t MultimodalInputConnectProxy::SetPointerVisible(bool visible)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteBool(visible)) {
        MMI_HILOGE("Failed to write filter");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t requestResult = Remote()->SendRequest(SET_POINTER_VISIBLE, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGE("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }

    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGE("reply readint32 error:%{public}d", result);
    }
    return result;
}

int32_t MultimodalInputConnectProxy::IsPointerVisible(bool &visible)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t requestResult = Remote()->SendRequest(IS_POINTER_VISIBLE, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGE("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }

    visible = reply.ReadBool();
    return RET_OK;
}
#ifdef OHOS_BUILD_KEY_MOUSE
int32_t MultimodalInputConnectProxy::SetPointerLocation(int32_t x, int32_t y)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    data.WriteInt32(x);
    data.WriteInt32(y);
    int32_t requestResult = Remote()->SendRequest(SIMULATE_CROSS_LOCATION, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }

    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}
#endif

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
int32_t MultimodalInputConnectProxy::GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    data.WriteString(deviceId);
    if (!data.WriteRemoteObject(ablitity->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    int32_t requestResult = Remote()->SendRequest(GET_REMOTE_ABILITY, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }
    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}

int32_t MultimodalInputConnectProxy::PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    data.WriteString(deviceId);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    int32_t requestResult = Remote()->SendRequest(PREPARE_DINPUT, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }
    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}

int32_t MultimodalInputConnectProxy::UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    data.WriteString(deviceId);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    int32_t requestResult = Remote()->SendRequest(UNPREPARE_DINPUT, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }
    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}
int32_t MultimodalInputConnectProxy::StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    data.WriteString(deviceId);
    data.WriteInt32(inputAbility);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    int32_t requestResult = Remote()->SendRequest(START_DINPUT, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }
    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}
int32_t MultimodalInputConnectProxy::StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    data.WriteString(deviceId);
    data.WriteInt32(inputAbility);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    int32_t requestResult = Remote()->SendRequest(STOP_DINPUT, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return RET_ERR;
    }
    int32_t result = reply.ReadInt32();
    if (result != RET_OK) {
        MMI_HILOGW("responce return error:%{public}d", result);
    }
    MMI_HILOGD("leave");
    return result;
}
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
} // namespace MMI
} // namespace OHOS
