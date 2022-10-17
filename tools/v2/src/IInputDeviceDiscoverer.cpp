#include "IInputDeviceDiscoverer.h"
#include "IInputDevice.h"
#include "LinuxInputDeviceDiscoverer.h"
#include "Log.h"

namespace Input {
    std::list<std::shared_ptr<IInputDeviceDiscoverer>> IInputDeviceDiscoverer::Create(const IInputContext* context)
    {
        std::list<std::shared_ptr<IInputDeviceDiscoverer>> result;
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return result;
        }

        auto deviceDiscoverer = LinuxInputDeviceDiscoverer::CreateInstance(context);
        if (!deviceDiscoverer) {
            LOG_W("Leave, LinuxInputDeviceDiscoverer CreateInstance Failed");
        } else {
            result.push_back(deviceDiscoverer);
        }

        return result;
    }
}
