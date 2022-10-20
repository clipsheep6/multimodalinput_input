#include <linux/input.h>

#include "KernelKeyEvent.h"
#include "IInputDefine.h"

namespace Input {

    const std::unique_ptr<KernelKeyEvent> KernelKeyEvent::NULL_EVENT;

    const char* KernelKeyEvent::ActionToString(int32_t action) {
        switch (action) {
            CASE_STR(ACTION_NONE);
            CASE_STR(ACTION_DOWN);
            CASE_STR(ACTION_UP);
            default:
            return "ACTION_UNKNOWN";
        }

    }
    KernelKeyEvent::KernelKeyEvent(int32_t deviceId) 
        : KernelEventBase(deviceId, ACTION_NONE), keyCode_(0) {
        }

    int32_t KernelKeyEvent::GetKeyCode() const {
        return keyCode_;
    }

    void KernelKeyEvent::SetKeyCode(int32_t keyCode) {
        keyCode_ = keyCode;
    }

    const char* KernelKeyEvent::ActionToStr(int32_t action) const {
        return ActionToString(action);
    }

}

