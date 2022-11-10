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
class AbsEvent : public KernelEventBase {
public:
    static constexpr int32_t ACTION_NONE = 0;
    static constexpr int32_t ACTION_DOWN = 1;
    static constexpr int32_t ACTION_MOVE = 2;
    static constexpr int32_t ACTION_UP = 3;

    static constexpr int32_t SOURCE_TYPE_NONE = 0;
    static constexpr int32_t SOURCE_TYPE_TOUCHSCREEN = 1;
    static constexpr int32_t SOURCE_TYPE_TOUCHPAD = 2;
    static constexpr int32_t SOURCE_TYPE_END = 3;

public:
    static const char* SourceToString(int32_t sourceType);
    static const char* ActionToString(int32_t action);

public:
    static const std::shared_ptr<AbsEvent> NULL_VALUE;

public:
    class Pointer {
        public:
            static const std::shared_ptr<Pointer> NULL_VALUE;
        public:
            int32_t GetX() const;
            int32_t GetY() const;
            int64_t GetDownTime() const;

            std::ostream& operator<<(std::ostream& outStream) const;

            void SetId(int32_t id);
            void SetX(int32_t x);
            void SetY(int32_t y);
            void SetDownTime(int64_t downTime);

        private:
            int32_t id_{-1};
            int32_t x_{-1};
            int32_t y_{-1};
            int64_t downTime_{-1};
    };

public:
    using pointer_list = std::list<std::shared_ptr<Pointer>>;

public:
    AbsEvent(int32_t deviceId, int32_t sourceType);
    virtual ~AbsEvent() = default;
    int32_t GetSourceType() const;
    std::shared_ptr<Pointer> GetPointer() const;
    void SetCurSlot(int32_t curSlot);
    int32_t GetCurSlot() const;

    virtual std::ostream& operator<<(std::ostream& outStream) const override;

    int32_t SetSourceType(int32_t sourceType);
    void SetAxisInfo(std::shared_ptr<IDevice::AxisInfo> xInfo, std::shared_ptr<IDevice::AxisInfo> yInfo);
    std::tuple<std::shared_ptr<IDevice::AxisInfo>, std::shared_ptr<IDevice::AxisInfo>> GetAxisInfo() const;
    int32_t AddPointer(const std::shared_ptr<Pointer> pointer);

protected:
    virtual const char* ActionToStr(int32_t action) const override;
    std::ostream& PrintInternal(std::ostream& outStream) const;

private:
    int32_t pointerId_;
    int32_t sourceType_;
    int32_t curSlot_;
    pointer_list pointers_;
    std::shared_ptr<IDevice::AxisInfo> xInfo_ { nullptr };
    std::shared_ptr<IDevice::AxisInfo> yInfo_ { nullptr };
    std::shared_ptr<AbsEvent::Pointer> curPointer_ {nullptr};
};

std::ostream& operator<<(std::ostream& outStream, const AbsEvent& absEvent);
std::ostream& operator<<(std::ostream& outStream, const AbsEvent::Pointer& pointer);
} // namespace MMI
} // namespace OHOS
#endif // ABS_EVNET_H