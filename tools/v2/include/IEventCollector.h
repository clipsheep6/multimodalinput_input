#pragma once
#include <memory>

struct input_event;

namespace Input {

    class IEventCollector {
        public:
            virtual ~IEventCollector() = default;
            virtual bool HandleEvent(struct input_event* inputEvent) = 0;
            virtual void TryReportEvent() = 0;
    };

}
