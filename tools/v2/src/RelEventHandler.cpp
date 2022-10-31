#include <iostream>

#include <linux/input-event-codes.h>

#include "RelEventHandler.h"
#include "RelEvent.h"
#include "KernelKeyEvent.h"
#include "IInputContext.h"
#include "IWindowStateManager.h"
#include "ISeat.h"
#include "Log.h"
#include "TimeUtils.h"
#include "Utils.h"

namespace Input {

    RelEventHandler::RelEventHandler(const IInputContext* context, const ISeat* seat) 
        : context_(context), seat_(seat), 
        pointerEvent_(PointerEvent::CreateInstance(PointerEvent::SOURCE_TYPE_MOUSE)),
        pointer_(new PointerEvent::PointerItem())
    {
        LOG_D("Enter");
        if (!pointerEvent_) {
            LOG_E("Leave, null pointerEvent_");
            return;
        }

        pointer_->SetId(0);
        pointer_->SetGlobalX(0);
        pointer_->SetGlobalY(0);
        auto retCode = pointerEvent_->AddPointer(pointer_);
        if (retCode < 0) {
            LOG_E("Leave, AddPointer Failed");
            return;
        }
        pointerEvent_->SetPointerId(pointer_->GetId());
        LOG_D("Leave");
    }

    std::shared_ptr<const PointerEvent> RelEventHandler::HandleEvent(const std::shared_ptr<const RelEvent>& event)
    {
        LOG_D("Enter");

        const auto display = AutofixTargetDisplay();
        if (!display) {
            LOG_E("Leave, null targetDisplay");
            return PointerEvent::NULL_VALUE;
        }

        auto targetDisplayId = display->GetId();
        auto xInDisplay = pointer_->GetGlobalX() + event->GetRelX();
        auto yInDisplay = pointer_->GetGlobalY() + event->GetRelY();

        auto width = display->GetWidth();
        auto height = display->GetHeight();

        if ( (xInDisplay < 0 || xInDisplay > width) || (yInDisplay < 0 || yInDisplay > height) ) {
            auto globalX = yInDisplay + display->GetTopLeftX();
            auto globalY = yInDisplay + display->GetTopLeftY();
            auto retCode = FindTarget(globalX, globalY, targetDisplayId, xInDisplay, yInDisplay);
            if (retCode < 0) {
                Utils::MakeInRange(xInDisplay, 0, width);
                Utils::MakeInRange(yInDisplay, 0, height);
            }
        } 

        pointer_->SetDeviceId(event->GetDeviceId());
        pointer_->SetGlobalX(xInDisplay);
        pointer_->SetGlobalY(yInDisplay);
        pointerEvent_->SetActionTime(event->GetActionTime());
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
        pointerEvent_->AssignNewId();

        LOG_D("Leave");
        return pointerEvent_;
    }

    std::shared_ptr<const PointerEvent> RelEventHandler::HandleEvent(const std::shared_ptr<const KernelKeyEvent>& event, bool& consumed) {
        LOG_D("Enter");
        auto keyCode = event->GetKeyCode();
        auto buttonId = KeyCodeToButtonId(keyCode);
        if (buttonId == PointerEvent::BUTTON_NONE) {
            consumed = false;
            LOG_D("Leave, Not Mouse Button");
            return nullptr;
        }

        consumed = true;
        auto deviceId = event->GetDeviceId();
        auto action = event->GetAction();
        auto actionTime = event->GetActionTime();

        std::shared_ptr<const PointerEvent> result;
        if (action == KernelKeyEvent::ACTION_DOWN) {
            result = HandleButtonDownEvent(buttonId, deviceId, actionTime);
        } else if (action == KernelKeyEvent::ACTION_UP) {
            result = HandleButtonUpEvent(buttonId, deviceId, actionTime);
        }  else {
            LOG_E("Leave, invalid keyAction:$s", KernelKeyEvent::ActionToString(action));
            return result;
        }

        LOG_D("Leave");
        return result;
    }

