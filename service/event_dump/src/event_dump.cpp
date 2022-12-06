/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "event_dump.h"

#include <getopt.h>

#include <climits>
#include <cstdarg>
#include <cstring>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "event_interceptor_handler.h"
#include "event_monitor_handler.h"
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include "input_device_cooperate_sm.h"
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "input_windows_manager.h"
#include "key_subscriber_handler.h"
#include "mouse_event_normalize.h"
#include "securec.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventDump" };
constexpr size_t MAX_COMMAND_COUNT { 32 };
} // namespace

EventDump::EventDump() {}
EventDump::~EventDump() {}

void EventDump::ParseCommand(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    size_t count = 0;
    for (const auto &str : args) {
        if (str.find("--") == 0) {
            ++count;
            continue;
        }
        if (str.find("-") == 0) {
            count += str.size() - 1;
            continue;
        }
    }
    if (count > MAX_COMMAND_COUNT) {
        MMI_HILOGE("cmd param number not more than 32");
        dprintf(fd, "cmd param number not more than 32\n");
        return;
    }
    int32_t optionIndex = 0;
    struct option dumpOptions[] = {
        {"help", no_argument, 0, 'h'},
        {"device", no_argument, 0, 'd'},
        {"devicelist", no_argument, 0, 'l'},
        {"windows", no_argument, 0, 'w'},
        {"udsserver", no_argument, 0, 'u'},
        {"subscriber", no_argument, 0, 's'},
        {"monitor", no_argument, 0, 'o'},
        {"interceptor", no_argument, 0, 'i'},
        {"mouse", no_argument, 0, 'm'},
#ifdef OHOS_BUILD_ENABLE_COOPERATE
        {"inputdevcoosm", no_argument, 0, 'k'},
#endif // OHOS_BUILD_ENABLE_COOPERATE
        {NULL, 0, 0, 0}
    };
    if (args.empty()) {
        MMI_HILOGE("size of args can't be zero");
        return;
    }
    char **argv = new (std::nothrow) char *[args.size()];
    CHKPV(argv);
    if (memset_s(argv, args.size() * sizeof(char*), 0, args.size() * sizeof(char*)) != EOK) {
        MMI_HILOGE("Call memset_s failed");
        delete[] argv;
        return;
    }
    for (size_t i = 0; i < args.size(); ++i) {
        argv[i] = new (std::nothrow) char[args[i].size() + 1];
        if (argv[i] == nullptr) {
            MMI_HILOGE("Failed to allocate memory");
            goto RELEASE_RES;
        }
        if (strcpy_s(argv[i], args[i].size() + 1, args[i].c_str()) != EOK) {
            MMI_HILOGE("strcpy_s error");
            goto RELEASE_RES;
        }
    }
    optind = 1;
    int32_t c;
    while ((c = getopt_long (args.size(), argv, "hdlwusoimc", dumpOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'h': {
                DumpEventHelp(fd, args);
                break;
            }
            case 'd': {
                InputDevMgr->Dump(fd, args);
                break;
            }
            case 'l': {
                InputDevMgr->DumpDeviceList(fd, args);
                break;
            }
            case 'w': {
                WinMgr->Dump(fd, args);
                break;
            }
            case 'u': {
                auto udsServer = InputHandler->GetUDSServer();
                CHKPV(udsServer);
                udsServer->Dump(fd, args);
                break;
            }
            case 'c': {
#ifdef OHOS_BUILD_ENABLE_COOPERATE
                InputDevCooSM->Dump(fd, args);
#else
                dprintf(fd, "Input device cooperate does not support\n");
#endif // OHOS_BUILD_ENABLE_COOPERATE
                break;
            }
            case 's': {
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
                auto subscriberHandler = InputHandler->GetSubscriberHandler();
                CHKPV(subscriberHandler);
                subscriberHandler->Dump(fd, args);
#else
                dprintf(fd, "Keyboard device does not support\n");
#endif // OHOS_BUILD_ENABLE_KEYBOARD
                break;
            }
            case 'o': {
#ifdef OHOS_BUILD_ENABLE_MONITOR
                auto monitorHandler = InputHandler->GetMonitorHandler();
                CHKPV(monitorHandler);
                monitorHandler->Dump(fd, args);
#else
                dprintf(fd, "Monitor function does not support\n");
#endif // OHOS_BUILD_ENABLE_MONITOR
                break;
            }
            case 'i': {
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
                auto interceptorHandler = InputHandler->GetInterceptorHandler();
                CHKPV(interceptorHandler);
                interceptorHandler->Dump(fd, args);
#else
                dprintf(fd, "Interceptor function does not support\n");
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
                break;
            }
            case 'm': {
#ifdef OHOS_BUILD_ENABLE_POINTER
                MouseEventHdr->Dump(fd, args);
#else
                dprintf(fd, "Pointer device does not support\n");
#endif // OHOS_BUILD_ENABLE_POINTER
                break;
            }
            default: {
                dprintf(fd, "cmd param is error\n");
                DumpHelp(fd);
                break;
            }
        }
    }
    RELEASE_RES:
    for (size_t i = 0; i < args.size(); ++i) {
        if (argv[i] != nullptr) {
            delete[] argv[i];
        }
    }
    delete[] argv;
}

void EventDump::DumpEventHelp(int32_t fd, const std::vector<std::string> &args)
{
    DumpHelp(fd);
}

void EventDump::DumpHelp(int32_t fd)
{
    dprintf(fd, "Usage:\n");
    dprintf(fd, "      -h, --help: dump help\n");
    dprintf(fd, "      -d, --device: dump the device information\n");
    dprintf(fd, "      -l, --devicelist: dump the device list information\n");
    dprintf(fd, "      -w, --windows: dump the windows information\n");
    dprintf(fd, "      -u, --udsserver: dump the uds_server information\n");
    dprintf(fd, "      -o, --monitor: dump the monitor information\n");
    dprintf(fd, "      -s, --subscriber: dump the subscriber information\n");
    dprintf(fd, "      -i, --interceptor: dump the interceptor information\n");
    dprintf(fd, "      -m, --mouse: dump the mouse information\n");
    dprintf(fd, "      -c, --dump Keyboard and mouse crossing information\n");
}
} // namespace MMI
} // namespace OHOS
