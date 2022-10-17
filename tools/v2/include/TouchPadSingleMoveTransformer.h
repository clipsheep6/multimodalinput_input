#include <memory>
#include <vector>
#include <string>

#include "IEventTransformer.h"
#include "IInputDefine.h"
#include "RelEvent.h"

namespace Input {

    class IInputContext;
    class TouchPadSingleMoveTransformer : public NonCopyable, public IEventTransformer {
        public:
            static std::shared_ptr<TouchPadSingleMoveTransformer> Create(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);

        public:
            virtual bool HandleEvent(const std::shared_ptr<const KeyEvent>& event) override;
            virtual bool HandleEvent(const std::shared_ptr<const PointerEvent>& event) override;
            virtual const std::string& GetName() const override;

        protected:
            TouchPadSingleMoveTransformer(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);

        private:
            enum State {Waiting, Moving, Dead};

        private:
            void ProcessOnWaiting(const std::shared_ptr<const PointerEvent>& event);
            void ProcessOnMoving(const std::shared_ptr<const PointerEvent>& event);
            void ProcessOnDead(const std::shared_ptr<const PointerEvent>& event);

            int32_t GetTouchedPointerCount(const std::shared_ptr<const PointerEvent>& event);

        private:
            [[maybe_unused]] const IInputContext* const context_;
            const std::shared_ptr<INewEventListener>& listener_;
            const std::string name_;

            State state_ {Waiting};
            int32_t pointerId_{-1};
            int32_t preX_{0};
            int32_t preY_{0};
            int32_t minDeltaX_{200};
            int32_t minDeltaY_{200};

            std::shared_ptr<RelEvent> relEvent_;
    };

}

