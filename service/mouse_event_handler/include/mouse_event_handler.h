/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_MULTIMDOALINPUT_MOUSE_EVENT_HANDLER_H
#define OHOS_MULTIMDOALINPUT_MOUSE_EVENT_HANDLER_H

#include <memory>
#include "libinput.h"
#include "pointer_event.h"
#include "c_singleton.h"
#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
class MouseEventHandler : public CSingleton<MouseEventHandler>{
public:
    MouseEventHandler();
    virtual ~MouseEventHandler();
    void ProcessMouseData(libinput_event *event, int32_t deviceId);
    void SetMouseAction(const int32_t action);
    void SetTimerId(const int32_t id);
    std::shared_ptr<PointerEvent> GetPointerEventPtr();
private:
    void SetMouseMotion(PointerEvent::PointerItem& pointerItem);
    void SetMouseButon(PointerEvent::PointerItem &pointerItem, struct libinput_event_pointer& pointEventData);
    void SetMouseAxis(struct libinput_event_pointer& pointEventData);
    void CalcMovedCoordinate(struct libinput_event_pointer &pointEventData);
private:
    std::shared_ptr<PointerEvent> pointerEvent_;
    int32_t timerId_ = -1;
    double coordinateX_ = 0;
    double coordinateY_ = 0;
};
#define MouseEvent OHOS::MMI::MouseEventHandler::GetInstance()
}
} // namespace OHOS::MMI
#endif // OHOS_MULTIMDOALINPUT_POINTER_EVENT_H
