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

#ifndef ABS_EVNET_H
#define ABS_EVNET_H

#include <list>
#include <memory>
#include <ostream>
#include <tuple>

#include "i_device.h"
#include "kernel_event_base.h"

namespace OHOS {
namespace MMI {
class AbsEvent final : public KernelEventBase {
public:
    static constexpr int32_t ACTION_NONE = 0;
    static constexpr int32_t ACTION_DOWN = 1;
    static constexpr int32_t ACTION_MOVE = 2;
    static constexpr int32_t ACTION_UP = 3;
    static constexpr int32_t SOURCE_TYPE_NONE = 0;
    static constexpr int32_t SOURCE_TYPE_TOUCHSCREEN = 1;
    static constexpr int32_t SOURCE_TYPE_TOUCHPAD = 2;
    static constexpr int32_t SOURCE_TYPE_END = 3;
    static const char* SourceToString(int32_t sourceType);
    static const char* ActionToString(int32_t action);
public:
    class Pointer {
        public:
            int32_t GetX() const { return x_; }
            int32_t GetY() const { return y_; }
            int64_t GetDownTime() const { return downTime_; }
            void SetId(int32_t id) { id_ = id; }
            void SetX(int32_t x) { x_ = x; }
            void SetY(int32_t y) { y_ = y; }
            void SetDownTime(int64_t downTime) { downTime_ = downTime; }
            void SetRefreshState(bool isReflashed) { isReflashed_ = isReflashed; }
            bool GetRefreshState() { return isReflashed_; }
            void SetAction(int32_t pointerAction) { pointerAction_ = pointerAction;}
            int32_t GetAction() { return pointerAction_;}
            friend std::ostream& operator<<(std::ostream& os, const AbsEvent::Pointer& pointer);
        private:
            int32_t id_ {-1};
            int32_t x_ {-1};
            int32_t y_ {-1};
            int64_t downTime_ {-1};
            bool isReflashed_ = false;
            int32_t pointerAction_ { AbsEvent::ACTION_NONE };
            
    };
public:
    AbsEvent(int32_t deviceId, int32_t sourceType);
    virtual ~AbsEvent() = default;
    int32_t GetSourceType() const;
    std::shared_ptr<AbsEvent::Pointer> GetPointer() const;
    void SetCurSlot(int32_t curSlot);
    int32_t GetCurSlot() const;
    friend std::ostream& operator<<(std::ostream& os, const AbsEvent &r);
    int32_t SetSourceType(int32_t sourceType);
    void SetAxisInfo(std::shared_ptr<IDevice::AxisInfo> xInfo, std::shared_ptr<IDevice::AxisInfo> yInfo);
    std::tuple<std::shared_ptr<IDevice::AxisInfo>, std::shared_ptr<IDevice::AxisInfo>> GetAxisInfo() const;
    int32_t SetPointer(const std::shared_ptr<Pointer> pointer);
protected:
    virtual std::string ActionToStr(int32_t action) const override;
private:
    int32_t pointerId_;
    int32_t sourceType_;
    int32_t curSlot_;
    std::shared_ptr<IDevice::AxisInfo> xInfo_ { nullptr };
    std::shared_ptr<IDevice::AxisInfo> yInfo_ { nullptr };
    std::shared_ptr<AbsEvent::Pointer> curPointer_ {nullptr};
};
} // namespace MMI
} // namespace OHOS
#endif // ABS_EVNET_H