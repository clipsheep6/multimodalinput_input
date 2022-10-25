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
    auto retCode = UpdateCapablility();
    if (retCode < 0) {
        MMI_HILOGE("Leave device Init, UpdateCapablility Failed");
        return -1;
    }

    mtdev_ = mmi_mtdev_new_open();
    return 0;
}

void Device::Uninit() 
{
    CloseDevice();
}

Device::Device(int32_t id, const std::shared_ptr<IInputContext> context, const InputDimensionInfo &dimensionInfoX,
               const InputDimensionInfo &dimensionInfoY)
   : id_(id), context_(context), dimensionInfoX_(dimensionInfoX), dimensionInfoY_(dimensionInfoY), keyEventCollector_(id), 
   absEventCollector_(id, AbsEvent::SOURCE_TYPE_NONE), eventHandler_(IKernelEventHandler::GetDefault()) {}

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

    // if (!HasEventCode(EV_ABS, absCode)) {
    //     MMI_HILOGE("Leave device:%{public}s axis:%{public}s, absCode:%{public}s, InputDevice Not support axis", GetName().c_str(), IInputDevice::AxisToString(axis),
    //             EnumUtils::InputEventAbsCodeToString(absCode));
    //     return nullptr;
    // }

    auto axisInfo = std::make_shared<IInputDevice::AxisInfo>();
    axisInfo->SetAxis(axis);
    axisInfo->SetMinimum(dimensionInfo.min);
    axisInfo->SetMaximum(dimensionInfo.max);
    axisInfo->SetFlat(dimensionInfo.flat);
    axisInfo->SetFuzz(dimensionInfo.fuzz);
    // axisInfo->SetResolution();

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
    CHKPV(mtdev_);
    mtdev_put_event(mtdev_, eventItem);
    if (mmi_libevdev_event_is_code(eventItem, EV_SYN, SYN_REPORT)) {
        while (!mtdev_empty(mtdev_)) {
            struct input_event e;
            mtdev_get_event(mtdev_, &e);
            mmi_evdev_process_event(&e);
        }
    }    
}

