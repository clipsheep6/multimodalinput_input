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

int32_t InputDeviceCooperateStateOut::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    std::string srcNetworkId = InputDevCooSM->GetSrcNetworkId();
    if (srcNetworkId.empty()) {
        std::pair<std::string, std::string> prepared = InputDevCooSM->GetPreparedDevices();
        srcNetworkId = prepared.first;
    }
    int32_t ret = IInputDeviceCooperateState::StopInputDeviceCooperate(srcNetworkId);
    if (ret != RET_OK) {
        MMI_HILOGE("Stop input device cooperate fail");
        return ret;
    }
    std::string taskName = "process_stop_task";
    std::function<void()> handleProcessStopFunc =
        std::bind(&InputDeviceCooperateStateOut::ProcessStop, this, srcNetworkId);
    CHKPR(eventHandler_, RET_ERR);
    eventHandler_->PostTask(handleProcessStopFunc, taskName, 0,
        AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

void InputDeviceCooperateStateOut::ProcessStop(const std::string& srcNetworkId)
{
    CALL_DEBUG_ENTER;
    std::string sink = InputDevMgr->GetOriginNetworkId(startDhid_);
    std::vector<std::string>  dhids = InputDevMgr->GetPointerKeyboardDhids(startDhid_);
    int32_t ret = DistributedAdapter->StopRemoteInput(srcNetworkId, sink, dhids, [this, srcNetworkId](bool isSuccess) {
        this->OnStopRemoteInput(isSuccess, srcNetworkId);
        });
    if (ret != RET_OK) {
        InputDevCooSM->OnStopFinish(false, srcNetworkId);
    }
}

void InputDeviceCooperateStateOut::OnStopRemoteInput(bool isSuccess, const std::string &srcNetworkId)
{
    CALL_DEBUG_ENTER;
    std::string taskName = "stop_finish_task";
    std::function<void()> handleStopFinishFunc =
        std::bind(&InputDeviceCooperateSM::StopFinish, InputDevCooSM, isSuccess, srcNetworkId);
    CHKPV(eventHandler_);
    eventHandler_->PostTask(handleStopFinishFunc, taskName, 0,
        AppExecFwk::EventQueue::Priority::HIGH);
}

void InputDeviceCooperateStateOut::OnKeyboardOnline(const std::string &dhid)
{
    std::pair<std::string, std::string> networkIds = InputDevCooSM->GetPreparedDevices();
    std::vector<std::string> dhids;
    dhids.push_back(dhid);
    DistributedAdapter->StartRemoteInput(networkIds.first, networkIds.second, dhids, [](bool isSuccess) {});
}
} // namespace MMI
} // namespace OHOS
