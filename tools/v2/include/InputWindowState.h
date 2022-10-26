#pragma once
#include <memory>
#include <ostream>

#include "IInputDefine.h"
#include "DisplayInfo.h"

namespace Input {

    class IInputContext;
    class InputWindowState : public NonCopyable
    {
        public:
            static constexpr uint32_t CHANGED_NONE = 0;
            static constexpr uint32_t CHANGED_POSITION = 0b1;
            static constexpr uint32_t CHANGED_SIZE = 0b10;
            static constexpr uint32_t CHANGED_DISPLAY = 0b100;
            static constexpr uint32_t CHANGED_AGENT = 0b1000;

        public:
            static std::unique_ptr<InputWindowState> CreateInstance(const IInputContext* context,
                    const WindowState& windowState);

        public:
            virtual ~InputWindowState() = default;

            int32_t GetId() const;
            int32_t GetDisplayId() const;
            uint32_t GetChanged() const;

            uint32_t Update(const WindowState& windowState);

            std::ostream& operator<<(std::ostream& outStream) const;

        protected:
            InputWindowState(const IInputContext* context, const WindowState& windowState);

        private:
            [[maybe_unused]] const IInputContext* const context_;

            const int32_t id_;
            int32_t topLeftX_;
            int32_t topLeftY_;
            int32_t width_;
            int32_t height_;
            int32_t displayId_;
            int32_t agentWindowId_;
            uint32_t changed_;
    };

    std::ostream& operator<<(std::ostream& outStream, const WindowState& windowState);
    std::ostream& operator<<(std::ostream& outStream, const InputWindowState& windowState);
}
