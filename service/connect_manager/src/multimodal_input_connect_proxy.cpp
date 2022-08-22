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

MultimodalInputConnectProxy::MultimodalInputConnectProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IMultimodalInputConnect>(impl)
{
    MMI_HILOGI("Enter MultimodalInputConnectProxy");
}

MultimodalInputConnectProxy::~MultimodalInputConnectProxy()
{
    MMI_HILOGI("Enter ~MultimodalInputConnectProxy");
}

int32_t MultimodalInputConnectProxy::AllocSocketFd(const std::string &programName,
    const int32_t moduleType, int32_t &socketFd, int32_t &tokenType)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    socketFd = reply.ReadFileDescriptor();
    READINT32(reply, tokenType, IPC_PROXY_DEAD_OBJECT_ERR);
    MMI_HILOGD("socketFd:%{public}d tokenType:%{public}d", socketFd, tokenType);
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputEventFilter(sptr<IEventFilter> filter)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Reply readint32 error:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerVisible(bool visible)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::IsPointerVisible(bool &visible)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    visible = reply.ReadBool();
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerSpeed(int32_t speed)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, speed, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SET_POINTER_SPEED, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetPointerSpeed(int32_t &speed)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(GET_POINTER_SPEED, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    speed = reply.ReadInt32();
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RegisterDevListener()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(REGISTER_DEV_MONITOR, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::UnregisterDevListener()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(UNREGISTER_DEV_MONITOR, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SupportKeys(int32_t userData, int32_t deviceId, std::vector<int32_t> &keys)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return RET_ERR;
    }
    WRITEINT32(data, userData);
    WRITEINT32(data, deviceId);
    WRITEINT32(data, static_cast<int32_t>(keys.size()));
    for (const auto &item : keys) {
        WRITEINT32(data, item);
    }

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SUPPORT_KEYS, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetDeviceIds(int32_t userData)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return RET_ERR;
    }
    WRITEINT32(data, userData);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(GET_DEVICE_IDS, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetDevice(int32_t userData, int32_t deviceId)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return RET_ERR;
    }
    WRITEINT32(data, userData);
    WRITEINT32(data, deviceId);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(GET_DEVICE, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetKeyboardType(int32_t userData, int32_t deviceId)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return RET_ERR;
    }
    WRITEINT32(data, userData);
    WRITEINT32(data, deviceId);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(GET_KEYBOARD_TYPE, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("send request fail, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputHandler(InputHandlerType handlerType,
    HandleEventType eventType)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, handlerType, ERR_INVALID_VALUE);
    WRITEUINT32(data, eventType, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(ADD_INPUT_HANDLER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    WRITEINT32(data, handlerType, ERR_INVALID_VALUE);
    WRITEUINT32(data, eventType, ERR_INVALID_VALUE);
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(REMOVE_INPUT_HANDLER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MarkEventConsumed(int32_t eventId)
{
    CALL_DEBUG_ENTER;
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
    int32_t ret = remote->SendRequest(MARK_EVENT_CONSUMED, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, result:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
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
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetAnrObserver()
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    int32_t ret = remote->SendRequest(SET_ANR_OBSERVER, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::WriteWindowsVecToParcel(const std::shared_ptr<DisplayGroupInfo> displayGroupInfo,
                                                             MessageParcel& data)
{
    int32_t windowsNum = static_cast<int32_t>(displayGroupInfo->windowsInfo.size());
    WRITEINT32(data, windowsNum, ERR_INVALID_VALUE);
    for (const auto& item : displayGroupInfo->windowsInfo) {
        WRITEINT32(data, item.id, ERR_INVALID_VALUE);
        WRITEINT32(data, item.pid, ERR_INVALID_VALUE);
        WRITEINT32(data, item.uid, ERR_INVALID_VALUE);
        WRITEINT32(data, item.area.x, ERR_INVALID_VALUE);
        WRITEINT32(data, item.area.y, ERR_INVALID_VALUE);
        WRITEINT32(data, item.area.width, ERR_INVALID_VALUE);
        WRITEINT32(data, item.area.height, ERR_INVALID_VALUE);
        WRITEINT32(data, static_cast<int32_t>(item.defaultHotAreas.size()));
        for (const auto& i : item.defaultHotAreas) {
            WRITEINT32(data, i.x, ERR_INVALID_VALUE);
            WRITEINT32(data, i.y, ERR_INVALID_VALUE);
            WRITEINT32(data, i.width, ERR_INVALID_VALUE);
            WRITEINT32(data, i.height, ERR_INVALID_VALUE);
        }
        WRITEINT32(data, static_cast<int32_t>(item.pointerHotAreas.size()));
        for (const auto& i : item.pointerHotAreas) {
            WRITEINT32(data, i.x, ERR_INVALID_VALUE);
            WRITEINT32(data, i.y, ERR_INVALID_VALUE);
            WRITEINT32(data, i.width, ERR_INVALID_VALUE);
            WRITEINT32(data, i.height, ERR_INVALID_VALUE);
        }
        WRITEINT32(data, item.agentWindowId, ERR_INVALID_VALUE);
        WRITEUINT32(data, item.flags, ERR_INVALID_VALUE);
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::WriteDisplayVecToParcel(const std::shared_ptr<DisplayGroupInfo> displayGroupInfo,
                                                             MessageParcel& data)
{
    int32_t displayNum = static_cast<int32_t>(displayGroupInfo->displaysInfo.size());
    WRITEINT32(data, displayNum, ERR_INVALID_VALUE);
    for (const auto& item : displayGroupInfo->displaysInfo) {
        WRITEINT32(data, item.id, ERR_INVALID_VALUE);
        WRITEINT32(data, item.x, ERR_INVALID_VALUE);
        WRITEINT32(data, item.y, ERR_INVALID_VALUE);
        WRITEINT32(data, item.width, ERR_INVALID_VALUE);
        WRITEINT32(data, item.height, ERR_INVALID_VALUE);
        WRITESTRING(data, item.name, ERR_INVALID_VALUE);
        WRITESTRING(data, item.uniq, ERR_INVALID_VALUE);
        WRITEINT32(data, static_cast<uint32_t>(item.direction), ERR_INVALID_VALUE);
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::WriteDisplayInfoToParcel(const std::shared_ptr<DisplayGroupInfo> displayGroupInfo,
                                                              MessageParcel& data)
{
    WRITEINT32(data, displayGroupInfo->width, ERR_INVALID_VALUE);
    WRITEINT32(data, displayGroupInfo->height, ERR_INVALID_VALUE);
    WRITEINT32(data, displayGroupInfo->focusWindowId, ERR_INVALID_VALUE);
    int32_t ret = WriteWindowsVecToParcel(displayGroupInfo, data);
    ret += WriteDisplayVecToParcel(displayGroupInfo, data);
    if (ret != RET_OK) {
        MMI_HILOGE("Failed to write display info to parcel!");
    }
    return ret;
}

int32_t MultimodalInputConnectProxy::UpdateDisplayInfo(const std::shared_ptr<DisplayGroupInfo> displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    if (!data.WriteInterfaceToken(MultimodalInputConnectProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return ERR_INVALID_VALUE;
    }
    int32_t ret = WriteDisplayInfoToParcel(displayGroupInfo, data);
    if (ret != RET_OK) {
        MMI_HILOGE("Failed to write display info");
        return ret;
    }
    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    ret = remote->SendRequest(UPDATE_DISPLAY_INFO, data, reply, option);
    if (ret != RET_OK) {
        MMI_HILOGE("Send request failed, ret:%{public}d", ret);
    }
    return ret;
}
} // namespace MMI
} // namespace OHOS
