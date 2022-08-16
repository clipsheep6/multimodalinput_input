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
#include "input_device_cooperate_sm.h"

#include <stdio.h>

#include "hitrace_meter.h"

#include "bytrace_adapter.h"
#include "cooperate_messages.h"
#include "define_multimodal.h"
#include "device_manager.h"
#include "device_profile_adapter.h"
#include "event_cooperate_manager.h"
#include "input_device_cooperate_state_free.h"
#include "input_device_cooperate_state_in.h"
#include "input_device_cooperate_state_out.h"
#include "input_device_manager.h"
#include "i_pointer_drawing_manager.h"
#include "mouse_event_handler.h"
#include "multimodal_input_connect_remoter.h"
#include "timer_manager.h"
#include "util_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateManager"};
const std::string BUNDLE_NAME = "ohos.multimodalinput.input";
constexpr int32_t INTERVAL_MS = 2000;
constexpr int32_t MOUSE_ABS_LOCATION_CONST = 100;
constexpr int32_t MOUSE_ABS_LOCATION_X = 50;
constexpr int32_t MOUSE_ABS_LOCATION_Y = 50;
} // namespace

void InputDeviceCooperateSM::Init()
{
    preparedNetworkId_ = std::make_pair("", "");
    Reset();
    TimerMgr->AddTimer(INTERVAL_MS, 1, [this]() {
        InputDevCooSM->InitDeviceManager();
    });
}

void InputDeviceCooperateSM::Reset()
{
    CALL_DEBUG_ENTER;
    startDhid_ = "";
    srcNetworkId_ = "";
    auto hasPointer = InputDevMgr->HasLocalPointerDevice();
    IPointerDrawingManager::GetInstance()->SetPointerVisible(getpid(), hasPointer);
    currentStateSM_ = std::make_shared<InputDeviceCooperateStateFree>();
    cooperateState_ = CooperateState::STATE_FREE;
}

void InputDeviceCooperateSM::OnCooperateChanged(const std::string &networkId, bool isOpen)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (!isOpen) {
        OnCloseCooperation(networkId, false);
    }
}

// DP 监听到 开关关闭
void InputDeviceCooperateSM::OnCloseCooperation(const std::string &networkId, bool isLocal)
{
    CALL_INFO_ENTER;
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STATE_OFF, networkId);
    if (!preparedNetworkId_.first.empty() && !preparedNetworkId_.second.empty()) {
        if (networkId ==preparedNetworkId_.first || networkId == preparedNetworkId_.second) {
            DistributedAdapter->UnPrepareRemoteInput(preparedNetworkId_.first, preparedNetworkId_.second,
                [](bool isSucess) {});
        }
    }
    UpdatePreparedDevices("", "");
    if (isLocal || networkId == srcNetworkId_) {
        if (InputDevMgr->HasLocalPointerDevice()) {
            MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_X, MOUSE_ABS_LOCATION_Y);
        }
        Reset();
        return;
    }
    auto originNetworkId = InputDevMgr->GetOriginNetworkId(startDhid_);
    if (originNetworkId == networkId) {
        Reset();
    }
}

int32_t InputDeviceCooperateSM::GetCooperateState(const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    bool state = DProfileAdapter->GetCrossingSwitchState(deviceId);
    EventCooperateMgr->OnCooperateState(state);
    return RET_OK;
}

int32_t InputDeviceCooperateSM::EnableInputDeviceCooperate(bool enabled)
{
    CALL_DEBUG_ENTER;
    DProfileAdapter->UpdateCrossingSwitchState(enabled, onLineDevice_);
    if (enabled) {
        BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::START_EVENT);
        EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_OPEN_SUCCESS);
        BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::START_EVENT);
    } else {
        std::string localNetworkId;
        InputDevMgr->GetLocalDeviceId(localNetworkId);
        OnCloseCooperation(localNetworkId, true);
        EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_CLOSE_SUCCESS);
    }
    return RET_OK;
}

