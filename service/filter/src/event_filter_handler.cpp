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

#include "event_filter_handler.h"

#include "error_multimodal.h"
#include "input_device_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventFilterHandler" };
const double EPS = 1e-8;
const double MOVE_EVENT_FILTER_THRESHOLD = 5.0;
const int32_t SQUARE = 2;
} // namespace

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void EventFilterHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    if (HandleKeyEventFilter(keyEvent)) {
        MMI_HILOGD("Key event is filtered");
        return;
    }
    CHKPV(nextHandler_);
    nextHandler_->HandleKeyEvent(keyEvent);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#ifdef OHOS_BUILD_ENABLE_POINTER
void EventFilterHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    if (HandlePointerEventFilter(pointerEvent)) {
        MMI_HILOGD("Pointer event is filtered");
        return;
    }
    CHKPV(nextHandler_);
    nextHandler_->HandlePointerEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_POINTER

#ifdef OHOS_BUILD_ENABLE_TOUCH
void EventFilterHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    if (HandleTouchEventWithFlag(pointerEvent)) {
        MMI_HILOGI("Touch event is filtered with flag");
        return;
    }
    if (HandlePointerEventFilter(pointerEvent)) {
        MMI_HILOGD("Touch event is filtered");
        return;
    }
    CHKPV(nextHandler_);
    nextHandler_->HandleTouchEvent(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

int32_t EventFilterHandler::AddInputEventFilter(sptr<IEventFilter> filter,
    int32_t filterId, int32_t priority, uint32_t deviceTags, int32_t clientPid)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(lockFilter_);
    CHKPR(filter, ERROR_NULL_POINTER);
    MMI_HILOGI("Add filter,filterId:%{public}d,priority:%{public}d,clientPid:%{public}d,filters_ size:%{public}zu",
        filterId, priority, clientPid, filters_.size());
    
    std::weak_ptr<EventFilterHandler> weakPtr = shared_from_this();
    auto deathCallback = [weakPtr, filterId, clientPid](const wptr<IRemoteObject> &object) {
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr != nullptr) {
            auto ret = sharedPtr->RemoveInputEventFilter(filterId, clientPid);
            if (ret != RET_OK) {
                MMI_HILOGW("Remove filter on dead return:%{public}d, filterId:%{public}d,clientPid:%{public}d",
                    ret, filterId, clientPid);
            } else {
                MMI_HILOGW("Remove filter on dead success, filterId:%{public}d,clientPid:%{public}d",
                    filterId, clientPid);
            }
        }
    };
    sptr<IRemoteObject::DeathRecipient> deathRecipient = new (std::nothrow) EventFilterDeathRecipient(deathCallback);
    CHKPR(deathRecipient, RET_ERR);
    filter->AsObject()->AddDeathRecipient(deathRecipient);
    
    FilterInfo info { .filter = filter, .deathRecipient = deathRecipient, .filterId = filterId,
        .priority = priority, .deviceTags = deviceTags, .clientPid = clientPid };
    auto it = filters_.cbegin();
    for (; it != filters_.cend(); ++it) {
        if (info.priority < it->priority) {
            break;
        }
    }
    auto it2 = filters_.emplace(it, std::move(info));
    if (it2 == filters_.end()) {
        MMI_HILOGE("Fail to add filter");
        return ERROR_FILTER_ADD_FAIL;
    }
    return RET_OK;
}

int32_t EventFilterHandler::RemoveInputEventFilter(int32_t filterId, int32_t clientPid)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(lockFilter_);
    if (filters_.empty()) {
        MMI_HILOGD("Filter is empty");
        return RET_OK;
    }
    for (auto it = filters_.begin(); it != filters_.end();) {
        if (filterId == -1) {
            if (it->clientPid == clientPid) {
                auto id = it->filterId;
                filters_.erase(it++);
                MMI_HILOGI("Filter remove success, filterId:%{public}d,clientPid:%{public}d", id, clientPid);
                continue;
            }
            ++it;
            continue;
        }
        if (it->IsSameClient(filterId, clientPid)) {
            filters_.erase(it++);
            MMI_HILOGI("Filter remove success, filterId:%{public}d,clientPid:%{public}d", filterId, clientPid);
            return RET_OK;
        }
        ++it;
    }
    if (filterId == -1) {
        return RET_OK;
    }
    MMI_HILOGI("Filter not found, filterId:%{public}d,clientPid:%{public}d", filterId, clientPid);
    return RET_OK;
}

void EventFilterHandler::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lockFilter_);
    dprintf(fd, "Filter information:\n");
    dprintf(fd, "Filters: count=%d\n", filters_.size());
    for (const auto &item : filters_) {
        dprintf(fd, "priority:%d | filterId:%d | Pid:%d\n", item.priority, item.filterId, item.clientPid);
    }
}

