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

namespace OHOS {
namespace MMI {
class IEventQueue {
struct EventData {
    int32_t size;
    int handlerId;
    unsigned char *eventData;
};
public:
    explicit IEventQueue() {}
    virtual ~IEventQueue() = default;
    virtual int32_t RegisterEventHandler(std::function<void(int32_t, void *, size_t)>) = 0;
    virtual void UnregisterEventHandler(int32_t handlerId) = 0;
    virtual int32_t SendEvent(const EventData &event) = 0;
    virtual int32_t SetDefaultHandler(std::function<void(int32_t, void *, size_t)>) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_CONTEXT