int32_t InputDeviceCooperateSM::StartInputDeviceCooperate(const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (isStopping_ || isStarting_) {
        MMI_HILOGE("In transition state, not process");
        return RET_ERR;
    }
    CHKPR(currentStateSM_, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::LAUNCH_EVENT);
    auto state = cooperateState_;
    isStarting_ = true;
    UpdateMouseLocation();
    int32_t ret = currentStateSM_->StartInputDeviceCooperate(networkId, startInputDeviceId);
    if (state == CooperateState::STATE_FREE && ret == RET_OK) {
        srcNetworkId_ = networkId;
    }
    if (ret != RET_OK) {
        MMI_HILOGE("Start input device cooperate fail");
        isStarting_ = false;
    }
    return ret;
}

int32_t InputDeviceCooperateSM::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    if (isStopping_ || isStarting_) {
        MMI_HILOGE("In transition state, not process");
        return RET_ERR;
    }
    CHKPR(currentStateSM_, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::STOP_EVENT);
    isStopping_ = true;
    int32_t ret = currentStateSM_->StopInputDeviceCooperate();
    if (ret != RET_OK) {
        MMI_HILOGE("Stop input device cooperate fail");
        isStopping_ = false;
    }
    return ret;
}

int32_t InputDeviceCooperateSM::StartRemoteCooperate(const std::string &remoteDeviceId)
{
    CALL_DEBUG_ENTER;
    isStarting_ = true;
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_INFO_START, remoteDeviceId);
    return RET_OK;
}

int32_t InputDeviceCooperateSM::StartRemoteCooperateResult(bool isSucess,
    const std::string& startDhid, int32_t xPercent, int32_t yPercent)
{
    CALL_DEBUG_ENTER;
    CHKPR(currentStateSM_, ERROR_NULL_POINTER);
    startDhid_ = startDhid;
    CooperateMessages msg =
            isSucess ? CooperateMessages::MSG_COOPERATE_INFO_SUCCESS : CooperateMessages::MSG_COOPERATE_INFO_FAIL;
        EventCooperateMgr->OnCooperateMessage(msg);

    if (cooperateState_ == CooperateState::STATE_FREE && isSucess) {
        MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_CONST - xPercent, yPercent);
        InputDevCooSM->UpdateState(CooperateState::STATE_IN);
    } else if (cooperateState_ == CooperateState::STATE_OUT && isSucess) {
        MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_CONST - xPercent, yPercent);
        InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    } else {
        MMI_HILOGI("Current state is in");
    }
    isStarting_ = false;
    isStopping_ = false;
    return RET_OK;
}

int32_t InputDeviceCooperateSM::StopRemoteCooperate()
{
    CALL_DEBUG_ENTER;
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STOP);
    isStopping_ = true;
    return RET_OK;
}

int32_t InputDeviceCooperateSM::StopRemoteCooperateRes(bool isSucess)
{
    CALL_DEBUG_ENTER;
    CooperateMessages msg =
        isSucess ? CooperateMessages::MSG_COOPERATE_STOP_SUCCESS : CooperateMessages::MSG_COOPERATE_STOP_FAIL;
    EventCooperateMgr->OnCooperateMessage(msg);
    if (isSucess) {
        if (InputDevMgr->HasLocalPointerDevice()) {
            MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_X, MOUSE_ABS_LOCATION_Y);
        }
        InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    }
    isStopping_ = false;
    return RET_OK;
}

int32_t InputDeviceCooperateSM::StartCooperateOtherRes(const std::string& srcNetworkId)
{
    CALL_DEBUG_ENTER;
    srcNetworkId_ = srcNetworkId;
    return RET_OK;
}