bool EventFilterHandler::HandleKeyEventFilter(std::shared_ptr<KeyEvent> event)
{
    CALL_DEBUG_ENTER;
    CHKPF(event);
    std::lock_guard<std::mutex> guard(lockFilter_);
    if (filters_.empty()) {
        return false;
    }
    std::vector<KeyEvent::KeyItem> keyItems = event->GetKeyItems();
    if (keyItems.empty()) {
        MMI_HILOGE("keyItems is empty");
        return false;
    }
    std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetInputDevice(keyItems.front().GetDeviceId());
    CHKPF(inputDevice);
    for (auto &i: filters_) {
        if (!inputDevice->HasCapability(i.deviceTags)) {
            continue;
        }
        if (i.filter->HandleKeyEvent(event)) {
            MMI_HILOGD("Call HandleKeyEventFilter return true");
            return true;
        }
    }
    return false;
}

bool EventFilterHandler::HandlePointerEventFilter(std::shared_ptr<PointerEvent> event)
{
    CALL_DEBUG_ENTER;
    CHKPF(event);
    std::lock_guard<std::mutex> guard(lockFilter_);
    if (filters_.empty()) {
        return false;
    }
    PointerEvent::PointerItem pointerItem;
    int32_t pointerId = event->GetPointerId();
    if (!event->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("GetPointerItem:%{public}d fail", pointerId);
        return false;
    }
    std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetInputDevice(pointerItem.GetDeviceId());
    CHKPF(inputDevice);
    for (auto &i: filters_) {
        if (!inputDevice->HasCapability(i.deviceTags)) {
            continue;
        }
        if (i.filter->HandlePointerEvent(event)) {
            MMI_HILOGD("Call HandlePointerEvent return true");
            return true;
        }
    }
    return false;
}

int32_t EventFilterHandler::SetMoveEventFilters(bool flag)
{
    std::lock_guard<std::mutex> guard(lockFlag_);
    moveEventFilterFlag_ = flag;
    return RET_OK;
}

bool EventFilterHandler::HandleTouchEventWithFlag(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(lockFlag_);
    if (!moveEventFilterFlag_) {
        MMI_HILOGI("touch move event flag is false");
        return false;
    }
    CHKPF(pointerEvent);
    if (pointerEvent->GetSourceType() != PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        MMI_HILOGI("touch event is not from touch screen");
        return false;
    }

    auto action = pointerEvent->GetPointerAction();
    if (action == PointerEvent::POINTER_ACTION_DOWN) {
        for (const auto &pointerId : pointerEvent->GetPointerIds()) {
            PointerEvent::PointerItem item;
            if (pointerEvent->GetPointerItem(pointerId, item)) {
                lastTouchDownItems_.emplace_back(item);
            }
        }
    } else if (action == PointerEvent::POINTER_ACTION_MOVE) {
        double offset = CalcTouchOffset(pointerEvent);
        MMI_HILOGI("touch move event, offset:%{public}f", offset);
        return (offset + EPS < MOVE_EVENT_FILTER_THRESHOLD);
    } else if (action == PointerEvent::POINTER_ACTION_UP) {
        lastTouchDownItems_.clear();
    }
    return false;
}

double EventFilterHandler::CalcTouchOffset(std::shared_ptr<PointerEvent> touchMoveEvent)
{
    CALL_INFO_TRACE;
    CHKPR(touchMoveEvent, ERROR_NULL_POINTER);
    if (touchMoveEvent->GetPointerIds().empty() || lastTouchDownItems_.empty()) {
        MMI_HILOGE("touchMoveEvent or lastTouchDownItems_ is invalid.");
        return 0.f;
    }
    PointerEvent::PointerItem itemMove;
    int32_t pointerIdMove = touchMoveEvent->GetPointerIds().at(0);
    if (!touchMoveEvent->GetPointerItem(pointerIdMove, itemMove)) {
        MMI_HILOGE("Invalid touch move pointer: %{public}d.", pointerIdMove);
        return 0.f;
    }
    MMI_HILOGI("Poniter itemMove, pointerId:%{public}d, DisplayX:%{public}d, DisplayY:%{public}d",
        pointerIdMove, itemMove.GetDisplayX(), itemMove.GetDisplayY());
    PointerEvent::PointerItem itemDown = lastTouchDownItems_.at(0);
    MMI_HILOGI("Poniter itemDown, pointerId:%{public}d, DisplayX:%{public}d, DisplayY:%{public}d",
        itemDown.GetPointerId(), itemDown.GetDisplayX(), itemDown.GetDisplayY());

    double offset = sqrt(pow(itemMove.GetDisplayX() - itemDown.GetDisplayX(), SQUARE) +
        pow(itemMove.GetDisplayY() - itemDown.GetDisplayY(), SQUARE));
    return offset;
}
} // namespace MMI
} // namespace OHOS
