#include <iostream>

#include "EventDispatcher.h"
#include "Log.h"

namespace Input {
    std::unique_ptr<EventDispatcher> EventDispatcher::CreateInstance(const IInputContext* context) {
        if (context == nullptr) {
            return nullptr;
        }

        return std::unique_ptr<EventDispatcher>(new EventDispatcher(context));
    }

    EventDispatcher::EventDispatcher(const IInputContext* context) 
        : context_(context)
    {
    }

    void EventDispatcher::DispatchEvent(const std::shared_ptr<const PointerEvent>& pointerEvent)  {
        LOG_D("Enter PointerEvent:$s", pointerEvent);
        LOG_D("Leave PointerEvent");
    }

    void EventDispatcher::DispatchEvent(const std::shared_ptr<const KeyEvent>& keyEvent)  {
        LOG_D("Enter KeyEvent:$s", keyEvent);
        LOG_D("Leave KeyEvent");
    }

}
