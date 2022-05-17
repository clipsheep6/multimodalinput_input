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
    auto packageResult = eventPackage_.PackageKeyEvent(event, keyEvent);
    if (packageResult == MULTIDEVICE_SAME_EVENT_MARK) {
        MMI_HILOGD("The same event reported by multi_device should be discarded");
        return RET_OK;
    }
    if (packageResult != RET_OK) {
        MMI_HILOGE("KeyEvent package failed. ret:%{public}d,errCode:%{public}d", packageResult, KEY_EVENT_PKG_FAIL);
        return KEY_EVENT_PKG_FAIL;
    }
    BytraceAdapter::StartBytrace(keyEvent);
    auto ret = HandleKeyEvent(keyEvent);
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
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Keyboard device does not support");
        return ERROR_UNSUPPORT;
    }
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    return nextHandler_->HandleKeyEvent(keyEvent);
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