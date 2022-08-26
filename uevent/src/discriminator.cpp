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

#include "discriminator.h"
#include <algorithm>
#include <unistd.h>
#include <linux/input.h>

#include "mmi_log.h"
#include "utility.h"


namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventDeviceDiscriminator" };

const Discriminator::DeviceCharacterMap deviceCharacters {
    {
        Discriminator::DeviceType::TOUCHSCREEN,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                    EV_ABS,
                },
                .keys {
                    BTN_TOUCH,
                },
                .abs {
                    ABS_MT_TOUCH_MAJOR,
                    ABS_MT_TOUCH_MINOR,
                    ABS_MT_POSITION_X,
                    ABS_MT_POSITION_Y,
                    ABS_MT_TRACKING_ID,
                    ABS_MT_PRESSURE,
                },
                .properties {
                    INPUT_PROP_DIRECT,
                },
            },
        }
    },
    {
        Discriminator::DeviceType::MOUSE,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                    EV_REL,
                },
                .keys {
                    BTN_LEFT,
                    BTN_RIGHT,
                    BTN_MIDDLE
                },
                .rels {
                    REL_X,
                    REL_Y,
                    REL_WHEEL
                },
            },
        },
    },
    {
        Discriminator::DeviceType::TABLET,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                    EV_ABS,
                },
                .keys {
                    BTN_TOOL_PEN,
                    BTN_TOUCH,
                    BTN_STYLUS,
                },
                .abs {
                    ABS_X,
                    ABS_Y,
                    ABS_PRESSURE,
                    ABS_TILT_X,
                    ABS_TILT_Y,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::TABLETPAD,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                    EV_ABS,
                },
                .keys {
                    BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_STYLUS,
                },
                .abs {
                    ABS_X, ABS_Y, ABS_WHEEL, ABS_MISC,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::TABLETTOUCHPAD,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                    EV_ABS,
                    EV_SW,
                },
                .keys {
                    BTN_TOOL_FINGER,
                    BTN_TOOL_QUINTTAP,
                    BTN_TOUCH,
                    BTN_TOOL_DOUBLETAP,
                    BTN_TOOL_TRIPLETAP,
                    BTN_TOOL_QUADTAP,
                },
                .abs {
                    ABS_X,
                    ABS_Y,
                    ABS_MT_SLOT,
                    ABS_MT_TOUCH_MAJOR,
                    ABS_MT_TOUCH_MINOR,
                    ABS_MT_ORIENTATION,
                    ABS_MT_POSITION_X,
                    ABS_MT_POSITION_Y,
                    ABS_MT_TRACKING_ID,
                },
                .switches {
                    SW_MUTE_DEVICE,
                },
                .properties {
                    INPUT_PROP_POINTER,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::KEYBOARD,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                },
                .keys {
                    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,
                    KEY_F10, KEY_F11, KEY_F12,
                    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
                    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
                    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
                    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
                    KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
                    KEY_TAB, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_BACKSLASH,
                    KEY_CAPSLOCK, KEY_SEMICOLON, KEY_ENTER,
                    KEY_LEFTSHIFT, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_RIGHTSHIFT,
                    KEY_LEFTCTRL, KEY_LEFTALT, KEY_SPACE, KEY_RIGHTALT, KEY_RIGHTCTRL,
                    KEY_SCROLLLOCK,
                    KEY_KP0, KEY_KP1, KEY_KP2, KEY_KP3, KEY_KP4, KEY_KP5, KEY_KP6, KEY_KP7, KEY_KP8, KEY_KP9,
                    KEY_NUMLOCK, KEY_KPSLASH, KEY_KPASTERISK, KEY_KPMINUS, KEY_KPPLUS, KEY_KPDOT, KEY_KPENTER,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::SYSKEY,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                },
                .keys {
                    KEY_MUTE, KEY_VOLUMEDOWN, KEY_VOLUMEUP, KEY_MENU,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::PWRKEY,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                },
                .keys {
                    KEY_POWER,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::HEADSET,
        {
            .includes {
                .eventTypes {
                    EV_KEY,
                },
                .keys {
                    KEY_MEDIA,
                },
            },
        },
    },
    {
        Discriminator::DeviceType::SWITCH,
        {
            .includes {
                .eventTypes {
                    EV_SW,
                },
            },
        },
    }
};
}

Discriminator::Discriminator() {}

Discriminator::~Discriminator() {}

bool Discriminator::UpdateDeviceCapability(std::shared_ptr<Device> &device) const
{
    CHKPF(device);
    return UpdateDeviceCapability(*device);
}

