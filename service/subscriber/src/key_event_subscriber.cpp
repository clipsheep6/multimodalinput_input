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

#include "key_event_subscriber.h"

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_event_data_transformation.h"
#include "input_event_handler.h"
#include "net_packet.h"
#include "proto.h"
#include "timer_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "KeyEventSubscriber"};
constexpr uint32_t MAX_PRE_KEY_COUNT = 4;
} // namespace

int32_t KeyEventSubscriber::SubscribeKeyEvent(
        SessionPtr sess, int32_t subscribeId, std::shared_ptr<KeyOption> keyOption)
{
    CALL_LOG_ENTER;
    if (subscribeId < 0) {
        MMI_HILOGE("Invalid subscribe");
        return RET_ERR;
    }
    CHKPR(sess, ERROR_NULL_POINTER);
    CHKPR(keyOption, ERROR_NULL_POINTER);
    uint32_t preKeySize = keyOption->GetPreKeys().size();
    if (preKeySize > MAX_PRE_KEY_COUNT) {
        MMI_HILOGE("Leave, preKeySize:%{public}u", preKeySize);
        return RET_ERR;
    }

    for (const auto &keyCode : keyOption->GetPreKeys()) {
        MMI_HILOGD("keyOption->prekey:%{public}d", keyCode);
    }
    MMI_HILOGD("subscribeId:%{public}d,keyOption->finalKey:%{public}d,"
        "keyOption->isFinalKeyDown:%{public}s,keyOption->finalKeyDownDuriation:%{public}d",
        subscribeId, keyOption->GetFinalKey(), keyOption->IsFinalKeyDown() ? "true" : "false",
        keyOption->GetFinalKeyDownDuration());
    auto subscriber = std::make_shared<Subscriber>(subscribeId, sess, keyOption);
    subscribers_.push_back(subscriber);
    InitSessionDeleteCallback();
    return RET_OK;
}

int32_t KeyEventSubscriber::UnSubscribeKeyEvent(SessionPtr sess, int32_t subscribeId)
{
    CALL_LOG_ENTER;
    MMI_HILOGD("subscribeId:%{public}d", subscribeId);
    for (auto it = subscribers_.begin(); it != subscribers_.end(); ++it) {
        if ((*it)->id_ == subscribeId && (*it)->sess_ == sess) {
            ClearTimer(*it);
            subscribers_.erase(it);
            return RET_OK;
        }
    }
    return RET_ERR;
}

bool KeyEventSubscriber::SubscribeKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    int32_t keyAction = keyEvent->GetKeyAction();
    MMI_HILOGD("keyCode:%{public}d,keyAction:%{public}s", keyEvent->GetKeyCode(), KeyEvent::ActionToString(keyAction));
    for (const auto &keyCode : keyEvent->GetPressedKeys()) {
        MMI_HILOGD("pressed KeyCode:%{public}d", keyCode);
    }
    bool handled = false;
    if (keyAction == KeyEvent::KEY_ACTION_DOWN) {
        handled = HandleKeyDown(keyEvent);
    } else if (keyAction == KeyEvent::KEY_ACTION_UP) {
        handled = HandleKeyUp(keyEvent);
    } else if (keyAction == KeyEvent::KEY_ACTION_CANCEL) {
        handled = HandleKeyCanel(keyEvent);
    } else {
        MMI_HILOGW("keyAction exception");
    }
    keyEvent_.reset();
    return handled;
}

void KeyEventSubscriber::OnSessionDelete(SessionPtr sess)
{
    CALL_LOG_ENTER;
    for (auto it = subscribers_.begin(); it != subscribers_.end();) {
        if ((*it)->sess_ == sess) {
            ClearTimer(*it);
            subscribers_.erase(it++);
            continue;
        }
        ++it;
    }
}

bool KeyEventSubscriber::IsPreKeysMatch(const std::set<int32_t>& preKeys,
        const std::vector<int32_t>& pressedKeys) const
{
    if (preKeys.size() != pressedKeys.size()) {
        return false;
    }

    for (const auto &pressedKey : pressedKeys) {
        auto it = std::find(preKeys.begin(), preKeys.end(), pressedKey);
        if (it == preKeys.end()) {
            return false;
        }
    }

    return true;
}

