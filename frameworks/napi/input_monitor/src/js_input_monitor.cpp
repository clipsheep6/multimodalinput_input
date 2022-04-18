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

#include "js_input_monitor.h"

#include <cinttypes>

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_manager.h"
#include "js_input_monitor_manager.h"
#include "js_input_monitor_util.h"

namespace OHOS {
namespace MMI {
#define InputMgr InputManager::GetInstance()
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsInputMonitor" };
constexpr int32_t NAPI_ERR = 3;
} // namespace

bool InputMonitor::Start()
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (monitorId_ < 0) {
        monitorId_ = InputMgr->AddMonitor(shared_from_this());
        return monitorId_ >= 0;
    }
    return true;
}

void InputMonitor::Stop()
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (monitorId_ < 0) {
        MMI_HILOGE("Invalid values");
        return;
    }
    InputMgr->RemoveMonitor(monitorId_);
    monitorId_ = -1;
    return;
}

void InputMonitor::SetCallback(std::function<void(std::shared_ptr<PointerEvent>)> callback)
{
    std::lock_guard<std::mutex> guard(mutex_);
    callback_ = callback;
}

void InputMonitor::OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const
{
    CALL_LOG_ENTER;
    CHKPV(pointerEvent);
    if (JsInputMonMgr.GetMonitor(id_) == nullptr) {
        MMI_HILOGE("failed to process pointer event, id:%{public}d", id_);
        return;
    }
    std::function<void(std::shared_ptr<PointerEvent>)> callback;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
            if (JsInputMonMgr.GetMonitor(id_)->GetTypeName() != "touch") {
                return;
            }
            if (pointerEvent->GetPointersIdList().size() == 1) {
                if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_DOWN) {
                    consumed_ = false;
                }
            }
        }
        if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
            if (JsInputMonMgr.GetMonitor(id_)->GetTypeName() != "mouse") {
                return;
            }
            if (pointerEvent->GetPointersIdList().size() == 1) {
                if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_DOWN) {
                    consumed_ = false;
                }
            }
        }
        callback = callback_;
    }
    CHKPV(callback);
    callback(pointerEvent);
}

void InputMonitor::SetId(int32_t id) {
    id_ = id;
}

void InputMonitor::OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const {}

void InputMonitor::OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const {}

void InputMonitor::MarkConsumed(int32_t eventId)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (consumed_) {
        MMI_HILOGD("consumed_ is true");
        return;
    }
    if (monitorId_ < 0) {
        MMI_HILOGE("Invalid values");
        return;
    }
    InputMgr->MarkConsumed(monitorId_, eventId);
    consumed_ = true;
}

JsInputMonitor::JsInputMonitor(napi_env jsEnv, const std::string &typeName, napi_value callback, int32_t id)
    : monitor_(std::make_shared<InputMonitor>()),
      jsEnv_(jsEnv),
      typeName_(typeName),
      monitorId_(id)
{
    SetCallback(callback);
    if (monitor_ == nullptr) {
        MMI_HILOGE("monitor is null");
        return;
    }
    monitor_->SetCallback([jsId=id](std::shared_ptr<PointerEvent> pointerEvent) {
        auto& jsMonitor {JsInputMonMgr.GetMonitor(jsId)};
        CHKPV(jsMonitor);
        jsMonitor->OnPointerEvent(pointerEvent);
    });
    monitor_->SetId(monitorId_);
}

void JsInputMonitor::SetCallback(napi_value callback)
{
    if (receiver_ == nullptr && jsEnv_ != nullptr) {
        uint32_t refCount = 1;
        auto status = napi_create_reference(jsEnv_, callback, refCount, &receiver_);
        if (status != napi_ok) {
            MMI_HILOGE("napi_create_reference is failed");
            return;
        }
    }
}

void JsInputMonitor::MarkConsumed(int32_t eventId)
{
    CHKPV(monitor_);
    monitor_->MarkConsumed(eventId);
}

int32_t JsInputMonitor::IsMatch(napi_env jsEnv, napi_value callback)
{
    CHKPR(callback, ERROR_NULL_POINTER);
    if (jsEnv_ == jsEnv) {
        napi_value handlerTemp = nullptr;
        auto status = napi_get_reference_value(jsEnv_, receiver_, &handlerTemp);
        if (status != napi_ok) {
            MMI_HILOGE("napi_get_reference_value is failed");
            return NAPI_ERR;
        }
        bool isEquals = false;
        status = napi_strict_equals(jsEnv_, handlerTemp, callback, &isEquals);
        if (status != napi_ok) {
            MMI_HILOGE("napi_strict_equals is failed");
            return NAPI_ERR;
        }
        if (isEquals) {
            MMI_HILOGI("js callback match success");
            return RET_OK;
        }
        MMI_HILOGI("js callback match failed");
        return RET_ERR;
    }
    MMI_HILOGI("js callback match failed");
    return RET_ERR;
}

