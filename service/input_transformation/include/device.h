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

#ifndef DEVICE_H
#define DEVICE_H

#include <memory>
#include <map>

#include <linux/input.h>
#include "mtdev.h"
#include <mtdev-plumbing.h>

#include "nocopyable.h"

#include "abs_event_collector.h"
#include "abs_event.h"
#include "hdf_adapter.h"
#include "i_device.h"

struct input_event;

namespace OHOS {
namespace MMI {
static const int ev_max[EV_MAX + 1] = {
    SYN_MAX, KEY_MAX, REL_MAX, ABS_MAX, MSC_MAX, 
    SW_MAX, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, 
    LED_MAX, SND_MAX, -1, REP_MAX, FF_MAX,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1,
};
class IKernelEventHandler;
class Device : public IDevice {
    static constexpr unsigned long BITS_PER_BYTE = 8;
    static constexpr unsigned long BITS_PER_LONG = sizeof(unsigned long) * BITS_PER_BYTE;
public:
    Device(int32_t devIndex, const InputDeviceInfo &devInfo);
    virtual ~Device();
    DISALLOW_COPY_AND_MOVE(Device);
    int32_t Init();
    virtual std::shared_ptr<AxisInfo> GetAxisInfo(int32_t axis) const override;
    virtual bool HasCapability(int32_t capability) const override;
    virtual int32_t StartReceiveEvent(const std::shared_ptr<IKernelEventHandler> eventHandler) override;
    virtual int32_t StopReceiveEvent() override;
    virtual void ProcessEvent(const struct input_event& event) override;
    virtual void SetDeviceId(int32_t deviceId) override { deviceId_ = deviceId; }
    virtual int32_t GetDeviceId() const override { return deviceId_; }
    virtual const InputDeviceInfo& GetInputDeviceInfo() const override { return deviceOrigin_; }
    virtual bool IsMtDevice() const override { return isMtFlag_; }
protected:
    void Uninit();
    int32_t CheckAndUpdateAxisInfo();
private:
    int32_t CloseDevice();
    int32_t UpdateCapability();
    bool HasInputProperty(unsigned int property);
    bool HasMouseCapability();
    bool HasKeyboardCapability();
    bool HasTouchscreenCapability();
    bool HasEventType(unsigned int evType) const;
    bool HasEventCode(unsigned int evType, unsigned int evCode) const;
    void ProcessSyncEvent();
    void ProcessAbsEvent(int32_t code, int32_t value);
    void ProcessKeyEvent(int32_t code, int32_t value);
    void ProcessMscEvent(int32_t code, int32_t value);
    void ProcessEventInner(const input_event &event);
    void OnEventCollected(const std::shared_ptr<AbsEvent> event);
    int EventIsType(const struct input_event& ev, unsigned int type);
    int EventTypeGetMax(unsigned int type);
    int EventIsCode(const struct input_event& ev, uint32_t type, uint32_t code);
    static std::tuple<unsigned int, unsigned int> GetBitLoc(unsigned long evMacro)
    {
        unsigned long index = evMacro / BITS_PER_LONG;
        unsigned long offset = evMacro % BITS_PER_LONG;
        return { index, offset };
    }
    bool IsMtDevice();
private:
    int32_t deviceId_ { -1 };
    int32_t capabilities_ { IDevice::CAPABILITY_UNKNOWN };
    unsigned long inputProperty[LongsOfBits(INPUT_PROP_MAX)] {};
    unsigned long evBit[LongsOfBits(EV_MAX)] {};
    unsigned long relBit[LongsOfBits(REL_MAX)] {};
    unsigned long absBit[LongsOfBits(ABS_MAX)] {};
    AbsEventCollector absEventCollector_;
    mtdev* mtdev_ { nullptr };
    bool isMtFlag_ = false;
    mutable std::map<int32_t, std::shared_ptr<IDevice::AxisInfo>> axises_;
    std::shared_ptr<IKernelEventHandler> eventHandler_ { nullptr };
    InputDeviceInfo deviceOrigin_ {};
};
} // namespace MMI
} // namespace OHOS
#endif // DEVICE_H