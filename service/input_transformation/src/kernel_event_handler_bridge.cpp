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

#include "kernel_event_handler_bridge.h"

#include "define_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KernelEventHandlerBridge" };
}

std::shared_ptr<IKernelEventHandler> KernelEventHandlerBridge::CreateInstance(const std::shared_ptr<ITouchScreenHandler> handler)
{
    return std::shared_ptr<IKernelEventHandler>(new KernelEventHandlerBridge(handler));
}

KernelEventHandlerBridge::KernelEventHandlerBridge(const std::shared_ptr<ITouchScreenHandler> handler)
    : touchScreenHandle_(handler)
{}

void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<KernelKeyEvent> event) {}

void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<AbsEvent> event)
{
    CHKPV(touchScreenHandle_);
    touchScreenHandle_->OnInputEvent(event);
}
} // namespace MMI
} // namespace OHOS