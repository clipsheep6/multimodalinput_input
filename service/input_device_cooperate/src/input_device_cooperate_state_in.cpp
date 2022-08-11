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

int32_t InputDeviceCooperateStateIn::StartInputDeviceCooperate(const std::string &remote, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    int32_t ret = IInputDeviceCooperateState::StartInputDeviceCooperate(remote, startInputDeviceId);
    if (ret != RET_OK) {
        return ret;
    }
    std::string taskName = "process_start_task";
    std::function<void()> handleProcessStartFunc =
        std::bind(&InputDeviceCooperateStateIn::ProcessStart, this, remote, startInputDeviceId);
    eventHandler_->PostTask(handleProcessStartFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

int32_t InputDeviceCooperateStateIn::ProcessStart(const std::string &remote, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    auto orginNetworkId = InputDevMgr->GetOrginNetworkId(startInputDeviceId);
    // 归属networkId 和 开始的一致，是穿越返回.不一致是 接力穿越或接力返回
    if (remote.compare(orginNetworkId) == 0) {
        ComeBack(remote, startInputDeviceId);
        return RET_OK;
    } else {
        return RelayOrRelayComeBack(remote, startInputDeviceId);
    }
}

int32_t InputDeviceCooperateStateIn::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    auto sink = InputDevMgr->GetOrginNetworkId(startDhid_);
    int32_t ret = IInputDeviceCooperateState::StopInputDeviceCooperate(sink);
    if (ret != RET_OK) {
        return ret;
    }
    std::string taskName = "process_stop_task";
    std::function<void()> handleProcessStopFunc = std::bind(&InputDeviceCooperateStateIn::ProcessStop, this);
    eventHandler_->PostTask(handleProcessStopFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return ret;
}

int32_t InputDeviceCooperateStateIn::ProcessStop()
{
    CALL_DEBUG_ENTER;
    auto unqs = InputDevMgr->GetPointerKeyboardUnqs(startDhid_);
    auto sink = InputDevMgr->GetOrginNetworkId(startDhid_);
    int32_t ret = DistributedAdapter->StopRemoteInput(
        sink, unqs, [this, sink](bool isSucess) { this->OnStopDistributedInput(isSucess, sink); });
    if (ret != RET_OK) {
        InputDevCooSM->StopFinish(false, sink);
    }
    return ret;
}

void InputDeviceCooperateStateIn::OnStartDistributedInput(
    bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (!isSucess) {
        IInputDeviceCooperateState::OnStartDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
        return;
    }
    auto sinkNetworkId = InputDevMgr->GetOrginNetworkId(startInputDeviceId);
    auto unq = InputDevMgr->GetPointerKeyboardUnqs(startInputDeviceId);

    std::string taskName = "relay_stop_task";
    std::function<void()> handleRelayStopFunc =
        std::bind(&InputDeviceCooperateStateIn::StopRemoteInput, this, sinkNetworkId, srcNetworkId, unq);
    eventHandler_->PostTask(handleRelayStopFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void InputDeviceCooperateStateIn::StopRemoteInput(
    const std::string &sinkNetworkId, const std::string &srcNetworkId, const std::vector<std::string> &unq)
{
    int32_t ret = DistributedAdapter->StopRemoteInput(sinkNetworkId, unq,
        [this, srcNetworkId](bool isSucess) { this->OnStopDistributedInput(isSucess, srcNetworkId); });
    if (ret != RET_OK) {
        InputDevCooSM->StartFinish(false, sinkNetworkId, -1);
    }
}

void InputDeviceCooperateStateIn::OnStopDistributedInput(bool isSucess, const std::string &remote)
{
    CALL_DEBUG_ENTER;
    if (InputDevCooSM->IsStarting()) {
        std::string taskName = "start_finish_task";
        std::function<void()> handleStartFinishFunc =
            std::bind(&InputDeviceCooperateSM::StartFinish, InputDevCooSM, isSucess, remote, -1);
        eventHandler_->PostTask(handleStartFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    } else if (InputDevCooSM->IsStopping()) {
        std::string taskName = "stop_finish_task";
        std::function<void()> handleStopFinishFunc =
            std::bind(&InputDeviceCooperateSM::StopFinish, InputDevCooSM, isSucess, remote);
        eventHandler_->PostTask(handleStopFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    }
}

void InputDeviceCooperateStateIn::ComeBack(const std::string &sinkNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::string localNetworkId;
    InputDevMgr->GetLocalDeviceId(localNetworkId);
    RemoteMgr->StartRemoteCooperate(localNetworkId, sinkNetworkId);
    auto unqs = InputDevMgr->GetPointerKeyboardUnqs(startInputDeviceId);
    int32_t ret = DistributedAdapter->StopRemoteInput(sinkNetworkId, unqs,
        [this, sinkNetworkId](bool isSucess) { this->OnStopDistributedInput(isSucess, sinkNetworkId); });
    if (ret != RET_OK) {
        InputDevCooSM->StartFinish(false, sinkNetworkId, -1);
    }
}

int32_t InputDeviceCooperateStateIn::RelayOrRelayComeBack(const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    return PrepareAndStart(srcNetworkId, startInputDeviceId);
}
} // namespace MMI
} // namespace OHOS