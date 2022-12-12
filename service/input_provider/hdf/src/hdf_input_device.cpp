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

#include "hdf_input_device.h"

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "i_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HDFInputDevice" };
} // namespace

HDFInputDevice::HDFInputDevice(uint32_t devIndex, IInputInterface *inputInterface , InputEventCb eventCb) 
    : IInputDevice(devIndex), deviceId_(0), inputInterface_(inputInterface), eventCb_(eventCb)
{}

int32_t HDFInputDevice::GetDeviceId()
{
    return deviceId_;
}

int32_t HDFInputDevice::Enable()
{
    CALL_DEBUG_ENTER;
    CHKPR(inputInterface_, ERROR_NULL_POINTER);
    CHKPR(inputInterface_->iInputManager, ERROR_NULL_POINTER);
    CHKPR(inputInterface_->iInputController, ERROR_NULL_POINTER);
    CHKPR(inputInterface_->iInputReporter, ERROR_NULL_POINTER);
    
    auto ret = inputInterface_->iInputManager->OpenInputDevice(index_);
    if (ret != RET_OK) {
        Disable();
        return RET_ERR;
    }

    do {
        InputDeviceInfo *devInfo { nullptr };
        ret = inputInterface_->iInputManager->GetInputDevice(index_, &devInfo);
        if (ret != INPUT_SUCCESS) {
            break;
        }
        CHKPB(devInfo);
        devInfo_ = *devInfo;
 
        UpdateCapability();

        if (HasCapability(IInputDevice::CAPABILITY_TOUCHSCREEN)) {
            iEventHandler_ =  IEventHandler::CreateInstance();
            iEventHandler_->SetDeviceId(deviceId_);
        }
        if(CheckAndUpdateAxisInfo() != RET_OK) {
            MMI_HILOGE("CheckAndUpdateAxisInfo failed");
            break;
        }
        ret = inputInterface_->iInputReporter->RegisterReportCallback(index_, &eventCb_);
        return RET_OK;
    } while (0);
    
    Disable();
    return RET_ERR;
}

int32_t HDFInputDevice::Disable()
{
    CALL_DEBUG_ENTER;
    CHKPR(inputInterface_, ERROR_NULL_POINTER);
    CHKPR(inputInterface_->iInputManager, ERROR_NULL_POINTER);

    auto ret = inputInterface_->iInputManager->CloseInputDevice(index_);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", index_, ret);
        return RET_ERR;
    }
    MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", index_);
    return RET_OK;
}

int32_t HDFInputDevice::CheckAndUpdateAxisInfo()
{
    auto xInfo = GetAxisInfo(AXIS_MT_X);
    if (!xInfo) {
        MMI_HILOGE("null AxisInfo Of AXIS_MT_X");
        return RET_ERR;
    }
    if (xInfo->GetMinimum() >= xInfo->GetMaximum()) {
        MMI_HILOGE("xInfo->GetMinimum():%{public}d >= xInfo->GetMaximum():%{public}d",
                    xInfo->GetMinimum(), xInfo->GetMaximum());
        return RET_ERR;
    }

    auto yInfo = GetAxisInfo(AXIS_MT_Y);
    if (!yInfo) {
        MMI_HILOGE("null AxisInfo Of AXIS_MT_Y");
        return RET_ERR;
    }
    if (yInfo->GetMinimum() >= yInfo->GetMaximum()) {
        MMI_HILOGE("yInfo->GetMinimum():%{public}d >= yInfo->GetMaximum():%{public}d",
                    yInfo->GetMinimum(), yInfo->GetMaximum());
        return RET_ERR;    
    }
    CHKPR(iEventHandler_, ERROR_NULL_POINTER);
    iEventHandler_->SetAxisInfo(xInfo, yInfo);
    return RET_OK;
}

