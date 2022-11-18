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

#include "input_key_subscriber.h"

#include <cinttypes>

#include "bytrace_adapter.h"
#include "input_event_data_transformation.h"
#include "input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputKeySubscriber" };
constexpr int32_t INVALID_SUBSCRIBE_ID = -1;
constexpr size_t PRE_KEYS_NUM = 4;
} // namespace
int32_t InputKeySubscriber::subscribeIdManager_ = 0;

InputKeySubscriber::SubscribeKeyEventInfo::SubscribeKeyEventInfo(
    std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
    : keyOption_(keyOption), callback_(callback)
{
    if (InputKeySubscriber::subscribeIdManager_ >= INT_MAX) {
        subscribeId_ = -1;
        MMI_HILOGE("The subscribeId has reached the upper limit, cannot continue the subscription");
        return;
    }
    subscribeId_ = InputKeySubscriber::subscribeIdManager_;
    ++InputKeySubscriber::subscribeIdManager_;
}

static bool operator<(const KeyOption &first, const KeyOption &second)
{
    if (first.GetFinalKey() != second.GetFinalKey()) {
        return (first.GetFinalKey() < second.GetFinalKey());
    }
    const std::set<int32_t> sPrekeys { first.GetPreKeys() };
    const std::set<int32_t> tPrekeys { second.GetPreKeys() };
    std::set<int32_t>::const_iterator sIter = sPrekeys.cbegin();
    std::set<int32_t>::const_iterator tIter = tPrekeys.cbegin();
    for (; sIter != sPrekeys.cend() && tIter != tPrekeys.cend(); ++sIter, ++tIter) {
        if (*sIter != *tIter) {
            return (*sIter < *tIter);
        }
    }
    if (sIter != sPrekeys.cend() || tIter != tPrekeys.cend()) {
        return (tIter != tPrekeys.cend());
    }
    if (first.IsFinalKeyDown()) {
        if (!second.IsFinalKeyDown()) {
            return false;
        }
    } else {
        if (second.IsFinalKeyDown()) {
            return true;
        }
    }
    return (first.GetFinalKeyDownDuration() < second.GetFinalKeyDownDuration());
}

bool InputKeySubscriber::SubscribeKeyEventInfo::operator<(const SubscribeKeyEventInfo &other) const
{
    if (keyOption_ == nullptr) {
        return (other.keyOption_ != nullptr);
    } else if (other.keyOption_ == nullptr) {
        return false;
    }
    return (*keyOption_ < *other.keyOption_);
}

int32_t InputKeySubscriber::SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
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

    std::lock_guard<std::mutex> guard(mtx_);
    auto [tIter, isOk] = subscribeInfos_.emplace(keyOption, callback);
    if (!isOk) {
        MMI_HILOGW("Subscription is duplicated");
        return tIter->GetSubscribeId();
    }
    if (HandlerSubscribeKeyEvent(*tIter) != RET_OK) {
        MMI_HILOGE("Subscribing key event failed");
    }

    MMI_HILOGD("subscribeId:%{public}d,keyOption->finalKey:%{public}d,"
        "keyOption->isFinalKeyDown:%{public}s,keyOption->finalKeyDownDuration:%{public}d",
        tIter->GetSubscribeId(), keyOption->GetFinalKey(),
        keyOption->IsFinalKeyDown() ? "true" : "false",
        keyOption->GetFinalKeyDownDuration());
    for (const auto &preKey : preKeys) {
        MMI_HILOGD("prekey:%{public}d", preKey);
    }
    return tIter->GetSubscribeId();
}

int32_t InputKeySubscriber::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_INFO_TRACE;
    if (subscribeId < 0) {
        MMI_HILOGE("The subscribe id is less than 0");
        return RET_ERR;
    }

    std::lock_guard<std::mutex> guard(mtx_);
    if (subscribeInfos_.empty()) {
        MMI_HILOGE("The subscribeInfos is empty");
        return RET_ERR;
    }

    for (auto it = subscribeInfos_.begin(); it != subscribeInfos_.end(); ++it) {
        if (it->GetSubscribeId() == subscribeId) {
            if (HandlerUnsubscribeKeyEvent(subscribeId) != RET_OK) {
                MMI_HILOGE("Leave, unsubscribe key event failed");
                return RET_ERR;
            }
            subscribeInfos_.erase(it);
            return RET_OK;
        }
    }
    return RET_ERR;
}

