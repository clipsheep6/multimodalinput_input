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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(ALLOC_SOCKET_FD, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(ADD_INPUT_EVENT_FILTER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("reply readint32 error:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerVisible(bool visible)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    WRITEBOOL(data, visible, ERR_INVALID_VALUE);

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SET_POINTER_VISIBLE, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(IS_POINTER_VISIBLE, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    visible = reply.ReadBool();
    return RET_OK;
}
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
    WRITEINT32(data, x, ERR_INVALID_VALUE);
    WRITEINT32(data, y, ERR_INVALID_VALUE);
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SIMULATE_CROSS_LOCATION, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetInputDeviceSeatName(const std::string& seatName, DeviceUniqId& deviceUniqId)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGW("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    data.WriteString(seatName);
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SET_INPUT_DEVICE_SEAT_NAME, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    int32_t bus = reply.ReadInt32();
    int32_t version = reply.ReadInt32();
    int32_t product = reply.ReadInt32();
    int32_t vendo = reply.ReadInt32();
    int32_t udevTags = reply.ReadInt32();
    std::string uniq = reply.ReadString();
    deviceUniqId = std::make_tuple(bus, version, product, vendo, udevTags, uniq);
    return RET_OK;
}

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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(GET_REMOTE_ABILITY, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(PREPARE_DINPUT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
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
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(UNPREPARE_DINPUT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}
int32_t MultimodalInputConnectProxy::StartRemoteInput(const std::string& deviceId,
    uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
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
    WRITEINT32(data, inputAbility, ERR_INVALID_VALUE);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(START_DINPUT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}
int32_t MultimodalInputConnectProxy::StopRemoteInput(const std::string& deviceId,
    uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
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
    WRITEINT32(data, inputAbility, ERR_INVALID_VALUE);
    if (!data.WriteRemoteObject(prepareDinput->AsObject().GetRefPtr())) {
        MMI_HILOGW("Failed to write monitor");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(STOP_DINPUT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGW("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

int32_t MultimodalInputConnectProxy::MarkEventProcessed(int32_t eventId)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, eventId, ERR_INVALID_VALUE);

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(MARK_EVENT_PROCESSED, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputHandler(int32_t handlerId, InputHandlerType handlerType,
    HandleEventType eventType)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, handlerId, ERR_INVALID_VALUE);
    WRITEINT32(data, handlerType, ERR_INVALID_VALUE);
    WRITEINT32(data, eventType, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(ADD_INPUT_HANDLER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, handlerId, ERR_INVALID_VALUE);
    WRITEINT32(data, handlerType, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(REMOVE_INPUT_HANDLER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MarkEventConsumed(int32_t monitorId, int32_t eventId)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, monitorId, ERR_INVALID_VALUE);
    WRITEINT32(data, eventId, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(MARK_EVENT_CONSUMED, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, offsetX, ERR_INVALID_VALUE);
    WRITEINT32(data, offsetY, ERR_INVALID_VALUE);

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(MOVE_MOUSE, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_LOG_ENTER;
    CHKPR(keyEvent, ERR_INVALID_VALUE);
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    if (!keyEvent->WriteToParcel(data)) {
        MMI_HILOGE("Failed to write inject event");
        return ERR_INVALID_VALUE;
    }
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(INJECT_KEY_EVENT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption)
{
    CALL_LOG_ENTER;
    CHKPR(keyOption, ERR_INVALID_VALUE);

    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, subscribeId, ERR_INVALID_VALUE);
    if (!keyOption->WriteToParcel(data)) {
        MMI_HILOGE("Failed to write key option");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SUBSCRIBE_KEY_EVENT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_LOG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, subscribeId, ERR_INVALID_VALUE);

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(UNSUBSCRIBE_KEY_EVENT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_LOG_ENTER;
    CHKPR(pointerEvent, ERR_INVALID_VALUE);
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    if (!pointerEvent->WriteToParcel(data)) {
        MMI_HILOGE("Failed to write inject point event");
        return ERR_INVALID_VALUE;
    }
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(INJECT_POINTER_EVENT, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
