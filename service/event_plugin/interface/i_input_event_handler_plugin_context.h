#ifndef I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H
#define I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H

namespace OHOS {
namespace MMI {

class IInputEventPluginContext
{
public:
    virtual ~IInputEventPluginContext() = 0;
    virtual void SetEventHandler(std::shared_ptr<IInputEventHandler> handler) = 0;
    virtual IInputDeviceManager* GetInputDeviceManager();
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H