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

#ifndef EVENT_COOPERATE_SERVICE_H
#define EVENT_COOPERATE_SERVICE_H

#include "nocopyable.h"

#include "event_cooperate_stub.h"
#include "cooperate_messages.h"

namespace OHOS {
namespace MMI {
class EventCooperateService final : public EventCooperateStub {
public:
    EventCooperateService();
    DISALLOW_COPY_AND_MOVE(EventCooperateService);
    ~EventCooperateService();

    virtual bool OnCooperateMessage(const std::string &deviceId, const CooperateMessages &msg) override;
    virtual bool OnCooperateState(bool state) override;
    int32_t SetCooperateMessageListener(std::function<void(std::string, CooperateMessages)> listener);
    int32_t SetCooperateStateCallback(std::function<void(bool)> callback);

private:
    std::function<void(std::string, CooperateMessages)> msgCooperateListener_ { nullptr };
    std::function<void(bool)> stateCooperateCallback_ { nullptr };
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_COOPERATE_SERVICE_H