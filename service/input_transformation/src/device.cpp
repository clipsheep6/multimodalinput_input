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
#include "mmi_log.h"
#include "i_kernel_event_handler.h"

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
        return RET_ERR;
    }
    if (HasCapability(IDevice::CAPABILITY_TOUCHSCREEN) && IsMtDevice()) {
        mtdev_ = input_mtdev_open();
        isMtFlag_ = true;
        absEventCollector_.SetMtFlag(true);
    }
    if (CheckAndUpdateAxisInfo() != RET_OK) {
        MMI_HILOGE("CheckAndUpdateAxisInfo failed");
        Uninit();
        return RET_ERR;
    }
    return RET_OK;
}

int32_t Device::CheckAndUpdateAxisInfo()
{
    auto xInfo = GetAxisInfo(IDevice::AXIS_MT_X);
    if (!xInfo) {
        MMI_HILOGE("null AxisInfo Of AXIS_MT_X");
        return RET_ERR;
    }
    if (xInfo->GetMinimum() >= xInfo->GetMaximum()) {
        MMI_HILOGE("xInfo->GetMinimum():%{public}d >= xInfo->GetMaximum():%{public}d",
                    xInfo->GetMinimum(), xInfo->GetMaximum());
        return RET_ERR;
    }
    auto yInfo = GetAxisInfo(IDevice::AXIS_MT_Y);
    if (!yInfo) {
        MMI_HILOGE("null AxisInfo Of AXIS_MT_Y");
        return RET_ERR;
    }
    if (yInfo->GetMinimum() >= yInfo->GetMaximum()) {
        MMI_HILOGE("yInfo->GetMinimum():%{public}d >= yInfo->GetMaximum():%{public}d",
                    yInfo->GetMinimum(), yInfo->GetMaximum());
        return RET_ERR;
    }
    absEventCollector_.SetAxisInfo(xInfo, yInfo);
    return RET_OK;
}

void Device::Uninit()
{
    CloseDevice();
}

Device::Device(int32_t devIndex, const InputDeviceInfo &devInfo) : IDevice(devIndex),
    absEventCollector_(devIndex, AbsEvent::SOURCE_TYPE_NONE, 
    std::bind(&Device::OnEventCollected, this, std::placeholders::_1)),
    deviceOrigin_(devInfo)
{}

Device::~Device()
{
    Uninit();
}

std::shared_ptr<IDevice::AxisInfo> Device::GetAxisInfo(int32_t axis) const
{
    auto it = axises_.find(axis);
    if (it != axises_.end()) {
        return it->second;
    }
    int32_t absCode = -1;
    switch (axis) {
        case IDevice::AXIS_MT_X: {
            absCode = ABS_MT_POSITION_X;
            break;
        }
        case IDevice::AXIS_MT_Y: {
            absCode = ABS_MT_POSITION_Y;
            break;
        }
        default: {
            return nullptr;
        }
    }
    const auto &dimensionInfo = deviceOrigin_.attrSet.axisInfo[absCode];
    auto axisInfo = std::make_shared<IDevice::AxisInfo>();
    CHKPP(axisInfo);
    axisInfo->SetAxis(axis);
    axisInfo->SetMinimum(dimensionInfo.min);
    axisInfo->SetMaximum(dimensionInfo.max);
    axisInfo->SetFlat(dimensionInfo.flat);
    axisInfo->SetFuzz(dimensionInfo.fuzz);
    // axisInfo->SetResolution();  //TO DO:...
    axises_.insert(std::make_pair(axis, axisInfo));
    return axisInfo;
}

int32_t Device::CloseDevice()
{
    CHKPR(mtdev_, ERROR_NULL_POINTER);
    mtdev_delete(mtdev_);
    mtdev_ = nullptr;
    return 0;
}

void Device::ProcessEvent(const struct input_event &event)
{
    if (mtdev_ == nullptr) {
        MMI_HILOGE("wjw ProcessEvent mtdev_ = nullptr");
        ProcessEventInner(event);
        return;
    }
    mtdev_put_event(mtdev_, &event);
    if (!EventIsCode(event, EV_SYN, SYN_REPORT)) {
        return;
    }
    while (!mtdev_empty(mtdev_)) {
        struct input_event e = {};
        mtdev_get_event(mtdev_, &e);
        ProcessEventInner(e);
    }
}

void Device::ProcessEventInner(const struct input_event& event)
{
    auto type = event.type;
    auto code = event.code;
    auto value = event.value;
    switch (type) {
        case EV_SYN: {
            ProcessSyncEvent();
            break;
        }
        case EV_ABS: {
            ProcessAbsEvent(code, value);
            break;
        }
        case EV_KEY: {
            ProcessKeyEvent(code, value);
            break;
        }
        case EV_REL:
        case EV_MSC:
        case EV_SW:
        case EV_LED:
        case EV_SND:
        case EV_REP:
        case EV_FF:
        case EV_PWR:
        case EV_FF_STATUS:
        default: {
            break;
        }
    }
}

