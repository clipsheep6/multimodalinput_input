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

#include <chrono>
#include <thread>
#include <unistd.h>

#include "define_multimodal.h"
#include "bluetooth_log.h"
#include "mock_distributed_device_profile_client.h"
#include "mock_idistributed_device_profile.h"

namespace OHOS {
namespace DeviceProfile {
using namespace std::chrono_literals;
static bool g_cooperateState = true;
const std::string CHARACTERISTICS_NAME = "CurrentState";
constexpr const int32_t DP_GET_SERVICE_FAILED = 98566147;
constexpr const int32_t DP_GET_SERVICE_SUCCESS = 98566148;
constexpr const int32_t DP_INVALID_PARAMS = 98566144;

void MockDistributedDeviceProfileClient::SetDPState(bool state)
{
    g_cooperateState = state;
}

IMPLEMENT_SINGLE_INSTANCE(DistributedDeviceProfileClient);

int32_t DistributedDeviceProfileClient::PutDeviceProfile(const ServiceCharacteristicProfile& profile)
{
    if (DistributedDeviceProfileClient::CheckProfileInvalidity(profile)) {
        return DP_INVALID_PARAMS;
    }
    HILOGD("Getting device profile service succeeded");
    return DP_GET_SERVICE_SUCCESS;
}

int32_t DistributedDeviceProfileClient::GetDeviceProfile(const std::string& udid, const std::string& serviceId,
    ServiceCharacteristicProfile& profile)
{
    if (udid.empty() || serviceId.empty()) {
        HILOGD("Failed to get device profile");
        return RET_ERR;
    }
    nlohmann::json data;
    data[CHARACTERISTICS_NAME] = g_cooperateState;
    profile.SetCharacteristicProfileJson(data.dump());
    HILOGD("Get device profile successfully");
    return RET_OK;
}

int32_t DistributedDeviceProfileClient::DeleteDeviceProfile(const std::string& serviceId)
{
    if (serviceId.empty()) {
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DistributedDeviceProfileClient::SubscribeProfileEvent(const SubscribeInfo& subscribeInfo,
    const std::shared_ptr<IProfileEventCallback>& eventCb)
{
    std::list<SubscribeInfo> subscribeInfos;
    subscribeInfos.emplace_back(subscribeInfo);
    std::list<ProfileEvent> failedEvents;
    return SubscribeProfileEvents(subscribeInfos, eventCb, failedEvents);
}

int32_t DistributedDeviceProfileClient::SubscribeProfileEvents(const std::list<SubscribeInfo>& subscribeInfos,
    const std::shared_ptr<IProfileEventCallback>& eventCb,
    std::list<ProfileEvent>& failedEvents)
{
    if (subscribeInfos.empty() || eventCb == nullptr) {
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DistributedDeviceProfileClient::UnsubscribeProfileEvent(ProfileEvent profileEvent,
    const std::shared_ptr<IProfileEventCallback>& eventCb)
{
    std::list<ProfileEvent> profileEvents;
    profileEvents.emplace_back(profileEvent);
    std::list<ProfileEvent> failedEvents;
    return UnsubscribeProfileEvents(profileEvents, eventCb, failedEvents);
}

int32_t DistributedDeviceProfileClient::UnsubscribeProfileEvents(const std::list<ProfileEvent>& profileEvents,
    const std::shared_ptr<IProfileEventCallback>& eventCb,
    std::list<ProfileEvent>& failedEvents)
{
    if (profileEvents.empty() || eventCb == nullptr) {
        return DP_INVALID_PARAMS;
    }
    return DP_GET_SERVICE_SUCCESS;
}


int32_t DistributedDeviceProfileClient::SyncDeviceProfile(const SyncOptions& syncOptions,
    const std::shared_ptr<IProfileEventCallback>& syncCb)
{
    auto dps = GetDeviceProfileService();
    if (dps == nullptr) {
        return DP_GET_SERVICE_FAILED;
    }
    HILOGD("Successfully synchronized device profile");
    return DP_GET_SERVICE_SUCCESS;
}

sptr<IDistributedDeviceProfile> DistributedDeviceProfileClient::GetDeviceProfileService()
{
    if (dpProxy_ != nullptr) {
        return dpProxy_;
    }
    HILOGD("Get device profile service succeeded");
    return dpProxy_;
}

bool DistributedDeviceProfileClient::CheckProfileInvalidity(const ServiceCharacteristicProfile& profile)
{
    bool state = profile.GetServiceId().empty() || profile.GetServiceType().empty() ||
                 profile.GetCharacteristicProfileJson().empty();
    HILOGD("The status of the invalid profile is :%{public}d", state);
    return state;
}
} // namespace DeviceProfile
} // namespace OHOS
