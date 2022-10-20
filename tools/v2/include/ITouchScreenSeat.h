#pragma once
#include <memory>

#include "AbsEvent.h"
#include "IInputDevice.h"

namespace Input {

    class IInputContext;
    class PhysicalDisplayState;
    class LogicalDisplayState;

    class ITouchScreenSeat {
        public:
            static std::unique_ptr<ITouchScreenSeat> CreateInstance(const IInputContext* contex, 
                    const std::string& seatId, const std::string& seatName);

            static std::shared_ptr<ITouchScreenSeat> GetDefault();

            virtual ~ITouchScreenSeat() = default;

            virtual const std::string& GetSeatId() const = 0;
            virtual const std::string& GetSeatName() const = 0;
            virtual bool IsEmpty() const = 0;

            virtual int32_t BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual int32_t UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual std::shared_ptr<IInputDevice> GetInputDevice() const = 0;

            virtual int32_t BindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            virtual int32_t UnbindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            virtual int32_t UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            virtual std::shared_ptr<PhysicalDisplayState> GetDisplay() const = 0;

            virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) = 0;

            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
    };

}
