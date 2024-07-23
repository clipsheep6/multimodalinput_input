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

#include "delegate_interface.h"

#include "input_event_handler.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "DelegateInterface"

namespace OHOS {
namespace MMI {
void DelegateInterface::OnInputEvent(
    InputHandlerType type, std::shared_ptr<PointerEvent> event) const
{
    CHKPV(event);
    OnInputEventHandler(type, event);
}

void DelegateInterface::OnInputEventHandler(InputHandlerType type, std::shared_ptr<PointerEvent> event) const
{
    for (const auto &handler : handlers) {
        auto summary = handler.second;
        if (handler.first != type) {
            continue;
        }
        if (type == InputHandlerType::MONITOR &&
            (summary.eventType & HANDLE_EVENT_TYPE_POINTER) != HANDLE_EVENT_TYPE_POINTER) {
            continue;
        }
        uint32_t deviceTags = 0;
        if (type == InputHandlerType::INTERCEPTOR &&
            ((deviceTags & summary.deviceTags) == summary.deviceTags) &&
            !EventInterceptorHandler::CheckInputDeviceSource(event, summary.deviceTags)) {
            continue;
        }
        CHKPV(summary.cb);
        if (summary.mode == HandlerMode::SYNC) {
            summary.cb(event);
        } else {
            CHKPV(delegateTasks_);
            if (OnPostSyncTask(std::bind(summary.cb, event)) != RET_OK) {
                MMI_HILOGE("Failed to execute the task(%{public}s)", summary.handlerName.c_str());
            }
        }
    }
}

int32_t DelegateInterface::OnPostSyncTask(DTaskCallback cb) const
{
    int32_t ret = delegateTasks_(cb);
    if (ret != RET_OK) {
        MMI_HILOGE("Failed to execute the task, ret: %{public}d", ret);
    }
    return ret;
}

int32_t DelegateInterface::AddHandler(InputHandlerType type, HandlerSummary summary)
{
    CHKPR(summary.cb, ERROR_NULL_POINTER);
    for (const auto &handler : handlers) {
        if (handler.second.handlerName == summary.handlerName) {
            MMI_HILOGW("The current handler(%{public}s) already exists", summary.handlerName.c_str());
            return RET_OK;
        }
    }
    const HandleEventType currentType = GetEventType(type);
    uint32_t currentTags = GetDeviceTags(type);
    handlers.emplace(type, summary);

    const HandleEventType newType = GetEventType(type);
    if (currentType != newType || ((currentTags & summary.deviceTags) != summary.deviceTags)) {
        uint32_t allDeviceTags = GetDeviceTags(type);
        if (type == InputHandlerType::INTERCEPTOR) {
            auto interceptorHandler = InputHandler->GetInterceptorHandler();
            CHKPR(interceptorHandler, ERROR_NULL_POINTER);
            return interceptorHandler->AddInputHandler(type,
                newType, summary.priority, allDeviceTags, nullptr);
        }
        if (type == InputHandlerType::MONITOR) {
            auto monitorHandler = InputHandler->GetMonitorHandler();
            CHKPR(monitorHandler, ERROR_NULL_POINTER);
            return monitorHandler->AddInputHandler(type,
                newType, shared_from_this());
        }
    }
    MMI_HILOGI("Service Add Monitor Success");
    return RET_OK;
}

HandleEventType DelegateInterface::GetEventType(InputHandlerType type) const
{
    uint32_t eventType { HANDLE_EVENT_TYPE_NONE };
    if (handlers.empty()) {
        MMI_HILOGD("handlers is empty");
        return HANDLE_EVENT_TYPE_NONE;
    }
    for (const auto &handler : handlers) {
        if (handler.first == type) {
            eventType |= handler.second.eventType;
        }
    }
    return eventType;
}

uint32_t DelegateInterface::GetDeviceTags(InputHandlerType type) const
{
    uint32_t deviceTags = 0;
    if (type == InputHandlerType::MONITOR) {
        return deviceTags;
    }
    if (handlers.empty()) {
        MMI_HILOGD("handlers is empty");
        return deviceTags;
    }
    for (const auto &handler : handlers) {
        if (handler.first == type) {
            deviceTags |= handler.second.deviceTags;
        }
    }
    return deviceTags;
}

void DelegateInterface::RemoveHandler(InputHandlerType type, std::string name)
{
    const HandleEventType currentType = GetEventType(type);
    uint32_t currentTags = GetDeviceTags(type);
    uint32_t deviceTags = 0;
    HandlerSummary summary;
    auto it = handlers.cbegin();
    for(; it != handlers.cend(); ++it) {
        summary = it->second;
        if (summary.handlerName != name) {
            continue;
        }
        handlers.erase(it);
        if (type == InputHandlerType::INTERCEPTOR) {
            deviceTags = summary.deviceTags;
        }
        break;
    }
    const HandleEventType newType = GetEventType(type);
    const int32_t newLevel = summary.priority;
    const uint64_t newTags = GetDeviceTags(type);
    if (currentType != newType || ((currentTags & deviceTags) != 0)) {
        if (type == InputHandlerType::INTERCEPTOR) {
            auto interceptorHandler = InputHandler->GetInterceptorHandler();
            CHKPV(interceptorHandler);
            interceptorHandler->RemoveInputHandler(type,
                newType, newLevel, newTags, nullptr);
        }
        if (type == InputHandlerType::MONITOR) {
            auto monitorHandler = InputHandler->GetMonitorHandler();
            CHKPV(monitorHandler);
            monitorHandler->RemoveInputHandler(type,
                newType, shared_from_this());
        }
    }
    MMI_HILOGI("Remove Handler:%{public}d:%{public}s, %{public}d:%{public}d", type,
               name.c_str(), currentType, currentTags);
}
} // namespace MMI
} // namespace OHOS