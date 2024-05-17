/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "crown_event_normalize.h"

#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_CROWN
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "CrownEventNormalize"};
} // namespace

CrownEventNormalize::CrownEventNormalize() {}

CrownEventNormalize::~CrownEventNormalize() {}

std::shared_ptr<CrownTransformProcessor> CrownEventNormalize::GetProcessor(int32_t deviceId) const
{
    std::shared_ptr<CrownTransformProcessor> processor { nullptr };
    if (auto it = processors_.find(deviceId); it != processors_.end()) {
        processor = it->second;
    } else {
        processor = std::make_shared<CrownTransformProcessor>(deviceId);
        auto [iIter, isOk] = processors_.emplace(deviceId, processor);
        if (!isOk) {
            MMI_HILOGE("Duplicate device record, deviceId: %{public}d", deviceId);
        }
    }

    return processor;
}

std::shared_ptr<CrownTransformProcessor> CrownEventNormalize::GetCurrentProcessor() const
{
    int32_t deviceId = GetCurrentDeviceId();
    auto iter = processors_.find(deviceId);
    if (iter == processors_.end()) {
        MMI_HILOGE("Can't find crown processor by deviceId: %{public}d", deviceId);
        return nullptr;
    }

    return iter->second;
}

void CrownEventNormalize::SetCurrentDeviceId(int32_t deviceId)
{
    currentDeviceId_ = deviceId;
}

int32_t CrownEventNormalize::GetCurrentDeviceId() const
{
    return currentDeviceId_;
}

bool CrownEventNormalize::IsCrownEvent(struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, false);
    auto device = libinput_event_get_device(event);
    CHKPR(device, false);
    std::string name = libinput_device_get_name(device);
    if (name == CROWN_SOURCE) {
        auto type = libinput_event_get_type(event);
        if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
            struct libinput_event_keyboard *keyBoard = libinput_event_get_keyboard_event(event);
            CHKPR(keyBoard, false);
            auto key = libinput_event_keyboard_get_key(keyBoard);
            if (key != CROWN_CODE_POWER) {
                MMI_HILOGD("not crown event, unknown key: %{public}d", key);
                return false;
            }
            return true;
        } else if (type == LIBINPUT_EVENT_POINTER_AXIS) {
            struct libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(event);
            CHKPR(pointerEvent, false);
            auto source = libinput_event_pointer_get_axis_source(event);
            if (source != LIBINPUT_POINTER_AXIS_SOURCE_WHEEL) {
                MMI_HILOGD("not crown event, unknown axis source: %{public}d", source);
                return false;
            }
            return true;
        }
    }
    
    MMI_HILOGD("not crown event, unknown device name: %{public}s", name.c_str());
    return false;
}

int32_t CrownEventNormalize::NormalizeKeyEvent(struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    int32_t deviceId = InputDevMgr->FindInputDeviceId(device);
    if (deviceId < 0) {
        MMI_HILOGE("The deviceId is invalid, deviceId: %{public}d", deviceId);
        return PARAM_INPUT_INVALID;
    }

    std::shared_ptr<CrownTransformProcessor> processor = GetProcessorId(deviceId);
    if (!processor) {
        MMI_HILOGE("not found crown processor for deviceId: %{public}d", deviceId);
        return PARAM_INPUT_INVALID;
    }

    return processor->NormalizeKeyEvent(event);
}

int32_t CrownEventNormalize::NormalizeRotateEvent(struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    int32_t deviceId = InputDevMgr->FindInputDeviceId(device);
    if (deviceId < 0) {
        MMI_HILOGE("The deviceId is invalid, deviceId: %{public}d", deviceId);
        return PARAM_INPUT_INVALID;
    }

    std::shared_ptr<CrownTransformProcessor> processor = GetProcessorId(deviceId);
    if (!processor) {
        MMI_HILOGE("not found crown processor for deviceId: %{public}d", deviceId);
        return PARAM_INPUT_INVALID;
    }

    return processor->NormalizeRotateEvent(event);
}

void CrownEventNormalize::Dump(int32_t fd, const std::vector<std::string> &args)
{
    auto processor = GetCurrentProcessor();
    CHKPV(processor);
    processor->Dump(fd, args);
}
#endif // OHOS_BUILD_ENABLE_CROWN
} // namespace MMI
} // namespace OHOS