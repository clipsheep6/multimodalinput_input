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

#include "server_msg_handler.h"

#include <cinttypes>

#include "define_interceptor_global.h"
#include "event_dump.h"
#include "event_package.h"
#include "hos_key_event.h"
#include "input_device_manager.h"
#include "input_event.h"
#include "input_event_data_transformation.h"
#include "input_event_monitor_manager.h"
#include "input_handler_manager_global.h"
#include "input_windows_manager.h"
#include "key_event_subscriber.h"
#include "mmi_func_callback.h"
#include "time_cost_chk.h"
#include "mouse_event_handler.h"
#ifdef OHOS_BUILD_HDF
#include "hdi_inject.h"
#endif

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ServerMsgHandler" };
} // namespace

ServerMsgHandler::ServerMsgHandler() {}

ServerMsgHandler::~ServerMsgHandler() {}

void ServerMsgHandler::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
#ifdef OHOS_BUILD_HDF
    if (!(MMIHdiInject->Init(udsServer))) {
        MMI_HILOGE("Input device initialization failed");
        return;
    }
#endif
    MsgCallback funs[] = {
        {MmiMessageId::DISPLAY_INFO, MsgCallbackBind2(&ServerMsgHandler::OnDisplayInfo, this)},
        {MmiMessageId::ADD_INPUT_EVENT_MONITOR, MsgCallbackBind2(&ServerMsgHandler::OnAddInputEventMontior, this)},
        {MmiMessageId::REMOVE_INPUT_EVENT_MONITOR, MsgCallbackBind2(&ServerMsgHandler::OnRemoveInputEventMontior, this)},
        {MmiMessageId::ADD_INPUT_EVENT_TOUCHPAD_MONITOR,
            MsgCallbackBind2(&ServerMsgHandler::OnAddInputEventTouchpadMontior, this)},
        {MmiMessageId::REMOVE_INPUT_EVENT_TOUCHPAD_MONITOR,
            MsgCallbackBind2(&ServerMsgHandler::OnRemoveInputEventTouchpadMontior, this)},
#ifdef OHOS_BUILD_MMI_DEBUG
        {MmiMessageId::BIGPACKET_TEST, MsgCallbackBind2(&ServerMsgHandler::OnBigPacketTest, this)},
#endif // OHOS_BUILD_MMI_DEBUG
#ifdef OHOS_BUILD_HDF
        {MmiMessageId::HDI_INJECT, MsgCallbackBind2(&ServerMsgHandler::OnHdiInject, this)},
#endif // OHOS_BUILD_HDF
    };
    for (auto& it : funs) {
        if (!RegistrationEvent(it)) {
            MMI_HILOGW("Failed to register event errCode:%{public}d", EVENT_REG_FAIL);
            continue;
        }
    }
}

void ServerMsgHandler::OnMsgHandler(SessionPtr sess, NetPacket& pkt)
{
    CHKPV(sess);
    auto id = pkt.GetMsgId();
    TimeCostChk chk("ServerMsgHandler::OnMsgHandler", "overtime 300(us)", MAX_OVER_TIME, id);
    auto callback = GetMsgCallback(id);
    if (callback == nullptr) {
        MMI_HILOGE("Unknown msg id:%{public}d,errCode:%{public}d", id, UNKNOWN_MSG_ID);
        return;
    }
    auto ret = (*callback)(sess, pkt);
    if (ret < 0) {
        MMI_HILOGE("Msg handling failed. id:%{public}d,errCode:%{public}d", id, ret);
    }
}

#ifdef OHOS_BUILD_HDF
int32_t ServerMsgHandler::OnHdiInject(SessionPtr sess, NetPacket& pkt)
{
    MMI_HILOGI("hdfinject server access hditools info");
    CHKPR(sess, ERROR_NULL_POINTER);
    CHKPR(udsServer_, ERROR_NULL_POINTER);
    const int32_t processingCode = MMIHdiInject->ManageHdfInject(sess, pkt);
    NetPacket pkt(MmiMessageId::HDI_INJECT);
    pkt << processingCode;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write reply messaage failed");
        return RET_ERR;
    }
    if (!sess->SendMsg(pkt)) {
        MMI_HILOGE("OnHdiInject reply messaage error");
        return RET_ERR;
    }
    return RET_OK;
}
#endif // OHOS_BUILD_HDF

int32_t ServerMsgHandler::MarkEventProcessed(SessionPtr sess, int32_t eventId)
{
    CALL_LOG_ENTER;
    CHKPR(sess, ERROR_NULL_POINTER);
    sess->DelEvents(eventId);
    return RET_OK;
}

int32_t ServerMsgHandler::OnInjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto result = eventDispatch_.DispatchKeyEventPid(*udsServer_, keyEvent);
    if (result != RET_OK) {
        MMI_HILOGE("Key event dispatch failed. ret:%{public}d,errCode:%{public}d", result, KEY_EVENT_DISP_FAIL);
    }
    MMI_HILOGD("Inject keyCode:%{public}d, action:%{public}d", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
    return RET_OK;
}

