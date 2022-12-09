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
#include <dlfcn.h>
#include <dirent.h>
#include <sys/inotify.h>
#include "util.h"

#include "input_event_handler_plugin.h"

#include "i_input_event_handler.h"
#include "input_event_handler.h"


namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventPluginsHandler"};
const std::string INPUT_EVENT_HANDLER_PLUGIN_HOME = "/system/lib/module/multimodalinput/plugins/";

int32_t max { 0 };
int32_t avg { 0 };
int32_t memMax { 0 };
int32_t memAvg { 0 };
#define TIME_OUT_MAX 500
#define TIME_OUT_INVALID 0
#define MEX_MAX 40
#define MEM_INVALID 0
std::map<std::shared_ptr<IInputEventHandler>, int32_t> timeOutPlugin;
std::map<std::shared_ptr<IInputEventHandler>, int32_t> memPlugin;

bool CheckFileExtendName(const std::string &filePath, const std::string &checkExtension)
{
    std::string::size_type pos = filePath.find_last_of('.');
    if (pos == std::string::npos) {
        MMI_HILOGE("File is not find extension");
        return false;
    }
    return (filePath.substr(pos + 1, filePath.npos) == checkExtension);
}


} // namespace

void InputEventHandlerPluginMgr::StartWatchPluginDir()
{
    ReadPluginDir(INPUT_EVENT_HANDLER_PLUGIN_HOME);
    ReadPluginDir(INPUT_EVENT_HANDLER_PLUGIN_USER);
    for (auto it = pluginInfoList.begin(); it != pluginInfoList.end(); it++) {
        bool ret = LoadPlugin(it->second.osPath, it->first, true);
        if (!ret) {
            MMI_HILOGE("Plugin %{public}s open error", it->second.osPath.data());
        }
    }
}

void InputEventHandlerPluginMgr::ReadPluginDir(const std::string pluginPath)
{
    DIR* dir = opendir(pluginPath.c_str());
    if (dir == nullptr) {
        MMI_HILOGE("Open plugin home(%{public}s) failed. errno: %{public}d",pluginPath.data(), errno);
        return;
    }
    dirent* p = nullptr;
    while ((p = readdir(dir)) != nullptr) {
        if (p->d_type != DT_REG) {
            continue;
        }
        char realPath[PATH_MAX] = {};
        std::string tmpPath = pluginPath + p->d_name;
        if (realpath(tmpPath.data(), realPath) == nullptr) {
            continue;
        }
        if (pluginInfoList.find(p->d_name) == pluginInfoList.end()) {
            inputEventHandlerPlugin pluginInfo;
            pluginInfo.osPath = realPath;
            pluginInfoList[p->d_name] = pluginInfo;
            continue;
        } else {
            pluginInfoList[p->d_name].osPath = realPath;
            continue;
        }
    }
    auto ret = closedir(dir);
    if (ret != 0) {
        MMI_HILOGE("Closedir failed, dirname:%{public}s, errno:%{public}d", pluginPath.data(), errno);
    }
}

bool InputEventHandlerPluginMgr::LoadPlugin(std::string pluginPath, std::string pluginName, bool initStatus)
{
    if (!CheckFileExtendName(pluginPath, "so")) {
        MMI_HILOGE("File %{public}s is not .so", pluginName.data())
        return false;
    }
    void *handle = dlopen(pluginPath.data(), RTLD_NOW);
    if(handle == nullptr){
        MMI_HILOGE("Open plugin failed, so name:%{public}s, msg:%{public}s", pluginPath.data(), dlerror());
        return false;
    }
    GetPlugin* getPlugin = (GetPlugin*) dlsym(handle, "create");
    auto error = dlerror();
    if(error != NULL) {
        MMI_HILOGE("Dlsym msg:%{public}s", error);
        dlclose(handle);
        return false;
    }
    auto plugin = getPlugin();
    if (!plugin) {
        MMI_HILOGE("Plugin cerate error");
        return false;
    }
    auto context = std::make_shared<PluginContext>();
    context_.push_back(context);
    if (plugin->Init(context_.back().get())) {
        pluginInfoList[pluginName].pluginHandler = context_.back()->GetEventHandler();
        context_.back()->inputDeviceMgr_ = inputDevMgr_;
        context_.back()->pluginName_ = pluginName;
    }
    pluginInfoList[pluginName].loadStatus = true;
    pluginInfoList[pluginName].osHandler = handle;
    pluginInfoList[pluginName].plugin = plugin;
    if (!initStatus) {
        InputHandler->Insert(pluginInfoList[pluginName].pluginHandler);
    }
    return true;
}

