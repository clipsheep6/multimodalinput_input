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

#include "nocopyable.h"

#include "abs_event.h"
#include "i_device.h"

namespace OHOS {
namespace MMI {
class AbsEventCollector {
    using OnCollectCallback = std::function<void(const std::shared_ptr<AbsEvent> event)>;
public:
    AbsEventCollector(int32_t devIndex, int32_t sourceType, OnCollectCallback callback);
    virtual ~AbsEventCollector() = default;
    DISALLOW_COPY_AND_MOVE(AbsEventCollector);
    void HandleKeyEvent(int32_t code, int32_t value);
    void HandleAbsEvent(int32_t code, int32_t value);
    void HandleAbsAbsoluteEvent(int32_t code, int32_t value);
    void HandleSyncEvent();
    int32_t SetSourceType(int32_t sourceType);
    void SetAxisInfo(std::shared_ptr<IDevice::AxisInfo> xInfo, std::shared_ptr<IDevice::AxisInfo> yInfo);
    void SetMtFlag(bool isMt) { isMt_ = isMt; }
protected:
    void HandleMtSlot(int32_t value);
    void HandleMtTrackingId(int32_t value);
    void HandleMtPositionX(int32_t value);
    void HandleMtPositionY(int32_t value);
    void FinishPointer();
    std::shared_ptr<AbsEvent::Pointer> GetCurrentPointer(bool createIfNotExist);
private:
   void FinishAbsoluteDown();
   void FinishAbsoluteUp();
   void FinishAbsoluteMotion();
private:
    int32_t curSlot_ {};
    int32_t slotNum_ { 10 };
    std::shared_ptr<AbsEvent> absEvent_ { nullptr };
    std::shared_ptr<IDevice::AxisInfo> xInfo_ { nullptr };
    std::shared_ptr<IDevice::AxisInfo> yInfo_ { nullptr };
    std::map<int32_t, std::shared_ptr<AbsEvent::Pointer>> pointers_;
	OnCollectCallback collectCallback_;
    int32_t action_ { AbsEvent::ACTION_NONE };
    uint32_t slot_map_;
    bool isMt_ = false;

};
} // namespace MMI
} // namespace OHOS
#endif // ABS_EVNET_COLLECTOR_H