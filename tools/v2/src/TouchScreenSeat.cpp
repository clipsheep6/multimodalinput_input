#include "TouchScreenSeat.h"
#include "IInputContext.h"
#include "IWindowStateManager.h"
#include "IEventDispatcher.h"
#include "PhysicalDisplayState.h"
#include "Utils.h"
#include "Log.h"

namespace Input {

    std::unique_ptr<TouchScreenSeat> TouchScreenSeat::CreateInstance(const IInputContext* context,
            const std::string& seatId, const std::string& seatName) 
    {
        if (context == nullptr) {
            return nullptr;
        }
        return std::unique_ptr<TouchScreenSeat>(new TouchScreenSeat(context, seatId, seatName));
    }

    TouchScreenSeat::TouchScreenSeat(const IInputContext* context, const std::string& seatId, 
            const std::string& seatName) 
        : context_(context), seatId_(seatId), seatName_(seatName)
    {
    }

    int32_t TouchScreenSeat::BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
    {
        LOG_D("Enter");
        if (!inputDevice) {
            LOG_E("Leave, null InputDevice");
            return -1;
        }

        auto inputDeviceId = inputDevice->GetId();
        if (inputDeviceId < 0) {
            LOG_E("Leave, inputDeviceId < 0");
            return -1;
        }

        std::shared_ptr<IInputDevice::AxisInfo> xInfo = inputDevice->GetAxisInfo(IInputDevice::AXIS_MT_X);
        if (!xInfo) {
            LOG_E("Leave, null AxisInfo Of AXIS_MT_X");
            return -1;
        }

        if (xInfo->GetMinimum() >= xInfo->GetMaximum()) {
            LOG_E("Leave, xInfo->GetMinimum():$s >= xInfo->GetMaximum():$s", xInfo->GetMinimum(), xInfo->GetMaximum());
            return -1;
        }

        std::shared_ptr<IInputDevice::AxisInfo> yInfo = inputDevice->GetAxisInfo(IInputDevice::AXIS_MT_Y);
        if (!yInfo) {
            LOG_E("Leave, null AxisInfo Of AXIS_MT_Y");
            return -1;
        }

        if (yInfo->GetMinimum() >= yInfo->GetMaximum()) {
            LOG_E("Leave, yInfo->GetMinimum():$s >= yInfo->GetMaximum():$s", yInfo->GetMinimum(), yInfo->GetMaximum());
            return -1;
        }

        inputDevice_ = inputDevice;
        xInfo_ = xInfo;
        yInfo_ = yInfo;
        
        LOG_D("Leave, inputDeviceId:$s xInfo:$s, yInfo:$s", inputDeviceId, xInfo, yInfo);
        return 0;
    }

    int32_t TouchScreenSeat::UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
    {
        LOG_D("Enter");
        if (inputDevice != inputDevice_) {
            LOG_E("Leave, inputDevice != inputDevice_");
            return -1;
        }

        inputDevice_.reset();
        targetDisplays_.clear();
        LOG_D("Leave");
        return 0;
    }

    std::shared_ptr<IInputDevice> TouchScreenSeat::GetInputDevice() const
    {
        return inputDevice_;
    }

    void TouchScreenSeat::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
    {
        LOG_D("Enter absEvent:$s", event);

        std::shared_ptr<const LogicalDisplayState> targetDisplay;
        int32_t pointerAction = PointerEvent::POINTER_ACTION_NONE;
        int64_t actionTime = 0;
        auto pointer = ConvertPointer(event, pointerAction, actionTime, targetDisplay);
        if (!pointer) {
            LOG_E("Leave ConvertPointer Failed");
            return;
        }

        auto retCode = DispatchTo(targetDisplay, pointerAction, actionTime,  pointer);
        if (retCode < 0) {
            LOG_E("Leave, Dispatch Failed");
            return;
        }

        LOG_D("Leave, targetDisplayId:$s, pointerAction:$s, pointer:$s", targetDisplay->GetId(), 
                PointerEvent::ActionToString(pointerAction), pointer);
    }

