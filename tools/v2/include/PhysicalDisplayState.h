#pragma once
#include <memory>
#include <ostream>

#include "IInputDefine.h"
#include "DisplayInfo.h"
#include "StreamUtils.h"

namespace Input {

    class IInputContext;
    class PhysicalDisplayState : public NonCopyable
    {
        public:
            static constexpr uint32_t CHANGED_NONE = 0;
            static constexpr uint32_t CHANGED_PHYSICAL_SIZE = 0b1;
            static constexpr uint32_t CHANGED_LOGICAL_SIZE = 0b10;
            static constexpr uint32_t CHANGED_LOCATION = 0b100;
            static constexpr uint32_t CHANGED_SEAT_ID = 0b1000;
            static constexpr uint32_t CHANGED_SEAT_NAME = 0b10000;
            static constexpr uint32_t CHANGED_NAME = 0b100000;

        public:
            static const std::unique_ptr<PhysicalDisplayState> NULL_VALUE;
        public:
            static std::unique_ptr<PhysicalDisplayState> CreateInstance(const IInputContext* context,
                    const PhysicalDisplay& display);

        public:
            virtual ~PhysicalDisplayState() = default;

            int32_t GetId() const;
            const std::string& GetSeatId() const;
            const std::string& GetSeatName() const;
            int32_t GetLeftDisplayId() const;
            int32_t GetUpDisplayId() const;
            int32_t GetWidth() const;
            int32_t GetHeight() const;

            uint32_t GetChanged() const;

            uint32_t Update(const PhysicalDisplay& display);

            int32_t TransformX(int32_t xPos, int32_t width) const;
            int32_t TransformY(int32_t yPos, int32_t height) const;

            std::ostream& operator<<(std::ostream& outStream) const;

        protected:
            PhysicalDisplayState(const IInputContext* context, const PhysicalDisplay& display);

        private:
            [[maybe_unused]] const IInputContext* const context_;

            int32_t id_;
            int32_t width_;
            int32_t height_;
            int32_t logicalWidth_;
            int32_t logicalHeight_;
            int32_t leftDisplayId_;
            int32_t upDisplayId_;
            std::string seatId_;
            std::string seatName_;
            std::string name_;
            uint32_t changed_;
    };

    std::ostream& operator<<(std::ostream& outStream, const PhysicalDisplay& display);
    std::ostream& operator<<(std::ostream& outStream, const PhysicalDisplayState& display);
}
