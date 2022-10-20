#include <linux/input.h>

#include "RelEventCollector.h"
#include "TimeUtils.h"
#include "Log.h"

namespace Input {
    RelEventCollector::RelEventCollector(int32_t deviceId)
        : deviceId_(deviceId), relEvent_(std::make_shared<RelEvent>(deviceId)) {
    }

    void RelEventCollector::HandleRelEvent(int32_t code, int32_t value) {
        switch(code) {
            case REL_X:
                relEvent_->AddRelX(value);
                relEvent_->SetAction(RelEvent::ACTION_MOVE);
                break;
            case REL_Y:
                relEvent_->AddRelY(value);
                relEvent_->SetAction(RelEvent::ACTION_MOVE);
                break;
            case REL_Z:
                break;
            case REL_RX:
                break;
            case REL_RY:
                break;
            case REL_RZ:
                break;
            case REL_HWHEEL:
                break;
            case REL_DIAL:
                break;
            case REL_WHEEL:
                break;
            case REL_MISC:
                break;
            case REL_RESERVED:
                break;
            case REL_WHEEL_HI_RES:
                break;
            case REL_HWHEEL_HI_RES:
                break;
            //case REL_MAX:
            //case REL_CNT:
            default:
                break;
        }
    }

    std::shared_ptr<const RelEvent> RelEventCollector::HandleSyncEvent(int32_t code, int32_t value) {
        if (relEvent_->GetAction() == RelEvent::ACTION_NONE) {
            return nullptr;
        }

        relEvent_->SetActionTime(TimeUtils::GetTimeStampMs());
        return relEvent_;
    }

    void RelEventCollector::AfterProcessed() {
        relEvent_->SetRelX(0);
        relEvent_->SetRelY(0);
        relEvent_->SetAction(RelEvent::ACTION_NONE);
    }
}
