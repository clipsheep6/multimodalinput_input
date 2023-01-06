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

#include "touch_manager_command.h"

#include <iostream>

#include "string_ex.h"

#include "error_multimodal.h"
#include "define_multimodal.h"
#include "input_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchManagerCommand" };
constexpr int32_t TIMECONVERSION = 1000;
struct option touchSensorOptions[] = {
    {"move", required_argument, NULL, 'm'},
    {"down", required_argument, NULL, 'd'},
    {"up", required_argument, NULL, 'u'},
    {"click", required_argument, NULL, 'c'},
    {"interval", required_argument, NULL, 'i'},
    {"drag", required_argument, NULL, 'g'},
    {NULL, 0, NULL, 0}
};
} // namespace

int32_t TouchManagerCommand::CommandHandle(int32_t argc, char *argv[], char* &optarg,
    int32_t &optind, int32_t &optionIndex)
{
    int32_t c = 0;
    while ((c = getopt_long(argc, argv, "m:d:u:c:i:g:", touchSensorOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'm': {
                RETCASE(TouchManagerCommand::MoveCommand(argc, argv, optarg, optind), EVENT_REG_FAIL);
                break;
            }
            case 'd':
            case 'u': {
                RETCASE(TouchManagerCommand::UpDownCommand(argc, argv, c, optarg, optind), EVENT_REG_FAIL);
                break;
            }
            case 'c': {
                RETCASE(TouchManagerCommand::ClickCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            case 'i': {
                RETCASE(TouchManagerCommand::IntervalCommand(optarg), EVENT_REG_FAIL);
                break;
            }
            case 'g': {
                RETCASE(TouchManagerCommand::DragCommand(argc, argv, optarg, optind), RET_ERR);
                break;
            }
            default: {
                std::cout << "invalid command" << std::endl;
                manager.ShowUsage();
                return EVENT_REG_FAIL;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
    }
    return RET_OK;
}

int32_t TouchManagerCommand::MoveCommand(int32_t &argc, char *argv[], char* &optarg, int32_t &optind)
{
    RETCASE(TouchManagerCommand::ParamMoveCommand(argc, argv, optarg, optind), EVENT_REG_FAIL);
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    SetTouchEvent(px1_, py1_, PointerEvent::POINTER_ACTION_DOWN, item, pointerEvent);

    int64_t startTimeUs = pointerEvent->GetActionStartTime();
    int64_t startTimeMs = startTimeUs / TIMECONVERSION;
    int64_t endTimeMs = 0;
    if (!AddInt64(startTimeMs, totalTimeMs_, endTimeMs)) {
        std::cout << "system time error." << std::endl;
        return EVENT_REG_FAIL;
    }
    int64_t currentTimeMs = startTimeMs;
    int64_t nowSysTimeUs = 0;
    int64_t nowSysTimeMs = 0;
    int64_t sleepTimeMs = 0;
    while (currentTimeMs < endTimeMs) {
        int32_t px = manager.NextPos(startTimeMs, currentTimeMs, totalTimeMs_, px1_, px2_);
        int32_t py = manager.NextPos(startTimeMs, currentTimeMs, totalTimeMs_, py1_, py2_);
        SetTouchEvent(px, py, PointerEvent::POINTER_ACTION_MOVE, item, pointerEvent, currentTimeMs * TIMECONVERSION);
        nowSysTimeUs = GetSysClockTime();
        nowSysTimeMs = nowSysTimeUs / TIMECONVERSION;
        sleepTimeMs = (currentTimeMs + BLOCK_TIME_MS) - nowSysTimeMs;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
        currentTimeMs += BLOCK_TIME_MS;
    }

    SetTouchEvent(px2_, py2_, PointerEvent::POINTER_ACTION_MOVE, item, pointerEvent, endTimeMs * TIMECONVERSION);
    std::this_thread::sleep_for(std::chrono::milliseconds(BLOCK_TIME_MS));

    int64_t blockTime = (endTimeMs + BLOCK_TIME_MS) * TIMECONVERSION;
    SetTouchEvent(px2_, py2_, PointerEvent::POINTER_ACTION_UP, item, pointerEvent, blockTime);
    optind =  optind + THREE_MORE_COMMAND;
    return RET_OK;
}

int32_t TouchManagerCommand::ParamMoveCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind)
{
    if (argc < moveArgcSeven_) {
        std::cout << "argc:" << argc << std::endl;
        std::cout << "wrong number of parameters" << std::endl;
        return EVENT_REG_FAIL;
    }
    if (argv[optind + 3] == nullptr || argv[optind + 3][0] == '-') {
        totalTimeMs_ = 1000;
        if ((!StrToInt(optarg, px1_)) ||
            (!StrToInt(argv[optind], py1_)) ||
            (!StrToInt(argv[optind + 1], px2_)) ||
            (!StrToInt(argv[optind + 2], py2_))) {
                std::cout << "invalid coordinate value" << std::endl;
                return EVENT_REG_FAIL;
        }
    } else {
        if ((!StrToInt(optarg, px1_)) ||
            (!StrToInt(argv[optind], py1_)) ||
            (!StrToInt(argv[optind + 1], px2_)) ||
            (!StrToInt(argv[optind + 2], py2_)) ||
            (!StrToInt(argv[optind + 3], totalTimeMs_))) {
                std::cout << "invalid coordinate value or total times" << std::endl;
                return EVENT_REG_FAIL;
        }
    }
    if ((px1_ < 0) || (py1_ < 0) || (px2_ < 0) || (py2_ < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    const int64_t minTotalTimeMs = 1;
    const int64_t maxTotalTimeMs = 15000;
    if ((totalTimeMs_ < minTotalTimeMs) || (totalTimeMs_ > maxTotalTimeMs)) {
        std::cout << "total time is out of range:" << std::endl;
        std::cout << minTotalTimeMs << " <= " << "total times" << " <= " << maxTotalTimeMs;
        std::cout << std::endl;
        return EVENT_REG_FAIL;
    }
    std::cout << "start coordinate: ("<< px1_ << ", "  << py1_ << ")" << std::endl;
    std::cout << "  end coordinate: ("<< px2_ << ", "  << py2_ << ")" << std::endl;
    std::cout << "     total times: " << totalTimeMs_ << " ms" << std::endl;
    return RET_OK;
}

void TouchManagerCommand::SetTouchEvent(int32_t px, int32_t py, int32_t action, PointerEvent::PointerItem &item,
    std::shared_ptr<PointerEvent>& pointerEvent, int64_t timeMs)
{
    if (px < 0 || py < 0) {
        return;
    }
    if (timeMs < 0) {
        item.SetDisplayX(px);
        item.SetDisplayY(py);
        item.SetPointerId(0);
        pointerEvent->AddPointerItem(item);
        pointerEvent->SetPointerId(0);
        pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    } else {
        item.SetDisplayX(px);
        item.SetDisplayY(py);
        pointerEvent->SetActionTime(timeMs);
        pointerEvent->UpdatePointerItem(0, item);
    }
    pointerEvent->SetPointerAction(action);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

int32_t TouchManagerCommand::UpDownCommand(int32_t argc, char *argv[], int32_t c, char* &optarg, int32_t &optind)
{
    if (argc < 0 || (c != static_cast<int32_t>('d') &&  c != static_cast<int32_t>('u'))) {
        return EVENT_REG_FAIL;
    }
    if (optind >= argc) {
        std::cout << "too few arguments to function" << std::endl;
        return EVENT_REG_FAIL;
    }
    if (!StrToInt(optarg, px1_) ||
        !StrToInt(argv[optind], py1_)) {
        std::cout << "invalid coordinate value" << std::endl;
        return EVENT_REG_FAIL;
    }
    if ((px1_ < 0) || (py1_ < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    auto typeMatch = MatchCommand(c, "down ", "up ");
    std::cout << "touch " << typeMatch << px1_ << " " << py1_ << std::endl;
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(px1_);
    item.SetDisplayY(py1_);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    int32_t action = MatchCommand(c, PointerEvent::POINTER_ACTION_DOWN, PointerEvent::POINTER_ACTION_UP);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return RET_OK;
}

int32_t TouchManagerCommand::ClickCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind)
{
    if (argc < 0) {
        return RET_ERR;
    }
    int32_t intervalTimeMs = 0;
    if (argc == 5) {
        if (!StrToInt(optarg, px1_) ||
            !StrToInt(argv[optind], py1_)) {
            std::cout << "input coordinate error" << std::endl;
            return RET_ERR;
        }
        intervalTimeMs = 100;
    } else if (argc == 6) {
        if (!StrToInt(optarg, px1_) ||
            !StrToInt(argv[optind], py1_) ||
            !StrToInt(argv[optind + 1], intervalTimeMs)) {
            std::cout << "input coordinate or time error" << std::endl;
            return RET_ERR;
        }
        const int64_t minIntervalTimeMs = 1;
        const int64_t maxIntervalTimeMs = 450;
        if ((minIntervalTimeMs > intervalTimeMs) || (maxIntervalTimeMs < intervalTimeMs)) {
            std::cout << "interval time is out of range: " << minIntervalTimeMs << "ms";
            std::cout << " < interval time < " << maxIntervalTimeMs << "ms" << std::endl;
            return RET_ERR;
        }
    } else {
        std::cout << "parameter error, unable to run" << std::endl;
        return RET_ERR;
    }
    if ((px1_ < 0) || (py1_ < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    std::cout << "   click coordinate: ("<< px1_ << ", "  << py1_ << ")" << std::endl;
    std::cout << "click interval time: " << intervalTimeMs      << "ms" << std::endl;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    item.SetPressed(true);
    SetTouchEvent(px1_, py1_, PointerEvent::POINTER_ACTION_DOWN, item, pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalTimeMs));

    item.SetPressed(false);
    SetTouchEvent(px1_, py1_, PointerEvent::POINTER_ACTION_UP, item, pointerEvent);
    return RET_OK;
}

int32_t TouchManagerCommand::IntervalCommand(char* &optarg)
{
    int32_t taktTime = 0;
    if (!StrToInt(optarg, taktTime)) {
        std::cout << "invalid command to interval time" << std::endl;
        return EVENT_REG_FAIL;
    }
    const int64_t minTaktTimeMs = 1;
    const int64_t maxTaktTimeMs = 15000;
    if ((minTaktTimeMs > taktTime) || (maxTaktTimeMs < taktTime)) {
        std::cout << "taktTime is out of range. ";
        std::cout << minTaktTimeMs << " < taktTime < " << maxTaktTimeMs;
        std::cout << std::endl;
        return EVENT_REG_FAIL;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(taktTime));
    return RET_OK;
}

int32_t TouchManagerCommand::DragCommand(int32_t& argc, char *argv[], char* &optarg, int32_t &optind)
{
    int32_t pressTimems = 500;
    RETCASE(TouchManagerCommand::ParamDragCommand(argc, argv, optarg, optind, pressTimems), RET_ERR);
    const int32_t minTotalTimeMs = 1000;
    const int32_t maxTotalTimeMs = 15000;
    if ((minTotalTimeMs > totalTimeMs_) || (maxTotalTimeMs < totalTimeMs_)) {
        std::cout << "total time input is error" << std::endl;
        return RET_ERR;
    }
    const int32_t minPressTimeMs = 500;
    const int32_t maxPressTimeMs = 14500;
    if ((minPressTimeMs > pressTimems) || (maxPressTimeMs < pressTimems)) {
        std::cout << "press time is out of range" << std::endl;
        return RET_ERR;
    }
    const int32_t minMoveTimeMs = 500;
    if ((totalTimeMs_ -  pressTimems) <  minMoveTimeMs) {
        std::cout << "move time is out of range" << std::endl;
        return RET_ERR;
    }
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    PointerEvent::PointerItem item;
    SetTouchEvent(px1_, py1_, PointerEvent::POINTER_ACTION_DOWN, item, pointerEvent);
    const int32_t conversionRate = 1000;
    int64_t startTimeMs = GetSysClockTime() / conversionRate;
    int64_t endTimeMs = 0;
    if (!AddInt64(startTimeMs, totalTimeMs_, endTimeMs)) {
        std::cout << "end time count error" << std::endl;
        return RET_ERR;
    }
    int64_t downTimeMs = 0;
    if (!AddInt64(startTimeMs, pressTimems, downTimeMs)) {
        std::cout << "down time count error" << std::endl;
        return RET_ERR;
    }
    int64_t currentTimeMs = startTimeMs;
    const int32_t moveTimeMs = totalTimeMs_ - pressTimems;
    while ((currentTimeMs < endTimeMs)) {
        if (currentTimeMs > downTimeMs) {
            int32_t px = manager.NextPos(downTimeMs, currentTimeMs, moveTimeMs, px1_, px2_);
            int32_t py = manager.NextPos(downTimeMs, currentTimeMs, moveTimeMs, py1_, py2_);
            SetTouchEvent(px, py, PointerEvent::POINTER_ACTION_MOVE, item, pointerEvent, currentTimeMs);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(BLOCK_TIME_MS));
        currentTimeMs = GetSysClockTime() / conversionRate;
    }
    SetTouchEvent(px2_, py2_, PointerEvent::POINTER_ACTION_UP, item, pointerEvent, endTimeMs);
    return RET_OK;
}

int32_t TouchManagerCommand::ParamDragCommand(int32_t& argc, char *argv[],
    char* &optarg, int32_t &optind, int32_t &pressTimems)
{
    const int32_t dragArgcSeven = 7;
    const int32_t dragArgcCommandNine = 9;
    if ((argc != dragArgcSeven) && (argc != dragArgcCommandNine)) {
        std::cout << "argc:" << argc << std::endl;
        std::cout << "wrong number of parameters" << std::endl;
        return RET_ERR;
    }
    totalTimeMs_ = 1000;
    if (argc == moveArgcSeven_) {
        if ((!StrToInt(optarg, px1_)) ||
            (!StrToInt(argv[optind], py1_)) ||
            (!StrToInt(argv[optind + 1], px2_)) ||
            (!StrToInt(argv[optind + 2], py2_))) {
                std::cout << "invalid coordinate value" << std::endl;
                return RET_ERR;
        }
    } else {
        if ((!StrToInt(optarg, px1_)) ||
            (!StrToInt(argv[optind], py1_)) ||
            (!StrToInt(argv[optind + 1], px2_)) ||
            (!StrToInt(argv[optind + 2], py2_)) ||
            (!StrToInt(argv[optind + 3], pressTimems)) ||
            (!StrToInt(argv[optind + 4], totalTimeMs_))) {
                std::cout << "invalid input coordinate or time" << std::endl;
                return RET_ERR;
        }
    }
    if ((px1_ < 0) || (py1_ < 0) || (px2_ < 0) || (py2_ < 0)) {
        std::cout << "Coordinate value must be greater than 0" << std::endl;
        return RET_ERR;
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS