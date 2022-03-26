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
#include "mmi_func_callback.h"
#include "bytrace.h"
#include "input_device_impl.h"
#include "input_event_data_transformation.h"
#include "input_event_monitor_manager.h"
#include "input_handler_manager.h"
#include "input_manager_impl.h"
#include "input_monitor_manager.h"
#include "interceptor_manager.h"
#include "mmi_client.h"
#include "multimodal_event_handler.h"
#include "proto.h"
#include "time_cost_chk.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "ClientMsgHandler"};
}

ClientMsgHandler::ClientMsgHandler()
{
    eventProcessedCallback_ = std::bind(&ClientMsgHandler::OnEventProcessed, std::placeholders::_1);
}

ClientMsgHandler::~ClientMsgHandler()
{
    eventProcessedCallback_ = std::function<void(int32_t)>();
}

bool ClientMsgHandler::Init()
{
    // LCOV_EXCL_START
    MsgCallback funs[] = {
        {MmiMessageId::ON_KEYEVENT, MsgCallbackBind2(&ClientMsgHandler::OnKeyEvent, this)},
        {MmiMessageId::ON_SUBSCRIBE_KEY, std::bind(&ClientMsgHandler::OnSubscribeKeyEventCallback,
                                                   this, std::placeholders::_1, std::placeholders::_2)},
        {MmiMessageId::ON_KEYMONITOR, MsgCallbackBind2(&ClientMsgHandler::OnKeyMonitor, this)},
        {MmiMessageId::ON_POINTER_EVENT, MsgCallbackBind2(&ClientMsgHandler::OnPointerEvent, this)},
        {MmiMessageId::ON_TOUCHPAD_MONITOR, MsgCallbackBind2(&ClientMsgHandler::OnTouchPadMonitor, this)},
        {MmiMessageId::GET_MMI_INFO_ACK, MsgCallbackBind2(&ClientMsgHandler::GetMultimodeInputInfo, this)},
        {MmiMessageId::INPUT_DEVICE, MsgCallbackBind2(&ClientMsgHandler::OnInputDevice, this)},
        {MmiMessageId::INPUT_DEVICE_IDS, MsgCallbackBind2(&ClientMsgHandler::OnInputDeviceIds, this)},
        {MmiMessageId::REPORT_KEY_EVENT, MsgCallbackBind2(&ClientMsgHandler::ReportKeyEvent, this)},
        {MmiMessageId::REPORT_POINTER_EVENT, MsgCallbackBind2(&ClientMsgHandler::ReportPointerEvent, this)},
        {MmiMessageId::TOUCHPAD_EVENT_INTERCEPTOR, MsgCallbackBind2(&ClientMsgHandler::TouchpadEventInterceptor, this)},
        {MmiMessageId::KEYBOARD_EVENT_INTERCEPTOR, MsgCallbackBind2(&ClientMsgHandler::KeyEventInterceptor, this)},
        {MmiMessageId::INPUT_VIRTUAL_DEVICE_IDS, MsgCallbackBind2(&ClientMsgHandler::OnInputVirtualDeviceIds, this)},
        {MmiMessageId::INPUT_VIRTUAL_DEVICE, MsgCallbackBind2(&ClientMsgHandler::OnInputVirtualDevice, this)},       
        {MmiMessageId::GET_ALL_NODE_DEVICE_INFO, MsgCallbackBind2(&ClientMsgHandler::OnGetAllNodeDeviceInfo, this)},    
        {MmiMessageId::SHOW_MOUSE, MsgCallbackBind2(&ClientMsgHandler::OnShowMouse, this)},
        {MmiMessageId::HIDE_MOUSE, MsgCallbackBind2(&ClientMsgHandler::OnHideMouse, this)},
        {MmiMessageId::INPUT_MOUSE_LOCATION, MsgCallbackBind2(&ClientMsgHandler::OnMouseLocation, this)},
        {MmiMessageId::INPUT_PREPARE_REMOTE, MsgCallbackBind2(&ClientMsgHandler::OnPrepareRemoteInput, this)},
        {MmiMessageId::INPUT_UNPREPARE_REMOTE, MsgCallbackBind2(&ClientMsgHandler::OnUnprepareRemoteInput, this)},
        {MmiMessageId::INPUT_START_REMOTE, MsgCallbackBind2(&ClientMsgHandler::OnStartRemoteInput, this)},
        {MmiMessageId::INPUT_STOP_REMOTE, MsgCallbackBind2(&ClientMsgHandler::OnStopRemoteInput, this)},
        {MmiMessageId::SIMULATE_CROSS_LOCATION, MsgCallbackBind2(&ClientMsgHandler::OnCrossLocation, this)},     
    };
    // LCOV_EXCL_STOP
    for (auto& it : funs) {
        CHKC(RegistrationEvent(it), EVENT_REG_FAIL);
    }
    return true;
}

