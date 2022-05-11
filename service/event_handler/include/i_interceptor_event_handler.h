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
#ifndef I_INTERCEPTOR_EVENT_HANDLER_H
#define I_INTERCEPTOR_EVENT_HANDLER_H

#include <memory>

#include "i_input_event_handler.h"
#include "input_handler_type.h"
#include "key_option.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class IInterceptorEventHandler : public IInputEventHandler {
public:
    virtual int32_t AddInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif  // I_INTERCEPTOR_EVENT_HANDLER_H