bool Discriminator::UpdateDeviceCapability(Device &device) const
{
    const char *devnode = device.GetDevnode();
    if (Utility::IsEmpty(devnode)) {
        return false;
    }
    MMI_HILOGD("Device:\'%{public}s\'", devnode);
    char rpath[PATH_MAX];
    if (realpath(devnode, rpath) == nullptr) {
        MMI_HILOGE("path(\'%{public}s\') error: %{public}s", devnode, strerror(errno));
        return false;
    }

    int fd = open(rpath, O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) {
        MMI_HILOGE("Opening device failed: %{public}s", strerror(errno));
        return false;
    }

    struct libevdev *evdev = nullptr;
    if (libevdev_new_from_fd(fd, &evdev) < 0) {
        MMI_HILOGD("path: %{public}s", rpath);
        MMI_HILOGE("libevdev error: %{public}s", strerror(errno));
        close(fd);
        return false;
    }

    for (const auto &[dtype, dcharacter] : deviceCharacters) {
        if (!MatchDeviceCapability(evdev, dcharacter.includes)) {
            continue;
        }
        if (!RuleOutDeviceCapability(evdev, dcharacter.excludes)) {
            continue;
        }
        TagDeviceCapability(evdev, dtype, device);
    }

    libevdev_free(evdev);
    close(fd);
    return true;
}

bool Discriminator::MatchDeviceCapability(const struct libevdev *evdev, const DeviceCapability &dcap) const
{
    for (const auto &type : dcap.eventTypes) {
        if (!libevdev_has_event_type(evdev, type)) {
            return false;
        }
        switch (type) {
            case EV_KEY: {
                if (!MatchEvents(evdev, type, dcap.keys)) {
                    return false;
                }
                break;
            }
            case EV_REL: {
                if (!MatchEvents(evdev, type, dcap.rels)) {
                    return false;
                }
                break;
            }
            case EV_ABS: {
                if (!MatchEvents(evdev, type, dcap.abs)) {
                    return false;
                }
                break;
            }
            case EV_SW: {
                if (!MatchEvents(evdev, type, dcap.switches)) {
                    return false;
                }
                break;
            }
            default: {
                MMI_HILOGD("Unrecognized event type");
                break;
            }
        }
    }

    for (const auto &property : dcap.properties) {
        if (!libevdev_has_property(evdev, property)) {
            return false;
        }
    }
    return true;
}

bool Discriminator::MatchEvents(const struct libevdev *evdev, unsigned int type,
    const std::set<unsigned int> &codes) const
{
    for (const auto &code : codes) {
        if (!libevdev_has_event_code(evdev, type, code)) {
            return false;
        }
    }
    return true;
}

bool Discriminator::RuleOutDeviceCapability(const struct libevdev *evdev, const DeviceCapability &dcap) const
{
    for (const auto &type : dcap.eventTypes) {
        if (libevdev_has_event_type(evdev, type)) {
            return false;
        }
    }
    if (!RuleOutEvents(evdev, EV_KEY, dcap.keys)) {
        return false;
    }
    if (!RuleOutEvents(evdev, EV_REL, dcap.rels)) {
        return false;
    }
    if (!RuleOutEvents(evdev, EV_ABS, dcap.abs)) {
        return false;
    }
    if (!RuleOutEvents(evdev, EV_SW, dcap.switches)) {
        return false;
    }
    for (const auto &property : dcap.properties) {
        if (libevdev_has_property(evdev, property)) {
            return false;
        }
    }
    return true;
}

bool Discriminator::RuleOutEvents(const struct libevdev *evdev, unsigned int type,
    const std::set<unsigned int> &codes) const
{
    for (const auto &code : codes) {
        if (libevdev_has_event_code(evdev, type, code)) {
            return false;
        }
    }
    return true;
}

void Discriminator::TagDeviceCapability(const struct libevdev *evdev,
    const DeviceType dtype, Device &device) const
{
    switch (dtype) {
        case Discriminator::DeviceType::TOUCHSCREEN: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_TOUCHSCREEN", "1");
            break;
        }
        case Discriminator::DeviceType::MOUSE: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_MOUSE", "1");
            break;
        }
        case Discriminator::DeviceType::TABLET: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_TABLET", "1");
            break;
        }
        case Discriminator::DeviceType::TABLETPAD: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_TABLET", "1");
            device.AddProperty("ID_INPUT_TABLET_PAD", "1");
            break;
        }
        case Discriminator::DeviceType::TABLETTOUCHPAD: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_TABLET", "1");
            device.AddProperty("ID_INPUT_TOUCHPAD", "1");
            break;
        }
        case Discriminator::DeviceType::KEYBOARD: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_KEYBOARD", "1");
            break;
        }
        case Discriminator::DeviceType::SYSKEY:
        case Discriminator::DeviceType::PWRKEY:
        case Discriminator::DeviceType::HEADSET: {
            device.AddProperty("ID_INPUT", "1");
            device.AddProperty("ID_INPUT_KEY", "1");
            break;
        }
        case Discriminator::DeviceType::SWITCH: {
            device.AddProperty("ID_INPUT_SWITCH", "1");
            break;
        }
        case Discriminator::DeviceType::UNKNOWN: {
            MMI_HILOGD("Unknown device type");
            break;
        }
        default: {
            MMI_HILOGE("Unrecognized device type");
            break;
        }
    }
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
