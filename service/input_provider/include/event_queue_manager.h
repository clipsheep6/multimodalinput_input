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

#include "i_event_queue_manager.h"

namespace OHOS {
namespace MMI {
class EventQueueManager : public IEventQueueManager {
public:
    EventQueueManager();
    virtual ~EventQueueManager();
    DISALLOW_COPY_AND_MOVE(EventQueueManager);
    virtual int32_t AddQueue() override;
    virtual void int32_t RemoveQueue(int32_t id) override;
    virtual IEventQueue GetDefaultQueue(int32_t id) override;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_QUEUE_MANAGER