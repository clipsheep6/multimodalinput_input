/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pointer_event_handler.h"

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_event_handler.h"
#include "mmi_log.h"
#include "mouse_event_handler.h"
#include "touch_transform_point_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "PointerEventHandler" };
}

int32_t PointerEventHandler::HandleLibinputEvent(libinput_event* event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto type = libinput_event_get_type(event);
    switch (type) {
        case LIBINPUT_EVENT_TOUCHPAD_DOWN:
        case LIBINPUT_EVENT_TOUCHPAD_UP:
        case LIBINPUT_EVENT_TOUCHPAD_MOTION: {
            HandleTouchPadEvent(event);
            break;
        }
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        case LIBINPUT_EVENT_GESTURE_PINCH_END: {
            HandleGestureEvent(event);
            break;
        }
        default: {
            HandleMouseEvent(event);
            break;
        }
    }
    return RET_OK;
}

int32_t PointerEventHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_UPDATE) {
        CHKPR(pointerEvent, ERROR_NULL_POINTER);
        MMI_HILOGI("MouseEvent Normalization Results, PointerAction:%{public}d,PointerId:%{public}d,"
                   "SourceType:%{public}d,ButtonId:%{public}d,"
                   "VerticalAxisValue:%{public}lf,HorizontalAxisValue:%{public}lf",
                   pointerEvent->GetPointerAction(), pointerEvent->GetPointerId(), pointerEvent->GetSourceType(),
                   pointerEvent->GetButtonId(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
                   pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));
        PointerEvent::PointerItem item;
        if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item)) {
            MMI_HILOGE("Get pointer item failed. pointer:%{public}d", pointerEvent->GetPointerId());
            return RET_ERR;
        }
        MMI_HILOGI("MouseEvent Item Normalization Results, DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
                   "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
                   "Width:%{public}d,Height:%{public}d,Pressure:%{public}d,Device:%{public}d",
                   item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetGlobalX(), item.GetGlobalY(),
                   item.GetLocalX(), item.GetLocalY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
                   item.GetDeviceId());
    }
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandlePointerEvent(pointerEvent);
}

int32_t PointerEventHandler::HandleTouchPadEvent(libinput_event* event)
{
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH_PAD);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    nextHandler_->HandlePointerEvent(pointerEvent);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCHPAD_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch pad event is up remove this finger");
        if (pointerEvent->GetPointersIdList().empty()) {
            MMI_HILOGD("This touch pad event is final finger up remove this finger");
            pointerEvent->Reset();
        }
    }
    return RET_OK;
}

int32_t PointerEventHandler::HandleGestureEvent(libinput_event* event)
{
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_GESTURE);
    CHKPR(pointerEvent, GESTURE_EVENT_PKG_FAIL);
    MMI_HILOGD("GestrueEvent package, eventType:%{public}d,actionTime:%{public}" PRId64 ","
               "action:%{public}d,actionStartTime:%{public}" PRId64 ","
               "pointerAction:%{public}d,sourceType:%{public}d,"
               "PinchAxisValue:%{public}.2f",
                pointerEvent->GetEventType(), pointerEvent->GetActionTime(),
                pointerEvent->GetAction(), pointerEvent->GetActionStartTime(),
                pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(),
                pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH));

    PointerEvent::PointerItem item;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
    MMI_HILOGD("Item:DownTime:%{public}" PRId64 ",IsPressed:%{public}s,"
               "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
               "Width:%{public}d,Height:%{public}d",
               item.GetDownTime(), (item.IsPressed() ? "true" : "false"),
               item.GetGlobalX(), item.GetGlobalY(), item.GetLocalX(), item.GetLocalY(),
               item.GetWidth(), item.GetHeight());

    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    nextHandler_->HandlePointerEvent(pointerEvent);
    return RET_OK;
}

int32_t PointerEventHandler::HandleMouseEvent(libinput_event* event)
{
    MouseEventHdr->Normalize(event);
    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    auto keyEvent = InputHandler->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    for (const int32_t& keyCode : pressedKeys) {
        MMI_HILOGI("Pressed keyCode:%{public}d", keyCode);
    }
    pointerEvent->SetPressedKeys(pressedKeys);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    nextHandler_->HandlePointerEvent(pointerEvent);
    return RET_OK;
}
}
// namespace MMI
} // namespace OHOS