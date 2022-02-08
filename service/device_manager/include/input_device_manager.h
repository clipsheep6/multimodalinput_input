/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_INPUT_DEVICE_MANAGER_H
#define OHOS_INPUT_DEVICE_MANAGER_H
#include <list>
#include <string>
#include "util.h"
#include "singleton.h"
#include "../../../common/include/observer.h"
#include "msg_handler.h"
#include "event_dispatch.h"
#include "event_package.h"
#include "input_device.h"
#include "message_post.h"

namespace OHOS {
namespace MMI {
class InputDeviceManager : public DelayedSingleton<InputDeviceManager>, public Subject {
public:
    void OnInputDeviceAdded(libinput_device* inputDevice);
    void OnInputDeviceRemoved(libinput_device* inputDevice);
    std::vector<int32_t> GetInputDeviceIds();
    std::shared_ptr<InputDevice> GetInputDevice(int32_t id);
    void GetInputDeviceIdsAsync(std::function<void(std::vector<int32_t>)> callback);
    void FindInputDeviceByIdAsync(int32_t deviceId, std::function<void(std::shared_ptr<InputDevice>)> callback);
    int32_t FindInputDeviceId(libinput_device* inputDevice);
    void Attach(Observer* observer); 
    void Detach(Observer* observer);
    void Notify(bool hasPointerDevice);

private:
    void Init(weston_compositor *wc);
    std::vector<int32_t> GetInputDeviceIdsSync(weston_compositor *wc);
    std::shared_ptr<InputDevice> FindInputDeviceByIdSync(weston_compositor *wc, int32_t deviceId);
    bool IsPointerDevice(struct libinput_device* device);

    std::map<int32_t, libinput_device*> inputDeviceMap_;
    bool initFlag_ {false};
    int32_t nextId_ {0};
    std::list<Observer*> observers_;
};
}
}
#define inputDeviceManager OHOS::MMI::InputDeviceManager::GetInstance()
#endif
