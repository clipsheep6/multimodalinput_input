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

#ifndef EVENT_QUEUE_MANAGER
#define EVENT_QUEUE_MANAGER

#include <memory>
#include <map>
#include <vector>

#include "nocopyable.h"

#include "i_event_queue_manager.h"

namespace OHOS {
namespace MMI {
class EventQueueManager : public IEventQueueManager {
public:
    EventQueueManager() = default;
    virtual ~EventQueueManager() = default;
    DISALLOW_COPY_AND_MOVE(EventQueueManager);
    virtual int32_t AddQueue(std::shared_ptr<IEventQueue>) override;
    virtual int32_t RemoveQueue(int32_t id) override;
    virtual std::shared_ptr<IEventQueue> GetDefaultQueue(int32_t id) override;
private:
    std::vector<std::shared_ptr<IEventQueue>> eventQueues_;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_QUEUE_MANAGER