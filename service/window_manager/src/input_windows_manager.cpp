/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "input_windows_manager.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <linux/input.h>

#include "dfx_hisysevent.h"
#include "input_device_manager.h"
#include "i_pointer_drawing_manager.h"
#include "mouse_event_normalize.h"
#include "pointer_drawing_manager.h"
#include "preferences.h"
#include "preferences_impl.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "preferences_xml_utils.h"
#include "util.h"
#include "mmi_matrix3.h"
#include "util_ex.h"
#include "util_napi_error.h"
#include "input_device_manager.h"
#include "scene_board_judgement.h"
#include "multimodal_input_preferences_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputWindowsManager" };
#ifdef OHOS_BUILD_ENABLE_POINTER
constexpr int32_t DEFAULT_POINTER_STYLE = 0;
#endif // OHOS_BUILD_ENABLE_POINTER
const std::string bindCfgFileName = "/data/service/el1/public/multimodalinput/display_bind.cfg";
const std::string mouseFileName = "mouse_settings.xml";
const std::string defaultIconPath = "/system/etc/multimodalinput/mouse_icon/Default.svg";
} // namespace

InputWindowsManager::InputWindowsManager() : bindInfo_(bindCfgFileName)
{
    MMI_HILOGI("Bind cfg file name:%{public}s", bindCfgFileName.c_str());
}

InputWindowsManager::~InputWindowsManager() {}

void InputWindowsManager::DeviceStatusChanged(int32_t deviceId, const std::string &sysUid, const std::string devStatus)
{
    CALL_INFO_TRACE;
    if (devStatus == "add") {
        bindInfo_.AddInputDevice(deviceId, sysUid);
    } else {
        bindInfo_.RemoveInputDevice(deviceId);
    }
}

void InputWindowsManager::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
    CHKPV(udsServer_);
    bindInfo_.Load();
#ifdef OHOS_BUILD_ENABLE_POINTER
    udsServer_->AddSessionDeletedCallback(std::bind(&InputWindowsManager::OnSessionLost, this, std::placeholders::_1));
    InitMouseDownInfo();
#endif // OHOS_BUILD_ENABLE_POINTER
    InputDevMgr->SetInputStatusChangeCallback(std::bind(&InputWindowsManager::DeviceStatusChanged, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

#ifdef OHOS_BUILD_ENABLE_POINTER
void InputWindowsManager::InitMouseDownInfo()
{
    mouseDownInfo_.id = -1;
    mouseDownInfo_.pid = -1;
    mouseDownInfo_.defaultHotAreas.clear();
    mouseDownInfo_.pointerHotAreas.clear();
}
#endif // OHOS_BUILD_ENABLE_POINTER

const std::vector<WindowInfo> &InputWindowsManager::GetWindowGroupInfoByDisplayId(int32_t displayId) const
{
    CALL_DEBUG_ENTER;
    auto iter = windowsPerDisplay_.find(displayId);
    if (displayId == -1 || iter == windowsPerDisplay_.end()) {
        MMI_HILOGW("GetWindowInfo displayId: %{public}d is null from windowGroupInfo_", displayId);
        return displayGroupInfo_.windowsInfo;
    }
    if (iter->second.windowsInfo.empty()) {
        MMI_HILOGW("GetWindowInfo displayId: %{public}d is empty", displayId);
        return displayGroupInfo_.windowsInfo;
    }
    return iter->second.windowsInfo;
}

int32_t InputWindowsManager::GetClientFd(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, INVALID_FD);
    const WindowInfo* windowInfo = nullptr;
    std::vector<WindowInfo> windowsInfo = GetWindowGroupInfoByDisplayId(pointerEvent->GetTargetDisplayId());
    for (const auto &item : windowsInfo) {
        if (item.id == pointerEvent->GetTargetWindowId()) {
            MMI_HILOGD("find windowinfo by window id %{public}d", item.id);
            windowInfo = &item;
            break;
        }
    }
    
    CHKPR(udsServer_, INVALID_FD);
    if (windowInfo != nullptr) {
        MMI_HILOGD("get pid:%{public}d from idxPidMap", windowInfo->pid);
        return udsServer_->GetClientFd(windowInfo->pid);
    }
    if (pointerEvent->GetPointerAction() != PointerEvent::POINTER_ACTION_CANCEL) {
        MMI_HILOGD("window info is null, so pointerEvent is dropped! return -1");
        return udsServer_->GetClientFd(-1);
    }
    int32_t pid = -1;
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        auto iter = touchItemDownInfos_.find(pointerEvent->GetPointerId());
        if (iter != touchItemDownInfos_.end()) {
            pid = GetWindowPid(iter->second.agentWindowId);
            MMI_HILOGD("touchscreen occurs, new pid:%{public}d", pid);
            touchItemDownInfos_.erase(iter);
        }
    }
#ifdef OHOS_BUILD_ENABLE_POINTER
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
        if (mouseDownInfo_.pid != -1) {
            pid = GetWindowPid(mouseDownInfo_.agentWindowId);
            MMI_HILOGD("mouseevent occurs, update the pid:%{public}d", pid);
            InitMouseDownInfo();
        }
    }
#endif // OHOS_BUILD_ENABLE_POINTER
    MMI_HILOGD("get clientFd by %{public}d", pid);
    return udsServer_->GetClientFd(pid);
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputWindowsManager::UpdateTarget(std::shared_ptr<InputEvent> inputEvent)
{
    CHKPR(inputEvent, INVALID_FD);
    CALL_DEBUG_ENTER;
    int32_t pid = GetPidAndUpdateTarget(inputEvent);
    if (pid <= 0) {
        MMI_HILOGE("Invalid pid");
        return INVALID_FD;
    }
    int32_t fd = udsServer_->GetClientFd(pid);
    if (fd < 0) {
        MMI_HILOGE("Invalid fd");
        return INVALID_FD;
    }
    return fd;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

int32_t InputWindowsManager::GetDisplayId(std::shared_ptr<InputEvent> inputEvent) const
{
    int32_t displayId = inputEvent->GetTargetDisplayId();
    if (displayId < 0) {
        MMI_HILOGD("Target display is -1");
        if (displayGroupInfo_.displaysInfo.empty()) {
            return displayId;
        }
        displayId = displayGroupInfo_.displaysInfo[0].id;
        inputEvent->SetTargetDisplayId(displayId);
    }
    return displayId;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputWindowsManager::GetPidAndUpdateTarget(std::shared_ptr<InputEvent> inputEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(inputEvent, INVALID_PID);
    const int32_t focusWindowId = displayGroupInfo_.focusWindowId;
    WindowInfo* windowInfo = nullptr;
    std::vector<WindowInfo> windowsInfo = GetWindowGroupInfoByDisplayId(inputEvent->GetTargetDisplayId());
    for (auto &item : windowsInfo) {
        if (item.id == focusWindowId) {
            windowInfo = &item;
            break;
        }
    }
    CHKPR(windowInfo, INVALID_PID);
    inputEvent->SetTargetWindowId(windowInfo->id);
    inputEvent->SetAgentWindowId(windowInfo->agentWindowId);
    MMI_HILOGD("focusWindowId:%{public}d, pid:%{public}d", focusWindowId, windowInfo->pid);
    return windowInfo->pid;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

int32_t InputWindowsManager::GetWindowPid(int32_t windowId) const
{
    CALL_DEBUG_ENTER;
    int32_t windowPid = -1;
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        MMI_HILOGD("get windowID %{public}d", item.id);
        if (item.id == windowId) {
            windowPid = item.pid;
            break;
        }
    }
    return windowPid;
}

int32_t InputWindowsManager::GetWindowPid(int32_t windowId, const std::vector<WindowInfo> &windowsInfo) const
{
    int32_t windowPid = -1;
    for (const auto &item : windowsInfo) {
        if (item.id == windowId) {
            windowPid = item.pid;
            break;
        }
    }
    return windowPid;
}

void InputWindowsManager::CheckFocusWindowChange(const DisplayGroupInfo &displayGroupInfo)
{
    const int32_t oldFocusWindowId = displayGroupInfo_.focusWindowId;
    const int32_t newFocusWindowId = displayGroupInfo.focusWindowId;
    if (oldFocusWindowId == newFocusWindowId) {
        return;
    }
    const int32_t oldFocusWindowPid = GetWindowPid(oldFocusWindowId);
    const int32_t newFocusWindowPid = GetWindowPid(newFocusWindowId, displayGroupInfo.windowsInfo);
    DfxHisysevent::OnFocusWindowChanged(oldFocusWindowId, newFocusWindowId, oldFocusWindowPid, newFocusWindowPid);
}

void InputWindowsManager::CheckZorderWindowChange(const std::vector<WindowInfo> &oldWindowsInfo,
    const std::vector<WindowInfo> &newWindowsInfo)
{
    int32_t oldZorderFirstWindowId = -1;
    int32_t newZorderFirstWindowId = -1;
    if (!oldWindowsInfo.empty()) {
        oldZorderFirstWindowId = oldWindowsInfo[0].id;
    }
    if (!newWindowsInfo.empty()) {
        newZorderFirstWindowId = newWindowsInfo[0].id;
    }
    if (oldZorderFirstWindowId == newZorderFirstWindowId) {
        return;
    }
    const int32_t oldZorderFirstWindowPid = GetWindowPid(oldZorderFirstWindowId);
    const int32_t newZorderFirstWindowPid = GetWindowPid(newZorderFirstWindowId, newWindowsInfo);
    DfxHisysevent::OnZorderWindowChanged(oldZorderFirstWindowId, newZorderFirstWindowId,
        oldZorderFirstWindowPid, newZorderFirstWindowPid);
}

void InputWindowsManager::UpdateDisplayIdAndName()
{
    using IdNames = std::set<std::pair<int32_t, std::string>>;
    IdNames newInfo;
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        newInfo.insert(std::make_pair(item.id, item.uniq));
    }
    auto oldInfo = bindInfo_.GetDisplayIdNames();
    if (newInfo == oldInfo) {
        return;
    }
    for (auto it = oldInfo.begin(); it != oldInfo.end();) {
        if (newInfo.find(*it) == newInfo.end()) {
            bindInfo_.RemoveDisplay(it->first);
            oldInfo.erase(it++);
        } else {
            ++it;
        }
    }
    for (const auto &item : newInfo) {
        if (!bindInfo_.IsDisplayAdd(item.first, item.second)) {
            bindInfo_.AddDisplay(item.first, item.second);
        }
    }
}

int32_t InputWindowsManager::GetDisplayBindInfo(DisplayBindInfos &infos)
{
    CALL_DEBUG_ENTER;
    return bindInfo_.GetDisplayBindInfo(infos);
}

int32_t InputWindowsManager::SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg)
{
    CALL_DEBUG_ENTER;
    return bindInfo_.SetDisplayBind(deviceId, displayId, msg);
}