void ClientMsgHandler::OnMsgHandler(const UDSClient& client, NetPacket& pkt)
{
    auto id = pkt.GetMsgId();
    TimeCostChk chk("ClientMsgHandler::OnMsgHandler", "overtime 300(us)", MAX_OVER_TIME, id);
    auto callback = GetMsgCallback(id);
    if (callback == nullptr) {
        MMI_LOGE("Unknown msg id:%{public}d", id);
        return;
    }
    auto ret = (*callback)(client, pkt);
    if (ret < 0) {
        MMI_LOGE("Msg handling failed. id:%{public}d,ret:%{public}d", id, ret);
        return;
    }
}

int32_t ClientMsgHandler::OnKeyMonitor(const UDSClient& client, NetPacket& pkt)
{
    auto key = KeyEvent::Create();
    CHKPR(key, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, key);
    if (ret != RET_OK) {
        MMI_LOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid;
    pkt >> pid;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    MMI_LOGD("Client receive the msg from server, keyCode:%{public}d,pid:%{public}d", key->GetKeyCode(), pid);
    return InputMonitorMgr.OnMonitorInputEvent(key);
}

int32_t ClientMsgHandler::OnKeyEvent(const UDSClient& client, NetPacket& pkt)
{
    int32_t fd = 0;
    uint64_t serverStartTime = 0;
    auto key = KeyEvent::Create();
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, key);
    if (ret != RET_OK) {
        MMI_LOGE("read netPacket failed");
        return RET_ERR;
    }
    pkt >> fd >> serverStartTime;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    MMI_LOGD("key event dispatcher of client, KeyCode:%{public}d,"
             "ActionTime:%{public}" PRId64 ",Action:%{public}d,ActionStartTime:%{public}" PRId64 ","
             "EventType:%{public}d,Flag:%{public}u,"
             "KeyAction:%{public}d,eventNumber:%{public}d,Fd:%{public}d,"
             "ServerStartTime:%{public}" PRId64"",
             key->GetKeyCode(), key->GetActionTime(), key->GetAction(),
             key->GetActionStartTime(), key->GetEventType(),
             key->GetFlag(), key->GetKeyAction(), key->GetId(), fd, serverStartTime);
    int32_t keyId = key->GetId();
    std::string keyCodestring = "KeyEventDispatch";
    StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, keyCodestring, keyId);
    int32_t getKeyCode = key->GetKeyCode();
    keyCodestring = "client dispatchKeyCode=" + std::to_string(getKeyCode);
    BYTRACE_NAME(BYTRACE_TAG_MULTIMODALINPUT, keyCodestring);
    key->SetProcessedCallback(eventProcessedCallback_);
    InputManagerImpl::GetInstance()->OnKeyEvent(key);
    key->MarkProcessed();
    return RET_OK;
}

