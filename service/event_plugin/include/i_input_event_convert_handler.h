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

#ifndef I_INPUT_EVENT_CONVERT_HANDLER_H
#define I_INPUT_EVENT_CONVERT_HANDLER_H
#include "i_input_event_handler.h"

namespace OHOS {
namespace MMI {

class IInputEventConvertHandler : public IInputEventHandler {
public:
    enum class PluginDispatchCmd {
        GOTO_NEXT,
        REDIRECT,
        DISCARD
    };
public:
    virtual PluginDispatchCmd GetDispatchCmd() = 0;
    virtual PluginDispatchEventType GetDispatchEventType() = 0;
    virtual const std::shared_ptr<KeyEvent> GetKeyEvent() = 0;
    virtual const std::shared_ptr<PointerEvent> GetPointEvent() = 0;
    template<typename T1, typename T2>
    const std::shared_ptr<T1> GetEvent();
};

template<>
inline const std::shared_ptr<KeyEvent> IInputEventConvertHandler::GetEvent<KeyEvent, PluginDispatchKeyEvent>()
{
    return GetKeyEvent();
}

template<>
inline const std::shared_ptr<PointerEvent> IInputEventConvertHandler::GetEvent<PointerEvent, PluginDispatchPointEvent>()
{
    return GetPointEvent();
}

template<>
inline const std::shared_ptr<PointerEvent> IInputEventConvertHandler::GetEvent<PointerEvent, PluginDispatchTouchEvent>()
{
    return GetPointEvent();
}
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_CONVERT_HANDLER_H
