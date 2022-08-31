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
#ifndef EVENT_LOG_HELPER_H
#define EVENT_LOG_HELPER_H

#include <memory>

#include "input_event.h"
#include "key_event.h"
#include "mmi_log.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
namespace EventLogHelper {
void Print(const std::shared_ptr<KeyEvent> event);
void Print(const std::shared_ptr<PointerEvent> event);
template <class Event>
void PrintEventData(std::shared_ptr<Event> event, int32_t actionType, int32_t itemNum);
template <class Event>
void PrintEventData(std::shared_ptr<Event> event);
} // namespace EventLogHelper

template <class Event>
void EventLogHelper::PrintEventData(std::shared_ptr<Event> event, int32_t actionType, int32_t itemNum)
{
    constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventLogHelper"};
    if (HiLogIsLoggable(OHOS::MMI::MMI_LOG_DOMAIN, LABEL.tag, LOG_DEBUG)) {
        static int64_t nowTimeUSec = 0;
        static int32_t dropped = 0;
        if (event->GetAction() == EVENT_TYPE_POINTER) {
            if ((actionType == POINTER_ACTION_MOVE) && (event->GetActionTime() - nowTimeUSec <= TIMEOUT)) {
                ++dropped;
                return;
            }
            if (actionType == POINTER_ACTION_UP && itemNum == FINAL_FINGER) {
                MMI_HILOGD("This touch process discards %{public}d high frequent events", dropped);
                dropped = 0;
            }
            nowTimeUSec = event->GetActionTime();
        }

        EventLogHelper::Print(event);
    }
}

template<class Event>
void EventLogHelper::PrintEventData(std::shared_ptr<Event> event)
{
    constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventLogHelper"};
    if (HiLogIsLoggable(OHOS::MMI::MMI_LOG_DOMAIN, LABEL.tag, LOG_DEBUG)) {
         EventLogHelper::Print(event);
    }
}

} // namespace MMI
} // namespace OHOS
#endif // EVENT_LOG_HELPER_H