void InputWindowsManager::UpdateCaptureMode(const DisplayGroupInfo &displayGroupInfo)
{
    if (captureModeInfo_.isCaptureMode &&
        ((displayGroupInfo_.focusWindowId != displayGroupInfo.focusWindowId) ||
        (displayGroupInfo_.windowsInfo[0].id != displayGroupInfo.windowsInfo[0].id))) {
        captureModeInfo_.isCaptureMode = false;
    }
}

void InputWindowsManager::UpdateWindowInfo(const WindowGroupInfo &windowGroupInfo)
{
    CALL_DEBUG_ENTER;
    PrintWindowGroupInfo(windowGroupInfo);
    DisplayGroupInfo displayGroupInfo = displayGroupInfo_;
    displayGroupInfo.focusWindowId = windowGroupInfo.focusWindowId;
    for (const auto item : windowGroupInfo.windowsInfo) {
        UpdateDisplayInfoByIncrementalInfo(item, displayGroupInfo);
    }
    UpdateDisplayInfo(displayGroupInfo);
}

void InputWindowsManager::UpdateDisplayInfoByIncrementalInfo(const WindowInfo item,
    DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    switch (item.action) {
        case WINDOW_UPDATE_ACTION::ADD: {
            displayGroupInfo.windowsInfo.emplace_back(item);
            break;
        }
        case WINDOW_UPDATE_ACTION::DEL: {
            auto oldWindow = displayGroupInfo.windowsInfo.begin();
            while (oldWindow != displayGroupInfo.windowsInfo.end()) {
                if (oldWindow->id == item.id) {
                    oldWindow = displayGroupInfo.windowsInfo.erase(oldWindow);
                } else {
                    oldWindow++;
                }
            }
            break;
        }
        case WINDOW_UPDATE_ACTION::CHANGE: {
            for (size_t idx = 0; idx < displayGroupInfo.windowsInfo.size(); idx++) {
                if (item.id == displayGroupInfo.windowsInfo[idx].id) {
                    displayGroupInfo.windowsInfo[idx] = item;
                }
            }
            break;
        }
        default: {
            MMI_HILOGI("WINDOW_UPDATE_ACTION is action:%{public}d", item.action);
            break;
        }
    }
}

void InputWindowsManager::UpdateWindowsInfoPerDisplay(const DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    std::map<int32_t, WindowGroupInfo> windowsPerDisplay;
    for (const auto &window : displayGroupInfo.windowsInfo) {
        auto it = windowsPerDisplay.find(window.displayId);
        if (it == windowsPerDisplay.end()) {
            windowsPerDisplay[window.displayId] = WindowGroupInfo {-1, window.displayId, {window}};
        } else {
            it->second.windowsInfo.emplace_back(window);
        }
        if (displayGroupInfo.focusWindowId == window.id) {
            windowsPerDisplay[window.displayId].focusWindowId = window.id;
        }
    }
    for (auto iter : windowsPerDisplay) {
        std::sort(iter.second.windowsInfo.begin(), iter.second.windowsInfo.end(),
            [](const WindowInfo &lwindow, const WindowInfo &rwindow) -> bool {
            return lwindow.zOrder > rwindow.zOrder;
        });
    }
    for (const auto &item : windowsPerDisplay) {
        int32_t displayId = item.first;
        if (windowsPerDisplay_.find(displayId) != windowsPerDisplay_.end()) {
            CheckZorderWindowChange(windowsPerDisplay_[displayId].windowsInfo, item.second.windowsInfo);
        }
    }
    
    windowsPerDisplay_.clear();
    windowsPerDisplay_ = windowsPerDisplay;
}


void InputWindowsManager::UpdateDisplayInfo(DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    std::sort(displayGroupInfo.windowsInfo.begin(), displayGroupInfo.windowsInfo.end(),
        [](const WindowInfo &lwindow, const WindowInfo &rwindow) -> bool {
        return lwindow.zOrder > rwindow.zOrder;
    });
    UpdateWindowsInfoPerDisplay(displayGroupInfo);
    CheckFocusWindowChange(displayGroupInfo);
    UpdateCaptureMode(displayGroupInfo);
    displayGroupInfo_ = displayGroupInfo;
    PrintDisplayInfo();
    UpdateDisplayIdAndName();
#ifdef OHOS_BUILD_ENABLE_POINTER
    InitPointerStyle();
#endif // OHOS_BUILD_ENABLE_POINTER
#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
    if (!displayGroupInfo.displaysInfo.empty()) {
        PointerDrawingManagerOnDisplayInfo(displayGroupInfo);
    }
    if (InputDevMgr->HasPointerDevice()) {
        NotifyPointerToWindow();
    }
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING
}

void InputWindowsManager::PointerDrawingManagerOnDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
{
    IPointerDrawingManager::GetInstance()->OnDisplayInfo(displayGroupInfo);
    if (InputDevMgr->HasPointerDevice()) {
        MouseLocation mouseLocation = GetMouseInfo();
        int32_t displayId = MouseEventHdr->GetDisplayId();
        if (displayId < 0) {
            displayId = displayGroupInfo_.displaysInfo[0].id;
        }
        auto displayInfo = GetPhysicalDisplay(displayId);
        CHKPV(displayInfo);
        int32_t logicX = mouseLocation.physicalX + displayInfo->x;
        int32_t logicY = mouseLocation.physicalY + displayInfo->y;
        std::optional<WindowInfo> windowInfo;
        CHKPV(lastPointerEvent_);
        if (lastPointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_MOVE &&
            lastPointerEvent_->GetPressedButtons().empty()) {
            windowInfo = GetWindowInfo(logicX, logicY);
        } else {
            windowInfo = SelectWindowInfo(logicX, logicY, lastPointerEvent_);
        }
        CHKFRV(windowInfo, "The windowInfo is nullptr");
        int32_t windowPid = GetWindowPid(windowInfo->id);
        WinInfo info = { .windowPid = windowPid, .windowId = windowInfo->id };
        IPointerDrawingManager::GetInstance()->OnWindowInfo(info);
        PointerStyle pointerStyle;
        int32_t ret = WinMgr->GetPointerStyle(info.windowPid, info.windowId, pointerStyle);
        MMI_HILOGD("get pointer style, pid: %{public}d, windowid: %{public}d, style: %{public}d",
            info.windowPid, info.windowId, pointerStyle.id);
        CHKNOKRV(ret, "Draw pointer style failed, pointerStyleInfo is nullptr");
        IPointerDrawingManager::GetInstance()->DrawPointerStyle(pointerStyle);
    }
}

void InputWindowsManager::GetPointerStyleByArea(WindowArea area, int32_t pid, int32_t winId, PointerStyle& pointerStyle)
{
    CALL_DEBUG_ENTER;
    switch (area) {
        case WindowArea::ENTER:
        case WindowArea::EXIT:
            MMI_HILOGD("SetPointerStyle for Enter or exit! No need to deal with it now");
            break;
        case WindowArea::FOCUS_ON_TOP_LEFT:
        case WindowArea::FOCUS_ON_BOTTOM_RIGHT:
            pointerStyle.id = MOUSE_ICON::NORTH_WEST_SOUTH_EAST;
            break;
        case WindowArea::FOCUS_ON_TOP_RIGHT:
        case WindowArea::FOCUS_ON_BOTTOM_LEFT:
            pointerStyle.id = MOUSE_ICON::NORTH_EAST_SOUTH_WEST;
            break;
        case WindowArea::FOCUS_ON_TOP:
        case WindowArea::FOCUS_ON_BOTTOM:
            pointerStyle.id = MOUSE_ICON::NORTH_SOUTH;
            break;
        case WindowArea::FOCUS_ON_LEFT:
        case WindowArea::FOCUS_ON_RIGHT:
            pointerStyle.id = MOUSE_ICON::WEST_EAST;
            break;
        case WindowArea::TOP_LEFT_LIMIT:
            pointerStyle.id = MOUSE_ICON::SOUTH_EAST;
            break;
        case WindowArea::TOP_RIGHT_LIMIT:
            pointerStyle.id = MOUSE_ICON::SOUTH_WEST;
            break;
        case WindowArea::TOP_LIMIT:
            pointerStyle.id = MOUSE_ICON::SOUTH;
            break;
        case WindowArea::LEFT_LIMIT:
            pointerStyle.id = MOUSE_ICON::EAST;
            break;
        case WindowArea::RIGHT_LIMIT:
            pointerStyle.id = MOUSE_ICON::WEST;
            break;
        case WindowArea::BOTTOM_LEFT_LIMIT:
            pointerStyle.id = MOUSE_ICON::NORTH_WEST;
            break;
        case WindowArea::BOTTOM_LIMIT:
            pointerStyle.id = MOUSE_ICON::NORTH_WEST;
            break;
        case WindowArea::BOTTOM_RIGHT_LIMIT:
            pointerStyle.id = MOUSE_ICON::NORTH_WEST;
            break;
        case WindowArea::FOCUS_ON_INNER:
            int32_t ret = GetPointerStyle(pid, winId, pointerStyle);
            CHKNOKRV(ret, "Get pointer style failed, pointerStyleInfo is nullptr");
            break;
    }
}

void InputWindowsManager::SetWindowPointerStyle(WindowArea area, int32_t pid, int32_t windowId)
{
    CALL_DEBUG_ENTER;
    PointerStyle pointerStyle;
    auto visible = IPointerDrawingManager::GetInstance()->GetPointerVisible(pid);
    IPointerDrawingManager::GetInstance()->SetPointerVisible(pid, visible);
    GetPointerStyleByArea(area, pid, windowId, pointerStyle);
    if (lastPointerStyle_.id == pointerStyle.id) {
        MMI_HILOGE("Tha lastPointerStyle is  totally equal with this, no need to change it");
        return;
    }
    lastPointerStyle_.id = pointerStyle.id;
    std::map<MOUSE_ICON, IconStyle> mouseIcons = IPointerDrawingManager::GetInstance()->GetMouseIconPath();
    if (windowId != GLOBAL_WINDOW_ID && (pointerStyle.id == MOUSE_ICON::DEFAULT &&
        mouseIcons[MOUSE_ICON(pointerStyle.id)].iconPath != defaultIconPath)) {
        PointerStyle style;
        int32_t ret = WinMgr->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style);
        if (ret != RET_OK) {
            MMI_HILOGE("Get global pointer style failed!");
            return;
        }
        lastPointerStyle_ = style;
    }
    MMI_HILOGI("Window id:%{public}d set pointer style:%{public}d success", windowId, lastPointerStyle_.id);
    IPointerDrawingManager::GetInstance()->DrawPointerStyle(lastPointerStyle_);
}