int32_t Device::UpdateCapability()
{
    CALL_DEBUG_ENTER;
    if (HasMouseCapability()) {
        capabilities_ |= IDevice::CAPABILITY_MOUSE;
    }
    if (HasKeyboardCapability()) {
        capabilities_ |= IDevice::CAPABILITY_KEYBOARD;
    }
    if (HasTouchscreenCapability()) {
        capabilities_ |= IDevice::CAPABILITY_TOUCHSCREEN;
        absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
    }
    return RET_OK;
}

bool Device::HasInputProperty(unsigned int property)
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(property);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(INPUT_PROP_CNT)) {
        MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
        return false;
    }
    auto curBit = (1UL) << offset;
    return deviceOrigin_.abilitySet.devProp[index] & curBit;
}

bool Device::HasEventType(unsigned int evType) const
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(evType);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(EV_CNT)) {
        MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
        return false;
    }
    auto curBit = (1UL) << offset;
    return deviceOrigin_.abilitySet.eventType[index] & curBit;
}

bool Device::HasEventCode(unsigned int evType, unsigned int evCode) const
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(evCode);
    if (offset > BITS_PER_LONG) {
        MMI_HILOGE("Error, offset:%{public}u", offset);
        return false;
    }
    auto curBit = (1UL) << offset;
    switch (evType) {
        case EV_KEY: {
            if (index >= BITS_TO_UINT64(KEY_CNT)) {
                MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
                return false;
            }
            return deviceOrigin_.abilitySet.keyCode[index] & curBit;
        }
        case EV_ABS: {
            if (index >= BITS_TO_UINT64(ABS_CNT)) {
                MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
                return false;
            }
            return deviceOrigin_.abilitySet.absCode[index] & curBit;
        }
        case EV_REL: {
            if (index >= BITS_TO_UINT64(REL_CNT)) {
                MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
                return false;
            }
            return deviceOrigin_.abilitySet.relCode[index] & curBit;
        }
        default: {
            MMI_HILOGE("The current evType:%{public}d is not supported, index:%{public}u, offset:%{public}u",
                evType, index, offset);
            return false;
        }
    }
}

bool Device::HasMouseCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_REL) && HasEventCode(EV_REL, REL_X) &&
        HasEventCode(EV_REL, REL_Y) && HasInputProperty(INPUT_PROP_POINTER));
}

bool Device::HasKeyboardCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_KEY) && !HasEventType(EV_ABS) && !HasEventType(EV_REL));
}

bool Device::HasTouchscreenCapability()
{
    CALL_DEBUG_ENTER;
    return (HasInputProperty(INPUT_PROP_DIRECT) && HasEventType(EV_ABS) &&
        ((HasEventCode(EV_ABS, ABS_X) && HasEventCode(EV_ABS, ABS_Y)) ||
        (HasEventCode(EV_ABS, ABS_MT_POSITION_X) && HasEventCode(EV_ABS, ABS_MT_POSITION_Y))));
}

void Device::ProcessSyncEvent()
{
    absEventCollector_.HandleSyncEvent();
}

void Device::ProcessAbsEvent(int32_t code, int32_t value)
{
    if (isMtFlag_) {
        absEventCollector_.HandleAbsEvent(code, value);
    } else {
        absEventCollector_.HandleAbsAbsoluteEvent(code, value);
    }

}

void Device::ProcessKeyEvent(int32_t code, int32_t value)
{
    absEventCollector_.HandleKeyEvent(code, value);
}

void Device::OnEventCollected(const std::shared_ptr<AbsEvent> event)
{
    if (!event) {
        MMI_HILOGE("OnEventCollected event is null");
        return;    
    }
    if (eventHandler_ == nullptr) {
        MMI_HILOGE("eventHandler_ is nullptr");
        return;
    }
    eventHandler_->OnInputEvent(event);
}

bool Device::HasCapability(int32_t capability) const
{
    return (capabilities_ & capability) != 0;
}

int32_t Device::StartReceiveEvent(const std::shared_ptr<IKernelEventHandler> handler)
{
    CALL_DEBUG_ENTER;
    CHKPR(handler, ERROR_NULL_POINTER);
    eventHandler_ = handler;
    return RET_OK;
}

int32_t Device::StopReceiveEvent()
{
    eventHandler_ = nullptr;
    return 0;
}

int Device::EventIsType(const struct input_event& ev, unsigned int type)
{
	return ((type < EV_CNT) && (ev.type == type));
}

int Device::EventTypeGetMax(unsigned int type)
{
	if (type > EV_MAX) {
        MMI_HILOGE("Error, type:%{public}u > EV_MAX", type);
		return RET_ERR;
    }
	return ev_max[type];
}

int Device::EventIsCode(const struct input_event& ev, unsigned int type, unsigned int code)
{
	if (!EventIsType(ev, type)) {
		return RET_OK;
    }
	int max = EventTypeGetMax(type);
	return ((max > -1) && (code <= static_cast<unsigned int>(max)) && (ev.code == code));
}

bool Device::IsMtDevice()
{
    return HasEventCode(EV_ABS, ABS_MT_SLOT);// To Do
}


} // namespace MMI
} // namespace OHOS