void InputDeviceCooperateSM::StartFinish(bool isSucess, const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::LAUNCH_EVENT);
    if (!isSucess) {
        MMI_HILOGE("Start distributed fail, startInputDevice: %{public}d", startInputDeviceId);
        NotifyRemoteStartFail(networkId);
    } else {
        auto startDhid = InputDevMgr->GetDhid(startInputDeviceId);
        NotifyRemoteStartSucess(networkId, startDhid);
        startDhid_ = startDhid;
    }
    if (cooperateState_ == CooperateState::STATE_FREE && isSucess) {
        InputDevCooSM->UpdateState(CooperateState::STATE_OUT);
    } else if (cooperateState_ == CooperateState::STATE_IN && isSucess) {
        auto sink = InputDevMgr->GetOriginNetworkId(startInputDeviceId);
        if (!sink.empty() && networkId != sink) {
            RemoteMgr->StartCooperateOtherRes(sink, networkId);
        }
        InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    } else {
        MMI_HILOGI("Current state is free");
    }
    isStarting_ = false;
    isStopping_ = false;
}

void InputDeviceCooperateSM::StopFinish(bool isSucess, const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::STOP_EVENT);
    NotifyRemoteStopFinish(isSucess, networkId);
    if (isSucess) {
        if (InputDevMgr->HasLocalPointerDevice()) {
            MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_X, MOUSE_ABS_LOCATION_Y);
        }
    }
    
    if (cooperateState_ == CooperateState::STATE_IN && isSucess) {
        InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    } else if (cooperateState_ == CooperateState::STATE_OUT && isSucess) {
        InputDevCooSM->UpdateState(CooperateState::STATE_FREE);
    } else {
        MMI_HILOGI("Current state is free");
    }
    isStarting_ = false;
    isStopping_ = false;
}

void InputDeviceCooperateSM::NotifyRemoteStartFail(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    RemoteMgr->StartRemoteCooperateResult(networkId, false, "",  0, 0);
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_INFO_FAIL);
}

void InputDeviceCooperateSM::NotifyRemoteStartSucess(const std::string &networkId, const std::string& startDhid)
{
    CALL_DEBUG_ENTER;
    RemoteMgr->StartRemoteCooperateResult(networkId, true, startDhid, mouseLocation_.first, mouseLocation_.second);
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_INFO_SUCCESS);
}

void InputDeviceCooperateSM::NotifyRemoteStopFinish(bool isSucess, const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    RemoteMgr->StopRemoteCooperateRes(networkId, isSucess);
    if (!isSucess) {
        EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STOP_FAIL);
    } else {
        EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STOP_SUCCESS);
    }
}

bool InputDeviceCooperateSM::UpdateMouseLocation()
{
    CALL_DEBUG_ENTER;
    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, false);
    auto displayId = pointerEvent->GetTargetDisplayId();
    auto physicalDisplayInfo = WinMgr->GetPhysicalDisplay(displayId);
    CHKPR(physicalDisplayInfo, false);
    auto displayWidth = physicalDisplayInfo->width;
    auto displayHeight = physicalDisplayInfo->height;
    if (displayWidth == 0 || displayHeight == 0) {
        return false;
    }
    auto mouseInfo = WinMgr->GetMouseInfo();
    int32_t xPercent = mouseInfo.physicalX * MOUSE_ABS_LOCATION_CONST / displayWidth;
    int32_t yPercent = mouseInfo.physicalY * MOUSE_ABS_LOCATION_CONST / displayHeight;
    MMI_HILOGI("displayWidth: %{public}d, displayHeight: %{public}d, physicalX: %{public}d, physicalY: %{public}d,",
        displayWidth, displayHeight, mouseInfo.physicalX, mouseInfo.physicalY);
    mouseLocation_ = std::make_pair(xPercent, yPercent);
    return true;
}

void InputDeviceCooperateSM::UpdateState(CooperateState state)
{
    MMI_HILOGI("state: %{public}d", state);
    switch (state) {
        case CooperateState::STATE_FREE: {
            Reset();
            break;
        }
        case CooperateState::STATE_IN: {
            currentStateSM_ = std::make_shared<InputDeviceCooperateStateIn>(startDhid_);
            break;
        }
        case CooperateState::STATE_OUT: {
            IPointerDrawingManager::GetInstance()->SetPointerVisible(getpid(), false);
            currentStateSM_ = std::make_shared<InputDeviceCooperateStateOut>(startDhid_);
            break;
        }
        default:
            break;
    }
    cooperateState_ = state;
}

