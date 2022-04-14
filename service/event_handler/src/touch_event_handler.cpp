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

#include "touch_event_handler.h"

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "mmi_log.h"
#include "touch_transform_point_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchEventHandler" };
}

int32_t TouchEventHandler::HandleLibinputEvent(libinput_event* event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCH_CANCEL || type == LIBINPUT_EVENT_TOUCH_FRAME) {
        MMI_HILOGD("This touch event is canceled type:%{public}d", type);
        return RET_OK;
    }
    switch (type) {
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_UP:
        case LIBINPUT_EVENT_TOUCH_MOTION: {
            auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH);
            CHKPR(pointerEvent, ERROR_NULL_POINTER);
            BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
            CHKPR(nextHandler_, ERROR_NULL_POINTER);
            nextHandler_->HandleTouchEvent(pointerEvent);
            if (type == LIBINPUT_EVENT_TOUCH_UP) {
                pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
                MMI_HILOGD("This touch event is up remove this finger");
                if (pointerEvent->GetPointersIdList().empty()) {
                    MMI_HILOGD("This touch event is final finger up remove this finger");
                    pointerEvent->Reset();
                }
                return RET_OK;
            }
            break;
        }
        case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        case LIBINPUT_EVENT_TABLET_TOOL_TIP: {
            auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TABLET_TOOL);
            CHKPR(pointerEvent, ERROR_NULL_POINTER);
            BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
            CHKPR(nextHandler_, ERROR_NULL_POINTER);
            nextHandler_->HandleTouchEvent(pointerEvent);
            if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
                pointerEvent->Reset();
            }
            break;
        }
        default: { 
            break;
        }  
    }
    return RET_OK;
}

int32_t TouchEventHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandleTouchEvent(pointerEvent);
}
}
// namespace MMI
} // namespace OHOS