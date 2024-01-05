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

#ifndef NATIVE_INPUT_EVENT_MANAGER_H
#define NATIVE_INPUT_EVENT_MANAGER_H

#include "capi_input_event.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    OTHER_ERROR = -1,
    COMMON_PARAMETER_ERROR = 401,
}; 

int32_t OH_InputEvent_InjectEvent(struct InputEvent_KeyEvent* keyEvent);

int32_t OH_InputEvent_InjectMouseEvent(struct InputEvent_MouseEvent* mouseEvent);

int32_t OH_InputEvent_InjectTouchEvent(struct InputEvent_TouchEvent* touchEvent);

#ifdef __cplusplus
}
#endif

#endif // NATIVE_INPUT_EVENT_MANAGER_H
