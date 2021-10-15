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

#ifndef OHOS_VIRTUAL_DEVICE_CONFIG_H
#define OHOS_VIRTUAL_DEVICE_CONFIG_H

#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <fstream>
#include <memory>
#include <cinttypes>
#include <cerrno>
#include <fcntl.h>
#include "securec.h"
#include "linux/input.h"
#include "linux/uinput.h"

#ifndef REL_WHEEL_HI_RES
#define REL_WHEEL_HI_RES    0x0b
#endif

#ifndef REL_HWHEEL_HI_RES
#define REL_HWHEEL_HI_RES    0x0c
#endif

namespace OHOS {
namespace MMI {
#define CLMAP       std::map
#define CLSET       std::set
#define String      std::string
#define IdsList     std::vector<int32_t>
#define StringList  std::vector<String>
#define StringSet   CLSET<String>
#define Size_type   String::size_type
#define SYMBOL_FOLDER_PERMISSIONS 775
#define MAX_PARAMETER_NUMBER 3
#define MAX_PARAMETER_NUMBER_FOR_ADD_DEL 2
#define IS_FILE_JUDGE 8
#define BIN_NAME "hosmmi-virtual-device-manger"

    const String g_pid = std::to_string(getpid());
    const String g_folderpath = "/data/symbol/";
}
}
#endif

