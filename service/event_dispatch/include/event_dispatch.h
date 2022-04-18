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
#ifndef EVENT_DISPATCH_H
#define EVENT_DISPATCH_H

#include "nocopyable.h"

#include "i_event_filter.h"
#include "i_input_event_handler.h"
#include "key_event.h"
#include "key_event_value_transformation.h"
#include "pointer_event.h"
#include "uds_server.h"

namespace OHOS {
namespace MMI {
class EventDispatch : public IInputEventHandler {
public:
    EventDispatch();
    DISALLOW_COPY_AND_MOVE(EventDispatch);
    virtual ~EventDispatch();
#ifdef OHOS_BUILD_KEYBOARD
    int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) override;
#endif
#ifdef OHOS_BUILD_MOUSE
    int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif
#ifdef OHOS_BUILD_TOUCH
    int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif

protected:
#ifdef OHOS_BUILD_TOUCH
    void OnEventTouchGetPointEventType(const EventTouch& touch, const int32_t fingerCount,
        POINT_EVENT_TYPE& pointEventType);
#endif
#if defined(OHOS_BUILD_POINTER) || defined(OHOS_BUILD_TOUCH)
    int32_t DispatchPointerEvent(std::shared_ptr<PointerEvent> point);
#endif
    bool TriggerANR(int64_t time, SessionPtr sess);
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_DISPATCH_H