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

#ifndef TOUCH_2_KEY_HANDLER_H
#define TOUCH_2_KEY_HANDLER_H
#include <thread>
#include "i_input_event_convert_handler.h"

namespace OHOS {
namespace MMI {
class Touch2KeyHandler : public IInputEventConvertHandler {

public:
    Touch2KeyHandler() = default;
    DISALLOW_COPY_AND_MOVE(Touch2KeyHandler);
    virtual ~Touch2KeyHandler() = default;
    virtual void HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
    virtual void HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    virtual void HandleTouchEvent(const std::shared_ptr<PointerEvent> touchEvent);
    virtual PluginDispatchCmd GetDispatchCmd();
    virtual PluginDispatchEventType GetDispatchEventType();
    virtual const std::shared_ptr<KeyEvent> GetKeyEvent();
    virtual const std::shared_ptr<PointerEvent> GetPointEvent();
    virtual IInputEventConvertHandler::PluginInfo GetPluginInfo();
    virtual void SetPluginfunctionStatus(std::map<PluginfunctionId, bool> FunctionStatus);
    virtual bool GetisPlugin() { return isPlugin; };
private:
    bool isPlugin { true };
    std::shared_ptr<PluginContext> pluginContext_ = nullptr;
};

//the class factories
extern "C" IInputEventConvertHandler* create(std::shared_ptr<PluginContext> pluginContext) {
    return new Touch2KeyHandler(pluginContext);
}
extern "C" void destroy(IInputEventConvertHandler* p) {
    delete p;
}
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_2_KEY_HANDLER_H


