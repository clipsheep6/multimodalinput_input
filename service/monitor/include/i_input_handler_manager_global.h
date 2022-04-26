/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef I_INPUT_HANDLER_MANAGER_GLOBAL_H
#define I_INPUT_HANDLER_MANAGER_GLOBAL_H

#include "define_multimodal.h"
#include "uds_session.h"
#include "key_event.h"
#include "pointer_event.h"
#include "input_handler_type.h"

namespace OHOS {
namespace MMI {
class IInputHandlerManagerGlobal {
public:
    IInputHandlerManagerGlobal() = default;
    virtual ~IInputHandlerManagerGlobal() = default;
    static std::shared_ptr<IInputHandlerManagerGlobal> GetInstance();
    virtual int32_t AddInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
    {
        return RET_ERR;
    }
    virtual void RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
    {
        return;
    }
    virtual void MarkConsumed(int32_t handlerId, int32_t eventId, SessionPtr session)
    {
        return;
    }
    virtual bool HandleEvent(std::shared_ptr<KeyEvent> KeyEvent)
    {
        return false;
    }
    virtual bool HandleEvent(std::shared_ptr<PointerEvent> PointerEvent)
    {
        return false;
    }

public:
    static inline std::shared_ptr<IInputHandlerManagerGlobal> iInputHandlerManagerGlobal_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_HANDLER_MANAGER_GLOBAL_H