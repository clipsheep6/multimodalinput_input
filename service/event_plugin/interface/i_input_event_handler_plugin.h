#ifndef I_INPUT_EVENT_HANDLER_PLUGIN_H
#define I_INPUT_EVENT_HANDLER_PLUGIN_H

#include "i_input_event_handler_plugin_context.h"
#include "i_input_event_handler.h"


namespace OHOS {
namespace MMI {
class IPlugin
{
    virtual bool Init(IInputEventPluginContext *context) = 0;
    virtual void Uninit() = 0;
};
typedef IPlugin* GetPlugin();
typedef void ReleasePlugin(IPlugin* &plugin);

} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_PLUGIN_H