int32_t ClientMsgHandler::OnPointerEvent(const UDSClient& client, NetPacket& pkt)
{
    auto pointerEvent { PointerEvent::Create() };
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_LOGE("Failed to deserialize pointer event.");
        return RET_ERR;
    }

    std::vector<int32_t> pointerIds { pointerEvent->GetPointersIdList() };
    MMI_LOGD("Pointer event dispatcher of client, eventType:%{public}s, actionTime:%{public}" PRId64 ", "
             "action:%{public}d, actionStartTime:%{public}" PRId64 ", flag:%{public}u, pointerAction:%{public}s, "
             "sourceType:%{public}s, VerticalAxisValue:%{public}.2f, HorizontalAxisValue:%{public}.2f, "
             "PinchAxisValue:%{public}.2f, pointerCount:%{public}zu, eventNumber:%{public}d",
             pointerEvent->DumpEventType(), pointerEvent->GetActionTime(), pointerEvent->GetAction(),
             pointerEvent->GetActionStartTime(), pointerEvent->GetFlag(), pointerEvent->DumpPointerAction(),
             pointerEvent->DumpSourceType(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
             pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
             pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH),
             pointerIds.size(), pointerEvent->GetId());
    std::vector<int32_t> pressedKeys = pointerEvent->GetPressedKeys();
    for (auto &item : pressedKeys) {
        MMI_LOGI("Pressed keyCode:%{public}d", item);
    }
    for (auto &pointerId : pointerIds) {
        PointerEvent::PointerItem item;
        CHKR(pointerEvent->GetPointerItem(pointerId, item), PARAM_INPUT_FAIL, RET_ERR);

        MMI_LOGD("DownTime:%{public}" PRId64 ",isPressed:%{public}s,"
                "globalX:%{public}d,globalY:%{public}d,localX:%{public}d,localY:%{public}d,"
                "width:%{public}d,height:%{public}d,pressure:%{public}d",
                 item.GetDownTime(), (item.IsPressed() ? "true" : "false"),
                 item.GetGlobalX(), item.GetGlobalY(), item.GetLocalX(), item.GetLocalY(),
                 item.GetWidth(), item.GetHeight(), item.GetPressure());
    }
    if (PointerEvent::POINTER_ACTION_CANCEL == pointerEvent->GetPointerAction()) {
        MMI_LOGD("Operation canceled.");
    }
    pointerEvent->SetProcessedCallback(eventProcessedCallback_);
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
        int32_t pointerId = pointerEvent->GetId();
        std::string pointerEventstring = "PointerEventDispatch";
        StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, pointerEventstring, pointerId);
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        int32_t touchId = pointerEvent->GetId();
        std::string touchEvent = "touchEventDispatch";
        StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, touchEvent, touchId);
    }
    InputManagerImpl::GetInstance()->OnPointerEvent(pointerEvent);
    return RET_OK;
}

int32_t ClientMsgHandler::OnSubscribeKeyEventCallback(const UDSClient &client, NetPacket &pkt)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent);
    if (ret != RET_OK) {
        MMI_LOGE("read net packet failed");
        return RET_ERR;
    }
    int32_t fd = -1;
    int32_t subscribeId = -1;
    pkt >> fd >> subscribeId;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    MMI_LOGD("Subscribe:%{public}d,Fd:%{public}d,KeyEvent:%{public}d,"
             "KeyCode:%{public}d,ActionTime:%{public}" PRId64 ",ActionStartTime:%{public}" PRId64 ","
             "Action:%{public}d,KeyAction:%{public}d,EventType:%{public}d,Flag:%{public}u",
        subscribeId, fd, keyEvent->GetId(), keyEvent->GetKeyCode(), keyEvent->GetActionTime(),
        keyEvent->GetActionStartTime(), keyEvent->GetAction(), keyEvent->GetKeyAction(),
        keyEvent->GetEventType(), keyEvent->GetFlag());
    int32_t keyId = keyEvent->GetId();
    std::string keyEventString = "keyEventSubscribe";
    StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, keyEventString, keyId);
    int32_t keyCode = keyEvent->GetKeyCode();
    keyEventString = "client subscribe keyCode=" + std::to_string(keyCode);
    BYTRACE_NAME(BYTRACE_TAG_MULTIMODALINPUT, keyEventString);
    return KeyEventInputSubscribeMgr.OnSubscribeKeyEventCallback(keyEvent, subscribeId);
}

