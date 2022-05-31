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

#ifndef INPUT_DEVICE_MANAGER_H
#define INPUT_DEVICE_MANAGER_H

#include <list>
#include <string>

#include "i_call_dinput.h"
#include "device_observer.h"
#include "device_manager_callback.h"
#include "dm_device_info.h"
#include "event_dispatch.h"
#include "event_package.h"
#include "input_device.h"
#include "msg_handler.h"
#include "nocopyable.h"
#include "singleton.h"
#include "util.h"

namespace OHOS {
namespace MMI {
class InputDeviceManager : public DelayedSingleton<InputDeviceManager>, public IDeviceObject {
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
class DeviceInitCallBack : public DistributedHardware::DmInitCallback {
    void OnRemoteDied() override;
};

class MmiDeviceStateCallback : public DistributedHardware::DeviceStateCallback {
    void OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
};
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
public:
    InputDeviceManager() = default;
    DISALLOW_COPY_AND_MOVE(InputDeviceManager);
    void OnInputDeviceAdded(struct libinput_device *inputDevice);
    void OnInputDeviceRemoved(struct libinput_device *inputDevice);
    std::vector<int32_t> GetInputDeviceIds() const;
    std::shared_ptr<InputDevice> GetInputDevice(int32_t id) const;
    std::vector<bool> SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes);
    int32_t FindInputDeviceId(struct libinput_device* inputDevice);
    void Attach(std::shared_ptr<IDeviceObserver> observer);
    void Detach(std::shared_ptr<IDeviceObserver> observer);
    void NotifyPointerDevice(bool hasPointerDevice, bool isPointerVisible);
    void AddDevMonitor(SessionPtr sess, std::function<void(std::string, int32_t)> callback);
    void RemoveDevMonitor(SessionPtr sess);
    bool HasPointerDevice();
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    std::shared_ptr<InputDevice> GetRemoteInputDevice(int32_t id);
    bool IsDistributedInput(struct libinput_device* device) const;
    int32_t GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity);
    void OnStartRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes);
    void OnStopRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes);
    void SetPointerVisible(int32_t pid, bool visible);
    void OnDeviceOffline();

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    bool InitDeviceManager();
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
private:
    bool IsPointerDevice(struct libinput_device* device);
    void ScanPointerDevice();
    std::shared_ptr<InputDevice> MakeInputDevice(int32_t id, struct libinput_device * libinputDevice) const;
    std::string MakeNetworkId(const std::string& phys) const;
    void HandleDeviceChanged(std::string changedType, int32_t id);
    bool IsRemote(struct libinput_device* inputDevice) const;
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    uint32_t DeviceUdevTagsToDinputType(enum evdev_device_udev_tags udevTags) const;
    void NotifyDeviceChanged(const std::string& deviceId, const uint32_t& inputTypes, std::string changedType);
    std::shared_ptr<DistributedHardware::DmInitCallback> initCallback_;
    std::shared_ptr<DistributedHardware::DeviceStateCallback> stateCallback_;
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
    std::map<int32_t, struct libinput_device *> inputDevice_;
    int32_t nextId_ {0};
    std::list<std::shared_ptr<IDeviceObserver>> observers_;
    std::map<SessionPtr, std::function<void(std::string, int32_t)>> devMonitor_;
};

#define InputDevMgr InputDeviceManager::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_MANAGER_H