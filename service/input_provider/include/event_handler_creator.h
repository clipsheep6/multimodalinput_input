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

#ifndef EVENT_HANDLER_CREATOR_H
#define EVENT_HANDLER_CREATOR_H

#include "i_event_handler.h"
#include "i_event_handler_creator.h"

#include <map>
#include <memory>
#include <linux/input.h>

namespace OHOS {
namespace MMI {
class EventHandlerCreator : public IEventHandlerCreator {
public:
    std::shared_ptr<IEventHandler> CreateInstance(std::shared_ptr<IInputContext> context, int32_t capabilities);
    void ReleaseInstance(std::shared_ptr<IEventHandler> handler);
private:
    std::shared_ptr<IEventHandler> LoadAndCheckInputDeviceAlgo(std::string path, 
        std::shared_ptr<IInputContext> context, int32_t capabilities);
private:
    std::map<void *, std::tuple<GetIEventHandler *, int32_t>> soInfos_;
    std::map<std::shared_ptr<IEventHandler>, void *> eventHandlers_;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_HANDLER_CREATOR_H