#include <algorithm>

#include "PointerEvent.h"
#include "TimeUtils.h"
#include "IInputDefine.h"
#include "Log.h"

namespace Input {
    const std::shared_ptr<PointerEvent> PointerEvent::NULL_VALUE;

    std::shared_ptr<PointerEvent> PointerEvent::CreateInstance(int32_t sourceType) {
        auto pointerEvent = std::shared_ptr<PointerEvent>(new PointerEvent(InputEvent::EVENT_TYPE_POINTER));
        pointerEvent->SetSourceType(sourceType);
        return pointerEvent;
    }

    const char* PointerEvent::ActionToString(int32_t action) {
        switch (action) {
            CASE_STR(POINTER_ACTION_NONE);
            CASE_STR(POINTER_ACTION_CANCEL);
            CASE_STR(POINTER_ACTION_DOWN);
            CASE_STR(POINTER_ACTION_MOVE);
            CASE_STR(POINTER_ACTION_UP);
            CASE_STR(POINTER_ACTION_BUTTON_DOWN);
            CASE_STR(POINTER_ACTION_BUTTON_UP);
            CASE_STR(POINTER_ACTION_AXIS_BEGIN);
            CASE_STR(POINTER_ACTION_AXIS_UPDATE);
            CASE_STR(POINTER_ACTION_AXIS_END);
            default:
            return "POINTER_ACTION_UNKNOWN";
        }
    }

    const char* PointerEvent::AxisToString(int32_t axis) {
        switch (axis) {
            CASE_STR(POINTER_AXIS_NONE);
            CASE_STR(POINTER_AXIS_VERTICAL);
            CASE_STR(POINTER_AXIS_HORIZONTAL);
            CASE_STR(POINTER_AXIS_PINCH);
            CASE_STR(POINTER_AXIS_SWIPE);
            default:
            return "POINTER_AXIS_UNKNOWN";
        }
    }

    const char* PointerEvent::ButtonToString(int32_t buttonId) {
        switch(buttonId) {
            CASE_STR(BUTTON_NONE);
            CASE_STR(BUTTON_MOUSE_LEFT);
            CASE_STR(BUTTON_MOUSE_MIDDLE);
            CASE_STR(BUTTON_MOUSE_RIGHT);
            CASE_STR(BUTTON_MOUSE_SIDE);
            CASE_STR(BUTTON_MOUSE_EXTRA);
            CASE_STR(BUTTON_MOUSE_FORWARD);
            CASE_STR(BUTTON_MOUSE_BACK);
            CASE_STR(BUTTON_MOUSE_TASK);
            CASE_STR(BUTTON_MAX);
            default:
            return "BUTTON_UNKNOWN";
        }
    }

    const char* PointerEvent::SourceTypeToString(int32_t sourceType) {
        switch(sourceType) {
            CASE_STR(SOURCE_TYPE_NONE);
            CASE_STR(SOURCE_TYPE_MOUSE);
            CASE_STR(SOURCE_TYPE_TOUCHSCREEN);
            CASE_STR(SOURCE_TYPE_TOUCHPAD);
            CASE_STR(SOURCE_TYPE_END);
            default:
            return "SOURCE_TYPE_UNKNOWN";
        }
    }

    int32_t PointerEvent::PointerItem::GetId() const {
        return id_;
    }

    int64_t PointerEvent::PointerItem::GetDownTime() const {
        return downTime_;
    }

    int32_t PointerEvent::PointerItem::GetGlobalX() const {
        return globalX_;
    }

    int32_t PointerEvent::PointerItem::GetGlobalY() const {
        return globalY_;
    }

    int32_t PointerEvent::PointerItem::GetDeviceId() const {
        return deviceId_;
    }

    int32_t PointerEvent::PointerItem::GetLocalX() const {
        return localX_;
    }

    int32_t PointerEvent::PointerItem::GetLocalY() const {
        return localY_;
    }

    void PointerEvent::PointerItem::SetId(int32_t id) {
        id_ = id;
    }

    void PointerEvent::PointerItem::SetDownTime(int64_t downTime) {
        downTime_ = downTime;
    }

    void PointerEvent::PointerItem::SetUpTime(int64_t upTime) {
        upTime_ = upTime;
    }

    void PointerEvent::PointerItem::SetGlobalX(int32_t globalX) {
        globalX_ = globalX;
    }

