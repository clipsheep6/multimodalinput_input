#include <algorithm>

#include "SeatManager.h"
#include "IInputContext.h"
#include "IInputDevice.h"
#include "ISeat.h"
#include "ITouchScreenSeat.h"
#include "Log.h"
#include "RelEvent.h"
#include "KernelKeyEvent.h"
#include "KernelEventHandlerBridge.h"
#include "PhysicalDisplayState.h"
#include "LogicalDisplayState.h"

namespace Input {

    std::unique_ptr<SeatManager> SeatManager::CreateInstance(const IInputContext* context) {
        if (context == nullptr) {
            return nullptr;
        }

        return std::unique_ptr<SeatManager>(new SeatManager(context));
    }

    SeatManager::SeatManager(const IInputContext* context) 
        : context_(context) {
    }

    std::shared_ptr<ISeat> SeatManager::FindSeat(std::string seatId, bool createIfNotExist) {
        LOG_D("Enter seatId:$s createIfNotExist:$s", seatId, createIfNotExist);

        for (auto& seat : seats_) {
            if (seat->GetSeatId() == seatId) {
                return seat;
            }
        }

        std::shared_ptr<ISeat> result;
        if (!createIfNotExist) {
            LOG_D("Leave seatId:$s createIfNotExist:$s, Not Exist", seatId, createIfNotExist);
            return result;
        }

        result = ISeat::CreateInstance(context_, seatId);
        if (!result) {
            LOG_E("Leave seatId:$s createIfNotExist:$s, Create Failed", seatId, createIfNotExist);
            return result;
        }

        seats_.push_back(result);
        LOG_D("Leave seatId:$s createIfNotExist:$s", seatId, createIfNotExist);
        return result;
    }

    std::shared_ptr<ITouchScreenSeat> SeatManager::FindTouchScreenSeat(const std::shared_ptr<PhysicalDisplayState>& display,
            bool createIfNotExist)
    {
        if (!display) {
            LOG_E("Leave, null display");
            return nullptr;
        }

        return FindTouchScreenSeat(display->GetSeatId(), display->GetSeatName(), createIfNotExist);
    }

    std::shared_ptr<ITouchScreenSeat> SeatManager::FindTouchScreenSeat(const std::string& seatId, 
            const std::string& seatName, bool createIfNotExist)
    {
        LOG_D("Enter seatId:$s seatName:$s createIfNotExist:$s", seatId, seatName, createIfNotExist);

        for (auto& seat : touchScreenSeats_) {
            if (seat->GetSeatId() == seatId && seat->GetSeatName() == seatName) {
                return seat;
            }
        }

        std::shared_ptr<ITouchScreenSeat> result;
        if (!createIfNotExist) {
            LOG_D("Leave seatId:$s seatName:$s createIfNotExist:$s", seatId, seatName, createIfNotExist);
            return result;
        }

        result = ITouchScreenSeat::CreateInstance(context_, seatId, seatName);
        if (!result) {
            LOG_E("Leave seatId:$s seatName:$s createIfNotExist:$s, Create Failed", seatId, seatName, createIfNotExist);
            return result;
        }

        touchScreenSeats_.push_back(result);
        LOG_D("Leave seatId:$s seatName:$s createIfNotExist:$s", seatId, seatName, createIfNotExist);
        return result;
    }

