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

#ifndef EVENT_COOPERATE_WRAP_H
#define EVENT_COOPERATE_WRAP_H

#include <mutex>
#include <list>

#include "nocopyable.h"
#include "singleton.h"

#include "i_event_cooperate.h"
#include "cooperate_messages.h"

namespace OHOS {
namespace MMI {
class EventCooperateManager : public DelayedSingleton<EventCooperateManager> {
public:
    EventCooperateManager();
    DISALLOW_COPY_AND_MOVE(EventCooperateManager);
    ~EventCooperateManager();

    int32_t AddCooperateEvent(sptr<IEventCooperate> event);
    bool OnCooperateMessage(const CooperateMessages &msg, const std::string &deviceId = "");
    bool OnCooperateState(bool state);

private:
    std::mutex lockCooperate_;
    std::list<sptr<IEventCooperate>> cooperateEvents_;
};
} // namespace MMI
} // namespace OHOS
#define EventCooperateMgr EventCooperateManager::GetInstance()
#endif // EVENT_COOPERATE_WRAP_H