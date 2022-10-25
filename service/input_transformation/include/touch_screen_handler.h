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

#ifndef TOUCH_SCREEN_HANDLE_H
#define TOUCH_SCREEN_HANDLE_H
#include <memory>
#include <map>

#include "abs_event.h"
#include "input_display_info.h"
#include "i_input_define.h"
#include "i_input_device.h"
#include "i_touch_screen_handler.h"
#include "pointer_event.h"
#include "struct_multimodal.h"

namespace OHOS {
namespace MMI {

class TouchScreenHandler : public NonCopyable, public ITouchScreenHandler {
public:
    static std::unique_ptr<TouchScreenHandler> CreateInstance(const IInputContext* context);

public:
    virtual ~TouchScreenHandler() = default;
    virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override;
    virtual std::shared_ptr<PointerEvent> GetPointerEvent() override;
    virtual int32_t BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;
    virtual int32_t UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;

protected:
    TouchScreenHandler(const IInputContext* context);    
    bool ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent, 
            int32_t& pointerAction, int64_t& actionTime);
    int32_t ConvertAction(int32_t absEventAction) const;
    bool OnEventTouchDown(const std::shared_ptr<const AbsEvent>& absEvent);
    bool OnEventTouchUp(const std::shared_ptr<const AbsEvent>& absEvent);
    bool OnEventTouchMotion(const std::shared_ptr<const AbsEvent>& absEvent);
    int32_t TransformX(int32_t xPos, int32_t width, int32_t logicalWidth) const;
    int32_t TransformY(int32_t yPos, int32_t height, int32_t logicalHeight) const;
    void GetPhysicalDisplayCoord(const std::shared_ptr<const AbsEvent>& absEvent,
    const DisplayInfo& info, EventTouch& touchInfo);
    int32_t TransformToPhysicalDisplayCoordinate(const DisplayInfo& info,
            int32_t tpX, int32_t tpY, int32_t& displayX, int32_t& displayY) const;
    bool TouchPointToDisplayPoint(int32_t deviceId, const std::shared_ptr<const AbsEvent>& absEvent,
            EventTouch& touchInfo, int32_t& physicalDisplayId);
    void ResetTouchUpEvent(std::shared_ptr<PointerEvent> pointerEvent);

private:
    [[maybe_unused]]const IInputContext* const context_;
    std::shared_ptr<IInputDevice> inputDevice_;
    std::shared_ptr<IInputDevice::AxisInfo> xInfo_;
    std::shared_ptr<IInputDevice::AxisInfo> yInfo_;
    std::shared_ptr<PointerEvent> pointerEvent_;
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_SCREEN_HANDLE_H