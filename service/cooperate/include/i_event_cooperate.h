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

#ifndef I_EVENT_COOPERATE_H
#define I_EVENT_COOPERATE_H

#include "iremote_broker.h"

#include "cooperate_messages.h"

namespace OHOS {
namespace MMI {
class IEventCooperate : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.multimodalinput.IEventCooperate");
    virtual bool OnCooperateMessage(const std::string &deviceId, const CooperateMessages &msg) = 0;
    virtual bool OnCooperateState(bool state) = 0;
    enum {
        ON_COOPERATE_MESSAGE_EVENT = 0,
        ON_COOPERATE_STATE_EVENT = 1,
    };
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_COOPERATE_H