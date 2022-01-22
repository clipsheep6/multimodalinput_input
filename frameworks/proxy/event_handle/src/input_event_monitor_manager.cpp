/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "input_event_monitor_manager.h"
#include "define_multimodal.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputEventMonitorManager" };
}

InputEventMonitorManager::InputEventMonitorManager()
{
}

InputEventMonitorManager::~InputEventMonitorManager()
{
}

int32_t InputEventMonitorManager::AddInputEventMontior(
    std::function<void (std::shared_ptr<OHOS::MMI::KeyEvent>)> keyEventMonitor)
{
    CHKPR(keyEventMonitor, ERROR_NULL_POINTER, ERROR_NULL_POINTER);
    static int32_t monitorId = INVALID_MONITOR_ID;
    MonitorItem monitorItem;
    monitorItem.keyEventMonitor = keyEventMonitor;
    monitorItem.id_ = ++monitorId;
    monitors_.push_back(monitorItem);
    MMI_LOGD("monitorId: %{public}d", monitorId);
    MMIEventHdl.AddInputEventMontior(OHOS::MMI::InputEvent::EVENT_TYPE_KEY);
    return OHOS::MMI_STANDARD_EVENT_SUCCESS;
}

void InputEventMonitorManager::RemoveInputEventMontior(int32_t monitorId)
{
    if (monitorId < 0 ) {
        MMI_LOGE("MonitorId invalid");
	    return;
    }
    MonitorItem monitorItem;
    monitorItem.id_ = monitorId;
    std::list<MonitorItem>::iterator iter;
    iter = std::find(monitors_.begin(), monitors_.end(), monitorItem);
    if (iter == monitors_.end()) {
        MMI_LOGW("MonitorItem does not exist");
        return;
    }
    iter = monitors_.erase(iter);
    MMIEventHdl.RemoveInputEventMontior(OHOS::MMI::InputEvent::EVENT_TYPE_KEY);
    MMI_LOGD("MonitorId: %{public}d removed", monitorId);
}

int32_t InputEventMonitorManager::OnMonitorInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent)
{
    if (keyEvent == nullptr) {
        MMI_LOGE("param should not be null!");
    }
    std::list<MonitorItem>::iterator iter;
    for (iter = monitors_.begin(); iter != monitors_.end(); iter++) {
        MMI_LOGD("send msg");
        iter->keyEventMonitor(keyEvent);
    }
    return OHOS::MMI_STANDARD_EVENT_SUCCESS;
}

int32_t InputEventMonitorManager::AddInputEventTouchpadMontior(
    std::function<void (std::shared_ptr<OHOS::MMI::PointerEvent>)> TouchPadEventMonitor)
{
    if (TouchPadEventMonitor == nullptr) {
        MMI_LOGE("param should not be null");
        return INVALID_MONITOR_ID;
    }
    static int32_t monitorId = 0;
    MonitorItem monitorItem;
    monitorItem.TouchPadEventMonitor = TouchPadEventMonitor;
    monitorItem.id_ = ++monitorId;
    monitors_.push_back(monitorItem);
    MMI_LOGD("monitorId: %{public}d", monitorId);
    MMIEventHdl.AddInputEventTouchpadMontior(OHOS::MMI::InputEvent::EVENT_TYPE_POINTER);
    MMI_LOGD("leave");
    return monitorItem.id_;
}

void InputEventMonitorManager::RemoveInputEventTouchpadMontior(int32_t monitorId)
{
    if (monitorId <= 0) {
        MMI_LOGE("monitorId invalid");
        return;
    }
    MonitorItem monitorItem;
    monitorItem.id_ = monitorId;
    std::list<MonitorItem>::iterator iter;
    iter = std::find(monitors_.begin(), monitors_.end(), monitorItem);
    if (iter == monitors_.end()) {
        MMI_LOGE("monitor item does not exist");
    } else {
        iter = monitors_.erase(iter);
        MMIEventHdl.RemoveInputEventTouchpadMontior(OHOS::MMI::InputEvent::EVENT_TYPE_POINTER);
        MMI_LOGD("monitorItem id: %{public}d removed", monitorId);
    }
}

int32_t InputEventMonitorManager::OnTouchpadMonitorInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent)
{
    MMI_LOGD("enter");
    if (pointerEvent == nullptr) {
        MMI_LOGE("param should not be null");
    }
    std::list<MonitorItem>::iterator iter;
    for (iter = monitors_.begin(); iter != monitors_.end(); iter++) {
        MMI_LOGD("send msg");
        iter->TouchPadEventMonitor(pointerEvent);
    }
    PointerEvent::PointerItem pointer;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointer);
    MMI_LOGT("\nmonitor-client\neventTouchpad:time=%{public}d;"
             "sourceType=%{public}d;action=%{public}d;"
             "pointerId=%{public}d;point.x=%{public}d;point.y=%{public}d;press=%{public}d",
             pointerEvent->GetActionTime(), pointerEvent->GetSourceType(), pointerEvent->GetPointerAction(),
             pointerEvent->GetPointerId(), pointer.GetGlobalX(), pointer.GetGlobalY(), pointer.IsPressed());
    return OHOS::MMI_STANDARD_EVENT_SUCCESS;
}
}
}