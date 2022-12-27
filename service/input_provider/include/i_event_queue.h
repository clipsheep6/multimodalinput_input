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

#ifndef I_EVENT_QUEUE
#define I_EVENT_QUEUE

#include <functional>
#include <memory>
#include <string>

#include "i_event_handler.h"
#include "i_event_queue.h"

namespace OHOS {
namespace MMI {

struct EventData {
    int32_t size;
    int32_t type;
    std::shared_ptr<IEventHandler> handler;
    struct input_event data;
};

class IEventQueue {
public:
    explicit IEventQueue() : id_(QueueIdSeed_++) {}
    virtual ~IEventQueue() = default;
    virtual int32_t RegisterEventHandler(std::shared_ptr<IEventHandler> handler) = 0;
    virtual int32_t UnregisterEventHandler() = 0;
    virtual int32_t SendEvent(EventData &event) = 0;
    virtual int32_t SetDefaultHandler(std::function<void(int32_t, void *, size_t)>) = 0;
    virtual int32_t GetId() = 0;
    int32_t GetQueueId() const { return id_; }
protected:
    const int32_t id_;
private:
    static inline int32_t QueueIdSeed_ = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_CONTEXT