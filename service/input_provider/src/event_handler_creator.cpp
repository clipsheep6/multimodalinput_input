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
#include <memory>

#include "config_multimodal.h"
#include "event_handler_creator.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventHandlerCreator" };
}
std::shared_ptr<IEventHandler> EventHandlerCreator::CreateInstance(std::shared_ptr<IInputContext> context, int32_t capabilities)
{
    for (auto &item : soInfos_) {
        auto &[getIEventHandler, useCnt] = item.second;
        IEventHandlerStruct *s = nullptr;
        auto ret = getIEventHandler(context, capabilities, &s);
        if (!ret) {
            MMI_HILOGE("GetIEventHandler by capabilities return s nullptr, capabilities:%{public}d", capabilities);
            continue;
        }
        auto eventHandler = s->eventHandler;
        if (eventHandler == nullptr) {
            delete s;
            MMI_HILOGE("GetIEventHandler by capabilities return event handler nullptr, capabilities:%{public}d", capabilities);
            continue;
        }
        delete s;
        s = nullptr;
        eventHandlers_.emplace(std::make_pair(eventHandler, item.first));
        soInfos_.emplace(std::make_pair(item.first, std::make_tuple(getIEventHandler, useCnt + 1)));
        return eventHandler;
    }

    const std::string algo_dir =  DEF_EXP_SOPATH "module/multimodalinput/plugins/input_device_algo";
    DIR* dir = opendir(algo_dir.c_str());
    if (dir == nullptr) {
        MMI_HILOGE("Open device algo dir(%{public}s) failed. errno: %{public}d", algo_dir.data(), errno);
        return nullptr;
    }
    std::shared_ptr<IEventHandler> algo;
    dirent* p = nullptr;
    while ((p = readdir(dir)) != nullptr) {
        if (p->d_type != DT_REG) {
            continue;
        }
        char realPath[PATH_MAX] = {};
        std::string path = algo_dir + p->d_name;
        if (realpath(path.data(), realPath) == nullptr) {
            continue;
        }
        algo = LoadAndCheckInputDeviceAlgo(realPath, context, capabilities);
        if (algo == nullptr) {
            continue;
        }
        MMI_HILOGI("Load input device algo success, path:%{public}s, capabilities:%{public}d", realPath, capabilities);
        break;
    }
    auto ret = closedir(dir);
    if (ret != 0) {
        MMI_HILOGE("Closedir failed, dirname:%{public}s, errno:%{public}d", algo_dir.c_str(), errno);
    }
    return algo;
}

std::shared_ptr<IEventHandler> EventHandlerCreator::LoadAndCheckInputDeviceAlgo(std::string path, 
    std::shared_ptr<IInputContext> context, int32_t capabilities)
{
    void *handle = dlopen(path.c_str(), RTLD_NOW);
    if(handle == nullptr){
        MMI_HILOGE("Open plugin failed, so name:%{public}s, msg:%{public}s", path.c_str(), dlerror());
        return nullptr;
    }        
    GetIEventHandler* getIEventHandler = (GetIEventHandler*)dlsym(handle, "GetIEventHandlerImpl");
    if (getIEventHandler == nullptr) {
        auto error = dlerror();
        MMI_HILOGE("Dlsym msg:%{public}s", error);
        dlclose(handle);
        return nullptr;
    }
    IEventHandlerStruct *s = nullptr;
    auto ret = getIEventHandler(context, capabilities, &s);
    if (!ret) {
        MMI_HILOGE("GetIEventHandler by capabilities return s nullptr, capabilities:%{public}d", capabilities);
        dlclose(handle);
        return nullptr;
    }
    auto eventHandler = s->eventHandler;
    if (eventHandler == nullptr) {
        delete s;
        MMI_HILOGE("GetIEventHandler by capabilities return event handler nullptr, capabilities:%{public}d", capabilities);
        dlclose(handle);
        return nullptr;
    }
    delete s;
    s = nullptr;
    eventHandlers_.emplace(std::make_pair(eventHandler, handle));
    soInfos_.insert(std::make_pair(handle, std::make_tuple(getIEventHandler, 1)));
    return eventHandler;
}

void EventHandlerCreator::ReleaseInstance(std::shared_ptr<IEventHandler> handler)
{
    auto it = eventHandlers_.find(handler);
    if (it == eventHandlers_.end()) {
        MMI_HILOGE("Not found handler");
        return;
    }
    auto soHandle = it->second;
    eventHandlers_.erase(it);

    auto itSo = soInfos_.find(soHandle);
    if (itSo == soInfos_.end()) {
        MMI_HILOGE("Not found so handle");
        return;
    }
    auto &[getIEventHandler, useCnt] = itSo->second;
    useCnt--;
    if (useCnt < 0) {
        MMI_HILOGE("The var useCnt less than 0, reset to 0, useCnt:%{public}d", useCnt);
        useCnt = 0;
    }
    if (useCnt > 0) {
        MMI_HILOGI("The var useCnt more than 0, replace soInfos_, useCnt:%{public}d", useCnt);
        soInfos_.emplace(std::make_pair(soHandle, std::make_tuple(getIEventHandler, useCnt)));
        return;
    }
    dlclose(soHandle);
    soInfos_.erase(soHandle);
    MMI_HILOGI("Dll has unload");
}
} // namespace MMI
} // namespace OHOS