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

#ifndef ABS_EVNET_HANDLER_H
#define ABS_EVNET_HANDLER_H

#include <map>
#include <string>

#include "i_input_define.h"
#include "pointer_event.h"
#include "abs_event.h"
// #include "LogicalDisplayState.h"

namespace OHOS {
namespace MMI {
    class IInputContext;
class AbsEventHandler : public NonCopyable {
public:
    AbsEventHandler(const IInputContext* context, const std::string& seatId);
    virtual ~AbsEventHandler() = default;

    std::shared_ptr<const PointerEvent> HandleEvent(const std::shared_ptr<const AbsEvent>& absEvent);

    // void OnDisplayAdded(const std::shared_ptr<const LogicalDisplayState>& display);
    // void OnDisplayRemoved(const std::shared_ptr<const LogicalDisplayState>& display);
    // void OnDisplayChanged(const std::shared_ptr<const LogicalDisplayState>& display);

private:
    int32_t ConvertSourceType(int32_t absEventSourceType) const;
    int32_t ConvertAction(int32_t absEventAction) const;
    std::shared_ptr<PointerEvent::PointerItem> ConvertPointer(const std::shared_ptr<const AbsEvent::Pointer>& absEventPointer) const;

private:
    [[maybe_unused]] const IInputContext* const context_;
    const std::string seatId_;
};
} // namespace MMI
} // namespace OHOS
#endif // ABS_EVNET_HANDLER_H