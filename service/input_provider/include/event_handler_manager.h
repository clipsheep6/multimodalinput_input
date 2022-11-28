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

#ifndef EVENT_HANDLER_MANAGER
#define EVENT_HANDLER_MANAGER

#include <memory>
#include <map>

#include "i_event_handler_manager.h"

namespace OHOS {
namespace MMI {
class EventHandlerManager : public IEventhandlerManager {
public:
    EventHandlerManager();
    virtual ~EventHandlerManager();
    DISALLOW_COPY_AND_MOVE(EventHandlerManager);

    virtual void DispatchEvent(PointerEvent event) override;
    virtual void DispatchEvent(KeyEvent event) override;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_HANDLER_MANAGER