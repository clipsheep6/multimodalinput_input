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
#include "plugin_context.h"
#include "i_input_event_handler_plugin.h"
namespace OHOS {
namespace MMI {
struct inputEventHandlerPlugin {
    void* osHandler;
    bool loadStatus;
    IPlugin* plugin;
    std::string osPath;
    std::shared_ptr<IInputEventHandler> pluginHandler;
};

// struct pluginMonitorTime {
//     std::string pluginName { 0 };
//     uint32_t pluginCostTimeMax { 0 };
// }

// struct pluginMonitorMem {
//     std::string pluginName { 0 };
//     uint32_t pluginCostMemMax { 0 };
// }

// struct pluginMonitor {
//     pluginMonitorTime pluginCostTime;
//     pluginCostTimeAvg { 0 };
//     pluginMonitorMem pluginCostMem;
//     pluginCostMemAvg { 0 };
// }

class InputEventHandlerPluginMgr
{
public:
    explicit InputEventHandlerPluginMgr();
    ~InputEventHandlerPluginMgr() = default;
    DISALLOW_COPY_AND_MOVE(InputEventHandlerPluginMgr);
    void SetDeivceManager(std::shared_ptr<IInputDeviceManager> inputDeviceMgr);
    void StartWatchPluginDir();
    void ReadPluginDir(const std::string pluginPath);
    bool LoadPlugin(std::string pluginPath,std::string pluginName, bool initStatus);
    void UnloadPlugin(std::string pluginPath);
    void UnloadPlugins();
    bool InitINotify();
    void OnTimer();
    int32_t GetReadFd();
    void stopINotify();
    std::list<std::shared_ptr<IInputEventPluginContext>> GetContext() const { return context_; }
    void SetHandler(std::shared_ptr<IInputEventHandler>& pHandlers);
    void DelPlugin(std::shared_ptr<IInputEventHandler> pluginHandler);
private:
    std::list<std::shared_ptr<IInputEventPluginContext>> context_;
    std::map<std::string , inputEventHandlerPlugin> pluginInfoList = {};
    int32_t fd_ { 0 };
    int32_t wd_ { 0 };
    std::shared_ptr<IInputDeviceManager> inputDevMgr_;
    //pluginMonitor pluginMonitor_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_HANDLER_PLUGIN_H
