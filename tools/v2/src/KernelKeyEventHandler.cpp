#include <iostream>

#include <linux/input-event-codes.h>

#include "KernelKeyEventHandler.h"
#include "TimeUtils.h"
#include "Log.h"

namespace Input {

    KernelKeyEventHandler::KernelKeyEventHandler()
    : keyEvent_(KeyEvent::CreateInstance())
    {
    }

    std::shared_ptr<const KeyEvent> KernelKeyEventHandler::HandleEvent(const std::shared_ptr<const KernelKeyEvent>& event)
    {
        LOG_D("Enter");
        {
            bool handled = false;
            std::shared_ptr<const KeyEvent> keyEvent = HandleButton(event, handled);
            if (handled) {
                LOG_D("Leave");
                return keyEvent;
            }
        }

        auto keyCode = event->GetKeyCode();
        auto deviceId = event->GetDeviceId();
        auto action = event->GetAction();
        auto actionTime = event->GetActionTime();

        KeyEvent::KeyItem* keyItem = nullptr;
        if (action == KernelKeyEvent::ACTION_DOWN) {
            keyItem = keyEvent_->SetKeyDown(keyCode, deviceId, actionTime);
        } else if (action == KernelKeyEvent::ACTION_UP){
            keyItem = keyEvent_->SetKeyUp(keyCode, deviceId, actionTime);
        } else {
            LOG_E("Leave, Invalid KeyAction:$s", KernelKeyEvent::ActionToString(action));
            return nullptr;
        }

        if (keyItem == nullptr) {
            LOG_E("Leave, Null KeyItem");
            return nullptr;
        }

        LOG_D("Leave");
        return keyEvent_;
    }

    std::shared_ptr<const KeyEvent> KernelKeyEventHandler::HandleButton(const std::shared_ptr<const KernelKeyEvent>& event, bool& handled) {
        auto keyCode = event->GetKeyCode();
        if ( (keyCode >= BTN_MISC && keyCode <= BTN_GEAR_UP) || 
                (keyCode >= BTN_TRIGGER_HAPPY && keyCode <= BTN_TRIGGER_HAPPY40) ) {
            handled = true;
            return nullptr;
        }

        return nullptr;
    }

    void KernelKeyEventHandler::OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

    void KernelKeyEventHandler::OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

    void KernelKeyEventHandler::OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display)
    {
    }

}
