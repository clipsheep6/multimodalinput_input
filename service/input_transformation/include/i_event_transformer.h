/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_EVENT_TRANSFORMER_H
#define I_EVENT_TRANSFORMER_H

#include <memory>
#include <list>

#include "i_event_handler.h"

namespace OHOS {
namespace MMI {
class IInputContext;
class IEventTransformer : public IEventHandler {
public:
    class INewEventListener {
        public:
            virtual ~INewEventListener() = default;
            virtual void OnEvent(const std::shared_ptr<const PointerEvent>& event) = 0;
    };

    static std::list<std::shared_ptr<IEventTransformer>> CreateTransformers(const IInputContext* context, 
                    const std::shared_ptr<INewEventListener>& listener);
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_TRANSFORMER_H