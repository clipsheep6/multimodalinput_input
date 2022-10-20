#pragma once
#include <set>
#include <memory>

#include "ISeat.h"
#include "IInputDefine.h"
#include "RelEventHandler.h"
#include "KernelKeyEventHandler.h"
#include "AbsEventHandler.h"
#include "LogicalDisplayState.h"
#include "IEventTransformer.h"

namespace Input {

    class Seat : public NonCopyable, public ISeat {
        public:
            static std::unique_ptr<Seat> CreateInstance(const IInputContext* context, const std::string& seatId);

        public:
            virtual ~Seat();

            virtual const std::string& GetSeatId() const override;

            virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) override;
            virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) override;
            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override;

            virtual int32_t AddDisplay(const std::shared_ptr<LogicalDisplayState>& display) override;
            virtual int32_t RemoveDisplay(const std::shared_ptr<LogicalDisplayState>& display) override;
            virtual int32_t UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& display) override;
            virtual std::list<std::shared_ptr<LogicalDisplayState>> GetDisplays() const override;

            virtual int32_t AddInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;
            virtual int32_t RemoveInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;
            virtual std::list<std::shared_ptr<IInputDevice>> GetInputDevices() const override;

            virtual bool IsEmpty() const override;

        protected:
            Seat(const IInputContext* context, const std::string& seatId);

            class NewEventListener : public NonCopyable, public IEventTransformer::INewEventListener {
                public:
                    NewEventListener(Seat* seat);
                    virtual ~NewEventListener() = default;
                public:
                    virtual void OnEvent(const std::shared_ptr<const KeyEvent>& event) override;
                    virtual void OnEvent(const std::shared_ptr<const PointerEvent>& event) override;
                    virtual void OnEvent(const std::shared_ptr<const RelEvent>& event) override;
                    virtual void OnEvent(const std::shared_ptr<const KernelKeyEvent>& event) override;
                    void Reset();
                private:
                    Seat* seat_;
            };

        private:
            void DispatchEvent(const std::shared_ptr<const KeyEvent>& event);
            void DispatchEvent(const std::shared_ptr<const PointerEvent>& event);

            bool TryTransform(const std::shared_ptr<const KeyEvent>& event);
            bool TryTransform(const std::shared_ptr<const PointerEvent>& event);

        private:
            const IInputContext* const context_;
            const std::string seatId_;
            std::list<std::shared_ptr<IInputDevice>> inputDevices_;
            std::list<std::shared_ptr<LogicalDisplayState>> displays_;
            RelEventHandler relEventHandler_;
            KernelKeyEventHandler kernelKeyEventHandler_;
            AbsEventHandler absEventHandler_;

            std::shared_ptr<NewEventListener> newEventListener_;
            std::list<std::shared_ptr<IEventTransformer>> transformers_;
    };

}
