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

#include "mouse_device_state.h"

namespace OHOS {
namespace MMI {
MouseDeviceState::MouseDeviceState()
{
    mouseCoord_ = {0, 0};
}

MouseDeviceState::~MouseDeviceState()
{
}

int32_t MouseDeviceState::GetMouseCoordsX() const
{
    return mouseCoord_.globleX;
}

int32_t MouseDeviceState::GetMouseCoordsY() const
{
    return mouseCoord_.globleY;
}

void MouseDeviceState::SetMouseCoords(const int32_t x, const int32_t y)
{
    MMI_LOGD("enter");
    mouseCoord_.globleX = x;
    mouseCoord_.globleY = y;
    MMI_LOGD("leave");
}

bool MouseDeviceState::IsLeftBtnPressed()
{
    MMI_LOGD("enter");
    std::lock_guard<std::mutex> lock(mu_);
    auto iter = mouseBtnState_.find(LIBINPUT_LEFT_BUTTON_CODE);
    if (iter == mouseBtnState_.end()) {
        return false;
    }
    if (iter->second > 0) {
        return true;
    }
    MMI_LOGD("leave");
    return false;
}

void MouseDeviceState::GetPressedButtons(std::vector<int32_t>& pressedButtons)
{
    MMI_LOGD("enter");
    std::lock_guard<std::mutex> lock(mu_);
    for (const auto &item : mouseBtnState_) {
        if (item.second > 0) {
            pressedButtons.push_back(LibinputChangeToPointer(item.first));
        }
    }
    MMI_LOGD("leave");
}

std::map<uint32_t, int32_t> MouseDeviceState::GetMouseBtnState()
{
    MMI_LOGD("enter");
    return mouseBtnState_;
}

void MouseDeviceState::MouseBtnStateCounts(uint32_t btnCode, const BUTTON_STATE btnState)
{
    MMI_LOGD("enter");
    std::lock_guard<std::mutex> lock(mu_);
    std::map<uint32_t, int32_t>::iterator iter = mouseBtnState_.find(btnCode);
    if (iter == mouseBtnState_.end()) {
        mouseBtnState_.insert(std::make_pair(btnCode, ((btnState == BUTTON_STATE_PRESSED) ? 1 : 0)));
        return;
    }
    ChangeMouseState(btnState, iter->second);
    MMI_LOGD("leave");
}

int32_t MouseDeviceState::LibinputChangeToPointer(const uint32_t keyValue)
{
    MMI_LOGD("enter");
    auto it = mapLibinputChangeToPointer.find(keyValue);
    if (it == mapLibinputChangeToPointer.end()) {
        return PointerEvent::BUTTON_NONE;
    }
    MMI_LOGD("leave");
    return it->second;
}

void MouseDeviceState::ChangeMouseState(const BUTTON_STATE btnState, int32_t &btnStateCount)
{
    MMI_LOGD("enter");
    if (btnState == BUTTON_STATE_PRESSED) {
        btnStateCount++;
    } else if (btnState == BUTTON_STATE_RELEASED) {
        btnStateCount--;
    }
    if (btnStateCount > mouseBtnMax) {
        btnStateCount = mouseBtnMax;
    } else if (btnStateCount < 0) {
        btnStateCount = 0;
    }
    MMI_LOGD("leave");
}
} // namespace MMI
} // namespace OHOS