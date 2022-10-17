#pragma once

#include <memory>
#include <list>

namespace Input {

    class IInputDevice;
    class IInputContext;
    class IInputDeviceDiscoverer {
        public:
            static std::list<std::shared_ptr<IInputDeviceDiscoverer>> Create(const IInputContext* context);

            virtual ~IInputDeviceDiscoverer() = default;
    };
};
