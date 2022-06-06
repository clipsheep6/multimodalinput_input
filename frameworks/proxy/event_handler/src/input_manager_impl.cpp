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

#include "input_manager_impl.h"

#include <cinttypes>

#include "define_multimodal.h"
#include "error_multimodal.h"

#include "bytrace_adapter.h"
#include "define_interceptor_manager.h"
#include "event_filter_service.h"
#include "input_event_monitor_manager.h"
#include "mmi_client.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerImpl" };
constexpr int32_t MAX_HOTAREA_NUM = 10;
} // namespace

struct MonitorEventConsumer : public IInputEventConsumer {
    explicit MonitorEventConsumer(const std::function<void(std::shared_ptr<PointerEvent>)>& monitor)
        : monitor_ (monitor)
    {
    }

    explicit MonitorEventConsumer(const std::function<void(std::shared_ptr<KeyEvent>)>& monitor)
        : keyMonitor_ (monitor)
    {
    }

    void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const
    {
        CHKPV(keyEvent);
        CHKPV(keyMonitor_);
        keyMonitor_(keyEvent);
    }

    void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const
    {
        CHKPV(pointerEvent);
        CHKPV(monitor_);
        monitor_(pointerEvent);
    }

    void OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const
    {
        CHKPV(axisEvent);
        CHKPV(axisMonitor_);
        axisMonitor_(axisEvent);
    }

private:
    std::function<void(std::shared_ptr<PointerEvent>)> monitor_;
    std::function<void(std::shared_ptr<KeyEvent>)> keyMonitor_;
    std::function<void(std::shared_ptr<AxisEvent>)> axisMonitor_;
};

bool InputManagerImpl::InitEventHandler()
{
    CALL_LOG_ENTER;
    if (mmiEventHandler_ != nullptr) {
        MMI_HILOGE("Repeated initialization operations");
        return false;
    }

    std::mutex mtx;
    static constexpr int32_t timeout = 3;
    std::unique_lock <std::mutex> lck(mtx);
    ehThread_ = std::thread(std::bind(&InputManagerImpl::OnThread, this));
    ehThread_.detach();
    if (cv_.wait_for(lck, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
        MMI_HILOGE("EventThandler thread start timeout");
        return false;
    }
    return true;
}

MMIEventHandlerPtr InputManagerImpl::GetEventHandler() const
{
    CHKPP(mmiEventHandler_);
    return mmiEventHandler_->GetSharedPtr();
}

EventHandlerPtr InputManagerImpl::GetCurrentEventHandler() const
{
    auto eventHandler = AppExecFwk::EventHandler::Current();
    if (eventHandler == nullptr) {
        eventHandler = GetEventHandler();
    }
    return eventHandler;
}

void InputManagerImpl::OnThread()
{
    CALL_LOG_ENTER;
    CHK_PIDANDTID();
    SetThreadName("mmi_client_EventHdr");
    mmiEventHandler_ = std::make_shared<MMIEventHandler>();
    CHKPV(mmiEventHandler_);
    auto eventRunner = mmiEventHandler_->GetEventRunner();
    CHKPV(eventRunner);
    cv_.notify_one();
    eventRunner->Run();
}

void InputManagerImpl::UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
    const std::vector<LogicalDisplayInfo> &logicalDisplays)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("get mmi client is nullptr");
        return;
    }
    if (physicalDisplays.empty() || logicalDisplays.empty()) {
        MMI_HILOGE("display info check failed! physicalDisplays size:%{public}zu,logicalDisplays size:%{public}zu",
            physicalDisplays.size(), logicalDisplays.size());
        return;
    }
    physicalDisplays_ = physicalDisplays;
    logicalDisplays_ = logicalDisplays;
    SendDisplayInfo();
    PrintDisplayInfo();
}

int32_t InputManagerImpl::AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    bool hasSendToMmiServer = true;
    if (eventFilterService_ == nullptr) {
        hasSendToMmiServer = false;
        eventFilterService_ = new (std::nothrow) EventFilterService();
        CHKPR(eventFilterService_, RET_ERR);
    }

    eventFilterService_->SetPointerEventPtr(filter);
    if (!hasSendToMmiServer) {
        int32_t ret = MultimodalInputConnectManager::GetInstance()->AddInputEventFilter(eventFilterService_);
        if (ret != RET_OK) {
            MMI_HILOGE("AddInputEventFilter has send to server fail, ret:%{public}d", ret);
            delete eventFilterService_;
            eventFilterService_ = nullptr;
            return RET_ERR;
        }
        MMI_HILOGI("AddInputEventFilter has send to server success");
        return RET_OK;
    }
    return RET_OK;
}

