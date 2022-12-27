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

#ifndef HDF_INPUT_DEVICE
#define HDF_INPUT_DEVICE

#include <map>
#include <memory>
#include <nocopyable.h>

#include "input_manager.h"

#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class IEventHandler;
class HDFInputDevice : public IInputDevice {
    static constexpr unsigned long BITS_PER_BYTE = 8;
    static constexpr unsigned long BITS_PER_LONG = sizeof(unsigned long) * BITS_PER_BYTE;
public:
    explicit HDFInputDevice(std::shared_ptr<IInputContext> context, uint32_t devIndex, IInputInterface *inputInterface , InputEventCb eventCb);
    virtual ~HDFInputDevice() = default;
    DISALLOW_COPY_AND_MOVE(HDFInputDevice);

    virtual void SetDeviceId(int32_t id) override;
    virtual int32_t Enable() override;
    virtual int32_t Disable() override;
    virtual std::string GetName() override;
    virtual int32_t GetCapabilities() override;
    virtual std::string GetPath() override;
    virtual bool HasCapability(int32_t capability) override;
    std::shared_ptr<InputDevice> GetInputDevice() override;
    virtual int32_t GetDeviceId() override;
    virtual std::shared_ptr<IEventHandler> GetEventHandler() override;

private:
    int32_t UpdateCapability();
    bool HasMouseCapability();
    bool HasKeyboardCapability();
    bool HasTouchscreenCapability();

    bool HasEventCode(unsigned int evType, unsigned int evCode);
    bool HasEventType(unsigned int evType);
    bool HasInputProperty(unsigned int property);
    
    int32_t CheckAndUpdateAxisInfo();
    std::shared_ptr<IInputDevice::AxisInfo> GetAxisInfo(int32_t axis) const;

    static std::tuple<unsigned int, unsigned int> GetBitLoc(unsigned long evMacro)
    {
        unsigned long index = evMacro / BITS_PER_LONG;
        unsigned long offset = evMacro % BITS_PER_LONG;
        return { index, offset };
    }
private:
    int32_t deviceId_;
    int32_t capabilities_ { IInputDevice::CAPABILITY_UNKNOWN };
    std::shared_ptr<IEventHandler> iEventHandler_;
    InputDeviceInfo devInfo_;
    mutable std::map<int32_t, std::shared_ptr<IInputDevice::AxisInfo>> axises_;
    IInputInterface *inputInterface_ { nullptr };
    InputEventCb eventCb_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_INPUT_DEVICE