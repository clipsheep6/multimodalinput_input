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

#include "event_cooperate_manager.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventCooperateManager" };
} // namespace

EventCooperateManager::EventCooperateManager()
{
    CALL_DEBUG_ENTER;
}

EventCooperateManager::~EventCooperateManager()
{
    CALL_DEBUG_ENTER;
}

int32_t EventCooperateManager::AddCooperateEvent(sptr<IEventCooperate> event)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lockCooperate_);
    cooperateEvents_.push_back(event);
    return RET_OK;
}

void EventCooperateManager::OnCooperateMessage(const CooperateMessages &msg, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lockCooperate_);
    MMI_HILOGI("cooperateEvents_ size:%{public}zu", cooperateEvents_.size());
    for (const auto &event : cooperateEvents_) {
        if (event != nullptr) {
            event->OnCooperateMessage(deviceId, msg);
        }
    }
}

void EventCooperateManager::OnCooperateState(bool state)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lockCooperate_);
    MMI_HILOGI("cooperateEvents_ size:%{public}zu", cooperateEvents_.size());
    for (const auto &event : cooperateEvents_) {
        if (event != nullptr) {
            event->OnCooperateState(state);
        }
    }
}
} // namespace MMI
} // namespace OHOS
