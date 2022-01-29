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

#include "mouse_state_gesture.h"

namespace OHOS {
namespace MMI {
MouseDeviceState::MouseDeviceState()
{
    mouseCoords_ = {0, 0};
}

MouseDeviceState::~MouseDeviceState() { }

double MouseDeviceState::GetMouseCoordsX()
{
    return mouseCoords_.x;
}
double MouseDeviceState::GetMouseCoordsY()
{
    return mouseCoords_.y;
}

void MouseDeviceState::SetMouseCoords(const double x, const double y)
{
    mouseCoords_.x = x;
    mouseCoords_.y = y;
}

bool MouseDeviceState::IsLiftBtnPressed()
{
    std::lock_guard<std::mutex> lock(mu_);
    auto iter = mapCountState_.find(LIBINPUT_LEFT_BUTTON_CODE);
    if (iter != mapCountState_.end()) {
        if (iter->second > 0) {
            return true;
        }
    }
    return false;
}

void MouseDeviceState::GetPressedButtons(std::vector<uint32_t>& pressedButtons)
{
    std::lock_guard<std::mutex> lock(mu_);
    if (!mapCountState_.empty()) {
        for (auto iter : mapCountState_) {
            if (iter.second > 0) {
                pressedButtons.push_back(LibinputChangeToPointer(iter.first));
            }
        }
    }
}

std::map<int16_t, uint32_t> MouseDeviceState::GetCountState()
{
    return mapCountState_;
}

void MouseDeviceState::CountState(int16_t btnCode, uint32_t btnState)
{
    std::lock_guard<std::mutex> lock(mu_);
    std::map<int16_t, uint32_t>::iterator iter = mapCountState_.find(btnCode);
    if (iter != mapCountState_.end()) {
        ChangeMouseState(btnState, iter->second);
    } else {
        mapCountState_.insert(std::make_pair(btnCode, ((btnState == BUTTON_STATE_PRESSED) ? 1 : 0)));
    }
}
int16_t MouseDeviceState::LibinputChangeToPointer(int16_t keyValue)
{
    auto it = mapLibinputChangeToPointer.find(keyValue);
    if (it != mapLibinputChangeToPointer.end()) {
        return it->second;
    } else {
        return 0;
    }
}

void MouseDeviceState::ChangeMouseState(uint32_t btnState, uint32_t &stateValue)
{
    if (btnState == BUTTON_STATE_PRESSED) {
        stateValue++;
    } else if (btnState == BUTTON_STATE_RELEASED) {
        stateValue--;
    }
    CheckMouseState(stateValue);
}

void MouseDeviceState::CheckMouseState(uint32_t &stateValue)
{
    const int mouseBtnMax = 8; // 鼠标按键最多为8个
    if (stateValue > mouseBtnMax) {
        stateValue = mouseBtnMax;
    } else if (stateValue < 0) {
        stateValue = 0;
    }
}
}
}