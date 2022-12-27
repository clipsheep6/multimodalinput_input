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

#ifndef INPUT_EVENT_HANDLER_PLUGIN_MGR_H
#define INPUT_EVENT_HANDLER_PLUGIN_MGR_H

#include <string>

#include "plugin_context.h"
#include "i_input_event_handler_plugin.h"

namespace OHOS {
namespace MMI {
struct InputEventHandlerPluginInfo {
    void* osHandler { nullptr };
    IInputEventHandlerPlugin* plugin { nullptr };
    std::string osPath;
    std::shared_ptr<IInputEventHandler> pluginHandler { nullptr };
};

class InputEventHandlerPluginMgr
{
public:
    explicit InputEventHandlerPluginMgr() = default;
    ~InputEventHandlerPluginMgr() = default;
    DISALLOW_COPY_AND_MOVE(InputEventHandlerPluginMgr);
    void ReadPluginDir();
    bool LoadPlugin(const std::string pluginPath, const std::string pluginName);
    void UnloadPlugin(const std::string pluginPath);
    void UnloadPlugins();
    void OnTimer();
    std::list<std::shared_ptr<IInputEventHandlerPluginContext>> GetContext() const { return contexts_; }
    void SetHandler(std::shared_ptr<IInputEventHandler>& pHandlers);
    void DelPlugin(std::shared_ptr<IInputEventHandler> pluginHandler);
private:
    std::list<std::shared_ptr<IInputEventHandlerPluginContext>> contexts_;
    std::map<std::string, InputEventHandlerPluginInfo> pluginInfos;
    int32_t max { 0 };
    int32_t avg { 0 };
    std::map<std::shared_ptr<IInputEventHandler>, int32_t> timeoutInfos;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_HANDLER_PLUGIN_MGR_H
