#pragma once
#include <map>
#include <string>

#include "IInputDefine.h"
#include "PointerEvent.h"
#include "AbsEvent.h"
#include "LogicalDisplayState.h"

namespace Input {
    class IInputContext;
    class AbsEventHandler : public NonCopyable {
        public:
            AbsEventHandler(const IInputContext* context, const std::string& seatId);
            virtual ~AbsEventHandler() = default;

            std::shared_ptr<const PointerEvent> HandleEvent(const std::shared_ptr<const AbsEvent>& absEvent);

            void OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display);
            void OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display);

        private:
            int32_t ConvertSourceType(int32_t absEventSourceType) const;
            int32_t ConvertAction(int32_t absEventAction) const;
            std::shared_ptr<PointerEvent::PointerItem> ConvertPointer(const std::shared_ptr<const AbsEvent::Pointer>& absEventPointer) const;

        private:
            [[maybe_unused]] const IInputContext* const context_;
            const std::string seatId_;
    };

}
