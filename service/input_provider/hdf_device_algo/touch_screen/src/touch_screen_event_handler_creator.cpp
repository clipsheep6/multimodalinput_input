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

#include "touch_screen_event_handler.h"
#include "i_input_context.h"

extern "C" bool GetIEventHandlerImpl(std::shared_ptr<OHOS::MMI::IInputContext> context, int32_t capabilities, IEventHandlerStruct **s)
{
    if (capabilities & OHOS::MMI::IInputDevice::CAPABILITY_TOUCHSCREEN) {
        *s = new IEventHandlerStruct;
        (*s)->eventHandler = OHOS::MMI::TouchScreenEventHandler::CreateInstance(context);
        return true;
    }
    return false;
}

