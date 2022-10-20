#include "KeyEventCollector.h"
#include "TimeUtils.h"

namespace Input {
    KeyEventCollector::KeyEventCollector(int32_t deviceId) 
    : deviceId_(deviceId), keyEvent_(std::make_shared<KernelKeyEvent>(deviceId)) {
    }

    std::shared_ptr<const KernelKeyEvent> KeyEventCollector::HandleKeyEvent(int32_t code, int32_t value) {
        keyEvent_->SetKeyCode(code);
        keyEvent_->SetAction(value > 0 ? KernelKeyEvent::ACTION_DOWN : KernelKeyEvent::ACTION_UP);
        keyEvent_->SetActionTime(TimeUtils::GetTimeStampMs());
        return keyEvent_;
    }

    void KeyEventCollector::AfterProcessed() {
        keyEvent_->SetAction(KernelKeyEvent::ACTION_NONE);
    }
}

