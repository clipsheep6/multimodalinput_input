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
#include "input_device_cooperate_manager.h"

#include "cooperation_message.h"
#include "cooperate_event_manager.h"
#include "define_multimodal.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateManager"};

} // namespace
InputDeviceCooperateManager::InputDeviceCooperateManager(){}

InputDeviceCooperateManager::~InputDeviceCooperateManager(){}

int32_t InputDeviceCooperateManager::StartInputDeviceCooperate(const std::string &remoteNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if(InputDevMgr->IsPointerDevice(startInputDeviceId)) {
        return InputDeviceCooperateSM::StartInputDeviceCooperate(remoteNetworkId, startInputDeviceId);
    }
    std::string sinkNetworkId = InputDevMgr->GetOriginNetworkId(startInputDeviceId);
    if (remoteNetworkId == preparedNetworkId_.first && sinkNetworkId == preparedNetworkId_.second) {
        return StartRemoteInput(startInputDeviceId);
    }
    UpdatePreparedDevices(remoteNetworkId, sinkNetworkId);
    int32_t ret = DistributedAdapter->PrepareRemoteInput(
        remoteNetworkId, sinkNetworkId, [this, startInputDeviceId](bool isSuccess) {
            this->OnPrepareDistributedInput(isSuccess, startInputDeviceId);
        });
    if (ret != RET_OK) {
        MMI_HILOGE("Prepare remote input fail");
        UpdatePreparedDevices("", "");
    }
    return ret;
}


void InputDeviceCooperateManager::OnPrepareDistributedInput(bool isSuccess, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (!isSuccess) {
        UpdatePreparedDevices("", "");
    } else {
        StartRemoteInput(startInputDeviceId);
    }
}

int32_t InputDeviceCooperateManager::StartRemoteInput(int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> dhids = InputDevMgr->GetCooperateDhids(startInputDeviceId);
    if (dhids.empty()) {
        CooperateEventMgr->OnStart(CooperationMessage::INFO_FAIL);
    }
    return DistributedAdapter->StartRemoteInput(
        preparedNetworkId_.first, preparedNetworkId_.second, dhids, [](bool isSuccess) {
            CooperationMessage msg = isSuccess ? CooperationMessage::INFO_SUCCESS : CooperationMessage::INFO_FAIL;
            CooperateEventMgr->OnStart(msg);
        });
}

int32_t InputDeviceCooperateManager::StopInputDeviceCooperate(int32_t stopInputDeviceId)
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> dhids = InputDevMgr->GetCooperateDhids(stopInputDeviceId);
    if (dhids.empty()) {
        return InputDeviceCooperateSM::StopInputDeviceCooperate();
    }
    return DistributedAdapter->StopRemoteInput(
    preparedNetworkId_.first, preparedNetworkId_.second, dhids, [](bool isSuccess) {
        CooperationMessage msg = isSuccess ? CooperationMessage::STOP_SUCCESS : CooperationMessage::STOP_FAIL;
        CooperateEventMgr->OnStop(msg);
    });
}
} // namespace MMI
} // namespace OHOS