#ifdef OHOS_BUILD_ENABLE_POINTER
void InputWindowsManager::SendPointerEvent(int32_t pointerAction)
{
    CALL_INFO_TRACE;
    CHKPV(udsServer_);
    auto pointerEvent = PointerEvent::Create();
    CHKPV(pointerEvent);
    pointerEvent->UpdateId();
    MouseLocation mouseLocation = GetMouseInfo();
    lastLogicX_ = mouseLocation.physicalX;
    lastLogicY_ = mouseLocation.physicalY;
    if (pointerAction == PointerEvent::POINTER_ACTION_ENTER_WINDOW ||
        Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        auto touchWindow = GetWindowInfo(lastLogicX_, lastLogicY_);
        if (!touchWindow) {
            MMI_HILOGE("TouchWindow is nullptr");
            return;
        }
        lastWindowInfo_ = *touchWindow;
    }
    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(lastLogicX_ - lastWindowInfo_.area.x);
    pointerItem.SetWindowY(lastLogicY_ - lastWindowInfo_.area.y);
    pointerItem.SetDisplayX(lastLogicX_);
    pointerItem.SetDisplayY(lastLogicY_);
    pointerItem.SetPointerId(0);

    pointerEvent->SetTargetDisplayId(-1);
    auto displayId = pointerEvent->GetTargetDisplayId();
    if (!UpdateDisplayId(displayId)) {
        MMI_HILOGE("This display:%{public}d is not existent", displayId);
        return;
    }
    pointerEvent->SetTargetDisplayId(displayId);
    pointerEvent->SetTargetWindowId(lastWindowInfo_.id);
    pointerEvent->SetAgentWindowId(lastWindowInfo_.agentWindowId);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    int64_t time = GetSysClockTime();
    pointerEvent->SetActionTime(time);
    pointerEvent->SetActionStartTime(time);
    pointerEvent->UpdateId();
    if (extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_MOUSE) {
        pointerEvent->SetBuffer(extraData_.buffer);
        UpdatePointerAction(pointerEvent);
    } else {
        pointerEvent->ClearBuffer();
    }
    auto fd = udsServer_->GetClientFd(lastWindowInfo_.pid);
    auto sess = udsServer_->GetSession(fd);
    CHKPRV(sess, "The last window has disappeared");
    NetPacket pkt(MmiMessageId::ON_POINTER_EVENT);
    InputEventDataTransformation::Marshalling(pointerEvent, pkt);
    if (!sess->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}

void InputWindowsManager::DispatchPointer(int32_t pointerAction)
{
    CALL_INFO_TRACE;
    CHKPV(udsServer_);
    if (!IPointerDrawingManager::GetInstance()->GetMouseDisplayState()) {
        MMI_HILOGI("the mouse is hide");
        return;
    }
    if (lastPointerEvent_ == nullptr) {
        SendPointerEvent(pointerAction);
        return;
    }
    auto pointerEvent = PointerEvent::Create();
    CHKPV(pointerEvent);
    pointerEvent->UpdateId();

    PointerEvent::PointerItem lastPointerItem;
    int32_t lastPointerId = lastPointerEvent_->GetPointerId();
    if (!lastPointerEvent_->GetPointerItem(lastPointerId, lastPointerItem)) {
        MMI_HILOGE("GetPointerItem:%{public}d fail", lastPointerId);
        return;
    }
    if (pointerAction == PointerEvent::POINTER_ACTION_ENTER_WINDOW) {
        std::optional<WindowInfo> windowInfo;
        if (lastPointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_MOVE &&
            lastPointerEvent_->GetPressedButtons().empty()) {
            windowInfo = GetWindowInfo(lastLogicX_, lastLogicY_);
        } else {
            windowInfo = SelectWindowInfo(lastLogicX_, lastLogicY_, lastPointerEvent_);
        }
        if (!windowInfo) {
            MMI_HILOGE("windowInfo is nullptr");
            return;
        }
        if (windowInfo->id != lastWindowInfo_.id) {
            lastWindowInfo_ = *windowInfo;
        }
    }
    PointerEvent::PointerItem currentPointerItem;
    currentPointerItem.SetWindowX(lastLogicX_ - lastWindowInfo_.area.x);
    currentPointerItem.SetWindowY(lastLogicY_ - lastWindowInfo_.area.y);
    currentPointerItem.SetDisplayX(lastPointerItem.GetDisplayX());
    currentPointerItem.SetDisplayY(lastPointerItem.GetDisplayY());
    currentPointerItem.SetPointerId(0);

    pointerEvent->SetTargetDisplayId(lastPointerEvent_->GetTargetDisplayId());
    pointerEvent->SetTargetWindowId(lastWindowInfo_.id);
    pointerEvent->SetAgentWindowId(lastWindowInfo_.agentWindowId);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(currentPointerItem);
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetSourceType(lastPointerEvent_->GetSourceType());
    int64_t time = GetSysClockTime();
    pointerEvent->SetActionTime(time);
    pointerEvent->SetActionStartTime(time);
    pointerEvent->SetDeviceId(lastPointerEvent_->GetDeviceId());
    if (extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_MOUSE) {
        pointerEvent->SetBuffer(extraData_.buffer);
        UpdatePointerAction(pointerEvent);
    } else {
        pointerEvent->ClearBuffer();
    }
    int pid = lastWindowInfo_.pid;
    if (pointerAction == PointerEvent::POINTER_ACTION_LEAVE_WINDOW) {
        pointerEvent->SetAgentWindowId(lastWindowInfo_.id);
    }
    auto fd = udsServer_->GetClientFd(pid);
    if (fd == RET_ERR) {
        auto windowInfo = GetWindowInfo(lastLogicX_, lastLogicY_);
        if (!windowInfo) {
            MMI_HILOGE("The windowInfo is nullptr");
            return;
        }
        fd = udsServer_->GetClientFd(windowInfo->pid);
    }
    auto sess = udsServer_->GetSession(fd);
    if (sess == nullptr) {
        MMI_HILOGI("The last window has disappeared");
        return;
    }

    NetPacket pkt(MmiMessageId::ON_POINTER_EVENT);
    InputEventDataTransformation::Marshalling(pointerEvent, pkt);
    if (!sess->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}

void InputWindowsManager::NotifyPointerToWindow()
{
    CALL_INFO_TRACE;
    std::optional<WindowInfo> windowInfo;
    CHKPV(lastPointerEvent_);
    if (lastPointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_MOVE &&
        lastPointerEvent_->GetPressedButtons().empty()) {
        windowInfo = GetWindowInfo(lastLogicX_, lastLogicY_);
    } else {
        windowInfo = SelectWindowInfo(lastLogicX_, lastLogicY_, lastPointerEvent_);
    }
    if (!windowInfo) {
        MMI_HILOGE("The windowInfo is nullptr");
        return;
    }
    if (windowInfo->id == lastWindowInfo_.id) {
        MMI_HILOGI("The mouse pointer does not leave the window:%{public}d", lastWindowInfo_.id);
        lastWindowInfo_ = *windowInfo;
        return;
    }
    bool isFindLastWindow = false;
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        if (item.id == lastWindowInfo_.id) {
            DispatchPointer(PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
            isFindLastWindow = true;
            break;
        }
    }
    if (!isFindLastWindow) {
        if (udsServer_ != nullptr && udsServer_->GetClientFd(lastWindowInfo_.pid) != INVALID_FD) {
            DispatchPointer(PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
        }
    }
    lastWindowInfo_ = *windowInfo;
    DispatchPointer(PointerEvent::POINTER_ACTION_ENTER_WINDOW);
}
#endif // OHOS_BUILD_ENABLE_POINTER

void InputWindowsManager::PrintWindowInfo(const std::vector<WindowInfo> &windowsInfo)
{
    for (const auto &item : windowsInfo) {
        MMI_HILOGD("windowsInfos,id:%{public}d,pid:%{public}d,uid:%{public}d,"
            "area.x:%{public}d,area.y:%{public}d,area.width:%{public}d,area.height:%{public}d,"
            "defaultHotAreas.size:%{public}zu,pointerHotAreas.size:%{public}zu,"
            "agentWindowId:%{public}d,flags:%{public}d,action:%{public}d,displayId:%{public}d,"
            "zOrder:%{public}f",
            item.id, item.pid, item.uid, item.area.x, item.area.y, item.area.width,
            item.area.height, item.defaultHotAreas.size(), item.pointerHotAreas.size(),
            item.agentWindowId, item.flags, item.action, item.displayId, item.zOrder);
        for (const auto &win : item.defaultHotAreas) {
            MMI_HILOGD("defaultHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                win.x, win.y, win.width, win.height);
        }
        for (const auto &pointer : item.pointerHotAreas) {
            MMI_HILOGD("pointerHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                pointer.x, pointer.y, pointer.width, pointer.height);
        }

        std::string dump;
        dump += StringPrintf("pointChangeAreas:[");
        for (auto it : item.pointerChangeAreas) {
            dump += StringPrintf("%d,", it);
        }
        dump += StringPrintf("]\n");

        dump += StringPrintf("transform:[");
        for (auto it : item.transform) {
            dump += StringPrintf("%f,", it);
        }
        dump += StringPrintf("]\n");
        std::istringstream stream(dump);
        std::string line;
        while (std::getline(stream, line, '\n')) {
            MMI_HILOGD("%{public}s", line.c_str());
        }
    }
}

void InputWindowsManager::PrintWindowGroupInfo(const WindowGroupInfo &windowGroupInfo)
{
    MMI_HILOGD("windowsGroupInfo,focusWindowId:%{public}d,displayId:%{public}d",
        windowGroupInfo.focusWindowId, windowGroupInfo.displayId);
    PrintWindowInfo(windowGroupInfo.windowsInfo);
}

void InputWindowsManager::PrintDisplayInfo()
{
    MMI_HILOGD("logicalInfo,width:%{public}d,height:%{public}d,focusWindowId:%{public}d",
        displayGroupInfo_.width, displayGroupInfo_.height, displayGroupInfo_.focusWindowId);
    MMI_HILOGD("windowsInfos,num:%{public}zu", displayGroupInfo_.windowsInfo.size());
    PrintWindowInfo(displayGroupInfo_.windowsInfo);

    MMI_HILOGD("displayInfos,num:%{public}zu", displayGroupInfo_.displaysInfo.size());
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        MMI_HILOGD("displayInfos,id:%{public}d,x:%{public}d,y:%{public}d,"
            "width:%{public}d,height:%{public}d,name:%{public}s,"
            "uniq:%{public}s,direction:%{public}d",
            item.id, item.x, item.y, item.width, item.height, item.name.c_str(),
            item.uniq.c_str(), item.direction);
    }
}

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
const DisplayInfo* InputWindowsManager::GetPhysicalDisplay(int32_t id) const
{
    for (auto &it : displayGroupInfo_.displaysInfo) {
        if (it.id == id) {
            return &it;
        }
    }
    MMI_HILOGW("Failed to obtain physical(%{public}d) display", id);
    return nullptr;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_TOUCH
const DisplayInfo* InputWindowsManager::FindPhysicalDisplayInfo(const std::string& uniq) const
{
    for (auto &it : displayGroupInfo_.displaysInfo) {
        if (it.uniq == uniq) {
            return &it;
        }
    }
    MMI_HILOGE("Failed to search for Physical,uniq:%{public}s", uniq.c_str());
    return nullptr;
}

void InputWindowsManager::RotateTouchScreen(DisplayInfo info, LogicalCoordinate& coord) const
{
    const Direction direction = info.direction;
    if (direction == DIRECTION0) {
        MMI_HILOGD("direction is DIRECTION0");
        return;
    }
    if (direction == DIRECTION90) {
        MMI_HILOGD("direction is DIRECTION90");
        int32_t temp = coord.x;
        coord.x = info.width - coord.y;
        coord.y = temp;
        MMI_HILOGD("physicalX:%{public}d, physicalY:%{public}d", coord.x, coord.y);
        return;
    }
    if (direction == DIRECTION180) {
        MMI_HILOGD("direction is DIRECTION180");
        coord.x = info.width - coord.x;
        coord.y = info.height - coord.y;
        MMI_HILOGD("physicalX:%{public}d, physicalY:%{public}d", coord.x, coord.y);
        return;
    }
    if (direction == DIRECTION270) {
        MMI_HILOGD("direction is DIRECTION270");
        int32_t temp = coord.y;
        coord.y = info.height - coord.x;
        coord.x = temp;
        MMI_HILOGD("physicalX:%{public}d, physicalY:%{public}d", coord.x, coord.y);
    }
}

void InputWindowsManager::GetPhysicalDisplayCoord(struct libinput_event_touch* touch,
    const DisplayInfo& info, EventTouch& touchInfo)
{
    auto width = info.width;
    auto height = info.height;
    if (info.direction == DIRECTION90 || info.direction == DIRECTION270) {
        width = info.height;
        height = info.width;
    }
    LogicalCoordinate coord {
        .x = static_cast<int32_t>(libinput_event_touch_get_x_transformed(touch, width)),
        .y = static_cast<int32_t>(libinput_event_touch_get_y_transformed(touch, height)),
    };
    RotateTouchScreen(info, coord);
    touchInfo.point.x = coord.x;
    touchInfo.point.y = coord.y;
    touchInfo.toolRect.point.x = static_cast<int32_t>(libinput_event_touch_get_tool_x_transformed(touch, width));
    touchInfo.toolRect.point.y = static_cast<int32_t>(libinput_event_touch_get_tool_y_transformed(touch, height));
    touchInfo.toolRect.width = static_cast<int32_t>(
        libinput_event_touch_get_tool_width_transformed(touch, width));
    touchInfo.toolRect.height = static_cast<int32_t>(
        libinput_event_touch_get_tool_height_transformed(touch, height));
}

bool InputWindowsManager::TouchPointToDisplayPoint(int32_t deviceId, struct libinput_event_touch* touch,
    EventTouch& touchInfo, int32_t& physicalDisplayId)
{
    CHKPF(touch);
    std::string screenId = bindInfo_.GetBindDisplayNameByInputDevice(deviceId);
    if (screenId.empty()) {
        screenId = "default0";
    }
    auto info = FindPhysicalDisplayInfo(screenId);
    CHKPF(info);
    physicalDisplayId = info->id;
    if ((info->width <= 0) || (info->height <= 0)) {
        MMI_HILOGE("Get DisplayInfo is error");
        return false;
    }
    GetPhysicalDisplayCoord(touch, *info, touchInfo);
    return true;
}

bool InputWindowsManager::TransformTipPoint(struct libinput_event_tablet_tool* tip,
    LogicalCoordinate& coord, int32_t& displayId) const
{
    CHKPF(tip);
    auto displayInfo = FindPhysicalDisplayInfo("default0");
    CHKPF(displayInfo);
    MMI_HILOGD("PhysicalDisplay.width:%{public}d, PhysicalDisplay.height:%{public}d, "
               "PhysicalDisplay.topLeftX:%{public}d, PhysicalDisplay.topLeftY:%{public}d",
               displayInfo->width, displayInfo->height, displayInfo->x, displayInfo->y);
    displayId = displayInfo->id;
    PhysicalCoordinate phys {
        .x = libinput_event_tablet_tool_get_x_transformed(tip, displayInfo->width),
        .y = libinput_event_tablet_tool_get_y_transformed(tip, displayInfo->height)
    };

    coord.x = static_cast<int32_t>(phys.x);
    coord.y = static_cast<int32_t>(phys.y);
    MMI_HILOGD("physicalX:%{public}f, physicalY:%{public}f, displayId:%{public}d", phys.x, phys.y, displayId);
    return true;
}

bool InputWindowsManager::CalculateTipPoint(struct libinput_event_tablet_tool* tip,
    int32_t& targetDisplayId, LogicalCoordinate& coord) const
{
    CHKPF(tip);
    if (!TransformTipPoint(tip, coord, targetDisplayId)) {
        return false;
    }
    return true;
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
const DisplayGroupInfo& InputWindowsManager::GetDisplayGroupInfo()
{
    return displayGroupInfo_;
}

#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
bool InputWindowsManager::IsNeedRefreshLayer(int32_t windowId)
{
    CALL_DEBUG_ENTER;
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        return true;
    }
    MouseLocation mouseLocation = GetMouseInfo();
    int32_t displayId = MouseEventHdr->GetDisplayId();
    if (displayId < 0) {
        displayId = displayGroupInfo_.displaysInfo[0].id;
    }
    auto displayInfo = GetPhysicalDisplay(displayId);
    CHKPF(displayInfo);
    int32_t logicX = mouseLocation.physicalX + displayInfo->x;
    int32_t logicY = mouseLocation.physicalY + displayInfo->y;
    std::optional<WindowInfo> touchWindow = GetWindowInfo(logicX, logicY);
    if (!touchWindow) {
        MMI_HILOGE("TouchWindow is nullptr");
        return false;
    }
    if (touchWindow->id == windowId || windowId == GLOBAL_WINDOW_ID) {
        MMI_HILOGD("Need refresh pointer style, focusWindow type:%{public}d, window type:%{public}d",
            touchWindow->id, windowId);
        return true;
    }
    MMI_HILOGD("Not need refresh pointer style, focusWindow type:%{public}d, window type:%{public}d",
        touchWindow->id, windowId);
    return false;
}
#endif

void InputWindowsManager::OnSessionLost(SessionPtr session)
{
    CALL_DEBUG_ENTER;
    CHKPV(session);
    int32_t pid = session->GetPid();

    auto it = pointerStyle_.find(pid);
    if (it != pointerStyle_.end()) {
        pointerStyle_.erase(it);
        MMI_HILOGD("Clear the pointer style map, pd:%{public}d", pid);
    }
}

int32_t InputWindowsManager::UpdatePoinerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    auto it = pointerStyle_.find(pid);
    if (it == pointerStyle_.end()) {
        MMI_HILOGE("The pointer style map is not include param pd:%{public}d", pid);
        return COMMON_PARAMETER_ERROR;
    }
    auto iter = it->second.find(windowId);
    if (iter != it->second.end()) {
        iter->second = pointerStyle;
        return RET_OK;
    }

    for (const auto& windowInfo : displayGroupInfo_.windowsInfo) {
        if (windowId == windowInfo.id && pid == windowInfo.pid) {
            auto iterator = it->second.insert(std::make_pair(windowId, pointerStyle));
            if (!iterator.second) {
                MMI_HILOGW("The window type is duplicated");
            }
            return RET_OK;
        }
    }
    MMI_HILOGE("The window id is invalid");
    return COMMON_PARAMETER_ERROR;
}

int32_t InputWindowsManager::UpdateSceneBoardPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    // update the pointerStyle for sceneboard
    if (displayGroupInfo_.windowsInfo.size() != 1) {
        MMI_HILOGE("More than one window given in scene board scene!");
        return RET_ERR;
    }
    auto scenePid = displayGroupInfo_.windowsInfo[0].pid;
    auto sceneWinId = displayGroupInfo_.windowsInfo[0].id;
    auto sceneIter = pointerStyle_.find(scenePid);
    if (sceneIter == pointerStyle_.end() || sceneIter->second.find(sceneWinId) == sceneIter->second.end()) {
        pointerStyle_[scenePid] = {};
        MMI_HILOGE("SceneBoardPid %{public}d or windowId:%{public}d  does not exist on pointerStyle_",
            scenePid, sceneWinId);
    }
    pointerStyle_[scenePid][sceneWinId] = pointerStyle;
    MMI_HILOGD("Sceneboard pid:%{public}d windowId:%{public}d is set to %{public}d",
        scenePid, sceneWinId, pointerStyle.id);
    auto it = pointerStyle_.find(pid);
    if (it == pointerStyle_.end()) {
        MMI_HILOGE("Pid:%{public}d does not exist in mmi,", pid);
        std::map<int32_t, PointerStyle> tmpPointerStyle = {{windowId, pointerStyle}};
        auto res = pointerStyle_.insert(std::make_pair(pid, tmpPointerStyle));
        if (!res.second) return RET_ERR;
        return RET_OK;
    }
    auto iter = it->second.find(windowId);
    if (iter == it->second.end()) {
        auto res = it->second.insert(std::make_pair(windowId, pointerStyle));
        if (!res.second) return RET_ERR;
        return RET_OK;
    }
    iter->second = pointerStyle;
    return RET_OK;
}

int32_t InputWindowsManager::SetPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (windowId == GLOBAL_WINDOW_ID) {
        globalStyle_.id = pointerStyle.id;
        MMI_HILOGD("Setting global pointer style");
        return RET_OK;
    }
    MMI_HILOGD("start to get pid by window %{public}d", windowId);
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        return UpdatePoinerStyle(pid, windowId, pointerStyle);
    }
    return UpdateSceneBoardPointerStyle(pid, windowId, pointerStyle);
}

int32_t InputWindowsManager::ClearWindowPointerStyle(int32_t pid, int32_t windowId)
{
    CALL_DEBUG_ENTER;
    auto it = pointerStyle_.find(pid);
    if (it == pointerStyle_.end()) {
        MMI_HILOGE("Pid: %{public}d does not exist in mmi", pid);
        return RET_OK;
    }
    auto windowIt = it->second.find(windowId);
    if (windowIt == it->second.end()) {
        MMI_HILOGE("windowId %{public}d does not exist in pid%{public}d", windowId, pid);
        return RET_OK;
    }

    it->second.erase(windowIt);
    return RET_OK;
}

int32_t InputWindowsManager::GetPointerStyle(int32_t pid, int32_t windowId, PointerStyle &pointerStyle) const
{
    CALL_DEBUG_ENTER;
    if (windowId == GLOBAL_WINDOW_ID) {
        MMI_HILOGD("Getting global pointer style");
        pointerStyle.id = globalStyle_.id;
        return RET_OK;
    }
    auto it = pointerStyle_.find(pid);
    if (it == pointerStyle_.end()) {
        if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
            pointerStyle.id = globalStyle_.id;
            return RET_OK;
        }
        MMI_HILOGE("The pointer style map is not include param pd, %{public}d", pid);
        return RET_OK;
    }
    auto iter = it->second.find(windowId);
    if (iter == it->second.end()) {
        pointerStyle.id = globalStyle_.id;
        return RET_OK;
    }

    MMI_HILOGD("Window type:%{public}d get pointer style:%{public}d success", windowId, iter->second.id);
    pointerStyle = iter->second;
    return RET_OK;
}

