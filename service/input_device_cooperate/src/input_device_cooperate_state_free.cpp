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

#include "input_device_cooperate_sm.h"
#include "input_device_manager.h"
#include "mouse_event_handler.h"
#include "multimodal_input_connect_remoter.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateStateFree"};
} // namespace

int32_t InputDeviceCooperateStateFree::StartInputDeviceCooperate(
    const std::string &remoteNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    if (remoteNetworkId.empty()) {
        MMI_HILOGE("RemoteNetworkId is empty");
        return RET_ERR;
    }
    std::string localNetworkId;
    InputDevMgr->GetLocalDeviceId(localNetworkId);
    if (localNetworkId.empty() || remoteNetworkId == localNetworkId) {
        MMI_HILOGE("Input Parameters error");
        return RET_ERR;
    }
    int32_t ret = RemoteMgr->StartRemoteCooperate(localNetworkId, remoteNetworkId);
    if (ret != RET_OK) {
        MMI_HILOGE("Start input device cooperate fail");
        return ret;
    }
    std::string taskName = "process_start_task";
    std::function<void()> handleProcessStartFunc =
        std::bind(&InputDeviceCooperateStateFree::ProcessStart, this, remoteNetworkId, startInputDeviceId);
    CHKPR(eventHandler_, RET_ERR);
    eventHandler_->PostTask(handleProcessStartFunc, taskName, 0, AppExecFwk::EventQueue::Priority::HIGH);
    return RET_OK;
}

int32_t InputDeviceCooperateStateFree::ProcessStart(const std::string &remoteNetworkId, int32_t startInputDeviceId)
{
    CALL_DEBUG_ENTER;
    return PrepareAndStart(remoteNetworkId, startInputDeviceId);
}
} // namespace MMI
} // namespace OHOS
