#pragma once

#include "IInputDefine.h"
#include "KernelKeyEvent.h"
#include "KeyEvent.h"

namespace Input {

    class LogicalDisplayState;
    class KernelKeyEventHandler : public NonCopyable {
        public:
            KernelKeyEventHandler();
            virtual ~KernelKeyEventHandler() = default;

        public:
            std::shared_ptr<const KeyEvent> HandleEvent(const std::shared_ptr<const KernelKeyEvent>& event);

            void OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display);

        private:
            std::shared_ptr<const KeyEvent> HandleButton(const std::shared_ptr<const KernelKeyEvent>& event, bool& handled);

        private:
            std::shared_ptr<KeyEvent> keyEvent_;
    };

}
