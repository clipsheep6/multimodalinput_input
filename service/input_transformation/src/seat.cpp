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

#include "seat.h"

#include <algorithm>

// #include "IInputContext.h"
#include "i_seat_manager.h"
#include "i_input_device.h"
// #include "IEventDispatcher.h"
// #include "IInputDevice.h"
// #include "Log.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {

Seat::NewEventListener::NewEventListener(Seat* seat)
    : seat_(seat) 
{
}

// void Seat::NewEventListener::OnEvent(const std::shared_ptr<const KeyEvent>& event)
// {
//     if (seat_ == nullptr) {
//         MMI_HILOGE("Leave, null seat_");
//         return;
//     }

//     seat_->DispatchEvent(event);
// }

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const PointerEvent>& event)
{
    if (seat_ == nullptr) {
        MMI_HILOGE("Leave, null seat_");
        return;
    }

    seat_->DispatchEvent(event);
}

// void Seat::NewEventListener::OnEvent(const std::shared_ptr<const RelEvent>& event)
// {
//     if (seat_ == nullptr) {
//         MMI_HILOGE("Leave, null seat_");
//         return;
//     }

//     MMI_HILOGD("RelEvent");
//     seat_->OnInputEvent(event);
// }

// void Seat::NewEventListener::OnEvent(const std::shared_ptr<const KernelKeyEvent>& event)
// {
//     if (seat_ == nullptr) {
//         MMI_HILOGE("Leave, null seat_");
//         return;
//     }

//     MMI_HILOGD("KernelKeyEvent");
//     seat_->OnInputEvent(event);
// }

void Seat::NewEventListener::Reset()
{
    seat_ = nullptr;    
}

std::unique_ptr<Seat> Seat::CreateInstance( const std::string& seatId) {
    // if (!context) {
    //     errno = EINVAL;
    //     return nullptr;
    // }

    return std::unique_ptr<Seat>(new Seat(seatId));
}

Seat::Seat(const std::string& seatId)
    seatId_(seatId), 
    absEventHandler_(seatId) ,
    newEventListener_(std::shared_ptr<NewEventListener>(new NewEventListener(this))), 
    transformers_(IEventTransformer::CreateTransformers(newEventListener_))
{
}

Seat::~Seat()
{
    if (newEventListener_) {
        newEventListener_->Reset();
    }
}

// void Seat::OnInputEvent(const std::shared_ptr<const RelEvent>& event) {
//     MMI_HILOGD("Enter RelEvent");
//     auto pointerEvent = relEventHandler_.HandleEvent(event);
//     if (pointerEvent) {
//         DispatchEvent(pointerEvent);
//     }
//     MMI_HILOGD("Leave RelEvent");
// }

// void Seat::OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) {
//     MMI_HILOGD("Enter KernelKeyEvent");
//     bool consumed = false;
//     auto pointerEvent = relEventHandler_.HandleEvent(event, consumed);
//     if (pointerEvent) {
//         DispatchEvent(pointerEvent);
//     }
//     if (consumed) {
//         MMI_HILOGD("Leave KernelKeyEvent, Consumed By RelEventHandler");
//         return;
//     }

//     auto keyEvent = kernelKeyEventHandler_.HandleEvent(event);
//     if (keyEvent) {
//         DispatchEvent(keyEvent);
//     }

//     MMI_HILOGD("Leave KernelKeyEvent");
// }

void Seat::OnInputEvent(const std::shared_ptr<const AbsEvent>& event) {
    MMI_HILOGD("Enter AbsEvent:$s", event);

    auto pointerEvent = absEventHandler_.HandleEvent(event);
    if (pointerEvent) {
        DispatchEvent(pointerEvent);
    }

    MMI_HILOGD("Leave AbsEvent");
}

// int32_t Seat::AddDisplay(const std::shared_ptr<LogicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     if (!display) {
//         MMI_HILOGE("Leave, null display");
//         return -1;
//     }

//     for (const auto& item : displays_) {
//         if (item == display) {
//             MMI_HILOGE("Leave, Already Added");
//             return -1;
//         }
//     }

//     if (seatId_ != display->GetSeatId()) {
//         MMI_HILOGE("Leave, seatId Mismatch");
//         return -1;
//     }

//     displays_.push_back(display);

//     auto displayId = display->GetId();
//     kernelKeyEventHandler_.OnDisplayAdded(display);
//     relEventHandler_.OnDisplayAdded(display);
//     absEventHandler_.OnDisplayAdded(display);

//     MMI_HILOGD("Leave displayId:$s", displayId);
//     return 0;
// }

// int32_t Seat::RemoveDisplay(const std::shared_ptr<LogicalDisplayState>& display) {
//     MMI_HILOGD("Enter display:$s", display);

//     if (!display) {
//         MMI_HILOGE("Leave, null display", display);
//         return -1;
//     }