int32_t InputKeySubscriber::HandlerSubscribeKeyCallback(std::shared_ptr<KeyEvent> event,
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
    auto callback = info->GetCallback();
    if (!callback) {
        MMI_HILOGE("Callback is null");
        return RET_ERR;
    }
    callback(event);
    MMI_HILOGD("Key event id:%{public}d keyCode:%{public}d", subscribeId, event->GetKeyCode());
    return RET_OK;
}

void InputKeySubscriber::OnConnected()
{
    CALL_DEBUG_ENTER;
    if (subscribeInfos_.empty()) {
        MMI_HILOGD("Leave, subscribeInfos_ is empty");
        return;
    }
    for (const auto& subscriberInfo : subscribeInfos_) {
        if (HandlerSubscribeKeyEvent(subscriberInfo) != RET_OK) {
            MMI_HILOGE("Subscribe key event failed");
        }
    }
}

const InputKeySubscriber::SubscribeKeyEventInfo* InputKeySubscriber::GetSubscribeKeyEvent(
    int32_t id)
{
    if (id < 0) {
        MMI_HILOGE("Invalid input param id:%{public}d", id);
        return nullptr;
    }
    for (const auto &subscriber : subscribeInfos_) {
        if (subscriber.GetSubscribeId() == id) {
            return &subscriber;
        }
    }
    return nullptr;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputKeySubscriber::HandlerSubscribeKeyEvent(
    const InputKeySubscriber::SubscribeKeyEventInfo &subscribeInfo)
{
    CALL_DEBUG_ENTER;
    return MultimodalInputConnMgr->SubscribeKeyEvent(subscribeInfo.GetSubscribeId(), subscribeInfo.GetKeyOption());
}

int32_t InputKeySubscriber::HandlerUnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_DEBUG_ENTER;
    return MultimodalInputConnMgr->UnsubscribeKeyEvent(subscribeId);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

bool InputKeySubscriber::GetFunctionKeyState(int32_t funcKey)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    bool state { false };
    int32_t ret = MultimodalInputConnMgr->GetFunctionKeyState(funcKey, state);
    if (ret != RET_OK) {
        MMI_HILOGE("Send to server failed, ret:%{public}d", ret);
    }
    return state;
}

int32_t InputKeySubscriber::SetFunctionKeyState(int32_t funcKey, bool enable)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->SetFunctionKeyState(funcKey, enable);
    if (ret != RET_OK) {
        MMI_HILOGE("Send to server failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t InputKeySubscriber::OnSubscribeKeyEventCallback(NetPacket &pkt)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    int32_t ret = InputEventDataTransformation::NetPacketToKeyEvent(pkt, keyEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("Read net packet failed");
        return RET_ERR;
    }
    int32_t fd = -1;
    int32_t subscribeId = -1;
    pkt >> fd >> subscribeId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read fd failed");
        return PACKET_READ_FAIL;
    }
    MMI_HILOGD("Subscribe:%{public}d,Fd:%{public}d,KeyEvent:%{public}d,"
               "KeyCode:%{public}d,ActionTime:%{public}" PRId64 ",ActionStartTime:%{public}" PRId64 ","
               "Action:%{public}d,KeyAction:%{public}d,EventType:%{public}d,Flag:%{public}u",
        subscribeId, fd, keyEvent->GetId(), keyEvent->GetKeyCode(), keyEvent->GetActionTime(),
        keyEvent->GetActionStartTime(), keyEvent->GetAction(), keyEvent->GetKeyAction(),
        keyEvent->GetEventType(), keyEvent->GetFlag());
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::TRACE_START, BytraceAdapter::KEY_SUBSCRIBE_EVENT);
    return HandlerSubscribeKeyCallback(keyEvent, subscribeId);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD
} // namespace MMI
} // namespace OHOS