    void SeatManager::OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice)
    {
        LOG_D("Enter");
        if (!inputDevice) {
            LOG_E("Leave, null inputDevice");
            return;
        }

        const auto& seatId = inputDevice->GetSeatId();
        const auto& seatName = inputDevice->GetSeatName();

        std::shared_ptr<ITouchScreenSeat> touchScreenSeat;
        if (inputDevice->HasCapability(IInputDevice::CAPABILITY_TOUCHSCREEN)) {
            touchScreenSeat = FindTouchScreenSeat(seatId, seatName, true);
            if (!touchScreenSeat) {
                LOG_E("Leave, null touchScreenSeat");
                return;
            }

            auto retCode = touchScreenSeat->BindInputDevice(inputDevice);
            if (retCode < 0) {
                RemoveSeat(touchScreenSeat);
                LOG_E("Leave, BindInputDevice Failed");
                return;
            }
        }

        auto seat = FindSeat(inputDevice->GetSeatId(), true);
        if (!seat) {
            if (touchScreenSeat) {
                touchScreenSeat->UnbindInputDevice(inputDevice);
                RemoveSeat(touchScreenSeat);
            }
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->AddInputDevice(inputDevice);
        if (retCode < 0) {
            if (touchScreenSeat) {
                touchScreenSeat->UnbindInputDevice(inputDevice);
                RemoveSeat(touchScreenSeat);
            }
            LOG_E("Leave, seat AddInputDevice Failed");
            return;
        }

        auto handler = KernelEventHandlerBridge::CreateInstance(seat, touchScreenSeat);
        if (!handler) {
            if (touchScreenSeat) {
                touchScreenSeat->UnbindInputDevice(inputDevice);
                RemoveSeat(touchScreenSeat);
            }

            seat->RemoveInputDevice(inputDevice);
            RemoveSeat(seat);
            LOG_E("Leave, null bridge handler");
            return;
        }

        inputDevice->StartReceiveEvents(handler);

        LOG_D("Leave");
    }

    void SeatManager::OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice)
    {
        LOG_D("Enter");
        if (!inputDevice) {
            LOG_E("Leave");
            return;
        }

        auto seat = FindSeat(inputDevice->GetSeatId(), false);
        if (!seat) {
            LOG_W("Leave, null seat");
        } else {
            auto retCode = seat->RemoveInputDevice(inputDevice);
            if (retCode < 0) {
                LOG_W("Leave, seat RemoveInputDevice Failed");
            }
            RemoveSeat(seat);
        }

        auto retCode = inputDevice->StopReceiveEvents();
        if (retCode < 0) {
            LOG_W("Leave, inputDevice StopReceiveEvents Failed");
        }

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        auto seat = FindTouchScreenSeat(display, true);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }
        auto retCode = seat->BindDisplay(display);
        if (retCode < 0) {
            RemoveSeat(seat);
            LOG_E("Leave, BindDisplay Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        auto seat = FindTouchScreenSeat(display, false);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->UnbindDisplay(display);
        if (retCode < 0) {
            LOG_E("Leave, BindDisplay Failed");
            return;
        }

        RemoveSeat(seat);

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        auto seat = FindTouchScreenSeat(display, false);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->UpdateDisplay(display);
        if (retCode < 0) {
            LOG_E("Leave, UpdateDisplay Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        for (const auto& item : touchScreenSeats_) {
            item->OnDisplayAdded(display);
        }

        auto seat = FindSeat(display->GetSeatId(), true);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->AddDisplay(display);
        if (retCode < 0) {
            RemoveSeat(seat);
            LOG_E("Leave, seat AddDisplay Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        for (const auto& item : touchScreenSeats_) {
            item->OnDisplayRemoved(display);
        }

        auto seat = FindSeat(display->GetSeatId(), false);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->RemoveDisplay(display);
        if (retCode < 0) {
            LOG_E("Leave, seat RemoveDisplay Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        for (const auto& item : touchScreenSeats_) {
            item->OnDisplayChanged(display);
        }

        auto seat = FindSeat(display->GetSeatId(), false);
        if (!seat) {
            LOG_E("Leave, null seat");
            return;
        }

        auto retCode = seat->UpdateDisplay(display);
        if (retCode < 0) {
            LOG_E("Leave, seat AddDisplay Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::OnTouchScreenRemoved(const std::shared_ptr<IInputDevice>& inputDevice)
    {
        LOG_D("Enter");
        if (!inputDevice) {
            LOG_E("Leave, null inputDevice");
            return;
        }

        const auto& seatId = inputDevice->GetSeatId();
        const auto& seatName = inputDevice->GetSeatName();

        auto seat = FindTouchScreenSeat(seatId, seatName, false);
        if (!seat) {
            LOG_E("Leave, no seat");
            return;
        }

        auto retCode = seat->UnbindInputDevice(inputDevice);
        if (retCode < 0) {
            LOG_E("Leave, BindInputDevice Failed");
            return;
        }

        LOG_D("Leave");
    }

    void SeatManager::RemoveSeat(const std::shared_ptr<ISeat>& seat)
    {
        if (!seat)  {
            return;
        }

        if (!seat->IsEmpty()) {
            return;
        }

        seats_.remove(seat);
    }

    void SeatManager::RemoveSeat(const std::shared_ptr<ITouchScreenSeat>& seat)
    {
        if (!seat)  {
            return;
        }

        if (!seat->IsEmpty()) {
            return;
        }

        touchScreenSeats_.remove(seat);
    }

}
