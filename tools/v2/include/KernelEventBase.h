#pragma once

#include <cstdint>
#include <ostream>
#include <memory>

#include "StreamUtils.h"

struct input_event;
namespace Input {

    class KernelEventBase
    {
        public:
            virtual ~KernelEventBase() = default;

            int32_t GetDeviceId() const;
            int32_t GetAction() const;
            int64_t GetActionTime() const;
            virtual std::ostream& operator<<(std::ostream& outStream) const;

            void SetAction(int32_t action);
            void SetActionTime(int64_t actionTime);

        protected:
            KernelEventBase(int32_t deviceId, int32_t action);
            virtual const char* ActionToStr(int32_t action) const = 0;

            std::ostream& PrintInternal(std::ostream& outStream) const;

        private:
            int32_t deviceId_;
            int32_t action_;
            int64_t actionTime_;
    };

    std::ostream& operator<<(std::ostream& outStream, const KernelEventBase& event);
}


