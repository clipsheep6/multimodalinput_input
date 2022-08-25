/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "key_event_input_subscribe_manager.h"

#include <cinttypes>

#include "define_multimodal.h"
#include "error_multimodal.h"

#include "bytrace_adapter.h"
#include "input_manager_impl.h"
#include "mmi_event_handler.h"
#include "multimodal_event_handler.h"
#include "standardized_event_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyEventInputSubscribeManager" };
constexpr int32_t INVALID_SUBSCRIBE_ID = -1;
constexpr size_t PRE_KEYS_NUM = 4;
} // namespace
int32_t KeyEventInputSubscribeManager::subscribeIdManager_ = 0;

KeyEventInputSubscribeManager::SubscribeKeyEventInfo::SubscribeKeyEventInfo(
    std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
    : keyOption_(keyOption), callback_(callback)
{
    if (KeyEventInputSubscribeManager::subscribeIdManager_ >= INT_MAX) {
        subscribeId_ = -1;
        MMI_HILOGE("The subscribeId has reached the upper limit, cannot continue the subscription");
        return;
    }
    subscribeId_ = KeyEventInputSubscribeManager::subscribeIdManager_;
    ++KeyEventInputSubscribeManager::subscribeIdManager_;
}

int32_t KeyEventInputSubscribeManager::SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
{
    CALL_INFO_TRACE;
    CHKPR(keyOption, INVALID_SUBSCRIBE_ID);
    CHKPR(callback, INVALID_SUBSCRIBE_ID);
    std::set<int32_t> preKeys = keyOption->GetPreKeys();
    if (preKeys.size() > PRE_KEYS_NUM) {
        MMI_HILOGE("PreKeys number invalid");
        return INVALID_SUBSCRIBE_ID;
    }

    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("Client init failed");
        return INVALID_SUBSCRIBE_ID;
    }
    for (const auto &preKey : preKeys) {
        MMI_HILOGD("prekey:%{public}d", preKey);
    }
    SubscribeKeyEventInfo subscribeInfo(keyOption, callback);
    MMI_HILOGD("subscribeId:%{public}d,keyOption->finalKey:%{public}d,"
        "keyOption->isFinalKeyDown:%{public}s,keyOption->finalKeyDownDuration:%{public}d",
        subscribeInfo.GetSubscribeId(), keyOption->GetFinalKey(), keyOption->IsFinalKeyDown() ? "true" : "false",
        keyOption->GetFinalKeyDownDuration());
    subscribeInfos_.push_back(subscribeInfo);
    if (EventManager.SubscribeKeyEvent(subscribeInfo) != RET_OK) {
        MMI_HILOGE("Leave, subscribe key event failed");
        return INVALID_SUBSCRIBE_ID;
    }
    return subscribeInfo.GetSubscribeId();
}

int32_t KeyEventInputSubscribeManager::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_INFO_TRACE;
    if (subscribeId < 0) {
        MMI_HILOGE("The subscribe id is less than 0");
        return RET_ERR;
    }

    if (!MMIEventHdl.InitClient()) {
        MMI_HILOGE("Client init failed");
        return INVALID_SUBSCRIBE_ID;
    }
    if (subscribeInfos_.empty()) {
        MMI_HILOGE("The subscribeInfos is empty");
        return RET_ERR;
    }

    for (auto it = subscribeInfos_.begin(); it != subscribeInfos_.end(); ++it) {
        if (it->GetSubscribeId() == subscribeId) {
            if (EventManager.UnsubscribeKeyEvent(subscribeId) != RET_OK) {
                MMI_HILOGE("Leave, unsubscribe key event failed");
                return RET_ERR;
            }
            subscribeInfos_.erase(it);
            return RET_OK;
        }
    }
    return RET_ERR;
}

int32_t KeyEventInputSubscribeManager::OnSubscribeKeyEventCallback(std::shared_ptr<KeyEvent> event,
    int32_t subscribeId)
{
    CHK_PID_AND_TID();
    CHKPR(event, ERROR_NULL_POINTER);
    if (subscribeId < 0) {
        MMI_HILOGE("Leave, the subscribe id is less than 0");
        return RET_ERR;
    }

    std::lock_guard<std::mutex> guard(mtx_);
    BytraceAdapter::StartBytrace(event, BytraceAdapter::TRACE_STOP, BytraceAdapter::KEY_SUBSCRIBE_EVENT);
    auto info = GetSubscribeKeyEvent(subscribeId);
    CHKPR(info, ERROR_NULL_POINTER);
    info->GetCallback()(event);
    MMI_HILOGD("Key event id:%{public}d keyCode:%{public}d", subscribeId, event->GetKeyCode());
    return RET_OK;
}

void KeyEventInputSubscribeManager::OnConnected()
{
    CALL_DEBUG_ENTER;
    if (subscribeInfos_.empty()) {
        MMI_HILOGE("Leave, subscribeInfos_ is empty");
        return;
    }
    for (const auto& subscriberInfo : subscribeInfos_) {
        if (EventManager.SubscribeKeyEvent(subscriberInfo) != RET_OK) {
            MMI_HILOGE("Subscribe key event failed");
        }
    }
}

const KeyEventInputSubscribeManager::SubscribeKeyEventInfo* KeyEventInputSubscribeManager::GetSubscribeKeyEvent(
    int32_t id)
{
    if (id < 0) {
        MMI_HILOGE("Invalid input param id:%{public}d", id);
        return nullptr;
    }
    for (const auto& subscriber : subscribeInfos_) {
        if (subscriber.GetSubscribeId() == id) {
            return &subscriber;
        }
    }
    return nullptr;
}
} // namespace MMI
} // namespace OHOS