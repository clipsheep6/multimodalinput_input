#pragma once

#include <ostream>
#include <memory>

#include "StreamUtils.h"

namespace Input {

    class InputEvent {
        public:
            static std::unique_ptr<InputEvent> CreateInstance();
            static std::unique_ptr<InputEvent> CreateInstance(const std::unique_ptr<InputEvent>& inputEvent);

            static const char* EventTypeToString(int32_t eventType);
            static const char* ActionToString(int32_t action);

        public:
            static constexpr int32_t EVENT_TYPE_DEFAULT = 0X00000000;
            static constexpr int32_t EVENT_TYPE_KEY = 0X00010000;
            static constexpr int32_t EVENT_TYPE_POINTER = 0X00020000;

            static constexpr int32_t ACTION_NONE = 0;

        public:
            virtual ~InputEvent() = default;

        public:
            int64_t GetId() const;
            int32_t GetAction() const;
            int64_t GetActionTime() const;
            int64_t GetActionStartTime() const;
            int32_t GetEventType() const;
            int32_t GetTargetDisplayId() const;
            int32_t GetDeviceId() const;

            void AssignNewId();

            void SetId(int64_t id);
            void SetAction(int32_t action);
            void SetActionTime(int64_t actionTime);
            void SetActionStartTime(int64_t actionStartTime);
            void SetTargetDisplayId(int32_t displayId);
            void SetDeviceId(int32_t deviceId);

            virtual std::ostream& operator<<(std::ostream& outStream) const;

        protected:
            InputEvent(int32_t eventType);
            InputEvent(const InputEvent& inputEvent) = default;

            virtual InputEvent* Clone() const;
            std::ostream& PrintInternal(std::ostream& outStream) const;

        protected:
            int64_t id_;
            int32_t action_;
            int64_t actionTime_;
            int64_t actionStartTime_;
            const int32_t eventType_;
            int32_t targetDisplayId_;
            int32_t deviceId_;
    };

    std::ostream& operator<<(std::ostream& outStream, const InputEvent& inputEvent);
    std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<const InputEvent>& inputEvent);
    std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<const InputEvent>& inputEvent);
}
