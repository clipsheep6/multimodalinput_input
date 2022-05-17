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

#ifndef POINTER_EVENT_HANDLER_H
#define POINTER_EVENT_HANDLER_H

#include <memory>

#include "i_input_event_handler.h"

namespace OHOS {
namespace MMI {
class PointerEventHandler : public IInputEventHandler {
public:
    PointerEventHandler() = default;
    ~PointerEventHandler() = default;
    int32_t HandleLibinputEvent(libinput_event* event) override;
    int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;

private:
    int32_t HandleTouchPadEvent(libinput_event* event);
    int32_t HandleGestureEvent(libinput_event* event);
    int32_t HandleMouseEvent(libinput_event* event);
};
} // namespace MMI
} // namespace OHOS
#endif // POINTER_EVENT_HANDLER_H