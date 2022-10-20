#pragma once

struct input_event;
namespace Input {
    class KernelAbsEvent {
        public:
            KernelAbsEvent(int32_t deviceId);
            ~KernelAbsEvent() = default;

            int32_t GetDeviceId() const;
            void SetTimeStamp(int64_t timeStamp);
            int64_t GetTimeStamp() const;
            bool IsNewEvent() const;
            bool HandleEvent(struct input_event* inputEvent);
            void Reset();
        private:
            int32_t deviceId_;
            int64_t timeStamp_;
            bool newEvent_;
    };
}
