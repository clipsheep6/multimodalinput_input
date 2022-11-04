#include "event_plugins_handler.h"

#include <dlfcn.h>
#include <dirent.h>
#include "util.h"

#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventPluginsHandler"};
/*
两个目录：
1，系统目录
2，定制目录（同名定制与系统目录，优先加载定制目录）
*/
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
std::map<IInputEventConvertHandler::PluginfunctionId, bool> PluginFunctionStatus = {
    {IInputEventConvertHandler::PluginfunctionId::PHALANGEAL_JOINT, true}
};
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
        openPlugins_.push_back(handle);
    }
    auto ret = closedir(dir);
    if (ret != 0) {
        MMI_HILOGE("closedir failed, dirname:%{public}s, errno:%{public}d", INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
    }
    auto cur1 = pluginInfos_.begin();
    for (auto cur = pluginInfos_.begin(); cur != pluginInfos_.end(); cur++) {
        (*cur1)->handler->SetNext((*cur)->handler);
        cur1 = cur;
    }
    return RET_OK;
}

int32_t EventPluginsHandler::UnloadPlugins()
{
    if (openPlugins_.empty()) {
        return RET_OK;
    }
    for (auto iter = openPlugins_.begin(); iter != openPlugins_.end(); ++iter){
        dlclose(*iter);
    }
    pluginInfos_.clear();
    openPlugins_.clear();
    return RET_OK;
}

class PluginContext
{

};

int32_t EventPluginsHandler::LoadPlugin(void *handle, std::shared_ptr<PluginContext> pluginContext)
{
    create_t* create_plu = (create_t*) dlsym(handle, "create");
    const char* retError = dlerror();
    if (retError != nullptr) {
        MMI_HILOGE("load plugin failed, error msg:%{public}s", retError);
        return RET_ERR;
    }
    IInputEventConvertHandler::PluginInfo *info = new IInputEventConvertHandler::PluginInfo;
    auto plugin = create_plu(pluginContext);
    info->handler = plugin->CreateHandler();
    info->handler = create_plu(pluginContext)->GetPluginInfo().handler;
    info->name = create_plu()->GetPluginInfo().name;
    info->version = create_plu()->GetPluginInfo().version;
    info->priority = create_plu()->GetPluginInfo().priority;
    if (pluginInfos_.empty()) {
        pluginInfos_.push_front(info);
    } else {
        for (auto it = pluginInfos_.begin(); it != pluginInfos_.end(); ++it) {
            if ((*it)->priority > info->priority) {
                pluginInfos_.insert(it, info);
                break;
            } else  {
                if ((*it)->priority == pluginInfos_.back()->priority) {
                    pluginInfos_.push_back(info);
                    break;
                }
            }
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
    handler->SetPluginfunctionStatus(PluginFunctionStatus);
    handler->HandleEvent<T1, T2>(event);
    IInputEventConvertHandler::PluginDispatchCmd cmd = handler->GetDispatchCmd();
    auto nextHandler = std::static_pointer_cast<IInputEventConvertHandler>(handler->GetNextHandler());
    switch(cmd) {
        case IInputEventConvertHandler::PluginDispatchCmd::REDIRECT: {
            IInputEventConvertHandler::PluginDispatchEventType eventType = handler->GetDispatchEventType();
            if (eventType == IInputEventConvertHandler::PluginDispatchEventType::KEY_EVENT) {
                auto keyEvent = handler->GetKeyEvent();
                if (nextHandler->GetisPlugin()) {
                    HandlePluginEventEx<KeyEvent, PluginDispatchKeyEvent>(nextHandler, keyEvent);
                } else {
                    nextHandler->HandleEvent<KeyEvent, PluginDispatchKeyEvent>(keyEvent);
                }
            } else if (eventType == IInputEventConvertHandler::PluginDispatchEventType::POINT_EVENT) {
                auto pointEvent = handler->GetPointEvent();
                if (nextHandler->GetisPlugin()) {
                    HandlePluginEventEx<PointerEvent, PluginDispatchPointEvent>(nextHandler, pointEvent);
                }  else {
                    nextHandler->HandleEvent<PointerEvent, PluginDispatchPointEvent>(pointEvent);
                }
            } else if (eventType == IInputEventConvertHandler::PluginDispatchEventType::TOUCH_EVENT) {
                auto pointEvent = handler->GetPointEvent();
                if (nextHandler->GetisPlugin()) {
                    HandlePluginEventEx<PointerEvent, PluginDispatchTouchEvent>(nextHandler, pointEvent);
                }  else {
                    nextHandler->HandleEvent<PointerEvent, PluginDispatchTouchEvent>(pointEvent);
                }
                // InputHandler->GetEventNormalizeHandler()->HandleTouchEvent(pointEvent);
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
    HandlePluginEventEx<T1, T2>((*pluginInfos_.begin())->handler, event);
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

int32_t EventPluginsHandler::SetPhalangealJointStatue(bool joint)
{
    PluginFunctionStatus[IInputEventConvertHandler::PluginfunctionId::PHALANGEAL_JOINT] = joint;
    return RET_OK;
}

bool EventPluginsHandler::GetPhalangealJointStatue()
{
    return PluginFunctionStatus[IInputEventConvertHandler::PluginfunctionId::PHALANGEAL_JOINT];
}

} // namespace MMI
} // namespace OHOS