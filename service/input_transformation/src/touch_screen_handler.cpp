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

#include "touch_screen_handler.h"

#include "i_input_context.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchScreenHandler" };
};
std::unique_ptr<TouchScreenHandler> TouchScreenHandler::CreateInstance(const IInputContext* context) 
{
    if (context == nullptr) {
         return nullptr;
     }
    return std::unique_ptr<TouchScreenHandler>(new TouchScreenHandler(context));
}

TouchScreenHandler::TouchScreenHandler(const IInputContext* context) 
        : context_(context)
{
}

void TouchScreenHandler::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
{
    MMI_HILOGD("Enter absEvent:%{public}p", event.get());
    int32_t pointerAction = PointerEvent::POINTER_ACTION_UNKNOWN;
    int64_t actionTime = 0;
    auto pointer = ConvertPointer(event, pointerAction, actionTime);
    if (!pointer) {
        MMI_HILOGE("Leave ConvertPointer Failed");
        return;
    }

    auto retCode = DispatchTo(pointerAction, actionTime,  pointer);
    if (retCode < 0) {
        MMI_HILOGE("Leave, Dispatch Failed");
        return;
    }

    MMI_HILOGD("Leave,  pointerAction:%{public}d, pointer:%{public}p", 
            pointerAction, pointer.get());
}

int32_t TouchScreenHandler::DispatchTo(int32_t pointerAction, int64_t actionTime, std::shared_ptr<PointerEvent::PointerItem>& pointer)
{

    // auto pointerEvent = targetDisplay->HandleEvent(pointerAction, actionTime, pointer);
    // if (!pointerEvent) {
    //     MMI_HILOGE("Leave, null pointerEvent");
    //     return -1;
    // }
    

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return -1;
    }
    // auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    // CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
    // inputEventNormalizeHandler->HandleTouchEvent(pointerEvent);
    return 0;
}

std::shared_ptr<PointerEvent::PointerItem> TouchScreenHandler::ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent,
        int32_t& pointerAction, int64_t& actionTime)
{
    std::shared_ptr<PointerEvent::PointerItem> pointer;
    if (!absEvent) {
        MMI_HILOGE("Leave, null absEvent");
        return pointer;
    }
    const auto& absEventPointer = absEvent->GetPointer();
    if (!absEventPointer) {
        MMI_HILOGE("Leave, null absEventPointer");
        return pointer;
    }

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return pointer;
    }

    auto action = ConvertAction(absEvent->GetAction());
    if (action == PointerEvent::POINTER_ACTION_UNKNOWN) {
        MMI_HILOGE("Leave, ConvertAction Failed");
        return pointer;
    }

    // int32_t inputDeviceId = absEvent->GetDeviceId();
    // auto pointerId = absEventPointer->GetId();
    // auto pointerKey = MMI_HILOGE::Combine(inputDeviceId, pointerId);
    //auto it = targetDisplays_.find(pointerKey);
    // int32_t logicalDisplayX = -1;
    // int32_t logicalDisplayY = -1;
    // int64_t upTime = -1;
    // if (action == PointerEvent::POINTER_ACTION_DOWN) {
    //     if (it != targetDisplays_.end()) {
    //         targetDisplays_.erase(it);
    //     }

    //     int32_t physicalDisplayX = -1;
    //     int32_t physicalDisplayY = -1;
    //     auto retCode = TransformToPhysicalDisplayCoordinate(absEventPointer->GetX(), absEventPointer->GetY(), 
    //             physicalDisplayX, physicalDisplayY);
    //     if (retCode < 0) {
    //         MMI_HILOGE("Leave, TransformToPhysicalDisplayCoordinate Failed");
    //         return pointer;
    //     }

    //     int32_t globalX = -1;
    //     int32_t globalY = -1;
    //     retCode = windowStateManager->TransformPhysicalDisplayCoordinateToPhysicalGlobalCoordinate(
    //             display_->GetId(), physicalDisplayX, physicalDisplayY,
    //             globalX, globalY);

    //     if (retCode < 0) {
    //         MMI_HILOGE("Leave, TransformPhysicalDisplayCooridateToLogicalDisplayCooridate Failed");
    //         return pointer;
    //     }

    //     targetDisplay = windowStateManager->TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate(globalX, globalY,
    //             logicalDisplayX, logicalDisplayY);
    //     if (!targetDisplay) {
    //         MMI_HILOGE("Leave, TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate Failed");
    //         return pointer;
    //     }
    //     targetDisplays_[pointerKey] = targetDisplay;
    // } else if (action == PointerEvent::POINTER_ACTION_MOVE || action == PointerEvent::POINTER_ACTION_UP) {
    //     if (it == targetDisplays_.end()) {
    //         MMI_HILOGE("Leave, No Target When MOVE OR UP");
    //         return pointer;
    //     }
    //     targetDisplay = it->second;
    //     if (action == PointerEvent::POINTER_ACTION_UP) {
    //         upTime = absEvent->GetActionTime();
    //     }

    //     int32_t physicalDisplayX = -1;
    //     int32_t physicalDisplayY = -1;
    //     auto retCode = TransformToPhysicalDisplayCoordinate(absEventPointer->GetX(), absEventPointer->GetY(), 
    //             physicalDisplayX, physicalDisplayY);
    //     if (retCode < 0) {
    //         MMI_HILOGE("Leave, TransformToPhysicalDisplayCoordinate Failed");
    //         return pointer;
    //     }

    //     retCode = targetDisplay->Transform(physicalDisplayX, physicalDisplayY, 
    //             true, logicalDisplayX, logicalDisplayY);
    //     if (retCode < 0) {
    //         MMI_HILOGE("Leave, TransformTo Target Display Coordinate Failed");
    //         return pointer;
    //     }

    // } else {
    //     MMI_HILOGE("Leve, unknown absEvent Action:$s", PointerEvent::ActionToString(action));
    //     return pointer;
    // }

    pointerAction = action;
    actionTime = absEvent->GetActionTime();

    pointer = std::make_shared<PointerEvent::PointerItem>();
    // pointer->SetId(pointerId);
    pointer->SetDownTime(absEventPointer->GetDownTime());
    // pointer->SetUpTime(upTime);
    // pointer->SetGlobalX(logicalDisplayX);
    // pointer->SetGlobalY(logicalDisplayY);
    pointer->SetDeviceId(absEvent->GetDeviceId());
    MMI_HILOGD("Leave");
    return pointer;
}

int32_t TouchScreenHandler::ConvertAction(int32_t absEventAction) const
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

    return PointerEvent::POINTER_ACTION_UNKNOWN;
}
} // namespace MMI
} // namespace OHOS