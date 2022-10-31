#pragma once
#include "IInputDefine.h"
#include "KernelKeyEvent.h"

namespace Input {
    class KeyEventCollector : public NonCopyable {
        public:
            KeyEventCollector(int32_t deviceId);
            virtual ~KeyEventCollector() = default;

            std::shared_ptr<const KernelKeyEvent> HandleKeyEvent(int32_t eventCode, int32_t eventValue);

            void AfterProcessed();

        private:
            [[maybe_unused]] int32_t deviceId_;
            std::shared_ptr<KernelKeyEvent> keyEvent_;
    };
}
