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

#include <algorithm>

#include "seat.h"
#include "i_seat_manager.h"
#include "i_input_device.h"
// #include "IEventDispatcher.h"
#include "i_input_device.h"
// #include "Log.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {

Seat::NewEventListener::NewEventListener(Seat* seat)
    : seat_(seat) 
{
}

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const KeyEvent>& event)
{
    if (seat_ == nullptr) {
        LOG_E("Leave, null seat_");
        return;
    }

    seat_->DispatchEvent(event);
}

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const PointerEvent>& event)
{
    if (seat_ == nullptr) {
        LOG_E("Leave, null seat_");
        return;
    }

    seat_->DispatchEvent(event);
}

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const RelEvent>& event)
{
    if (seat_ == nullptr) {
        LOG_E("Leave, null seat_");
        return;
    }

    LOG_D("RelEvent");
    seat_->OnInputEvent(event);
}

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const KernelKeyEvent>& event)
{
    if (seat_ == nullptr) {
        LOG_E("Leave, null seat_");
        return;
    }

    LOG_D("KernelKeyEvent");
    seat_->OnInputEvent(event);
}

void Seat::NewEventListener::Reset()
{
    seat_ = nullptr;    
}

std::unique_ptr<Seat> Seat::CreateInstance(const IInputContext* context, const std::string& seatId) {
    if (!context) {
        errno = EINVAL;
        return nullptr;
    }

    return std::unique_ptr<Seat>(new Seat(context, seatId));
}

Seat::Seat(const IInputContext* context, const std::string& seatId)
    : context_(context), 
    seatId_(seatId), 
    relEventHandler_(context, this), 
    absEventHandler_(context_, seatId) ,
    newEventListener_(std::shared_ptr<NewEventListener>(new NewEventListener(this))), 
    transformers_(IEventTransformer::CreateTransformers(context, newEventListener_))
{
}

Seat::~Seat()
{
    if (newEventListener_) {
        newEventListener_->Reset();
    }
}

void Seat::OnInputEvent(const std::shared_ptr<const RelEvent>& event) {
    LOG_D("Enter RelEvent");
    auto pointerEvent = relEventHandler_.HandleEvent(event);
    if (pointerEvent) {
        DispatchEvent(pointerEvent);
    }
    LOG_D("Leave RelEvent");
}

void Seat::OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) {
    LOG_D("Enter KernelKeyEvent");
    bool consumed = false;
    auto pointerEvent = relEventHandler_.HandleEvent(event, consumed);
    if (pointerEvent) {
        DispatchEvent(pointerEvent);
    }
    if (consumed) {
        LOG_D("Leave KernelKeyEvent, Consumed By RelEventHandler");
        return;
    }

    auto keyEvent = kernelKeyEventHandler_.HandleEvent(event);
    if (keyEvent) {
        DispatchEvent(keyEvent);
    }

    LOG_D("Leave KernelKeyEvent");
}

void Seat::OnInputEvent(const std::shared_ptr<const AbsEvent>& event) {
    LOG_D("Enter AbsEvent:$s", event);

    auto pointerEvent = absEventHandler_.HandleEvent(event);
    if (pointerEvent) {
        DispatchEvent(pointerEvent);
    }

    LOG_D("Leave AbsEvent");
}

int32_t Seat::AddDisplay(const std::shared_ptr<LogicalDisplayState>& display)
{
    LOG_D("Enter display:$s", display);
    if (!display) {
        LOG_E("Leave, null display");
        return -1;
    }

    for (const auto& item : displays_) {
        if (item == display) {
            LOG_E("Leave, Already Added");
            return -1;
        }
    }

    if (seatId_ != display->GetSeatId()) {
        LOG_E("Leave, seatId Mismatch");
        return -1;
    }

    displays_.push_back(display);

    auto displayId = display->GetId();
    kernelKeyEventHandler_.OnDisplayAdded(display);
    relEventHandler_.OnDisplayAdded(display);
    absEventHandler_.OnDisplayAdded(display);

    LOG_D("Leave displayId:$s", displayId);
    return 0;
}

