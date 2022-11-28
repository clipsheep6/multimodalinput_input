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

#ifndef EVENT_QUEUE
#define EVENT_QUEUE

#include <memory>
#include <map>
#include "i_event_queue.h"

namespace OHOS {
namespace MMI {
class EventQueue : public IEventQueue {
public:
    EventQueue(int32_t devIndex);
    virtual ~EventQueue();
    DISALLOW_COPY_AND_MOVE(EventQueue);
    virtual int32_t RegisterEventHandler(std::function<void(int32_t, void *, size_t)>) override;
    virtual void UnregisterEventHandler(int32_t handlerId) override;
    virtual int32_t SendEvent(EventData event) override;
    virtual int32_t SetDefaultHandler(std::function<void(int32_t, void *, size_t)>) override;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_QUEUE