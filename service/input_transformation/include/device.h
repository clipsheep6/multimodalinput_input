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

#include "abs_event_collector.h"
#include "abs_event.h"
#include "hdf_adapter.h"
#include "i_event_collector.h"
#include "i_input_context.h"
#include "i_input_define.h"
#include "i_input_device.h"

struct input_event;

namespace OHOS {
namespace MMI {
static const int ev_max[EV_MAX + 1] = {
    SYN_MAX,
    KEY_MAX,
    REL_MAX,
    ABS_MAX,
    MSC_MAX,
    SW_MAX,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    LED_MAX,
    SND_MAX,
    -1,
    REP_MAX,
    FF_MAX,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
};

typedef unsigned int bitmask_t;
static constexpr unsigned long BITS_PER_BYTE = 8;
static constexpr unsigned long BITS_PER_LONG = sizeof(unsigned long) * BITS_PER_BYTE;
class IKernelEventHandler;
class Device : public NonCopyable, public IInputDevice {

public:
    Device(int32_t id, const std::shared_ptr<IInputContext> context, const InputDeviceInfo &devInfo);
    virtual ~Device();
    int32_t Init();
    virtual int32_t GetId() const override;
    virtual const std::string& GetName() const override;
    virtual std::shared_ptr<AxisInfo> GetAxisInfo(int32_t axis) const override;
    virtual bool HasCapability(int32_t capability) const override;
    virtual int32_t StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler) override;
    virtual int32_t StopReceiveEvents() override;
    virtual void ProcessEventItem(const struct input_event* eventItem) override;
    virtual void SetDeviceId(int32_t deviceId) override;
    virtual int32_t GetDeviceId() const override;

protected:
    void Uninit();

private:
    void OnFdEvent(int fd, int event);
    void ReadEvents();
    int32_t CloseDevice();
    int32_t UpdateCapability();
    bool HasInputProperty(unsigned long property);
    bool HasMouseCapability();
    bool HasKeyboardCapability();
    bool HasTouchscreenCapability();

    bool HasEventType(unsigned long evType) const;
    bool HasEventCode(unsigned long evType, unsigned long evCode) const;

    void ProcessSyncEvent(int32_t code, int32_t value);
    void ProcessAbsEvent(int32_t code, int32_t value);
    void ProcessMscEvent(int32_t code, int32_t value);
    void OnEventCollected(const std::shared_ptr<const AbsEvent>& event);
    void ProcessEvent(const struct input_event* eventItem);

    int EventIsType(const struct input_event *ev, unsigned int type);
    int EventtTypeGetMax(unsigned int type);
    int EventIsCode(const struct input_event *ev, unsigned int type, unsigned int code);

    static std::tuple<unsigned long , unsigned long> GetBitLoc(unsigned long evMacro)
    {
        unsigned long index = evMacro / BITS_PER_LONG;
        unsigned long offset = evMacro % BITS_PER_LONG;
        return {index, offset};
    }

private:
    const int32_t id_;
    const std::shared_ptr<IInputContext> context_;
    std::string name_;
    int32_t deviceId_;
    InputDimensionInfo dimensionInfoX_;
    InputDimensionInfo dimensionInfoY_;
    InputDevAbility devAbility_;

    int32_t capabilities_ {IInputDevice::CAPABILITY_UNKNOWN};
    unsigned long inputProperty[LongsOfBits(INPUT_PROP_MAX)];
    unsigned long evBit[LongsOfBits(EV_MAX)];
    unsigned long relBit[LongsOfBits(REL_MAX)];
    unsigned long absBit[LongsOfBits(ABS_MAX)];
    AbsEventCollector absEventCollector_;
    mtdev* mtdev_ {nullptr};

    mutable std::map<int32_t, std::shared_ptr<IInputDevice::AxisInfo>> axises_;

    std::shared_ptr<IKernelEventHandler> eventHandler_;
};
} // namespace MMI
} // namespace OHOS
#endif // DEVICE_H