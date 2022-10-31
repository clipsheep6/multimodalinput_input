#pragma once
#include <memory>

#include "KernelEventBase.h"

namespace Input {
    class KernelKeyEvent : public KernelEventBase {
        public:
            static const std::unique_ptr<KernelKeyEvent> NULL_EVENT;

            static constexpr int32_t ACTION_NONE = 0;
            static constexpr int32_t ACTION_DOWN = 1;
            static constexpr int32_t ACTION_UP = 2;
            
        public:
            static const char* ActionToString(int32_t action);

        public:
            KernelKeyEvent(int32_t deviceId);
            ~KernelKeyEvent() = default;

            int32_t GetKeyCode() const;
            void SetKeyCode(int32_t keyCode);

        protected:
            virtual const char* ActionToStr(int32_t action) const override;

        private:
            int32_t keyCode_;
    };
}
