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
int32_t OHInputInterceptor::Start(OHInterceptorType type)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (type == INTERCEPTOR_TYPE_KEY) {
        if (keyInterceptorId_ < 0) {
            keyInterceptorId_ =  InputMgrImpl.AddMonitor(shared_from_this());
        }
        return keyInterceptorId_;
    } else {
        if (pointerInterceptorId_ < 0) {
            pointerInterceptorId_ =  InputMgrImpl.AddMonitor(shared_from_this());
        }
        return pointerInterceptorId_;
    }
    
}

void OHInputInterceptor::Stop()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (keyInterceptorId_ < 0 && pointerInterceptorId_ < 0) {
        MMI_HILOGE("Invalid values");
        return;
    }
    if (keyInterceptorId_ != INVALID_INTERCEPTOR_ID) {
        InputMgrImpl.RemoveMonitor(keyInterceptorId_);
        keyInterceptorId_ = INVALID_INTERCEPTOR_ID;
    } else if (pointerInterceptorId_ != INVALID_INTERCEPTOR_ID) {
        InputMgrImpl.RemoveMonitor(pointerInterceptorId_);
        pointerInterceptorId_ = INVALID_INTERCEPTOR_ID;
    }
    return;
}

void OHInputInterceptor::SetCallback(std::function<void(std::shared_ptr<PointerEvent>)> callback)
{
    std::lock_guard<std::mutex> guard(mutex_);
    pointerCallback_ = callback;
}

void OHInputInterceptor::SetCallback(std::function<void(std::shared_ptr<KeyEvent>)> callback)
{
    std::lock_guard<std::mutex> guard(mutex_);
    keyCallback_ = callback;
}

void OHInputInterceptor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    std::lock_guard<std::mutex> guard(mutex_);
    CHKPV(pointerCallback_);
    pointerCallback_(pointerEvent);
}

void OHInputInterceptor::OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    std::lock_guard<std::mutex> guard(mutex_);
    CHKPV(keyCallback_);
    keyCallback_(keyEvent);
}

void OHInputInterceptor::OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const {}
}
}