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

#include "call_dinput_proxy.h"
#include "message_option.h"
#include "mmi_log.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "CallDinputProxy" };
} // namespace

// 获取其他设备注册的SA的Proxy
CallDinputProxy::CallDinputProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ICallDinput>(impl)
{
    MMI_HILOGD("CallDinputProxy()");
}

CallDinputProxy::~CallDinputProxy()
{
    MMI_HILOGD("~CallDinputProxy()");
}

bool CallDinputProxy::HandlePrepareDinput(std::string deviceId, int32_t status)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(CallDinputProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return false;
    }
    data.WriteString(deviceId);
    data.WriteInt32(status);
    const uint32_t code = static_cast<uint32_t>(OPERATOR_TYPE::HANDLE_PREPARE_DINPUT);
    int32_t requestResult = Remote()->SendRequest(code, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return false;
    }

    MMI_HILOGD("have recieve message from server");

    bool result = false;
    if (!reply.ReadBool(result)) {
        MMI_HILOGW("reply ReadBool fail");
        return false;
    }

    MMI_HILOGD("leave");
    return result;
}

bool CallDinputProxy::HandleUnprepareDinput(std::string deviceId, int32_t status)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(CallDinputProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return false;
    }
    data.WriteString(deviceId);
    data.WriteInt32(status);
    const uint32_t code = static_cast<uint32_t>(OPERATOR_TYPE::HANDLE_UNPREPARE_DINPUT);
    int32_t requestResult = Remote()->SendRequest(code, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return false;
    }

    MMI_HILOGD("have recieve message from server");

    bool result = false;
    if (!reply.ReadBool(result)) {
        MMI_HILOGW("reply ReadBool fail");
        return false;
    }

    MMI_HILOGD("leave");
    return result;
}

bool CallDinputProxy::HandleStartDinput(std::string deviceId, uint32_t inputTypes, int32_t status)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(CallDinputProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return false;
    }
    data.WriteString(deviceId);
    data.WriteInt32(inputTypes);
    data.WriteInt32(status);
    const uint32_t code = static_cast<uint32_t>(OPERATOR_TYPE::HANDLE_START_DINPUT);
    int32_t requestResult = Remote()->SendRequest(code, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return false;
    }

    MMI_HILOGD("have recieve message from server");

    bool result = false;
    if (!reply.ReadBool(result)) {
        MMI_HILOGW("reply ReadBool fail");
        return false;
    }

    MMI_HILOGD("leave");
    return result;
}

bool CallDinputProxy::HandleStopDinput(std::string deviceId, uint32_t inputTypes, int32_t status)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(CallDinputProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return false;
    }
    data.WriteString(deviceId);
    data.WriteInt32(inputTypes);
    data.WriteInt32(status);
    const uint32_t code = static_cast<uint32_t>(OPERATOR_TYPE::HANDLE_STOP_DINPUT);
    int32_t requestResult = Remote()->SendRequest(code, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return false;
    }

    MMI_HILOGD("have recieve message from server");

    bool result = false;
    if (!reply.ReadBool(result)) {
        MMI_HILOGW("reply ReadBool fail");
        return false;
    }

    MMI_HILOGD("leave");
    return result;
}

bool CallDinputProxy::HandleRemoteInputAbility(const std::set<int32_t> remoteInputAbility)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(CallDinputProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return false;
    }
    data.WriteInt32(remoteInputAbility.size());
    for (auto item : remoteInputAbility) {
        data.WriteInt32(item);
    }
    const uint32_t code = static_cast<uint32_t>(OPERATOR_TYPE::HANDLE_REMOTE_INPUT_ABILITY);
    int32_t requestResult = Remote()->SendRequest(code, data, reply, option);
    if (requestResult != NO_ERROR) {
        MMI_HILOGW("send request fail, result:%{public}d", requestResult);
        return false;
    }

    MMI_HILOGD("have recieve message from server");

    bool result = false;
    if (!reply.ReadBool(result)) {
        MMI_HILOGW("reply ReadBool fail");
        return false;
    }

    MMI_HILOGD("leave");
    return result;
}
} // namespace MMI
} // namespace OHOS
