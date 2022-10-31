#include "KernelEventBase.h"
#include "Log.h"

namespace Input {

    KernelEventBase::KernelEventBase(int32_t deviceId, int32_t action) 
        : deviceId_(deviceId), action_(action), actionTime_(-1)
    {
    }

    int32_t KernelEventBase::GetDeviceId() const {
        return deviceId_;
    }

    int32_t KernelEventBase::GetAction() const {
        return action_;
    }

    int64_t KernelEventBase::GetActionTime() const {
        return actionTime_;
    }

    void KernelEventBase::SetAction(int32_t action) {
        LOG_D("action:$s, action_:$s", ActionToStr(action),
                ActionToStr(action_));
        action_ = action;
    }

    void KernelEventBase::SetActionTime(int64_t actionTime) {
        actionTime_ = actionTime;
    }

    std::ostream& KernelEventBase::operator<<(std::ostream& outStream) const 
    {
        return PrintInternal(outStream);
    }

    std::ostream& KernelEventBase::PrintInternal(std::ostream& outStream) const
    {
        return outStream << '{'
            << "deviceId:" << deviceId_ << ','
            << "action:" << ActionToStr(action_) << ','
            << "actionTime:" << actionTime_
            << '}';
    }

    std::ostream& operator<<(std::ostream& outStream, const KernelEventBase& event)
    {
        return event.operator<<(outStream);
    }
}
