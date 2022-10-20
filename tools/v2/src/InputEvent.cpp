#include "InputEvent.h"
#include "TimeUtils.h"
#include "IInputDefine.h"

namespace Input {

    static int64_t gs_nextId = 0;

    std::unique_ptr<InputEvent> InputEvent::CreateInstance() {
        return std::unique_ptr<InputEvent>(new InputEvent(EVENT_TYPE_DEFAULT));
    }

    std::unique_ptr<InputEvent> InputEvent::CreateInstance(const std::unique_ptr<InputEvent>& inputEvent) {
        if (!inputEvent) {
            return nullptr;
        }

        InputEvent* newInstance = inputEvent->Clone();
        if (newInstance == nullptr) {
            return nullptr;
        }

        return std::unique_ptr<InputEvent>(newInstance);
    }

    const char* InputEvent::EventTypeToString(int32_t eventType) {
        switch (eventType) {
            CASE_STR(EVENT_TYPE_DEFAULT);
            CASE_STR(EVENT_TYPE_KEY);
            CASE_STR(EVENT_TYPE_POINTER);
            default:
            return "EVENT_TYPE_UNKNOWN";
        }
    }

    const char* InputEvent::ActionToString(int32_t action) {
        switch (action) {
            CASE_STR(ACTION_NONE);
            default:
            return "ACTION_UNKNOWN";
        }
    }

    InputEvent::InputEvent(int32_t eventType)
        : id_(gs_nextId++), action_(ACTION_NONE), actionTime_(TimeUtils::GetTimeStampMs()), 
        actionStartTime_(actionTime_), eventType_(eventType), targetDisplayId_(-1), deviceId_(-1)
    {
    }

    int64_t InputEvent::GetId() const 
    {
        return id_;
    }

    int32_t InputEvent::GetAction() const {
        return action_;
    }

    int64_t InputEvent::GetActionTime() const {
        return actionTime_;
    }

    int64_t InputEvent::GetActionStartTime() const {
        return actionStartTime_;
    }

    int32_t InputEvent::GetEventType() const {
        return eventType_;
    }

    int32_t InputEvent::GetTargetDisplayId() const {
        return targetDisplayId_;
    }

    int32_t InputEvent::GetDeviceId() const {
        return deviceId_;
    }

    void InputEvent::SetId(int64_t id) {
        id_ = id;
    }

    void InputEvent::AssignNewId() {
       id_ = gs_nextId++; 
    }

    void InputEvent::SetAction(int32_t action) {
        action_ = action;
    }

    void InputEvent::SetActionTime(int64_t actionTime) {
        actionTime_ = actionTime;
    }

    void InputEvent::SetActionStartTime(int64_t actionStartTime) {
        actionStartTime_ = actionStartTime;
    }

    void InputEvent::SetTargetDisplayId(int32_t displayId) {
        targetDisplayId_ = displayId;
    }

    void InputEvent::SetDeviceId(int32_t deviceId) {
        deviceId_ = deviceId;
    }

    InputEvent* InputEvent::Clone() const {
        if (eventType_ == EVENT_TYPE_DEFAULT) {
            return new InputEvent(*this);
        }

        return nullptr;
    }

    std::ostream& InputEvent::operator<<(std::ostream& outStream) const
    {
        return PrintInternal(outStream);
    }

    std::ostream& InputEvent::PrintInternal(std::ostream& outStream) const {
        return outStream << '{' 
            << "id:" << id_ << ','
            << "action:" << ActionToString(action_) << ','
            << "actionTime:" << actionTime_ << ','
            << "actionStartTime:" << actionStartTime_ << ','
            << "eventType:" << EventTypeToString(eventType_) << ','
            << "targetDisplayId:" << targetDisplayId_ 
            << '}';
    }

    std::ostream& operator<<(std::ostream& outStream, const InputEvent& inputEvent) {
        return inputEvent.operator<<(outStream);
    }
}

