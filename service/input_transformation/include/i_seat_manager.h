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

#ifndef I_SEAT_MANAGER_H
#define I_SEAT_MANAGER_H

#include <memory>
#include <string>

namespace OHOS {
namespace MMI {

    // class IInputContext;
    class ISeat;
    class ITouchScreenSeat;
    class IInputDevice;
    // class PhysicalDisplayState;
    // class LogicalDisplayState;

    class ISeatManager {
        public:
            static std::unique_ptr<ISeatManager> CreateInstance();
            static const std::unique_ptr<ISeatManager> NULL_VALUE;

            virtual ~ISeatManager() = default;

            virtual void OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice) = 0;
            virtual void OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice) = 0;

            // virtual void OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            // virtual void OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display) = 0;
            // virtual void OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display) = 0;

            // virtual void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            // virtual void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) = 0;
            // virtual void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) = 0;

    };
} // namespace MMI
} // namespace OHOS
#endif // I_SEAT_MANAGER_H
