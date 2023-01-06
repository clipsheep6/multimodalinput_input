/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "keyboard_manager_commond.h"

#include <iostream>

#include "string_ex.h"

#include "error_multimodal.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "key_event.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyboardManagerCommand" };
struct option keyboardSensorOptions[] = {
    {"down", required_argument, NULL, 'd'},
    {"up", required_argument, NULL, 'u'},
    {"long_press", required_argument, NULL, 'l'},
    {"interval", required_argument, NULL, 'i'},
    {NULL, 0, NULL, 0}
};
} // namespace

int32_t KeyboardManagerCommand::CommandHandle(int32_t argc, char *argv[], char* &optarg,
    int32_t &optind, int32_t &optionIndex)
{
    time_ = GetSysClockTime();
    int32_t c = 0;
    while ((c = getopt_long(argc, argv, "d:u:l:i:", keyboardSensorOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'd': {
                RETCASE(KeyboardManagerCommand::DownCommand(optarg, optind), EVENT_REG_FAIL);
                break;
            }
            case 'u': {
                RETCASE(KeyboardManagerCommand::UpCommand(optarg, optind), EVENT_REG_FAIL);
                break;
            }
            case 'l': {
                RETCASE(KeyboardManagerCommand::LongPressCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            case 'i': {
                RETCASE(KeyboardManagerCommand::IntervalCommand(optarg, optind), EVENT_REG_FAIL);
                break;
            }
            default: {
                std::cout << "invalid command to keyboard key" << std::endl;
                manager.ShowUsage();
                return EVENT_REG_FAIL;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
    }
    for (size_t i = 0; i < downKey_.size(); i++) {
        std::cout << "you have a key " << downKey_[i] << " not release" << std::endl;
    }
    return RET_OK;
}

int32_t KeyboardManagerCommand::DownCommand(char* &optarg, int32_t &optind)
{
    if (!StrToInt(optarg, keyCode_)) {
        std::cout << "invalid command to down key" << std::endl;
    }
    if (optind == isCombinationKey_ + TWO_MORE_COMMAND) {
        downKey_.push_back(keyCode_);
        isCombinationKey_ = optind;
        auto KeyEvent = KeyEvent::Create();
        CHKPR(KeyEvent, ERROR_NULL_POINTER);
        if (downKey_.size() > MAX_PRESSED_COUNT) {
            std::cout << "pressed button count should less than 30" << std::endl;
            return EVENT_REG_FAIL;
        }
        KeyEvent::KeyItem item[downKey_.size()];
        for (size_t i = 0; i < downKey_.size(); i++) {
            KeyEvent->SetKeyCode(keyCode_);
            KeyEvent->SetActionTime(time_);
            KeyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
            item[i].SetKeyCode(downKey_[i]);
            item[i].SetDownTime(time_);
            item[i].SetPressed(true);
            KeyEvent->AddKeyItem(item[i]);
        }
        InputManager::GetInstance()->SimulateInputEvent(KeyEvent);
        return RET_OK;
    }
    downKey_.push_back(keyCode_);
    auto KeyEvent = KeyEvent::Create();
    CHKPR(KeyEvent, ERROR_NULL_POINTER);
    KeyEvent->SetKeyCode(keyCode_);
    KeyEvent->SetActionTime(time_);
    KeyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    KeyEvent::KeyItem item1;
    item1.SetKeyCode(keyCode_);
    item1.SetDownTime(time_);
    item1.SetPressed(true);
    KeyEvent->AddKeyItem(item1);
    InputManager::GetInstance()->SimulateInputEvent(KeyEvent);
    isCombinationKey_ = optind;
    return RET_OK;
}

int32_t KeyboardManagerCommand::UpCommand(char* &optarg, int32_t &optind)
{
    if (!StrToInt(optarg, keyCode_)) {
        std::cout << "invalid button press command" << std::endl;
        return EVENT_REG_FAIL;
    }
    std::vector<int32_t>::iterator iter = std::find(downKey_.begin(), downKey_.end(), keyCode_);
    if (iter != downKey_.end()) {
        std::cout << "you raised the key " << keyCode_ << std::endl;
        auto KeyEvent = KeyEvent::Create();
        CHKPR(KeyEvent, ERROR_NULL_POINTER);
        KeyEvent->SetKeyCode(keyCode_);
        KeyEvent->SetActionTime(time_);
        KeyEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
        KeyEvent::KeyItem item;
        item.SetKeyCode(keyCode_);
        item.SetDownTime(time_);
        item.SetPressed(false);
        KeyEvent->AddKeyItem(item);
        InputManager::GetInstance()->SimulateInputEvent(KeyEvent);
        iter = downKey_.erase(iter);
        return RET_OK;
    } else {
        std::cout << "please press the " << keyCode_ << " key first "<< std::endl;
        return EVENT_REG_FAIL;
    }
}

int32_t KeyboardManagerCommand::LongPressCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind)
{
    int32_t pressTimeMs = 3000;
    RETCASE(ParamLongCommand(argc, argv, optarg, optind, pressTimeMs), EVENT_REG_FAIL);
    auto keyEvent = KeyEvent::Create();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    keyEvent->SetKeyCode(keyCode_);
    keyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    KeyEvent::KeyItem item;
    item.SetKeyCode(keyCode_);
    item.SetPressed(true);
    auto keyEventTemp = KeyEvent::Clone(keyEvent);
    CHKPR(keyEventTemp, ERROR_NULL_POINTER);
    keyEventTemp->AddKeyItem(item);
    InputManager::GetInstance()->SimulateInputEvent(keyEventTemp);
    std::this_thread::sleep_for(std::chrono::milliseconds(pressTimeMs));

    keyEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    item.SetPressed(false);
    keyEvent->AddKeyItem(item);
    InputManager::GetInstance()->SimulateInputEvent(keyEvent);
    return RET_OK;
}

int32_t KeyboardManagerCommand::ParamLongCommand(int32_t& argc, char *argv[],
    char* &optarg, int32_t &optind, int32_t &pressTimeMs)
{
    if (argc < 4) {
        std::cout << "argc:" << argc << std::endl;
        std::cout << "wrong number of parameters" << std::endl;
        return RET_ERR;
    }
    if (argc >= 4) {
        if (!StrToInt(optarg, keyCode_)) {
            std::cout << "invalid key code value" << std::endl;
            return RET_ERR;
        }
    }
    if (argc >= 5) {
        if (!StrToInt(argv[optind], pressTimeMs)) {
            std::cout << "invalid key code value or press time" << std::endl;
            return RET_ERR;
        }
    }
    const int32_t minKeyCode = 0;
    const int32_t maxKeyCode = 5000;
    if ((keyCode_ < minKeyCode) || (keyCode_ > maxKeyCode)) {
        std::cout << "key code is out of range:" << minKeyCode << " <= "
            << keyCode_ << " <= " << maxKeyCode << std::endl;
        return RET_ERR;
    }
    const int32_t minPressTimeMs = 3000;
    const int32_t maxPressTimeMs = 15000;
    if ((pressTimeMs < minPressTimeMs) || (pressTimeMs > maxPressTimeMs)) {
        std::cout << "press time is out of range:" << minPressTimeMs << " ms" << " <= "
            << pressTimeMs << " <= " << maxPressTimeMs << " ms" << std::endl;
        return RET_ERR;
    }
    std::cout << " key code: " << keyCode_ << std::endl
        << "long press time: " << pressTimeMs << " ms" << std::endl;
    return RET_OK;
}

int32_t KeyboardManagerCommand::IntervalCommand(char* &optarg, int32_t &optind)
{
    int32_t taktTime = 0;
    if (!StrToInt(optarg, taktTime)) {
        std::cout << "invalid command to interval time" << std::endl;
        return EVENT_REG_FAIL;
    }
    const int64_t minTaktTimeMs = 1;
    const int64_t maxTaktTimeMs = 15000;
    if ((minTaktTimeMs > taktTime) || (maxTaktTimeMs < taktTime)) {
        std::cout << "taktTime is error" << std::endl;
        std::cout << minTaktTimeMs << " < taktTime < " << maxTaktTimeMs;
        std::cout << std::endl;
        return EVENT_REG_FAIL;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(taktTime));
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
