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

#ifndef EVENT_PLUGINS_HANDLER_H
#define EVENT_PLUGINS_HANDLER_H

#include "i_input_event_handler.h"
#include "i_input_event_convert_handler.h"
namespace OHOS {
namespace MMI {
enum class EventHandlerType
{
    KEY_EVENT,
    POINT_EVENT,
    TOUCH_EVENT,
};
class EventPluginsHandler : public IInputEventHandler {
public:
    EventPluginsHandler();
    virtual ~EventPluginsHandler() = default;
    DISALLOW_COPY_AND_MOVE(EventPluginsHandler);
    virtual void HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
    virtual void HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    virtual void HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    virtual void SetNext(std::shared_ptr<IInputEventHandler> nextHandler);
private:
    std::list<IInputEventConvertHandler::PluginInfo *> pluginInfos_;
private:
    int32_t ScanPlugins();
    int32_t UnloadPlugins();
    int32_t LoadPlugin(void *handle);
    void Dump(int32_t fd) {};
    template<typename T1, typename T2>
    void HandlePluginEventEx(std::shared_ptr<IInputEventConvertHandler> handler, const std::shared_ptr<T1> event, bool isfast);
    template<typename T1, typename T2>
    void HandlePluginEvent(const std::shared_ptr<T1> event);
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_PLUGINS_HANDLER_H
