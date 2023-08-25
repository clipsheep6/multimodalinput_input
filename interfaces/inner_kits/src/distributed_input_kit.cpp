/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "distributed_input_kit.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
int32_t DistributedInputKit::PrepareRemoteInput(
    const std::string &sinkId, sptr<IPrepareDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().PrepareRemoteInput(sinkId, callback);
}

int32_t DistributedInputKit::UnprepareRemoteInput(
    const std::string &sinkId, sptr<IUnprepareDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().UnprepareRemoteInput(sinkId, callback);
}

int32_t DistributedInputKit::StartRemoteInput(
    const std::string &sinkId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().StartRemoteInput(sinkId, inputTypes, callback);
}

int32_t DistributedInputKit::StopRemoteInput(
    const std::string &sinkId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().StopRemoteInput(sinkId, inputTypes, callback);
}

int32_t DistributedInputKit::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().StartRemoteInput(srcId, sinkId, inputTypes, callback);
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().StopRemoteInput(srcId, sinkId, inputTypes, callback);
}

bool DistributedInputKit::IsNeedFilterOut(const std::string &sinkId, const BusinessEvent &event)
{
    return DistributedInputClient::GetInstance().IsNeedFilterOut(sinkId, event);
}

bool DistributedInputKit::IsTouchEventNeedFilterOut(const TouchScreenEvent &event)
{
    return DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
}

DInputServerType DistributedInputKit::IsStartDistributedInput(const uint32_t &inputType)
{
    return DInputServerType::NULL_SERVER_TYPE;
}

bool DistributedInputKit::IsStartDistributedInput(const std::string &dhId)
{
    return DistributedInputClient::GetInstance().IsStartDistributedInput(dhId);
}

int32_t DistributedInputKit::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().PrepareRemoteInput(srcId, sinkId, callback);
}

int32_t DistributedInputKit::UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    return DistributedInputClient::GetInstance().UnprepareRemoteInput(srcId, sinkId, callback);
}

int32_t DistributedInputKit::StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    return DistributedInputClient::GetInstance().StartRemoteInput(sinkId, dhIds, callback);
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    return DistributedInputClient::GetInstance().StopRemoteInput(sinkId, dhIds, callback);
}

int32_t DistributedInputKit::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DistributedInputClient::GetInstance().StartRemoteInput(srcId, sinkId, dhIds, callback);
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DistributedInputClient::GetInstance().StopRemoteInput(srcId, sinkId, dhIds, callback);
}

int32_t DistributedInputKit::RegisterInputNodeListener(sptr<InputNodeListener> listener)
{
    return DistributedInputClient::GetInstance().RegisterInputNodeListener(listener);
}

int32_t DistributedInputKit::UnregisterInputNodeListener(sptr<InputNodeListener> listener)
{
    return DistributedInputClient::GetInstance().UnregisterInputNodeListener(listener);
}

int32_t DistributedInputKit::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    return DistributedInputClient::GetInstance().RegisterSimulationEventListener(listener);
}

int32_t DistributedInputKit::UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    return DistributedInputClient::GetInstance().UnregisterSimulationEventListener(listener);
}

int32_t DistributedInputKit::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    return DistributedInputClient::GetInstance().RegisterSessionStateCb(callback);
}
int32_t DistributedInputKit::UnregisterSessionStateCb()
{
    return DistributedInputClient::GetInstance().UnregisterSessionStateCb();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
