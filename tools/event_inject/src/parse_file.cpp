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
#include "parse_file.h"
#include <thread>
#include <chrono>
#include "proto.h"

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ParseFile" };
}

int32_t ParseFile::index_ = 1;
/*
 * When the program exits,
 * check and try to close all the driver files
 */
ParseFile::~ParseFile()
{
    for (int32_t i = 0; i < FILE_NUM; i++) {
        int32_t fp = eventArray_.drivers[i].fp;
        if (fp > 0) {
            CloseDrivers(eventArray_.drivers[i]);
        }
    }
    index_ = 0;
}

/*
 * Get the list of operable driver files
 * through the target file
 */
int32_t ParseFile::getDriverFile(const std::string path)
{
    if (path.empty()) {
        MMI_LOGE("drive file path invalid, errCode:%{public}d", DRIVE_PATH_INVALID);
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("open drive file failderrCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    while (getline(infile, line)) {
        if (line == "pass") {
            index_++;
            continue;
        }
        if (index_ <= FILE_NUM) {
            CHKR(EOK == memcpy_s(&eventArray_.drivers[index_++].path, FILE_BUFF_SIZE, line.c_str(), line.size()),
                 MEMCPY_SEC_FUN_FAIL, RET_ERR);
            line.clear();
        }
    }
    infile.close();

    return RET_OK;
}

/*
 * Get the set of event commands
 * from the target file
 */
int32_t ParseFile::getCommandFile(const std::string path)
{
    if (path.empty()) {
        MMI_LOGE("command file path invalid,errCode:%{public}d", CMD_PATH_INVALID);
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("open command file faild,errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    InputEvent inputEvent;

    while (getline(infile, line)) {
        if (AnalysisData(line, inputEvent) == RET_ERR) {
            MMI_LOGE("command string:[%{public}s] invalid,errCode:%{public}d",
                     line.c_str(), CMD_STR_INVALID);
            continue;
        }
        if (SendCommand(inputEvent) == RET_OK) {
            MMI_LOGI("success executed command:[%{public}s]\n", line.c_str());
        } else {
            MMI_LOGE("faild executed command:[%{public}s]\n", line.c_str());
        }
        line.clear();
        CHKR(EOK == memset_s(&inputEvent, sizeof(inputEvent), 0, sizeof(inputEvent)),
            STRSET_SEC_FUN_FAIL, RET_ERR);
    }
    infile.close();

    return RET_OK;
}

int32_t ParseFile::getDriveCommandFile(const std::string path)
{
    if (path.empty()) {
        MMI_LOGE("command file path invalid,errCode:%{public}d", CMD_PATH_INVALID);
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("open command file faild,errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    InputEvent inputEvent;

    while (getline(infile, line)) {
        if (AnalysisDriveData(line, inputEvent) == RET_ERR) {
            MMI_LOGE("command string:[%{public}s] invalid,errCode:%{public}d",
                     line.c_str(), CMD_STR_INVALID);
            continue;
        }
        if (SendCommand(inputEvent) == RET_OK) {
            MMI_LOGI("success executed command:[%{public}s]\n", line.c_str());
        } else {
            MMI_LOGE("faild executed command:[%{public}s]\n", line.c_str());
        }
        line.clear();
        CHKR(EOK == memset_s(&inputEvent, sizeof(inputEvent), 0, sizeof(inputEvent)),
            STRSET_SEC_FUN_FAIL, RET_ERR);
    }
    infile.close();

    return RET_OK;
}


int32_t ParseFile::SetDeviceStatus(const int32_t deviceType)
{
    return RET_OK;
}

int32_t ParseFile::GetDeviceStatus(const int32_t deviceType)
{
    return RET_OK;
}

int32_t ParseFile::AnalysisCommandFile(const std::string path, std::vector<InputEvent>& inputEventArray)
{
    if (path.empty()) {
        MMI_LOGE("command file path invalid,errCode:%{public}d", CMD_PATH_INVALID);
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("open command file faild,errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    InputEvent inputEvent = {};
    inputEventArray.clear();
    while (getline(infile, line)) {
        if (AnalysisData(line, inputEvent) == RET_ERR) {
            MMI_LOGE("command string:[%{public}s] invalid,errCode:%{public}d",
                     line.c_str(), CMD_STR_INVALID);
            continue;
        }
        inputEventArray.push_back(inputEvent);
        line.clear();
        CHKR(EOK == memset_s(&inputEvent, sizeof(inputEvent), 0, sizeof(inputEvent)),
            STRSET_SEC_FUN_FAIL, RET_ERR);
    }
    infile.close();

    return RET_OK;
}

int32_t ParseFile::AnalysisDriveCommandFile(const std::string path, std::vector<InputEvent>& inputEventArray)
{
    if (path.empty()) {
        MMI_LOGE("command file path invalid,errCode:%{public}d", CMD_PATH_INVALID);
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("open command file faild,errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    InputEvent inputEvent = {};
    inputEventArray.clear();
    while (getline(infile, line)) {
        if (AnalysisData(line, inputEvent) == RET_ERR) {
            MMI_LOGE("command string:[%{public}s] invalid,errCode:%{public}d",
                     line.c_str(), CMD_STR_INVALID);
            continue;
        }
        inputEventArray.push_back(inputEvent);
        line.clear();
        CHKR(memset_s(&inputEvent, sizeof(inputEvent), 0, sizeof(inputEvent) == EOK),
            STRSET_SEC_FUN_FAIL, RET_ERR);
    }
    infile.close();

    return RET_OK;
}

/*
 * Converts the command string
 * to the specified data type
 * for event injection
 */
int32_t ParseFile::AnalysisData(const std::string &eventLine, InputEvent &event)
{
    if (eventLine.empty()) {
        return RET_ERR;
    }
    bool result = std::all_of(eventLine.begin(), eventLine.end(), [](char c) {
        if ((c >= '0' && c <= '9') || (c == '-')) {
            return true;
        }
        return false;
        });
    if (!result) {
        MMI_LOGI("command string [%{public}s] invalid", eventLine.c_str());
        return RET_ERR;
    }

    event.target = std::stoi(eventLine.substr(EVENT_TARGET_POS, EVENT_TARGET_LEN));
    if (event.target == INPUT_DEVICE_CAP_TOUCH) {
        event.type = stoi(eventLine.substr(EVENT_TYPE_POS, EVENT_TYPE_LEN));
        event.track = stoi(eventLine.substr(EVENT_TRACK_POS, EVENT_TRACK_LEN));
        event.x = stoi(eventLine.substr(EVENT_X_POS, EVENT_X_LEN));
        event.y = stoi(eventLine.substr(EVENT_Y_POS, EVENT_Y_LEN));
        if (event.x < 0 || event.x > MUT_SCREEN_WIDTH || event.y < 0 || event.y > MUT_SCREEN_HEIGHT) {
            MMI_LOGE("input event x or y error, errCode:%{public}d", PARAM_INPUT_FAIL);
            return RET_ERR;
        }
        event.multiReprot = stoi(eventLine.substr(EVENT_REPORT_TYPE_POS, EVENT_REPORT_TYPE_LEN));
        event.blockTime = stoi(eventLine.substr(EVENT_BLOCK_TIME_POS, EVENT_BLOCK_TIME_LEN));
    } else {
        event.type = std::stoi(eventLine.substr(EVENT_TYPE_POS, EVENT_TYPE_LEN));
        int32_t count = std::stoi(eventLine.substr(EVENT_ARGV_COUNT_POS, EVENT_ARGV_COUNT_LEN));
        int32_t len = std::stoi(eventLine.substr(EVENT_ARGV_LENGTH_POS, EVENT_ARGV_LENGTH_LEN));
        if (len > EVENT_ALLOWED_LEN_SIZE) {
            MMI_LOGE("input event data length error, length: [%{public}d], errCode:%{public}d", len,
                     EVENT_DATA_LEN_INPUT_FAIL);
            return RET_ERR;
        }
        int32_t code = std::stoi(eventLine.substr(EVENT_CODE_POS, len));
        int32_t value = std::stoi(eventLine.substr(EVENT_CODE_POS + len, len));

        event.blockTime = (count < EVENT_BLOCK_TIME_INDEX) ? BLOCK_TIME_DEFAULT_VALUE : 
                               std::stoi(eventLine.substr(EVENT_CODE_POS + len * BLOCK_TIME_DEFAULT_INDEX, len));
        switch (event.type) {
            case MOUSE_MOVE_EVENT:
                event.x = code;
                event.y = value;
                event.code = (uint16_t)code;
                event.value = value;
                break;
            default:
                event.code = (uint16_t)code;
                event.value = value;
                break;
        }
    }
    return RET_OK;
}

int32_t ParseFile::AnalysisDriveData(const std::string& eventLine, InputEvent& inputEvent)
{
    if (eventLine.empty()) {
        return RET_ERR;
    }
    bool result = std::all_of(eventLine.begin(), eventLine.end(), [](char c) {
        if ((c >= '0' && c <= '9') || (c == '-')) {
            return true;
        }
        return false;
        });
    if (!result) {
        MMI_LOGI("command string [%{public}s] invalid", eventLine.c_str());
        return RET_ERR;
    }
    inputEvent.target = std::stoi(eventLine.substr(EVENT_TARGET_POS, EVENT_TARGET_LEN));
    inputEvent.type = std::stoi(eventLine.substr(EVENT_TYPE_POS, EVENT_TYPE_LEN));
    int32_t count = std::stoi(eventLine.substr(EVENT_ARGV_COUNT_POS, EVENT_ARGV_COUNT_LEN));
    int32_t len = std::stoi(eventLine.substr(EVENT_ARGV_LENGTH_POS, EVENT_ARGV_LENGTH_LEN));
    if (len > EVENT_COUNT_ARROW_LEN) {
        MMI_LOGE("input event data length error, length: [%{public}d], errCode:%{public}d", len,
                 EVENT_DATA_LEN_INPUT_FAIL);
        return RET_ERR;
    }
    int32_t code = std::stoi(eventLine.substr(EVENT_CODE_POS, len));
    int32_t value = std::stoi(eventLine.substr(EVENT_CODE_POS + len, len));
    inputEvent.blockTime = (count < EVENT_BLOCK_TIME_INDEX) ? BLOCK_TIME_DEFAULT_VALUE : 
                           std::stoi(eventLine.substr(EVENT_CODE_POS + len * BLOCK_TIME_DEFAULT_INDEX, len));
    switch (inputEvent.type) {
        case MOUSE_MOVE_EVENT:
            inputEvent.x = code;
            inputEvent.y = value;
            inputEvent.code = (uint16_t)code;
            inputEvent.value = value;
            break;
        default:
            inputEvent.code = (uint16_t)code;
            inputEvent.value = value;
            break;
        }
    return RET_OK;
}

/*
 * Write the specified command
 * set to the driver file
 */
int32_t ParseFile::SendCommand(InputEvent& inputEvent)
{
    if (inputEvent.target < 0 || inputEvent.target >= FILE_NUM) {
        MMI_LOGE("Wrong number of input parameters! inputEvent.target[%{public}d] errCode:%{public}d",
                 inputEvent.target, PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    int32_t ret = RET_ERR;
    int32_t result = RET_ERR;
    msg_.injectToHdf_ = injectToHdf_;
    if (!injectToHdf_) {
        DriverFileHander driver = eventArray_.drivers[inputEvent.target];
        ret = OpenDrivers(driver);
        if (ret > 0) {
            inputEvent.fp = ret;
            result = msg_.WriteDevice(inputEvent);
            if (result == RET_ERR) {
                return RET_ERR;
            }
            if (inputEvent.blockTime > 0 && inputEvent.type != KEY_LONG_EVENT) {
                std::this_thread::sleep_for(std::chrono::milliseconds(inputEvent.blockTime));
            }
            MMI_LOGI("success open drive file:[%{public}s]", driver.path);
            return RET_OK;
        }
        MMI_LOGI("faild open drive file:[%{public}s]", driver.path);
        return RET_ERR;
    } else {
        inputEvent.fp = inputEvent.target;
        result = msg_.WriteDevice(inputEvent);
    }
    return RET_ERR;
}

int32_t ParseFile::OpenDrivers(DriverFileHander& driverHander)
{
    if (strlen(driverHander.path) == 0) {
        MMI_LOGI("open driver path:%{public}s not exit!", driverHander.path);
        return RET_ERR;
    }

    int32_t f_drive = driverHander.fp;
    int32_t modRest = chmod(driverHander.path, PERMISSION_RWX);
    if (modRest == -1) {
        MMI_LOGE("Error mode");
    }
    if (f_drive > 0) {
        close(f_drive);
    }
    f_drive = open(driverHander.path, O_RDWR);
    driverHander.fp = f_drive;

    return f_drive;
}

int32_t ParseFile::CloseDrivers(DriverFileHander& driverHander)
{
    int32_t f_drive = driverHander.fp;
    if (f_drive > 0) {
        close(f_drive);
    }

    return RET_OK;
}