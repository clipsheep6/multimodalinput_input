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

#include "input_device_cooperate_state_in.h"

#include "cooperate_messages.h"
#include "define_multimodal.h"
#include "distributed_input_adapter.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"
#include "multimodal_input_connect_remoter.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateIn"};
} // namespace

InputDeviceCooperateStateIn::InputDeviceCooperateStateIn(const std::string &startDhid) : startDhid_(startDhid) {}

int32_t InputDeviceCooperateStateIn::StartInputDeviceCooperate(const std::string &remoteNetworkId,
    int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    int32_t ret = IInputDeviceCooperateState::StartInputDeviceCooperate(remoteNetworkId, startInputDeviceId);
    if (ret != RET_OK) {
        return ret;
    }
    std::string taskName = "process_start_task";
    std::function<void()> handleProcessStartFunc =
        std::bind(&InputDeviceCooperateStateIn::ProcessStart, this, remoteNetworkId, startInputDeviceId);
    CHKPR(eventHandler_, RET_ERR);
    eventHandler_->PostTask(handleProcessStartFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

int32_t InputDeviceCooperateStateIn::ProcessStart(const std::string &remoteNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::string originNetworkId = InputDevMgr->GetOriginNetworkId(startInputDeviceId);
    if (remoteNetworkId == originNetworkId) {
        ComeBack(remoteNetworkId, startInputDeviceId);
        return RET_OK;
    } else {
        return RelayOrRelayComeBack(remoteNetworkId, startInputDeviceId);
    }
}

int32_t InputDeviceCooperateStateIn::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    std::string sink = InputDevMgr->GetOriginNetworkId(startDhid_);
    int32_t ret = IInputDeviceCooperateState::StopInputDeviceCooperate(sink);
    if (ret != RET_OK) {
        MMI_HILOGE("Stop input device cooperate fail");
        return ret;
    }
    std::string taskName = "process_stop_task";
    std::function<void()> handleProcessStopFunc = std::bind(&InputDeviceCooperateStateIn::ProcessStop, this);
    CHKPR(eventHandler_, RET_ERR);
    eventHandler_->PostTask(handleProcessStopFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

int32_t InputDeviceCooperateStateIn::ProcessStop()
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> dhids = InputDevMgr->GetPointerKeyboardDhids(startDhid_);
    std::string sink = InputDevMgr->GetOriginNetworkId(startDhid_);
    int32_t ret = DistributedAdapter->StopRemoteInput(
        sink, dhids, [this, sink](bool isSucess) { this->OnStopDistributedInput(isSucess, sink, -1); });
    if (ret != RET_OK) {
        InputDevCooSM->StopFinish(false, sink);
    }
    return RET_OK;
}

void InputDeviceCooperateStateIn::OnStartDistributedInput(
    bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (!isSucess) {
        IInputDeviceCooperateState::OnStartDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
        return;
    }
    std::string sinkNetworkId = InputDevMgr->GetOriginNetworkId(startInputDeviceId);
    std::vector<std::string> dhid = InputDevMgr->GetPointerKeyboardDhids(startInputDeviceId);

    std::string taskName = "relay_stop_task";
    std::function<void()> handleRelayStopFunc = std::bind(&InputDeviceCooperateStateIn::StopRemoteInput,
        this, sinkNetworkId, srcNetworkId, dhid, startInputDeviceId);
    CHKPV(eventHandler_);
    eventHandler_->PostTask(handleRelayStopFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void InputDeviceCooperateStateIn::StopRemoteInput(const std::string &sinkNetworkId,
    const std::string &srcNetworkId, const std::vector<std::string> &dhid, int32_t startInputDeviceId)
{
    int32_t ret = DistributedAdapter->StopRemoteInput(sinkNetworkId, dhid,
        [this, srcNetworkId, startInputDeviceId](bool isSucess) {
            this->OnStopDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
    });
    if (ret != RET_OK) {
        InputDevCooSM->StartFinish(false, sinkNetworkId, startInputDeviceId);
    }
}

void InputDeviceCooperateStateIn::OnStopDistributedInput(bool isSucess,
    const std::string &remoteNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (InputDevCooSM->IsStarting()) {
        std::string taskName = "start_finish_task";
        std::function<void()> handleStartFinishFunc = std::bind(&InputDeviceCooperateSM::StartFinish,
            InputDevCooSM, isSucess, remoteNetworkId, startInputDeviceId);
        CHKPV(eventHandler_);
        eventHandler_->PostTask(handleStartFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    } else if (InputDevCooSM->IsStopping()) {
        std::string taskName = "stop_finish_task";
        std::function<void()> handleStopFinishFunc =
            std::bind(&InputDeviceCooperateSM::StopFinish, InputDevCooSM, isSucess, remoteNetworkId);
        CHKPV(eventHandler_);
        eventHandler_->PostTask(handleStopFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    }
}

void InputDeviceCooperateStateIn::ComeBack(const std::string &sinkNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> dhids = InputDevMgr->GetPointerKeyboardDhids(startInputDeviceId);
    int32_t ret = DistributedAdapter->StopRemoteInput(sinkNetworkId, dhids,
        [this, sinkNetworkId, startInputDeviceId](bool isSucess) {
            this->OnStopDistributedInput(isSucess, sinkNetworkId, startInputDeviceId);
            });
    if (ret != RET_OK) {
        InputDevCooSM->StartFinish(false, sinkNetworkId, startInputDeviceId);
    }
}

int32_t InputDeviceCooperateStateIn::RelayOrRelayComeBack(const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    return PrepareAndStart(srcNetworkId, startInputDeviceId);
}
} // namespace MMI
} // namespace OHOS
