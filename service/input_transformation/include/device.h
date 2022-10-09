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

#ifndef DEVICE_H
#define DEVICE_H

#include <memory>
#include <map>

#include <linux/input.h>


#include "i_input_define.h"
#include "i_input_device.h"
#include "i_input_context.h"
#include "i_event_collector.h"
// #include "i_seat.h"
// #include "i_touch_screen_handler.h"
#include "abs_event_collector.h"
#include "abs_event.h"

struct input_event;

namespace OHOS {
namespace MMI {

class IKernelEventHandler;
class Device : public NonCopyable, public IInputDevice {

public:
    Device(int32_t id, const IInputContext* context);
    virtual ~Device();
    virtual int32_t GetId() const override;
    virtual const std::string& GetName() const override;
    virtual std::shared_ptr<AxisInfo> GetAxisInfo(int32_t axis) const override;
    virtual bool HasCapability(int32_t capability) const override;
    virtual int32_t StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler) override;
    virtual int32_t StopReceiveEvents() override;
    virtual void ProcessEventItem(const struct input_event* eventItem) override;
    virtual void SetDeviceId(int32_t deviceId) override;
    virtual int32_t GetDeviceId() override;

protected:
    int32_t Init();
    void Uninit();

private:
    void OnFdEvent(int fd, int event);
    void ReadEvents();
    int32_t CloseDevice();
    int32_t UpdateCapablility();
    int32_t UpdateInputProperty();
    int32_t UpdateBitStat(int32_t evType, int32_t maxValue, unsigned long* resultValue, size_t len);
    bool TestBit(int32_t bitIndex, const unsigned long* bitMap, size_t count) const;
    bool HasInputProperty(int32_t property);
    bool HasMouseCapability();
    bool HasKeyboardCapability();
    bool HasTouchscreenCapability();
    bool HasTouchpadCapability();

    bool HasEventType(int32_t evType) const;
    bool HasEventCode(int32_t evType, int32_t evCode) const;

    void ProcessSyncEvent(int32_t code, int32_t value);
    void ProcessAbsEvent(int32_t code, int32_t value);
    void ProcessMscEvent(int32_t code, int32_t value);
    void OnEventCollected(const std::shared_ptr<const AbsEvent>& event);

private:
    const int32_t id_;
    const int32_t deviceId_;
    const IInputContext* const context_;
    int32_t fd_;
    // std::string seatId_;
    // std::string seatName_;
    std::string name_;

    int32_t capabilities_ {IInputDevice::CAPABILITY_UNKNOWN};
    unsigned long inputProperty[LongsOfBits(INPUT_PROP_MAX)];
    unsigned long evBit[LongsOfBits(EV_MAX)];
    unsigned long relBit[LongsOfBits(REL_MAX)];
    unsigned long absBit[LongsOfBits(ABS_MAX)];
    AbsEventCollector absEventCollector_;

    mutable std::map<int32_t, std::shared_ptr<IInputDevice::AxisInfo>> axises_;

    std::shared_ptr<IKernelEventHandler> eventHandler_;
};
} // namespace MMI
} // namespace OHOS
#endif // DEVICE_H