int32_t ServerMsgHandler::OnInjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_LOG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    pointerEvent->UpdateId();
    int32_t action = pointerEvent->GetPointerAction();
    if ((action == PointerEvent::POINTER_ACTION_MOVE || action == PointerEvent::POINTER_ACTION_UP)
        && targetWindowId_ > 0) {
        pointerEvent->SetTargetWindowId(targetWindowId_);
    }
    if (eventDispatch_.HandlePointerEvent(pointerEvent) != RET_OK) {
        MMI_HILOGE("HandlePointerEvent failed");
        return RET_ERR;
    }
    if (action == PointerEvent::POINTER_ACTION_DOWN) {
        targetWindowId_ = pointerEvent->GetTargetWindowId();
    }
    return RET_OK;
}

int32_t ServerMsgHandler::OnDisplayInfo(SessionPtr sess, NetPacket &pkt)
{
    CALL_LOG_ENTER;
    CHKPR(sess, ERROR_NULL_POINTER);
    DisplayGroupInfo displayGroupInfo;
    pkt >> displayGroupInfo.width >> displayGroupInfo.height >> displayGroupInfo.focusWindowId;
    int32_t num = 0;
    pkt >> num;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read display info failed");
        return RET_ERR;
    }
    for (int32_t i = 0; i < num; i++) {
        WindowInfo info;
        pkt >> info.id >> info.pid >> info.uid >> info.area >> info.defaultHotAreas
            >> info.pointerHotAreas >> info.agentWindowId >> info.flags;
        displayGroupInfo.windowsInfo.push_back(info);
        if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read display info failed");
        return RET_ERR;
    }
    }
    pkt >> num;
    for (int32_t i = 0; i < num; i++) {
        DisplayInfo info;
        pkt >> info.id >> info.x >> info.y >> info.width >> info.height
            >> info.name >> info.uniq >> info.direction;
        displayGroupInfo.displaysInfo.push_back(info);
        if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read display info failed");
        return RET_ERR;
    }
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read display info failed");
        return RET_ERR;
    }
    InputWindowsManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
    return RET_OK;
}

int32_t ServerMsgHandler::OnAddInputHandler(SessionPtr sess, int32_t handlerId, InputHandlerType handlerType,
    HandleEventType eventType)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    MMI_HILOGD("OnAddInputHandler handler:%{public}d,handlerType:%{public}d", handlerId, handlerType);
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        return InterHdlGl->AddInputHandler(handlerId, handlerType, eventType, sess);
    }
    if (handlerType == InputHandlerType::MONITOR) {
        return InputHandlerManagerGlobal::GetInstance().AddInputHandler(handlerId, handlerType, sess);
    }
    return RET_OK;
}

int32_t ServerMsgHandler::OnRemoveInputHandler(SessionPtr sess, int32_t handlerId, InputHandlerType handlerType)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    MMI_HILOGD("OnRemoveInputHandler handler:%{public}d,handlerType:%{public}d", handlerId, handlerType);
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        InterHdlGl->RemoveInputHandler(handlerId, handlerType, sess);
    }
    if (handlerType == InputHandlerType::MONITOR) {
        InputHandlerManagerGlobal::GetInstance().RemoveInputHandler(handlerId, handlerType, sess);
    }
    return RET_OK;
}

int32_t ServerMsgHandler::OnMarkConsumed(SessionPtr sess, int32_t monitorId, int32_t eventId)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    InputHandlerManagerGlobal::GetInstance().MarkConsumed(monitorId, eventId, sess);
    return RET_OK;
}

#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
int32_t ServerMsgHandler::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    CALL_LOG_ENTER;
    if (MouseEventHdr->NormalizeMoveMouse(offsetX, offsetY)) {
        auto pointerEvent = MouseEventHdr->GetPointerEvent();
        eventDispatch_.HandlePointerEvent(pointerEvent);
        MMI_HILOGD("Mouse movement message processed successfully");
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING

int32_t ServerMsgHandler::OnSubscribeKeyEvent(IUdsServer *server, int32_t pid,
    int32_t subscribeId, const std::shared_ptr<KeyOption> option)
{
    CALL_LOG_ENTER;
    CHKPR(server, ERROR_NULL_POINTER);
    auto sess = server->GetSessionByPid(pid);
    CHKPR(sess, ERROR_NULL_POINTER);
    return KeyEventSubscriber_.SubscribeKeyEvent(sess, subscribeId, option);
}

int32_t ServerMsgHandler::OnUnsubscribeKeyEvent(IUdsServer *server, int32_t pid, int32_t subscribeId)
{
    CALL_LOG_ENTER;
    CHKPR(server, ERROR_NULL_POINTER);
    auto sess = server->GetSessionByPid(pid);
    CHKPR(sess, ERROR_NULL_POINTER);
    return KeyEventSubscriber_.UnSubscribeKeyEvent(sess, subscribeId);
}

int32_t ServerMsgHandler::OnAddInputEventMontior(SessionPtr sess, NetPacket& pkt)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    int32_t eventType = 0;
    pkt >> eventType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read eventType failed");
        return PACKET_READ_FAIL;
    }
    if (eventType != InputEvent::EVENT_TYPE_KEY) {
        MMI_HILOGE("Wrong event type, eventType:%{public}d", eventType);
        return RET_ERR;
    }
    InputMonitorServiceMgr.AddInputEventMontior(sess, eventType);
    return RET_OK;
}