void InputWindowsManager::InitPointerStyle()
{
    CALL_DEBUG_ENTER;
    PointerStyle pointerStyle;
    pointerStyle.id = DEFAULT_POINTER_STYLE;
    for (const auto& windowItem : displayGroupInfo_.windowsInfo) {
        int32_t pid = windowItem.pid;
        auto it = pointerStyle_.find(pid);
        if (it == pointerStyle_.end()) {
            std::map<int32_t, PointerStyle> tmpPointerStyle = {};
            auto iter = pointerStyle_.insert(std::make_pair(pid, tmpPointerStyle));
            if (!iter.second) {
                MMI_HILOGW("The pd is duplicated");
            }
            continue;
        }
    }
    MMI_HILOGD("Number of pointer style:%{public}zu", pointerStyle_.size());
}

#endif // OHOS_BUILD_ENABLE_POINTER

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
bool InputWindowsManager::IsInHotArea(int32_t x, int32_t y, const std::vector<Rect> &rects,
    const WindowInfo &window) const
{
    auto windowXY = TransformWindowXY(window, x, y);
    auto windowX = windowXY.first;
    auto windowY = windowXY.second;
    for (const auto &item : rects) {
        int32_t displayMaxX = 0;
        int32_t displayMaxY = 0;
        if (!AddInt32(item.x, item.width, displayMaxX)) {
            MMI_HILOGE("The addition of displayMaxX overflows");
            return false;
        }
        if (!AddInt32(item.y, item.height, displayMaxY)) {
            MMI_HILOGE("The addition of displayMaxY overflows");
            return false;
        }
        if (((windowX >= item.x) && (windowX < displayMaxX)) &&
            (windowY >= item.y) && (windowY < displayMaxY)) {
            return true;
        }
    }
    return false;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_TOUCH
void InputWindowsManager::AdjustDisplayCoordinate(
    const DisplayInfo& displayInfo, int32_t& physicalX, int32_t& physicalY) const
{
    int32_t width = displayInfo.width;
    int32_t height = displayInfo.height;
    if (physicalX <= 0) {
        physicalX = 0;
    }
    if (physicalX >= width && width > 0) {
        physicalX = width - 1;
    }
    if (physicalY <= 0) {
        physicalY = 0;
    }
    if (physicalY >= height && height > 0) {
        physicalY = height - 1;
    }
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
bool InputWindowsManager::UpdateDisplayId(int32_t& displayId)
{
    if (displayGroupInfo_.displaysInfo.empty()) {
        MMI_HILOGE("logicalDisplays_is empty");
        return false;
    }
    if (displayId < 0) {
        displayId = displayGroupInfo_.displaysInfo[0].id;
        return true;
    }
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        if (item.id == displayId) {
            return true;
        }
    }
    return false;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
std::optional<WindowInfo> InputWindowsManager::SelectWindowInfo(int32_t logicalX, int32_t logicalY,
    const std::shared_ptr<PointerEvent>& pointerEvent)
{
    CALL_DEBUG_ENTER;
    int32_t action = pointerEvent->GetPointerAction();
    bool checkFlag = (firstBtnDownWindowId_ == -1) ||
        ((action == PointerEvent::POINTER_ACTION_BUTTON_DOWN) && (pointerEvent->GetPressedButtons().size() == 1)) ||
        ((action == PointerEvent::POINTER_ACTION_MOVE) && (pointerEvent->GetPressedButtons().empty())) ||
        (extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_MOUSE) ||
        (action == PointerEvent::POINTER_ACTION_PULL_UP);
    std::vector<WindowInfo> windowsInfo = GetWindowGroupInfoByDisplayId(pointerEvent->GetTargetDisplayId());
    if (checkFlag) {
        int32_t targetWindowId = pointerEvent->GetTargetWindowId();
        for (const auto &item : windowsInfo) {
            if ((item.flags & WindowInfo::FLAG_BIT_UNTOUCHABLE) == WindowInfo::FLAG_BIT_UNTOUCHABLE ||
                !IsValidZorderWindow(item, pointerEvent)) {
                MMI_HILOGD("Skip the untouchable or invalid zOrder window to continue searching, "
                    "window:%{public}d, flags:%{public}d, pid:%{public}d", item.id, item.flags, item.pid);
                continue;
            } else if ((extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_MOUSE) ||
                (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP)) {
                if (IsInHotArea(logicalX, logicalY, item.pointerHotAreas, item)) {
                    firstBtnDownWindowId_ = item.id;
                    MMI_HILOGD("Mouse event select pull window, window:%{public}d, pid:%{public}d",
                        firstBtnDownWindowId_, item.pid);
                    break;
                } else {
                    continue;
                }
            } else if ((targetWindowId < 0) && (IsInHotArea(logicalX, logicalY, item.pointerHotAreas, item))) {
                firstBtnDownWindowId_ = item.id;
                MMI_HILOGD("Find out the dispatch window of this pointer event when the targetWindowId "
                    "hasn't been set up yet, window:%{public}d, pid:%{public}d", firstBtnDownWindowId_, item.pid);
                break;
            } else if ((targetWindowId >= 0) && (targetWindowId == item.id)) {
                firstBtnDownWindowId_ = targetWindowId;
                MMI_HILOGD("Find out the dispatch window of this pointer event when the targetWindowId "
                    "has been set up already, window:%{public}d, pid:%{public}d", firstBtnDownWindowId_, item.pid);
                break;
            } else {
                MMI_HILOGW("Continue searching for the dispatch window of this pointer event");
            }
        }
    }
    MMI_HILOGD("firstBtnDownWindowId_:%{public}d", firstBtnDownWindowId_);
    for (const auto &item : windowsInfo) {
        if (item.id == firstBtnDownWindowId_) {
            return std::make_optional(item);
        }
    }
    return std::nullopt;
}

std::optional<WindowInfo> InputWindowsManager::GetWindowInfo(int32_t logicalX, int32_t logicalY)
{
    CALL_DEBUG_ENTER;
    for (const auto& item : displayGroupInfo_.windowsInfo) {
        if ((item.flags & WindowInfo::FLAG_BIT_UNTOUCHABLE) == WindowInfo::FLAG_BIT_UNTOUCHABLE) {
            MMI_HILOGD("Skip the untouchable window to continue searching, "
                       "window:%{public}d, flags:%{public}d", item.id, item.flags);
            continue;
        } else if (IsInHotArea(logicalX, logicalY, item.pointerHotAreas, item)) {
            return std::make_optional(item);
        } else {
            MMI_HILOGW("Continue searching for the dispatch window");
        }
    }
    return std::nullopt;
}

void InputWindowsManager::UpdatePointerEvent(int32_t logicalX, int32_t logicalY,
    const std::shared_ptr<PointerEvent>& pointerEvent, const WindowInfo& touchWindow)
{
    CHKPV(pointerEvent);
    MMI_HILOGD("LastWindowInfo:%{public}d, touchWindow:%{public}d", lastWindowInfo_.id, touchWindow.id);
    if (lastWindowInfo_.id != touchWindow.id) {
        DispatchPointer(PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
        lastLogicX_ = logicalX;
        lastLogicY_ = logicalY;
        lastPointerEvent_ = pointerEvent;
        lastWindowInfo_ = touchWindow;
        DispatchPointer(PointerEvent::POINTER_ACTION_ENTER_WINDOW);
        return;
    }
    lastLogicX_ = logicalX;
    lastLogicY_ = logicalY;
    lastPointerEvent_ = pointerEvent;
    lastWindowInfo_ = touchWindow;
}

int32_t InputWindowsManager::SetHoverScrollState(bool state)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("Set mouse hover scroll state:%{public}d", state);
    std::string name = "isEnableHoverScroll";
    return PREFERENCES_MANAGER->SetBoolValue(name, mouseFileName, state);
}

bool InputWindowsManager::GetHoverScrollState() const
{
    CALL_DEBUG_ENTER;
    std::string name = "isEnableHoverScroll";
    bool state = PREFERENCES_MANAGER->GetBoolValue(name, true);
    MMI_HILOGD("Get mouse hover scroll state:%{public}d", state);
    return state;
}

int32_t InputWindowsManager::UpdateMouseTarget(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    auto displayId = pointerEvent->GetTargetDisplayId();
    if (!UpdateDisplayId(displayId)) {
        MMI_HILOGE("This display:%{public}d is not existent", displayId);
        return RET_ERR;
    }
    pointerEvent->SetTargetDisplayId(displayId);

    int32_t pointerId = pointerEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
        return RET_ERR;
    }
    auto physicalDisplayInfo = GetPhysicalDisplay(displayId);
    CHKPR(physicalDisplayInfo, ERROR_NULL_POINTER);
    int32_t logicalX = 0;
    int32_t logicalY = 0;
    if (!AddInt32(pointerItem.GetDisplayX(), physicalDisplayInfo->x, logicalX)) {
        MMI_HILOGE("The addition of logicalX overflows");
        return RET_ERR;
    }
    if (!AddInt32(pointerItem.GetDisplayY(), physicalDisplayInfo->y, logicalY)) {
        MMI_HILOGE("The addition of logicalY overflows");
        return RET_ERR;
    }
    auto touchWindow = SelectWindowInfo(logicalX, logicalY, pointerEvent);
    if (!touchWindow) {
        if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_BUTTON_DOWN || mouseDownInfo_.id == -1) {
            MMI_HILOGE("touchWindow is nullptr, targetWindow:%{public}d", pointerEvent->GetTargetWindowId());
            return RET_ERR;
        }
        touchWindow = std::make_optional(mouseDownInfo_);
        pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
        MMI_HILOGI("mouse event send cancel, window:%{public}d, pid:%{public}d", touchWindow->id, touchWindow->pid);
    }

    bool checkFlag = pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_UPDATE ||
        pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_BEGIN ||
        pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_END;
    if (checkFlag) {
        if ((!GetHoverScrollState()) && (displayGroupInfo_.focusWindowId != touchWindow->id)) {
            MMI_HILOGD("disable mouse hover scroll in inactive window, targetWindowId:%{public}d", touchWindow->id);
            return RET_OK;
        }
    }
    PointerStyle pointerStyle;
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        if (!IPointerDrawingManager::GetInstance()->GetMouseDisplayState()) {
            MMI_HILOGD("turn the mouseDisplay from false to true");
            IPointerDrawingManager::GetInstance()->SetMouseDisplayState(true);
        }
        pointerStyle = IPointerDrawingManager::GetInstance()->GetLastMouseStyle();
        MMI_HILOGD("showing the lastMouseStyle %{public}d, lastPointerStyle %{public}d",
            pointerStyle.id, lastPointerStyle_.id);
    } else {
        int32_t ret = GetPointerStyle(touchWindow->pid, touchWindow->id, pointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Get pointer style failed, pointerStyleInfo is nullptr");
            return ret;
        }
        if (!IPointerDrawingManager::GetInstance()->GetMouseDisplayState()) {
            IPointerDrawingManager::GetInstance()->SetMouseDisplayState(true);
            DispatchPointer(PointerEvent::POINTER_ACTION_ENTER_WINDOW);
        }
        IPointerDrawingManager::GetInstance()->UpdateDisplayInfo(*physicalDisplayInfo);
        WinInfo info = { .windowPid = touchWindow->pid, .windowId = touchWindow->id };
        IPointerDrawingManager::GetInstance()->OnWindowInfo(info);
    }
    IPointerDrawingManager::GetInstance()->DrawPointer(displayId, pointerItem.GetDisplayX(),
        pointerItem.GetDisplayY(), pointerStyle);

    if (captureModeInfo_.isCaptureMode && (touchWindow->id != captureModeInfo_.windowId)) {
        captureModeInfo_.isCaptureMode = false;
    }
    pointerEvent->SetTargetWindowId(touchWindow->id);
    pointerEvent->SetAgentWindowId(touchWindow->agentWindowId);
    auto windowXY = TransformWindowXY(*touchWindow, logicalX, logicalY);
    auto windowX = windowXY.first;
    auto windowY = windowXY.second;
    pointerItem.SetWindowX(windowX);
    pointerItem.SetWindowY(windowY);
    pointerEvent->UpdatePointerItem(pointerId, pointerItem);
    if ((extraData_.appended && (extraData_.sourceType == PointerEvent::SOURCE_TYPE_MOUSE)) ||
        (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP)) {
        pointerEvent->SetBuffer(extraData_.buffer);
        UpdatePointerAction(pointerEvent);
    } else {
        pointerEvent->ClearBuffer();
    }
    CHKPR(udsServer_, ERROR_NULL_POINTER);
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    UpdatePointerEvent(logicalX, logicalY, pointerEvent, *touchWindow);
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
    int32_t action = pointerEvent->GetPointerAction();
    if (action == PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        mouseDownInfo_ = *touchWindow;
    }
    if (action == PointerEvent::POINTER_ACTION_BUTTON_UP) {
        InitMouseDownInfo();
        MMI_HILOGD("Mouse up, clear mouse down info");
    }
    MMI_HILOGD("pid:%{public}d,id:%{public}d,agentWindowId:%{public}d,"
               "logicalX:%{public}d,logicalY:%{public}d,"
               "displayX:%{public}d,displayY:%{public}d,windowX:%{public}d,windowY:%{public}d",
               touchWindow->pid, touchWindow->id, touchWindow->agentWindowId,
               logicalX, logicalY, pointerItem.GetDisplayX(), pointerItem.GetDisplayY(), windowX, windowY);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP) {
        MMI_HILOGD("Clear extra data");
        ClearExtraData();
    }
    return ERR_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER

