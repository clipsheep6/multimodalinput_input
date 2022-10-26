#pragma once

#include "PointerEvent.h"
#include "KeyEvent.h"

namespace Input {
    class IInputContext;
    class IEventDispatcher {
        public:
            static std::unique_ptr<IEventDispatcher> CreateInstance(const IInputContext* context);

            virtual ~IEventDispatcher() = default;

            virtual void DispatchEvent(const std::shared_ptr<const PointerEvent>& pointerEvent) = 0;
            virtual void DispatchEvent(const std::shared_ptr<const KeyEvent>& keyEvent) = 0;
    };
}