int32_t ServerMsgHandler::OnAddInputEventTouchpadMontior(SessionPtr sess, NetPacket& pkt)
{
    CALL_LOG_ENTER;
    CHKPR(sess, ERROR_NULL_POINTER);
    int32_t eventType = 0;
    pkt >> eventType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read eventType failed");
        return PACKET_READ_FAIL;
    }
    if (eventType != InputEvent::EVENT_TYPE_POINTER) {
        MMI_HILOGE("Wrong event type, eventType:%{public}d", eventType);
        return RET_ERR;
    }
    InputMonitorServiceMgr.AddInputEventTouchpadMontior(eventType, sess);
    return RET_OK;
}

int32_t ServerMsgHandler::OnRemoveInputEventMontior(SessionPtr sess, NetPacket& pkt)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    int32_t eventType = 0;
    pkt >> eventType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read eventType failed");
        return PACKET_READ_FAIL;
    }
    if (eventType != InputEvent::EVENT_TYPE_KEY) {
        MMI_HILOGE("Wrong event type, eventType:%{public}d", eventType);
        return RET_ERR;
    }
    InputMonitorServiceMgr.RemoveInputEventMontior(sess, eventType);
    return RET_OK;
}

int32_t ServerMsgHandler::OnRemoveInputEventTouchpadMontior(SessionPtr sess, NetPacket& pkt)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    int32_t eventType = 0;
    pkt >> eventType;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read eventType failed");
        return PACKET_READ_FAIL;
    }
    if (eventType != InputEvent::EVENT_TYPE_POINTER) {
        MMI_HILOGE("Wrong event type, eventType:%{public}d", eventType);
        return RET_ERR;
    }
    InputMonitorServiceMgr.RemoveInputEventMontior(sess, eventType);
    return RET_OK;
}

#ifdef OHOS_BUILD_MMI_DEBUG
int32_t ServerMsgHandler::OnBigPacketTest(SessionPtr sess, NetPacket& pkt)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    int32_t pid = 0;
    int32_t id = 0;
    pkt >> pid >> id;
    int32_t phyNum = 0;
    pkt >> phyNum;
    MMI_HILOGD("BigPacketsTest pid:%{public}d id:%{public}d phyNum:%{public}d size:%{public}zu",
        pid, id, phyNum, pkt.Size());
    for (auto i = 0; i < phyNum; i++) {
        PhysicalDisplayInfo info = {};
        pkt >> info.id >> info.leftDisplayId >> info.upDisplayId >> info.topLeftX >> info.topLeftY;
        pkt >> info.width >> info.height >> info.name >> info.seatId >> info.seatName >> info.logicWidth;
        pkt >> info.logicHeight >> info.direction;
        MMI_HILOGD("\tPhysical: idx:%{public}d id:%{public}d seatId:%{public}s", i, info.id, info.seatId.c_str());
    }
    int32_t logcNum = 0;
    pkt >> logcNum;
    MMI_HILOGD("\tlogcNum:%{public}d", logcNum);
    for (auto i = 0; i < logcNum; i++) {
        LogicalDisplayInfo info = {};
        pkt >> info.id >> info.topLeftX >> info.topLeftY;
        pkt >> info.width >> info.height >> info.name >> info.seatId >> info.seatName >> info.focusWindowId;
        MMI_HILOGD("\t\tLogical: idx:%{public}d id:%{public}d seatId:%{public}s", i, info.id, info.seatId.c_str());
        int32_t winNum = 0;
        pkt >> winNum;
        MMI_HILOGD("\t\twinNum:%{public}d", winNum);
        for (auto j = 0; j < winNum; j++) {
            WindowInfo winInfo;
            pkt >> winInfo;
            MMI_HILOGD("\t\t\tWindows: idx:%{public}d id:%{public}d displayId:%{public}d",
                j, winInfo.id, winInfo.displayId);
        }
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read data failed");
        return PACKET_READ_FAIL;
    }
    return RET_OK;
}
#endif // OHOS_BUILD_MMI_DEBUG
} // namespace MMI
} // namespace OHOS