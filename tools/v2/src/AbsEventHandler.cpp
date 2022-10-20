#include "AbsEventHandler.h"
#include "AbsEvent.h"
#include "IInputContext.h"
#include "IInputDeviceManager.h"
#include "PointerEvent.h"
#include "Log.h"

namespace Input {

    AbsEventHandler::AbsEventHandler(const IInputContext* context, const std::string& seatId)
        : context_(context), seatId_(seatId)
    {
    }

    std::shared_ptr<const PointerEvent> AbsEventHandler::HandleEvent(const std::shared_ptr<const AbsEvent>& absEvent)
    {
        if (!absEvent) {
            return PointerEvent::NULL_VALUE;
        }

        auto deviceId = absEvent->GetDeviceId();
        auto sourceType = ConvertSourceType(absEvent->GetSourceType());
        if (sourceType == PointerEvent::SOURCE_TYPE_NONE) {
            LOG_E("Leave, ConvertSourceType Failed");
            return PointerEvent::NULL_VALUE;
        }

        auto action = ConvertAction(absEvent->GetAction());
        if (action == PointerEvent::POINTER_ACTION_NONE) {
            LOG_E("Leave, ConvertAction Failed");
            return PointerEvent::NULL_VALUE;
        }

        auto pointerEvent = PointerEvent::CreateInstance(sourceType);
        if (!pointerEvent) {
            LOG_E("Leave, null pointerEvent");
            return PointerEvent::NULL_VALUE;
        }

        for (const auto& absEventPointer : absEvent->GetPointerList()) {
            auto pointer = ConvertPointer(absEventPointer);
            if (!pointer) {
                LOG_E("Leave, ConvertPointer Failed");
                return PointerEvent::NULL_VALUE;
            }
            pointer->SetDeviceId(deviceId);
            auto retCode = pointerEvent->AddPointer(pointer);
            if (retCode < 0) {
                LOG_E("Leave, AddPointer Failed");
                return PointerEvent::NULL_VALUE;
            }
        }

        pointerEvent->SetDeviceId(deviceId);
        pointerEvent->SetPointerAction(action);
        pointerEvent->SetPointerId(absEvent->GetPointerId());
        pointerEvent->SetActionTime(absEvent->GetActionTime());

        return pointerEvent;
    }

    void AbsEventHandler::OnDisplayAdded([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

    void AbsEventHandler::OnDisplayRemoved([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

    void AbsEventHandler::OnDisplayChanged([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

    int32_t AbsEventHandler::ConvertSourceType(int32_t absEventSourceType) const
    {
        if (absEventSourceType == AbsEvent::SOURCE_TYPE_TOUCHPAD) {
            return PointerEvent::SOURCE_TYPE_TOUCHPAD;
        }

        if (absEventSourceType == AbsEvent::SOURCE_TYPE_TOUCHSCREEN) {
            return PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
        }

        LOG_E("Leave, Invalid AbsEvent SourceType:$s", AbsEvent::SourceToString(absEventSourceType));
        return PointerEvent::POINTER_ACTION_NONE;
    }

    int32_t AbsEventHandler::ConvertAction(int32_t absEventAction) const
    {
        if (absEventAction == AbsEvent::ACTION_DOWN) {
            return PointerEvent::POINTER_ACTION_DOWN;
        }

        if (absEventAction == AbsEvent::ACTION_UP) {
            return PointerEvent::POINTER_ACTION_UP;
        }

        if (absEventAction == AbsEvent::ACTION_MOVE) {
            return PointerEvent::POINTER_ACTION_MOVE;
        }

        return PointerEvent::POINTER_ACTION_NONE;
    }

    std::shared_ptr<PointerEvent::PointerItem> AbsEventHandler::ConvertPointer(const std::shared_ptr<const AbsEvent::Pointer>& absEventPointer) const
    {
        std::shared_ptr<PointerEvent::PointerItem> pointer;
        if (!absEventPointer) {
            LOG_E("Leave, null absEventPointer");
            return pointer;
        }

        pointer = std::make_shared<PointerEvent::PointerItem>();
        pointer->SetId(absEventPointer->GetId());
        pointer->SetDownTime(absEventPointer->GetDownTime());
        pointer->SetGlobalX(absEventPointer->GetX());
        pointer->SetGlobalY(absEventPointer->GetY());

        return pointer;
    }
}

