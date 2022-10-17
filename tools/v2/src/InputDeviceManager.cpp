#include <cstring>

#include <dirent.h>

#include "InputDeviceManager.h"
#include "IoUtils.h"
#include "Log.h"
#include "InputDevice.h"
#include "ISeatManager.h"
#include "InputErrorCode.h"

namespace Input {

    std::unique_ptr<InputDeviceManager> InputDeviceManager::CreateInstance(IInputContext* context) 
    {
        if (context == nullptr) {
            errno = EINVAL;
            return nullptr;
        }
        return std::unique_ptr<InputDeviceManager>(new InputDeviceManager(context));
    }

    InputDeviceManager::InputDeviceManager(IInputContext* context)
        : context_(context)
    {
    }

    InputDeviceManager::~InputDeviceManager() 
    {
    }

    std::shared_ptr<IInputDevice> InputDeviceManager::GetDevice(int32_t id) const
    {
        auto it = inputDevices_.find(id);
        if (it == inputDevices_.end()) {
            return nullptr;
        }
        return it->second;
    }

    std::list<int32_t> InputDeviceManager::GetDeviceIdList() const
    {
        std::list<int32_t> result;
        for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ++it) {
            result.push_back(it->first);
        }
        return result;
    }

    bool InputDeviceManager::AddDevice(const std::shared_ptr<IInputDevice>& device)
    {
        LOG_D("Enter");
        if (!device) {
            LOG_E("Leave, null device");
            return false;
        }

        auto id = device->GetId();
        if (id < 0) {
            LOG_E("Leave, id < 0");
            return false;
        }

        if (GetDevice(id)) {
            LOG_E("Leave, repeat id");
            return false;
        }

        inputDevices_[id] = device;

        NotifyDeviceAdded(device);
        LOG_D("Leave");
        return true;
    }

    std::shared_ptr<IInputDevice> InputDeviceManager::RemoveDevice(int32_t id)
    {
        LOG_D("Enter id:$s", id);
        auto it = inputDevices_.find(id);
        if (it == inputDevices_.end()) {
            LOG_E("Leave id:$s not exist device", id);
            return nullptr;
        }

        std::shared_ptr<IInputDevice> device = it->second;
        inputDevices_.erase(it);

        NotifyDeviceRemoved(device);

        LOG_D("Leave id:$s", id);
        return device;
    }

    std::shared_ptr<IInputDevice> InputDeviceManager::RemoveDevice(const std::string& deviceFile)
    {
        LOG_D("Enter deviceFile:$s", deviceFile);
        for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ) {
            std::shared_ptr<IInputDevice> inputDevice = it->second;
            if (!inputDevice) {
                LOG_W("null inputDevice, remove it");
                it = inputDevices_.erase(it);
                continue;
            }

            if (inputDevice->GetDeviceFile() == deviceFile) {
                inputDevices_.erase(it);
                NotifyDeviceRemoved(inputDevice);
                LOG_D("Leave");
                return inputDevice;
            }

            ++it;
        }
        
        LOG_E("Leave deviceFile:$s", deviceFile);
        return nullptr;
    }

    const std::unique_ptr<ISeatManager>& InputDeviceManager::GetSeatManager() const
    {
        LOG_D("Enter");
        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return ISeatManager::NULL_VALUE;
        }

        const auto& seatManager = context_->GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return seatManager;
        }

        LOG_D("Leave");
        return seatManager;
    }

    void InputDeviceManager::NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device)
    {
        LOG_D("Enter");
        if (!device) {
            LOG_E("Leave, null device");
            return;
        }

        const auto& seatManager = context_->GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnInputDeviceAdded(device);
    }

    void InputDeviceManager::NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device)
    {
        LOG_D("Enter");
        if (!device) {
            LOG_E("Leave, null device");
            return;
        }

        const auto& seatManager = context_->GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnInputDeviceRemoved(device);
    }

}
