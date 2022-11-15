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

#ifndef INPUT_EVENT_HANDLER_PLUGIN_H
#define INPUT_EVENT_HANDLER_PLUGIN_H

#include <string>
#include <map>

namespace OHOS {
namespace MMI {
struct inputEventHandlerPlugin {
    void* plugin;
    std::string pluginName;
    bool loadStatus;
};
class InputEventHandlerPluginMgr
{
public:
    explict InputEventHandlerPluginMgr()
    {
        context_ = std::make_shared<IPluginContext>();
    }
    ~InputEventHandlerPluginMgr() = default;
    DISALLOW_COPY_AND_MOVE(InputEventHandlerPluginMgr);
    void SetDeivceManager();
    void StartWatchPluginDir();
    void ReadPluginDir(const std::string pluginPath);
    bool LoadPlugin(std::string pluginPath, std::string pluginName);
    bool UnloadPlugin(std::string pluginPath);
    std::shared_ptr<IPluginContext> GetContext() const { return context_; }
private:
    std::shared_ptr<IPluginContext> context_;
    std::map<std::string , inputEventHandlerPlugin> pluginInfoList = {};
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_HANDLER_PLUGIN_H
