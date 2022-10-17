#pragma once
#include <memory>
#include <string>

namespace Input {
    class IInputContext;
    class ISeat;
    class ITouchScreenSeat;
    class IInputDevice;
    class PhysicalDisplayState;
    class LogicalDisplayState;

    class ISeatManager {
        public:
            static std::unique_ptr<ISeatManager> CreateInstance(const IInputContext* context);
            static const std::unique_ptr<ISeatManager> NULL_VALUE;

            virtual ~ISeatManager() = default;

            virtual void OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual void OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice) = 0;

            virtual void OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            virtual void OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            virtual void OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display) = 0;

            virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) = 0;

    };
}
