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

#include "client_msg_handler.h"

#include <cinttypes>
#include <iostream>
#include <sstream>

#include "bytrace_adapter.h"
#include "define_interceptor_manager.h"
#include "input_device_impl.h"
#include "input_event_data_transformation.h"
#include "input_event_monitor_manager.h"
#include "input_handler_manager.h"
#include "input_manager_impl.h"
#include "input_monitor_manager.h"
#include "mmi_client.h"
#include "mmi_func_callback.h"
#include "multimodal_event_handler.h"
#include "proto.h"
#include "time_cost_chk.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "ClientMsgHandler"};
} // namespace

ClientMsgHandler::ClientMsgHandler()
{
    eventProcessedCallback_ = std::bind(&ClientMsgHandler::OnEventProcessed, std::placeholders::_1);
}

ClientMsgHandler::~ClientMsgHandler()
{
    eventProcessedCallback_ = std::function<void(int32_t)>();
}

void ClientMsgHandler::Init()
{
    MsgCallback funs[] = {
        {MmiMessageId::ON_KEYEVENT, MsgCallbackBind2(&ClientMsgHandler::OnKeyEvent, this)},
        {MmiMessageId::ON_SUBSCRIBE_KEY, std::bind(&ClientMsgHandler::OnSubscribeKeyEventCallback,
                                                   this, std::placeholders::_1, std::placeholders::_2)},
        {MmiMessageId::ON_KEYMONITOR, MsgCallbackBind2(&ClientMsgHandler::OnKeyMonitor, this)},
        {MmiMessageId::ON_POINTER_EVENT, MsgCallbackBind2(&ClientMsgHandler::OnPointerEvent, this)},
        {MmiMessageId::ON_TOUCHPAD_MONITOR, MsgCallbackBind2(&ClientMsgHandler::OnTouchPadMonitor, this)},
        {MmiMessageId::INPUT_DEVICE, MsgCallbackBind2(&ClientMsgHandler::OnInputDevice, this)},
        {MmiMessageId::INPUT_DEVICE_IDS, MsgCallbackBind2(&ClientMsgHandler::OnInputDeviceIds, this)},
        {MmiMessageId::INPUT_DEVICE_KEYSTROKE_ABILITY, MsgCallbackBind2(&ClientMsgHandler::OnSupportKeys, this)},
        {MmiMessageId::INPUT_DEVICE_KEYBOARD_TYPE, MsgCallbackBind2(&ClientMsgHandler::OnInputKeyboardType, this)},
        {MmiMessageId::ADD_INPUT_DEVICE_MONITOR, MsgCallbackBind2(&ClientMsgHandler::OnDevMonitor, this)},
        {MmiMessageId::REPORT_KEY_EVENT, MsgCallbackBind2(&ClientMsgHandler::ReportKeyEvent, this)},
        {MmiMessageId::REPORT_POINTER_EVENT, MsgCallbackBind2(&ClientMsgHandler::ReportPointerEvent, this)},
    };
    for (auto& it : funs) {
        if (!RegistrationEvent(it)) {
            MMI_HILOGW("Failed to register event errCode:%{public}d", EVENT_REG_FAIL);
            continue;
        }
    }
}

void ClientMsgHandler::OnMsgHandler(const UDSClient& client, NetPacket& pkt)
{
    auto id = pkt.GetMsgId();
    TimeCostChk chk("ClientMsgHandler::OnMsgHandler", "overtime 300(us)", MAX_OVER_TIME, id);
    auto callback = GetMsgCallback(id);
    if (callback == nullptr) {
        MMI_HILOGE("Unknown msg id:%{public}d", id);
        return;
    }
    auto ret = (*callback)(client, pkt);
    if (ret < 0) {
        MMI_HILOGE("Msg handling failed. id:%{public}d,ret:%{public}d", id, ret);
        return;
    }
}

