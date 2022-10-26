#include <linux/input.h>

#include "RelEvent.h"
#include "IInputDefine.h"

namespace Input {

    RelEvent::RelEvent(int32_t deviceId)
        : KernelEventBase(deviceId, ACTION_NONE), relX_(0), relY_(0)
    {
    }

    int32_t RelEvent::GetRelX() const
    {
        return relX_;
    }

    int32_t RelEvent::GetRelY() const
    {
        return relY_;
    }

    std::ostream& RelEvent::operator<<(std::ostream& outStream) const
    {
        return PrintInternal(outStream);
    }

    void RelEvent::SetRelX(int32_t relX) {
        relX_ = relX;
    }

    void RelEvent::SetRelY(int32_t relY) {
        relY_ = relY;
    }

    void RelEvent::AddRelX(int32_t relX) {
        SetRelX(relX_ + relX);
    }

    void RelEvent::AddRelY(int32_t relY) {
        SetRelX(relY_ + relY);
    }

    std::ostream& RelEvent::PrintInternal(std::ostream& outStream) const
    {
        outStream << '{';
        KernelEventBase::PrintInternal(outStream);
        return outStream << ','
            << "relX_:" << relX_ << ','
            << "relY_:" << relY_
            << '}';
    }

    const char* RelEvent::ActionToStr(int32_t action) const {
        switch (action) {
            CASE_STR(ACTION_NONE);
            CASE_STR(ACTION_MOVE);
            default:
                return "ACTION_UNKNOWN";
        }
    }

    std::ostream& operator<<(std::ostream& outStream, const RelEvent& event)
    {
        return event.operator<<(outStream);
    }
}
