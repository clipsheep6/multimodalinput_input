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

#include "injection_tools_help_func.h"

#include <algorithm>
#include <getopt.h>
#include <iostream>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InjectionToolsHelpFunc" };
constexpr int32_t SEND_EVENT_ARGV_COUNTS = 6;
constexpr int32_t JSON_ARGV_COUNTS = 3;
constexpr int32_t HELP_ARGV_COUNTS = 2;

} // namespace


bool InjectionToolsHelpFunc::CheckInjectionCommand(int32_t argc, char **argv)
{
    CALL_DEBUG_ENTER;
    struct option longOptions[] = {
        {"sendevent", no_argument, NULL, 'S'},
        {"json", no_argument, NULL, 'J'},
        {"help", no_argument, NULL, '?'},
        {NULL, 0 , NULL, 0}
    };
    int32_t c = 0;
    int32_t optionIndex = 0;
    c = getopt_long(argc, argv, "SJ?", longOptions, &optionIndex);
    if (c == -1) {
        std::cout << "Nonstandard input parameters" << std::endl;
        return false;
    }
    switch (c) {
        case 'S': {
            if (argc != SEND_EVENT_ARGV_COUNTS) {
                std::cout<< "Wrong number of input parameters" << std::endl;
                return false;
            }
            std::string deviceNode = argv[optind];
            if (deviceNode.empty()) {
                printf("Device node:%s is not existent\n", deviceNode.c_str());
                return false;
            }
            char realPath[PATH_MAX] = {};
            if (realpath(deviceNode.c_str(), realPath) == nullptr) {
                printf("Path is error, path:%s\n", deviceNode.c_str());
                return false;
            }
            while (++optind < argc) {
                std::string deviceInfo = argv[optind];
                if (!IsNumberic(deviceInfo)) {
                    printf("Parameter is error, element:%s\n", deviceInfo.c_str());
                    return false;
                }
            }
            SetArgvs(argc, argv, "sendevent");
            break;
        }
        case 'J': {
            if (argc < JSON_ARGV_COUNTS) {
                std::cout<< "Wrong number of input parameters" << std::endl;
                return false;
            }
            std::string jsonFile = argv[optind];
            char realPath[PATH_MAX] = {};
            if (realpath(jsonFile.c_str(), realPath) == nullptr) {
                printf("Path is error, path:%s\n", jsonFile.c_str());
                return false;
            }
            std::string jsonBuf = ReadJsonFile(jsonFile);
            if (jsonBuf.empty()) {
                return false;
            }
            SetArgvs(argc, argv, "json");
            break;
        }
        case '?': {
            if (argc != HELP_ARGV_COUNTS) {
                std::cout<< "Wrong number of input parameters" << std::endl;
                return false;
            }
            SetArgvs(argc, argv, "help");
            break;
        }
        default: {
            std::cout << "invalid command" << std::endl;
            return false;
        }
    }
    return true;
}

bool InjectionToolsHelpFunc::IsNumberic(std::string const &str)
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void InjectionToolsHelpFunc::SetArgvs(int argc, char **argv, std::string const &str)
{
    injectArgvs_.clear();
    injectArgvs_.push_back(str);
    for (uint64_t i = 2; i < static_cast<uint64_t>(argc); i++) {
        injectArgvs_.push_back(argv[i]);
    }
}

std::vector<std::string> InjectionToolsHelpFunc::GetArgvs()
{
    return injectArgvs_;
}


void InjectionToolsHelpFunc::ShowUsage()
{
    std::cout << "Usage: mmi-event-injection <option> <command> <arg>..." << std::endl;
    std::cout << "The option are:                                       " << std::endl;
    std::cout << "commands for sendevent:                               " << std::endl;
    std::cout << "-S <device_node> <type> <code> <value> --sendevent <device_node> <type> <code> <value>" << std::endl;
    std::cout << "                                         -inject the original event to the device node" << std::endl;
    std::cout << "commands for json:                                    " << std::endl;
    std::cout << "-J <file_name>              --json <file_name>"         << std::endl;
    std::cout << "          -Inject a json file that writes all action information to the virtual device" << std::endl;
    std::cout << "-?  --help                                            " << std::endl;
}
} // namespace MMI
} // namespace OHOS