int32_t ClientMsgHandler::OnTouchPadMonitor(const UDSClient& client, NetPacket& pkt)
{
    auto pointer = PointerEvent::Create();
    int32_t ret = InputEventDataTransformation::Unmarshalling(pkt, pointer);
    if (ret != RET_OK) {
        MMI_LOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid = 0;
    pkt >> pid;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    MMI_LOGD("client receive the msg from server: EventType:%{public}d,pid:%{public}d",
        pointer->GetEventType(), pid);
    return InputMonitorMgr.OnTouchpadMonitorInputEvent(pointer);
}

int32_t ClientMsgHandler::GetMultimodeInputInfo(const UDSClient& client, NetPacket& pkt)
{
    TagPackHead tagPackHeadAck;
    pkt >> tagPackHeadAck;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    std::cout << "GetMultimodeInputInfo: The client fd is " << tagPackHeadAck.sizeEvent[0] << std::endl;
    return RET_OK;
}

int32_t ClientMsgHandler::OnInputDeviceIds(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGD("enter");
    int32_t userData;
    int32_t size;
    std::vector<int32_t> inputDeviceIds;
    CHKR(pkt.Read(userData), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(size), STREAM_BUF_READ_FAIL, RET_ERR);
    for (int32_t i = 0; i < size; i++) {
        int32_t deviceId = 0;
        CHKR(pkt.Read(deviceId), STREAM_BUF_READ_FAIL, RET_ERR);
        inputDeviceIds.push_back(deviceId);
    }
    InputDeviceImpl::GetInstance().OnInputDeviceIds(userData, inputDeviceIds);
    return RET_OK;
}

int32_t ClientMsgHandler::OnInputDevice(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGD("enter");
    int32_t userData;
    int32_t id;
    std::string name;
    int32_t deviceType;
    CHKR(pkt.Read(userData), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(id), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(name), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(deviceType), STREAM_BUF_READ_FAIL, RET_ERR);

    InputDeviceImpl::GetInstance().OnInputDevice(userData, id, name, deviceType);
    return RET_OK;
}

int32_t ClientMsgHandler::ReportKeyEvent(const UDSClient& client, NetPacket& pkt)
{
    int32_t handlerId;
    CHKR(pkt.Read(handlerId), STREAM_BUF_READ_FAIL, RET_ERR);
    auto keyEvent = KeyEvent::Create();
    if (InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent) != ERR_OK) {
        MMI_LOGE("Failed to deserialize key event.");
        return RET_ERR;
    }
    InputHandlerManager::GetInstance().OnInputEvent(handlerId, keyEvent);
    return RET_OK;
}

int32_t ClientMsgHandler::ReportPointerEvent(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGD("enter");
    int32_t handlerId;
    InputHandlerType handlerType;
    CHKR(pkt.Read(handlerId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(handlerType), STREAM_BUF_READ_FAIL, RET_ERR);
    MMI_LOGD("Client handlerId:%{public}d,handlerType:%{public}d", handlerId, handlerType);
    auto pointerEvent { PointerEvent::Create() };
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_LOGE("Failed to deserialize pointer event");
        return RET_ERR;
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
        int32_t pointerId = pointerEvent->GetId();
        std::string pointerEventString = "pointerEventFilter";
        StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, pointerEventString, pointerId);
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        int32_t touchId = pointerEvent->GetId();
        std::string touchEventString = "touchEventFilter";
        StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, touchEventString, touchId);
    }
    InputHandlerManager::GetInstance().OnInputEvent(handlerId, pointerEvent);
    return RET_OK;
}

int32_t ClientMsgHandler::TouchpadEventInterceptor(const UDSClient& client, NetPacket& pkt)
{
    auto pointerEvent = PointerEvent::Create();
    int32_t ret = InputEventDataTransformation::Unmarshalling(pkt, pointerEvent);
    if (ret != RET_OK) {
        MMI_LOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid = 0;
    int32_t id = 0;
    pkt >> pid >> id;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);
    MMI_LOGD("client receive the msg from server: pointId:%{public}d,pid:%{public}d",
             pointerEvent->GetPointerId(), pid);
    return InterceptorMgr.OnPointerEvent(pointerEvent, id);
}

int32_t ClientMsgHandler::KeyEventInterceptor(const UDSClient& client, NetPacket& pkt)
{
    auto keyEvent = KeyEvent::Create();
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent);
    if (ret != RET_OK) {
        MMI_LOGE("read netPacket failed");
        return RET_ERR;
    }
    int32_t pid = 0;
    pkt >> pid;
    CHKR(!pkt.ChkRWError(), PACKET_READ_FAIL, PACKET_READ_FAIL);

    int32_t keyId = keyEvent->GetId();
    std::string keyEventString = "keyEventFilter";
    StartAsyncTrace(BYTRACE_TAG_MULTIMODALINPUT, keyEventString, keyId);
    int32_t keyCode = keyEvent->GetKeyCode();
    keyEventString = "client filter keyCode=" + std::to_string(keyCode);
    BYTRACE_NAME(BYTRACE_TAG_MULTIMODALINPUT, keyEventString);
    MMI_LOGD("client receive the msg from server: keyCode:%{public}d,pid:%{public}d",
        keyEvent->GetKeyCode(), pid);
    return InterceptorMgr.OnKeyEvent(keyEvent);
}

void ClientMsgHandler::OnEventProcessed(int32_t eventId)
{
    MMIClientPtr client = MMIEventHdl.GetMMIClient();
    CHKPV(client);
    NetPacket pkt(MmiMessageId::NEW_CHECK_REPLY_MESSAGE);
    pkt << eventId;
    CHK(client->SendMessage(pkt), MSG_SEND_FAIL);
}

