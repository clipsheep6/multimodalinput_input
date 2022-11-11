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

#include "device.h"

#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "error_multimodal.h"
#include "enum_utils.h"
#include "i_kernel_event_handler.h"
#include "mmi_log.h"
#include "time_utils.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "Device" };
};

int32_t Device::Init()
{
    CALL_DEBUG_ENTER;
    auto retCode = UpdateCapability();
    if (retCode < 0) {
        MMI_HILOGE("Leave device Init, UpdateCapability Failed");
        return -1;
    }
    if (HasCapability(IInputDevice::CAPABILITY_TOUCHSCREEN)) {
        mtdev_ = mmi_mtdev_new_open();
    }
    return 0;
}

void Device::Uninit()
{
    CloseDevice();
}

Device::Device(int32_t id, const std::shared_ptr<IInputContext> context, const InputDeviceInfo &devInfo) : id_(id),
   context_(context), absEventCollector_(id, AbsEvent::SOURCE_TYPE_NONE),
   eventHandler_(IKernelEventHandler::GetDefault()) {
       dimensionInfoX_ = devInfo.attrSet.axisInfo[ABS_MT_POSITION_X];
       dimensionInfoY_ = devInfo.attrSet.axisInfo[ABS_MT_POSITION_Y];
       devAbility_ = devInfo.abilitySet;
   }
Device::~Device()
{
    Uninit();
}

int32_t Device::GetId() const
{
    return id_;
}

const std::string& Device::GetName() const
{
    return name_;
}

void Device::SetDeviceId(int32_t deviceId)
{
    deviceId_ = deviceId;
}

int32_t Device::GetDeviceId() const
{
    return deviceId_;
}

std::shared_ptr<IInputDevice::AxisInfo> Device::GetAxisInfo(int32_t axis) const
{
    auto it = axises_.find(axis);
    if (it != axises_.end()) {
        MMI_HILOGD("Leave deivce:%{public}s axis:%{public}s", GetName().c_str(), IInputDevice::AxisToString(axis));
        return it->second;
    }

    int32_t absCode = -1;
    InputDimensionInfo dimensionInfo = {};
    switch (axis) {
        case IInputDevice::AXIS_MT_X:
            absCode = ABS_MT_POSITION_X;
            dimensionInfo = dimensionInfoX_;
            break;
        case IInputDevice::AXIS_MT_Y:
            absCode = ABS_MT_POSITION_Y;
            dimensionInfo = dimensionInfoY_;
            break;
        default:
            MMI_HILOGE("Leave device:%{public}s axis:%{public}s, Unknown axis", GetName().c_str(), IInputDevice::AxisToString(axis));
            return nullptr;
    }

    auto axisInfo = std::make_shared<IInputDevice::AxisInfo>();
    axisInfo->SetAxis(axis);
    axisInfo->SetMinimum(dimensionInfo.min);
    axisInfo->SetMaximum(dimensionInfo.max);
    axisInfo->SetFlat(dimensionInfo.flat);
    axisInfo->SetFuzz(dimensionInfo.fuzz);
    // axisInfo->SetResolution();  //TO DO:...

    axises_[axis] = axisInfo;
    return axisInfo;
}

void Device::OnFdEvent(int fd, int event)
{
}

void Device::ReadEvents()
{
}

int32_t Device::CloseDevice()
{
    CHKPR(mtdev_, ERROR_NULL_POINTER);
    mtdev_delete(mtdev_);
    mtdev_ = nullptr;
    return 0;
}

void Device::ProcessEventItem(const struct input_event* eventItem)
{
    CALL_DEBUG_ENTER;
    if (mtdev_ == nullptr) {
        ProcessEvent(eventItem);
    } else {
        mtdev_put_event(mtdev_, eventItem);
        if (EventIsCode(eventItem, EV_SYN, SYN_REPORT)) {
            while (!mtdev_empty(mtdev_)) {
               struct input_event e;
               mtdev_get_event(mtdev_, &e);
               ProcessEvent(&e);
            }
        }
    }
}

void Device::ProcessEvent(const struct input_event* eventItem)
{
    CALL_DEBUG_ENTER;
    auto type = eventItem->type;
    auto code = eventItem->code;
    auto value = eventItem->value;
    switch (type) {
        case EV_SYN:
            ProcessSyncEvent(code, value);
            break;
        case EV_ABS:
            ProcessAbsEvent(code, value);
            break;
        case EV_KEY:
        case EV_REL:
        case EV_MSC:
        case EV_SW:
        case EV_LED:
        case EV_SND:
        case EV_REP:
        case EV_FF:
        case EV_PWR:
        case EV_FF_STATUS:
            break;
        default:
            break;
    }
}