int32_t InputWindowsManager::SetMouseCaptureMode(int32_t windowId, bool isCaptureMode)
{
    if (windowId < 0) {
        MMI_HILOGE("Windowid(%{public}d) is invalid", windowId);
        return RET_ERR;
    }
    if ((captureModeInfo_.isCaptureMode == isCaptureMode) && !isCaptureMode) {
        MMI_HILOGE("Windowid:(%{public}d) is not capture mode", windowId);
        return RET_OK;
    }
    captureModeInfo_.windowId = windowId;
    captureModeInfo_.isCaptureMode = isCaptureMode;
    MMI_HILOGI("Windowid:(%{public}d) is (%{public}d)", windowId, isCaptureMode);
    return RET_OK;
}

bool InputWindowsManager::GetMouseIsCaptureMode() const
{
    return captureModeInfo_.isCaptureMode;
}

bool InputWindowsManager::IsNeedDrawPointer(PointerEvent::PointerItem &pointerItem) const
{
    if (pointerItem.GetToolType() == PointerEvent::TOOL_TYPE_PEN) {
        std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetInputDevice(pointerItem.GetDeviceId());
        if (inputDevice != nullptr) {
            MMI_HILOGD("name:%{public}s type:%{public}d bus:%{public}d, "
                       "version:%{public}d product:%{public}d vendor:%{public}d, "
                       "phys:%{public}s uniq:%{public}s",
                       inputDevice->GetName().c_str(), inputDevice->GetType(), inputDevice->GetBus(),
                       inputDevice->GetVersion(), inputDevice->GetProduct(), inputDevice->GetVendor(),
                       inputDevice->GetPhys().c_str(), inputDevice->GetUniq().c_str());
        }
        if (inputDevice != nullptr && inputDevice->GetBus() == BUS_USB) {
            return true;
        }
    }
    return false;
}