    void PointerEvent::PointerItem::SetGlobalY(int32_t globalY) {
        globalY_ = globalY;
    }

    void PointerEvent::PointerItem::SetDeviceId(int32_t deviceId) {
        deviceId_ = deviceId;
    }

    void PointerEvent::PointerItem::SetLocalX(int32_t localX) {
        localX_ = localX;
    }

    void PointerEvent::PointerItem::SetLocalY(int32_t localY) {
        localY_ = localY;
    }

    bool PointerEvent::PointerItem::IsPressed() const {
        return downTime_ > 0 && upTime_ == -1;
    }

    std::ostream& PointerEvent::PointerItem::operator<<(std::ostream& outStream) const {
        outStream
            << '{'
            << "id:" << id_ << ','
            << "downTime:" << downTime_ << ','
            << "upTime:" << upTime_<< ','
            << "globalX:" << globalX_ << ','
            << "globalY:" << globalY_ << ','
            << "deviceId:" << deviceId_ << ','
            << "localX:" << localX_ << ','
            << "localY:" << localY_ << ','
            << '}';
        return outStream;
    }

    PointerEvent::PointerEvent() : PointerEvent(InputEvent::EVENT_TYPE_POINTER)
    {
    }

    PointerEvent::PointerEvent(int32_t eventType) 
        : InputEvent(InputEvent::EVENT_TYPE_POINTER),
        pointerId_(-1), pointerAction_(POINTER_ACTION_NONE), sourceType_(SOURCE_TYPE_NONE),
        buttonId_(BUTTON_NONE)
    {
    }

    int32_t PointerEvent::GetSourceType() const {
        return sourceType_; 
    }

    int32_t PointerEvent::GetPointerId() const {
        return pointerId_;
    }

    int32_t PointerEvent::GetPointerAction() const {
        return pointerAction_;
    }

    std::shared_ptr<const PointerEvent::PointerItem> PointerEvent::GetPointerItem() const 
    {
        return GetPointerItem(pointerId_);
    }

    std::shared_ptr<const PointerEvent::PointerItem> PointerEvent::GetPointerItem(int32_t pointerId) const {
        auto it = pointers_.find(pointerId);
        if (it == pointers_.end()) {
            return nullptr;
        }

        return it->second;
    }

    std::shared_ptr<PointerEvent::PointerItem> PointerEvent::GetPointerItem(int32_t pointerId)
    {
        auto it = pointers_.find(pointerId);
        if (it == pointers_.end()) {
            return nullptr;
        }

        return it->second;

    }

    std::list<int32_t> PointerEvent::GetPointerIdList() const
    {
        std::list<int32_t> result;
        for (const auto& item : pointers_) {
            result.push_back(item.first);
        }
        return result;
    }

    int32_t PointerEvent::GetButtonId() const {
        return buttonId_;
    }

    bool PointerEvent::IsButtonPressed(int32_t buttonId) const {
        auto it = std::find(pressedButtons_.begin(), pressedButtons_.end(), buttonId);
        return it != pressedButtons_.end();
    }

    std::list<int32_t> PointerEvent::GetPressedButtons() const {
        return pressedButtons_;
    }

    std::ostream& PointerEvent::operator<<(std::ostream& outStream) const {
        return PrintInternal(outStream);
    }

    void PointerEvent::SetSourceType(int32_t sourceType) {
        sourceType_ = sourceType;
    }

    void PointerEvent::SetPointerId(int32_t pointerId) {
        pointerId_ = pointerId;
    }

    void PointerEvent::SetPointerAction(int32_t pointerAction) {
        pointerAction_ = pointerAction;
    }

    int32_t PointerEvent::AddPointer(const std::shared_ptr<PointerEvent::PointerItem>& pointerItem)
    {
        if (!pointerItem) {
            LOG_E("Leave, null pointerItem");
            return -1;
        }

        auto pointerId = pointerItem->GetId();
        if (pointerId < 0) {
            LOG_E("Leave, pointerId:$s < 0", pointerId);
            return -1;
        }

        if (GetPointerItem(pointerId)) {
            LOG_E("Leave, Pointer Exist");
            return -1;
        }

        pointers_[pointerId] = pointerItem;
        return 0;
    }

