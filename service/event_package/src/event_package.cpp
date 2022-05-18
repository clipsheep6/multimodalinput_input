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

#include "event_package.h"

#include "config_key_value_transform.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
EventPackage::EventPackage() {}

EventPackage::~EventPackage() {}

int32_t EventPackage::PackageKeyEvent(struct libinput_event *event, std::shared_ptr<KeyEvent> key)
{
    CALL_LOG_ENTER;
    CHKPR(event, PARAM_INPUT_INVALID);
    CHKPR(key, ERROR_NULL_POINTER);
    key->UpdateId();
    auto data = libinput_event_get_keyboard_event(event);
    CHKPR(data, ERROR_NULL_POINTER);

    auto device = libinput_event_get_device(event);
    int32_t deviceId = InputDevMgr->FindInputDeviceId(device);
    int32_t keyCode = static_cast<int32_t>(libinput_event_keyboard_get_key(data));
    MMI_HILOGD("The linux input keyCode:%{public}d", keyCode);
    auto Okey = KeyValueTransform->TransferDeviceKeyValue(event, keyCode);
    keyCode = Okey.sysKeyValue;
    int32_t keyAction = (libinput_event_keyboard_get_key_state(data) == 0) ?
        (KeyEvent::KEY_ACTION_UP) : (KeyEvent::KEY_ACTION_DOWN);
    auto preAction = key->GetAction();
    if (preAction == KeyEvent::KEY_ACTION_UP) {
        auto preUpKeyItem = key->GetKeyItem();
        if (preUpKeyItem != nullptr) {
            key->RemoveReleasedKeyItems(*preUpKeyItem);
        } else {
            MMI_HILOGE("preUpKeyItem is null");
        }
    }
    int64_t time = GetSysClockTime();
    key->SetActionTime(time);
    key->SetAction(keyAction);
    key->SetDeviceId(deviceId);
    key->SetKeyCode(keyCode);
    key->SetKeyAction(keyAction);
    if (key->GetPressedKeys().empty()) {
        key->SetActionStartTime(time);
    }

    KeyEvent::KeyItem item;
    bool isKeyPressed = (libinput_event_keyboard_get_key_state(data) != KEYSTATUS);
    item.SetDownTime(time);
    item.SetKeyCode(keyCode);
    item.SetDeviceId(deviceId);
    item.SetPressed(isKeyPressed);

    if (keyAction == KeyEvent::KEY_ACTION_DOWN) {
        key->AddPressedKeyItems(item);
    }
    if (keyAction == KeyEvent::KEY_ACTION_UP) {
        auto pressedKeyItem = key->GetKeyItem(keyCode);
        if (pressedKeyItem != nullptr) {
            item.SetDownTime(pressedKeyItem->GetDownTime());
        } else {
            MMI_HILOGE("Find pressed key failed, keyCode:%{public}d", keyCode);
        }
        key->RemoveReleasedKeyItems(item);
        key->AddPressedKeyItems(item);
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