    int32_t TouchScreenSeat::BindDisplay(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter");

        if (!display) {
            LOG_E("Leave, null display");
            return -1;
        }

        if (display_) {
            LOG_E("Leave, nonnull display_");
            return -1;
        }

        if (display->GetSeatId() != seatId_ || display->GetSeatName() != seatName_) {
            LOG_E("Leave, seatId,seatName Mismatch");
            return -1;
        }

        display_ = display;
        LOG_D("Leave");
        return 0;
    }

    int32_t TouchScreenSeat::UnbindDisplay(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter");

        if (!display) {
            LOG_E("Leave, null display");
            return -1;
        }

        if (display != display_) {
            return -1;
        }

        display_.reset();
        LOG_D("Leave");
        return 0;
    }

    int32_t TouchScreenSeat::UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter");

        if (!display) {
            LOG_E("Leave, null display");
            return -1;
        }

        if (display != display_) {
            return -1;
        }

        if (display->GetChanged() != PhysicalDisplayState::CHANGED_NONE) {
            targetDisplays_.clear();
        }

        LOG_D("Leave");
        return 0;
    }

    std::shared_ptr<PhysicalDisplayState> TouchScreenSeat::GetDisplay() const
    {
        return display_; 
    }

    const std::string& TouchScreenSeat::GetSeatId() const
    {
        return seatId_; 
    }

    const std::string& TouchScreenSeat::GetSeatName() const
    {
        return seatName_;
    }

    int32_t TouchScreenSeat::TransformToPhysicalDisplayCoordinate(int32_t tpX, int32_t tpY, int32_t& displayX, int32_t& displayY) const
    {
        if (!xInfo_) {
            LOG_E("Leave, null xInfo_");
            return -1;
        }

        if (!yInfo_) {
            LOG_E("Leave, null yInfo_");
            return -1;
        }

        if (!display_) {
            LOG_E("Leave, null display_");
            return -1;
        }

        int32_t deltaX = tpX  - xInfo_->GetMinimum();
        int32_t width = xInfo_->GetMaximum() - xInfo_->GetMinimum() + 1;
        displayX = display_->TransformX(deltaX, width);

        int32_t deltaY = tpY - yInfo_->GetMinimum();
        int32_t height = yInfo_->GetMaximum() - yInfo_->GetMinimum() + 1;
        displayY = display_->TransformY(deltaY, height);

        LOG_D("Leave");
        return 0;
    }

    bool TouchScreenSeat::IsEmpty() const
    {
        return !display_ && !inputDevice_;
    }

    int32_t TouchScreenSeat::DispatchTo(const std::shared_ptr<const LogicalDisplayState>& targetDisplay, 
                    int32_t pointerAction, int64_t actionTime, std::shared_ptr<PointerEvent::PointerItem>& pointer)
    {
        if (!targetDisplay) {
            LOG_E("Leave, null targetDisplay");
            return -1;
        }

        auto pointerEvent = targetDisplay->HandleEvent(pointerAction, actionTime, pointer);
        if (!pointerEvent) {
            LOG_E("Leave, null pointerEvent");
            return -1;
        }

        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return -1;
        }

        const auto& dispatcher = context_->GetEventDispatcher();
        if (!dispatcher) {
            LOG_E("Leave, null dispatcher");
            return -1;
        }

        dispatcher->DispatchEvent(pointerEvent);
        return 0;
    }

    void TouchScreenSeat::OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display)
    {
    }

    void TouchScreenSeat::OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display)
    {
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        for (auto it = targetDisplays_.begin(); it != targetDisplays_.end();) {
            if (it->second == display) {
                it = targetDisplays_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void TouchScreenSeat::OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display)
    {
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        auto changed = display->GetChanged();
        if (changed == LogicalDisplayState::CHANGED_NONE) {
            return;
        }

        for (auto it = targetDisplays_.begin(); it != targetDisplays_.end();) {
            if (it->second == display) {
                it = targetDisplays_.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::shared_ptr<PointerEvent::PointerItem> TouchScreenSeat::ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent,
            int32_t& pointerAction, int64_t& actionTime, std::shared_ptr<const LogicalDisplayState>& targetDisplay)
    {
        std::shared_ptr<PointerEvent::PointerItem> pointer;
        if (!absEvent) {
            LOG_E("Leave, null absEvent");
            return pointer;
        }
        const auto& absEventPointer = absEvent->GetPointer();
        if (!absEventPointer) {
            LOG_E("Leave, null absEventPointer");
            return pointer;
        }

        if (!display_) {
            LOG_E("Leave, null display_");
            return pointer;
        }

        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return pointer;
        }

        const auto& windowStateManager = context_->GetWindowStateManager();
        if (!windowStateManager) {
            LOG_E("Leave, null windowStateManager");
            return pointer;
        }

        auto action = ConvertAction(absEvent->GetAction());
        if (action == PointerEvent::POINTER_ACTION_NONE) {
            LOG_E("Leave, ConvertAction Failed");
            return pointer;
        }

        int32_t inputDeviceId = absEvent->GetDeviceId();
        auto pointerId = absEventPointer->GetId();
        auto pointerKey = Utils::Combine(inputDeviceId, pointerId);
        auto it = targetDisplays_.find(pointerKey);
        int32_t logicalDisplayX = -1;
        int32_t logicalDisplayY = -1;
        int64_t upTime = -1;
        if (action == PointerEvent::POINTER_ACTION_DOWN) {
            if (it != targetDisplays_.end()) {
                targetDisplays_.erase(it);
            }

            int32_t physicalDisplayX = -1;
            int32_t physicalDisplayY = -1;
            auto retCode = TransformToPhysicalDisplayCoordinate(absEventPointer->GetX(), absEventPointer->GetY(), 
                    physicalDisplayX, physicalDisplayY);
            if (retCode < 0) {
                LOG_E("Leave, TransformToPhysicalDisplayCoordinate Failed");
                return pointer;
            }

            int32_t globalX = -1;
            int32_t globalY = -1;
            retCode = windowStateManager->TransformPhysicalDisplayCoordinateToPhysicalGlobalCoordinate(
                    display_->GetId(), physicalDisplayX, physicalDisplayY,
                    globalX, globalY);

            if (retCode < 0) {
                LOG_E("Leave, TransformPhysicalDisplayCooridateToLogicalDisplayCooridate Failed");
                return pointer;
            }

            targetDisplay = windowStateManager->TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate(globalX, globalY,
                    logicalDisplayX, logicalDisplayY);
            if (!targetDisplay) {
                LOG_E("Leave, TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate Failed");
                return pointer;
            }
            targetDisplays_[pointerKey] = targetDisplay;
        } else if (action == PointerEvent::POINTER_ACTION_MOVE || action == PointerEvent::POINTER_ACTION_UP) {
            if (it == targetDisplays_.end()) {
                LOG_E("Leave, No Target When MOVE OR UP");
                return pointer;
            }
            targetDisplay = it->second;
            if (action == PointerEvent::POINTER_ACTION_UP) {
                upTime = absEvent->GetActionTime();
            }

            int32_t physicalDisplayX = -1;
            int32_t physicalDisplayY = -1;
            auto retCode = TransformToPhysicalDisplayCoordinate(absEventPointer->GetX(), absEventPointer->GetY(), 
                    physicalDisplayX, physicalDisplayY);
            if (retCode < 0) {
                LOG_E("Leave, TransformToPhysicalDisplayCoordinate Failed");
                return pointer;
            }

            retCode = targetDisplay->Transform(physicalDisplayX, physicalDisplayY, 
                    true, logicalDisplayX, logicalDisplayY);
            if (retCode < 0) {
                LOG_E("Leave, TransformTo Target Display Coordinate Failed");
                return pointer;
            }

        } else {
            LOG_E("Leve, unknown absEvent Action:$s", PointerEvent::ActionToString(action));
            return pointer;
        }

        pointerAction = action;
        actionTime = absEvent->GetActionTime();

        pointer = std::make_shared<PointerEvent::PointerItem>();
        pointer->SetId(pointerId);
        pointer->SetDownTime(absEventPointer->GetDownTime());
        pointer->SetUpTime(upTime);
        pointer->SetGlobalX(logicalDisplayX);
        pointer->SetGlobalY(logicalDisplayY);
        pointer->SetDeviceId(absEvent->GetDeviceId());
        LOG_D("Leave");
        return pointer;
    }

    int32_t TouchScreenSeat::ConvertAction(int32_t absEventAction) const
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


}
