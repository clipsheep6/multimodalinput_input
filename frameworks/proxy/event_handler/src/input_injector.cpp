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

#include "input_injector.h"

#include "event_log_helper.h"
#include "input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputInjector" };
} // namespace

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void InputInjector::InjectKeyEvent(const std::shared_ptr<KeyEvent> &keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mtx_);
    keyEvent->UpdateId();
    if (keyEvent->GetKeyCode() < 0) {
        MMI_HILOGE("KeyCode is invalid:%{public}u", keyEvent->GetKeyCode());
        return;
    }
    int32_t ret = MultimodalInputConnMgr->InjectKeyEvent(keyEvent);
    if (ret != 0) {
        MMI_HILOGE("Inject key event failed, ret:%{public}d", ret);
    }
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputInjector::InjectPointerEvent(std::shared_ptr<PointerEvent> &pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mtx_);
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE ||
        pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHPAD) {
#ifndef OHOS_BUILD_ENABLE_POINTER
        MMI_HILOGW("Pointer device does not support");
        return;
#endif
    }
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
#ifndef OHOS_BUILD_ENABLE_TOUCH
        MMI_HILOGW("Touchscreen device does not support");
        return;
#endif
    }
#ifndef OHOS_BUILD_ENABLE_JOYSTICK
    if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_JOYSTICK) {
        MMI_HILOGW("Joystick device does not support");
        return;
    }
#endif
    EventLogHelper::PrintEventData(pointerEvent);
    int32_t ret = MultimodalInputConnMgr->InjectPointerEvent(pointerEvent);
    if (ret != 0) {
        MMI_HILOGE("Inject pointer event failed, ret:%{public}d", ret);
    }
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS
