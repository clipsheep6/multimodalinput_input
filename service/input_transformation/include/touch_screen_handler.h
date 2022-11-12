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

#ifndef TOUCH_SCREEN_HANDLE_H
#define TOUCH_SCREEN_HANDLE_H
#include <memory>
#include <map>

#include "nocopyable.h"

#include "abs_event.h"
#include "input_display_info.h"
#include "i_device.h"
#include "i_touch_screen_handler.h"
#include "pointer_event.h"
#include "struct_multimodal.h"

namespace OHOS {
namespace MMI {
class TouchScreenHandler final : public ITouchScreenHandler {
    friend ITouchScreenHandler;
public:
    virtual ~TouchScreenHandler() = default;
    DISALLOW_COPY_AND_MOVE(TouchScreenHandler);
    virtual std::shared_ptr<IDevice> GetDevice() override { return inputDevice_; }
    virtual std::shared_ptr<PointerEvent> GetPointerEvent() override { return pointerEvent_; }
    virtual int32_t BindInputDevice(const std::shared_ptr<IDevice> inputDevice) override;
    virtual int32_t UnbindInputDevice(const std::shared_ptr<IDevice> inputDevice) override;
    virtual void OnInputEvent(const std::shared_ptr<AbsEvent> event) override;
protected:
    static std::shared_ptr<TouchScreenHandler> CreateInstance();
    TouchScreenHandler();
    const std::shared_ptr<PointerEvent> ConvertPointer(const std::shared_ptr<AbsEvent> absEvent);
    bool OnEventTouchDown(const std::shared_ptr<AbsEvent> absEvent);
    bool OnEventTouchUp(const std::shared_ptr<AbsEvent> absEvent);
    bool OnEventTouchMotion(const std::shared_ptr<AbsEvent> absEvent);
    int32_t TransformX(int32_t xPos, int32_t width, int32_t logicalWidth) const;
    int32_t TransformY(int32_t yPos, int32_t height, int32_t logicalHeight) const;
    void ResetTouchUpEvent(std::shared_ptr<PointerEvent> pointerEvent);
private:
    std::shared_ptr<IDevice> inputDevice_;
    std::shared_ptr<PointerEvent> pointerEvent_ { nullptr };
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_SCREEN_HANDLE_H