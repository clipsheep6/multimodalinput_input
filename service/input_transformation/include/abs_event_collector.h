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

#ifndef ABS_EVNET_COLLECTOR_H
#define ABS_EVNET_COLLECTOR_H

#include <memory>
#include <map>
#include <list>

#include "abs_event.h"
#include "i_input_define.h"

namespace OHOS {
namespace MMI {
class AbsEventCollector : public NonCopyable {
public:
    AbsEventCollector(int32_t deviceId, int32_t sourceType);
    virtual ~AbsEventCollector() = default;

    const std::shared_ptr<AbsEvent>& HandleAbsEvent(int32_t code, int32_t value);
    const std::shared_ptr<AbsEvent>& HandleSyncEvent(int32_t code, int32_t value);

    void AfterProcessed();

    int32_t SetSourceType(int32_t sourceType);
    void SetAction(int32_t action);

protected:
    const std::shared_ptr<AbsEvent>& HandleMtSlot(int32_t value);
    void HandleMtPositionX(int32_t value);
    void HandleMtPositionY(int32_t value);
    const std::shared_ptr<AbsEvent>& HandleMtTrackingId(int32_t value);

    std::shared_ptr<AbsEvent::Pointer> GetCurrentPointer(bool createIfNotExist);
    const std::shared_ptr<AbsEvent>& FinishPointer();

    void RemoveReleasedPointer();

private:
    [[maybe_unused]] const int32_t deviceId_;
    int32_t sourceType_;
    int32_t curSlot_;
    int32_t nextId_;
    std::shared_ptr<AbsEvent::Pointer> curPointer_;
    int32_t absEventAction_ {AbsEvent::ACTION_NONE};
    std::shared_ptr<AbsEvent> absEvent_;
    std::map<int32_t, std::shared_ptr<AbsEvent::Pointer>> pointers_;

};
} // namespace MMI
} // namespace OHOS
#endif // ABS_EVNET_COLLECTOR_H