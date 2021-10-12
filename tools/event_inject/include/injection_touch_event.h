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

#ifndef OHOS_INJECTION_TOUCH_EVENT_H
#define OHOS_INJECTION_TOUCH_EVENT_H

#include "msg_head.h"
#include "parse_file.h"

namespace OHOS {
namespace MMI {
    class InjectionTouchEvent {
    public:
        InjectionTouchEvent() = default;
        ~InjectionTouchEvent() = default;
        int32_t GetTouchFile(const std::string &path);
        int32_t AnalysisData(const std::string &eventLine, InputEvent &inputEvent);
        int32_t SetDeviceHandle(const std::string &deviceFile);
    private:
        static int32_t index_;
        ParseFile parseFile_ = {};
    private:
        static constexpr uint32_t TOUCH_EVENT_TYPE_POS = 0;
        static constexpr uint32_t TOUCH_EVENT_TYPE_LEN = 2;
        static constexpr uint32_t TOUCH_EVENT_TRACK_POS = 2;
        static constexpr uint32_t TOUCH_EVENT_TRACK_LEN = 2;
        static constexpr uint32_t TOUCH_EVENT_X_POS = 4;
        static constexpr uint32_t TOUCH_EVENT_X_LEN = 4;
        static constexpr uint32_t TOUCH_EVENT_Y_POS = 8;
        static constexpr uint32_t TOUCH_EVENT_Y_LEN = 4;
        static constexpr uint32_t TOUCH_EVENT_REPROT_TYPE_POS = 12;
        static constexpr uint32_t TOUCH_EVENT_REPROT_TYPE_LEN = 1;
        static constexpr uint32_t TOUCH_EVENT_BLOCK_TIME_POS = 13;
        static constexpr uint32_t TOUCH_EVENT_BLOCK_TIME_LEN = 4;
};
}
}
#endif