    int32_t PointerEvent::RemovePointer(const std::shared_ptr<PointerEvent::PointerItem>& pointerItem)
    {
        if (!pointerItem) {
            LOG_E("Leave, null pointerItem");
            return -1;
        }

        auto pointerId = pointerItem->GetId();
        if (pointerId < 0) {
            LOG_E("Leave, pointerId:$s < 0", pointerId);
            return -1;
        }

        for (auto it = pointers_.begin(); it != pointers_.end(); ++it) {
            if (it->second == pointerItem) {
                pointers_.erase(it);
                return 0;
            }
        }

        LOG_E("Leave, Not Exist Pointer");
        return 0;
    }

    std::shared_ptr<PointerEvent::PointerItem> PointerEvent::RemovePointer(int32_t pointerId) {
        auto it = pointers_.find(pointerId);
        if (it == pointers_.end()) {
            return nullptr;
        }

        std::shared_ptr<PointerEvent::PointerItem> result = it->second;
        pointers_.erase(it);
        return result;
    }

    int32_t PointerEvent::SetButtonId(int32_t buttonId) {
        if (buttonId < BUTTON_NONE || buttonId >= BUTTON_MAX) {
            return -1;
        }

        buttonId_ = buttonId;
        return 0;
    }

    int32_t PointerEvent::AddPressedButton(int32_t buttonId, int32_t deviceId, int64_t actionTime)
    {
        LOG_D("Enter buttonId:$s deviceId:$s actionTime:$s", ButtonToString(buttonId), deviceId, actionTime);
        if (buttonId <= BUTTON_NONE || buttonId >= BUTTON_MAX) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, Invalid ButtonId", ButtonToString(buttonId), deviceId, actionTime);
            return -1;
        }

        if (sourceType_ != SOURCE_TYPE_MOUSE) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, SourceType:$ Not Mouse", 
                    ButtonToString(buttonId), deviceId, actionTime, SourceTypeToString(sourceType_));
            return -1;
        }

        auto it = std::find(pressedButtons_.begin(), pressedButtons_.end(), buttonId);
        if (it != pressedButtons_.end()) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, Already Pressed", 
                    ButtonToString(buttonId), deviceId, actionTime);
            return -1;
        }

        pressedButtons_.push_back(buttonId);

        LOG_D("Leave buttonId:$s deviceId:$s actionTime:$s", 
                ButtonToString(buttonId), deviceId, actionTime);
        return 0;
    }

    int32_t PointerEvent::RemovePressedButton(int32_t buttonId, int32_t deviceId, int64_t actionTime){
        LOG_D("Enter buttonId:$s deviceId:$s actionTime:$s", ButtonToString(buttonId), deviceId, actionTime);
        if (sourceType_ != SOURCE_TYPE_MOUSE) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, SourceType:$ Not Mouse", 
                    ButtonToString(buttonId), deviceId, actionTime, SourceTypeToString(sourceType_));
            return -1;
        }

        auto it = std::find(pressedButtons_.begin(), pressedButtons_.end(), buttonId);
        if (it == pressedButtons_.end()) {
            LOG_E("Leave buttonId:$s deviceId:$s actionTime:$s, Not Pressed", 
                    ButtonToString(buttonId), deviceId, actionTime);
            return -1;
        }

        pressedButtons_.erase(it);

        LOG_D("Leave buttonId:$s deviceId:$s actionTime:$s", 
                ButtonToString(buttonId), deviceId, actionTime);
        return 0;
    }

    PointerEvent* PointerEvent::Clone() const {
        if (GetEventType() != EVENT_TYPE_POINTER) {
            return nullptr;
        }

        return new PointerEvent(*this);
    }

    std::ostream& PointerEvent::PrintInternal(std::ostream& outStream) const {
        outStream << '{'
            << "InputEvent:";

        InputEvent::PrintInternal(outStream);

        return outStream << ','
            << "pointerId:" << pointerId_  << ','
            << "pointerAction:" << ActionToString(pointerAction_) << ','
            << "sourceType:" << SourceTypeToString(sourceType_) << ','
            << "pointers:" << pointers_ << ','
            << "buttonId:" << buttonId_ << ','
            << "pressedButtons:" << pressedButtons_ << ','
            << '}';
    }

    std::ostream& operator<< (std::ostream& outStream, const PointerEvent& pointerEvent) {
        return pointerEvent.operator<<(outStream);
    }

    std::ostream& operator<< (std::ostream& outStream, const PointerEvent::PointerItem& pointerItem) {
        return pointerItem.operator<<(outStream);
    }

}
