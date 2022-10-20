#pragma once
#include <list>

#include "IEventDispatcher.h"
#include "IInputDefine.h"

namespace Input {
    class EventDispatcher : public NonCopyable,  public IEventDispatcher {
        public:
            static std::unique_ptr<EventDispatcher> CreateInstance(const IInputContext* context);

        public:
            virtual ~EventDispatcher() = default;

            virtual void DispatchEvent(const std::shared_ptr<const PointerEvent>& pointerEvent) override;
            virtual void DispatchEvent(const std::shared_ptr<const KeyEvent>& keyEvent) override;

        protected:
            EventDispatcher(const IInputContext* context);

        private:
            [[maybe_unused]] const IInputContext* const context_;
    };
}
