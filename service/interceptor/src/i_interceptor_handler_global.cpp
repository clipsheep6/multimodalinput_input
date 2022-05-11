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

#include "i_interceptor_handler_global.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IInterceptorHandlerGlobal" };
} // namespace

IInterceptorHandlerGlobal::IInterceptorHandlerGlobal(int32_t priority) : IInputEventHandler(priority) {}

int32_t IInterceptorHandlerGlobal::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    HandleEvent(pointerEvent);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandlePointerEvent(pointerEvent);
}

int32_t IInterceptorHandlerGlobal::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    HandleEvent(pointerEvent);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandlePointerEvent(pointerEvent);
}

int32_t IInterceptorHandlerGlobal::AddInputHandler(int32_t handlerId,
    InputHandlerType handlerType, SessionPtr session)
{
    MMI_HILOGD("Add hdl module does not support");
    return RET_ERR;
}

void IInterceptorHandlerGlobal::RemoveInputHandler(int32_t handlerId,
    InputHandlerType handlerType, SessionPtr session)
{
    MMI_HILOGD("Remove hdl module does not support");
    return;
}

bool IInterceptorHandlerGlobal::HandleEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    MMI_HILOGD("Key handle module does not support");
    return false;
}

bool IInterceptorHandlerGlobal::HandleEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    MMI_HILOGD("Pointer handle module does not support");
    return false;
}

std::shared_ptr<IInterceptorHandlerGlobal> IInterceptorHandlerGlobal::CreateInstance(int32_t priority)
{
    return std::make_shared<IInterceptorHandlerGlobal>(priority);
}
} // namespace MMI
} // namespace OHOS
