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
    struct PluginDispatchKeyEvent {};
    struct PluginDispatchPointEvent {};
    struct PluginDispatchTouchEvent {};
class IInputEventHandler {
public:
    enum class PluginDispatchEventType {
        KEY_EVENT,
        POINT_EVENT,
        TOUCH_EVENT
    };
    IInputEventHandler() = default;
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
    virtual std::shared_ptr<IInputEventHandler> GetNextHandler()
    {
        return nextHandler_;
    }
    template<typename T1, typename T2>
    void HandleEvent(const std::shared_ptr<T1> event);

protected:
    std::shared_ptr<IInputEventHandler> nextHandler_ { nullptr };
};
template<>
inline void IInputEventHandler::HandleEvent<KeyEvent, PluginDispatchKeyEvent>(const std::shared_ptr<KeyEvent> event)
{
    HandleKeyEvent(event);
}

template<>
inline void IInputEventHandler::HandleEvent<PointerEvent, PluginDispatchPointEvent>(const std::shared_ptr<PointerEvent> event)
{
    HandlePointerEvent(event);
}

template<>
inline void IInputEventHandler::HandleEvent<PointerEvent, PluginDispatchTouchEvent>(const std::shared_ptr<PointerEvent> event)
{
    HandleTouchEvent(event);
}

} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_H