void InputManagerImpl::SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CALL_LOG_ENTER;
    CHKPV(inputEventConsumer);
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    consumer_ = inputEventConsumer;
    eventHandler_ = eventHandler;
    if (eventHandler_ == nullptr) {
        eventHandler_ = InputMgrImpl->GetCurrentEventHandler();
    }
}

void InputManagerImpl::OnKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PIDANDTID();
    CHKPV(consumer);
    consumer->OnInputEvent(keyEvent);
    MMI_HILOGD("key event callback keyCode:%{public}d", keyEvent->GetKeyCode());
}

void InputManagerImpl::OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHK_PIDANDTID();
    CHKPV(keyEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    std::lock_guard<std::mutex> guard(mtx_);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_DISPATCH_EVENT);
    if (!MMIEventHandler::PostTask(eventHandler_,
        std::bind(&InputManagerImpl::OnKeyEventTask, this, consumer_, keyEvent))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("key event keyCode:%{public}d", keyEvent->GetKeyCode());
}

void InputManagerImpl::OnPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
    std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PIDANDTID();
    CHKPV(consumer);
    CHKPV(pointerEvent);
    consumer->OnInputEvent(pointerEvent);
    MMI_HILOGD("pointer event callback pointerId:%{public}d", pointerEvent->GetPointerId());
}

void InputManagerImpl::OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHK_PIDANDTID();
    CHKPV(pointerEvent);
    CHKPV(eventHandler_);
    CHKPV(consumer_);
    std::lock_guard<std::mutex> guard(mtx_);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP, BytraceAdapter::POINT_DISPATCH_EVENT);
    if (!MMIEventHandler::PostTask(eventHandler_,
        std::bind(&InputManagerImpl::OnPointerEventTask, this, consumer_, pointerEvent))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("pointer event pointerId:%{public}d", pointerEvent->GetPointerId());
}

int32_t InputManagerImpl::PackDisplayData(NetPacket &pkt)
{
    if (PackPhysicalDisplay(pkt) == RET_ERR) {
        MMI_HILOGE("pack physical display failed");
        return RET_ERR;
    }
    return PackLogicalDisplay(pkt);
}

