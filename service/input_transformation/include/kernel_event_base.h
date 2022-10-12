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

#ifndef I_KERNEL_EVENT_BASE_H
#define I_KERNEL_EVENT_BASE_H

#include <cstdint>
#include <ostream>
#include <memory>

#include "stream_utils.h"

struct input_event;
namespace OHOS {
namespace MMI {
class KernelEventBase {
public:
    virtual ~KernelEventBase() = default;

    int32_t GetDeviceId() const;
    int32_t GetAction() const;
    int64_t GetActionTime() const;
    virtual std::ostream& operator<<(std::ostream& outStream) const;

    void SetAction(int32_t action);
    void SetActionTime(int64_t actionTime);

protected:
    KernelEventBase(int32_t deviceId, int32_t action);
    virtual const char* ActionToStr(int32_t action) const = 0;

    std::ostream& PrintInternal(std::ostream& outStream) const;

private:
    int32_t deviceId_;
    int32_t action_;
    int64_t actionTime_;
};

std::ostream& operator<<(std::ostream& outStream, const KernelEventBase& event);

} // namespace MMI
} // namespace OHOS
#endif // I_KERNEL_EVENT_BASE_H


