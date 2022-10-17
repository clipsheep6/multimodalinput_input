#include "IEventHandler.h"
#include "TouchPadEventTransformer.h"
#include "Log.h"

namespace Input {

    std::list<std::shared_ptr<IEventHandler>> IEventHandler::PrepareHandlers(const IInputContext* context)
    {
        std::list<std::shared_ptr<IEventHandler>> result;
        if (context == nullptr) {
            return result;
        }

        return result;
    }

}
