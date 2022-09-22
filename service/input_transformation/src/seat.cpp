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

#include "i_input_context.h"
#include "i_seat_manager.h"
#include "i_input_device.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "Seat" };
};
Seat::NewEventListener::NewEventListener(Seat* seat)
    : seat_(seat) 
{
}

void Seat::NewEventListener::OnEvent(const std::shared_ptr<const PointerEvent>& event)
{
    if (seat_ == nullptr) {
        MMI_HILOGE("Leave, null seat_");
        return;
    }

    // seat_->DispatchEvent(event);
}

void Seat::NewEventListener::Reset()
{
    seat_ = nullptr;    
}

std::unique_ptr<Seat> Seat::CreateInstance(const IInputContext* context, const std::string& seatId) {
    if (!context) {
       return nullptr;
    }

    return std::unique_ptr<Seat>(new Seat(context, seatId));
}

Seat::Seat(const IInputContext* context, const std::string& seatId)
    : context_(context), 
    seatId_(seatId), 
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

void Seat::OnInputEvent(const std::shared_ptr<const AbsEvent>& event) {
    MMI_HILOGD("Enter AbsEvent:%{public}p", event.get());

    auto pointerEvent = absEventHandler_.HandleEvent(event);
    if (pointerEvent) {
        //DispatchEvent(pointerEvent);
    }

    MMI_HILOGD("Leave AbsEvent");
}

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
    return inputDevices_.empty();
}

const std::string& Seat::GetSeatId() const {
    return seatId_;
}

bool Seat::TryTransform(const std::shared_ptr<const PointerEvent>& event)
{
    bool consumed = false;
    for (const auto& transformer : transformers_)  {
        MMI_HILOGD("Enter Transformer:%{public}s", transformer->GetName().c_str());
        if (transformer->HandleEvent(event)) {
            consumed = true;
            MMI_HILOGD("KeyEvent %{public}p Handled by %{public}s", event.get(), transformer->GetName().c_str());
            break;
        }
    }
    return consumed;
}
} // namespace MMI
} // namespace OHOS