/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CAPI_INPUT_EVENT_H
#define CAPI_INPUT_EVENT_H

#include <stdint.h>
#include "capi_key_event.h"
#include "capi_mouse_event.h"
#include "capi_touch_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inject system keys.
 *
 * @param { CAPI_KeyEvent } keyEvent - the key event to be injected.
 * @return Returns <b>0</b> if success; returns a non-0 value otherwise
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @systemapi hide for inner use
 * @since 11
 */
int32_t OH_InputEvent_InjectEvent(struct InputEvent_KeyEvent* keyEvent);

/**
 * Inject mouse event.
 *
 * @param { CAPI_MouseEvent } mouseEvent - the mouse event to be injected.
 * @return Returns <b>0</b> if success; returns a non-0 value otherwise
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @systemapi hide for inner use
 * @since 11
 */
int32_t OH_InputEvent_InjectMouseEvent(struct InputEvent_MouseEvent* mouseEvent);

/**
 * Inject touch event.
 *
 * @param { CAPI_TouchEvent } touchEvent - the touch event to be injected.
 * @return Returns <b>0</b> if success; returns a non-0 value otherwise
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @systemapi hide for inner use
 * @since 11
 */
int32_t OH_InputEvent_InjectTouchEvent(struct InputEvent_TouchEvent* touchEvent);

#ifdef __cplusplus
}
#endif
#endif // CAPI_INPUT_EVENT_H
