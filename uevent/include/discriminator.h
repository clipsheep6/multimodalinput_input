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

#ifndef UEVENT_DISCRIMINATOR_H
#define UEVENT_DISCRIMINATOR_H

#include <memory>
#include <map>
#include <set>

#include <libevdev/libevdev.h>
#include <nocopyable.h>
#include <singleton.h>
#include "device.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {

class Discriminator {
    DECLARE_DELAYED_SINGLETON(Discriminator);

public:
    enum class DeviceType {
        TOUCHSCREEN,
        MOUSE,
        TABLET,
        TABLETPAD,
        TABLETTOUCHPAD,
        KEYBOARD,
        SYSKEY,
        PWRKEY,
        HEADSET,
        SWITCH,
        UNKNOWN,
    };

    struct DeviceCapability {
        std::set<unsigned int> eventTypes;
        std::set<unsigned int> keys;
        std::set<unsigned int> rels;
        std::set<unsigned int> abs;
        std::set<unsigned int> switches;
        std::set<unsigned int> properties;
    };

    struct DeviceCharacter {
        DeviceCapability includes;
        DeviceCapability excludes;
    };

    using DeviceCharacterMap = std::map<DeviceType, DeviceCharacter>;

public:
    DISALLOW_COPY_AND_MOVE(Discriminator);
    bool UpdateDeviceCapability(std::shared_ptr<Device> &device) const;
    bool UpdateDeviceCapability(Device &device) const;

private:
    bool MatchDeviceCapability(const struct libevdev *evdev, const DeviceCapability &dcap) const;
    bool MatchEvents(const struct libevdev *evdev, unsigned int type, const std::set<unsigned int> &codes) const;
    bool RuleOutDeviceCapability(const struct libevdev *evdev, const DeviceCapability &dcap) const;
    bool RuleOutEvents(const struct libevdev *evdev, unsigned int type, const std::set<unsigned int> &codes) const;
    void TagDeviceCapability(const struct libevdev *evdev,
        const DeviceType dtype, Device &device) const;
};

#define UEDiscriminator DelayedSingleton<Discriminator>::GetInstance()
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_DISCRIMINATOR_H