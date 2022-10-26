#pragma once
#include <memory>
#include <list>

namespace Input {
    class KeyEvent;
    class PointerEvent;
    class IInputContext;
    class IEventHandler {
        public:
            static std::list<std::shared_ptr<IEventHandler>> PrepareHandlers(const IInputContext* context);

            virtual ~IEventHandler() = default;
            virtual bool HandleEvent(const std::shared_ptr<const KeyEvent>& event) = 0;
            virtual bool HandleEvent(const std::shared_ptr<const PointerEvent>& event) = 0;
            virtual const std::string& GetName() const = 0;
    };
}
