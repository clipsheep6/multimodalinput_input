/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "input_window_transfer.h"

#include "bytrace_adapter.h"
#include "event_log_helper.h"
#include "input_event_data_transformation.h"
#include "input_connect_manager.h"
#include "input_manager_impl.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputWindowTransfer" };
} // namespace

int32_t InputWindowTransfer::GetDisplayBindInfo(DisplayBindInfos &infos)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->GetDisplayBindInfo(infos);
    if (ret != RET_OK) {
        MMI_HILOGE("GetDisplayBindInfo failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputWindowTransfer::SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg)
{
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->SetDisplayBind(deviceId, displayId, msg);
    if (ret != RET_OK) {
        MMI_HILOGE("SetDisplayBind failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

void InputWindowTransfer::UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (displayGroupInfo.windowsInfo.empty() || displayGroupInfo.displaysInfo.empty()) {
        MMI_HILOGE("The windows info or display info is empty!");
        return;
    }
    for (const auto &item : displayGroupInfo.windowsInfo) {
        if ((item.defaultHotAreas.size() > WindowInfo::MAX_HOTAREA_COUNT) ||
            (item.pointerHotAreas.size() > WindowInfo::MAX_HOTAREA_COUNT) ||
            item.defaultHotAreas.empty() || item.pointerHotAreas.empty()) {
            MMI_HILOGE("Hot areas check failed! defaultHotAreas:size:%{public}zu,"
                       "pointerHotAreas:size:%{public}zu",
                       item.defaultHotAreas.size(), item.pointerHotAreas.size());
            return;
        }
    }
    displayGroupInfo_ = displayGroupInfo;
    SendDisplayInfo();
    PrintDisplayInfo();
}

void InputWindowTransfer::SendDisplayInfo()
{
    auto client = InputMgrImpl.GetMMIClient();
    CHKPV(client);
    NetPacket pkt(MmiMessageId::DISPLAY_INFO);
    if (PackDisplayData(pkt) == RET_ERR) {
        MMI_HILOGE("Pack display info failed");
        return;
    }
    if (!client->SendMessage(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
    }
}

int32_t InputWindowTransfer::PackDisplayData(NetPacket &pkt)
{
    pkt << displayGroupInfo_.width << displayGroupInfo_.height << displayGroupInfo_.focusWindowId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write logical data failed");
        return RET_ERR;
    }
    if (PackWindowInfo(pkt) == RET_ERR) {
        MMI_HILOGE("Packet write windows info failed");
        return RET_ERR;
    }
    return PackDisplayInfo(pkt);
}

int32_t InputWindowTransfer::PackWindowInfo(NetPacket &pkt)
{
    uint32_t num = static_cast<uint32_t>(displayGroupInfo_.windowsInfo.size());
    pkt << num;
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        pkt << item.id << item.pid << item.uid << item.area
            << item.defaultHotAreas << item.pointerHotAreas
            << item.agentWindowId << item.flags;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write windows data failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputWindowTransfer::PackDisplayInfo(NetPacket &pkt)
{
    uint32_t num = static_cast<uint32_t>(displayGroupInfo_.displaysInfo.size());
    pkt << num;
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        pkt << item.id << item.x << item.y << item.width
            << item.height << item.name << item.uniq << item.direction;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write display data failed");
        return RET_ERR;
    }
    return RET_OK;
}

void InputWindowTransfer::PrintDisplayInfo()
{
    MMI_HILOGI("logicalInfo,width:%{public}d,height:%{public}d,focusWindowId:%{public}d",
        displayGroupInfo_.width, displayGroupInfo_.height, displayGroupInfo_.focusWindowId);
    MMI_HILOGI("windowsInfos,num:%{public}zu", displayGroupInfo_.windowsInfo.size());
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        MMI_HILOGI("windowsInfos,id:%{public}d,pid:%{public}d,uid:%{public}d,"
            "area.x:%{public}d,area.y:%{public}d,area.width:%{public}d,area.height:%{public}d,"
            "defaultHotAreas.size:%{public}zu,pointerHotAreas.size:%{public}zu,"
            "agentWindowId:%{public}d,flags:%{public}d",
            item.id, item.pid, item.uid, item.area.x, item.area.y, item.area.width,
            item.area.height, item.defaultHotAreas.size(), item.pointerHotAreas.size(),
            item.agentWindowId, item.flags);
        for (const auto &win : item.defaultHotAreas) {
            MMI_HILOGI("defaultHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                win.x, win.y, win.width, win.height);
        }
        for (const auto &pointer : item.pointerHotAreas) {
            MMI_HILOGI("pointerHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                pointer.x, pointer.y, pointer.width, pointer.height);
        }
    }

    MMI_HILOGI("displayInfos,num:%{public}zu", displayGroupInfo_.displaysInfo.size());
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        MMI_HILOGI("displayInfos,id:%{public}d,x:%{public}d,y:%{public}d,"
            "width:%{public}d,height:%{public}d,name:%{public}s,"
            "uniq:%{public}s,direction:%{public}d",
            item.id, item.x, item.y, item.width, item.height, item.name.c_str(),
            item.uniq.c_str(), item.direction);
    }
}

void InputWindowTransfer::OnConnected()
{
    CALL_DEBUG_ENTER;
    InitProcessedCallback();
    if (displayGroupInfo_.windowsInfo.empty() || displayGroupInfo_.displaysInfo.empty()) {
        MMI_HILOGE("The windows info or display info is empty");
        return;
    }
    SendDisplayInfo();
    PrintDisplayInfo();
}

void InputWindowTransfer::SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    consumer_ = inputEventConsumer;
    eventHandler_ = eventHandler;
}

void InputWindowTransfer::InitProcessedCallback()
{
    CALL_DEBUG_ENTER;
    int32_t tokenType = MultimodalInputConnMgr->GetTokenType();
    if (tokenType == TokenType::TOKEN_HAP) {
        MMI_HILOGD("Current session is hap");
        dispatchCallback_ = std::bind(&InputWindowTransfer::OnDispatchEventProcessed, this, std::placeholders::_1,
            std::placeholders::_2);
    } else if (tokenType == static_cast<int32_t>(TokenType::TOKEN_NATIVE)) {
        MMI_HILOGD("Current session is native");
    } else {
        MMI_HILOGE("Current session is unknown tokenType:%{public}d", tokenType);
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputWindowTransfer::OnKeyEvent(NetPacket& pkt)
{
    auto key = KeyEvent::Create();
    CHKPR(key, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, key);
    if (ret != RET_OK) {
        MMI_HILOGE("Read netPacket failed");
        return RET_ERR;
    }
    int32_t fd = 0;
    pkt >> fd;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read fd failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGI("Key event dispatcher of client, Fd:%{public}d", fd);
    EventLogHelper::PrintEventData(key);
    BytraceAdapter::StartBytrace(key, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_DISPATCH_EVENT);
    key->SetProcessedCallback(dispatchCallback_);
    HandlerKeyEvent(key);
    key->MarkProcessed();
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t InputWindowTransfer::OnPointerEvent(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (InputEventDataTransformation::Unmarshalling(pkt, pointerEvent) != ERR_OK) {
        MMI_HILOGE("Failed to deserialize pointer event.");
        return RET_ERR;
    }
    MMI_HILOGD("Pointer event dispatcher of client:");
    EventLogHelper::PrintEventData(pointerEvent);
    if (PointerEvent::POINTER_ACTION_CANCEL == pointerEvent->GetPointerAction()) {
        MMI_HILOGI("Operation canceled.");
    }
    pointerEvent->SetProcessedCallback(dispatchCallback_);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START, BytraceAdapter::POINT_DISPATCH_EVENT);
    HandlerPointerEvent(pointerEvent);
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_JOYSTICK) {
        pointerEvent->MarkProcessed();
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

void InputWindowTransfer::OnDispatchEventProcessed(int32_t eventId, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    InputMgrImpl.GetAnrCollecter().SetLastProcessedEventId(ANR_DISPATCH, eventId, actionTime);
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputWindowTransfer::HandlerKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(consumer);
    consumer->OnInputEvent(keyEvent);
    MMI_HILOGD("Key event callback keyCode:%{public}d", keyEvent->GetKeyCode());
}

void InputWindowTransfer::HandlerKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PID_AND_TID();
    CHKPV(keyEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    auto client = InputMgrImpl.GetMMIClient();
    CHKPV(client);
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler = nullptr;
    std::shared_ptr<IInputEventConsumer> inputConsumer = nullptr;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        eventHandler = eventHandler_;
        inputConsumer = consumer_;
    }
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_DISPATCH_EVENT);
    if (client->IsEventHandlerChanged()) {
        if (!eventHandler->PostHighPriorityTask(std::bind(&InputWindowTransfer::HandlerKeyEventTask,
            this, inputConsumer, keyEvent))) {
            MMI_HILOGE("Post task failed");
            return;
        }
    } else {
        inputConsumer->OnInputEvent(keyEvent);
        MMI_HILOGD("Key event report keyCode:%{public}d", keyEvent->GetKeyCode());
    }
    MMI_HILOGD("Key event keyCode:%{public}d", keyEvent->GetKeyCode());
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputWindowTransfer::HandlerPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PID_AND_TID();
    CHKPV(consumer);
    CHKPV(pointerEvent);
    consumer->OnInputEvent(pointerEvent);
    MMI_HILOGD("Pointer event callback pointerId:%{public}d", pointerEvent->GetPointerId());
}

void InputWindowTransfer::HandlerPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    CHKPV(pointerEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    auto client = InputMgrImpl.GetMMIClient();
    CHKPV(client);
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler = nullptr;
    std::shared_ptr<IInputEventConsumer> inputConsumer = nullptr;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        eventHandler = eventHandler_;
        inputConsumer = consumer_;
    }
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_DISPATCH_EVENT);
    if (client->IsEventHandlerChanged()) {
        if (!eventHandler->PostHighPriorityTask(std::bind(&InputWindowTransfer::HandlerPointerEventTask,
            this, inputConsumer, pointerEvent))) {
            MMI_HILOGE("Post task failed");
            return;
        }
    } else {
        inputConsumer->OnInputEvent(pointerEvent);
        MMI_HILOGD("Pointer event report pointerId:%{public}d", pointerEvent->GetPointerId());
    }
    MMI_HILOGD("Pointer event pointerId:%{public}d", pointerEvent->GetPointerId());
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS
