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

#ifndef I_SIMULATION_EVENT_LISTENER_H
#define I_SIMULATION_EVENT_LISTENER_H

#include <string>

#include <iremote_broker.h>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class ISimulationEventListener : public IRemoteBroker {
public:
    virtual int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value) = 0;

    enum class Message {
        RESULT_ON,
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.DistributedHardware.DistributedInput.ISimulationEventListener");
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // I_SIMULATION_EVENT_LISTENER_H