/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef MOUSE_EVENT_HANDLER_H
#define MOUSE_EVENT_HANDLER_H

#include <memory>

#include "libinput.h"
#include "nocopyable.h"
#include "singleton.h"

#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class MouseEventHandler : public DelayedSingleton<MouseEventHandler>,
    public std::enable_shared_from_this<MouseEventHandler> {
public:
    MouseEventHandler();
    ~MouseEventHandler() = default;
    DISALLOW_COPY_AND_MOVE(MouseEventHandler);

    std::shared_ptr<PointerEvent> GetPointerEvent() const;
    void Normalize(struct libinput_event *event);
    bool NormalizeMoveMouse(int32_t offsetX, int32_t offsetY);
private:
    void HandleMotionInner(libinput_event_pointer* data);
    void HandleButonInner(libinput_event_pointer* data);
    void HandleAxisInner(libinput_event_pointer* data);
    void HandlePostInner(libinput_event_pointer* data, int32_t deviceId, PointerEvent::PointerItem& pointerItem);
    void HandleMotionMoveMouse(int32_t offsetX, int32_t offsetY);
    void HandlePostMoveMouse(PointerEvent::PointerItem& pointerItem);
    void DumpInner();
    void InitAbsolution();

private:
    std::shared_ptr<PointerEvent> pointerEvent_ = nullptr;
    int32_t timerId_ = -1;
    double absolutionX_ = -1;
    double absolutionY_ = -1;
    int32_t buttionId_ = -1;
    bool isPressed_ = false;
};

#define MouseEventHdr MouseEventHandler::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // MOUSE_EVENT_HANDLER_H