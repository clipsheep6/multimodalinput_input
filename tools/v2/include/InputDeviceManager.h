#pragma once
#include <memory>
#include <map>

#include "IInputDefine.h"
#include "IInputDeviceManager.h"

namespace Input {

class IInputContext;
class ISeatManager;
class InputDeviceManager : public NonCopyable, public IInputDeviceManager {
    public:
        static std::unique_ptr<InputDeviceManager> CreateInstance(IInputContext* context);

    public:
        virtual ~InputDeviceManager();
        virtual std::shared_ptr<IInputDevice> GetDevice(int32_t id) const override;
        virtual std::list<int32_t> GetDeviceIdList() const override;

        virtual bool AddDevice(const std::shared_ptr<IInputDevice>& device) override;
        virtual std::shared_ptr<IInputDevice> RemoveDevice(int32_t id) override;
        virtual std::shared_ptr<IInputDevice> RemoveDevice(const std::string& deviceFile) override;

    protected:
        InputDeviceManager(IInputContext* context);

    private:
        const std::unique_ptr<ISeatManager>& GetSeatManager() const;
        void NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device);
        void NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device);

    private:
        IInputContext* const context_;
        std::map<int32_t, std::shared_ptr<IInputDevice>> inputDevices_;
        static const int32_t MAX_INPUT_DEVICE {1024};
};

}


