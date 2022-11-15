#include "plugin_manager.h"

#include <dlfcn.h>
#include <dirent.h>
#include "util.h"

#include "input_event_handler_plugin.h"
#include "i_input_event_handler_plugin.h"
#include "i_input_event_handler.h"
namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "EventPluginsHandler"};
const std::string INPUT_EVENT_HANDLER_PLUGIN_HOME = "/system/lib/module/multimodalinput/plugins/";
const std::string INPUT_EVENT_HANDLER_PLUGIN_USER = "/data/user/plugins/";//plugin的移除和添加
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

void InputEventHandlerPluginMgr::PluginErgodic()
{
    ReadPlugin(INPUT_EVENT_HANDLER_PLUGIN_USER); //优先加载用户定制插件
    ReadPlugin(INPUT_EVENT_HANDLER_PLUGIN_HOME);
}

void InputEventHandlerPluginMgr::StartWatchUserPluginDir()
{
    WatchPlugin(INPUT_EVENT_HANDLER_PLUGIN_USER);
}

void InputEventHandlerPluginMgr::ReadPluginDir(const std::string pluginPath)
{
    DIR* dir = opendir(pluginPath.c_str());
    if (dir == nullptr) {
        MMI_HILOGE("open plugin home(%{public}s) failed. errno: %{public}d",pluginPath.data(), errno);
        return;
    }
    dirent* p = nullptr;
    while ((p = readdir(dir)) != nullptr) {
        if (p->d_type != DT_REG) {
            continue;
        }
        char realPath[PATH_MAX] = {};

        if (realpath((pluginPath + p->d_name).data(), realPath) == nullptr) {
            MMI_HILOGE("Path is error, path:%{public}s", p->d_name);
            continue;
        }
        if (!CheckFileExtendName(realPath, "so")) {
            continue;
        }
        int32_t ret = LoadPlugin(realPath, p->d_name);
    }
    auto ret = closedir(dir);
    if (ret != 0) {
        MMI_HILOGE("closedir failed, dirname:%{public}s, errno:%{public}d", INPUT_EVENT_HANDLER_PLUGIN_HOME.data(), errno);
    }
}

int32_t InputEventHandlerPluginMgr::UnloadPlugins()
{
    auto it = pluginInfoList.begin();
    while (it != pluginInfoList.end()) {
        dlclose(it->second);
        it++;
    }
    pluginInfos_.clear();
    return RET_OK;
}

int32_t InputEventHandlerPluginMgr::LoadPlugin(std::string &pluginPath,std::string pluginName)
{

    void *handle = dlopen(pluginPath, RTLD_NOW);
    if(handle == nullptr){
        MMI_HILOGE("open plugin failed, soname:%{public}s, msg:%{public}s", pluginPath, dlerror());
        continue;
    }
    if(ret != RET_OK){
        auto retClose = dlclose(handle);
        MMI_HILOGE("load plugin failed, soname:%{public}s, ret:%{public}d. retClose:%{public}d", p->d_name, ret, retClose);
        continue;
    }
    GetPlugin* getPlugin = (GetPlugin*) dlsym(handle, "creat");
    if(dlerror()) {
        dlclose(handle);
    }
    Iplugin* plugin = getPlugin();

    plugin->init(mmi_service);

    inputEventHandlerPlugin pluginInfo;
    pluginInfo.loadStatus = true;
    pluginInfo.pluginName = pluginName;
    pluginInfo.plugin = handler;
    pluginInfoList[pluginPath] = pluginInfo;
    return RET_OK;
}

void InputEventHandlerPluginMgr::UnloadPlugin(std::string pluginName)
{

    ReleasePlugin* relessePlugin = (ReleasePlugin*) dlsym(pluginInfoList[pluginName].plugin, "Release");
    relessePlugin(handle);
    dlclose(handle);
    pluginInfoList[pluginName].loadStatus = false;
}

} // namespace MMI
} // namespace OHOS