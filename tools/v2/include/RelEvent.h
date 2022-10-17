#pragma once
#include <cstdint>
#include <memory>
#include <ostream>

#include "KernelEventBase.h"

namespace Input {
    class RelEvent : public KernelEventBase {
        public:
            static constexpr int32_t ACTION_NONE = 0;
            static constexpr int32_t ACTION_MOVE = 1;

        public:
            RelEvent(int32_t deviceId);
            ~RelEvent() = default;

            int32_t GetRelX() const;
            int32_t GetRelY() const;

            virtual std::ostream& operator<<(std::ostream& outStream) const override;

            void AddRelX(int32_t relX);
            void AddRelY(int32_t relY);

            void SetRelX(int32_t relX);
            void SetRelY(int32_t relY);
            
        protected:
            virtual const char* ActionToStr(int32_t action) const override;
            std::ostream& PrintInternal(std::ostream& outStream) const;

        private:
            int32_t relX_;
            int32_t relY_;
    };

    std::ostream& operator<<(std::ostream& outStream, const RelEvent& event);
    std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<const RelEvent>& event);
    std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<const RelEvent>& event);
}
