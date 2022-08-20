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

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
#include "i_call_dinput.h"
#include "device_manager_callback.h"
#include "dm_device_info.h"
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

#include "device_observer.h"
#include "event_dispatch.h"
#include "key_event_handler.h"
#include "input_device.h"
#include "key_auto_repeat.h"
#include "key_map_manager.h"
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
    int32_t GetKeyboardBusMode(int32_t deviceId);
    bool GetDeviceConfig(int32_t deviceId, int32_t &KeyboardType);
    int32_t GetDeviceSupportKey(int32_t deviceId);
    int32_t GetKeyboardType(int32_t deviceId);
    void Attach(std::shared_ptr<IDeviceObserver> observer);
    void Detach(std::shared_ptr<IDeviceObserver> observer);
    void NotifyPointerDevice(bool hasPointerDevice, bool isVisible);
    void AddDevListener(SessionPtr sess, std::function<void(int32_t, const std::string&)> callback);
    void RemoveDevListener(SessionPtr sess);
    void Dump(int32_t fd, const std::vector<std::string> &args);
    void DumpDeviceList(int32_t fd, const std::vector<std::string> &args);
    bool HasPointerDevice();

    std::string FindInputDeviceName(int32_t deviceId);
    void SetLastTouchDeviceId(int32_t lastTouchDeviceID);
    using DeviceUniqId = std::tuple<int32_t, int32_t, int32_t, int32_t, int32_t, std::string>;
    int32_t SetInputDeviceSeatName(const std::string& seatName, DeviceUniqId& deviceUniqId);
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    std::shared_ptr<InputDevice> GetRemoteInputDevice(int32_t id);
    bool IsDistributedInput(struct libinput_device* device) const;
    int32_t GetRemoteInputAbility(const std::string& deviceId, sptr<ICallDinput> ablitity);
    void OnStartRemoteInput(const std::string& deviceId, const uint32_t& inputTypes);
    void OnStopRemoteInput(const std::string& deviceId, const uint32_t& inputTypes);
    void SetPointerVisible(int32_t pid, bool isVisible);
    void OnDeviceOffline();
    bool InitDeviceManager();
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

private:
    bool IsPointerDevice(struct libinput_device* device);
    void ScanPointerDevice();
    std::shared_ptr<InputDevice> MakeInputDevice(int32_t id, struct libinput_device* libinputDevice) const;
    std::string MakeNetworkId(const std::string& phys) const;
    void HandleDeviceChanged(const std::string& changedType, int32_t id);
    bool IsRemote(struct libinput_device* inputDevice) const;
	
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    uint32_t DeviceUdevTagsToDinputType(enum evdev_device_udev_tags udevTags) const;
    void NotifyDeviceChanged(const std::string& deviceId, const uint32_t& inputTypes,
        const std::string& changedType);
    std::shared_ptr<DistributedHardware::DmInitCallback> initCallback_;
    std::shared_ptr<DistributedHardware::DeviceStateCallback> stateCallback_;
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

    std::map<int32_t, struct libinput_device *> inputDevice_;
    std::map<int32_t, std::string> inputDeviceSeat_;
    int32_t nextId_ { 0 };
    std::list<std::shared_ptr<IDeviceObserver>> observers_;
    std::map<SessionPtr, std::function<void(int32_t, const std::string&)>> devListener_;
    int32_t lastTouchDeviceID_ { -1 };
};

#define InputDevMgr InputDeviceManager::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_MANAGER_H