//     auto it = std::find(displays_.begin(), displays_.end(), display);
//     if (it == displays_.end()) {
//         MMI_HILOGE("Leave, display not it seat");
//         return -1;
//     }

//     displays_.erase(it);

//     auto displayId = display->GetId();
//     absEventHandler_.OnDisplayRemoved(display);
//     relEventHandler_.OnDisplayRemoved(display);
//     kernelKeyEventHandler_.OnDisplayRemoved(display);

//     MMI_HILOGD("Leave displayId:$s", displayId);
//     return 0;
// }

// int32_t Seat::UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);

//     if (!display) {
//         MMI_HILOGE("Leave, null display", display);
//         return -1;
//     }

//     auto it = std::find(displays_.begin(), displays_.end(), display);
//     if (it == displays_.end()) {
//         MMI_HILOGE("Leave, display not it seat");
//         return -1;
//     }

//     displays_.erase(it);

//     auto displayId = display->GetId();
//     kernelKeyEventHandler_.OnDisplayChanged(display);
//     relEventHandler_.OnDisplayChanged(display);
//     absEventHandler_.OnDisplayChanged(display);

//     MMI_HILOGD("Leave displayId:$s", displayId);
//     return 0;
// }

// std::list<std::shared_ptr<LogicalDisplayState>> Seat::GetDisplays() const {
//     return displays_;
// }

int32_t Seat::AddInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (!inputDevice) {
        MMI_HILOGE("Leave, null inputDevice");
        return -1;
    }

    for (const auto& item : inputDevices_) {
        if (item == inputDevice) {
            MMI_HILOGE("Leave, Alreay Added");
            return -1;
        }
    }

    inputDevices_.push_back(inputDevice);

    MMI_HILOGD("Leave");
    return 0;
}

int32_t Seat::RemoveInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (!inputDevice) {
        MMI_HILOGE("Leave, null inputDevice");
        return -1;
    }

    for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ++it) {
        if ((*it) == inputDevice) {
            inputDevices_.erase(it);
            MMI_HILOGD("Leave");
            return 0;
        }
    }

    MMI_HILOGD("Leave, inputDevice not in seat");
    return -1;
}

std::list<std::shared_ptr<IInputDevice>> Seat::GetInputDevices() const
{
    return inputDevices_;
}

bool Seat::IsEmpty() const
{
    return displays_.empty() && inputDevices_.empty();
}

void Seat::DispatchEvent(const std::shared_ptr<const KeyEvent>& event) {
    MMI_HILOGD("Enter");
    if (!event) {
        MMI_HILOGE("Leave, null event");
        return;
    }

    if (TryTransform(event)) {
        MMI_HILOGD("Leave, consumed by Transformer");
        return;
    }

    const auto& eventDispatcher = context_->GetEventDispatcher();
    if (!eventDispatcher) {
        MMI_HILOGE("Leave, null eventDispatcher");
        return;
    }

    eventDispatcher->DispatchEvent(event);
    MMI_HILOGD("Leave");
}

void Seat::DispatchEvent(const std::shared_ptr<const PointerEvent>& event) {
    MMI_HILOGD("Enter PointerEvent:$s", PointerEvent::SourceTypeToString(event->GetSourceType()));
    if (!event) {
        MMI_HILOGE("Leave PointerEvent, null event");
        return;
    }

    if (TryTransform(event)) {
        MMI_HILOGD("Leave PointerEvent, consumed by Transformer");
        return;
    }

    const auto& eventDispatcher = context_->GetEventDispatcher();
    if (!eventDispatcher) {
        MMI_HILOGE("Leave PointerEvent, null eventDispatcher");
        return;
    }

    eventDispatcher->DispatchEvent(event);
    MMI_HILOGD("Leave PointerEvent");
}

const std::string& Seat::GetSeatId() const {
    return seatId_;
}

bool Seat::TryTransform(const std::shared_ptr<const KeyEvent>& event)
{
    bool consumed = false;
    for (const auto& transformer : transformers_)  {
        MMI_HILOGD("Enter Transformer:$s", transformer->GetName());
        if (transformer->HandleEvent(event)) {
            consumed = true;
            MMI_HILOGD("KeyEvent $s Handled by $s", event, transformer->GetName());
            break;
        }
    }
    return consumed;
}

bool Seat::TryTransform(const std::shared_ptr<const PointerEvent>& event)
{
    bool consumed = false;
    for (const auto& transformer : transformers_)  {
        MMI_HILOGD("Enter Transformer:$s", transformer->GetName());
        if (transformer->HandleEvent(event)) {
            consumed = true;
            MMI_HILOGD("KeyEvent $s Handled by $s", event, transformer->GetName());
            break;
        }
    }
    return consumed;
}
} // namespace MMI
} // namespace OHOS