/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "device_event_monitor.h"

#include "mmi_log.h"
#include "want.h"

namespace OHOS {
namespace MMI {
namespace {

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DeviceEventMonitor" };
std::mutex stateMutex_;
} // namespace

DeviceEventMonitor::DeviceEventMonitor() {}
DeviceEventMonitor::~DeviceEventMonitor() {}

class DeviceChangedReceiver : public EventFwk::CommonEventSubscriber {
public:
    explicit DeviceChangedReceiver(const OHOS::EventFwk::CommonEventSubscribeInfo& subscribeInfo)
        : OHOS::EventFwk::CommonEventSubscriber(subscribeInfo)
    {
        MMI_HILOGD("DeviceEventMonitor register");
    }

    virtual ~DeviceChangedReceiver() = default;
    __attribute__((no_sanitize("cfi")))

    void OnReceiveEvent(const EventFwk::CommonEventData &eventData)
    {
        CALL_DEBUG_ENTER;
        std::string action = eventData.GetWant().GetAction();
        if (action.empty()) {
            MMI_HILOGE("action is empty");
            return;
        }
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_CALL_STATE_CHANGED) {
            int32_t callState = 0;
            DEVICE_MONITOR->SetCallState(eventData, callState);
            MMI_HILOGI("state: %{public}d", callState);
        } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
            MMI_HILOGD("display screen on");
            if (FINGERSENSE_WRAPPER->enableFingersense_ != nullptr) {
                MMI_HILOGD("start enable fingersense");
                FINGERSENSE_WRAPPER->enableFingersense_();
            }
            DEVICE_MONITOR->UpdateShieldStatusOnScreenOn();
        } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
            MMI_HILOGD("display screen off");
            if (FINGERSENSE_WRAPPER->disableFingerSense_ != nullptr) {
                FINGERSENSE_WRAPPER->disableFingerSense_();
            }
            DEVICE_MONITOR->UpdateShieldStatusOnScreenOff();
        } else {
            MMI_HILOGW("Screen changed receiver event: unknown");
            return;
        }
    }
};

void DeviceEventMonitor::InitCommonEventSubscriber()
{
    CALL_DEBUG_ENTER;
    if (hasInit_) {
        MMI_HILOGE("current common event has subscribered");
        return;
    }
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_CALL_STATE_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventSubscribeInfo commonEventSubscribeInfo(matchingSkills);
    hasInit_ = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(
        std::make_shared<DeviceChangedReceiver>(commonEventSubscribeInfo));
}

bool DeviceEventMonitor::IsCommonEventSubscriberInit()
{
    return hasInit_;
}

void DeviceEventMonitor::SetCallState(const EventFwk::CommonEventData &eventData, int32_t callState)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> lock(stateMutex_);
    callState = eventData.GetWant().GetIntParam("state", -1);
    MMI_HILOGI("state %{public}d", callState);
    callState_ = callState;
}

int32_t DeviceEventMonitor::GetCallState()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> lock(stateMutex_);
    return callState_;
}

void DeviceEventMonitor::UpdateShieldStatusOnScreenOn()
{
    CALL_DEBUG_ENTER;
    if (shieldModeBeforeSreenOff_ != SHIELD_MODE::UNSET_MODE) {
        KeyEventHdr->SetCurrentShieldMode(shieldModeBeforeSreenOff_);
    } else {
        MMI_HILOGD("shield mode before screen off: %{public}d", shieldModeBeforeSreenOff_);
    }
}

void DeviceEventMonitor::UpdateShieldStatusOnScreenOff()
{
    CALL_DEBUG_ENTER;
    shieldModeBeforeSreenOff_ = KeyEventHdr->GetCurrentShieldMode();
    if (shieldModeBeforeSreenOff_ != SHIELD_MODE::UNSET_MODE) {
        KeyEventHdr->SetCurrentShieldMode(SHIELD_MODE::UNSET_MODE);
    } else {
        MMI_HILOGD("shield mode before screen off: %{public}d", shieldModeBeforeSreenOff_);
    }
}

} // namespace MMI
} // namespace OHOS