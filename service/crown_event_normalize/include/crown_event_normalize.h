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

#ifndef CROWN_EVENT_NORMALIZE_H
#define CROWN_EVENT_NORMALIZE_H

#include <map>
#include <memory>

#include "libinput.h"
#include "singleton.h"
#include "crown_transform_processor.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_CROWN
class CrownEventNormalize final : public std::enable_shared_from_this<CrownEventNormalize> {
    DECLARE_DELAYED_SINGLETON(CrownEventNormalize);

public:
    DISALLOW_COPY_AND_MOVE(CrownEventNormalize);
    CrownEventNormalize() = default;
    ~CrownEventNormalize() = default;
    bool IsCrownEvent(const struct libinput_event *event);
    int32_t NormalizeKeyEvent(const struct libinput_event *event);
    int32_t NormalizeRotateEvent(const struct libinput_event *event);
    void Dump(int32_t fd, const std::vector<std::string> &args);

    static constexpr int32_t CROWN_CODE_POWER = 116;

private:
    std::shared_ptr<CrownTransformProcessor> GetProcessor(int32_t deviceId) const;
    std::shared_ptr<CrownTransformProcessor> GetCurrentProcessor() const;
    void SetCurrentDeviceId(int32_t deviceId);
    int32_t GetCurrentDeviceId() const;
    
private:
    const std::string CROWN_SOURCE = "rotary_crown";

    std::map<int32_t, std::shared_ptr<CrownTransformProcessor>> processors_;
    int32_t currentDeviceId_ { -1 };
};
#define CROWNEVENTHDR ::OHOS::DelayedSingleton<CrownEventNormalize>::GetInstance()
#endif // OHOS_BUILD_ENABLE_CROWN
} // namespace MMI
} // namespace OHOS
#endif // CROWN_EVENT_NORMALIZE_H