int32_t ClientMsgHandler::OnKeyMonitor(const UDSClient& client, NetPacket& pkt)
{
    auto key = KeyEvent::Create();
    CHKPR(key, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, key);
    if (ret != RET_OK) {
        MMI_HILOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid;
    pkt >> pid;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read pid failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGD("Client receive the msg from server, keyCode:%{public}d,pid:%{public}d", key->GetKeyCode(), pid);
    return InputMonitorMgr.OnMonitorInputEvent(key);
}

int32_t ClientMsgHandler::OnKeyEvent(const UDSClient& client, NetPacket& pkt)
{
    auto key = KeyEvent::Create();
    CHKPR(key, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, key);
    if (ret != RET_OK) {
        MMI_HILOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t fd = 0;
    pkt >> fd;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read fd failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGD("key event dispatcher of client, KeyCode:%{public}d,"
               "ActionTime:%{public}" PRId64 ",Action:%{public}d,ActionStartTime:%{public}" PRId64 ","
               "EventType:%{public}d,Flag:%{public}u,"
               "KeyAction:%{public}d,eventNumber:%{public}d,Fd:%{public}d,",
               key->GetKeyCode(), key->GetActionTime(), key->GetAction(),
               key->GetActionStartTime(), key->GetEventType(),
               key->GetFlag(), key->GetKeyAction(), key->GetId(), fd);
    BytraceAdapter::StartBytrace(key, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_DISPATCH_EVENT);
    key->SetProcessedCallback(eventProcessedCallback_);
    InputMgrImpl->OnKeyEvent(key);
    key->MarkProcessed();
    return RET_OK;
}

int32_t ClientMsgHandler::OnPointerEvent(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize pointer event.");
        return RET_ERR;
    }
    MMI_HILOGD("Pointer event dispatcher of client:");
    std::stringstream sStream;
    sStream << *pointerEvent;
    std::string sLine;
    while (std::getline(sStream, sLine)) {
        MMI_HILOGD("%{public}s", sLine.c_str());
    }
    if (PointerEvent::POINTER_ACTION_CANCEL == pointerEvent->GetPointerAction()) {
        MMI_HILOGD("Operation canceled.");
    }
    pointerEvent->SetProcessedCallback(eventProcessedCallback_);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START, BytraceAdapter::POINT_DISPATCH_EVENT);
    InputMgrImpl->OnPointerEvent(pointerEvent);
    return RET_OK;
}

int32_t ClientMsgHandler::OnSubscribeKeyEventCallback(const UDSClient &client, NetPacket &pkt)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("read net packet failed");
        return RET_ERR;
    }
    int32_t fd = -1;
    int32_t subscribeId = -1;
    pkt >> fd >> subscribeId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read fd failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGD("Subscribe:%{public}d,Fd:%{public}d,KeyEvent:%{public}d,"
               "KeyCode:%{public}d,ActionTime:%{public}" PRId64 ",ActionStartTime:%{public}" PRId64 ","
               "Action:%{public}d,KeyAction:%{public}d,EventType:%{public}d,Flag:%{public}u",
        subscribeId, fd, keyEvent->GetId(), keyEvent->GetKeyCode(), keyEvent->GetActionTime(),
        keyEvent->GetActionStartTime(), keyEvent->GetAction(), keyEvent->GetKeyAction(),
        keyEvent->GetEventType(), keyEvent->GetFlag());
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_SUBSCRIBE_EVENT);
    return KeyEventInputSubscribeMgr.OnSubscribeKeyEventCallback(keyEvent, subscribeId);
}

