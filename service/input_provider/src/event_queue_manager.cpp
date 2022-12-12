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

#include "event_queue_manager.h"

#include <cstddef>
#include <memory>

#include "define_multimodal.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventQueueManager" };
} // namespace

int32_t EventQueueManager::AddQueue(const std::shared_ptr<IEventQueue> eventQueue)
{
    CALL_DEBUG_ENTER;
    CHKPR(eventQueue, ERROR_NULL_POINTER);
    auto iter = std::find(eventQueues_.begin(), eventQueues_.end(), eventQueue);
    if (iter != eventQueues_.end()) {
        return RET_ERR;
    }
    eventQueues_.push_back(eventQueue);
    return RET_OK;
}

int32_t EventQueueManager::RemoveQueue(int32_t id)
{
    CALL_DEBUG_ENTER;
    std::vector<std::shared_ptr<IEventQueue>>::iterator iter;
    for (iter = eventQueues_.begin(); iter != eventQueues_.end(); iter++) {
        if((*iter)->GetId() == id) {
            eventQueues_.erase(iter);
            return RET_OK;
        }
    }
    return RET_ERR;
}

std::shared_ptr<IEventQueue> EventQueueManager::GetDefaultQueue(int32_t id)
{
    for (const auto &item : eventQueues_) {
        if (item->GetId() == id) {
          return item;
        }
    }
    return nullptr;
}
} // namespace MMI
} // namespace OHOS