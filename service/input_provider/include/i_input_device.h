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

#ifndef I_INPUT_DEVICE_H
#define I_INPUT_DEVICE_H

#include <memory>
#include <string>

#include "input_type.h"

namespace OHOS {
namespace MMI {

struct HDFDeviceStatusEventNew {
    uint32_t index;
    uint64_t time;
    uint32_t devType;
    uint32_t devStatus;
    InputDeviceInfo devInfo; 
};

class IEventHandler;
class IInputDevice {
public:
    static const int32_t CAPABILITY_UNKNOWN = 0b0;
    static const int32_t CAPABILITY_MOUSE = 0b1;
    static const int32_t CAPABILITY_KEYBOARD = 0b10;
    static const int32_t CAPABILITY_TOUCHSCREEN = 0b100;
    static const int32_t CAPABILITY_TOUCHPAD = 0b1000;
    static const int32_t AXIS_NONE = 0b0;
    static const int32_t AXIS_MT_X = 0b1;
    static const int32_t AXIS_MT_Y = 0b10;
    static const int32_t AXIS_MAX = 0b11;
    class AxisInfo {
        public:
            int32_t GetAxis() const { return axis_; }
            int32_t GetMinimum() const { return minimum_; }
            int32_t GetMaximum() const { return maximum_; }
            int32_t GetFuzz() const { return fuzz_; }
            int32_t GetFlat() const { return flat_; }
            int32_t GetResolution() const { return resolution_; }

            void SetAxis(int32_t axis) { axis_ = axis; }
            void SetMinimum(int32_t minimum) { minimum_ = minimum; }
            void SetMaximum(int32_t maximum) { maximum_ = maximum; }
            void SetFuzz(int32_t fuzz) { fuzz_ = fuzz; }
            void SetFlat(int32_t flat) { flat_ = flat; }
            void SetResolution(int32_t resolution) { resolution_ = resolution; }
        private:
            int32_t axis_ { -1 };
            int32_t minimum_ { -1 };
            int32_t maximum_ { -1 };
            int32_t fuzz_ { -1 };
            int32_t flat_ { -1 };
            int32_t resolution_ { -1 };
    };
public:
    explicit IInputDevice(uint32_t index) : index_(index) {}
    virtual ~IInputDevice() = default;
    virtual void SetDeviceId(int32_t id) = 0;
    virtual int32_t Enable() = 0;
    virtual int32_t Disable() = 0;
    virtual std::string GetName() = 0;
    virtual int32_t GetCapabilities() = 0;
    virtual std::string GetPath() = 0;
    virtual bool HasCapability(int32_t capability) = 0;
    virtual int32_t GetDeviceId() = 0;
    virtual std::shared_ptr<IEventHandler> GetEventHandler() = 0;

    virtual InputDeviceInfo GetInputDevInfo() = 0;
    virtual uint32_t GetDeviceIndex() const { return index_; }

protected:
    uint32_t index_;

};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE_H