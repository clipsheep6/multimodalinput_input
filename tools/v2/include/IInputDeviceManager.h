#pragma once

#include <memory>
#include <list>

#include "IInputDevice.h"

namespace Input {

class IInputContext;
class IInputDeviceManager {
    public:
        static std::unique_ptr<IInputDeviceManager> CreateInstance(IInputContext* context);

        virtual ~IInputDeviceManager() = default;
        virtual std::shared_ptr<IInputDevice> GetDevice(int32_t id) const = 0;
        virtual std::list<int32_t> GetDeviceIdList() const = 0;

        virtual bool AddDevice(const std::shared_ptr<IInputDevice>& device) = 0;
        virtual std::shared_ptr<IInputDevice> RemoveDevice(int32_t id) = 0;
        virtual std::shared_ptr<IInputDevice> RemoveDevice(const std::string& deviceFile) = 0;
};

}
