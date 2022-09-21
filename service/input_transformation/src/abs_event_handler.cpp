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

#include "abs_event_handler.h"

#include "abs_event.h"
// #include "IInputContext.h"
// #include "IInputDeviceManager.h"
#include "pointer_event.h"
// #include "Log.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {

AbsEventHandler::AbsEventHandler(const std::string& seatId)
    : seatId_(seatId)
{
}

std::shared_ptr<const PointerEvent> AbsEventHandler::HandleEvent(const std::shared_ptr<const AbsEvent>& absEvent)
{
    if (!absEvent) {
        return PointerEvent::NULL_VALUE;
    }

    auto deviceId = absEvent->GetDeviceId();
    auto sourceType = ConvertSourceType(absEvent->GetSourceType());
    if (sourceType == PointerEvent::SOURCE_TYPE_NONE) {
        MMI_HILOGE("Leave, ConvertSourceType Failed");
        return PointerEvent::NULL_VALUE;
    }

    auto action = ConvertAction(absEvent->GetAction());
    if (action == PointerEvent::POINTER_ACTION_NONE) {
        MMI_HILOGE("Leave, ConvertAction Failed");
        return PointerEvent::NULL_VALUE;
    }

    auto pointerEvent = PointerEvent::CreateInstance(sourceType);
    if (!pointerEvent) {
        MMI_HILOGE("Leave, null pointerEvent");
        return PointerEvent::NULL_VALUE;
    }

    for (const auto& absEventPointer : absEvent->GetPointerList()) {
        auto pointer = ConvertPointer(absEventPointer);
        if (!pointer) {
            MMI_HILOGE("Leave, ConvertPointer Failed");
            return PointerEvent::NULL_VALUE;
        }
        pointer->SetDeviceId(deviceId);
        auto retCode = pointerEvent->AddPointer(pointer);
        if (retCode < 0) {
            MMI_HILOGE("Leave, AddPointer Failed");
            return PointerEvent::NULL_VALUE;
        }
    }

    pointerEvent->SetDeviceId(deviceId);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetPointerId(absEvent->GetPointerId());
    pointerEvent->SetActionTime(absEvent->GetActionTime());

    return pointerEvent;
}

// void AbsEventHandler::OnDisplayAdded([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
// {
// }

// void AbsEventHandler::OnDisplayRemoved([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
// {
// }

// void AbsEventHandler::OnDisplayChanged([[maybe_unused]] const std::shared_ptr<const LogicalDisplayState>& display)
// {
// }

int32_t AbsEventHandler::ConvertSourceType(int32_t absEventSourceType) const
{
    if (absEventSourceType == AbsEvent::SOURCE_TYPE_TOUCHPAD) {
        return PointerEvent::SOURCE_TYPE_TOUCHPAD;
    }

    if (absEventSourceType == AbsEvent::SOURCE_TYPE_TOUCHSCREEN) {
        return PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
    }

    MMI_HILOGE("Leave, Invalid AbsEvent SourceType:$s", AbsEvent::SourceToString(absEventSourceType));
    return PointerEvent::POINTER_ACTION_NONE;
}

int32_t AbsEventHandler::ConvertAction(int32_t absEventAction) const
{
    if (absEventAction == AbsEvent::ACTION_DOWN) {
        return PointerEvent::POINTER_ACTION_DOWN;
    }

    if (absEventAction == AbsEvent::ACTION_UP) {
        return PointerEvent::POINTER_ACTION_UP;
    }

    if (absEventAction == AbsEvent::ACTION_MOVE) {
        return PointerEvent::POINTER_ACTION_MOVE;
    }

    return PointerEvent::POINTER_ACTION_NONE;
}

std::shared_ptr<PointerEvent::PointerItem> AbsEventHandler::ConvertPointer(const std::shared_ptr<const AbsEvent::Pointer>& absEventPointer) const
{
    std::shared_ptr<PointerEvent::PointerItem> pointer;
    if (!absEventPointer) {
        MMI_HILOGE("Leave, null absEventPointer");
        return pointer;
    }

    pointer = std::make_shared<PointerEvent::PointerItem>();
    pointer->SetId(absEventPointer->GetId());
    pointer->SetDownTime(absEventPointer->GetDownTime());
    pointer->SetGlobalX(absEventPointer->GetX());
    pointer->SetGlobalY(absEventPointer->GetY());

    return pointer;
}
} // namespace MMI
} // namespace OHOS