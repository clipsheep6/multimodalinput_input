#include <memory>
#include <vector>
#include <string>

#include "IEventTransformer.h"
#include "IInputDefine.h"

namespace Input {

    class IInputContext;
    class TouchPadSingleTapTransformer : public NonCopyable, public IEventTransformer {
        public:
            static std::shared_ptr<TouchPadSingleTapTransformer> Create(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);

        public:
            virtual bool HandleEvent(const std::shared_ptr<const KeyEvent>& event) override;
            virtual bool HandleEvent(const std::shared_ptr<const PointerEvent>& event) override;
            virtual const std::string& GetName() const override;

        protected:
            TouchPadSingleTapTransformer(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);

        private:
            enum State {Idle, FirstDown};

        private:
            void ProcessOnIdle(const std::shared_ptr<const PointerEvent>& event);
            void ProcessOnFirstDown(const std::shared_ptr<const PointerEvent>& event);

        private:
            [[maybe_unused]] const IInputContext* const context_;
            const std::shared_ptr<INewEventListener>& listener_;
            const std::string name_;

            State state_ {Idle};
            int32_t pointerId_ {-1};
            int32_t downX_ {-1};
            int32_t downY_ {-1};
            int64_t pointerDownTime_ {-1};

            int32_t maxIntervalMs_ {100};
            int32_t maxDeltaX_ {200};
            int32_t maxDeltaY_ {200};
    };

}

