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

#ifndef I_TOUCH_SCREEN_H
#define I_TOUCH_SCREEN_H

#include <memory>

#include "abs_event.h"
#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class IInputContext;
// class PhysicalDisplayState;
// class LogicalDisplayState;

class ITouchScreenSeat {
public:
    static std::unique_ptr<ITouchScreenSeat> CreateInstance(const IInputContext* contex, 
                    const std::string& seatId, const std::string& seatName);

    static std::shared_ptr<ITouchScreenSeat> GetDefault();

    virtual ~ITouchScreenSeat() = default;

    virtual const std::string& GetSeatId() const = 0;
    virtual const std::string& GetSeatName() const = 0;
    virtual bool IsEmpty() const = 0;

    virtual int32_t BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
    virtual int32_t UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
    virtual std::shared_ptr<IInputDevice> GetInputDevice() const = 0;

    // virtual int32_t BindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
    // virtual int32_t UnbindDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
    // virtual int32_t UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
    // virtual std::shared_ptr<PhysicalDisplayState> GetDisplay() const = 0;

    // virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) = 0;
    // virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) = 0;
    // virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) = 0;

    virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_TOUCH_SCREEN_H