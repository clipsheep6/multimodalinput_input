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

#include "i_input_device_cooperate_state.h"

#include "cooperate_event_manager.h"
#include "define_multimodal.h"
#include "distributed_input_adapter.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"
#include "multimodal_input_connect_remoter.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "IInputDeviceCooperateState"};
} // namespace

IInputDeviceCooperateState::IInputDeviceCooperateState()
    :runner_(AppExecFwk::EventRunner::Create(true))
{
    if (runner_ == nullptr) {
        MMI_HILOGE("Create event runner fail");
        return;
    }
    eventHandler_ = std::make_shared<CooperateEventHandler>(runner_);
}

int32_t IInputDeviceCooperateState::StartInputDeviceCooperate(const std::string &remoteNetworkId,
    int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (remoteNetworkId.empty()) {
        MMI_HILOGE("Parameter error");
        return RET_ERR;
    }
    std::string localNetworkId;
    InputDevMgr->GetLocalDeviceId(localNetworkId);
    if (remoteNetworkId.empty() || localNetworkId.empty() || remoteNetworkId == localNetworkId) {
        MMI_HILOGE("Parameter error");
        return RET_ERR;
    }
    return RemoteMgr->StartRemoteCooperate(localNetworkId, remoteNetworkId);
}

int32_t IInputDeviceCooperateState::PrepareAndStart(const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::string sinkNetworkId = InputDevMgr->GetOriginNetworkId(startInputDeviceId);
    int32_t ret = RET_ERR;
    if (NeedPrepare(srcNetworkId, sinkNetworkId)) {
        InputDevCooSM->UpdatePreparedDevices(srcNetworkId, sinkNetworkId);
        ret = DistributedAdapter->PrepareRemoteInput(
            srcNetworkId, sinkNetworkId, [this, srcNetworkId, startInputDeviceId](bool isSucess) {
                this->OnPrepareDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
            });
        if (ret != RET_OK) {
            MMI_HILOGE("Prepare remoteNetworkId input fail");
            InputDevCooSM->StartFinish(false, sinkNetworkId, startInputDeviceId);
            InputDevCooSM->UpdatePreparedDevices("", "");
        }
    } else {
        ret = StartDistributedInput(startInputDeviceId);
        if (ret != RET_OK) {
            MMI_HILOGE("Start remoteNetworkId input fail");
            InputDevCooSM->StartFinish(false, sinkNetworkId, startInputDeviceId);
        }
    }
    return ret;
}

void IInputDeviceCooperateState::OnPrepareDistributedInput(
    bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    MMI_HILOGI("isSucess: %{public}s", isSucess ? "true" : "false");
    if (!isSucess) {
        InputDevCooSM->UpdatePreparedDevices("", "");
        InputDevCooSM->StartFinish(false, srcNetworkId, startInputDeviceId);
        return;
    } else {
        std::string taskName = "start_dinput_task";
        std::function<void()> handleStartDinputFunc =
            std::bind(&IInputDeviceCooperateState::StartDistributedInput, this, startInputDeviceId);
        CHKPV(eventHandler_);
        eventHandler_->PostTask(handleStartDinputFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    }
}

int32_t IInputDeviceCooperateState::StartDistributedInput(int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::pair<std::string, std::string> networkIds = InputDevCooSM->GetPreparedDevices();
    std::vector<std::string> dhids = InputDevMgr->GetPointerKeyboardDhids(startInputDeviceId);
    if (dhids.empty()) {
        InputDevCooSM->StartFinish(false, networkIds.first, startInputDeviceId);
        return RET_OK;
    }
    return DistributedAdapter->StartRemoteInput(
        networkIds.first, networkIds.second, dhids, [this, src = networkIds.first, startInputDeviceId](bool isSucess) {
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
    CHKPV(eventHandler_);
    eventHandler_->PostTask(handleStartFinishFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void IInputDeviceCooperateState::OnStopDistributedInput(bool isSucess,
    const std::string &srcNetworkId, int32_t startInputDeviceId) {}

bool IInputDeviceCooperateState::NeedPrepare(const std::string &srcNetworkId, const std::string &sinkNetworkId)
{
    CALL_DEBUG_ENTER;
    std::pair<std::string, std::string> prepared = InputDevCooSM->GetPreparedDevices();
    bool isNeed =  !(srcNetworkId == prepared.first && sinkNetworkId == prepared.second);
    MMI_HILOGI("NeedPrepare?: %{public}s", isNeed ? "true" : "false");
    return isNeed;
}

int32_t IInputDeviceCooperateState::StopInputDeviceCooperate()
{
    return RET_ERR;
}

int32_t IInputDeviceCooperateState::StopInputDeviceCooperate(const std::string &remoteNetworkId)
{
    CALL_DEBUG_ENTER;
    return RemoteMgr->StopRemoteCooperate(remoteNetworkId);
}
} // namespace MMI
} // namespace OHOS