int32_t InputManagerImpl::PackPhysicalDisplay(NetPacket &pkt)
{
    uint32_t num = static_cast<uint32_t>(physicalDisplays_.size());
    if (num > MAX_PHYSICAL_SIZE) {
        MMI_HILOGE("Physical exceeds the max range");
        return RET_ERR;
    }
    pkt << num;
    for (uint32_t i = 0; i < num; i++) {
        pkt << physicalDisplays_[i].id << physicalDisplays_[i].leftDisplayId << physicalDisplays_[i].upDisplayId
            << physicalDisplays_[i].topLeftX << physicalDisplays_[i].topLeftY << physicalDisplays_[i].width
            << physicalDisplays_[i].height << physicalDisplays_[i].name << physicalDisplays_[i].seatId
            << physicalDisplays_[i].seatName << physicalDisplays_[i].logicWidth
            << physicalDisplays_[i].logicHeight << physicalDisplays_[i].direction;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write physical data failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputManagerImpl::PackLogicalDisplay(NetPacket &pkt)
{
    int32_t num = static_cast<int32_t>(logicalDisplays_.size());
    if (num > MAX_LOGICAL_SIZE) {
        MMI_HILOGE("Logical exceeds the max range");
        return RET_ERR;
    }
    pkt << num;
    for (int32_t i = 0; i < num; i++) {
        int32_t windowsInfoSize = static_cast<int32_t>(logicalDisplays_[i].windowsInfo.size());
        pkt << logicalDisplays_[i].id << logicalDisplays_[i].topLeftX << logicalDisplays_[i].topLeftY
            << logicalDisplays_[i].width << logicalDisplays_[i].height << logicalDisplays_[i].name
            << logicalDisplays_[i].seatId << logicalDisplays_[i].seatName << logicalDisplays_[i].focusWindowId
            << windowsInfoSize;
        for (int32_t j = 0; j < windowsInfoSize; ++j) {
            pkt << logicalDisplays_[i].windowsInfo[j].id
                << logicalDisplays_[i].windowsInfo[j].pid
                << logicalDisplays_[i].windowsInfo[j].uid
                << logicalDisplays_[i].windowsInfo[j].hotZoneTopLeftX
                << logicalDisplays_[i].windowsInfo[j].hotZoneTopLeftY
                << logicalDisplays_[i].windowsInfo[j].hotZoneWidth
                << logicalDisplays_[i].windowsInfo[j].hotZoneHeight
                << logicalDisplays_[i].windowsInfo[j].displayId
                << logicalDisplays_[i].windowsInfo[j].agentWindowId
                << logicalDisplays_[i].windowsInfo[j].winTopLeftX
                << logicalDisplays_[i].windowsInfo[j].winTopLeftY
                << logicalDisplays_[i].windowsInfo[j].flags
                << logicalDisplays_[i].windowsInfo[j].hotArea;
        }
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write logical data failed");
        return RET_ERR;
    }
    return RET_OK;
}

void InputManagerImpl::PrintDisplayInfo()
{
    MMI_HILOGD("physicalDisplays,num:%{public}zu", physicalDisplays_.size());
    for (const auto &item : physicalDisplays_) {
        MMI_HILOGD("physicalDisplays,id:%{public}d,leftDisplay:%{public}d,upDisplay:%{public}d,"
            "topLeftX:%{public}d,topLeftY:%{public}d,width:%{public}d,height:%{public}d,"
            "name:%{public}s,seatId:%{public}s,seatName:%{public}s,logicWidth:%{public}d,"
            "logicHeight:%{public}d,direction:%{public}d",
            item.id, item.leftDisplayId, item.upDisplayId,
            item.topLeftX, item.topLeftY, item.width,
            item.height, item.name.c_str(), item.seatId.c_str(),
            item.seatName.c_str(), item.logicWidth, item.logicHeight,
            item.direction);
    }

    MMI_HILOGD("logicalDisplays,num:%{public}zu", logicalDisplays_.size());
    for (const auto &item : logicalDisplays_) {
        MMI_HILOGD("logicalDisplays, id:%{public}d,topLeftX:%{public}d,topLeftY:%{public}d,"
            "width:%{public}d,height:%{public}d,name:%{public}s,"
            "seatId:%{public}s,seatName:%{public}s,focusWindowId:%{public}d,window num:%{public}zu",
            item.id, item.topLeftX, item.topLeftY,
            item.width, item.height, item.name.c_str(),
            item.seatId.c_str(), item.seatName.c_str(),
            item.focusWindowId, item.windowsInfo.size());
        for (const auto &win : item.windowsInfo) {
            MMI_HILOGD("windowid:%{public}d,pid:%{public}d,uid:%{public}d,hotZoneTopLeftX:%{public}d,"
                "hotZoneTopLeftY:%{public}d,hotZoneWidth:%{public}d,hotZoneHeight:%{public}d,display:%{public}d,"
                "agentWindowId:%{public}d,winTopLeftX:%{public}d,winTopLeftY:%{public}d,flags:%{public}d",
                win.id, win.pid,
                win.uid, win.hotZoneTopLeftX,
                win.hotZoneTopLeftY, win.hotZoneWidth,
                win.hotZoneHeight, win.displayId,
                win.agentWindowId,
                win.winTopLeftX, win.winTopLeftY, win.flags);
        }
    }
}

int32_t InputManagerImpl::AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor)
{
    CHKPR(monitor, ERROR_NULL_POINTER);
    auto consumer = std::make_shared<MonitorEventConsumer>(monitor);
    CHKPR(consumer, ERROR_NULL_POINTER);
    return InputManagerImpl::AddMonitor(consumer);
}

int32_t InputManagerImpl::AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor)
{
    CHKPR(monitor, ERROR_NULL_POINTER);
    auto consumer = std::make_shared<MonitorEventConsumer>(monitor);
    CHKPR(consumer, ERROR_NULL_POINTER);
    return InputManagerImpl::AddMonitor(consumer);
}

int32_t InputManagerImpl::AddMonitor(std::shared_ptr<IInputEventConsumer> consumer)
{
    CHKPR(consumer, ERROR_NULL_POINTER);
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return -1;
    }
    int32_t monitorId = monitorManager_.AddMonitor(consumer);
    return monitorId;
}

