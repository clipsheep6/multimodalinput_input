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

namespace OHOS {
namespace MMI {
std::shared_ptr<IKernelEventHandler> KernelEventHandlerBridge::CreateInstance(const std::shared_ptr<ITouchScreenHandler>& touchScreenHandle)
{
    return std::shared_ptr<IKernelEventHandler>(new KernelEventHandlerBridge(touchScreenHandle));
}

KernelEventHandlerBridge::KernelEventHandlerBridge(const std::shared_ptr<ITouchScreenHandler>& touchScreenHandle)
    : touchScreenHandle_(touchScreenHandle)
{
}

void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
{
    touchScreenHandle_->OnInputEvent(event);
}

std::shared_ptr<ITouchScreenHandler> KernelEventHandlerBridge::GetTouchEventHandler()
{
    return touchScreenHandle_;
}
} // namespace MMI
} // namespace OHOS