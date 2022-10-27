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

#include <string>
#include <memory>
#include <ostream>

namespace OHOS {
namespace MMI {

class IInputContext;
class IKernelEventHandler;
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

    static const char* AxisToString(int32_t axis);

    class AxisInfo {
        public:
            static const std::unique_ptr<AxisInfo> NULL_VALUE;
        public:
            int32_t GetAxis() const;
            int32_t GetMinimum() const;
            int32_t GetMaximum() const;
            int32_t GetFuzz() const;
            int32_t GetFlat() const;
            int32_t GetResolution() const;
            std::ostream& Print(std::ostream& outStream) const;

            void SetAxis(int32_t axis);
            void SetMinimum(int32_t minimum);
            void SetMaximum(int32_t maximum);
            void SetFuzz(int32_t fuzz);
            void SetFlat(int32_t flat);
            void SetResolution(int32_t resolution);

        private:
            int32_t axis_;
            int32_t minimum_;
            int32_t maximum_;
            int32_t fuzz_;
            int32_t flat_;
            int32_t resolution_;
    };

    virtual ~IInputDevice() = default;

    virtual int32_t GetId() const = 0;
    virtual const std::string& GetName() const = 0;

    virtual std::shared_ptr<AxisInfo> GetAxisInfo(int32_t axis) const = 0;
    virtual bool HasCapability(int32_t capability) const = 0;
    virtual void ProcessEventItem(const struct input_event* eventItem) = 0;
    virtual int32_t StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler) = 0;
    virtual int32_t StopReceiveEvents() = 0;
    virtual void SetDeviceId(int32_t deviceId) = 0;
    virtual int32_t GetDeviceId() const = 0;
};

std::ostream& operator<<(std::ostream& outStream, const IInputDevice::AxisInfo& axisInfo);
std::ostream& operator<<(std::ostream& outStream, const IInputDevice::AxisInfo* axisInfo);
std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<IInputDevice::AxisInfo>& axisInfo);
std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<IInputDevice::AxisInfo>& axisInfo);
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE_H