#ifdef OHOS_BUILD_ENABLE_TOUCH
int32_t InputWindowsManager::UpdateTouchScreenTarget(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    auto displayId = pointerEvent->GetTargetDisplayId();
    if (!UpdateDisplayId(displayId)) {
        MMI_HILOGE("This display is not existent");
        return RET_ERR;
    }
    pointerEvent->SetTargetDisplayId(displayId);

    int32_t pointerId = pointerEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
        return RET_ERR;
    }
    MMI_HILOGD("display:%{public}d", displayId);
    auto physicDisplayInfo = GetPhysicalDisplay(displayId);
    CHKPR(physicDisplayInfo, ERROR_NULL_POINTER);
    int32_t physicalX = pointerItem.GetDisplayX();
    int32_t physicalY = pointerItem.GetDisplayY();
    AdjustDisplayCoordinate(*physicDisplayInfo, physicalX, physicalY);
    int32_t logicalX = 0;
    int32_t logicalY = 0;
    if (!AddInt32(physicalX, physicDisplayInfo->x, logicalX)) {
        MMI_HILOGE("The addition of logicalX overflows");
        return RET_ERR;
    }
    if (!AddInt32(physicalY, physicDisplayInfo->y, logicalY)) {
        MMI_HILOGE("The addition of logicalY overflows");
        return RET_ERR;
    }
    WindowInfo *touchWindow = nullptr;
    auto targetWindowId = pointerItem.GetTargetWindowId();
    std::vector<WindowInfo> windowsInfo = GetWindowGroupInfoByDisplayId(pointerEvent->GetTargetDisplayId());
    for (auto &item : windowsInfo) {
        for (const auto &win : item.defaultHotAreas) {
            MMI_HILOGE("defaultHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                win.x, win.y, win.width, win.height);
        }

        if ((item.flags & WindowInfo::FLAG_BIT_UNTOUCHABLE) == WindowInfo::FLAG_BIT_UNTOUCHABLE ||
            !IsValidZorderWindow(item, pointerEvent)) {
            MMI_HILOGD("Skip the untouchable or invalid zOrder window to continue searching, "
                       "window:%{public}d, flags:%{public}d", item.id, item.flags);
            continue;
        }

        bool checkToolType = extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
            ((pointerItem.GetToolType() == PointerEvent::TOOL_TYPE_FINGER && extraData_.pointerId == pointerId) ||
            pointerItem.GetToolType() == PointerEvent::TOOL_TYPE_PEN);
        checkToolType = checkToolType || (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP);
        if (checkToolType) {
            if (IsInHotArea(logicalX, logicalY, item.defaultHotAreas, item)) {
                touchWindow = &item;
                break;
            } else {
                continue;
            }
        }
        if (targetWindowId >= 0) {
            if (item.id == targetWindowId) {
                touchWindow = &item;
                break;
            }
        } else if (IsInHotArea(logicalX, logicalY, item.defaultHotAreas, item)) {
            touchWindow = &item;
            break;
        }
    }
    if (touchWindow == nullptr) {
        auto it = touchItemDownInfos_.find(pointerId);
        if (it == touchItemDownInfos_.end() ||
            pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_DOWN) {
            MMI_HILOGE("The touchWindow is nullptr, logicalX:%{public}d, logicalY:%{public}d",
                logicalX, logicalY);
            return RET_ERR;
        }
        touchWindow = &it->second;
        pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
        MMI_HILOGD("touch event send cancel, window:%{public}d", touchWindow->id);
    }
    auto windowXY = TransformWindowXY(*touchWindow, logicalX, logicalY);
    auto windowX = windowXY.first;
    auto windowY = windowXY.second;
    MMI_HILOGE("touch event send to window:%{public}d", touchWindow->id);
    pointerEvent->SetTargetWindowId(touchWindow->id);
    pointerEvent->SetAgentWindowId(touchWindow->agentWindowId);
    pointerItem.SetDisplayX(physicalX);
    pointerItem.SetDisplayY(physicalY);
    pointerItem.SetWindowX(windowX);
    pointerItem.SetWindowY(windowY);
    pointerItem.SetToolWindowX(pointerItem.GetToolDisplayX() + physicDisplayInfo->x - touchWindow->area.x);
    pointerItem.SetToolWindowY(pointerItem.GetToolDisplayY() + physicDisplayInfo->y - touchWindow->area.y);
    pointerItem.SetTargetWindowId(touchWindow->id);
    pointerEvent->UpdatePointerItem(pointerId, pointerItem);
    bool checkExtraData = extraData_.appended && extraData_.sourceType == PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
        ((pointerItem.GetToolType() == PointerEvent::TOOL_TYPE_FINGER && extraData_.pointerId == pointerId) ||
        pointerItem.GetToolType() == PointerEvent::TOOL_TYPE_PEN);
    checkExtraData = checkExtraData || (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP);
    if (checkExtraData) {
        pointerEvent->SetBuffer(extraData_.buffer);
        UpdatePointerAction(pointerEvent);
        PullEnterLeaveEvent(logicalX, logicalY, pointerEvent, touchWindow);
    } else {
        pointerEvent->ClearBuffer();
        lastTouchEvent_ = nullptr;
        lastTouchWindowInfo_.id = -1;
    }
    MMI_HILOGI("pid:%{public}d,eventId:%{public}d,", touchWindow->pid, pointerEvent->GetId());
    MMI_HILOGD("logicalX:%{public}d,logicalY:%{public}d,"
               "physicalX:%{public}d,physicalY:%{public}d,windowX:%{public}d,windowY:%{public}d,"
               "displayId:%{public}d,TargetWindowId:%{public}d,AgentWindowId:%{public}d",
               logicalX, logicalY, physicalX, physicalY, windowX, windowY, displayId,
               pointerEvent->GetTargetWindowId(), pointerEvent->GetAgentWindowId());
    if (IsNeedDrawPointer(pointerItem)) {
        if (!IPointerDrawingManager::GetInstance()->GetMouseDisplayState()) {
            IPointerDrawingManager::GetInstance()->SetMouseDisplayState(true);
            DispatchPointer(PointerEvent::POINTER_ACTION_ENTER_WINDOW);
        }
        PointerStyle pointerStyle;
        int32_t ret = GetPointerStyle(touchWindow->pid, touchWindow->id, pointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Get pointer style failed, pointerStyleInfo is nullptr");
            return ret;
        }
        IPointerDrawingManager::GetInstance()->UpdateDisplayInfo(*physicDisplayInfo);
        WinInfo info = { .windowPid = touchWindow->pid, .windowId = touchWindow->id };
        IPointerDrawingManager::GetInstance()->OnWindowInfo(info);
        IPointerDrawingManager::GetInstance()->DrawPointer(displayId,
            pointerItem.GetDisplayX(), pointerItem.GetDisplayY(), pointerStyle);
    } else {
        if (IPointerDrawingManager::GetInstance()->GetMouseDisplayState()) {
            if (!checkExtraData) {
                DispatchPointer(PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
                IPointerDrawingManager::GetInstance()->SetMouseDisplayState(false);
            }
        }
    }

    int32_t pointerAction = pointerEvent->GetPointerAction();
    if (pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
        touchItemDownInfos_.insert(std::make_pair(pointerId, *touchWindow));
    }
    if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
        auto iter = touchItemDownInfos_.find(pointerId);
        if (iter != touchItemDownInfos_.end()) {
            touchItemDownInfos_.erase(iter);
            MMI_HILOGD("Clear the touch info, action is up, pointerid:%{public}d", pointerId);
        }
    }
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP) {
        MMI_HILOGD("Clear extra data");
        ClearExtraData();
    }
    return ERR_OK;
}

