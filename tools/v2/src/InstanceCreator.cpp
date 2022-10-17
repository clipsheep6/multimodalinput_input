#include "EventLooper.h"
#include "InputDeviceManager.h"
#include "InputContext.h"
#include "SeatManager.h"
#include "Seat.h"
#include "WindowStateManager.h"
#include "EventDispatcher.h"
#include "TouchScreenSeat.h"

namespace Input {
    std::unique_ptr<IEventLooper> IEventLooper::CreateInstance(IInputContext* context)
    {
        return EventLooper::CreateInstance(context);
    }

    std::unique_ptr<IInputDeviceManager> IInputDeviceManager::CreateInstance(IInputContext* context) 
    {
        return InputDeviceManager::CreateInstance(context);
    }

    std::unique_ptr<IInputContext> IInputContext::CreateInstance() 
    {
        return InputContext::CreateInstance();
    }

    std::unique_ptr<ISeatManager> ISeatManager::CreateInstance(const IInputContext* context) 
    {
        return SeatManager::CreateInstance(context);
    }

    std::unique_ptr<ISeat> ISeat::CreateInstance(const IInputContext* context, const std::string& seatId) {
        return Seat::CreateInstance(context, seatId);
    }

    std::unique_ptr<IWindowStateManager> IWindowStateManager::CreateInstance(const IInputContext* context) {
        return WindowStateManager::CreateInstance(context);
    }

    std::unique_ptr<IEventDispatcher> IEventDispatcher::CreateInstance(const IInputContext* context) {
        return EventDispatcher::CreateInstance(context);
    }

    std::unique_ptr<ITouchScreenSeat> ITouchScreenSeat::CreateInstance(const IInputContext* context, const std::string& seatId, const std::string& seatName) {
        return TouchScreenSeat::CreateInstance(context, seatId, seatName);
    }
}



