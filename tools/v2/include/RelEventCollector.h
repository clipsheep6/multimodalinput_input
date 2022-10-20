#pragma once
#include "IInputDefine.h"
#include "RelEvent.h"

namespace Input {
    class RelEventCollector : public NonCopyable {
        public:
            RelEventCollector(int32_t deviceId);
            virtual ~RelEventCollector() = default;

            void HandleRelEvent(int32_t eventCode, int32_t eventValue);
            std::shared_ptr<const RelEvent> HandleSyncEvent(int32_t eventCode, int32_t eventValue);

            void AfterProcessed();

        private:
            [[maybe_unused]] int32_t deviceId_;
            std::shared_ptr<RelEvent> relEvent_;
    };
}