void InputWindowsManager::PullEnterLeaveEvent(int32_t logicalX, int32_t logicalY,
    const std::shared_ptr<PointerEvent> pointerEvent, const WindowInfo* touchWindow)
{
    CHKPV(pointerEvent);
    CHKPV(touchWindow);
    MMI_HILOGD("LastTouchWindowInfo:%{public}d, touchWindow:%{public}d", lastTouchWindowInfo_.id, touchWindow->id);
    if (lastTouchWindowInfo_.id != -1 && lastTouchWindowInfo_.id != touchWindow->id) {
        DispatchTouch(PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW);
        lastTouchLogicX_ = logicalX;
        lastTouchLogicY_ = logicalY;
        lastTouchEvent_ = pointerEvent;
        lastTouchWindowInfo_ = *touchWindow;
        DispatchTouch(PointerEvent::POINTER_ACTION_PULL_IN_WINDOW);
        return;
    }
    lastTouchLogicX_ = logicalX;
    lastTouchLogicY_ = logicalY;
    lastTouchEvent_ = pointerEvent;
    lastTouchWindowInfo_ = *touchWindow;
}

void InputWindowsManager::DispatchTouch(int32_t pointerAction)
{
    CALL_INFO_TRACE;
    CHKPV(udsServer_);
    CHKPV(lastTouchEvent_);
    if (pointerAction == PointerEvent::POINTER_ACTION_PULL_IN_WINDOW) {
        WindowInfo *touchWindow = nullptr;
        for (auto item : displayGroupInfo_.windowsInfo) {
            if ((item.flags & WindowInfo::FLAG_BIT_UNTOUCHABLE) == WindowInfo::FLAG_BIT_UNTOUCHABLE) {
                MMI_HILOGD("Skip the untouchable window to continue searching, "
                    "window:%{public}d, flags:%{public}d", item.id, item.flags);
                continue;
            }
            if (IsInHotArea(lastTouchLogicX_, lastTouchLogicY_, item.defaultHotAreas, item)) {
                touchWindow = &item;
                break;
            }
        }
        if (touchWindow == nullptr) {
            MMI_HILOGE("touchWindow is nullptr");
            return;
        }
        if (touchWindow->id != lastTouchWindowInfo_.id) {
            lastTouchWindowInfo_ = *touchWindow;
        }
    }
    auto pointerEvent = PointerEvent::Create();
    CHKPV(pointerEvent);
    PointerEvent::PointerItem lastPointerItem;
    int32_t lastPointerId = lastTouchEvent_->GetPointerId();
    if (!lastTouchEvent_->GetPointerItem(lastPointerId, lastPointerItem)) {
        MMI_HILOGE("GetPointerItem:%{public}d fail", lastPointerId);
        return;
    }
    PointerEvent::PointerItem currentPointerItem;
    currentPointerItem.SetWindowX(lastTouchLogicX_ - lastTouchWindowInfo_.area.x);
    currentPointerItem.SetWindowY(lastTouchLogicY_ - lastTouchWindowInfo_.area.y);
    currentPointerItem.SetDisplayX(lastPointerItem.GetDisplayX());
    currentPointerItem.SetDisplayY(lastPointerItem.GetDisplayY());
    currentPointerItem.SetPointerId(lastPointerId);

    pointerEvent->UpdateId();
    pointerEvent->SetTargetDisplayId(lastTouchEvent_->GetTargetDisplayId());
    pointerEvent->SetTargetWindowId(lastTouchWindowInfo_.id);
    pointerEvent->SetAgentWindowId(lastTouchWindowInfo_.agentWindowId);
    pointerEvent->SetPointerId(lastPointerId);
    pointerEvent->AddPointerItem(currentPointerItem);
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetBuffer(extraData_.buffer);
    pointerEvent->SetSourceType(lastTouchEvent_->GetSourceType());
    int64_t time = GetSysClockTime();
    pointerEvent->SetActionTime(time);
    pointerEvent->SetActionStartTime(time);
    pointerEvent->SetDeviceId(lastTouchEvent_->GetDeviceId());
    auto fd = udsServer_->GetClientFd(lastTouchWindowInfo_.pid);
    auto sess = udsServer_->GetSession(fd);
    if (sess == nullptr) {
        MMI_HILOGI("The last window has disappeared");
        return;
    }

    NetPacket pkt(MmiMessageId::ON_POINTER_EVENT);
    InputEventDataTransformation::Marshalling(pointerEvent, pkt);
    if (!sess->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
int32_t InputWindowsManager::UpdateTouchPadTarget(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    int32_t pointerAction =  pointerEvent->GetPointerAction();
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    switch (pointerAction) {
        case PointerEvent::POINTER_ACTION_BUTTON_DOWN:
        case PointerEvent::POINTER_ACTION_BUTTON_UP:
        case PointerEvent::POINTER_ACTION_MOVE: {
            return UpdateMouseTarget(pointerEvent);
        }
        case PointerEvent::POINTER_ACTION_DOWN: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
            pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
            return UpdateMouseTarget(pointerEvent);
        }
        case PointerEvent::POINTER_ACTION_UP: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
            pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
            pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
            return UpdateMouseTarget(pointerEvent);
        }
        default: {
            MMI_HILOGE("pointer action is unknown, pointerAction:%{public}d", pointerAction);
            return RET_ERR;
        }
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER

#ifdef OHOS_BUILD_ENABLE_JOYSTICK
int32_t InputWindowsManager::UpdateJoystickTarget(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    int32_t focusWindowId = displayGroupInfo_.focusWindowId;
    const WindowInfo* windowInfo = nullptr;
    std::vector<WindowInfo> windowsInfo = GetWindowGroupInfoByDisplayId(pointerEvent->GetTargetDisplayId());
    for (const auto &item : windowsInfo) {
        if (item.id == focusWindowId) {
            windowInfo = &item;
            break;
        }
    }
    CHKPR(windowInfo, ERROR_NULL_POINTER);
    pointerEvent->SetTargetWindowId(windowInfo->id);
    pointerEvent->SetAgentWindowId(windowInfo->agentWindowId);
    MMI_HILOGD("focusWindow:%{public}d, pid:%{public}d", focusWindowId, windowInfo->pid);

    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_JOYSTICK

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t InputWindowsManager::UpdateTargetPointer(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    auto source = pointerEvent->GetSourceType();
    switch (source) {
#ifdef OHOS_BUILD_ENABLE_TOUCH
        case PointerEvent::SOURCE_TYPE_TOUCHSCREEN: {
            return UpdateTouchScreenTarget(pointerEvent);
        }
#endif // OHOS_BUILD_ENABLE_TOUCH
#ifdef OHOS_BUILD_ENABLE_POINTER
        case PointerEvent::SOURCE_TYPE_MOUSE: {
            return UpdateMouseTarget(pointerEvent);
        }
        case PointerEvent::SOURCE_TYPE_TOUCHPAD: {
            return UpdateTouchPadTarget(pointerEvent);
        }
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_JOYSTICK
        case PointerEvent::SOURCE_TYPE_JOYSTICK: {
            return UpdateJoystickTarget(pointerEvent);
        }
#endif // OHOS_BUILD_ENABLE_JOYSTICK
        default: {
            MMI_HILOGE("Source type is unknown, source:%{public}d", source);
            break;
        }
    }
    return RET_ERR;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
bool InputWindowsManager::IsInsideDisplay(const DisplayInfo& displayInfo, int32_t physicalX, int32_t physicalY)
{
    return (physicalX >= 0 && physicalX < displayInfo.width) && (physicalY >= 0 && physicalY < displayInfo.height);
}

void InputWindowsManager::FindPhysicalDisplay(const DisplayInfo& displayInfo, int32_t& physicalX,
    int32_t& physicalY, int32_t& displayId)
{
    CALL_DEBUG_ENTER;
    int32_t logicalX = 0;
    int32_t logicalY = 0;
    if (!AddInt32(physicalX, displayInfo.x, logicalX)) {
        MMI_HILOGE("The addition of logicalX overflows");
        return;
    }
    if (!AddInt32(physicalY, displayInfo.y, logicalY)) {
        MMI_HILOGE("The addition of logicalY overflows");
        return;
    }
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        int32_t displayMaxX = 0;
        int32_t displayMaxY = 0;
        if (!AddInt32(item.x, item.width, displayMaxX)) {
            MMI_HILOGE("The addition of displayMaxX overflows");
            return;
        }
        if (!AddInt32(item.y, item.height, displayMaxY)) {
            MMI_HILOGE("The addition of displayMaxY overflows");
            return;
        }
        if ((logicalX >= item.x && logicalX < displayMaxX) &&
            (logicalY >= item.y && logicalY < displayMaxY)) {
            physicalX = logicalX - item.x;
            physicalY = logicalY - item.y;
            displayId = item.id;
            break;
        }
    }
}
void InputWindowsManager::UpdateAndAdjustMouseLocation(int32_t& displayId, double& x, double& y)
{
    auto displayInfo = GetPhysicalDisplay(displayId);
    CHKPV(displayInfo);
    int32_t integerX = static_cast<int32_t>(x);
    int32_t integerY = static_cast<int32_t>(y);
    int32_t lastDisplayId = displayId;
    if (!IsInsideDisplay(*displayInfo, integerX, integerY)) {
        FindPhysicalDisplay(*displayInfo, integerX, integerY, displayId);
    }
    if (displayId != lastDisplayId) {
        displayInfo = GetPhysicalDisplay(displayId);
        CHKPV(displayInfo);
    }
    int32_t width = 0;
    int32_t height = 0;
    if (displayInfo->direction == DIRECTION0 || displayInfo->direction == DIRECTION180) {
        width = displayInfo->width;
        height = displayInfo->height;
    } else {
        height = displayInfo->width;
        width = displayInfo->height;
    }
    if (integerX < 0) {
        integerX = 0;
    }
    if (integerX >= width) {
        integerX = width - 1;
    }
    if (integerY < 0) {
        integerY = 0;
    }
    if (integerY >= height) {
        integerY = height - 1;
    }
    x = static_cast<double>(integerX) + (x - floor(x));
    y = static_cast<double>(integerY) + (y - floor(y));
    mouseLocation_.physicalX = integerX;
    mouseLocation_.physicalY = integerY;
    MMI_HILOGD("Mouse Data: physicalX:%{public}d,physicalY:%{public}d, displayId:%{public}d",
        mouseLocation_.physicalX, mouseLocation_.physicalY, displayId);
}

MouseLocation InputWindowsManager::GetMouseInfo()
{
    if (mouseLocation_.physicalX == -1 || mouseLocation_.physicalY == -1) {
        if (!displayGroupInfo_.displaysInfo.empty()) {
            mouseLocation_.physicalX = displayGroupInfo_.displaysInfo[0].width / 2;
            mouseLocation_.physicalY = displayGroupInfo_.displaysInfo[0].height / 2;
        }
    }
    return mouseLocation_;
}
#endif // OHOS_BUILD_ENABLE_POINTER

int32_t InputWindowsManager::AppendExtraData(const ExtraData& extraData)
{
    CALL_DEBUG_ENTER;
    extraData_.appended = extraData.appended;
    extraData_.buffer = extraData.buffer;
    extraData_.sourceType = extraData.sourceType;
    extraData_.pointerId = extraData.pointerId;
    return RET_OK;
}

void InputWindowsManager::ClearExtraData()
{
    CALL_DEBUG_ENTER;
    extraData_.appended = false;
    extraData_.buffer.clear();
    extraData_.sourceType = -1;
    extraData_.pointerId = -1;
}

bool InputWindowsManager::IsWindowVisible(int32_t pid)
{
    CALL_DEBUG_ENTER;
    if (pid < 0) {
        MMI_HILOGE("pid is invalid");
        return true;
    }
    std::vector<sptr<Rosen::WindowVisibilityInfo>> infos;
    Rosen::WindowManager::GetInstance().GetVisibilityWindowInfo(infos);
    for (const auto &it: infos) {
        if (pid == it->pid_ &&
            it->visibilityState_ < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION) {
            MMI_HILOGD("pid:%{public}d has visible window", pid);
            return true;
        }
    }
    MMI_HILOGD("pid:%{public}d doesn't have visible window", pid);
    return false;
}

void InputWindowsManager::UpdatePointerAction(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    int32_t action = pointerEvent->GetPointerAction();
    switch (action) {
        case PointerEvent::POINTER_ACTION_MOVE: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_PULL_MOVE);
            break;
        }
        case PointerEvent::POINTER_ACTION_BUTTON_UP:
        case PointerEvent::POINTER_ACTION_UP: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_PULL_UP);
            break;
        }
        case PointerEvent::POINTER_ACTION_ENTER_WINDOW: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_PULL_IN_WINDOW);
            break;
        }
        case PointerEvent::POINTER_ACTION_LEAVE_WINDOW: {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW);
            break;
        }
        default: {
            MMI_HILOGI("Action is:%{public}d, no need change", action);
            break;
        }
    }
}

