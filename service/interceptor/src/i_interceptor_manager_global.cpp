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

#include "i_interceptor_manager_global.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IInterceptorManagerGlobal" };
} // namespace

IInterceptorManagerGlobal::IInterceptorManagerGlobal(int32_t priority) : IInterceptorManagerGlobal(priority) {}

int32_t IInterceptorManagerGlobal::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    OnKeyEvent(keyEvent);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandleKeyEvent(keyEvent);
}
void IInterceptorManagerGlobal::OnAddInterceptor(int32_t sourceType, int32_t id, SessionPtr session)
{
    MMI_HILOGD("Add inter module does not support");
    return;
}

void IInterceptorManagerGlobal::OnRemoveInterceptor(int32_t id)
{
    MMI_HILOGD("Remove inter module does not support");
    return;
}

bool IInterceptorManagerGlobal::OnKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    MMI_HILOGD("Key inter module does not support");
    return false;
}

std::shared_ptr<IInterceptorManagerGlobal> IInterceptorManagerGlobal::CreateInstance(int32_t priority)
{
    return std::make_shared<IInterceptorManagerGlobal>(priority);
}
} // namespace MMI
} // namespace OHOS