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

#ifndef INPUT_FILTER_H
#define INPUT_FILTER_H

#include <mutex>

#include "nocopyable.h"

#include "event_filter_service.h"

namespace OHOS {
namespace MMI {
class InputFilter final {
public:
    InputFilter() = default;
    DISALLOW_MOVE(InputFilter);
    ~InputFilter() = default;
    int32_t AddInputEventFilter(std::shared_ptr<IInputEventFilter> filter, int32_t priority);
    int32_t RemoveInputEventFilter(int32_t filterId);
    void OnConnected();
private:
    std::mutex mtx_;
    std::map<int32_t, std::tuple<sptr<IEventFilter>, int32_t>> eventFilterServices_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_FILTER_H