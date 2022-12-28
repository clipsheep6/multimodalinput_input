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

#include "input_event_handler_plugin_mgr.h"

#include <dlfcn.h>
#include <dirent.h>

#include "util.h"

#include "i_input_event_handler.h"
#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputEventHandlerPluginMgr"};
const std::string INPUT_EVENT_HANDLER_PLUGIN_HOME = "/system/lib/module/multimodalinput/plugins/event_handler/";
#define TIMEOUT_MS 3000
#define TIMEOUT_MAX 500
#define ERRORS_NUMBER_MAX 5

bool CheckFileExtendName(const std::string &filePath, const std::string &extension)
{
    std::string::size_type pos = filePath.find_last_of('.');
    if (pos == std::string::npos) {
        MMI_HILOGE("File is not find extension");
        return false;
    }
    return (filePath.substr(pos + 1, filePath.npos) == extension);
}
} // namespace

void InputEventHandlerPluginMgr::ReadPluginDir()
{
    DIR* dir = opendir(INPUT_EVENT_HANDLER_PLUGIN_HOME.c_str());
    if (dir == nullptr) {
        MMI_HILOGE("Open plugin home(%{public}s) failed. errno: %{public}d",
            INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
        return;
    }
    dirent* p = nullptr;
    while ((p = readdir(dir)) != nullptr) {
        if (p->d_type != DT_REG) {
            continue;
        }
        char realPath[PATH_MAX] = {};
        std::string tmpPath = INPUT_EVENT_HANDLER_PLUGIN_HOME + p->d_name;
        if (realpath(tmpPath.data(), realPath) == nullptr) {
            continue;
        }
        LoadPlugin(realPath, p->d_name);
    }
    auto ret = closedir(dir);
    if (ret != 0) {
        MMI_HILOGE("Closedir failed, dirname:%{public}s, errno:%{public}d",
            INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
    }
}

bool InputEventHandlerPluginMgr::LoadPlugin(const std::string pluginPath, const std::string pluginName)
{
    if (!CheckFileExtendName(pluginPath, "so")) {
        MMI_HILOGE("File %{public}s is not .so", pluginName.data());
        return false;
    }
    if (pluginInfos.find(pluginName) != pluginInfos.end()) {
        MMI_HILOGE("Plugin %{public}s already exists", pluginName.data());
        return false;
    }
    void *handle = dlopen(pluginPath.data(), RTLD_NOW);
    if(handle == nullptr){
        MMI_HILOGE("Open plugin failed, so name:%{public}s, msg:%{public}s", pluginPath.data(), dlerror());
        return false;
    }
    GetPlugin* getPlugin = (GetPlugin*) dlsym(handle, "create");
    if(getPlugin == NULL) {
        MMI_HILOGE("Dlsym msg:%{public}s", dlerror());
        dlclose(handle);
        return false;
    }
    auto plugin = getPlugin();
    if (!plugin) {
        MMI_HILOGE("Plugin cerate error");
        return false;
    }
    auto context = std::make_shared<PluginContext>();
    contexts_.push_back(context);
    InputEventHandlerPluginInfo pluginInfo = {};
    if (plugin->Init(contexts_.back())) {
        pluginInfo.pluginHandler = context->GetEventHandler();
    }
    pluginInfo.osHandler = handle;
    pluginInfo.plugin = plugin;
    pluginInfos.insert(std::make_pair(pluginName, pluginInfo));
    return true;
}

void InputEventHandlerPluginMgr::UnloadPlugin(const std::string pluginName)
{
    auto pluginInfo = pluginInfos.find(pluginName);
    if (pluginInfo == pluginInfos.end()) {
        MMI_HILOGE("Not found plugin %{plugin}s", pluginName.data());
        return;
    }
    InputHandler->Remove(pluginInfo->second.pluginHandler);
    for (auto it = contexts_.begin(); it != contexts_.end(); ++it) {
        if ((*it)->GetEventHandler() == pluginInfo->second.pluginHandler) {
            (*it)->SetEventHandler(nullptr);
            it = contexts_.erase(it);
            break;
        }
    }
    ReleasePlugin* releasePlugin = (ReleasePlugin*) dlsym(pluginInfo->second.osHandler, "Release");
    if(releasePlugin == nullptr) {
        MMI_HILOGE("ReleasePlugin msg:%{public}s", dlerror());
    }
    releasePlugin(pluginInfo->second.plugin);
    pluginInfos.erase(pluginName);
}

void InputEventHandlerPluginMgr::DelPlugin(std::shared_ptr<IInputEventHandler> pluginHandler)
{
    for (const auto &item : pluginInfos) {
        if (item.second.pluginHandler != pluginHandler) {
            continue;
        }
        timeoutInfos.erase(pluginHandler);
        UnloadPlugin(item.first);
        return;
    }
}

void InputEventHandlerPluginMgr::OnTimer()
{
    auto timeout = GetSysClockTime() + TIMEOUT_MS;
    for (auto &context : contexts_) {
        CHKPV(context);
        context->OnReport(max, avg);
        auto timeoutInfo = timeoutInfos.find(context->GetEventHandler());
        if (timeoutInfo == timeoutInfos.end()) {
            timeoutInfo->second = 0;
        }
        if (max > TIMEOUT_MAX) {
            timeoutInfo->second = timeoutInfo->second + 1;
            if (timeoutInfo->second < ERRORS_NUMBER_MAX) {
                continue;
            }
            DelPlugin(context->GetEventHandler());
            return;
        } else {
            timeoutInfo->second = 0;
        }
        if (timeout > GetSysClockTime()) {
            return;
        }
    }
}
} // namespace MMI
} // namespace OHOS