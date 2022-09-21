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

#ifndef TOUCH_SCREEN_SEAT_H
#define TOUCH_SCREEN_SEAT_H
#include <memory>
#include <map>

#include "i_touch_screen_seat.h"
#include "i_input_define.h"
#include "i_input_device.h"
#include "abs_event.h"
#include "pointer_event.h"
// #include "LogicalDisplayState.h"

namespace OHOS {
namespace MMI {

class TouchScreenSeat : public NonCopyable, public ITouchScreenSeat {
public:
    static std::unique_ptr<TouchScreenSeat> CreateInstance(const std::string& seatId, const std::string& seatName);

public:
    virtual ~TouchScreenSeat() = default;

    virtual const std::string& GetSeatId() const override;
    virtual const std::string& GetSeatName() const override;
    virtual bool IsEmpty() const override;

    virtual int32_t BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;
    virtual int32_t UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) override;
    virtual std::shared_ptr<IInputDevice> GetInputDevice() const override;

    // virtual int32_t BindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) override;
    // virtual int32_t UnbindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) override;
    // virtual int32_t UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& display) override;
    // virtual std::shared_ptr<PhysicalDisplayState> GetDisplay() const override;

    // virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) override;
    // virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) override;
    // virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) override;

    virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override;

protected:
    TouchScreenSeat(const std::string& seatId, const std::string& seatName);

    // std::shared_ptr<PhysicalDisplayState> GetTargetDisplay() const;

    // int32_t TransformToPhysicalDisplayCoordinate(int32_t tpX, int32_t tpY, int32_t& displayX, int32_t& displayY) const;

    // int32_t DispatchTo(const std::shared_ptr<const LogicalDisplayState>& targetDisplay, 
    //         int32_t pointerAction, int64_t actionTime, std::shared_ptr<PointerEvent::PointerItem>& pointer);

    std::shared_ptr<PointerEvent::PointerItem> ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent, 
            int32_t& pointerAction, int64_t& actionTime);
    int32_t ConvertAction(int32_t absEventAction) const;

private:
    // const IInputContext* const context_;
    const std::string seatId_;
    const std::string seatName_;

    // int32_t displayId_ {-1};
    std::shared_ptr<IInputDevice> inputDevice_;
    // std::shared_ptr<PhysicalDisplayState> display_;
    std::shared_ptr<IInputDevice::AxisInfo> xInfo_;
    std::shared_ptr<IInputDevice::AxisInfo> yInfo_;
    // std::map<int64_t, std::shared_ptr<const LogicalDisplayState>> targetDisplays_;
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_SCREEN_SEAT_H