int32_t JsInputMonitor::IsMatch(napi_env jsEnv)
{
    if (jsEnv_ == jsEnv) {
        MMI_HILOGI("env match success");
        return RET_OK;
    }
    MMI_HILOGI("env match failed");
    return RET_ERR;
}

std::string JsInputMonitor::GetAction(int32_t action) const
{
    switch (action) {
        case PointerEvent::POINTER_ACTION_CANCEL:
            return "cancel";
        case PointerEvent::POINTER_ACTION_DOWN:
            return "down";
        case PointerEvent::POINTER_ACTION_MOVE:
            return "move";
        case PointerEvent::POINTER_ACTION_UP:
            return "up";
        default:
            return "";
    }
}

int32_t JsInputMonitor::GetJsPointerItem(const PointerEvent::PointerItem &item, napi_value value) const
{
    if (SetNameProperty(jsEnv_, value, "globalX", item.GetGlobalX()) != napi_ok) {
        MMI_HILOGE("Set globalX property failed");
        return RET_ERR;
    }
    if (SetNameProperty(jsEnv_, value, "globalY", item.GetGlobalY()) != napi_ok) {
        MMI_HILOGE("Set globalY property failed");
        return RET_ERR;
    }
    if (SetNameProperty(jsEnv_, value, "localX", 0) != napi_ok) {
        MMI_HILOGE("Set localX property failed");
        return RET_ERR;
    }
    if (SetNameProperty(jsEnv_, value, "localY", 0) != napi_ok) {
        MMI_HILOGE("Set localY property failed");
        return RET_ERR;
    }
    int32_t touchArea = (item.GetWidth() + item.GetHeight()) / 2;
    if (SetNameProperty(jsEnv_, value, "size", touchArea) != napi_ok) {
        MMI_HILOGE("Set size property failed");
        return RET_ERR;
    }
    if (SetNameProperty(jsEnv_, value, "force", item.GetPressure()) != napi_ok) {
        MMI_HILOGE("Set force property failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t JsInputMonitor::TransformPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent, napi_value result)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (SetNameProperty(jsEnv_, result, "type", GetAction(pointerEvent->GetPointerAction())) != napi_ok) {
        MMI_HILOGE("Set type property failed");
        return RET_ERR;
    }
    napi_value pointers = nullptr;
    auto status = napi_create_array(jsEnv_, &pointers);
    if (status != napi_ok) {
        MMI_HILOGE("napi_create_array is failed");
        return RET_ERR;
    }
    std::vector<PointerEvent::PointerItem> pointerItems;
    for (const auto &item : pointerEvent->GetPointersIdList()) {
        PointerEvent::PointerItem pointerItem;
        if (!pointerEvent->GetPointerItem(item, pointerItem)) {
            MMI_HILOGE("Get pointer item failed");
            return RET_ERR;
        }
        pointerItems.push_back(pointerItem);
    }
    uint32_t index = 0;
    napi_value currentPointer = nullptr;
    int32_t currentPointerId = pointerEvent->GetPointerId();
    for (const auto &it : pointerItems) {
        napi_value element = nullptr;
        status = napi_create_object(jsEnv_, &element);
        if (status != napi_ok) {
            MMI_HILOGE("napi_create_object is failed");
            return RET_ERR;
        }
        if (currentPointerId == it.GetPointerId()) {
            status = napi_create_object(jsEnv_, &currentPointer);
            if (status != napi_ok) {
                MMI_HILOGE("napi_create_object is failed");
                return RET_ERR;
            }
            if (GetJsPointerItem(it, currentPointer) != RET_OK) {
                MMI_HILOGE("transform pointerItem failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "timestamp", pointerEvent->GetActionTime()) != napi_ok) {
                MMI_HILOGE("Set timestamp property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "deviceId", it.GetDeviceId()) != napi_ok) {
                MMI_HILOGE("Set deviceId property failed");
                return RET_ERR;
            }
        }
        if (GetJsPointerItem(it, element) != RET_OK) {
            MMI_HILOGE("transform pointerItem failed");
            return RET_ERR;
        }
        status = napi_set_element(jsEnv_, pointers, index, element);
        if (status != napi_ok) {
            MMI_HILOGE("napi_set_element is failed");
            return RET_ERR;
        }
        ++index;
    }
    if (SetNameProperty(jsEnv_, result, "touches", pointers) != napi_ok) {
            MMI_HILOGE("Set touches property failed");
            return RET_ERR;
    }
    if (SetNameProperty(jsEnv_, result, "changedTouches", currentPointer) != napi_ok) {
            MMI_HILOGE("Set changedTouches property failed");
            return RET_ERR;
    }
    return RET_OK;
}

int32_t JsInputMonitor::GetMousePointerItem(const std::shared_ptr<PointerEvent> pointerEvent, napi_value result)
{
    int32_t currentPointerId = pointerEvent->GetPointerId();
    std::vector<int32_t> pointerIds { pointerEvent->GetPointersIdList() };
    for (const auto& pointerId : pointerIds) {
        if (pointerId == currentPointerId) {
            PointerEvent::PointerItem item;
            if (!pointerEvent->GetPointerItem(pointerId, item)) {
                MMI_HILOGE("Invalid pointer: %{public}d.", pointerId);
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "timestamp", pointerEvent->GetActionTime()) != napi_ok) {
                MMI_HILOGE("Set timestamp property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "deviceId", item.GetDeviceId()) != napi_ok) {
                MMI_HILOGE("Set deviceId property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "globalX", item.GetGlobalX()) != napi_ok) {
                MMI_HILOGE("Set globalX property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "globalY", item.GetGlobalY()) != napi_ok) {
                MMI_HILOGE("Set globalY property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "localX", item.GetLocalX()) != napi_ok) {
                MMI_HILOGE("Set localX property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "localY", item.GetLocalY()) != napi_ok) {
                MMI_HILOGE("Set localY property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "displayId", pointerEvent->GetTargetDisplayId()) != napi_ok) {
                MMI_HILOGE("Set displayId property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "targetWindowId", pointerEvent->GetTargetWindowId()) != napi_ok) {
                MMI_HILOGE("Set targetWindowId property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "agentWindowId", pointerEvent->GetAgentWindowId()) != napi_ok) {
                MMI_HILOGE("Set agentWindowId property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "axisVerticalValue",
                pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL)) != napi_ok) {
                MMI_HILOGE("Set axisVerticalValue property failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, result, "axisHorizontalValue",
                pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL)) != napi_ok) {
                MMI_HILOGE("Set axisVerticalValue property failed");
                return RET_ERR;
            }
        }
    }
    return RET_OK;
}

int32_t JsInputMonitor::TransformMousePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent, napi_value result)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (SetNameProperty(jsEnv_, result, "type", GetAction(pointerEvent->GetPointerAction())) != napi_ok) {
        MMI_HILOGE("Set actionType property failed");
        return RET_ERR;
    }
    if (GetMousePointerItem(pointerEvent, result) != RET_OK) {
        MMI_HILOGE("GetMousePointerItem failed");
        return RET_ERR;
    }
    napi_value buttons = nullptr;
    napi_status status = napi_create_array(jsEnv_, &buttons);
    if (status != napi_ok || buttons == nullptr) {
        MMI_HILOGE("napi_create_array is failed");
        return RET_ERR;
    }
    uint32_t index = 0;
    std::set<int32_t> pressedButtons { pointerEvent->GetPressedButtons() };
    if (!pressedButtons.empty()) {
        for (const auto &it : pressedButtons) {
            napi_value element = nullptr;
            status = napi_create_object(jsEnv_, &element);
            if (status != napi_ok) {
                MMI_HILOGE("napi_create_object is failed");
                return RET_ERR;
            }
            if (SetNameProperty(jsEnv_, element, "pressedButton", it) != napi_ok) {
                MMI_HILOGE("Set pressedButton property failed");
                return RET_ERR;
            }
            status = napi_set_element(jsEnv_, buttons, index, element);
            if (status != napi_ok) {
                MMI_HILOGE("napi_set_element is failed");
                return RET_ERR;
            }
            ++index;
        }
    }
    if (SetNameProperty(jsEnv_, result, "pressedButtons", buttons) != napi_ok) {
        MMI_HILOGE("Set pressedButtons property failed");
        return RET_ERR;
    }
    return RET_OK;
}

bool JsInputMonitor::Start() {
    CALL_LOG_ENTER;
    CHKPF(monitor_);
    if (isMonitoring_) {
        MMI_HILOGW("js is monitoring");
        return true;
    }
    if (monitor_->Start()) {
        isMonitoring_ = true;
        return true;
    }
    return false;
}

JsInputMonitor::~JsInputMonitor()
{
    CALL_LOG_ENTER;
    if (isMonitoring_) {
        isMonitoring_ = false;
        if (monitor_ != nullptr) {
            monitor_->Stop();
        }
    }
    uint32_t refCount = 0;
    auto status = napi_reference_unref(jsEnv_, receiver_, &refCount);
    if (status != napi_ok) {
        MMI_HILOGE("napi_reference_unref is failed");
        return;
    }
}

void JsInputMonitor::Stop() {
    CALL_LOG_ENTER;
    CHKPV(monitor_);
    if (isMonitoring_) {
        isMonitoring_ = false;
        if (monitor_ != nullptr) {
            monitor_->Stop();
        }
    }
}

int32_t JsInputMonitor::GetId() const
{
    return monitorId_;
}

std::string JsInputMonitor::GetTypeName() const
{
    return typeName_;
}

void JsInputMonitor::OnPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_LOG_ENTER;
    if (!isMonitoring_) {
        MMI_HILOGE("js monitor stop");
        return;
    }
    CHKPV(monitor_);
    CHKPV(pointerEvent);
    int32_t num = 0;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        evQueue_.push(pointerEvent);
        num = jsTaskNum_;
    }
    if (num < 1) {
        int32_t *id = &monitorId_;
        uv_work_t *work = new (std::nothrow) uv_work_t;
        CHKPV(work);
        work->data = id;
        uv_loop_s *loop = nullptr;
        auto status = napi_get_uv_event_loop(jsEnv_, &loop);
        if (status != napi_ok) {
            MMI_HILOGE("napi_get_uv_event_loop is failed");
            return;
        }
        uv_queue_work(loop, work, [](uv_work_t *work){}, &JsInputMonitor::JsCallback);
        std::lock_guard<std::mutex> guard(mutex_);
        ++jsTaskNum_;
    }
}

void JsInputMonitor::JsCallback(uv_work_t *work, int32_t status)
{
    CALL_LOG_ENTER;
    CHKPV(work);
    int32_t *id = static_cast<int32_t *>(work->data);
    delete work;
    work = nullptr;
    auto& jsMonitor {JsInputMonMgr.GetMonitor(*id)};
    CHKPV(jsMonitor);
    jsMonitor->OnPointerEventInJsThread(jsMonitor->GetTypeName());
    id = nullptr;
}

void JsInputMonitor::OnPointerEventInJsThread(const std::string &typeName)
{
    CALL_LOG_ENTER;
    if (!isMonitoring_) {
        MMI_HILOGE("js monitor stop");
        return;
    }
    CHKPV(jsEnv_);
    CHKPV(receiver_);
    std::lock_guard<std::mutex> guard(mutex_);
    napi_handle_scope scope = nullptr;
    while (!evQueue_.empty()) {
        if (!isMonitoring_) {
            MMI_HILOGE("js monitor stop handle callback");
            break;
        }
        auto pointerEvent = evQueue_.front();
        CHKPC(pointerEvent);
        evQueue_.pop();
        auto status = napi_open_handle_scope(jsEnv_, &scope);
        if (status != napi_ok) {
            break;
        }
        napi_value napiPointer = nullptr;
        status = napi_create_object(jsEnv_, &napiPointer);
        if (status != napi_ok) {
            napi_close_handle_scope(jsEnv_, scope);
            break;
        }
        auto ret = RET_ERR;
        if (typeName == "touch") {
            ret = TransformPointerEvent(pointerEvent, napiPointer);
        } else {
            ret = TransformMousePointerEvent(pointerEvent, napiPointer);
        }
        if (ret != RET_OK) {
            napi_close_handle_scope(jsEnv_, scope);
            break;
        }
        if (napiPointer == nullptr) {
            napi_close_handle_scope(jsEnv_, scope);
            break;
        }
        napi_value callback = nullptr;
        status = napi_get_reference_value(jsEnv_, receiver_, &callback);
        if (status != napi_ok) {
            napi_close_handle_scope(jsEnv_, scope);
            break;
        }
        napi_value result = nullptr;
        status = napi_call_function(jsEnv_, nullptr, callback, 1, &napiPointer, &result);
        if (status != napi_ok) {
            napi_close_handle_scope(jsEnv_, scope);
            break;
        }
        bool retValue = false;
        status = napi_get_value_bool(jsEnv_, result, &retValue);
        if (status != napi_ok) {
            napi_close_handle_scope(jsEnv_, scope);
            --jsTaskNum_;
            return;
        }
        if (retValue) {
            auto eventId = pointerEvent->GetId();
            MarkConsumed(eventId);
        }
        napi_close_handle_scope(jsEnv_, scope);
    }
    --jsTaskNum_;
}
} // namespace MMI
} // namespace OHOS
