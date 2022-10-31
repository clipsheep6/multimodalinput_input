#include "IEventTransformer.h"
#include "TouchPadEventTransformer.h"
#include "Log.h"

namespace Input {
    std::list<std::shared_ptr<IEventTransformer>> IEventTransformer::CreateTransformers(const IInputContext* context, 
            const std::shared_ptr<INewEventListener>& listener)
    {
        std::list<std::shared_ptr<IEventTransformer>> result;
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return result;
        }

        if (!listener) {
            LOG_E("Leave, null listener");
            return result;
        }

        std::shared_ptr<IEventTransformer> transformer = TouchPadEventTransformer::Create(context, listener);
        if (!transformer) {
            LOG_W("TouchPadEventTransformer Create Failed");
        } else {
            result.push_back(transformer);
        }

        return result;
    }
}
