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

#ifndef I_INPUT_EVENT_HANDLER_H
#define I_INPUT_EVENT_HANDLER_H

#include <memory>

#include "define_multimodal.h"
#include "key_event.h"
#include "pointer_event.h"

struct libinput_event;

namespace OHOS {
namespace MMI {
class IInputEventHandler {
    friend class PluginContext;
    friend class InputEventHandler;
public:
    enum class Priority {
        NORMALIZE = 0,
        FILTER = 200,
        INTERCEPTIR = 500,
        KEY_COMMAND = 600,
        KEY_SUBSCEIBER = 700,
        MONITOR = 800,
        DISPATCH = 1000,
    };
    IInputEventHandler(int32_t handlerPriority) : handlerPriority_(handlerPriority) {}
    DISALLOW_COPY_AND_MOVE(IInputEventHandler);
    virtual ~IInputEventHandler() = default;
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    virtual void HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent) = 0;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    virtual void HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent) = 0;
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    virtual void HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent) = 0;
#endif // OHOS_BUILD_ENABLE_TOUCH
    virtual void SetNext(std::shared_ptr<IInputEventHandler> nextHandler)
    {
        nextHandler_ = nextHandler;
    };

protected:
    std::shared_ptr<IInputEventHandler> nextHandler_ { nullptr };
    const int32_t handlerPriority_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_H