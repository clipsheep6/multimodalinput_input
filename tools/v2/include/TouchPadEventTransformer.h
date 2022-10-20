#include <memory>
#include <map>
#include <list>
#include <string>

#include "IInputDefine.h"
#include "IEventTransformer.h"

namespace Input {

    class IInputContext;
    class TouchPadEventTransformer : public NonCopyable, public IEventTransformer {
        public:
            static std::shared_ptr<TouchPadEventTransformer> Create(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);
            
        public:
            TouchPadEventTransformer();
            virtual bool HandleEvent(const std::shared_ptr<const KeyEvent>& event) override;
            virtual bool HandleEvent(const std::shared_ptr<const PointerEvent>& event) override;
            virtual const std::string& GetName() const override;

        public:
            virtual ~TouchPadEventTransformer() = default;

        protected:
            TouchPadEventTransformer(const IInputContext* context, const std::shared_ptr<INewEventListener>& listener);

        private:
            const std::list<std::shared_ptr<IEventTransformer>>& PrepareTransformers(int32_t deviceId);

        private:
            const IInputContext* const context_;
            const std::shared_ptr<INewEventListener> listener_;
            const std::string name_;
            std::map<int32_t, std::list<std::shared_ptr<IEventTransformer>>> transformers_;
    };

}