    void RelEventHandler::OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display) 
    {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        auto displayId = display->GetId();
        displays_[displayId] = display;

        auto targetDisplayId = pointerEvent_->GetTargetDisplayId();
        if (targetDisplayId >= 0) {
            LOG_D("Leave displayId:$s, Aready Have TargetDisplayId:$s", displayId, targetDisplayId);
            return;
        }

        pointerEvent_->SetTargetDisplayId(display->GetId());
        pointer_->SetGlobalX(display->GetWidth() / 2);
        pointer_->SetGlobalY(display->GetHeight() / 2);
        LOG_D("Leave");
    }

    void RelEventHandler::OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display)
    {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        auto displayId = display->GetId();
        auto targetDisplayId = pointerEvent_->GetTargetDisplayId();
        if (targetDisplayId != displayId) {
            LOG_D("Leave");
            return;
        }

        auto changed = display->GetChanged();
        if ((changed & LogicalDisplayState::CHANGED_SIZE)) {
            pointer_->SetGlobalX(display->GetWidth() / 2);
            pointer_->SetGlobalY(display->GetHeight() / 2);
            LOG_D("CHANGED_SIZE");
        } 
        LOG_D("Leave");
    }

    void RelEventHandler::OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display) {
        LOG_D("Enter display:$s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }
        auto displayId = display->GetId();
        auto targetDisplayId = pointerEvent_->GetTargetDisplayId();
        if (targetDisplayId != displayId) {
            LOG_D("Leave");
            return;
        }
        pointerEvent_->SetTargetDisplayId(-1);
        if (!AutofixTargetDisplay()) {
            LOG_E("Leave displayId:$s, AutofixTargetDisplay Failed", displayId);
            return;
        }
        LOG_D("Leave displayId:$s", displayId);
        return;
    }

    std::shared_ptr<const LogicalDisplayState> RelEventHandler::AutofixTargetDisplay() {
        LOG_D("Enter");
        auto targetDisplayId = pointerEvent_->GetTargetDisplayId();
        if (targetDisplayId >= 0) {
            const auto& display = GetDisplay(targetDisplayId);
            if (display) {
                LOG_D("Leave");
                return display;
            }

            pointerEvent_->SetTargetDisplayId(-1);
        }

        auto it = displays_.begin();
        if (it == displays_.end()) {
            LOG_E("Leave, No Display");
            return LogicalDisplayState::NULL_VALUE;
        }

        const auto& display = it->second;
        pointerEvent_->SetTargetDisplayId(display->GetId());
        pointer_->SetGlobalX(display->GetWidth() / 2);
        pointer_->SetGlobalY(display->GetHeight() / 2);

        LOG_D("Leave");
        return display;
    }

    int32_t RelEventHandler::AddPressedButton(int32_t buttonId, int32_t deviceId) {
        LOG_D("Enter buttonId:$s deviceId:$s", buttonId, deviceId);
        auto& devices = pressedButtons_[buttonId];
        for (auto item : devices) {
            if (item == deviceId) {
                LOG_E("Leave buttonId:$s deviceId:$s, Already PressedButton", buttonId, deviceId);
                return -1;
            }
        }

        devices.push_back(deviceId);

        auto result = (int)(devices.size());
        LOG_D("Leave buttonId:$s deviceId:$s, result:$s", buttonId, deviceId, result);
        return result;
    }

    int32_t RelEventHandler::RemovePressedButton(int32_t buttonId, int32_t deviceId) {
        LOG_D("Enter buttonId:$s deviceId:$s", buttonId, deviceId);
        auto it = pressedButtons_.find(buttonId);
        if (it == pressedButtons_.end()) {
            LOG_E("Leave buttonId:$s deviceId:$s, Not Pressed Button", buttonId, deviceId);
            return -1;
        }

        for (auto itemIt = it->second.begin(); itemIt != it->second.end(); ++itemIt) {
            if ((*itemIt) == deviceId) {
                it->second.erase(itemIt);
                int32_t result = (int)(it->second.size());
                if (it->second.empty()) {
                    pressedButtons_.erase(it);
                }

                LOG_D("Leave buttonId:$s deviceId:$s, PressedButtonCount:$s", buttonId, deviceId, result);
                return result;
            }
        }

        LOG_E("Leave buttonId:$s deviceId:$s, Not PressedButton", buttonId, deviceId);
        return -1;
    }

    int32_t RelEventHandler::KeyCodeToButtonId(int32_t keyCode) const {
        switch(keyCode) {
            case BTN_LEFT:
                return PointerEvent::BUTTON_MOUSE_LEFT;
            case BTN_RIGHT:
                return PointerEvent::BUTTON_MOUSE_RIGHT;
            case BTN_MIDDLE:
                return PointerEvent::BUTTON_MOUSE_MIDDLE;
            case BTN_SIDE:
                return PointerEvent::BUTTON_MOUSE_SIDE;
            case BTN_EXTRA:
                return PointerEvent::BUTTON_MOUSE_EXTRA;
            case BTN_FORWARD:
                return PointerEvent::BUTTON_MOUSE_FORWARD;
            case BTN_BACK:
                return PointerEvent::BUTTON_MOUSE_BACK;
            case BTN_TASK:
                return PointerEvent::BUTTON_MOUSE_TASK;
            default:
                return PointerEvent::BUTTON_NONE;
        }
    }

    std::shared_ptr<const PointerEvent> RelEventHandler::HandleButtonDownEvent(int32_t buttonId, int32_t deviceId, int64_t actionTime) {
        LOG_D("Enter buttonId:$s deviceId:$s actionTime:$s", buttonId, deviceId, actionTime);
        auto pressedCount = AddPressedButton(buttonId, deviceId);
        if (pressedCount != 1) {
            LOG_D("Leave buttonId:$s deviceId:$s actionTime:$s, PressedButton:$s != 1", buttonId, deviceId, actionTime, pressedCount);
            return nullptr;
        }
        auto ret = pointerEvent_->AddPressedButton(buttonId, deviceId, actionTime);
        if (ret != 0) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, PointerEvent AddPressedButton Failed", buttonId, deviceId, actionTime);
            return nullptr;
        }

        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        pointerEvent_->SetActionTime(actionTime);
        pointerEvent_->AssignNewId();

        LOG_D("Leave buttonId:$s deviceId:$s actionTime:$s", buttonId, deviceId, actionTime);
        return pointerEvent_;
    }

    std::shared_ptr<const PointerEvent> RelEventHandler::HandleButtonUpEvent(int32_t buttonId, int32_t deviceId, int64_t actionTime) {
        LOG_D("Enter buttonId:$s deviceId:$s actionTime:$s", buttonId, deviceId, actionTime);
        auto pressedCount = RemovePressedButton(buttonId, deviceId);
        if (pressedCount != 0)  {
            LOG_D("Leave buttonId:$s deviceId:$s actionTime:$s", buttonId, deviceId, actionTime);
            return nullptr;
        }
        auto ret = pointerEvent_->RemovePressedButton(buttonId, deviceId, actionTime);
        if (ret != 0) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, PointerEvent RemovePressedButton Failed", buttonId, deviceId, actionTime);
            return nullptr;
        }

        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
        pointerEvent_->SetActionTime(actionTime);
        pointerEvent_->AssignNewId();
        return pointerEvent_;
    }

    std::shared_ptr<const LogicalDisplayState> RelEventHandler::GetDisplay(int32_t displayId) const
    {
        auto it = displays_.find(displayId);
        if (it == displays_.end()) {
            return LogicalDisplayState::NULL_VALUE;
        }

        return it->second;
    }

    int32_t RelEventHandler::FindTarget(int32_t globalX, int32_t globalY, 
            int32_t& targetDisplayId, int32_t& localX, int32_t localY) const
    {
        for (const auto& item : displays_) {
            int32_t topLeftX = item.second->GetTopLeftX();
            int32_t width = item.second->GetWidth();
            if (globalX < topLeftX) {
                continue;
            }
            if (globalX > topLeftX + width) {
                continue;
            }

            int32_t topLeftY = item.second->GetTopLeftY();
            int32_t height = item.second->GetHeight();
            if (globalY < topLeftY) {
                continue;
            }
            if (globalY > topLeftY + height) {
                continue;
            }

            localX = globalX - topLeftX;
            localY = globalY - topLeftY;
            targetDisplayId = item.second->GetId();
            return 0;
        }

        return -1;
    }

}
