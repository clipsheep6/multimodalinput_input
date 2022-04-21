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

#ifndef TOUCH_TRANSFORM_POINT_PROCESSOR_H
#define TOUCH_TRANSFORM_POINT_PROCESSOR_H

#include <memory>

#include "nocopyable.h"

#include "input_windows_manager.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class TouchTransformPointProcessor {
public:
    explicit TouchTransformPointProcessor(int32_t deviceId);
    DISALLOW_COPY_AND_MOVE(TouchTransformPointProcessor);
    ~TouchTransformPointProcessor();
    std::shared_ptr<PointerEvent> OnLibinputTouchEvent(struct libinput_event *event);

private:
    bool OnEventTouchDown(struct libinput_event *event);
    bool OnEventTouchMotion(struct libinput_event *event);
    bool OnEventTouchUp(struct libinput_event *event);
    int32_t GetTouchToolType(struct libinput_event *event);
    int32_t GetTouchToolType(struct libinput_device *device);
private:
    int32_t deviceId_ {0};
    std::shared_ptr<PointerEvent> pointerEvent_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_TRANSFORM_POINT_PROCESSOR_H