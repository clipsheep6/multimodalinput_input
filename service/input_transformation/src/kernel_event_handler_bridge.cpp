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

// #include "Log.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
std::shared_ptr<IKernelEventHandler> KernelEventHandlerBridge::CreateInstance(const std::shared_ptr<ISeat>& seat, 
        const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat)
{
    if (!seat) {
        return nullptr;
    }

    if (!touchScreenSeat) {
        return std::shared_ptr<IKernelEventHandler>(
                new KernelEventHandlerBridge(seat, ITouchScreenSeat::GetDefault()));
    }

    return std::shared_ptr<IKernelEventHandler>(new KernelEventHandlerBridge(seat, 
                touchScreenSeat));
}

// void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const RelEvent>& event)
// {
//     seat_->OnInputEvent(event);
// }

// void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event)
// {
//     seat_->OnInputEvent(event);
// }

void KernelEventHandlerBridge::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
{
    if (event->GetSourceType() == AbsEvent::SOURCE_TYPE_TOUCHPAD) {
        seat_->OnInputEvent(event);
        return;
    }

    touchScreenSeat_->OnInputEvent(event);
}
KernelEventHandlerBridge::KernelEventHandlerBridge(const std::shared_ptr<ISeat>& seat, 
        const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat)
    : seat_(seat), touchScreenSeat_(touchScreenSeat)
{
}
} // namespace MMI
} // namespace OHOS