std::shared_ptr<IInputDeviceCooperateState> InputDeviceCooperateSM::GetCurrentState() const
{
    return currentStateSM_;
}

CooperateState InputDeviceCooperateSM::GetCurrentCooperateState() const
{
    return cooperateState_;
}

void InputDeviceCooperateSM::UpdatePreparedDevices(const std::string &srcNetworkId, const std::string &sinkNetworkId)
{
    CALL_DEBUG_ENTER;
    preparedNetworkId_ = std::make_pair(srcNetworkId, sinkNetworkId);
}

std::pair<std::string, std::string> InputDeviceCooperateSM::GetPreparedDevices() const
{
    CALL_DEBUG_ENTER;
    return preparedNetworkId_;
}

const std::string& InputDeviceCooperateSM::GetSrcNetworkId() const
{
    return srcNetworkId_;
}

void InputDeviceCooperateSM::OnKeyboardOnline(const std::string &dhid)
{
    CHKPV(currentStateSM_);
    currentStateSM_->OnKeyboardOnline(dhid);
}

void InputDeviceCooperateSM::OnPointerOffline(const std::string &dhid, const std::string sinkNetworkId,
    const std::vector<std::string> &keyboards)
{
    CALL_DEBUG_ENTER;
    if (cooperateState_ == CooperateState::STATE_FREE) {
        return;
    }
    if (startDhid_ == dhid) {
        if (cooperateState_ == CooperateState::STATE_OUT) {
            auto src = srcNetworkId_;
            if (src.empty()) {
                auto prepared = InputDevCooSM->GetPreparedDevices();
                src = prepared.first;
            }
            DistributedAdapter->StopRemoteInput(src, sinkNetworkId, keyboards, [this, src](bool isSucess) {});
        }
        startDhid_ = "";
        srcNetworkId_ = "";
        currentStateSM_ = std::make_shared<InputDeviceCooperateStateFree>();
        cooperateState_ = CooperateState::STATE_FREE;
    }
}

void InputDeviceCooperateSM::HandleLibinputEvent(libinput_event *event)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("current state :%{public}d", cooperateState_);
    CHKPV(event);
    auto type = libinput_event_get_type(event);
    switch (type) {
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
        case LIBINPUT_EVENT_POINTER_BUTTON:
        case LIBINPUT_EVENT_POINTER_AXIS: {
            CheckPointerEvent(event);
            break;
        }
        default: {
            MMI_HILOGW("This device does not support");
            CHKPV(nextHandler_);
            nextHandler_->HandleLibinputEvent(event);
            break;
        }
    }
}

void InputDeviceCooperateSM::CheckPointerEvent(struct libinput_event *event)
{
    if (isStopping_ || isStarting_) {
        MMI_HILOGE("In transition state, not process");
        return;
    }
    auto inputDevice = libinput_event_get_device(event);
    if (cooperateState_ == CooperateState::STATE_IN) {
        if (!InputDevMgr->IsRemote(inputDevice)) {
            StopInputDeviceCooperate();
            return;
        }
    } else if (cooperateState_ == CooperateState::STATE_OUT) {
        int32_t deviceId = InputDevMgr->FindInputDeviceId(inputDevice);
        auto dhid = InputDevMgr->GetDhid(deviceId);
        if (startDhid_ != dhid) {
            MMI_HILOGI("Move other mouse, stop input device cooperate");
            StopInputDeviceCooperate();
        }
        return;
    } else {
        if (InputDevMgr->IsRemote(inputDevice)) {
            return;
        }
    }
    CHKPV(nextHandler_);
    nextHandler_->HandleLibinputEvent(event);
}

bool InputDeviceCooperateSM::IsStarting() const
{
    return isStarting_;
}

