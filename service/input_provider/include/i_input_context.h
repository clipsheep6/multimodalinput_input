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

#ifndef I_INPUT_CONTEXT
#define I_INPUT_CONTEXT

// compile: I_INPUT_CONTEXT

#include <memory>
#include <string>

#include "i_event_handler_manager.h"
#include "i_event_queue_manager.h"
#include "i_input_device_manager.h"
#include "i_input_provider_manager.h"

namespace OHOS {
namespace MMI {
class IInputProviderManager;
class IInputContext {
public:
    virtual ~IInputContext() = default;
    virtual std::shared_ptr<IInputDeviceManager> GetInputDeviceManager() = 0;
    virtual std::shared_ptr<IEventQueueManager> GetEventQueueManager() = 0;
    virtual std::shared_ptr<IEventHandlerManager> GetEventHandlerManager() = 0;
    virtual std::shared_ptr<IInputProviderManager> GetInputProviderManager() = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_CONTEXT