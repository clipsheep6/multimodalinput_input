/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef POINTER_EVENT_NDK_H
#define POINTER_EVENT_NDK_H

#include <memory>
#include "oh_input_manager.h"
#include "pointer_event.h"

std::shared_ptr<OHOS::MMI::PointerEvent> OH_Input_TouchEventToPointerEvent(Input_TouchEvent *touchEvent,
    int32_t windowX, int32_t windowY);

#endif // POINTER_EVENT_NDK_H