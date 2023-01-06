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

#include "mouse_manager_command.h"

#include <iostream>

#include "string_ex.h"

#include "error_multimodal.h"
#include "define_multimodal.h"
#include "input_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MouseManagerCommand" };
struct option mouseSensorOptions[] = {
    {"move", required_argument, NULL, 'm'},
    {"click", required_argument, NULL, 'c'},
    {"double_click", required_argument, NULL, 'b'},
    {"down", required_argument, NULL, 'd'},
    {"up", required_argument, NULL, 'u'},
    {"scroll", required_argument, NULL, 's'},
    {"drag", required_argument, NULL, 'g'},
    {"interval", required_argument, NULL, 'i'},
    {NULL, 0, NULL, 0}
};
} // namespace

int32_t MouseManagerCommand::CommandHandle(int32_t argc, char *argv[], char* &optarg,
    int32_t &optind, int32_t &optionIndex)
{
    int c = 0;
    while ((c = getopt_long(argc, argv, "m:d:u:c:b:s:g:i:", mouseSensorOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'm': {
                RETCASE(MouseManagerCommand::MoveCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            case 'd':
            case 'u': {
                RETCASE(MouseManagerCommand::UpDownCommand(c, optarg, optind), EVENT_REG_FAIL);
                break;
            }
            case 's': {
                RETCASE(MouseManagerCommand::ScrollCommand(optarg), EVENT_REG_FAIL);
                break;
            }
            case 'c': {
                RETCASE(MouseManagerCommand::ClickCommand(optarg), EVENT_REG_FAIL);
                break;
            }
            case 'b': {
                RETCASE(MouseManagerCommand::DoubleClickCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            case 'g': {
                RETCASE(MouseManagerCommand::DoubleClickCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            case 'i': {
                RETCASE(MouseManagerCommand::IntervalCommand(optarg), EVENT_REG_FAIL);
                break;
            }
            default: {
                std::cout << "invalid command to virtual mouse" << std::endl;
                manager.ShowUsage();
                return EVENT_REG_FAIL;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
    }
    return RET_OK;
}

int32_t MouseManagerCommand::MoveCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind)
{
    if (argc - optind < 1) {
        std::cout << "too few arguments to function" << std::endl;
        return RET_ERR;
    }
    if (!TraceMode(argc - optind + 1, &argv[optind - 1])) {
        if (!StrToInt(optarg, px_) || !StrToInt(argv[optind], py_)) {
            std::cout << "invalid parameter to move mouse" << std::endl;
            return RET_ERR;
        }
        if ((px_ < 0) || (py_ < 0)) {
            std::cout << "Coordinate value must be greater than 0" << std::endl;
            return RET_ERR;
        }
        std::cout << "move to " << px_ << " " << py_ << std::endl;
        std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
        CHKPR(pointerEvent, ERROR_NULL_POINTER);
        PointerEvent::PointerItem item;
        SetMoveEvent(px_, py_, item, pointerEvent);
        optind++;
    } else {
        RETCASE(MouseManagerCommand::NoTraceMode(argc, argv, optarg, optind), RET_ERR);
    }
    return RET_OK;
}

int32_t MouseManagerCommand::NoTraceMode(int32_t &argc, char *argv[], char* &optarg, int32_t &optind)
{
    struct Param param;
    RETCASE(MouseManagerCommand::NoTraceModeParam(argc, argv, optarg, optind, param), RET_ERR);
    static const int64_t minTotalTimeMs = 1;
    static const int64_t maxTotalTimeMs = 15000;
    if ((param.totalTimeMs < minTotalTimeMs) || (param.totalTimeMs > maxTotalTimeMs)) {
        std::cout << "total time is out of range:"
            << minTotalTimeMs << " <= " << param.totalTimeMs << " <= " << maxTotalTimeMs
            << std::endl;
        return RET_ERR;
    }
    std::cout << "start coordinate: (" << param.px1 << ", "  << param.py1 << ")" << std::endl;
    std::cout << "  end coordinate: (" << param.px2 << ", "  << param.py2 << ")" << std::endl;
    std::cout << "     total times: "  << param.totalTimeMs  << " ms"      << std::endl;
    std::cout << "      trace mode: " << std::boolalpha << param.foundTraceOption << std::endl;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    SetMoveEvent(param.px1, param.py1, item, pointerEvent);

    int64_t startTimeUs = GetSysClockTime();
    int64_t startTimeMs = startTimeUs / 1000;
    int64_t endTimeMs = 0;
    if (!AddInt64(startTimeMs, param.totalTimeMs, endTimeMs)) {
        std::cout << "system time error" << std::endl;
        return RET_ERR;
    }
    int64_t currentTimeMs = startTimeMs;
    while (currentTimeMs < endTimeMs) {
        int32_t px = manager.NextPos(startTimeMs, currentTimeMs, param.totalTimeMs, param.px1, param.px2);
        int32_t py = manager.NextPos(startTimeMs, currentTimeMs, param.totalTimeMs, param.py1, param.py2);
        SetMoveEvent(px, py, item, pointerEvent, currentTimeMs);
        manager.SleepAndUpdateTime(currentTimeMs);
    }

    SetMoveEvent(param.px2, param.py2, item, pointerEvent, currentTimeMs);
    return RET_OK;
}

int32_t MouseManagerCommand::NoTraceModeParam(int32_t &argc, char *argv[], char* &optarg, int32_t &optind, Param &param)
{
    if (argc - optind >= 3) {
        if ((!StrToInt(optarg, param.px1)) ||
            (!StrToInt(argv[optind], param.py1)) ||
            (!StrToInt(argv[optind + 1], param.px2)) ||
            (!StrToInt(argv[optind + 2], param.py2))) {
                std::cout << "invalid coordinate value" << std::endl;
                return RET_ERR;
        }
        optind += 3;
    }
    if ((param.px1 < 0) || (param.py1 < 0) || (param.px2 < 0) || (param.py2 < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    if (argc - optind >= 1) {
        std::string arg5 = argv[optind];
        if (!arg5.empty() && arg5.at(0) == '-') {
            if (IsTraceOption(arg5)) {
                param.foundTraceOption = true;
            } else {
                std::cout << "invalid option, the 5th position parameter must be --trace"
                    << std::endl;
                return RET_ERR;
            }
        } else if (!StrToInt(arg5, param.totalTimeMs)) {
            std::cout << "invalid total times" << std::endl;
            return RET_ERR;
        }
        optind++;
    }
    if (!param.foundTraceOption) {
        if (argc - optind < 1) {
            std::cout << "missing 6th position parameter --trace" << std::endl;
            return RET_ERR;
        }
        std::string arg6 = argv[optind];
        if (!IsTraceOption(arg6)) {
            std::cout << "invalid option, the 6th position parameter must be --trace"
                << std::endl;
            return RET_ERR;
        }
        optind++;
        param.foundTraceOption = true;
    }
    return RET_OK;
}

int32_t MouseManagerCommand::UpDownCommand(int32_t c, char* &optarg, int32_t &optind)
{
    if (c != static_cast<int32_t>('d') &&  c != static_cast<int32_t>('u')) {
        return EVENT_REG_FAIL;
    }
    auto typeMatch = MatchCommand(c, "press", "raise");
    if (!StrToInt(optarg, buttonId_) && buttonId_ > MOUSE_ID) {
        std::cout << "invalid " << typeMatch << " button command" << std::endl;
        return EVENT_REG_FAIL;
    }
    typeMatch = MatchCommand(c, "press down ", "lift up button ");
    std::cout <<  typeMatch << buttonId_ << std::endl;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(px_);
    item.SetDisplayY(py_);
    bool state = MatchCommand(c, true, false);
    item.SetPressed(state);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonId(buttonId_);
    pointerEvent->SetButtonPressed(buttonId_);
    int32_t action = MatchCommand(c, PointerEvent::POINTER_ACTION_BUTTON_DOWN, PointerEvent::POINTER_ACTION_BUTTON_UP);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t MouseManagerCommand::ScrollCommand(char* &optarg)
{
    if (!StrToInt(optarg, scrollValue_)) {
        std::cout << "invalid  scroll button command" << std::endl;
        return EVENT_REG_FAIL;
    }
    std::cout << "scroll wheel " << scrollValue_ << std::endl;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(px_);
    item.SetDisplayY(py_);
    item.SetPressed(false);
    int64_t time = pointerEvent->GetActionStartTime();
    pointerEvent->SetActionTime(time + ACTION_TIME);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_BEGIN);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_SCROLL_VERTICAL,
        scrollValue_);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    time = pointerEvent->GetActionStartTime();
    time = pointerEvent->GetActionStartTime();
    pointerEvent->SetActionTime(time + ACTION_TIME);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_UPDATE);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_SCROLL_VERTICAL,
        scrollValue_);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

    time = pointerEvent->GetActionStartTime();
    pointerEvent->SetActionTime(time + ACTION_TIME);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_END);
    pointerEvent->SetAxisValue(PointerEvent::AxisType::AXIS_TYPE_SCROLL_VERTICAL, scrollValue_);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t MouseManagerCommand::ClickCommand(char* &optarg)
{
    if (!StrToInt(optarg, buttonId_) && buttonId_ > MOUSE_ID) {
        std::cout << "invalid click button command" << std::endl;
        return EVENT_REG_FAIL;
    }
    std::cout << "click   " << buttonId_ << std::endl;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetPressed(true);
    item.SetDisplayX(px_);
    item.SetDisplayY(py_);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonId(buttonId_);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    item.SetPointerId(0);
    item.SetPressed(false);
    item.SetDisplayX(px_);
    item.SetDisplayY(py_);
    pointerEvent->SetPointerId(0);
    pointerEvent->UpdatePointerItem(0, item);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetButtonId(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t MouseManagerCommand::DoubleClickCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind)
{
    if (argc < 6 || argc > 8) {
        std::cout << "wrong number of parameters" << std::endl;
        return RET_ERR;
    }
    int32_t pressTimeMs = 50;
    int32_t clickIntervalTimeMs = 300;
    RETCASE(MouseManagerCommand::ParamDoubleClickCommand(argc, argv, optarg, optind, pressTimeMs, clickIntervalTimeMs),
        RET_ERR);
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, RET_ERR);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetPressed(true);
    item.SetDisplayX(px_);
    item.SetDisplayY(py_);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonId(buttonId_);
    pointerEvent->SetButtonPressed(buttonId_);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(pressTimeMs));
    item.SetPressed(false);
    pointerEvent->UpdatePointerItem(0, item);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(clickIntervalTimeMs));
    item.SetPressed(true);
    pointerEvent->UpdatePointerItem(0, item);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(pressTimeMs));
    item.SetPressed(false);
    pointerEvent->UpdatePointerItem(0, item);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t MouseManagerCommand::ParamDoubleClickCommand(int32_t &argc, char *argv[], char* &optarg,
    int32_t &optind, int32_t pressTimeMs, int32_t clickIntervalTimeMs)
{
    if (!StrToInt(optarg, px_) || !StrToInt(argv[optind], py_)) {
        std::cout << "invalid coordinate value" << std::endl;
        return RET_ERR;
    }
    if ((px_ < 0) || (py_ < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    if (!StrToInt(argv[optind + 1], buttonId_)) {
        std::cout << "invalid key" << std::endl;
        return RET_ERR;
    }
    int32_t minButtonId = 0;
    int32_t maxButtonId = 7;
    int32_t minPressTimeMs = 1;
    int32_t maxPressTimeMs = 300;
    int32_t minClickIntervalTimeMs = 1;
    int32_t maxClickIntervalTimeMs = 450;
    if (argc >= 7 && !StrToInt(argv[optind + 2], pressTimeMs)) {
            std::cout << "invalid press time" << std::endl;
            return RET_ERR;
    }
    if (argc == 8 && !StrToInt(argv[optind + 3], clickIntervalTimeMs)) {
            std::cout << "invalid interval between hits" << std::endl;
            return RET_ERR;
    }
    if ((buttonId_ < minButtonId) || (buttonId_ > maxButtonId)) {
        std::cout << "button is out of range:" << minButtonId << " < " << buttonId_ << " < "
            << maxButtonId << std::endl;
        return RET_ERR;
    }
    if ((pressTimeMs < minPressTimeMs) || (pressTimeMs > maxPressTimeMs)) {
        std::cout << "press time is out of range:" << minPressTimeMs << " ms" << " < "
            << pressTimeMs << " < " << maxPressTimeMs << " ms" << std::endl;
        return RET_ERR;
    }
    if ((clickIntervalTimeMs < minClickIntervalTimeMs) || (clickIntervalTimeMs > maxClickIntervalTimeMs)) {
        std::cout << "click interval time is out of range:" << minClickIntervalTimeMs << " ms"
            " < " << clickIntervalTimeMs << " < " << maxClickIntervalTimeMs << " ms"
            << std::endl;
    }
    std::cout << "   coordinate: ("<< px_ << ", "  << py_ << ")" << std::endl;
    std::cout << "    button id: " << buttonId_    << std::endl;
    std::cout << "   press time: " << pressTimeMs << " ms" << std::endl;
    std::cout << "interval time: " << clickIntervalTimeMs  << " ms" << std::endl;
    return RET_OK;
}

int32_t MouseManagerCommand::ParamDragParam(int32_t &argc, char *argv[], char* &optarg, int32_t &optind, Param &param)
{
    if (argc < 7) {
        std::cout << "argc:" << argc << std::endl;
        std::cout << "Wrong number of parameters" << std::endl;
        return RET_ERR;
    }
    if (argc >= 7) {
        if ((!StrToInt(optarg, param.px1)) ||
            (!StrToInt(argv[optind], param.py1)) ||
            (!StrToInt(argv[optind + 1], param.px2)) ||
            (!StrToInt(argv[optind + 2], param.py2))) {
                std::cout << "Invalid coordinate value" << std::endl;
                return RET_ERR;
        }
    }
    if ((param.px1 < 0) || (param.py1 < 0) || (param.px2 < 0) || (param.py2 < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    if (argc >= 8) {
        if (!StrToInt(argv[optind + 3], param.totalTimeMs)) {
            std::cout << "Invalid total times" << std::endl;
            return RET_ERR;
        }
    }
    static const int64_t minTotalTimeMs = 1;
    static const int64_t maxTotalTimeMs = 15000;
    if ((param.totalTimeMs < minTotalTimeMs) || (param.totalTimeMs > maxTotalTimeMs)) {
        std::cout << "Total time is out of range:"
            << minTotalTimeMs << "ms" << " <= " << param.totalTimeMs << "ms" << " <= "
            << maxTotalTimeMs << "ms" << std::endl;
        return RET_ERR;
    }
    std::cout << "start coordinate: (" << param.px1 << ", "  << param.py1 << ")" << std::endl;
    std::cout << "  end coordinate: (" << param.px2 << ", "  << param.py2 << ")" << std::endl;
    std::cout << "      total time: "  << param.totalTimeMs  << "ms"       << std::endl;
    return RET_OK;
}

int32_t MouseManagerCommand::DragCommand(int32_t &argc, char *argv[], char* &optarg, int32_t &optind)
{
    struct Param param;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(param.px1);
    item.SetDisplayY(param.py1);
    item.SetPressed(false);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetButtonPressed(0);
    int32_t buttonId = 0;
    pointerEvent->SetButtonId(buttonId);
    pointerEvent->SetButtonPressed(buttonId);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

    int64_t startTimeMs = GetSysClockTime() / 1000;
    int64_t endTimeMs = 0;
    if (!AddInt64(startTimeMs, param.totalTimeMs, endTimeMs)) {
        std::cout << "System time error" << std::endl;
        return RET_ERR;
    }
    auto setEvent = [pointerEvent](int32_t px, int32_t py, int32_t timeMs,
        PointerEvent::PointerItem &item, int32_t action) {
            item.SetDisplayX(px);
            item.SetDisplayY(py);
            pointerEvent->SetActionTime(timeMs);
            pointerEvent->UpdatePointerItem(0, item);
            pointerEvent->SetPointerAction(action);
            InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
        };
    int64_t currentTimeMs = startTimeMs;
    while (currentTimeMs < endTimeMs) {
        int32_t px = manager.NextPos(startTimeMs, currentTimeMs, param.totalTimeMs, param.px1, param.px2);
        int32_t py = manager.NextPos(startTimeMs, currentTimeMs, param.totalTimeMs, param.py1, param.py2);
        setEvent(px, py, currentTimeMs, item, PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        manager.SleepAndUpdateTime(currentTimeMs);
    }
    setEvent(param.px2, param.py2, endTimeMs, item, PointerEvent::POINTER_ACTION_MOVE);
    std::this_thread::sleep_for(std::chrono::milliseconds(BLOCK_TIME_MS));

    item.SetPressed(true);
    setEvent(param.px2, param.py2, endTimeMs, item, PointerEvent::POINTER_ACTION_BUTTON_UP);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t MouseManagerCommand::IntervalCommand(char* &optarg)
{
    int32_t taktTime = 0;
    if (!StrToInt(optarg, taktTime)) {
    std::cout << "invalid command to interval time" << std::endl;
        return EVENT_REG_FAIL;
    }
    const int64_t minTaktTimeMs = 1;
    const int64_t maxTaktTimeMs = 15000;
    if ((minTaktTimeMs > taktTime) || (maxTaktTimeMs < taktTime)) {
        std::cout << "taktTime is out of range" << std::endl;
        std::cout << minTaktTimeMs << " < taktTime < " << maxTaktTimeMs;
        std::cout << std::endl;
        return EVENT_REG_FAIL;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(taktTime));
    return RET_OK;
}

bool MouseManagerCommand::IsTraceOption(const std::string &opt1)
{
    return opt1 == std::string("--trace");
}

bool MouseManagerCommand::TraceMode(int32_t argCount, char *argvOffset[])
{
    if (argCount <= 3) {
        return false;
    }
    std::string arg3 = argvOffset[2];
    if (!arg3.empty() && arg3.at(0) == '-') {
        return false;
    }
    if ((argCount >= 5) && IsTraceOption(std::string(argvOffset[4]))) {
        return true;
    }
    if ((argCount >= 6) && IsTraceOption(std::string(argvOffset[5]))) {
        return true;
    }
    return false;
}

void MouseManagerCommand::SetMoveEvent(int32_t px, int32_t py, PointerEvent::PointerItem &item,
    std::shared_ptr<PointerEvent>& pointerEvent, int64_t timeMs)
{
    if (px < 0 || py < 0) {
        return;
    }
    item.SetDisplayX(px);
    item.SetDisplayY(py);
    if (timeMs < 0) {
        item.SetPointerId(0);
        pointerEvent->AddPointerItem(item);
        pointerEvent->SetPointerId(0);
        pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    } else {
        pointerEvent->SetActionTime(timeMs);
        pointerEvent->UpdatePointerItem(0, item);
    }
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}
} // namespace MMI
} // namespace OHOS