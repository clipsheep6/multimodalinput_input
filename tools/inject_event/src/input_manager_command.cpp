/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "input_manager_command.h"

#include <getopt.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <thread>

#include <sys/time.h>
#include <unistd.h>

#include "error_multimodal.h"
#include "mmi_log.h"
#include "multimodal_event_handler.h"
#include "util.h"

#include "i_input_command.h"
#include "input_device_manger.h"

class InputManagerCommand {
public:
    int32_t ParseCommand(int32_t argc, char *argv[]);
    int32_t ConnectService();
    void ShowUsage();
private:
    void InitializeMouseDeathStub();
};
namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerCommand" };
} // namespace

void InputManagerCommand::SleepAndUpdateTime(int64_t &currentTimeMs)
{
    int64_t nowEndSysTimeMs = GetSysClockTime() / 1000;
    int64_t sleepTimeMs = BLOCK_TIME_MS - (nowEndSysTimeMs - currentTimeMs) % BLOCK_TIME_MS;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
    currentTimeMs = nowEndSysTimeMs + sleepTimeMs;
}

int32_t InputManagerCommand::NextPos(int64_t begTimeMs, int64_t curtTimeMs, int32_t totalTimeMs,
    int32_t begPos, int32_t endPos)
{
    int64_t endTimeMs = 0;
    if (!AddInt64(begTimeMs, totalTimeMs, endTimeMs)) {
        return begPos;
    }
    if (curtTimeMs < begTimeMs || curtTimeMs > endTimeMs) {
        return begPos;
    }
    if (totalTimeMs == 0) {
        std::cout << "invalid totalTimeMs" << std::endl;
        return begPos;
    }
    double tmpTimeMs = static_cast<double>(curtTimeMs - begTimeMs) / totalTimeMs;
    int32_t offsetPos = std::ceil(tmpTimeMs * (endPos - begPos));
    int32_t retPos = 0;
    if (offsetPos == 0) {
        return begPos;
    } else if (offsetPos > 0) {
        if (!AddInt32(begPos, offsetPos, retPos)) {
            return begPos;
        }
        return retPos > endPos ? endPos : retPos;
    }
    if (!AddInt32(begPos, offsetPos, retPos)) {
        return begPos;
    }
    return retPos < endPos ? endPos : retPos;
}

