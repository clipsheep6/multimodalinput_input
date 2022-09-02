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

#include "touch_transform_point_manager.h"

#include "tablet_tool_processor.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchTransformPointManager" };
} // namespace

TouchTransformPointManager::TouchTransformPointManager() {}
TouchTransformPointManager::~TouchTransformPointManager() {}

std::shared_ptr<PointerEvent> TouchTransformPointManager::OnLibInput(
    struct libinput_event *event, INPUT_DEVICE_TYPE deviceType)
{
    CHKPP(event);
    switch (deviceType) {
        case INPUT_DEVICE_CAP_TOUCH: {
#ifdef OHOS_BUILD_ENABLE_TOUCH
            return OnLibinputTouchEvent(event);
#endif // OHOS_BUILD_ENABLE_TOUCH
        }
        case INPUT_DEVICE_CAP_TABLET_TOOL: {
#ifdef OHOS_BUILD_ENABLE_TOUCH
            return OnLibinputTabletToolEvent(event);
#endif // OHOS_BUILD_ENABLE_TOUCH
        }
        case INPUT_DEVICE_CAP_TOUCH_PAD: {
#ifdef OHOS_BUILD_ENABLE_POINTER
            return OnLibinputTouchPadEvent(event);
#endif // OHOS_BUILD_ENABLE_POINTER
        }
        case INPUT_DEVICE_CAP_GESTURE: {
#ifdef OHOS_BUILD_ENABLE_POINTER
            return OnTouchPadGestureEvent(event);
#endif // OHOS_BUILD_ENABLE_POINTER
        }
        default: {
            MMI_HILOGE("The in parameter deviceType is error, deviceType:%{public}d", deviceType);
            break;
        }
    }
    return nullptr;
}

#ifdef OHOS_BUILD_ENABLE_TOUCH
std::shared_ptr<PointerEvent> TouchTransformPointManager::OnLibinputTouchEvent(struct libinput_event *event)
{
    CHKPP(event);
    auto device = libinput_event_get_device(event);
    CHKPP(device);
    std::shared_ptr<TouchTransformPointProcessor> processor = nullptr;
    auto deviceId = InputDevMgr->FindInputDeviceId(device);
    if (auto it = touchPro_.find(deviceId); it != touchPro_.end()) {
        processor = it->second;
    } else {
        processor = std::make_shared<TouchTransformPointProcessor>(deviceId);
        CHKPP(processor);
        auto iter = touchPro_.insert(
            std::pair<int32_t, std::shared_ptr<TouchTransformPointProcessor>>(deviceId, processor));
        if (!iter.second) {
            MMI_HILOGE("Insert value failed, touch device:%{public}d", deviceId);
        }
    }
    return processor->OnLibinputTouchEvent(event);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_TOUCH
std::shared_ptr<PointerEvent> TouchTransformPointManager::OnLibinputTabletToolEvent(struct libinput_event *event)
{
    CHKPP(event);
    auto device = libinput_event_get_device(event);
    CHKPP(device);
    std::shared_ptr<TransformPointProcessor> processor = nullptr;
    auto deviceId = InputDevMgr->FindInputDeviceId(device);

    if (auto it = processors_.find(deviceId); it != processors_.end()) {
        processor = it->second;
    } else {
        processor.reset(new (std::nothrow) TabletToolProcessor(deviceId));
        CHKPP(processor);
        auto ret = processors_.emplace(deviceId, processor);
        if (!ret.second) {
            MMI_HILOGE("Duplicate device record:%{public}d", deviceId);
        }
    }
    return processor->OnEvent(event);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
std::shared_ptr<PointerEvent> TouchTransformPointManager::OnLibinputTouchPadEvent(struct libinput_event *event)
{
    CHKPP(event);
    auto device = libinput_event_get_device(event);
    CHKPP(device);
    std::shared_ptr<TouchPadTransformPointProcessor> processor = nullptr;
    auto deviceId = InputDevMgr->FindInputDeviceId(device);
    auto it = touchpadPro_.find(deviceId);
    if (it != touchpadPro_.end()) {
        processor = it->second;
    } else {
        processor = std::make_shared<TouchPadTransformPointProcessor>(deviceId);
        CHKPP(processor);
        auto iter = touchpadPro_.insert(
            std::pair<int32_t, std::shared_ptr<TouchPadTransformPointProcessor>>(deviceId, processor));
        if (!iter.second) {
            MMI_HILOGE("Insert value failed, touchpad device:%{public}d", deviceId);
        }
    }
    return processor->OnLibinputTouchPadEvent(event);
}
#endif // OHOS_BUILD_ENABLE_POINTER

#ifdef OHOS_BUILD_ENABLE_POINTER
std::shared_ptr<PointerEvent> TouchTransformPointManager::OnTouchPadGestureEvent(struct libinput_event *event)
{
    CHKPP(event);
    auto device = libinput_event_get_device(event);
    CHKPP(device);
    std::shared_ptr<GestureTransformPointProcessor> processor = nullptr;
    auto deviceId = InputDevMgr->FindInputDeviceId(device);
    auto it = gesturePro_.find(deviceId);
    if (it != gesturePro_.end()) {
        processor = it->second;
    } else {
        processor = std::make_shared<GestureTransformPointProcessor>(deviceId);
        CHKPP(processor);
        auto iter = gesturePro_.insert(
            std::pair<int32_t, std::shared_ptr<GestureTransformPointProcessor>>(deviceId, processor));
        if (!iter.second) {
            MMI_HILOGE("Insert value failed, gesture device:%{public}d", deviceId);
        }
    }
    return processor->OnTouchPadGestureEvent(event);
}
#endif // OHOS_BUILD_ENABLE_POINTER
} // namespace MMI
} // namespace OHOS