void InputManagerImpl::RemoveMonitor(int32_t monitorId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    monitorManager_.RemoveMonitor(monitorId);
}

void InputManagerImpl::MarkConsumed(int32_t monitorId, int32_t eventId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    monitorManager_.MarkConsumed(monitorId, eventId);
}

void InputManagerImpl::MoveMouse(int32_t offsetX, int32_t offsetY)
{
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    if (MMIEventHdl.MoveMouseEvent(offsetX, offsetY) != RET_OK) {
        MMI_HILOGE("Failed to inject move mouse offset event");
    }
#else
    MMI_HILOGW("Pointer drawing module does not support");
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
}

int32_t InputManagerImpl::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor)
{
    CHKPR(interceptor, INVALID_HANDLER_ID);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return -1;
    }
    std::lock_guard<std::mutex> guard(mtx_);
    return InputInterMgr->AddInterceptor(interceptor);
}

int32_t InputManagerImpl::AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor)
{
    CHKPR(interceptor, ERROR_NULL_POINTER);
    auto consumer = std::make_shared<MonitorEventConsumer>(interceptor);
    CHKPR(consumer, ERROR_NULL_POINTER);
    return InputManagerImpl::AddInterceptor(consumer);
}

void InputManagerImpl::RemoveInterceptor(int32_t interceptorId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    InputInterMgr->RemoveInterceptor(interceptorId);
}

void InputManagerImpl::SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    if (MMIEventHdl.InjectEvent(keyEvent) != RET_OK) {
        MMI_HILOGE("Failed to inject keyEvent");
    }
}

void InputManagerImpl::SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("client init failed");
        return;
    }
    if (MMIEventHdl.InjectPointerEvent(pointerEvent) != RET_OK) {
        MMI_HILOGE("Failed to inject pointer event");
    }
}

int32_t InputManagerImpl::SetPointerVisible(bool visible)
{
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    CALL_LOG_ENTER;
    int32_t ret = MultimodalInputConnectManager::GetInstance()->SetPointerVisible(visible);
    if (ret != RET_OK) {
        MMI_HILOGE("send to server fail, ret:%{public}d", ret);
    }
    return ret;
#else
    MMI_HILOGW("Pointer drawing module does not support");
    return ERROR_UNSUPPORT;
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
}

bool InputManagerImpl::IsPointerVisible()
{
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    CALL_LOG_ENTER;
    bool visible;
    int32_t ret = MultimodalInputConnectManager::GetInstance()->IsPointerVisible(visible);
    if (ret != 0) {
        MMI_HILOGE("send to server fail, ret:%{public}d", ret);
    }
    return visible;
#else
    MMI_HILOGW("Pointer drawing module dose not support");
    return false;
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
}

void InputManagerImpl::OnConnected()
{
    CALL_LOG_ENTER;
    if (physicalDisplays_.empty() || logicalDisplays_.empty()) {
        MMI_HILOGE("display info check failed! physicalDisplays_ size:%{public}zu,logicalDisplays_ size:%{public}zu",
            physicalDisplays_.size(), logicalDisplays_.size());
        return;
    }
    SendDisplayInfo();
    PrintDisplayInfo();
}

void InputManagerImpl::SendDisplayInfo()
{
    MMIClientPtr client = MMIEventHdl.GetMMIClient();
    CHKPV(client);

    for (const auto &item : logicalDisplays_) {
        for (const auto &win : item.windowsInfo) {
            if (win.hotArea.size() >= MAX_HOTAREA_NUM) {
                MMI_HILOGE("There are too many hotArea, hotArea size:%{public}zu", win.hotArea.size());
                return;
            }
        }
    }

    NetPacket pkt(MmiMessageId::DISPLAY_INFO);
    if (PackDisplayData(pkt) == RET_ERR) {
        MMI_HILOGE("pack display info failed");
        return;
    }
    if (!client->SendMessage(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
    }
}

void InputManagerImpl::SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes,
    std::function<void(std::vector<bool>&)> callback)
{
    InputDevImpl.SupportKeys(deviceId, keyCodes, callback);
}

void InputManagerImpl::GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("Client init failed");
        return;
    }
    InputDevImpl.GetKeyboardType(deviceId, callback);
}
} // namespace MMI
} // namespace OHOS
