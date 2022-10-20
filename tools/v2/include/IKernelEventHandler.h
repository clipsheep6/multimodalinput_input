#pragma once
#include <string>
#include <memory>

namespace Input {
    class RelEvent;
    class KernelKeyEvent;
    class AbsEvent;
    class IKernelEventHandler {
        public:
            static const std::shared_ptr<IKernelEventHandler>& GetDefault();

            virtual ~IKernelEventHandler() = default;
            virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) = 0;
            virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) = 0;
            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
    };
}