void KeyEventSubscriber::NotifySubscriber(std::shared_ptr<KeyEvent> keyEvent,
        const std::shared_ptr<Subscriber>& subscriber)
{
    CALL_LOG_ENTER;
    CHKPV(keyEvent);
    CHKPV(subscriber);
    auto udsServerPtr = InputHandler->GetUDSServer();
    CHKPV(udsServerPtr);
    NetPacket pkt(MmiMessageId::ON_SUBSCRIBE_KEY);
    InputEventDataTransformation::KeyEventToNetPacket(keyEvent, pkt);
    int32_t fd = subscriber->sess_->GetFd();
    pkt << fd << subscriber->id_;
    if (!udsServerPtr->SendMsg(fd, pkt)) {
        MMI_HILOGE("Leave, server disaptch subscriber failed");
        return;
    }
}

bool KeyEventSubscriber::AddTimer(const std::shared_ptr<Subscriber>& subscriber,
        const std::shared_ptr<KeyEvent>& keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    CHKPF(subscriber);

    if (subscriber->timerId_ >= 0) {
        MMI_HILOGW("Leave, timer already added, it may have been added by injection");
        return true;
    }

    auto& keyOption = subscriber->keyOption_;
    if (keyOption->GetFinalKeyDownDuration() <= 0) {
        MMI_HILOGE("Leave, duration <= 0");
        return true;
    }

    if (!CloneKeyEvent(keyEvent)) {
        MMI_HILOGE("Leave, cloneKeyEvent failed");
        return false;
    }

    std::weak_ptr<Subscriber> weakSubscriber = subscriber;
    subscriber->timerId_ = TimerMgr->AddTimer(keyOption->GetFinalKeyDownDuration(), 1, [this, weakSubscriber] () {
        MMI_HILOGD("timer callback");
        auto subscriber = weakSubscriber.lock();
        CHKPV(subscriber);
        OnTimer(subscriber);
    });

    if (subscriber->timerId_ < 0) {
        MMI_HILOGE("Leave, addTimer failed");
        return false;
    }
    subscriber->keyEvent_ = keyEvent_;
    MMI_HILOGD("leave, add timer success, subscribeId:%{public}d,"
        "duration:%{public}d,timerId:%{public}d",
        subscriber->id_, keyOption->GetFinalKeyDownDuration(), subscriber->timerId_);
    return true;
}

void KeyEventSubscriber::ClearTimer(const std::shared_ptr<Subscriber>& subscriber)
{
    CALL_LOG_ENTER;
    CHKPV(subscriber);

    if (subscriber->timerId_ < 0) {
        MMI_HILOGE("Leave, subscribeId:%{public}d,null timerId < 0", subscriber->id_);
        return;
    }

    auto timerId = subscriber->timerId_;
    subscriber->keyEvent_.reset();
    subscriber->timerId_ = -1;
    TimerMgr->RemoveTimer(timerId);
    MMI_HILOGD("subscribeId:%{public}d,subscribeId:%{public}d", subscriber->id_, timerId);
}

void KeyEventSubscriber::OnTimer(const std::shared_ptr<Subscriber> subscriber)
{
    CALL_LOG_ENTER;
    CHKPV(subscriber);
    subscriber->timerId_ = -1;
    if (subscriber->keyEvent_ == nullptr) {
        MMI_HILOGE("Leave, subscriber->keyEvent is nullptr, subscribeId:%{public}d", subscriber->id_);
        return;
    }

    NotifySubscriber(subscriber->keyEvent_, subscriber);
    subscriber->keyEvent_.reset();
    MMI_HILOGD("subscribeId:%{public}d", subscriber->id_);
}

bool KeyEventSubscriber::InitSessionDeleteCallback()
{
    CALL_LOG_ENTER;
    if (callbackInitialized_)  {
        MMI_HILOGD("session delete callback has already been initialized");
        return true;
    }
    auto udsServerPtr = InputHandler->GetUDSServer();
    CHKPF(udsServerPtr);
    std::function<void(SessionPtr)> callback = std::bind(&KeyEventSubscriber::OnSessionDelete,
            this, std::placeholders::_1);
    udsServerPtr->AddSessionDeletedCallback(callback);

    callbackInitialized_ = true;
    return true;
}

