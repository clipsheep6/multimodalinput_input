#pragma once

#include <string>
#include <map>
#include <list>

#include "IInputDefine.h"
#include "ISeatManager.h"

namespace Input {

    class IInputContext;
    class SeatManager : public NonCopyable, public ISeatManager {
        public:
            static std::unique_ptr<SeatManager> CreateInstance(const IInputContext* context);
        public:
            virtual ~SeatManager() = default;

            virtual void OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice) override;
            virtual void OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice) override;

            virtual void OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display) override;
            virtual void OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display) override;
            virtual void OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display) override;

            virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) override;
            virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) override;
            virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) override;

        private:
            SeatManager(const IInputContext* context);

            std::shared_ptr<ITouchScreenSeat> FindTouchScreenSeat(const std::shared_ptr<PhysicalDisplayState>& display,
                    bool createIfNotExist);
            std::shared_ptr<ITouchScreenSeat> FindTouchScreenSeat(const std::string& seatId, 
                    const std::string& seatName, bool createIfNotExist);

            std::shared_ptr<ISeat> FindSeat(std::string seatId, bool createIfNotExist);

            void OnTouchScreenRemoved(const std::shared_ptr<IInputDevice>& inputDevice);

            void RemoveSeat(const std::shared_ptr<ISeat>& seat);
            void RemoveSeat(const std::shared_ptr<ITouchScreenSeat>& seat);

        private:
            const IInputContext* const context_;
            std::list<std::shared_ptr<ISeat>> seats_;
            std::list<std::shared_ptr<ITouchScreenSeat>> touchScreenSeats_;
    };

}
