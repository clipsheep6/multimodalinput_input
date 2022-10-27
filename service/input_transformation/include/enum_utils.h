/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ENUM_UTILS_H
#define ENUM_UTILS_H

#include <cstdint>

namespace OHOS {
namespace MMI {
namespace EnumUtils {
    const char* InputEventTypeToString(int32_t eventType);
    const char* InputEventCodeToString(int32_t eventType, int32_t eventCode);
    const char* InputEventSynCodeToString(int32_t eventCode);
    const char* InputEventKeyCodeToString(int32_t eventCode);
    const char* InputEventRelCodeToString(int32_t eventCode);
    const char* InputEventAbsCodeToString(int32_t eventCode);
    const char* InputEventMscCodeToString(int32_t eventCode);
    const char* InputEventSwCodeToString(int32_t eventCode);
    const char* InputEventLedCodeToString(int32_t eventCode);
    const char* InputEventSndCodeToString(int32_t eventCode);
    const char* InputEventRepCodeToString(int32_t eventCode);
    const char* InputEventFfCodeToString(int32_t eventCode);
    const char* InputEventPwrCodeToString(int32_t eventCode);
    const char* InputEventFfstatusCodeToString(int32_t eventCode);
    const char* InputPropertyToString(int32_t property);
};
} // namespace MMI
} // namespace OHOS
#endif // ENUM_UTILS_H