#pragma once
#include <map>
#include <list>
#include <ostream>

#include "InputEvent.h"

namespace Input {

    class PointerEvent : public InputEvent {
        public:
            static const std::shared_ptr<PointerEvent> NULL_VALUE;

            static std::shared_ptr<PointerEvent> CreateInstance(int32_t sourceType);

            static const char* ActionToString(int32_t action);
            static const char* AxisToString(int32_t axis);
            static const char* ButtonToString(int32_t buttonId);
            static const char* SourceTypeToString(int32_t sourceType);

        public:
            static constexpr int32_t POINTER_ACTION_NONE = 0;
            static constexpr int32_t POINTER_ACTION_CANCEL = 1;
            static constexpr int32_t POINTER_ACTION_DOWN = 2;
            static constexpr int32_t POINTER_ACTION_MOVE = 3;
            static constexpr int32_t POINTER_ACTION_UP = 4;
            static constexpr int32_t POINTER_ACTION_BUTTON_DOWN = 5;
            static constexpr int32_t POINTER_ACTION_BUTTON_UP = 6;
            static constexpr int32_t POINTER_ACTION_AXIS_BEGIN = 7;
            static constexpr int32_t POINTER_ACTION_AXIS_UPDATE = 8;
            static constexpr int32_t POINTER_ACTION_AXIS_END = 9;

            static constexpr int32_t POINTER_AXIS_NONE = 0;
            static constexpr int32_t POINTER_AXIS_VERTICAL = 1;
            static constexpr int32_t POINTER_AXIS_HORIZONTAL = 2;
            static constexpr int32_t POINTER_AXIS_PINCH = 3;
            static constexpr int32_t POINTER_AXIS_SWIPE = 4;

            static constexpr int32_t BUTTON_NONE = 0;
            static constexpr int32_t BUTTON_MOUSE_LEFT = 1;
            static constexpr int32_t BUTTON_MOUSE_MIDDLE = 2;
            static constexpr int32_t BUTTON_MOUSE_RIGHT = 3;
            static constexpr int32_t BUTTON_MOUSE_SIDE = 4;
            static constexpr int32_t BUTTON_MOUSE_EXTRA = 5;
            static constexpr int32_t BUTTON_MOUSE_FORWARD = 6;
            static constexpr int32_t BUTTON_MOUSE_BACK = 7;
            static constexpr int32_t BUTTON_MOUSE_TASK = 8;
            static constexpr int32_t BUTTON_MAX = BUTTON_MOUSE_TASK + 1;

            static constexpr int32_t SOURCE_TYPE_NONE = 0;
            static constexpr int32_t SOURCE_TYPE_MOUSE = 1;
            static constexpr int32_t SOURCE_TYPE_TOUCHSCREEN = 2;
            static constexpr int32_t SOURCE_TYPE_TOUCHPAD = 3;
            static constexpr int32_t SOURCE_TYPE_END = 4;

        public:
            class PointerItem {
                public:
                    PointerItem() = default;
                    virtual ~PointerItem() = default;

                    int32_t GetId() const;
                    int64_t GetDownTime() const;
                    int64_t GetUpTime() const;
                    int32_t GetGlobalX() const;
                    int32_t GetGlobalY() const;
                    int32_t GetDeviceId() const;
                    int32_t GetLocalX() const;
                    int32_t GetLocalY() const;

                    void SetId(int32_t id);
                    void SetDownTime(int64_t downTime);
                    void SetUpTime(int64_t upTime);
                    void SetGlobalX(int32_t globalX);
                    void SetGlobalY(int32_t globalY);
                    void SetDeviceId(int32_t deviceId);
                    void SetLocalX(int32_t localX);
                    void SetLocalY(int32_t localY);

                    bool IsPressed() const;

                    std::ostream& operator<<(std::ostream& outStream) const;

                private:
                    int32_t id_ {-1};
                    int64_t downTime_ {-1};
                    int64_t upTime_ {-1};
                    int32_t globalX_ {-1};
                    int32_t globalY_ {-1};
                    int32_t deviceId_ {-1};
                    int32_t localX_ {-1};
                    int32_t localY_ {-1};
            };

        public:
            PointerEvent();

        public:
            int32_t GetSourceType() const;
            int32_t GetPointerId() const;
            int32_t GetPointerAction() const;
            std::shared_ptr<const PointerItem> GetPointerItem() const;
            std::shared_ptr<const PointerItem> GetPointerItem(int32_t pointerId) const;
            std::list<int32_t> GetPointerIdList() const;

            int32_t GetButtonId() const;
            bool IsButtonPressed(int32_t buttonId) const;
            std::list<int32_t> GetPressedButtons() const;

            virtual std::ostream& operator<<(std::ostream& outStream) const override;

            std::shared_ptr<PointerItem> GetPointerItem(int32_t pointerId);

            void SetSourceType(int32_t sourceType);
            void SetPointerId(int32_t pointerId);
            void SetPointerAction(int32_t pointerId);
            int32_t AddPointer(const std::shared_ptr<PointerItem>& pointer);
            int32_t RemovePointer(const std::shared_ptr<PointerItem>& pointer);
            std::shared_ptr<PointerItem> RemovePointer(int32_t pointerId);
                
            int32_t SetButtonId(int32_t buttonId);
            int32_t AddPressedButton(int32_t buttonId, int32_t deviceId, int64_t actionTime);
            int32_t RemovePressedButton(int32_t buttonId, int32_t deviceId, int64_t actionTime);
            
        protected:
            PointerEvent(int32_t eventType);
            virtual PointerEvent* Clone() const override;

            std::ostream& PrintInternal(std::ostream& outStream) const;

        private:
            int32_t pointerId_;
            int32_t pointerAction_;
            int32_t sourceType_;

            std::map<int32_t, std::shared_ptr<PointerItem>> pointers_;

            int32_t buttonId_;
            std::list<int32_t> pressedButtons_;
    };

    std::ostream& operator<< (std::ostream& outStream, const PointerEvent& pointerEvent);
    std::ostream& operator<< (std::ostream& outStream, const PointerEvent::PointerItem& pointerItem);
}
