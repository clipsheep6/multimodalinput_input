
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

#ifndef TOUCH_2_KEY_NEW_HANDLER_H
#define TOUCH_2_KEY_NEW_HANDLER_H

#include "touch_2_key_plugin.h"
#include "i_input_event_handler.h"
namespace OHOS {
namespace MMI {
class Touch2KeyNewHandler : public iinputeventhandler
{
public:
    Touch2KeyNewHandler(IInputEventPluginContext *context) : IInputEventHandler(NORMALLIZE, 100), context_(context)
    {

    }
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    virtual void HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
    {
        nextHandler_->HandleKeyEvent(keyEvent);
    }
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    virtual void HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
    {
        nextHandler_->HandlePointerEvent(pointerEvent);
    }
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    virtual void HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
    {
        auto devMgr = context_->GetInputDeviceManager();
        if (pointerEvent->GetItem() > 3) {
            return;
        }
        auto keyEvent = KeyEvent::Create();
        //
        nextHandler_->HandleKeyEvent(keyEvent);
    }
#endif // OHOS_BUILD_ENABLE_TOUCH
private:
    IInputEventPluginContext *context_;
};

} // namespace MMI
} // namespace OHOS


#endif // TOUCH_2_KEY_NEW_HANDLER_H