bool InputDeviceCooperateSM::IsStopping() const
{
    return isStopping_;
}

bool InputDeviceCooperateSM::InitDeviceManager()
{
    CALL_DEBUG_ENTER;
    initCallback_ = std::make_shared<DeviceInitCallBack>();
    CHKPR(initCallback_, false);
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(BUNDLE_NAME, initCallback_);
    if (ret != 0) {
        MMI_HILOGE("Init device manager failed, ret:%{public}d", ret);
        return false;
    }
    stateCallback_ = std::make_shared<MmiDeviceStateCallback>();
    ret =
        DistributedHardware::DeviceManager::GetInstance().RegisterDevStateCallback(BUNDLE_NAME, "", stateCallback_);
    if (ret != 0) {
        MMI_HILOGE("Register devStateCallback failed, ret:%{public}d", ret);
        return false;
    }
    return true;
}

void InputDeviceCooperateSM::OnDeviceOnLine(const std::string& networkId)
{
    CALL_INFO_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    onLineDevice_.push_back(networkId);
    DProfileAdapter->RegisterCrossingStateListener(networkId,
        std::bind(&InputDeviceCooperateSM::OnCooperateChanged,
        InputDevCooSM, std::placeholders::_1, std::placeholders::_2));
}

void InputDeviceCooperateSM::OnDeviceOffline(const std::string& networkId)
{
    CALL_INFO_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    bool needReset = true;
    if (cooperateState_ == CooperateState::STATE_OUT) {
        if (networkId != srcNetworkId_) {
            needReset = false;
            MMI_HILOGE("OnDeviceOffline: needReset false");
        }
    }
    if (cooperateState_ == CooperateState::STATE_IN) {
        auto sinkNetwoekId = InputDevMgr->GetOriginNetworkId(startDhid_);
        if (networkId != sinkNetwoekId) {
             needReset = false;
             MMI_HILOGE("OnDeviceOffline: needReset false");
        }
    }
    if (needReset) {
        MMI_HILOGE("OnDeviceOffline: needReset true");
        UpdatePreparedDevices("", "");
        if (InputDevMgr->HasLocalPointerDevice()) {
            MouseEventHdr->SetAbsolutionLocation(MOUSE_ABS_LOCATION_X, MOUSE_ABS_LOCATION_Y);
        }
        Reset();
    }
    if (!onLineDevice_.empty()) {
        auto it = std::find(onLineDevice_.begin(), onLineDevice_.end(), networkId);
        if (it != onLineDevice_.end()) {
            onLineDevice_.erase(it);
        }
    }
}

void InputDeviceCooperateSM::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    mprintf(fd, "Keyboard and mouse crossing information:");
    mprintf(fd, "State machine status: %d\t", cooperateState_);
    mprintf(fd, "Peripheral keyboard and mouse information: startDhid_  srcNetworkId_:\t");
    mprintf(fd, "%s", startDhid_.c_str());
    mprintf(fd, "%s", srcNetworkId_.c_str());
    mprintf(fd, "Run successfully");
}

void InputDeviceCooperateSM::DeviceInitCallBack::OnRemoteDied()
{
    CALL_DEBUG_ENTER;
}

void InputDeviceCooperateSM::MmiDeviceStateCallback::OnDeviceOnline(
    const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_DEBUG_ENTER;
    InputDevCooSM->OnDeviceOnLine(deviceInfo.deviceId);
}

void InputDeviceCooperateSM::MmiDeviceStateCallback::OnDeviceOffline(
    const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_DEBUG_ENTER;
    InputDevCooSM->OnDeviceOffline(deviceInfo.deviceId);
}

void InputDeviceCooperateSM::MmiDeviceStateCallback::OnDeviceChanged(
    const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_DEBUG_ENTER;
}

void InputDeviceCooperateSM::MmiDeviceStateCallback::OnDeviceReady(
    const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_DEBUG_ENTER;
}
} // namespace MMI
} // namespace OHOS