int32_t Seat::RemoveDisplay(const std::shared_ptr<LogicalDisplayState>& display) {
    LOG_D("Enter display:$s", display);

    if (!display) {
        LOG_E("Leave, null display", display);
        return -1;
    }

    auto it = std::find(displays_.begin(), displays_.end(), display);
    if (it == displays_.end()) {
        LOG_E("Leave, display not it seat");
        return -1;
    }

    displays_.erase(it);

    auto displayId = display->GetId();
    absEventHandler_.OnDisplayRemoved(display);
    relEventHandler_.OnDisplayRemoved(display);
    kernelKeyEventHandler_.OnDisplayRemoved(display);

    LOG_D("Leave displayId:$s", displayId);
    return 0;
}

int32_t Seat::UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& display)
{
    LOG_D("Enter display:$s", display);

    if (!display) {
        LOG_E("Leave, null display", display);
        return -1;
    }

    auto it = std::find(displays_.begin(), displays_.end(), display);
    if (it == displays_.end()) {
        LOG_E("Leave, display not it seat");
        return -1;
    }

    displays_.erase(it);

    auto displayId = display->GetId();
    kernelKeyEventHandler_.OnDisplayChanged(display);
    relEventHandler_.OnDisplayChanged(display);
    absEventHandler_.OnDisplayChanged(display);

    LOG_D("Leave displayId:$s", displayId);
    return 0;
}

std::list<std::shared_ptr<LogicalDisplayState>> Seat::GetDisplays() const {
    return displays_;
}

int32_t Seat::AddInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    LOG_D("Enter");
    if (!inputDevice) {
        LOG_E("Leave, null inputDevice");
        return -1;
    }

    for (const auto& item : inputDevices_) {
        if (item == inputDevice) {
            LOG_E("Leave, Alreay Added");
            return -1;
        }
    }

    inputDevices_.push_back(inputDevice);

    LOG_D("Leave");
    return 0;
}

int32_t Seat::RemoveInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    LOG_D("Enter");
    if (!inputDevice) {
        LOG_E("Leave, null inputDevice");
        return -1;
    }

    for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ++it) {
        if ((*it) == inputDevice) {
            inputDevices_.erase(it);
            LOG_D("Leave");
            return 0;
        }
    }

    LOG_D("Leave, inputDevice not in seat");
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
    LOG_D("Enter");
    if (!event) {
        LOG_E("Leave, null event");
        return;
    }

    if (TryTransform(event)) {
        LOG_D("Leave, consumed by Transformer");
        return;
    }

    const auto& eventDispatcher = context_->GetEventDispatcher();
    if (!eventDispatcher) {
        LOG_E("Leave, null eventDispatcher");
        return;
    }

    eventDispatcher->DispatchEvent(event);
    LOG_D("Leave");
}

void Seat::DispatchEvent(const std::shared_ptr<const PointerEvent>& event) {
    LOG_D("Enter PointerEvent:$s", PointerEvent::SourceTypeToString(event->GetSourceType()));
    if (!event) {
        LOG_E("Leave PointerEvent, null event");
        return;
    }

    if (TryTransform(event)) {
        LOG_D("Leave PointerEvent, consumed by Transformer");
        return;
    }

    const auto& eventDispatcher = context_->GetEventDispatcher();
    if (!eventDispatcher) {
        LOG_E("Leave PointerEvent, null eventDispatcher");
        return;
    }

    eventDispatcher->DispatchEvent(event);
    LOG_D("Leave PointerEvent");
}

const std::string& Seat::GetSeatId() const {
    return seatId_;
}

bool Seat::TryTransform(const std::shared_ptr<const KeyEvent>& event)
{
    bool consumed = false;
    for (const auto& transformer : transformers_)  {
        LOG_D("Enter Transformer:$s", transformer->GetName());
        if (transformer->HandleEvent(event)) {
            consumed = true;
            LOG_D("KeyEvent $s Handled by $s", event, transformer->GetName());
            break;
        }
    }
    return consumed;
}

bool Seat::TryTransform(const std::shared_ptr<const PointerEvent>& event)
{
    bool consumed = false;
    for (const auto& transformer : transformers_)  {
        LOG_D("Enter Transformer:$s", transformer->GetName());
        if (transformer->HandleEvent(event)) {
            consumed = true;
            LOG_D("KeyEvent $s Handled by $s", event, transformer->GetName());
            break;
        }
    }
    return consumed;
}
} // namespace MMI
} // namespace OHOS