int32_t OHOS::MMI::ClientMsgHandler::OnInputVirtualDeviceIds(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnInputVirtualDeviceIds enter");
    int32_t taskId;
    int32_t size = 0;
    std::vector<int32_t> virtualDeviceIds;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(size), STREAM_BUF_READ_FAIL, RET_ERR);
    for (int32_t i = 0; i < size; i++) {
        int32_t deviceId = 0;
        CHKR(pkt.Read(deviceId), STREAM_BUF_READ_FAIL, RET_ERR);
        virtualDeviceIds.push_back(deviceId);
    }
    InputManagerImpl::GetInstance()->OnVirtualDeviceIds(taskId, virtualDeviceIds);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnInputVirtualDevice(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnInputVirtualDevice enter");
    int32_t taskId;
    int32_t id;
    std::string name;
    int32_t deviceType;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(id), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(name), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(deviceType), STREAM_BUF_READ_FAIL, RET_ERR);  

    InputManagerImpl::GetInstance()->OnVirtualDevice(taskId, id, name, deviceType);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnGetAllNodeDeviceInfo(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnGetAllNodeDeviceInfo enter");
    int32_t taskId;
    int32_t size = 0;
    std::vector<std::string> deviceInfos;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(size), STREAM_BUF_READ_FAIL, RET_ERR);
    for (int32_t i = 0; i < size; i++) {
        std::string deviceId = "";
        CHKR(pkt.Read(deviceId), STREAM_BUF_READ_FAIL, RET_ERR);
        deviceInfos.push_back(deviceId);
    }
    InputManagerImpl::GetInstance()->OnGetAllNodeDeviceInfo(taskId, deviceInfos);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnShowMouse(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnShowMouse enter");
    int32_t taskId;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);

    auto& instance = InputDeviceImpl::GetInstance();
    instance.OnShowMouse(taskId);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnHideMouse(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnHideMouse enter");
    int32_t taskId;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);

    auto& instance = InputDeviceImpl::GetInstance();
    instance.OnHideMouse(taskId);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnMouseLocation(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnMouseLocation enter");
    int32_t taskId;
    auto mouseLocation = std::make_shared<DMouseLocation>();
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->globalX), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->globalY), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->displayId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->dx), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->dy), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->logicalDisplayWidth), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->logicalDisplayHeight), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->logicalDisplayTopLeftX), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(mouseLocation->logicalDisplayTopLeftY), STREAM_BUF_READ_FAIL, RET_ERR);
    InputManagerImpl::GetInstance()->OnMouseLocation(taskId, mouseLocation);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnPrepareRemoteInput(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnPrepareRemoteInput enter");
    int32_t taskId;
    int32_t dinputState;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(dinputState), STREAM_BUF_READ_FAIL, RET_ERR);
    InputManagerImpl::GetInstance()->OnPrepareRemoteInput(taskId, dinputState);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnUnprepareRemoteInput(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnUnprepareRemoteInput enter");
    int32_t taskId;
    int32_t dinputState;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(dinputState), STREAM_BUF_READ_FAIL, RET_ERR);
    InputManagerImpl::GetInstance()->OnUnprepareRemoteInput(taskId, dinputState);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnStartRemoteInput(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnStartRemoteInput enter");
    int32_t taskId;
    int32_t dinputState;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(dinputState), STREAM_BUF_READ_FAIL, RET_ERR);

    InputManagerImpl::GetInstance()->OnStartRemoteInput(taskId, dinputState);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnStopRemoteInput(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnStopRemoteInput enter");
    int32_t taskId;
    int32_t dinputState;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(dinputState), STREAM_BUF_READ_FAIL, RET_ERR);

    InputManagerImpl::GetInstance()->OnStopRemoteInput(taskId, dinputState);
    return RET_OK;
}

int32_t OHOS::MMI::ClientMsgHandler::OnCrossLocation(const UDSClient& client, NetPacket& pkt)
{
    MMI_LOGT("ClientMsgHandler::OnCrossLocation enter");
    int32_t taskId = 0;
    int32_t status = 0;
    CHKR(pkt.Read(taskId), STREAM_BUF_READ_FAIL, RET_ERR);
    CHKR(pkt.Read(status), STREAM_BUF_READ_FAIL, RET_ERR);

    InputManagerImpl::GetInstance()->OnCrossLocation(taskId, status);
    return RET_OK;
}

} // namespace MMI
} // namespace OHOS
