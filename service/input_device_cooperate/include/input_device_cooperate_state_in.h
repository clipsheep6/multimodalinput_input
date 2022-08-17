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

#ifndef INPUT_DEVICE_COOPERATE_STATE_IN_H
#define INPUT_DEVICE_COOPERATE_STATE_IN_H

#include "i_input_device_cooperate_state.h"

namespace OHOS {
namespace MMI {
class InputDeviceCooperateStateIn : public IInputDeviceCooperateState {
public:
    explicit InputDeviceCooperateStateIn(const std::string &startDhid);
    virtual int32_t StartInputDeviceCooperate(const std::string &networkId, int32_t startInputDeviceId) override;
    virtual int32_t StopInputDeviceCooperate() override;

private:
    void ComeBack(const std::string &sinkNetworkId, int32_t startInputDeviceId);
    int32_t RelayOrRelayComeBack(const std::string &srcNetworkId, int32_t startInputDeviceId);
    void OnStartDistributedInput(bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId) override;
    void StopRemoteInput(const std::string &sinkNetworkId, const std::string &srcNetworkId,
        const std::vector<std::string> &dhid, int32_t startInputDeviceId);
    void OnStopDistributedInput(bool isSucess, const std::string &srcNetworkId, int32_t startInputDeviceId) override;
    int32_t ProcessStart(const std::string &remote, int32_t startInputDeviceId);
    int32_t ProcessStop();

private:
    std::string startDhid_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_COOPERATE_STATE_IN_H