void InputEventHandlerPluginMgr::UnloadPlugin(std::string pluginName)
{
    if (pluginInfoList.find(pluginName) == pluginInfoList.end()) {
        MMI_HILOGE("Not find plugin %{plugin}s", pluginName.data());
        return;
    }
    InputHandler->Remove(pluginInfoList[pluginName].pluginHandler);
    for (auto it = context_.begin(); it != context_.end(); ++it) {
        if ((*it)->GetEventHandler() == pluginInfoList[pluginName].pluginHandler) {
            (*it)->SetEventHandler(nullptr);
            it = context_.erase(it);
            break;
        }
    }
    pluginInfoList.erase(pluginName);
}

void InputEventHandlerPluginMgr::DelPlugin(std::shared_ptr<IInputEventHandler> pluginHandler)
{
    for (auto &item : pluginInfoList) {
        if (item.second.pluginHandler == pluginHandler) {
            UnloadPlugin(item.first);
            return;
        }
    }
}

bool InputEventHandlerPluginMgr::InitINotify()
{
    int32_t fd = inotify_init();
    if (fd < 0) {
        MMI_HILOGE("Plugin initalize inotify failed");
        return false;
    }

    int wd = inotify_add_watch(fd, INPUT_EVENT_HANDLER_PLUGIN_USER.data(), IN_ALL_EVENTS);
    if(wd < 0) {
        MMI_HILOGE("Add directory watch failed");
        return false;
    }
    fd_ = fd;
    wd_ = wd;
    return true;
}

int32_t InputEventHandlerPluginMgr::GetReadFd()
{
    return fd_;
}

void InputEventHandlerPluginMgr::StopINotify()
{
    inotify_rm_watch(fd_, wd_);
    close(fd_);
}

void InputEventHandlerPluginMgr::SetDeivceManager(std::shared_ptr<IInputDeviceManager> inputDeviceMgr)
{
    inputDevMgr_ = inputDeviceMgr;
}

void InputEventHandlerPluginMgr::OnTimer()
{
    auto timeout = GetSysClockTime() + 3000;
    for (auto &item: context_) {
        item->OnReport(max, avg, memMax, memAvg);
        if (timeOutPlugin.find(item->GetEventHandler()) == timeOutPlugin.end()) {
            timeOutPlugin[item->GetEventHandler()] = 0;
            memPlugin[item->GetEventHandler()] = 0;
        }
        if (max > TIME_OUT_MAX) {
            timeOutPlugin[item->GetEventHandler()] = timeOutPlugin[item->GetEventHandler()] + 1;
            if (timeOutPlugin[item->GetEventHandler()] > 10) {
                DelPlugin(item->GetEventHandler());
            }
        } else {
            if (max > TIME_OUT_INVALID) {
                timeOutPlugin[item->GetEventHandler()] = 0;
            }
        }

        if (memMax > MEX_MAX) {
            memPlugin[item->GetEventHandler()] = memPlugin[item->GetEventHandler()] + 1;
            if (memPlugin[item->GetEventHandler()] > 10) {
                DelPlugin(item->GetEventHandler());
            }
        } else {
            if (memMax > MEM_INVALID) {
                memPlugin[item->GetEventHandler()] = 0;
            }
        }
        if (timeout > GetSysClockTime()) {
            return;
        }
    }
}
} // namespace MMI
} // namespace OHOS