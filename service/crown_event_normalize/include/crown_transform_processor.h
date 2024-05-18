/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CROWN_TRANSFORM_PROCESSOR_H
#define CROWN_TRANSFORM_PROCESSOR_H

#include <map>
#include <memory>

#include "libinput.h"
#include "singleton.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_CROWN
class CrownTransformProcessor final : public std::enable_shared_from_this<CrownTransformProcessor> {
public:
    DISALLOW_COPY_AND_MOVE(CrownTransformProcessor);
    explicit CrownTransformProcessor(int32_t deviceId);
    ~CrownTransformProcessor() = default;
    std::shared_ptr<PointerEvent> GetPointerEvent() const;
    int32_t NormalizeKeyEvent(const struct libinput_event *event);
    int32_t NormalizeRotateEvent(const struct libinput_event *event);
    void Dump(int32_t fd, const std::vector<std::string> &args);

private:
    void DumpInner();
    
    std::shared_ptr<PointerEvent> pointerEvent_ { nullptr };
    int32_t deviceId_ { -1 };
    int32_t timerId_ { -1 };
    uint64_t lastTime_ { 0 };
};
#endif // OHOS_BUILD_ENABLE_CROWN
} // namespace MMI
} // namespace OHOS
#endif // CROWN_TRANSFORM_PROCESSOR_H