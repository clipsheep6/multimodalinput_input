/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "touch_gesture_adapter.h"

#include "input_event_handler.h"
#include "util.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_HANDLER
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "TouchGestureAdapter"

namespace OHOS {
namespace MMI {
TouchGestureAdapter::TouchGestureAdapter(TouchGestureType type, std::shared_ptr<TouchGestureAdapter> next)
    : gestureType_(type), nextAdapter_(next)
{}

void TouchGestureAdapter::SetGestureCondition(bool flag, TouchGestureType type, int32_t fingers)
{
    static bool isAll = false;
    if (gestureDetector_ != nullptr &&
        (type == gestureType_ || type == TOUCH_GESTURE_TYPE_ALL)) {
        if (type == TOUCH_GESTURE_TYPE_ALL) {
            isAll = flag;
        }
        if (flag) {
            gestureDetector_->AddGestureFingers(fingers);
        }
        if (!flag && !isAll) {
            gestureDetector_->RemoveGestureFingers(fingers);
        }
    }
    if (nextAdapter_ != nullptr) {
        nextAdapter_->SetGestureCondition(flag, type, fingers);
    }
}

void TouchGestureAdapter::process(std::shared_ptr<PointerEvent> event)
{
    OnTouchEvent(event);
    if (ShouldDeliverToNext() && nextAdapter_ != nullptr) {
        nextAdapter_->process(event);
    }
}

void TouchGestureAdapter::Init()
{
    if (gestureDetector_ == nullptr) {
        gestureDetector_ = std::make_shared<TouchGestureDetector>(gestureType_, shared_from_this());
    }
    if (nextAdapter_ != nullptr) {
        nextAdapter_->Init();
    }
}

std::shared_ptr<TouchGestureAdapter> TouchGestureAdapter::GetGestureFactory()
{
    std::shared_ptr<TouchGestureAdapter> pinch =
        std::make_shared<TouchGestureAdapter>(TOUCH_GESTURE_TYPE_PINCH, nullptr);
    std::shared_ptr<TouchGestureAdapter> swipe =
        std::make_shared<TouchGestureAdapter>(TOUCH_GESTURE_TYPE_SWIPE, pinch);
    swipe->Init();
    return swipe;
}

void TouchGestureAdapter::OnTouchEvent(std::shared_ptr<PointerEvent> event)
{
    CHKPV(event);
    CHKPV(gestureDetector_);
    if (event->GetSourceType() != PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        return;
    }
    UpdateTouchMovement(event);
    shouldDeliverToNext_ = true;

    if (!gestureStarted_ && (event->GetPointerAction() == PointerEvent::POINTER_ACTION_UP)) {
        gestureDetector_->OnTouchEvent(event);
        return;
    }
    if (gestureType_ == TOUCH_GESTURE_TYPE_SWIPE) {
        OnSwipeGesture(event);
    } else if (gestureType_ == TOUCH_GESTURE_TYPE_PINCH) {
        OnPinchGesture(event);
    }
    if (gestureStarted_ && (event->GetPointerAction() == PointerEvent::POINTER_ACTION_MOVE)) {
        shouldDeliverToNext_ = false;
    }
}

void TouchGestureAdapter::OnGestureSuccessful(std::shared_ptr<PointerEvent> event)
{
    CHKPV(event);
    auto items = event->GetAllPointerItems();
    for (const auto &item : items) {
        if (!item.IsPressed()) {
            continue;
        }
        auto iter = touches_.find(item.GetPointerId());
        if (iter == touches_.end()) {
            MMI_HILOGW("No touch(%{public}d) record", item.GetPointerId());
            continue;
        }
        if (iter->second.cancelled_) {
            continue;
        }
        iter->second.cancelled_ = true;
        MMI_HILOGI("Cancel touch(%{public}d)", item.GetPointerId());
        auto pointerEvent = std::make_shared<PointerEvent>(*event);
        pointerEvent->SetPointerId(item.GetPointerId());
        pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
        pointerEvent->SetActionTime(GetSysClockTime());
        pointerEvent->UpdateId();
        pointerEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT | InputEvent::EVENT_FLAG_NO_MONITOR);
        auto eventDispatchHandler = InputHandler->GetEventDispatchHandler();
        CHKPV(eventDispatchHandler);
        eventDispatchHandler->HandlePointerEvent(pointerEvent);
    }
}

void TouchGestureAdapter::OnSwipeGesture(std::shared_ptr<PointerEvent> event)
{
    CHKPV(gestureDetector_);
    if (state_ == GestureState::PINCH) {
        return;
    }
    gestureStarted_ = gestureDetector_->OnTouchEvent(event);
    state_ = gestureStarted_ ? GestureState::SWIPE : GestureState::IDLE;
}

void TouchGestureAdapter::OnPinchGesture(std::shared_ptr<PointerEvent> event)
{
    CHKPV(gestureDetector_);
    if (state_ == GestureState::SWIPE) {
        return;
    }
    gestureStarted_ = gestureDetector_->OnTouchEvent(event);
    state_ = gestureStarted_ ? GestureState::PINCH : GestureState::IDLE;
}

bool TouchGestureAdapter::OnGestureEvent(std::shared_ptr<PointerEvent> event, GestureMode mode)
{
#ifdef OHOS_BUILD_ENABLE_MONITOR
    auto pointEvent = std::make_shared<PointerEvent>(*event);
    pointEvent->UpdateId();
    pointEvent->SetHandlerEventType(HANDLE_EVENT_TYPE_TOUCH_GESTURE);
    switch (mode) {
        case GestureMode::ACTION_SWIPE_DOWN:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_SWIPE_DOWN);
            break;
        case GestureMode::ACTION_SWIPE_UP:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_SWIPE_UP);
            break;
        case GestureMode::ACTION_SWIPE_LEFT:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_SWIPE_LEFT);
            break;
        case GestureMode::ACTION_SWIPE_RIGHT:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_SWIPE_RIGHT);
            break;
        case GestureMode::ACTION_PINCH_CLOSED:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_PINCH_CLOSEED);
            break;
        case GestureMode::ACTION_PINCH_OPENED:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_PINCH_OPENED);
            break;
        case GestureMode::ACTION_GESTURE_END:
            pointEvent->SetPointerAction(PointerEvent::TOUCH_ACTION_GESTURE_END);
            break;
        default:
            MMI_HILOGW("unknow mode:%{public}d", mode);
            return false;
    }
    auto monitor = InputHandler->GetMonitorHandler();
    CHKPF(monitor);
    monitor->HandlePointerEvent(pointEvent);
#endif // OHOS_BUILD_ENABLE_MONITOR
    return true;
}

void TouchGestureAdapter::OnGestureTrend(std::shared_ptr<PointerEvent> event)
{
    OnGestureSuccessful(event);
}

void TouchGestureAdapter::UpdateTouchMovement(std::shared_ptr<PointerEvent> event)
{
    if (!TouchGestureDetector::IsPhysicalPointer(event)) {
        return;
    }
    switch (event->GetPointerAction()) {
        case PointerEvent::POINTER_ACTION_DOWN: {
            touches_.insert_or_assign(event->GetPointerId(), TouchItem {});
            break;
        }
        case PointerEvent::POINTER_ACTION_UP: {
            touches_.erase(event->GetPointerId());
            break;
        }
        default: {
            break;
        }
    }
}
} // namespace MMI
} // namespace OHOS