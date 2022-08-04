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

#ifndef EVENT_COOPERATE_PROXY_H
#define EVENT_COOPERATE_PROXY_H

#include "iremote_proxy.h"
#include "iremote_object.h"
#include "nocopyable.h"

#include "i_event_cooperate.h"

namespace OHOS {
namespace MMI {
class EventCooperateProxy final : public IRemoteProxy<IEventCooperate> {
public:
    explicit EventCooperateProxy(const sptr<IRemoteObject> &impl);
    DISALLOW_COPY_AND_MOVE(EventCooperateProxy);
    virtual ~EventCooperateProxy() override;
    virtual bool OnCooperateMessage(const std::string &deviceId, const CooperateMessages &msg) override;
    virtual bool OnCooperateState(bool state) override;

private:
    static inline BrokerDelegator<EventCooperateProxy> delegator_;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_COOPERATE_PROXY_H