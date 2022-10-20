#pragma once

#include <string>
#include <map>

#include "IInputDefine.h"
#include "RelEvent.h"
#include "KernelKeyEvent.h"
#include "PointerEvent.h"
#include "LogicalDisplayState.h"

namespace Input {

    class IInputContext;
    class ISeat;
    class RelEventHandler : public NonCopyable {
        public:
            std::shared_ptr<const PointerEvent> HandleEvent(const std::shared_ptr<const RelEvent>& event);
            std::shared_ptr<const PointerEvent> HandleEvent(const std::shared_ptr<const KernelKeyEvent>& event, bool& consumed);

            void OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display);

        public:
            RelEventHandler(const IInputContext* context, const ISeat* seat);
            virtual ~RelEventHandler() = default;

        private:
            void DispatchEvent();
            std::shared_ptr<const LogicalDisplayState> AutofixTargetDisplay();

            std::shared_ptr<const PointerEvent> HandleButtonDownEvent(int32_t buttonId, int32_t deviceId, int64_t actionTime);
            std::shared_ptr<const PointerEvent> HandleButtonUpEvent(int32_t buttonId, int32_t deviceId, int64_t actionTime);

            int32_t AddPressedButton(int32_t buttonId, int32_t deviceId);
            int32_t RemovePressedButton(int32_t buttonId, int32_t deviceId);

            int32_t KeyCodeToButtonId(int32_t keyCode) const;

            std::shared_ptr<const LogicalDisplayState> GetDisplay(int32_t displayId) const;

            int32_t FindTarget(int32_t globalX, int32_t globalY, 
                    int32_t& targetDisplayId, int32_t& localX, int32_t localY) const;

        private:
            const IInputContext* const context_;
            const ISeat* const seat_;
            std::shared_ptr<PointerEvent> pointerEvent_;
            std::shared_ptr<PointerEvent::PointerItem> pointer_;
            std::map<int32_t, std::list<int32_t>> pressedButtons_;
            std::map<int32_t, std::shared_ptr<const LogicalDisplayState>> displays_;
    };

}
