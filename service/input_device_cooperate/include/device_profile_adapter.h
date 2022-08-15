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

#ifndef DEVICE_PROFILE_ADAPTER_H
#define DEVICE_PROFILE_ADAPTER_H

#include <functional>
#include <memory>
#include <vector>

#include "define_multimodal.h"
#include "iprofile_event_callback.h"
#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {
class DeviceProfileAdapter : public DelayedSingleton<DeviceProfileAdapter> {
public:
    using ProfileEventCallback = std::function<void(const std::string &, bool)>;
    DeviceProfileAdapter();
    ~DeviceProfileAdapter();
    DISALLOW_COPY_AND_MOVE(DeviceProfileAdapter);

    /**
     * Abandoned: it will cause this change can not notify other device
     *
     * @param state The new Switch state
     * @return int32_t 0 : OK , Other: FAIL
     */
    int32_t UpdateCrossingSwitchState(bool state);

    /**
     * UpdateCrossingSwtichState
     *
     * @param state The new Switch state
     * @param deviceIds Which device you want notify this change
     * @return int32_t 0 : OK , Other: FAIL
     */
    int32_t UpdateCrossingSwitchState(bool state, std::vector<std::string> &deviceIds);

    /**
     * @brief Get the Crossing Switch State object
     *
     * @param deviceId The device you want to get
     * @return true SwitchState
     * @return false SwitchState
     */
    bool GetCrossingSwitchState(const std::string &deviceId);

    /**
     * Regist a listener to listen for one device switch change
     *
     * @param deviceId The device you want to listen
     * @param callback The function for call you
     * @return int32_t 0 : OK , Other: FAIL
     */
    int32_t RegisterCrossingStateListener(const std::string &deviceId, ProfileEventCallback callback);

    /**
     * Unregist the listener to listen for one device switch change
     *
     * @param deviceId The device you want to Unregist
     * @param callback The which you set listen
     * @return int32_t 0 : OK , Other: FAIL
     */
    int32_t UnregisterCrossingStateListener(const std::string &deviceId);

private:
    class ProfileEventCallbackImpl : public DeviceProfile::IProfileEventCallback {
    public:
        void OnSyncCompleted(const DeviceProfile::SyncResult &syncResults) override;
        void OnProfileChanged(const DeviceProfile::ProfileChangeNotification &changeNotification) override;
    };
    void Init();
    void Release();
    int32_t RegisterProfileListener(const std::string &deviceId);
    std::map<std::string, ProfileEventCallback> callbacks_;
    std::shared_ptr<DeviceProfile::IProfileEventCallback> profileEventCallback_;
};

#define DProfileAdapter DeviceProfileAdapter::GetInstance()
} // namespace MMI
} // namespace OHOS

#endif // DEVICE_PROFILE_ADAPTER_H
