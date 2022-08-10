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
    struct InputDeviceObj {
        struct libinput_device *inputDeviceOrgin_ { nullptr };
        std::string networkIdOrgin_ { "" };
        bool isRemote_ { false };
        int32_t fd_;
        std::string dhid_ { "" };
    };
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
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    std::vector<int32_t> GetPointerKeyboardFds(int32_t pointerId);
    std::string GetOrginNetworkId(int32_t id);
    std::string GetOrginNetworkId(const std::string& dhid);
    void GetLocalDeviceId(std::string& networkId);
    std::string GetDhid(int32_t deviceId);
    std::vector<std::string> GetPointerKeyboardUnqs(int32_t pointerId);
    std::vector<std::string> GetPointerKeyboardUnqs(const std::string& dhid);
    bool HasLocalPointerDevice() const;
    bool IsRemote(struct libinput_device* inputDevice) const;
    bool IsRemote(int32_t id) const;
#endif // OHOS_BUILD_ENABLE_COOPERATE
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    bool HasPointerDevice();
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING

private:
    bool IsPointerDevice(struct libinput_device* device) const;
    void ScanPointerDevice();
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    std::string MakeNetworkId(const char* phys) const;
    bool IsKeyboard(struct libinput_device* device) const;
    std::string StringPrintf(const char *format, ...) const;
    std::string Sha256(const std::string &in) const;
    std::string GenerateDescriptor(struct libinput_device *inputDevice, bool isRemote) const;
    std::vector<std::string> Split(std::string str, std::string pattern) const;
#endif // OHOS_BUILD_ENABLE_COOPERATE
    std::map<int32_t, struct InputDeviceObj> inputDevice_;
    int32_t nextId_ {0};
    std::list<std::shared_ptr<IDeviceObserver>> observers_;
    std::map<SessionPtr, std::function<void(int32_t, const std::string&)>> devListener_;
};

#define InputDevMgr InputDeviceManager::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_MANAGER_H