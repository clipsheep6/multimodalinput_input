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

#ifndef I_EVENT_HANDLER_CREATOR_H
#define I_EVENT_HANDLER_CREATOR_H

#include "i_event_handler.h"

#include <memory>

namespace OHOS {
namespace MMI {
class IInputContext;
class IEventHandlerCreator {
public:
    virtual ~IEventHandlerCreator() = default;
    virtual std::shared_ptr<IEventHandler> CreateInstance(std::shared_ptr<IInputContext> context, int32_t capabilities) = 0;
    virtual void ReleaseInstance(std::shared_ptr<IEventHandler> handler) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_HANDLER_CREATOR_H