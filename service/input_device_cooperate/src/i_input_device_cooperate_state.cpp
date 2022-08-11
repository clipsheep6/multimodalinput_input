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

#include "define_multimodal.h"
#include "distributed_input_adapter.h"
#include "event_cooperate_manager.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"
#include "multimodal_input_connect_remoter.h"

#include "i_input_device_cooperate_state.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "IInputDeviceCooperateState"};
} // namespace

IInputDeviceCooperateState::IInputDeviceCooperateState()
{
    runner_ = AppExecFwk::EventRunner::Create(true);
    if (runner_ == nullptr) {
        return;
    }

    eventHandler_ = std::make_shared<CooperateEventHandler>(runner_);
    MMI_HILOGI("init success");
}

int32_t IInputDeviceCooperateState::StartInputDeviceCooperate(const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (networkId.empty()) {
        return RET_ERR;
    }
    return RET_OK;
}

int32_t IInputDeviceCooperateState::PrepareAndStart(const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    MMI_HILOGD("srcNetworkId: %{public}s, startInputDeviceId: %{public}d", srcNetworkId.c_str(), startInputDeviceId);
    auto sinkNetworkId = InputDevMgr->GetOrginNetworkId(startInputDeviceId);
    if (srcNetworkId.empty() || sinkNetworkId.empty() || srcNetworkId.compare(sinkNetworkId) == 0) {
        MMI_HILOGE("parameter error! srcNetworkId: %{public}s, sinkNetworkId: %{public}s", srcNetworkId.c_str(),
            sinkNetworkId.c_str());
        return RET_ERR;
    }
    RemoteMgr->StartRemoteCooperate(sinkNetworkId, srcNetworkId);
    int32_t ret = RET_ERR;
    if (NeedPrepare(srcNetworkId, sinkNetworkId)) {
        InputDevCooSM->UpdatePreparedDevices(srcNetworkId, sinkNetworkId);
        ret = DistributedAdapter->PrepareRemoteInput(
            srcNetworkId, sinkNetworkId, [this, srcNetworkId, startInputDeviceId](bool isSucess) {
                this->OnPrepareDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
            });
        if (ret != RET_OK) {
            InputDevCooSM->UpdatePreparedDevices("", "");
        }
    } else {
        RemoteMgr->StartRemoteCooperate(sinkNetworkId, srcNetworkId);
        ret = StartDistributedInput(startInputDeviceId);
    }
    if (ret != RET_OK) {
        InputDevCooSM->StartFinish(false, srcNetworkId, startInputDeviceId);
        return RET_ERR;
    }
    return RET_OK;
}

void IInputDeviceCooperateState::OnPrepareDistributedInput(
    bool isSucess, const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("isSucess: %{public}s", isSucess ? "true" : "false");
    if (!isSucess) {
        InputDevCooSM->UpdatePreparedDevices("", "");
        InputDevCooSM->StartFinish(false, networkId, startInputDeviceId);
        return;
    } else {
        std::string taskName = "start_dinput_task";
        std::function<void()> handleStartDinputFunc =
            std::bind(&IInputDeviceCooperateState::StartDistributedInput, this, startInputDeviceId);
        eventHandler_->PostTask(handleStartDinputFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    }
}

int32_t IInputDeviceCooperateState::StartDistributedInput(int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    auto networkIds = InputDevCooSM->GetPreparedDevices();
    auto uniqs = InputDevMgr->GetPointerKeyboardUnqs(startInputDeviceId);
    if (uniqs.empty()) {
        InputDevCooSM->StartFinish(false, networkIds.first, startInputDeviceId);
    }
    return DistributedAdapter->StartRemoteInput(
        networkIds.first, networkIds.second, uniqs, [this, src = networkIds.first, startInputDeviceId](bool isSucess) {
            this->OnStartDistributedInput(isSucess, src, startInputDeviceId);
        });
}

void IInputDeviceCooperateState::OnStartDistributedInput(
    bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::string taskName = "start_finish_task";
    std::function<void()> handleStartFinishFunc =
        std::bind(&InputDeviceCooperateSM::StartFinish, InputDevCooSM, isSucess, srcNetworkId, startInputDeviceId);
    eventHandler_->PostTask(handleStartFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void IInputDeviceCooperateState::OnStopDistributedInput(bool isSucess, const std::string &srcNetworkId) {}

bool IInputDeviceCooperateState::NeedPrepare(const std::string &srcNetworkId, const std::string &sinkNetworkId)
{
    CALL_DEBUG_ENTER;
    auto prepared = InputDevCooSM->GetPreparedDevices();
    MMI_HILOGD("preSrcId: %{public}s, preSinkId: %{public}s, srcId: %{public}s, sinkId: %{public}s",
        prepared.first.c_str(), prepared.second.c_str(), srcNetworkId.c_str(), sinkNetworkId.c_str());
    bool isNeed = !(srcNetworkId.compare(prepared.first) == 0 && sinkNetworkId.compare(prepared.second) == 0);
    MMI_HILOGD("NeedPrepare?: %{public}s", isNeed ? "true" : "false");
    return isNeed;
}

int32_t IInputDeviceCooperateState::StopInputDeviceCooperate()
{
    return RET_ERR;
}

int32_t IInputDeviceCooperateState::StopInputDeviceCooperate(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    RemoteMgr->StopRemoteCooperate(networkId);
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STOP);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS