#pragma once
#include <memory>
#include <list>

#include "IKernelEventHandler.h"
#include "LogicalDisplayState.h"

namespace Input {

    class IInputContext;
    class IInputDevice;

    class ISeat {
        public:
            static std::unique_ptr<ISeat> CreateInstance(const IInputContext* inputContext, const std::string& seatId);

            virtual ~ISeat() = default;

            virtual const std::string& GetSeatId() const = 0;

            virtual int32_t AddDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual int32_t RemoveDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual int32_t UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual std::list<std::shared_ptr<LogicalDisplayState>> GetDisplays() const = 0;

            virtual int32_t AddInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual int32_t RemoveInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual std::list<std::shared_ptr<IInputDevice>> GetInputDevices() const = 0;

            virtual bool IsEmpty() const = 0;

            virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) = 0;
            virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) = 0;
            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
    };

}
