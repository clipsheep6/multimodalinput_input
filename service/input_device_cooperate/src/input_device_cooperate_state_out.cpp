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

#include "input_device_cooperate_state_out.h"

#include "cooperate_messages.h"
#include "define_multimodal.h"
#include "distributed_input_adapter.h"
#include "event_cooperate_manager.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateOut"};

} // namespace

InputDeviceCooperateStateOut::InputDeviceCooperateStateOut(const std::string& startDhid)
    : startDhid_(startDhid)
{
}

int32_t InputDeviceCooperateStateOut::StartInputDeviceCooperate(const std::string &networkId, int32_t sharedInputDevice)
{
    CALL_DEBUG_ENTER;
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_INFO_FAIL);
    return RET_ERR;
}

int32_t InputDeviceCooperateStateOut::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    auto src = InputDevCooSM->GetSrcNetworkId();
    if (src.empty()) {
        auto prepared = InputDevCooSM->GetPreparedDevices();
        src = prepared.first;
    }
    int32_t ret = IInputDeviceCooperateState::StopInputDeviceCooperate(src);
    if (ret != RET_OK) {
        return ret;
    }
    std::string taskName = "process_stop_task";
    std::function<void()> handleProcessStopFunc =
        std::bind(&InputDeviceCooperateStateOut::ProcessStop, this, src);
    eventHandler_->PostTask(handleProcessStopFunc, taskName, 0,
        AppExecFwk::EventQueue::Priority::HIGH);
    return ret;
}

int32_t InputDeviceCooperateStateOut::ProcessStop(const std::string& src)
{
    CALL_DEBUG_ENTER;
    auto sink = InputDevMgr->GetOrginNetworkId(startDhid_);
    auto unqs = InputDevMgr->GetPointerKeyboardUnqs(startDhid_);
    MMI_HILOGI("src: %{public}s, sink: %{public}s", src.c_str(), sink.c_str());
    int32_t ret = DistributedAdapter->StopRemoteInput(src, sink, unqs, [this, src](bool isSucess) {
        this->OnStopRemoteInput(isSucess, src);
        });
    if (ret != RET_OK) {
       InputDevCooSM->StopFinish(false, src);
    }
    return ret;
}

void InputDeviceCooperateStateOut::OnStopRemoteInput(bool isSucess, const std::string &srcNetworkId)
{
    CALL_DEBUG_ENTER;
    std::string taskName = "stop_finish_task";
    std::function<void()> handleStopFinishFunc =
    std::bind(&InputDeviceCooperateSM::StopFinish, InputDevCooSM, isSucess, srcNetworkId);
    eventHandler_->PostTask(handleStopFinishFunc, taskName, 0,
        AppExecFwk::EventQueue::Priority::HIGH);
}

void InputDeviceCooperateStateOut::OnKeyboardOnline(const std::string &unq)
{
    auto networkIds = InputDevCooSM->GetPreparedDevices();
    std::vector<std::string> unqs;
    unqs.push_back(unq);
    DistributedAdapter->StartRemoteInput(networkIds.first, networkIds.second, unqs, [](bool isSucess) {});
}
} // namespace MMI
} // namespace OHOS