std::shared_ptr<IInputDevice::AxisInfo> HDFInputDevice::GetAxisInfo(int32_t axis) const
{
    auto it = axises_.find(axis);
    if (it != axises_.end()) {
        return it->second;
    }
    int32_t absCode = -1;
    switch (axis) {
        case AXIS_MT_X: {
            absCode = ABS_MT_POSITION_X;
            break;
        }
        case AXIS_MT_Y: {
            absCode = ABS_MT_POSITION_Y;
            break;
        }
        default: {
            return nullptr;
        }
    }
    const auto &dimensionInfo = devInfo_.attrSet.axisInfo[absCode];
    auto axisInfo = std::make_shared<IInputDevice::AxisInfo>();
    CHKPP(axisInfo);
    axisInfo->SetMinimum(dimensionInfo.min);
    axisInfo->SetMaximum(dimensionInfo.max);
    axisInfo->SetFlat(dimensionInfo.flat);
    axisInfo->SetFuzz(dimensionInfo.fuzz);
    axises_.insert(std::make_pair(axis, axisInfo));
    return axisInfo;
}

std::string HDFInputDevice::GetName()
{
    return "HDFInputDevice";
}

int32_t HDFInputDevice::GetCapabilities()
{
    return capabilities_;
}

std::string HDFInputDevice::GetPath()
{
    return ""; 
}

void HDFInputDevice::SetDeviceId(int32_t id)
{
    deviceId_ = id;
}

InputDeviceInfo HDFInputDevice::GetInputDevInfo()
{
    return devInfo_;
}


std::shared_ptr<IEventHandler> HDFInputDevice::GetEventHandler() {
    return iEventHandler_;
}

int32_t HDFInputDevice::UpdateCapability()
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
    }
    return RET_OK;
}
bool HDFInputDevice::HasMouseCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_REL) && HasEventCode(EV_REL, REL_X) &&
        HasEventCode(EV_REL, REL_Y) && HasInputProperty(INPUT_PROP_POINTER));
}

bool HDFInputDevice::HasKeyboardCapability()
{
    CALL_DEBUG_ENTER;
    return (HasEventType(EV_KEY) && !HasEventType(EV_ABS) && !HasEventType(EV_REL));
}

bool HDFInputDevice::HasTouchscreenCapability()
{
    CALL_DEBUG_ENTER;
    return (HasInputProperty(INPUT_PROP_DIRECT) && HasEventType(EV_ABS) &&
        ((HasEventCode(EV_ABS, ABS_X) && HasEventCode(EV_ABS, ABS_Y)) ||
        (HasEventCode(EV_ABS, ABS_MT_POSITION_X) && HasEventCode(EV_ABS, ABS_MT_POSITION_Y))));

}

bool HDFInputDevice::HasEventType(unsigned int evType)
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(evType);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(EV_CNT)) {
        MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
        return false;
    }
    auto curBit = (1UL) << offset;
    return devInfo_.abilitySet.eventType[index] & curBit;
}

bool HDFInputDevice::HasEventCode(unsigned int evType, unsigned int evCode)
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
            return devInfo_.abilitySet.keyCode[index] & curBit;
        }
        case EV_ABS: {
            if (index >= BITS_TO_UINT64(ABS_CNT)) {
                MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
                return false;
            }
            return devInfo_.abilitySet.absCode[index] & curBit;
        }
        case EV_REL: {
            if (index >= BITS_TO_UINT64(REL_CNT)) {
                MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
                return false;
            }
            return devInfo_.abilitySet.relCode[index] & curBit;
        }
        default: {
            MMI_HILOGE("The current evType:%{public}d is not supported, index:%{public}u, offset:%{public}u",
                evType, index, offset);
            return false;
        }
    }  
}

bool HDFInputDevice::HasInputProperty(unsigned int property)
{
    CALL_DEBUG_ENTER;
    auto [index, offset] = GetBitLoc(property);
    if (offset > BITS_PER_LONG || index >= BITS_TO_UINT64(INPUT_PROP_CNT)) {
        MMI_HILOGE("Error, index:%{public}u, offset:%{public}u", index, offset);
        return false;
    }
    auto curBit = (1UL) << offset;
    return devInfo_.abilitySet.devProp[index] & curBit;
}

bool HDFInputDevice::HasCapability(int32_t capability)
{
    return (capabilities_ & capability) != 0;
}
} // namespace MMI
} // namespace OHOS