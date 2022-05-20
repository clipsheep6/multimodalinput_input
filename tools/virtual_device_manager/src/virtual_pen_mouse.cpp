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

#include "virtual_pen_mouse.h"

namespace OHOS {
namespace MMI {
VirtualPenMouse::VirtualPenMouse() : VirtualDevice("V-Pencil-mouse", BUS_BLUETOOTH, 0x12d1, 0x10a5) {}

const std::vector<uint32_t>& VirtualPenMouse::GetEventTypes() const
{
    static const std::vector<uint32_t> eventTypes { EV_KEY, EV_REL, EV_MSC };
    return eventTypes;
}

const std::vector<uint32_t>& VirtualPenMouse::GetKeys() const
{
    static const std::vector<uint32_t> keys { BTN_LEFT, BTN_RIGHT, BTN_MIDDLE };
    return keys;
}

const std::vector<uint32_t>& VirtualPenMouse::GetRelBits() const
{
    static const std::vector<uint32_t> rels { REL_X, REL_Y, REL_WHEEL };
    return rels;
}

const std::vector<uint32_t>& VirtualPenMouse::GetMiscellaneous() const
{
    static const std::vector<uint32_t> miscellaneous { MSC_SCAN };
    return miscellaneous;
}
} // namespace MMI
} // namespace OHOS