bool KeyEventSubscriber::HandleKeyDown(const std::shared_ptr<KeyEvent>& keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    bool handled = false;
    auto keyCode = keyEvent->GetKeyCode();
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    RemoveKeyCode(keyCode, pressedKeys);
    for (const auto &subscriber : subscribers_) {
        auto& keyOption = subscriber->keyOption_;
        MMI_HILOGD("subscribeId:%{public}d,keyOption->finalKey:%{public}d,"
            "keyOption->isFinalKeyDown:%{public}s,keyOption->finalKeyDownDuriation:%{public}d",
            subscriber->id_, keyOption->GetFinalKey(), keyOption->IsFinalKeyDown() ? "true" : "false",
            keyOption->GetFinalKeyDownDuration());
        for (const auto &keyCode : keyOption->GetPreKeys()) {
            MMI_HILOGD("keyOption->prekey:%{public}d", keyCode);
        }

        if (!keyOption->IsFinalKeyDown()) {
            MMI_HILOGD("!keyOption->IsFinalKeyDown()");
            continue;
        }

        if (keyCode != keyOption->GetFinalKey()) {
            ClearTimer(subscriber);
            MMI_HILOGD("keyCode != keyOption->GetFinalKey()");
            continue;
        }

        if (!IsPreKeysMatch(keyOption->GetPreKeys(), pressedKeys)) {
            ClearTimer(subscriber);
            MMI_HILOGD("preKeysMatch failed");
            continue;
        }

        if (keyOption->GetFinalKeyDownDuration() <= 0) {
            MMI_HILOGD("keyOption->GetFinalKeyDownDuration() <= 0");
            NotifySubscriber(keyEvent, subscriber);
            handled = true;
            continue;
        }

        if (!AddTimer(subscriber, keyEvent)) {
            MMI_HILOGE("Leave, add timer failed");
        }
    }

    MMI_HILOGD("%{public}s", handled ? "true" : "false");
    return handled;
}

bool KeyEventSubscriber::HandleKeyUp(const std::shared_ptr<KeyEvent>& keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    bool handled = false;
    auto keyCode = keyEvent->GetKeyCode();
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    RemoveKeyCode(keyCode, pressedKeys);
    for (const auto &subscriber : subscribers_) {
        auto& keyOption = subscriber->keyOption_;
        MMI_HILOGD("subscribeId:%{public}d,keyOption->finalKey:%{public}d,"
            "keyOption->isFinalKeyDown:%{public}s,keyOption->finalKeyDownDuriation:%{public}d",
            subscriber->id_, keyOption->GetFinalKey(), keyOption->IsFinalKeyDown() ? "true" : "false",
            keyOption->GetFinalKeyDownDuration());
        for (auto keyCode : keyOption->GetPreKeys()) {
            MMI_HILOGD("keyOption->prekey:%{public}d", keyCode);
        }

        if (keyOption->IsFinalKeyDown()) {
            ClearTimer(subscriber);
            MMI_HILOGD("keyOption->IsFinalKeyDown()");
            continue;
        }

        if (keyCode != keyOption->GetFinalKey()) {
            MMI_HILOGD("keyCode != keyOption->GetFinalKey()");
            continue;
        }

        if (!IsPreKeysMatch(keyOption->GetPreKeys(), pressedKeys)) {
            MMI_HILOGD("preKeysMatch failed");
            continue;
        }

        auto duration = keyOption->GetFinalKeyDownDuration();
        if (duration <= 0) {
            MMI_HILOGD("duration <= 0");
            NotifySubscriber(keyEvent, subscriber);
            handled = true;
            continue;
        }

        const KeyEvent::KeyItem* keyItem = keyEvent->GetKeyItem();
        CHKPF(keyItem);
        auto upTime = keyEvent->GetActionTime();
        auto downTime = keyItem->GetDownTime();
        if (upTime - downTime >= (static_cast<int64_t>(duration) * 1000)) {
            MMI_HILOGE("upTime - downTime >= duration");
            continue;
        }

        MMI_HILOGD("upTime - downTime < duration");
        NotifySubscriber(keyEvent, subscriber);
        handled = true;
    }

    MMI_HILOGD("%{public}s", handled ? "true" : "false");
    return handled;
}

bool KeyEventSubscriber::HandleKeyCanel(const std::shared_ptr<KeyEvent>& keyEvent)
{
    CALL_LOG_ENTER;
    CHKPF(keyEvent);
    for (const auto &subscriber : subscribers_) {
        ClearTimer(subscriber);
    }
    return false;
}

bool KeyEventSubscriber::CloneKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPF(keyEvent);
    if (keyEvent_ == nullptr) {
        MMI_HILOGW("keyEvent_ is nullptr");
        keyEvent_ = KeyEvent::Clone(keyEvent);
    }
    CHKPF(keyEvent_);
    return true;
}

void KeyEventSubscriber::RemoveKeyCode(int32_t keyCode, std::vector<int32_t>& keyCodes)
{
    for (auto it = keyCodes.begin(); it != keyCodes.end(); ++it) {
        if (*it == keyCode) {
            keyCodes.erase(it);
            return;
        }
    }
}
} // namespace MMI
} // namespace OHOS
