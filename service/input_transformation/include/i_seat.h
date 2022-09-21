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

#ifndef I_SEAT_H
#define I_SEAT_H

#include <memory>
#include <list>

#include "i_kernel_event_handler.h"
// #include "LogicalDisplayState.h"

namespace OHOS {
namespace MMI {
    
    // class IInputContext;
    class IInputDevice;

    class ISeat {
        public:
            static std::unique_ptr<ISeat> CreateInstance(const std::string& seatId);

            virtual ~ISeat() = default;

            virtual const std::string& GetSeatId() const = 0;

            // virtual int32_t AddDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            // virtual int32_t RemoveDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            // virtual int32_t UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            // virtual std::list<std::shared_ptr<LogicalDisplayState>> GetDisplays() const = 0;

            virtual int32_t AddInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual int32_t RemoveInputDevice(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual std::list<std::shared_ptr<IInputDevice>> GetInputDevices() const = 0;

            virtual bool IsEmpty() const = 0;

            // virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) = 0;
            // virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) = 0;
            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
    };

} // namespace MMI
} // namespace OHOS
#endif // SEAT_H