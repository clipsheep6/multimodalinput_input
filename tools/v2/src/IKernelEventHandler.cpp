#include "IKernelEventHandler.h"

namespace Input {

    class DefaultHandler : public IKernelEventHandler {
        virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) override
        {
        }

        virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) override
        {
        }

        virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override
        {
        }
    };

    const std::shared_ptr<IKernelEventHandler>& IKernelEventHandler::GetDefault() {
        static std::shared_ptr<IKernelEventHandler> defaultHandler(new DefaultHandler());
        return defaultHandler;
    }
}
