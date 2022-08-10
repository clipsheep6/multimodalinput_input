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

#include "input_device_cooperate_state_free.h"

#include "define_multimodal.h"
#include "event_cooperate_manager.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateFree"};
} // namespace

int32_t InputDeviceCooperateStateFree::StartInputDeviceCooperate(
    const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    int32_t ret = IInputDeviceCooperateState::StartInputDeviceCooperate(networkId, startInputDeviceId);
    if (ret != RET_OK) {
        return ret;
    }
    std::string taskName = "process_start_task";
    std::function<void()> handleProcessStartFunc =
        std::bind(&InputDeviceCooperateStateFree::ProcessStart, this, networkId, startInputDeviceId);
    eventHandler_->PostTask(handleProcessStartFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

int32_t InputDeviceCooperateStateFree::ProcessStart(const std::string &networkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    return PrepareAndStart(networkId, startInputDeviceId);
}

int32_t InputDeviceCooperateStateFree::StopInputDeviceCooperate()
{
    CALL_DEBUG_ENTER;
    EventCooperateMgr->OnCooperateMessage(CooperateMessages::MSG_COOPERATE_STOP_FAIL);
    return RET_ERR;
}

void InputDeviceCooperateStateFree::OnStartDistributedInput(
    bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    IInputDeviceCooperateState::OnStartDistributedInput(isSucess, srcNetworkId, startInputDeviceId);
}
} // namespace MMI
} // namespace OHOS