#include "ITouchScreenSeat.h"
#include "Log.h"

namespace Input {
    class DefaultSeat : public ITouchScreenSeat {
        virtual int32_t BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override
        {
            return 0;
        }

        virtual int32_t UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override
        {
            return 0;
        }

        virtual std::shared_ptr<IInputDevice> GetInputDevice() const override
        {
            return nullptr;
        }

        virtual bool IsEmpty() const override
        {
            return true;
        }

        virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override
        {
            LOG_D("Enter");
            LOG_D("Leave");
        }

        virtual int32_t BindDisplay(const std::shared_ptr<PhysicalDisplayState>& display)  override
        {
            return -1;
        }

        virtual int32_t UnbindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) override
        {
            return -1;
        }

        virtual int32_t UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& display) override
        {
            return -1;
        }

        virtual std::shared_ptr<PhysicalDisplayState> GetDisplay() const override
        {
            return nullptr;
        }

        virtual const std::string& GetSeatId() const override
        {
            return seatId_;
        }

        virtual const std::string& GetSeatName() const override
        {
            return seatName_;
        }

        virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) override
        {
        }

        virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) override
        {
        }

        virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) override
        {
        }

        private:
        const std::string seatId_{"seat0"};
        const std::string seatName_{"default0"};
    };


    std::shared_ptr<ITouchScreenSeat> ITouchScreenSeat::GetDefault()
    {
        static std::shared_ptr<ITouchScreenSeat> seat;
        if (!seat) {
            seat = std::make_shared<DefaultSeat>();
        }
        return seat;
    }
}
