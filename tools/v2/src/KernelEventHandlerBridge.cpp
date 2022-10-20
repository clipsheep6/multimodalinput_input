#include "KernelEventHandlerBridge.h"
#include "Log.h"

namespace Input {
    std::shared_ptr<IKernelEventHandler> KernelEventHandlerBridge::CreateInstance(const std::shared_ptr<ISeat>& seat, 
            const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat)
    {
        if (!seat) {
            return nullptr;
        }

        if (!touchScreenSeat) {
            return std::shared_ptr<IKernelEventHandler>(
                    new KernelEventHandlerBridge(seat, ITouchScreenSeat::GetDefault()));
        }

        return std::shared_ptr<IKernelEventHandler>(new KernelEventHandlerBridge(seat, 
                    touchScreenSeat));
    }

    void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const RelEvent>& event)
    {
        seat_->OnInputEvent(event);
    }

    void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event)
    {
        seat_->OnInputEvent(event);
    }

    void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
    {
        if (event->GetSourceType() == AbsEvent::SOURCE_TYPE_TOUCHPAD) {
            seat_->OnInputEvent(event);
            return;
        }

        touchScreenSeat_->OnInputEvent(event);
    }
    KernelEventHandlerBridge::KernelEventHandlerBridge(const std::shared_ptr<ISeat>& seat, 
            const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat)
        : seat_(seat), touchScreenSeat_(touchScreenSeat)
    {
    }
}

