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

#include "cooperation_message.h"
#include "distributed_input_adapter.h"
#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_normalize.h"
#include "multimodal_input_connect_remoter.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateOut"};
} // namespace

InputDeviceCooperateStateOut::InputDeviceCooperateStateOut(const std::string& startDhid)
    : startDhid_(startDhid)
{
}

int32_t InputDeviceCooperateStateOut::StopInputDeviceCooperate(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    std::string srcNetworkId = networkId;
    if (srcNetworkId.empty()) {
        std::pair<std::string, std::string> prepared = InputDevCooSM->GetPreparedDevices();
        srcNetworkId = prepared.first;
    }
    int32_t ret = RemoteMgr->StopRemoteCooperate(networkId);
    if (ret != RET_OK) {
        MMI_HILOGE("Stop input device cooperate fail");
        return ret;
    }
    return ProcessStop(srcNetworkId);
}

int32_t InputDeviceCooperateStateOut::ProcessStop(const std::string& srcNetworkId)
{
    CALL_DEBUG_ENTER;
    std::string sink = InputDevMgr->GetOriginNetworkId(startDhid_);
    std::vector<std::string>  dhids = InputDevMgr->GetCooperateDhids(startDhid_);
    int32_t ret = DistributedAdapter->StopRemoteInput(srcNetworkId, sink, dhids, [this, srcNetworkId](bool isSuccess) {
        this->OnStopRemoteInput(isSuccess, srcNetworkId);
        });
    if (ret != RET_OK) {
        InputDevCooSM->OnStopFinish(false, srcNetworkId);
    }
    return ret;
}

void InputDeviceCooperateStateOut::OnStopRemoteInput(bool isSuccess, const std::string &srcNetworkId)
{
    CALL_DEBUG_ENTER;
    InputDevCooSM->OnStopFinish(isSuccess, srcNetworkId);
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
