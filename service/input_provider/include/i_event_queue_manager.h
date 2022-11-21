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

#ifndef I_EVENT_QUEUE_MANAGER
#define I_EVENT_QUEUE_MANAGER

#include <memory>

#include "i_event_queue.h"

namespace OHOS {
namespace MMI {
class IEventQueueManager {
public:
    virtual int32_t AddQueue() = 0;
    virtual void RemoveQueue(int32_t id) = 0;
    virtual std::shared_ptr<IEventQueue> GetDefaultQueue(int32_t id) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_QUEUE_MANAGER