int32_t InputManagerCommand::ParseCommand(int32_t argc, char *argv[])
{
    struct option headOptions[] = {
        {"mouse", no_argument, NULL, 'M'},
        {"keyboard", no_argument, NULL, 'K'},
        {"touch", no_argument, NULL, 'T'},
        {"joystick", no_argument, NULL, 'J'},
        {"help", no_argument, NULL, '?'},
        {NULL, 0, NULL, 0}
    };
    int32_t c = 0;
    int32_t optionIndex = 0;
    optind = 0;
    if ((c = getopt_long(argc, argv, "MKTJ?", headOptions, &optionIndex)) != -1) {
        InputDeviceManager deviceManager;
        auto device = deviceManager.CreatDevice(c);
        CHKPR(device, ERROR_NULL_POINTER);
        RETCASE(device->CommandHandle(argc, argv, optarg, optind, optionIndex), EVENT_REG_FAIL);
    } else {
        std::cout << "too few arguments to function" << std::endl;
        ShowUsage();
        return EVENT_REG_FAIL;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
    return ERR_OK;
}

void InputManagerCommand::ShowUsage()
{
    std::cout << "Usage: uinput <option> <command> <arg>..." << std::endl;
    std::cout << "The option are:                                " << std::endl;
    MouseShowUsage();
    keyboardShowUsage();
    TouchShowUsage();
    JoystickShowUsage();
    std::cout << "-?  --help                                                    " << std::endl;
}

void InputManagerCommand::MouseShowUsage()
{
    std::cout << "-M  --mouse                                    " << std::endl;
    std::cout << "commands for mouse:                            " << std::endl;
    std::cout << "-m <dx> <dy>              --move   <dx> <dy>  -move to relative position (dx,dy),"    << std::endl;
    std::cout << "   <dx1> <dy1> <dx2> <dy2> [smooth time] --trace -dx1 dy1 to dx2 dy2 smooth movement" << std::endl;
    std::cout << "-d <key>                  --down   key        -press down a button, "                 << std::endl;
    std::cout << "                                               0 is the left button, 1 is the right," << std::endl;
    std::cout << "                                               2 is the middle"   << std::endl;
    std::cout << "-u <key>                  --up     <key>      -release a button " << std::endl;
    std::cout << "-c <key>                  --click  <key>      -press the left button down,then raise" << std::endl;
    std::cout << "-b <dx1> <dy1> <id> [press time] [click interval time]                --double click" << std::endl;
    std::cout << "  [press time] the time range is more than 1ms but less than 300ms, "       << std::endl;
    std::cout << "  [click interval time] the time range is more than 1ms but less than 450ms, " << std::endl;
    std::cout << "  Otherwise the operation result may produce error or invalid operation"       << std::endl;
    std::cout << " -press the left button down,then raise" << std::endl;
    std::cout << "   key value:0 - button left"     << std::endl;
    std::cout << "   key value:1 - button right"    << std::endl;
    std::cout << "   key value:2 - button middle"   << std::endl;
    std::cout << "   key value:3 - button side"     << std::endl;
    std::cout << "   key value:4 - button extra"    << std::endl;
    std::cout << "   key value:5 - button forward"  << std::endl;
    std::cout << "   key value:6 - button back"     << std::endl;
    std::cout << "   key value:7 - button task"     << std::endl;
    std::cout << "-s <key>                  --scroll <key>      -positive values are sliding backwards" << std::endl;
    std::cout << "-g <dx1> <dy1> <dx2> <dy2> [total time]       --drag <dx1> <dy1> <dx2> <dy2> [total time],";
    std::cout << std::endl;
    std::cout << "                                              dx1 dy1 to dx2 dy2 smooth drag"         << std::endl;
    std::cout << "-i <time>                 --interval <time>   -the program interval for the (time) milliseconds";
    std::cout << std::endl;
    std::cout << "                                               negative values are sliding forwards"  << std::endl;
    std::cout << std::endl;
}

void InputManagerCommand::keyboardShowUsage()
{
    std::cout << "-K  --keyboard                                                " << std::endl;
    std::cout << "commands for keyboard:                                        " << std::endl;
    std::cout << "-d <key>                   --down   <key>     -press down a key" << std::endl;
    std::cout << "-u <key>                   --up     <key>     -release a key   " << std::endl;
    std::cout << "-l <key> [long press time] --long_press <key> [long press time] -press and hold the key";
    std::cout << std::endl;
    std::cout << "-i <time>                  --interval <time>  -the program interval for the (time) milliseconds";
    std::cout << std::endl;
}

void InputManagerCommand::TouchShowUsage()
{
    std::cout << "-T  --touch                                                   " << std::endl;
    std::cout << "commands for touch:                                           " << std::endl;
    std::cout << "-d <dx1> <dy1>             --down   <dx1> <dy1> -press down a position  dx1 dy1, " << std::endl;
    std::cout << "-u <dx1> <dy1>             --up     <dx1> <dy1> -release a position dx1 dy1, "     << std::endl;
    std::cout << "-m <dx1> <dy1> <dx2> <dy2> [smooth time]      --smooth movement"   << std::endl;
    std::cout << "   <dx1> <dy1> <dx2> <dy2> [smooth time]      -smooth movement, "  << std::endl;
    std::cout << "                                              dx1 dy1 to dx2 dy2 smooth movement"  << std::endl;
    std::cout << "-c <dx1> <dy1> [click interval]               -touch screen click dx1 dy1"         << std::endl;
    std::cout << "-i <time>                  --interval <time>  -the program interval for the (time) milliseconds";
    std::cout << std::endl;
    std::cout << "-g <dx1> <dy1> <dx2> <dy2> [press time] [total time]     -drag, "                       << std::endl;
    std::cout << "  [Press time] not less than 500ms and [total time] - [Press time] not less than 500ms" << std::endl;
    std::cout << "  Otherwise the operation result may produce error or invalid operation"                << std::endl;
    std::cout << std::endl;
}

void InputManagerCommand::JoystickShowUsage()
{
    std::cout << "-J  --joystick                                                " << std::endl;
    std::cout << "commands for joystick:                                        " << std::endl;
    std::cout << "-m x=<x>               --move x=<x>                 -<x> is the axis offset" << std::endl;
    std::cout << "   y=<y>                      y=<y>                 -<y> is the axis offset" << std::endl;
    std::cout << "   z=<z>                      z=<z>                 -<z> is the axis offset" << std::endl;
    std::cout << "   rz=<rz>                    rz=<rz>               -<rz> is the axis offset" << std::endl;
    std::cout << "   gas=<gas>                  gas=<gas>             -<gas> is the axis offset" << std::endl;
    std::cout << "   brake=<brake>              brake=<brake>         -<brake> is the axis offset" << std::endl;
    std::cout << "   hat0x=<hat0x>              hat0x=<hat0x>         -<hat0x> is the axis offset" << std::endl;
    std::cout << "   hat0y=<haty0>              hat0y=<haty0>         -<haty0> is the axis offset" << std::endl;
    std::cout << "   throttle=<throttle>        throttle=<throttle>   -<throttle> is the axis offset" << std::endl;
    std::cout << "-d <key>               --down <key>       -press down a button" << std::endl;
    std::cout << "-u <key>               --up <key>         -release a button" << std::endl;
    std::cout << "-c <key>               --click <key>      -press the button down,then raise";
    std::cout << " -joystick key value" << std::endl;
    std::cout << "   key value:0 - button tl2"     << std::endl;
    std::cout << "   key value:1 - button tr2"    << std::endl;
    std::cout << "   key value:2 - button tl"   << std::endl;
    std::cout << "   key value:3 - button tr"     << std::endl;
    std::cout << "   key value:4 - button west"    << std::endl;
    std::cout << "   key value:5 - button south"  << std::endl;
    std::cout << "   key value:6 - button notrh"     << std::endl;
    std::cout << "   key value:7 - button east"     << std::endl;
    std::cout << "   key value:8 - button start"     << std::endl;
    std::cout << "   key value:9 - button select"    << std::endl;
    std::cout << "   key value:10 - button homepage"   << std::endl;
    std::cout << "   key value:11 - button thumbl"     << std::endl;
    std::cout << "   key value:12 - button thumbr"    << std::endl;
    std::cout << "   key value:13 - button trigger"  << std::endl;
    std::cout << "   key value:14 - button thumb"     << std::endl;
    std::cout << "   key value:15 - button thumb2"     << std::endl;
    std::cout << "   key value:16 - button top"     << std::endl;
    std::cout << "   key value:17 - button top2"    << std::endl;
    std::cout << "   key value:18 - button pinkie"  << std::endl;
    std::cout << "   key value:19 - button base"     << std::endl;
    std::cout << "   key value:20 - button base2"     << std::endl;
    std::cout << "   key value:21 - button base3"     << std::endl;
    std::cout << "   key value:22 - button base4"    << std::endl;
    std::cout << "   key value:23 - button base5"   << std::endl;
    std::cout << "   key value:24 - button base6"     << std::endl;
    std::cout << "   key value:25 - button dead"    << std::endl;
    std::cout << "   key value:26 - button c"  << std::endl;
    std::cout << "   key value:27 - button z"     << std::endl;
    std::cout << "   key value:28 - button mode"     << std::endl;
    std::cout << "-i <time>              --interval <time>  -the program interval for the (time) milliseconds";
    std::cout << std::endl;
}
} // namespace MMI
} // namespace OHOS
