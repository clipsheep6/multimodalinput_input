/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "oh_input_interceptor.h"

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_manager.h"
#include "input_manager_impl.h"
#include "mmi_log.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "OHInputInterceptor"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t INVALID_INTERCEPTOR_ID = -1;
}
int32_t OHInputInterceptor::Start()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (interceptorId_ < 0) {
        interceptorId_ =  OHOS::Singleton<InputManagerImpl>::GetInstance()->AddMonitor(shared_from_this());
    }
    return interceptorId_;
}

void OHInputInterceptor::Stop()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (interceptorId_ < 0) {
        MMI_HILOGE("Invalid values");
        return;
    }
    OHOS::Singleton<InputManagerImpl>::GetInstance()->RemoveMonitor(interceptorId_);
    interceptorId_ = INVALID_INTERCEPTOR_ID;
    return;
}

void OHInputInterceptor::SetCallback(std::function<void(std::shared_ptr<PointerEvent>)> callback)
{
    std::lock_guard<std::mutex> guard(mutex_);
    callback_ = callback;
}

void OHInputInterceptor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mutex_);
    CHKPV(callback_);
    callback_(pointerEvent);
}
}
}