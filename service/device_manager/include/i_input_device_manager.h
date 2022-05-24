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

#ifndef I_INPUT_DEVICE_MANAGER_H
#define I_INPUT_DEVICE_MANAGER_H

#include <memory>
#include <vector>
#include <list>

#include "libinput.h"
#include "nocopyable.h"
#include "singleton.h"
#include "device_observer.h"
#include "input_device.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class IInputDeviceManager : public DelayedSingleton<IInputDeviceManager>, public IDeviceObject {
public:
    IInputDeviceManager() = default;
    DISALLOW_COPY_AND_MOVE(IInputDeviceManager);
    ~IInputDeviceManager() = default;

    void OnInputDeviceAdded(struct libinput_device *inputDevice);
    void OnInputDeviceRemoved(struct libinput_device *inputDevice);
    std::vector<int32_t> GetInputDeviceIds() const;
    std::shared_ptr<InputDevice> GetInputDevice(int32_t id) const;
    std::vector<bool> SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes);
    int32_t FindInputDeviceId(struct libinput_device* inputDevice);
    void Attach(std::shared_ptr<IDeviceObserver> observer);
    void Detach(std::shared_ptr<IDeviceObserver> observer);
    void NotifyPointerDevice(bool hasPointerDevice);
    void AddDevMonitor(SessionPtr sess, std::function<void(std::string, int32_t)> callback);
    void RemoveDevMonitor(SessionPtr sess);
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    bool HasPointerDevice();
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING

private:
    std::list<std::shared_ptr<IDeviceObserver>> observers_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE_MANAGER_H