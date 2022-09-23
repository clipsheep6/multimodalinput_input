#include "event_plugins_handler.h"

#include <dlfcn.h>
#include <dirent.h>
#include "util.h"

#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventPluginsHandler"};
const std::string INPUT_EVENT_HANDLER_PLUGIN_HOME = "/data/mmi/plugins/";
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

int32_t EventPluginsHandler::ScanPlugins()
{
        DIR* dir = opendir(INPUT_EVENT_HANDLER_PLUGIN_HOME.c_str());
        if (dir == nullptr) {
            MMI_HILOGE("open plugin home(%{public}s) failed. errno: %{public}d",INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
            return RET_ERR;
        }
        dirent* p = nullptr;
        while ((p = readdir(dir)) != nullptr) {
            if (p->d_type != DT_REG) {
                continue;
            }
            char realPath[PATH_MAX] = {};
            if (realpath(p->d_name, realPath) == nullptr) {
                MMI_HILOGE("Path is error, path:%{public}s", p->d_name);
                continue;
            }
            if (!CheckFileExtendName(p->d_name, "so")) {
                continue;
            }
            void *handle = dlopen(realPath, RTLD_NOW);
            if(handle == nullptr){
                MMI_HILOGE("open plugin failed, soname:%{public}s, msg:%{public}s", p->d_name, dlerror());
                continue;
            }
            int32_t ret = LoadPlugin(handle);
            if(ret != RET_OK){
                auto retClose = dlclose(handle);
                MMI_HILOGE("load plugin failed, soname:%{public}s, ret:%{public}d. retClose:%{public}d", p->d_name, ret, retClose);
                continue;
            }
        }
        auto ret = closedir(dir);
        if (ret != 0) {
            MMI_HILOGE("closedir failed, dirname:%{public}s, errno:%{public}d", INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
        }
        auto cur = pluginInfos_.begin();
        while (cur != pluginInfos_.end()) {
            auto next = cur++;
            if (next != pluginInfos_.end()) {
                (*cur)->handler->SetNext((*next)->handler);
            }
            cur = next;
        }

        return RET_OK;
}

int32_t EventPluginsHandler::UnloadPlugins()
{
    for (auto iter = pluginInfos_.begin(); iter != pluginInfos_.end(); ++iter){
        //UnloadPlugin(iter->second);
        //dlclose(iter->second);
    }
    pluginInfos_.clear();
    return RET_OK;
}

#if 0
int32_t EventPluginsHandler::UnloadPlugin(void *handle)
{
    UnloadPluginFunc* unloadFunc = (LoadPluginFunc*)dlsym(handle, "UnloadPlugin");
    const char* retError = dlerror();
    if (retError != nullptr) {
        MMI_HILOGE("unload plugin failed, error msg:%{public}s", retError);
        return RET_ERR;
    }
    int32_t ret = (*unloadFunc)(*info);
    if (ret != RET_OK) {
        MMI_HILOGE("call unload plugin func failed. ret:%{public}d", ret);
        return RET_ERR;
    }
    for (auto it = pluginInfos_.begin(); it != pluginInfos_.end(); ++it) {
        if ((*it)->priority > info->priority) {
            (void)pluginInfos_.insert(it, info);
            break;
        }
    }
    return RET_OK;
}
#endif

int32_t EventPluginsHandler::LoadPlugin(void *handle)
{
    LoadPluginFunc* loadFunc = (LoadPluginFunc*)dlsym(handle, "LoadPlugin");
    const char* retError = dlerror();
    if (retError != nullptr) {
        MMI_HILOGE("load plugin failed, error msg:%{public}s", retError);
        return RET_ERR;
    }
    PluginInfo *info = new PluginInfo;
    int32_t ret = (*loadFunc)(*info);
    if (ret != RET_OK) {
        MMI_HILOGE("call load plugin func failed. ret:%{public}d", ret);
        return RET_ERR;
    }
    for (auto it = pluginInfos_.begin(); it != pluginInfos_.end(); ++it) {
        if ((*it)->priority > info->priority) {
            (void)pluginInfos_.insert(it, info);
            break;
        }
    }
    return RET_OK;
}

void EventPluginsHandler::SetNext(std::shared_ptr<IInputEventHandler> nextHandler)
{
    MMI_HILOGE("3333333333333333333333333333333333333333333333333333");
    nextHandler_ = nextHandler;
    if (pluginInfos_.empty()) {
        return;
    }

    pluginInfos_.back()->handler->SetNext(nextHandler);
}

template<typename T1, typename T2>
void EventPluginsHandler::HandlePluginEventEx(std::shared_ptr<IInputEventConvertHandler> handler, const std::shared_ptr<T1> event)
{
    MMI_HILOGE("44444444444444444444444444");
    handler->HandleEvent<T1, T2>(event);

    IInputEventConvertHandler::PluginDispatchCmd cmd = handler->GetDispatchCmd();
    switch(cmd) {
        case IInputEventConvertHandler::PluginDispatchCmd::GOTO_NEXT: {
            auto nextHandler = handler->GetNextHandler();
            auto newEvent = handler->GetEvent<T1, T2>();
            nextHandler->HandleEvent<T1, T2>(newEvent);
            break;
        }
        case IInputEventConvertHandler::PluginDispatchCmd::REDIRECT: {
            IInputEventConvertHandler::PluginDispatchEventType eventType = IInputEventConvertHandler::PluginDispatchEventType::KEY_EVENT; //handler->GetEventType();
            if (eventType == IInputEventConvertHandler::PluginDispatchEventType::KEY_EVENT) {
                auto keyEvent = handler->GetKeyEvent();
                InputHandler->GetEventNormalizeHandler()->HandleKeyEvent(keyEvent);
            } else if (eventType == IInputEventConvertHandler::PluginDispatchEventType::POINT_EVENT) {
                auto pointEvent = handler->GetPointEvent();
                InputHandler->GetEventNormalizeHandler()->HandlePointerEvent(pointEvent);
            } else if (eventType == IInputEventConvertHandler::PluginDispatchEventType::TOUCH_EVENT) {
                auto pointEvent = handler->GetPointEvent();
                InputHandler->GetEventNormalizeHandler()->HandleTouchEvent(pointEvent);
            } else {
                MMI_HILOGE("unsupport eventType: %{public}d", int32_t(eventType));
            }
            break;
        }
        case IInputEventConvertHandler::PluginDispatchCmd::DISCARD:{
            MMI_HILOGD("no need process. eventId: %{public}d", event->GetId());
            break;
        }
    }
}

template<typename T1, typename T2>
void EventPluginsHandler::HandlePluginEvent(const std::shared_ptr<T1> event)
{
    if (pluginInfos_.empty()) {
        GetNextHandler()->HandleEvent<T1, T2>(event);
        return;
    }

    auto handler = (*pluginInfos_.begin())->handler;
    HandlePluginEventEx<T1, T2>(handler, event);
}

EventPluginsHandler::EventPluginsHandler()
{
    MMI_HILOGE("2222222222222222222222222222222222222222222222");
    auto ret = ScanPlugins();
    if (ret != RET_OK) {
        MMI_HILOGE("ScanPlugins failed, ret:%{public}d", ret);
    }

}

void EventPluginsHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> event)
{
    if (pluginInfos_.empty()) {
        GetNextHandler()->HandleKeyEvent(event);
        return;
    }
    HandlePluginEvent<KeyEvent, PluginDispatchKeyEvent>(event);
}

void EventPluginsHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> event)
{
    HandlePluginEvent<PointerEvent, PluginDispatchPointEvent>(event);
}

void EventPluginsHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> event)
{
    HandlePluginEvent<PointerEvent, PluginDispatchTouchEvent>(event);
}

} // namespace MMI
} // namespace OHOS