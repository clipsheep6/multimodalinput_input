#pragma once
#include <memory>
#include <list>

#include "IEventHandler.h"

namespace Input {

    class IInputContext;
    class RelEvent;
    class KernelKeyEvent;
    class IEventTransformer : public IEventHandler {
        public:
            class INewEventListener {
                public:
                    virtual ~INewEventListener() = default;
                    virtual void OnEvent(const std::shared_ptr<const KeyEvent>& event) = 0;
                    virtual void OnEvent(const std::shared_ptr<const PointerEvent>& event) = 0;
                    virtual void OnEvent(const std::shared_ptr<const RelEvent>& event) = 0;
                    virtual void OnEvent(const std::shared_ptr<const KernelKeyEvent>& event) = 0;
            };

            static std::list<std::shared_ptr<IEventTransformer>> CreateTransformers(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);
    };

}
