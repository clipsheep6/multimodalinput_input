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

#include "virtual_trackpad_sys_ctrl.h"

OHOS::MMI::VirtualTrackpadSysCtrl::VirtualTrackpadSysCtrl() : VirtualDevice("Virtual TrackpadSysCtrl",
    BUS_USB, 0x62a, 0x8255)
{
}

OHOS::MMI::VirtualTrackpadSysCtrl::~VirtualTrackpadSysCtrl() {}

const std::vector<uint32_t>& OHOS::MMI::VirtualTrackpadSysCtrl::GetEventTypes() const
{
    static const std::vector<uint32_t> evt_types {
        EV_KEY, EV_MSC
    };
    return evt_types;
}

const std::vector<uint32_t>& OHOS::MMI::VirtualTrackpadSysCtrl::GetKeys() const
{
    static const std::vector<uint32_t> keys {
        KEY_POWER, KEY_SLEEP, KEY_WAKEUP
    };

    return keys;
}

const std::vector<uint32_t>& OHOS::MMI::VirtualTrackpadSysCtrl::GetMscs() const
{
    static const std::vector<uint32_t> mscs {
        MSC_SCAN
    };
    return mscs;
}