void Device::mmi_evdev_process_event(const struct input_event* eventItem)
{
    CALL_DEBUG_ENTER;
    auto type = eventItem->type;
    auto code = eventItem->code;
    auto value = eventItem->value;
    switch (type) {
        case EV_SYN:
            ProcessSyncEvent(code, value);
            break;
        case EV_KEY:
            ProcessKeyEvent(code, value);
            break;
        case EV_ABS:
            ProcessAbsEvent(code, value);
            break;
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

int32_t Device::UpdateCapablility()
{
    CALL_DEBUG_ENTER;
    // auto retCode = UpdateInputProperty();
    // if (retCode < 0) {
    //     return -1;
    // }

    // auto retCode = UpdateBitStat(0, EV_MAX, &evBit[0], LENTH_OF_ARRAY(evBit));
    // if (retCode < 0) {
    //     MMI_HILOGD("UpdateBitStat 0");
    //     return -1;
    // }

    // retCode = UpdateBitStat(EV_REL, REL_MAX, &relBit[0], LENTH_OF_ARRAY(relBit));
    // if (retCode < 0) {
    //     MMI_HILOGD("UpdateBitStat EV_REL");
    //     return -1;
    // }

    // retCode = UpdateBitStat(EV_ABS, ABS_MAX, &absBit[0], LENTH_OF_ARRAY(absBit));
    // if (retCode < 0) {
    //     MMI_HILOGD("UpdateBitStat EV_ABS");
    //     return -1;
    // }

    if (HasMouseCapability()) {
        capabilities_ |= IInputDevice::CAPABILITY_MOUSE;
    }

    if (HasKeyboardCapability()) {
        capabilities_ |= IInputDevice::CAPABILITY_KEYBOARD;
    }

    if (HasTouchscreenCapability()) {
        MMI_HILOGD("HasTouchscreenCapability");
        capabilities_ |= IInputDevice::CAPABILITY_TOUCHSCREEN;
        absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
    } else {
        MMI_HILOGD("!HasTouchscreenCapability");
    }

    // if (HasTouchpadCapability()) {
    //     capabilities_ |= IInputDevice::CAPABILITY_TOUCHPAD;
    //     absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHPAD);
    //     // capabilities_ |= IInputDevice::CAPABILITY_TOUCHSCREEN;
    //     // absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
    // }

    return 0;
}

// int32_t Device::UpdateInputProperty()
// {
//     auto ret = ioctl(fd_, EVIOCGPROP(sizeof(inputProperty)), &inputProperty[0]);
//     if (ret < 0) {
//         return -1;
//     }

//     for (int32_t property = 0; property <= INPUT_PROP_MAX; ++property) {
//         bool has = HasInputProperty(property);
//         MMI_HILOGD("InputDevice(Name:%{public}s) %{public}s Property:%{public}s",  GetName().c_str(),
//                 has ? "has" : "hasn't", 
//                 EnumUtils::InputPropertyToString(property));
//     }
//     return 0;
// }

int32_t Device::UpdateBitStat(int32_t evType, int32_t maxValue, unsigned long* resultValue, size_t len)
{
    // auto ret = ioctl(fd_, EVIOCGBIT(evType, maxValue), resultValue);
    // if (ret < 0) {
    //     MMI_HILOGE("Leave, Failed for %{public}s", EnumUtils::InputEventTypeToString(evType));
    //     return -1;
    // }

    /*
        std::stringstream debugStr;
        debugStr << EnumUtils::InputEventTypeToString(evType) << " : ";
        for (auto i = len; i > 0; --i) {
        debugStr << std::hex << std::setw(16) << std::setfill('0') << resultValue[i-1];
        }
        debugStr << "[END]";
        MMI_HILOGD("Device:%{public}s BITSTAT:%{public}s", GetName(), debugStr.str());
        */

    const char* typeStr = "";
    if (evType != 0) {
        typeStr = EnumUtils::InputEventTypeToString(evType);
    }

    for (int32_t item = 0; item <= maxValue; ++item) {
        const char* has = TestBit(item, resultValue, len) ? "has" : "hasn't";
        const char* valueStr = (evType == 0 ? EnumUtils::InputEventTypeToString(item) : EnumUtils::InputEventCodeToString(evType, item));
    }

    return 0;
}

bool Device::TestBit(int32_t bitIndex, const unsigned long* bitMap, size_t count) const
{
    if (bitIndex < 0) {
        return false;
    }

    size_t idx = bitIndex / (sizeof(unsigned long) * 8);
    if (idx >= count) {
        return false;
    }

    size_t offset = bitIndex % (sizeof(unsigned long) * 8);
    unsigned long bitOne = 1UL;
    return (bitMap[idx] & (bitOne << offset)) != 0;
}

bool Device::HasInputProperty(int32_t property)
{
    return TestBit(property, &inputProperty[0], sizeof(inputProperty) / sizeof(inputProperty[0]));
}

bool Device::HasMouseCapability()
{
    if (HasEventCode(EV_REL, REL_X) && HasEventCode(EV_REL, REL_Y)) {
        return true;
    }
    return false;
}

bool Device::HasKeyboardCapability()
{
    if (!HasEventType(EV_KEY)) {
        return false;
    }

    return true;
}

bool Device::HasTouchscreenCapability()
{
    // if (!HasEventType(EV_ABS)) {
    //     return false;
    // }

    // if (HasInputProperty(INPUT_PROP_POINTER)) {
    //     return false;
    // }

    return true;
}

bool Device::HasTouchpadCapability()
{
    if (!HasEventType(EV_ABS)) {
        return false;
    }

    // if (!HasInputProperty(INPUT_PROP_POINTER)) {
    //     return false;
    // }

    return true;
}

bool Device::HasEventType(int32_t evType) const
{
    return TestBit(evType, &evBit[0], LENTH_OF_ARRAY(evBit));
}

bool Device::HasEventCode(int32_t evType, int32_t evCode) const
{
    if (!HasEventType(evType)) {
        return false;
    }

    switch(evType) {
        case EV_REL:
            return TestBit(evCode, &relBit[0], LENTH_OF_ARRAY(relBit));
        case EV_ABS:
            return TestBit(evCode, &absBit[0], LENTH_OF_ARRAY(absBit));
        default:
            MMI_HILOGE("NOT IMPLEMENTATION");
            return false;
    }
}

void Device::ProcessSyncEvent(int32_t code, int32_t value)
{
    const auto& event = absEventCollector_.HandleSyncEvent(code, value);
    if (event) {
        OnEventCollected(event);
        absEventCollector_.AfterProcessed();
    }
}

void Device::ProcessKeyEvent(int32_t code, int32_t value) {
    CALL_DEBUG_ENTER;
	if (value == 2) {
        return;
    }
    if (code == BTN_TOUCH) {
        return;
    }
    auto event = keyEventCollector_.HandleKeyEvent(code, value);
    if (event) {
        OnEventCollected(event);
        keyEventCollector_.AfterProcessed();
        MMI_HILOGD("HandleKeyEvent code:%{public}s value::%{public}d KeyEvent", EnumUtils::InputEventKeyCodeToString(code), value);
        return;
    }
}

void Device::ProcessAbsEvent(int32_t code, int32_t value)
{
    const auto& event = absEventCollector_.HandleAbsEvent(code, value);
    if (event) {
        // OnEventCollected(event);
        absEventCollector_.AfterProcessed();
    }
}

void Device::OnEventCollected(const std::shared_ptr<const KernelKeyEvent>& event) {
    if (!event) {
        return;
    }
    //eventHandler_->OnInputEvent(event);
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

int Device::mmi_libevdev_event_is_type(const struct input_event *ev, unsigned int type)
{
	return type < EV_CNT && ev->type == type;
}

int Device::mmi_libevdev_event_type_get_max(unsigned int type)
{
	if (type > EV_MAX)
		return -1;

	return ev_max[type];
}

int Device::mmi_libevdev_event_is_code(const struct input_event *ev, unsigned int type, unsigned int code)
{
	int max;

	if (!mmi_libevdev_event_is_type(ev, type))
		return 0;

	max = mmi_libevdev_event_type_get_max(type);
	return (max > -1 && code <= (unsigned int)max && ev->code == code);
}
} // namespace MMI
} // namespace OHOS