int32_t ClientMsgHandler::OnTouchPadMonitor(const UDSClient& client, NetPacket& pkt)
{
    auto pointer = PointerEvent::Create();
    CHKPR(pointer, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::Unmarshalling(pkt, pointer);
    if (ret != RET_OK) {
        MMI_HILOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid = 0;
    pkt >> pid;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read pid failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGD("client receive the msg from server: EventType:%{public}d,pid:%{public}d",
        pointer->GetEventType(), pid);
    return InputMonitorMgr.OnTouchpadMonitorInputEvent(pointer);
}

int32_t ClientMsgHandler::OnInputDeviceIds(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t userData;
    std::vector<int32_t> inputDeviceIds;
    pkt >> userData >> inputDeviceIds;
    if (inputDeviceIds.size() > MAX_INPUT_DEVICE) {
        MMI_HILOGE("Device exceeds the max range");
        return RET_ERR;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read device Data failed");
        return RET_ERR;
    }
    InputDevImpl.OnInputDeviceIds(userData, inputDeviceIds);
    return RET_OK;
}

int32_t ClientMsgHandler::OnInputDevice(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t userData;
    size_t size;
    auto devData = std::make_shared<InputDeviceImpl::InputDeviceInfo>();
    pkt >> userData >> devData->id >> devData->name >> devData->deviceType >> devData->busType >> devData->product
        >> devData->vendor >> devData->version >> devData->phys >> devData->uniq >> size;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read basic data failed");
        return PACKET_READ_FAIL;
    }
    std::vector<InputDeviceImpl::AxisInfo> axisInfo;
    InputDeviceImpl::AxisInfo axis;
    for (size_t i = 0; i < size; ++i) {
        pkt >> axis.axisType >> axis.min >> axis.max >> axis.fuzz >> axis.flat >> axis.resolution;
        if (pkt.ChkRWError()) {
            MMI_HILOGE("Packet read axis data failed");
            return PACKET_READ_FAIL;
        }
        axisInfo.push_back(axis);
    }
    devData->axis = axisInfo;
    CHKPR(devData, RET_ERR);
    InputDevImpl.OnInputDevice(userData, devData);
    return RET_OK;
}

int32_t ClientMsgHandler::OnSupportKeys(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t userData;
    size_t size;
    pkt >> userData >> size;
    if (size > MAX_SUPPORT_KEY) {
        MMI_HILOGE("Keys exceeds the max range");
        return RET_ERR;
    }
    std::vector<bool> abilityRet;
    bool ret;
    for (size_t i = 0; i < size; ++i) {
        pkt >> ret;
        abilityRet.push_back(ret);
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read key Data failed");
        return RET_ERR;
    }
    InputDevImpl.OnSupportKeys(userData, abilityRet);
    return RET_OK;
}

int32_t ClientMsgHandler::OnInputKeyboardType(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t userData;
    int32_t KeyboardType;
    pkt >> userData >> KeyboardType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read failed");
        return PACKET_WRITE_FAIL;
    }
    InputDevImpl.OnKeyboardType(userData, KeyboardType);
    return RET_OK;
}

int32_t ClientMsgHandler::OnDevMonitor(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    std::string type;
    int32_t deviceId;
    pkt >> type >> deviceId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read type failed");
        return RET_ERR;
    }
    InputDeviceImpl::GetInstance().OnDevMonitor(type, deviceId);
    return RET_OK;
}

int32_t ClientMsgHandler::ReportKeyEvent(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t handlerId;
    pkt >> handlerId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read handler failed");
        return RET_ERR;
    }
    auto keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize key event.");
        return RET_ERR;
    }
    InputHandlerManager::GetInstance().OnInputEvent(handlerId, keyEvent);
    return RET_OK;
}

int32_t ClientMsgHandler::ReportPointerEvent(const UDSClient& client, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    int32_t handlerId;
    InputHandlerType handlerType;
    pkt >> handlerId >> handlerType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read Pointer data failed");
        return RET_ERR;
    }
    MMI_HILOGD("Client handlerId:%{public}d,handlerType:%{public}d", handlerId, handlerType);
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize pointer event");
        return RET_ERR;
    }
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START, BytraceAdapter::POINT_INTERCEPT_EVENT);
    InputHandlerManager::GetInstance().OnInputEvent(handlerId, pointerEvent);
    return RET_OK;
}

void ClientMsgHandler::OnEventProcessed(int32_t eventId)
{
    MMIClientPtr client = MMIEventHdl.GetMMIClient();
    CHKPV(client);
    NetPacket pkt(MmiMessageId::MARK_PROCESS);
    pkt << eventId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write event failed");
        return;
    }
    if (!client->SendMessage(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}
} // namespace MMI
} // namespace OHOS
