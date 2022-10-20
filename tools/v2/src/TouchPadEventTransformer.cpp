#include "TouchPadEventTransformer.h"
#include "TouchPadSingleMoveTransformer.h"
#include "TouchPadSingleTapTransformer.h"
#include "PointerEvent.h"
#include "Log.h"

namespace Input {

    std::shared_ptr<TouchPadEventTransformer> TouchPadEventTransformer::Create(const IInputContext* context,
            const std::shared_ptr<INewEventListener>& listener)
    {
        if (context == nullptr) {
            return nullptr;
        }

        if (!listener) {
            return nullptr;
        }

        return std::shared_ptr<TouchPadEventTransformer>(new TouchPadEventTransformer(context, listener));
    }

    bool TouchPadEventTransformer::HandleEvent(const std::shared_ptr<const KeyEvent>& event)
    {
        LOG_D("Handle KeyEvent");
        return false;
    }

    bool TouchPadEventTransformer::HandleEvent(const std::shared_ptr<const PointerEvent>& event)
    {
        LOG_D("Enter");
        if (event->GetSourceType() != PointerEvent::SOURCE_TYPE_TOUCHPAD) {
            return false;
        }

        bool consumed = false;
        int32_t deviceId = event->GetDeviceId();
        const auto& transformers = PrepareTransformers(deviceId);
        for (const auto& transformer : transformers) {
            if (transformer->HandleEvent(event)) {
                consumed = true;
                break;
            }
        }

        LOG_D("Handle TouchPad PointerEvent consumed:$s", consumed);
        return consumed;
    }

    TouchPadEventTransformer::TouchPadEventTransformer(const IInputContext* context,
          const std::shared_ptr<INewEventListener>& listener)
        : context_(context), listener_(listener), name_("TouchPadEventTransformer")
    {
    }

    const std::list<std::shared_ptr<IEventTransformer>>& TouchPadEventTransformer::PrepareTransformers(int32_t deviceId)
    {
        auto it = transformers_.find(deviceId);
        if (it != transformers_.end()) {
            return it->second;
        }

        auto& transformers = transformers_[deviceId];
        {
            auto transformer = TouchPadSingleMoveTransformer::Create(context_, listener_);
            if (!transformer) {
                LOG_E("Create TouchPadSingleMoveTransformer Failed");
            } else {
                transformers.push_back(transformer);
            }
        }

        {
            auto transformer = TouchPadSingleTapTransformer::Create(context_, listener_);
            if (!transformer) {
                LOG_E("Create TouchPadSingleTapTransformer Failed");
            } else {
                transformers.push_back(transformer);
            }
        }

        return transformers;
    }

    const std::string& TouchPadEventTransformer::GetName() const
    {
        return name_;
    }
}
