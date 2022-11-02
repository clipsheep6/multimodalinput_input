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
#include "abs_event.h"

#include <algorithm>
#include <linux/input.h>

#include "i_input_define.h"
#include "mmi_log.h"
#include "stream_utils.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbsEvent" };
};
const std::shared_ptr<AbsEvent> AbsEvent::NULL_VALUE;
const std::shared_ptr<AbsEvent::Pointer> AbsEvent::Pointer::NULL_VALUE;

const char* AbsEvent::SourceToString(int32_t sourceType)
{
    switch (sourceType) {
        CASE_STR(SOURCE_TYPE_NONE);
        CASE_STR(SOURCE_TYPE_TOUCHSCREEN);
        CASE_STR(SOURCE_TYPE_TOUCHPAD);
        CASE_STR(SOURCE_TYPE_END);
        default:
        return "SOURCE_TYPE_UNKNOWN";
    }
}

const char* AbsEvent::ActionToString(int32_t action)
{
    switch(action) {
        CASE_STR(ACTION_NONE);
        CASE_STR(ACTION_DOWN);
        CASE_STR(ACTION_MOVE);
        CASE_STR(ACTION_UP);
        default:
        return "ACTION_UNKOWN";
    }
}
// int32_t AbsEvent::Pointer::GetId() const
// {
//     return id_;
// }

int32_t AbsEvent::Pointer::GetX() const
{
    return x_;
}

int32_t AbsEvent::Pointer::GetY() const
{
    return y_;
}

int64_t AbsEvent::Pointer::GetDownTime() const
{
    return downTime_;
}

std::ostream& AbsEvent::Pointer::operator<<(std::ostream& outStream) const
{
    outStream << '{' 
        << "id:" << id_ << ','
        << "x:" << x_ << ','
        << "y:" << y_ 
        << '}';
    return outStream;
}

void AbsEvent::Pointer::SetId(int32_t id)
{
    id_ = id;
}

void AbsEvent::Pointer::SetX(int32_t x)
{
    x_ = x;
}

void AbsEvent::Pointer::SetY(int32_t y)
{
    y_ = y;
}

void AbsEvent::Pointer::SetDownTime(int64_t downTime)
{
    downTime_ = downTime;
}

AbsEvent::AbsEvent(int32_t deviceId, int32_t sourceType)
    : KernelEventBase(deviceId, ACTION_NONE), pointerId_(-1), sourceType_(sourceType)
{
}

int32_t AbsEvent::GetSourceType() const
{
    return sourceType_;
}

// int32_t AbsEvent::GetPointerId() const
// {
//     return pointerId_;
// }

std::shared_ptr<AbsEvent::Pointer> AbsEvent::GetPointer() const
{
    // return GetPointer(pointerId_);
    return curPointer_;
}

// std::shared_ptr<AbsEvent::Pointer> AbsEvent::GetPointer(int32_t id) const
// {
//     for (const auto& pointer : pointers_) {
//         if (pointer->GetId() == id) {
//             return pointer;
//         }
//     }

//     return nullptr;
// }

// std::list<std::shared_ptr<const AbsEvent::Pointer>> AbsEvent::GetPointerList() const
// {
//     std::list<std::shared_ptr<const AbsEvent::Pointer>> result;
//     for (const auto& pointer : pointers_) {
//         result.push_back(pointer);
//     }
//     return result;
// }

void AbsEvent::SetCurSlot(int32_t curSlot)
{
    curSlot_ = curSlot;
}

int32_t AbsEvent::GetCurSlot() const
{
    return curSlot_;
}

// std::list<int32_t> AbsEvent::GetPointerIdList() const
// {
//     std::list<int32_t> result;
//     for (const auto& pointer: pointers_) {
//         result.push_back(pointer->GetId());
//     }
//     return result;
// }

std::ostream& AbsEvent::operator<<(std::ostream& outStream) const
{
    return PrintInternal(outStream);
}

int32_t AbsEvent::SetSourceType(int32_t sourceType)
{
    if (sourceType <= SOURCE_TYPE_NONE || sourceType >= SOURCE_TYPE_END)
    {
        MMI_HILOGE("Leave, Invalid sourceType:%{public}s", SourceToString(sourceType));
        return -1;
    }

    if (sourceType_ > SOURCE_TYPE_NONE && sourceType_ < SOURCE_TYPE_END) {
        MMI_HILOGE("Leave, Valid sourceType_:%{public}s", SourceToString(sourceType_));
        return -1;
    }

    sourceType_ = sourceType;
    return 0;
}

// int32_t AbsEvent::SetPointerId(int32_t pointerId)
// {
//     // if (!GetPointer(pointerId)) {
//     //     MMI_HILOGE("Leave, not exist pointer:%{public}d", pointerId_);
//     //     return -1;
//     // }

//     pointerId_ = pointerId;
//     return 0;
// }

int32_t AbsEvent::AddPointer(const std::shared_ptr<Pointer>& pointer)
{
    if (!pointer) {
        MMI_HILOGE("Leave, null pointer");
        return -1;
    }

    // if (pointer->GetId() < 0) {
    //     MMI_HILOGE("Leave, null pointer");
    //     return -1;
    // }

    // for (const auto& item : pointers_) {
    //     if (item == pointer) {
    //         MMI_HILOGE("Leave, Repeat Added Pointer");
    //         return -1;
    //     }

    //     if (item->GetId() == pointer->GetId()) {
    //         MMI_HILOGE("Leave, Repeat Added Pointer");
    //         return -1;
    //     }
    // }

    // pointers_.push_back(pointer);
    curPointer_ = pointer;
    return 0;
}

// int32_t AbsEvent::RemovePointer(const std::shared_ptr<Pointer>& pointer)
// {
//     if (!pointer) {
//         MMI_HILOGE("Leave, null pointer");
//         return -1;
//     }

//     for (auto it = pointers_.begin(); it != pointers_.end(); ++it) {
//         if (*it == pointer) {
//             pointers_.erase(it);
//             return 0;
//         }
//     }

//     MMI_HILOGE("Leave, not exist pointer");
//     return -1;
// }

const char* AbsEvent::ActionToStr(int32_t action) const
{
    return ActionToString(action);
}

std::ostream& AbsEvent::PrintInternal(std::ostream& outStream) const
{
    outStream << '{'
        << "KernelEventBase:";

    KernelEventBase::PrintInternal(outStream);
    outStream << ',';

    outStream << "pointerId:" << pointerId_ << ','
        << "sourceType:" << SourceToString(sourceType_) << ','
        << "pointers" << pointers_
        << '}';

    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const AbsEvent& absEvent)
{
    return absEvent.operator<<(outStream);
}

std::ostream& operator<<(std::ostream& outStream, const AbsEvent::Pointer& pointer)
{
    return pointer.operator<<(outStream);
}
} // namespace MMI
} // namespace OHOS