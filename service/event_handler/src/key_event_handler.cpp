/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "key_event_handler.h"

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "key_event_value_transformation.h"
#include "mmi_log.h"
#include "timer_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyEventHandler" };
}

int32_t KeyEventHandler::HandleLibinputEvent(libinput_event* event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto keyEvent = InputHandler->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto result = Normalize(event, keyEvent);
    if (result == MULTIDEVICE_SAME_EVENT_MARK) {
        MMI_HILOGD("The same event reported by multi_device should be discarded");
        return RET_OK;
    }
    if (result != RET_OK) {
        MMI_HILOGE("KeyEvent package failed. ret:%{public}d,errCode:%{public}d", result, KEY_EVENT_PKG_FAIL);
        return KEY_EVENT_PKG_FAIL;
    }
    BytraceAdapter::StartBytrace(keyEvent);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);

    auto ret = nextHandler_->HandleKeyEvent(keyEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("KeyEvent dispatch failed. ret:%{public}d,errCode:%{public}d", ret, KEY_EVENT_DISP_FAIL);
        return KEY_EVENT_DISP_FAIL;
    }
    Repeat(keyEvent);
    MMI_HILOGD("keyCode:%{public}d,action:%{public}d", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
    return RET_OK;
}

int32_t KeyEventHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandleKeyEvent(keyEvent);
}

int32_t KeyEventHandler::Normalize(libinput_event* event, std::shared_ptr<KeyEvent> keyEvent)
{
    keyEvent->UpdateId();
    auto data = libinput_event_get_keyboard_event(event);
    CHKPR(data, ERROR_NULL_POINTER);

    auto device = libinput_event_get_device(event);
    int32_t deviceId = InputDevMgr->FindInputDeviceId(device);
    int32_t keyCode = static_cast<int32_t>(libinput_event_keyboard_get_key(data));
    auto Okey = TransferKeyValue(keyCode);
    keyCode = Okey.sysKeyValue;
    int32_t keyAction = (libinput_event_keyboard_get_key_state(data) == 0) ?
        (KeyEvent::KEY_ACTION_UP) : (KeyEvent::KEY_ACTION_DOWN);
    auto preAction = keyEvent->GetAction();
    if (preAction == KeyEvent::KEY_ACTION_UP) {
        auto preUpKeyItem = keyEvent->GetKeyItem();
        if (preUpKeyItem != nullptr) {
            keyEvent->RemoveReleasedKeyItems(*preUpKeyItem);
        } else {
            MMI_HILOGE("preUpKeyItem is null");
        }
    }
    int64_t time = GetSysClockTime();
    keyEvent->SetActionTime(time);
    keyEvent->SetAction(keyAction);
    keyEvent->SetDeviceId(deviceId);
    keyEvent->SetKeyCode(keyCode);
    keyEvent->SetKeyAction(keyAction);
    if (keyEvent->GetPressedKeys().empty()) {
        keyEvent->SetActionStartTime(time);
    }

    KeyEvent::KeyItem item;
    bool isKeyPressed = (libinput_event_keyboard_get_key_state(data) != KEYSTATUS);
    item.SetDownTime(time);
    item.SetKeyCode(keyCode);
    item.SetDeviceId(deviceId);
    item.SetPressed(isKeyPressed);

    if (keyAction == KeyEvent::KEY_ACTION_DOWN) {
        keyEvent->AddPressedKeyItems(item);
    }
    if (keyAction == KeyEvent::KEY_ACTION_UP) {
        auto pressedKeyItem = keyEvent->GetKeyItem(keyCode);
        if (pressedKeyItem != nullptr) {
            item.SetDownTime(pressedKeyItem->GetDownTime());
        } else {
            MMI_HILOGE("Find pressed key failed, keyCode:%{public}d", keyCode);
        }
        keyEvent->RemoveReleasedKeyItems(item);
        keyEvent->AddPressedKeyItems(item);
    }
    return RET_OK;
}

void KeyEventHandler::Repeat(const std::shared_ptr<KeyEvent> keyEvent)
{
        if (keyEvent->GetKeyCode() == KeyEvent::KEYCODE_VOLUME_UP ||
            keyEvent->GetKeyCode() == KeyEvent::KEYCODE_VOLUME_DOWN ||
            keyEvent->GetKeyCode() == KeyEvent::KEYCODE_DEL) {
            if (!TimerMgr->IsExist(timerId_) && keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
                AddHandleTimer();
                MMI_HILOGD("add a timer");
            }
        if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP && TimerMgr->IsExist(timerId_)) {
            TimerMgr->RemoveTimer(timerId_);
            timerId_ = -1;
        }
    }
}

void KeyEventHandler::AddHandleTimer(int32_t timeout)
{
    timerId_ = TimerMgr->AddTimer(timeout, 1, [this]() {
        MMI_HILOGD("enter");
        auto keyEvent = InputHandler->GetKeyEvent();
        CHKPV(keyEvent);
        if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP) {
            MMI_HILOGD("key up");
            return;
        }
        auto ret = HandleKeyEvent(keyEvent);
        if (ret != RET_OK) {
            MMI_HILOGE("KeyEvent dispatch failed. ret:%{public}d,errCode:%{public}d", ret, KEY_EVENT_DISP_FAIL);
        }
        constexpr int32_t triggerTime = 100;
        this->AddHandleTimer(triggerTime);
        MMI_HILOGD("leave");
    });
}
} // namespace MMI
} // namespace OHOS