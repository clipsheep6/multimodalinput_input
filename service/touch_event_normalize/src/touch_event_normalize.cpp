/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "touch_event_normalize.h"
#include "gesture_transform_processor.h"
#include "input_device_manager.h"
#include "joystick_transform_processor.h"
#include "tablet_tool_tranform_processor.h"
#include "touch_transform_processor.h"
#include "touchpad_transform_processor.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL{ LOG_CORE, MMI_LOG_DOMAIN, "TouchEventNormalize" };
} // namespace

TouchEventNormalize::TouchEventNormalize() {}
TouchEventNormalize::~TouchEventNormalize() {}

std::shared_ptr<PointerEvent> TouchEventNormalize::OnLibInput(struct libinput_event *event, DeviceType deviceType)
{
    CHKPP(event);
    auto device = libinput_event_get_device(event);
    CHKPP(device);
    std::shared_ptr<TransformProcessor> processor{ nullptr };
    auto deviceId = InputDevMgr->FindInputDeviceId(device);
    if (auto it = processors_.find(deviceId); it != processors_.end()) {
        processor = it->second;
    } else {
        processor = MakeTransformProcessor(deviceId, deviceType);
        CHKPP(processor);
        auto [tIter, isOk] = processors_.emplace(deviceId, processor);
        if (!isOk) {
            MMI_HILOGE("Duplicate device record:%{public}d", deviceId);
        }
    }
    return processor->OnEvent(event);
}

std::shared_ptr<TransformProcessor> TouchEventNormalize::MakeTransformProcessor(int32_t deviceId,
    DeviceType deviceType) const
{
    std::shared_ptr<TransformProcessor> processor{ nullptr };
    switch (deviceType) {
#ifdef OHOS_BUILD_ENABLE_TOUCH
        case DeviceType::TOUCH: {
            processor = std::make_shared<TouchTransformProcessor>(deviceId);
            break;
        }
        case DeviceType::TABLET_TOOL: {
            processor = std::make_shared<TabletToolTransformProcessor>(deviceId);
            break;
        }
#endif // OHOS_BUILD_ENABLE_TOUCH
#ifdef OHOS_BUILD_ENABLE_POINTER
        case DeviceType::TOUCH_PAD: {
            processor = std::make_shared<TouchPadTransformProcessor>(deviceId);
            break;
        }
        case DeviceType::GESTURE: {
            processor = std::make_shared<GestureTransformProcessor>(deviceId);
            break;
        }
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_JOYSTICK
        case DeviceType::JOYSTICK: {
            processor = std::make_shared<JoystickTransformProcessor>(deviceId);
            break;
        }
#endif // OHOS_BUILD_ENABLE_JOYSTICK
        default: {
            MMI_HILOGE("Unsupported device type: %{public}d", deviceType);
            break;
        }
    }
    return processor;
}

int32_t TouchEventNormalize::SetTouchpadPinchSwitch(bool switchFlag) const
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    return TouchPadTransformProcessor::SetTouchpadPinchSwitch(switchFlag);
#else
    return RET_OK;
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t TouchEventNormalize::GetTouchpadPinchSwitch(bool &switchFlag) const
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    return TouchPadTransformProcessor::GetTouchpadPinchSwitch(switchFlag);
#else
    return RET_OK;
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t TouchEventNormalize::SetTouchpadSwipeSwitch(bool switchFlag) const
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    return TouchPadTransformProcessor::SetTouchpadSwipeSwitch(switchFlag);
#else
    return RET_OK;
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t TouchEventNormalize::GetTouchpadSwipeSwitch(bool &switchFlag) const
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    return TouchPadTransformProcessor::GetTouchpadSwipeSwitch(switchFlag);
#else
    return RET_OK;
#endif // OHOS_BUILD_ENABLE_POINTER
}
} // namespace MMI
} // namespace OHOS
