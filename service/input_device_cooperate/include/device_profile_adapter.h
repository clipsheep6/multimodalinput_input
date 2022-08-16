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
     * 更新开关状态，但不会通知到设备
     *
     * @param state 开关状态
     * @return 0表示成功，其他表示失败
     */
    int32_t UpdateCrossingSwitchState(bool state);

    /**
     * 更新开关状态，并通知到设备
     *
     * @param state 开关状态
     * @param deviceIds 设备id列表
     * @return 0表示成功，其他表示失败
     */
    int32_t UpdateCrossingSwitchState(bool state, std::vector<std::string> &deviceIds);

    /**
     * 注册事件回调函数
     *
     * @param deviceId 设备id
     * @return true 开关状态
     * @return false 开关状态
     */
    bool GetCrossingSwitchState(const std::string &deviceId);

    /**
     * 注册事件回调函数
     *
     * @param deviceId 设备id
     * @param callback 事件回调函数
     * @return 0表示成功，其他表示失败
     */
    int32_t RegisterCrossingStateListener(const std::string &deviceId, ProfileEventCallback callback);

    /**
     * 取消注册事件回调函数
     *
     * @param deviceId 设备id
     * @param callback 事件回调函数
     * @return 0表示成功，其他表示失败
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
