#pragma once
#include <memory>
#include <map>
#include <list>

#include "IInputDefine.h"
#include "AbsEvent.h"

namespace Input {
    class AbsEventCollector : public NonCopyable {
        public:
            AbsEventCollector(int32_t deviceId, int32_t sourceType);
            virtual ~AbsEventCollector() = default;

            const std::shared_ptr<AbsEvent>& HandleAbsEvent(int32_t code, int32_t value);
            const std::shared_ptr<AbsEvent>& HandleSyncEvent(int32_t code, int32_t value);

            void AfterProcessed();

            int32_t SetSourceType(int32_t sourceType);

        protected:
            const std::shared_ptr<AbsEvent>& HandleMtSlot(int32_t value);
            void HandleMtPositionX(int32_t value);
            void HandleMtPositionY(int32_t value);
            const std::shared_ptr<AbsEvent>& HandleMtTrackingId(int32_t value);

            std::shared_ptr<AbsEvent::Pointer> GetCurrentPointer(bool createIfNotExist);
            const std::shared_ptr<AbsEvent>& FinishPointer();

            void RemoveReleasedPointer();

        private:
            [[maybe_unused]] const int32_t deviceId_;
            int32_t sourceType_;
            int32_t curSlot_;
            int32_t nextId_;
            std::shared_ptr<AbsEvent::Pointer> curPointer_;
            int32_t absEventAction_ {AbsEvent::ACTION_NONE};
            std::shared_ptr<AbsEvent> absEvent_;
            std::map<int32_t, std::shared_ptr<AbsEvent::Pointer>> pointers_;

    };
}
