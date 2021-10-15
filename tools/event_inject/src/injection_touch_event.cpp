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
#include "injection_touch_event.h"
#include "libmmi_util.h"

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InjectionTouchEvent" };
}

int32_t InjectionTouchEvent::GetTouchFile(const std::string& path)
{
    if (path.empty()) {
        return RET_ERR;
    }
    std::ifstream infile;
    infile.open(path, std::ios::in);
    if (!infile) {
        MMI_LOGE("fopen file error! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    std::string line;
    InputEvent inputEvent = {};

    while (getline(infile, line)) {
        CHKR(EOK == memset_s(&inputEvent, sizeof(inputEvent), 0, sizeof(inputEvent)),
            STRSET_SEC_FUN_FAIL, RET_ERR);
        if (AnalysisData(line, inputEvent) == RET_ERR) {
            MMI_LOGI("faild executed command:[%{public}s], errCode:%{public}d", line.c_str(), TOUCH_CMD_INPUT_FAIL);
            return RET_ERR;
        }

        if (parseFile_.SendCommand(inputEvent) == RET_OK) {
            MMI_LOGI("success executed command:[%{public}s]", line.c_str());
        } else {
            MMI_LOGI("faild executed command:[%{public}s]", line.c_str());
        }
        line.clear();
    }
    infile.close();

    return RET_OK;
}

int32_t InjectionTouchEvent::AnalysisData(const string& eventLine, InputEvent& inputEvent)
{
    if (eventLine.empty()) {
        return RET_ERR;
    }
    
    inputEvent.target = TOUCH_DEVICE_INDEX;
    inputEvent.type = stoi(eventLine.substr(TOUCH_EVENT_TYPE_POS, TOUCH_EVENT_TYPE_LEN));
    inputEvent.track = stoi(eventLine.substr(TOUCH_EVENT_TRACK_POS, TOUCH_EVENT_TRACK_LEN));
    inputEvent.x = stoi(eventLine.substr(TOUCH_EVENT_X_POS, TOUCH_EVENT_X_LEN));
    inputEvent.y = stoi(eventLine.substr(TOUCH_EVENT_Y_POS, TOUCH_EVENT_Y_LEN));

    if (inputEvent.x < 0 || inputEvent.x > MUT_SCREEN_WIDTH ||
        inputEvent.y < 0 || inputEvent.y > MUT_SCREEN_HEIGHT) {
        MMI_LOGE("input event x or y error, errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    inputEvent.multiReprot = stoi(eventLine.substr(TOUCH_EVENT_REPROT_TYPE_POS, TOUCH_EVENT_REPROT_TYPE_LEN));
    inputEvent.blockTime = stoi(eventLine.substr(TOUCH_EVENT_BLOCK_TIME_POS, TOUCH_EVENT_BLOCK_TIME_LEN));
    
    return RET_OK;
}

int32_t InjectionTouchEvent::SetDeviceHandle(const string &deviceFile)
{
    if (deviceFile.empty()) {
        return RET_ERR;
    }
    CHKR(EOK == memcpy_s(&parseFile_.eventArray_.drivers[TOUCH_DEVICE_INDEX].path, FILE_BUFF_SIZE,
                         deviceFile.c_str(), deviceFile.size()), MEMCPY_SEC_FUN_FAIL, RET_ERR);

    return RET_OK;
}
