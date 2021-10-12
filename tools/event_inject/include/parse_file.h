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

#ifndef OHOS_PARSE_FILE_H
#define OHOS_PARSE_FILE_H
#include "send_message.h"

namespace OHOS {
    namespace MMI {
        class ParseFile {
        public:
            ParseFile() = default;
            ~ParseFile();

            int32_t AnalysisCommandFile(const std::string path, std::vector<InputEvent>& inputEventArray);

            int32_t GetDeviceStatus(const int32_t deviceType);

            int32_t SetDeviceStatus(const int32_t deviceType);

            /* get all driverfiles by target file path */
            int32_t getDriverFile(const std::string path);

            int32_t AnalysisDriveCommandFile(const std::string path, std::vector<InputEvent>& inputEventArray);
            int32_t AnalysisDriveData(const std::string& eventLine, InputEvent& inputEvent);
            int32_t getDriveCommandFile(const std::string path);

            /* get all command buff by driverfile path */
            int32_t getCommandFile(const std::string path);

            /* translate command buff to InputEvent */
            int32_t AnalysisData(const std::string& eventLine, InputEvent& inputEvent);

            /* write command to target driverfile Simulate event */
            int32_t SendCommand(InputEvent& inputEvent);

            /* open the target driverfile */
            int32_t OpenDrivers(DriverFileHander& driverHander);

            /* close the target driverfile */
            int32_t CloseDrivers(DriverFileHander& driverHander);
        public:
            bool injectToHdf_ = false;
            EventArray eventArray_;
        private:
            static int32_t index_;
            SendMessage msg_;
        private:
            static constexpr uint32_t INPUT_EVENT_X = 4;
            static constexpr uint32_t INPUT_EVENT_Y = 4;
            static constexpr uint32_t INPUT_EVENT_TYPE = 2;
            static constexpr uint32_t INPUT_EVENT_TRACK = 2;
            static constexpr uint32_t INPUT_EVENT_REPORT = 1;
            static constexpr uint32_t INPUT_EVENT_BLOCKTIME = 4;
            static constexpr uint32_t INPUT_EVENT_DATA_LEN = 17;
            static constexpr uint32_t BLOCK_TIME_DEFAULT_VALUE = 1;
            static constexpr uint32_t BLOCK_TIME_DEFAULT_INDEX = 2;
            static constexpr uint32_t EVENT_TARGET_POS = 0;
            static constexpr uint32_t EVENT_TARGET_LEN = 2;
            static constexpr uint32_t EVENT_TYPE_POS = 2;
            static constexpr uint32_t EVENT_TYPE_LEN = 2;
            static constexpr uint32_t EVENT_TRACK_POS = 4;
            static constexpr uint32_t EVENT_TRACK_LEN = 2;
            static constexpr uint32_t EVENT_X_POS = 6;
            static constexpr uint32_t EVENT_X_LEN = 4;
            static constexpr uint32_t EVENT_Y_POS = 10;
            static constexpr uint32_t EVENT_Y_LEN = 4;
            static constexpr uint32_t EVENT_REPORT_TYPE_POS = 14;
            static constexpr uint32_t EVENT_REPORT_TYPE_LEN = 1;
            static constexpr uint32_t EVENT_BLOCK_TIME_POS = 15;
            static constexpr uint32_t EVENT_BLOCK_TIME_LEN = 4;
            static constexpr uint32_t EVENT_ARGV_COUNT_POS = 4;
            static constexpr uint32_t EVENT_ARGV_COUNT_LEN = 2;
            static constexpr uint32_t EVENT_ARGV_LENGTH_POS = 6;
            static constexpr uint32_t EVENT_ARGV_LENGTH_LEN = 2;
            static constexpr int32_t EVENT_ALLOWED_LEN_SIZE = 8;
            static constexpr uint32_t EVENT_CODE_POS = 8;
            static constexpr int32_t EVENT_BLOCK_TIME_INDEX = 3;
            static constexpr int32_t EVENT_COUNT_ARROW_LEN = 8;
        };
    }
}
#endif
