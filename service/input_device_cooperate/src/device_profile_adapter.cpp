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

#include "device_profile_adapter.h"

#include <algorithm>
#include <mutex>

#include "distributed_device_profile_client.h"
#include "nlohmann/json.hpp"
#include "service_characteristic_profile.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "sync_options.h"

namespace OHOS {
namespace MMI {
using namespace OHOS::DeviceProfile;
namespace {
class ProfileEventCallbackImpl : public DeviceProfile::IProfileEventCallback {
public:
    void OnSyncCompleted(const DeviceProfile::SyncResult &syncResults) override;
    void OnProfileChanged(const DeviceProfile::ProfileChangeNotification &changeNotification) override;
};

constexpr const char *SERVICE_ID = "InputDeviceCooperation";
constexpr const char *SERVICE_TYPE = "InputDeviceCooperation";
constexpr const char *CHARACTERISTICS_NAME = "CurrentState";
std::mutex adapterLock;
std::map<std::string, DeviceProfileAdapter::ProfileEventCallback> callbacks_;
} // namespace
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DeviceProfileAdapter" };

DeviceProfileAdapter::DeviceProfileAdapter()
{
    profileEventCallback_ = std::make_shared<ProfileEventCallbackImpl>();
}

DeviceProfileAdapter::~DeviceProfileAdapter()
{
    DistributedDeviceProfileClient::GetInstance().UnsubscribeProfileEvent(ProfileEvent::EVENT_PROFILE_CHANGED,
                                                                          profileEventCallback_);
    profileEventCallback_ = nullptr;
    callbacks_.clear();
}

int32_t DeviceProfileAdapter::UpdateCrossingSwitchState(bool state, const std::vector<std::string> &deviceIds)
{
    ServiceCharacteristicProfile profile;
    profile.SetServiceId(SERVICE_ID);
    profile.SetServiceType(SERVICE_TYPE);
    nlohmann::json data;
    data[CHARACTERISTICS_NAME] = state;
    profile.SetCharacteristicProfileJson(data.dump());

    int32_t putRet = DistributedDeviceProfileClient::GetInstance().PutDeviceProfile(profile);
    SyncOptions syncOptions;
    std::for_each(deviceIds.begin(), deviceIds.end(),
                  [&syncOptions](auto &deviceId) { syncOptions.AddDevice(deviceId); });
    int32_t syncRet =
        DistributedDeviceProfileClient::GetInstance().SyncDeviceProfile(syncOptions, profileEventCallback_);
    if (syncRet != 0) {
        MMI_HILOGW("Sync device profile failed code:%{public}d", syncRet);
    }
    return putRet;
}

int32_t DeviceProfileAdapter::UpdateCrossingSwitchState(bool state)
{
    ServiceCharacteristicProfile profile;
    profile.SetServiceId(SERVICE_ID);
    profile.SetServiceType(SERVICE_TYPE);
    nlohmann::json data;
    data[CHARACTERISTICS_NAME] = state;
    profile.SetCharacteristicProfileJson(data.dump());
    return DistributedDeviceProfileClient::GetInstance().PutDeviceProfile(profile);
}

bool DeviceProfileAdapter::GetCrossingSwitchState(const std::string &deviceId)
{
    ServiceCharacteristicProfile profile;
    DistributedDeviceProfileClient::GetInstance().GetDeviceProfile(deviceId, SERVICE_ID, profile);
    std::string jsonData = profile.GetCharacteristicProfileJson();
    nlohmann::json jsonObject = nlohmann::json::parse(jsonData, nullptr, false);
    bool result = false;
    if (jsonObject.is_discarded()) {
        MMI_HILOGE("JsonData is discarded");
        return result;
    }
    result = jsonObject[CHARACTERISTICS_NAME].get<bool>();
    return result;
}

int32_t DeviceProfileAdapter::RegisterCrossingStateListener(const std::string &deviceId, ProfileEventCallback callback)
{
    CHKPR(callback, RET_ERR);
    std::lock_guard<std::mutex> lock(adapterLock);
    if (deviceId.empty()) {
        MMI_HILOGE("DeviceId is nullptr");
        return RET_ERR;
    }
    auto callbackIter = callbacks_.find(deviceId);
    if (callbackIter != callbacks_.end()) {
        if (&callbackIter->second == &callback) {
            MMI_HILOGW("Callback is already registered");
            return RET_OK;
        }
        callbackIter->second = callback;
        MMI_HILOGW("Callback is updated");
        return RET_OK;
    }
    callbacks_[deviceId] = callback;
    MMI_HILOGI("Register crossing state listener success");
    if (profileEventCallback_ == nullptr) {
        profileEventCallback_ = std::make_shared<ProfileEventCallbackImpl>();
    }
    int32_t registerRet = RegisterProfileListener(deviceId);
    if (registerRet != RET_OK) {
        MMI_HILOGE("Register profile listener failed error :%{public}d", registerRet);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DeviceProfileAdapter::UnregisterCrossingStateListener(const std::string &deviceId)
{
    if (deviceId.empty()) {
        MMI_HILOGE("DeviceId is nullptr");
        return RET_ERR;
    }
    auto callbackIter = callbacks_.find(deviceId);
    if (callbackIter == callbacks_.end()) {
        MMI_HILOGW("This device has no callback");
        return RET_OK;
    }
    callbacks_.erase(callbackIter);
    return RET_OK;
}

int32_t DeviceProfileAdapter::RegisterProfileListener(const std::string &deviceId)
{
    std::list<std::string> serviceIdList;
    serviceIdList.emplace_back(SERVICE_ID);
    ExtraInfo extraInfo;
    extraInfo["deviceId"] = deviceId;
    extraInfo["serviceIds"] = serviceIdList;
    SubscribeInfo changeEventInfo;
    changeEventInfo.profileEvent = ProfileEvent::EVENT_PROFILE_CHANGED;
    changeEventInfo.extraInfo = std::move(extraInfo);
    std::list<SubscribeInfo> subscribeInfos;
    subscribeInfos.emplace_back(changeEventInfo);
    SubscribeInfo syncEventInfo;
    syncEventInfo.profileEvent = ProfileEvent::EVENT_SYNC_COMPLETED;
    subscribeInfos.emplace_back(syncEventInfo);
    std::list<ProfileEvent> failedEvents;
    int32_t ret = DistributedDeviceProfileClient::GetInstance().SubscribeProfileEvents(
        subscribeInfos, profileEventCallback_, failedEvents);
    return ret;
}

void ProfileEventCallbackImpl::OnProfileChanged(
    const ProfileChangeNotification &changeNotification)
{
    std::string deviceId = changeNotification.GetDeviceId();
    MMI_HILOGD("The profile has changed deviceId is %{public}s", deviceId.c_str());
    auto it = callbacks_.find(deviceId);
    if (it == callbacks_.end()) {
        MMI_HILOGW("The device has no callback");
        return;
    }
    if (it->second) {
        auto state = DProfileAdapter->GetCrossingSwitchState(deviceId);
        it->second(deviceId, state);
    } else {
        callbacks_.erase(it);
    }
}

void ProfileEventCallbackImpl::OnSyncCompleted(const DeviceProfile::SyncResult &syncResults)
{
    std::for_each(syncResults.begin(), syncResults.end(), [](const auto &syncResult) {
        MMI_HILOGD("Sync result : deviceId: %{public}s, result:%{public}d", syncResult.first.c_str(),
                   syncResult.second);
    });
}
} // namespace MMI
} // namespace OHOS