void InputWindowsManager::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    mprintf(fd, "Windows information:\t");
    mprintf(fd, "windowsInfos,num:%zu", displayGroupInfo_.windowsInfo.size());
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        mprintf(fd,
                "\t windowsInfos: id:%d | pid:%d | uid:%d | area.x:%d | area.y:%d "
                "| area.width:%d | area.height:%d | defaultHotAreas.size:%zu "
                "| pointerHotAreas.size:%zu | agentWindowId:%d | flags:%d "
                "| action:%d | displayId:%d | zOrder:%f \t",
                item.id, item.pid, item.uid, item.area.x, item.area.y, item.area.width,
                item.area.height, item.defaultHotAreas.size(), item.pointerHotAreas.size(),
                item.agentWindowId, item.flags, item.action, item.displayId, item.zOrder);
        for (const auto &win : item.defaultHotAreas) {
            mprintf(fd,
                    "\t defaultHotAreas: x:%d | y:%d | width:%d | height:%d \t",
                    win.x, win.y, win.width, win.height);
        }
        for (const auto &pointer : item.pointerHotAreas) {
            mprintf(fd,
                    "\t pointerHotAreas: x:%d | y:%d | width:%d | height:%d \t",
                    pointer.x, pointer.y, pointer.width, pointer.height);
        }
        
        std::string dump;
        dump += StringPrintf("\t pointerChangeAreas: ");
        for (auto it : item.pointerChangeAreas) {
            dump += StringPrintf("%d | ", it);
        }
        dump += StringPrintf("\n\t transform: ");
        for (auto it : item.transform) {
            dump += StringPrintf("%f | ", it);
        }
        std::istringstream stream(dump);
        std::string line;
        while (std::getline(stream, line, '\n')) {
            mprintf(fd, "%s", line.c_str());
        }
    }
    mprintf(fd, "Displays information:\t");
    mprintf(fd, "displayInfos,num:%zu", displayGroupInfo_.displaysInfo.size());
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        mprintf(fd,
                "\t displayInfos: id:%d | x:%d | y:%d | width:%d | height:%d | name:%s "
                "| uniq:%s | direction:%d | displayMode:%d \t",
                item.id, item.x, item.y, item.width, item.height, item.name.c_str(),
                item.uniq.c_str(), item.direction, item.displayMode);
    }
    mprintf(fd, "Input device and display bind info:\n%s\n", bindInfo_.Dumps().c_str());
}

std::pair<int32_t, int32_t> InputWindowsManager::TransformWindowXY(const WindowInfo &window,
    int32_t logicX, int32_t logicY) const
{
    Matrix3f transform(window.transform);
    if (transform.IsIdentity()) {
        return {logicX, logicY};
    }
    Vector3f logicXY(logicX, logicY, 1.0);
    Vector3f windowXY = transform * logicXY;
    return {(int)(round(windowXY[0])), (int)(round(windowXY[1]))};
}

bool InputWindowsManager::IsValidZorderWindow(const WindowInfo &window,
    const std::shared_ptr<PointerEvent>& pointerEvent)
{
    if (!(pointerEvent->HasFlag(InputEvent::EVENT_FLAG_SIMULATE)) || MMI_LE(pointerEvent->GetZOrder(), 0.0f)) {
        return true;
    }
    if (MMI_GE(window.zOrder, pointerEvent->GetZOrder())) {
        MMI_HILOGE("current window zorder:%{public}f greater than the simulate target zOrder:%{public}f, "
            "ignore this window::%{public}d", window.zOrder, pointerEvent->GetZOrder(), window.id);
        return false;
    }
    return true;
}
} // namespace MMI
} // namespace OHOS
