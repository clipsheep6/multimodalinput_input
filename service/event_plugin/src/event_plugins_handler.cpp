#include "event_plugins_handler.h"

#include <dlfcn.h>
#include <dirent.h>
#include "util.h"

#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventPluginsHandler"};
const std::string INPUT_EVENT_HANDLER_PLUGIN_HOME = "/system/lib/module/multimodalinput/plugins/";
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

            if (realpath((INPUT_EVENT_HANDLER_PLUGIN_HOME + p->d_name).data(), realPath) == nullptr) {
                MMI_HILOGE("Path is error, path:%{public}s", p->d_name);
                continue;
            }
            if (!CheckFileExtendName(realPath, "so")) {
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

        for (auto cur = pluginInfos_.begin(); cur != pluginInfos_.end(); cur++) {
                SetNext((*cur)->handler);
                MMI_HILOGE("1010101010101010101010101");
        }
        // auto cur = pluginInfos_.begin();

        // MMI_HILOGE("66666666666666666666666666");
        // while (cur != pluginInfos_.end()) {
        //     MMI_HILOGE("777777777777777777777777777777777");
        //     auto next = cur++;
        //     MMI_HILOGE("88888888888888888888888888888888888");
        //     if (next != pluginInfos_.end()) {
        //         MMI_HILOGE("9999999999999999999999999999999");
        //         SetNext((*next)->handler);
        //         MMI_HILOGE("1010101010101010101010101");
        //     }
        //     MMI_HILOGE("12121212121212121212221222112121212");
        //     cur = next;
        // }

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
    create_t* create_plu = (create_t*) dlsym(handle, "create");
    const char* retError = dlerror();
    if (retError != nullptr) {
        MMI_HILOGE("load plugin failed, error msg:%{public}s", retError);
        return RET_ERR;
    }
    IInputEventConvertHandler::PluginInfo *info = new IInputEventConvertHandler::PluginInfo;
    info->handler = create_plu()->GetPluginInfo().handler;
    info->name = create_plu()->GetPluginInfo().name;
    info->version = create_plu()->GetPluginInfo().version;
    info->priority = create_plu()->GetPluginInfo().priority;
    MMI_HILOGE("8888888888888888888888888888info->priority is %{public}d ", info->priority);
    pluginInfos_.push_front(info);
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
    nextHandler_ = nextHandler;
    if (pluginInfos_.empty()) {
        return;
    }

    pluginInfos_.back()->handler->SetNext(nextHandler);
}

template<typename T1, typename T2>
void EventPluginsHandler::HandlePluginEventEx(std::shared_ptr<IInputEventConvertHandler> handler, const std::shared_ptr<T1> event)
{
    handler->HandleEvent<T1, T2>(event);

    IInputEventConvertHandler::PluginDispatchCmd cmd = handler->GetDispatchCmd();
    MMI_HILOGE("44444444444444444444444444444444444444444444444 cmd====%{public}d" ,int32_t(cmd));
    switch(cmd) {
        case IInputEventConvertHandler::PluginDispatchCmd::GOTO_NEXT: {
            auto nextHandler = handler->GetNextHandler();
            auto newEvent = handler->GetEvent<T1, T2>();
            nextHandler->HandleEvent<T1, T2>(newEvent);
            break;
        }
        case IInputEventConvertHandler::PluginDispatchCmd::REDIRECT: {
            IInputEventConvertHandler::PluginDispatchEventType eventType = handler->GetDispatchEventType();
            MMI_HILOGE("999999999999999999999999999999999unsupport eventType: %{public}d", int32_t(eventType));
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