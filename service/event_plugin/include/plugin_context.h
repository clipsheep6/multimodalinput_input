/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLUGIN_CONTEXT_H
#define PLUGIN_CONTEXT_H
#include "i_input_event_handle_plugin_context.h"

namespace OHOS {
namespace MMI {
class PluginContext : public IInputEventPluginContext
{
public:
    PluginContext() = default;
    virtual ~PluginContext() = default;
    void SetEventHandler(std::shared_ptr<IInputEventHandler> handler)
    {
        auto it = handlers_.find(handler->handlerType_);
        if (it == handlers_.end()) {
            handlers_.insert(handler->handlerType_, handler);
            return;
        }
        auto head = *it;
        if (head->handlerPriority_ > handler->handlerPriority_) {
            handler->SetNext(head);
            handlers_.emplace(std::make_pair(handler->handlerType_, handler));
            return;
        }
        for (auto cur = head; !cur; cur = cur->next) {
            auto next = cur->next;
            if (!next) {
                cur->SetNext(handler);
                return;
            }
            if (next->handlerPriority_ > handler->handlerPriority_) {
                cur->SetNext(handler);
                handler->SetNext(next);
                return;
            }
        }
        MMI_HILOGW("Cannot reach here! handlerType_:%{public}d", handler->handlerType_);
    }
    std::shared_ptr<IInputEventHandler> GetEventHandler(EventHandlerType type) const {
        auto it = handlers_.find(handler->handlerType_);
        if (it == handlers_.end()) {
            return nullptr;
        }
        return *it;
    }
private:
    std::map<EventHandlerType, std::shared_ptr<IInputEventHandler>> handlers_;
};
} // namespace MMI
} // namespace OHOS
#endif // PLUGIN_CONTEXT_H