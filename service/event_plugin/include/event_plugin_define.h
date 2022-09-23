#include <iostream>

#include "define_multimodal.h"
#include "i_input_event_convert_handler.h"

typedef struct {
    std::string name;
    std::string version;
    int32_t priority;
    std::shared_ptr<OHOS::MMI::IInputEventConvertHandler> handler;
} PluginInfo;

extern "C" {
    typedef int32_t LoadPluginFunc(PluginInfo &);
    typedef int32_t UnloadPluginFunc(PluginInfo &);
    int32_t LoadPlugin(PluginInfo *&pluginInfo);
    int32_t UnloadPlugin(PluginInfo *pluginInfo);
};
