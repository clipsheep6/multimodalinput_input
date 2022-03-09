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

#ifndef KEY_EVENT_VALUE_TRANSFORMATION_H
#define KEY_EVENT_VALUE_TRANSFORMATION_H

#include <string>

namespace OHOS {
namespace MMI {
struct KeyEventValueTransformations {
    std::string keyEvent;
    int16_t keyValueOfNative;
    int16_t keyValueOfSys;
    int16_t keyEventOfSys;
};

KeyEventValueTransformations KeyValueTransformationInput(int16_t keyValueOfInput);
} // namespace MMI
} // namespace OHOS

#endif // KEY_EVENT_VALUE_TRANSFORMATION_H