int32_t Device::UpdateCapability()
{
    CALL_DEBUG_ENTER;
    if (HasMouseCapability()) {
        capabilities_ |= IInputDevice::CAPABILITY_MOUSE;
    }
    if (HasKeyboardCapability()) {
        capabilities_ |= IInputDevice::CAPABILITY_KEYBOARD;
    }
    if (HasTouchscreenCapability()) {
        capabilities_ |= IInputDevice::CAPABILITY_TOUCHSCREEN;
        absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
    }
    return 0;
}

bool Device::HasInputProperty(unsigned long property)
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(property);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(INPUT_PROP_CNT)) {
        MMI_HILOGE("Error");
    }
    auto curBit = (1UL) << offset;
    return devAbility_.devProp[index] & curBit;
}

bool Device::HasEventType(unsigned long evType) const
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(evType);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(EV_CNT)) {
        MMI_HILOGE("Error");
    }
    auto curBit = (1UL) << offset;
    return devAbility_.eventType[index] & curBit;
}

bool Device::HasEventCode(unsigned long evType, unsigned long evCode) const
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(evCode);
    if (offset > BITS_PER_LONG) {
        MMI_HILOGE("Error");
    }
    auto curBit = (1UL) << offset;

    switch (evType) {
        case EV_KEY: {
            if (index >= BITS_TO_UINT64(KEY_CNT)) {
                MMI_HILOGE("Error");
            }
            return devAbility_.keyCode[index] & curBit;
        }
        case EV_ABS: {
            if (index >= BITS_TO_UINT64(ABS_CNT)) {
                MMI_HILOGE("Error");
            }
            return devAbility_.absCode[index] & curBit;
        }
        case EV_REL: {
            if (index >= BITS_TO_UINT64(REL_CNT)) {
                MMI_HILOGE("Error");
            }
            return devAbility_.relCode[index] & curBit;
        }
        default: {
            MMI_HILOGE("The current evType:%{public}d is not supported", evType);
            return false;
        }
    }
}

bool Device::HasMouseCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_REL) &&
            HasEventCode(EV_REL, REL_X) &&
            HasEventCode(EV_REL, REL_Y) &&
            HasInputProperty( ));
}

bool Device::HasKeyboardCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_KEY) &&
            !HasEventType(EV_ABS) &&
            !HasEventType(EV_REL));
}

bool Device::HasTouchscreenCapability()
{
    CALL_DEBUG_ENTER;
    return (HasInputProperty(INPUT_PROP_DIRECT) &&
            HasEventType(EV_ABS) &&
            ((HasEventCode(EV_ABS, ABS_X) && HasEventCode(EV_ABS, ABS_Y)) ||
            (HasEventCode(EV_ABS, ABS_MT_POSITION_X) && HasEventCode(EV_ABS, ABS_MT_POSITION_Y))));
}

void Device::ProcessSyncEvent(int32_t code, int32_t value)
{
    const auto& event = absEventCollector_.HandleSyncEvent(code, value);
    if (event) {
        OnEventCollected(event);
        absEventCollector_.AfterProcessed();
    }
}

void Device::ProcessAbsEvent(int32_t code, int32_t value)
{
    absEventCollector_.HandleAbsEvent(code, value);
}

void Device::OnEventCollected(const std::shared_ptr<const AbsEvent>& event)
{
    if (!event) {
        MMI_HILOGE("OnEventCollected event is null");
        return;
    }
    eventHandler_->OnInputEvent(event);
    return;
}

bool Device::HasCapability(int32_t capability) const
{
    return (capabilities_ & capability) != 0;
}

int32_t Device::StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler)
{
    CALL_DEBUG_ENTER;
    if (!eventHandler) {
        MMI_HILOGE("Leave, null eventHandler");
        return -1;
    }

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return -1;
    }

    eventHandler_ = eventHandler;
    return 0;
}

int32_t Device::StopReceiveEvents()
{
    eventHandler_ = IKernelEventHandler::GetDefault();

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return -1;
    }
    return 0;
}

int Device::EventIsType(const struct input_event *ev, unsigned int type)
{
	return type < EV_CNT && ev->type == type;
}

int Device::EventtTypeGetMax(unsigned int type)
{
	if (type > EV_MAX)
		return -1;

	return ev_max[type];
}

int Device::EventIsCode(const struct input_event *ev, unsigned int type, unsigned int code)
{
	int max;

	if (!EventIsType(ev, type))
		return 0;

	max = EventtTypeGetMax(type);
	return (max > -1 && code <= (unsigned int)max && ev->code == code);
}
} // namespace MMI
} // namespace OHOS