#pragma once
#include <memory>
#include <ostream>
#include <list>
#include <functional>

#include "IInputDefine.h"
#include "DisplayInfo.h"
#include "InputWindowState.h"
#include "AbsEvent.h"
#include "PointerEvent.h"
#include "StreamUtils.h"

namespace Input {

    class IInputContext;
    class LogicalDisplayState : public NonCopyable
    {
        public:
            class UpdateAssist : public NonCopyable {
                public:
                    UpdateAssist() = default;
                    virtual ~UpdateAssist() = default;

                    virtual std::shared_ptr<InputWindowState> ProvideInputWindowState(const IInputContext* context, const WindowState& window);
                    virtual void RecordWindowAdded(std::shared_ptr<InputWindowState> inputWindow);
                    virtual void RecordWindowUpdate(std::shared_ptr<InputWindowState> inputWindow);
                    virtual void RecordWindowRemoved(std::shared_ptr<InputWindowState> inputWindow);
            };

        public:
            static constexpr uint32_t CHANGED_NONE = 0;
            static constexpr uint32_t CHANGED_POSITION = 0b1;
            static constexpr uint32_t CHANGED_SIZE = 0b10;
            static constexpr uint32_t CHANGED_LOCATION = 0b100;
            static constexpr uint32_t CHANGED_SEAT_ID = 0b1000;
            static constexpr uint32_t CHANGED_SEAT_NAME = 0b10000;

        public:
            static const std::shared_ptr<LogicalDisplayState> NULL_VALUE;

        public:
            static std::shared_ptr<LogicalDisplayState> CreateInstance(const IInputContext* context,
                    const LogicalDisplay& display, UpdateAssist& updateAssist);

        public:
            virtual ~LogicalDisplayState() = default;

            int32_t GetId() const;
            int32_t GetWidth() const;
            int32_t GetHeight() const;
            int32_t GetTopLeftX() const;
            int32_t GetTopLeftY() const;
            const std::string& GetSeatId() const;
            const std::string& GetSeatName() const;
            std::list<int32_t> GetWindowIds() const;
            uint32_t GetChanged() const;

            std::shared_ptr<InputWindowState> FindWindow(int32_t windowId) const;

            std::shared_ptr<InputWindowState> RemoveWindow(int32_t windowId);
            void RemoveAllWindows();
            uint32_t Update(const LogicalDisplay& display);

            std::ostream& operator<<(std::ostream& outStream) const;

            std::shared_ptr<PointerEvent> HandleEvent(int32_t pointerAction, int64_t actionTime,
                    const std::shared_ptr<const PointerEvent::PointerItem>& pointer) const;

            bool Transform(int32_t globalX, int32_t globalY, bool canOutOfDisplay, int32_t& displayX, int32_t& displayY) const;
            void MakeInDisplay(int32_t& displayX, int32_t& displayY) const;

        protected:
            LogicalDisplayState(const IInputContext* context, const LogicalDisplay& display,
                    UpdateAssist& updateAssist);

        private:
            void UpdateWindows(const std::vector<WindowState>& windows);

        private:
            const IInputContext* const context_;

            int32_t id_;
            int32_t topLeftX_;
            int32_t topLeftY_;
            int32_t width_;
            int32_t height_;
            int32_t leftDisplayId_;
            int32_t upDisplayId_;
            std::string seatId_;
            std::string seatName_;
            std::list<std::shared_ptr<InputWindowState>> inputWindows_;
            uint32_t changed_;

            UpdateAssist& assist_;

            std::shared_ptr<PointerEvent> pointerEvent_;
            mutable std::map<int64_t, int32_t> pointerIdMap_;
            mutable int32_t nextPointerId_{0};
    };

    std::ostream& operator<<(std::ostream& outStream, const LogicalDisplay& display);
    std::ostream& operator<<(std::ostream& outStream, const LogicalDisplayState& display);
}
