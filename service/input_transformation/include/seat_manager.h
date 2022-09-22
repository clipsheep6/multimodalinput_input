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

#ifndef SEAT_MANAGER_H
#define SEAT_MANAGER_H

#include <string>
#include <map>
#include <list>

#include "i_input_define.h"
#include "i_seat_manager.h"

namespace OHOS {
namespace MMI {

class IInputContext;
class SeatManager : public NonCopyable, public ISeatManager {
public:
    static std::unique_ptr<SeatManager> CreateInstance(const IInputContext* context);
public:
    virtual ~SeatManager() = default;

    virtual void OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice) override;
    virtual void OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice) override;
private:
    SeatManager(const IInputContext* context);

    std::shared_ptr<ITouchScreenSeat> FindTouchScreenSeat(const std::string& seatId, 
            const std::string& seatName, bool createIfNotExist);

    std::shared_ptr<ISeat> FindSeat(std::string seatId, bool createIfNotExist);

    void OnTouchScreenRemoved(const std::shared_ptr<IInputDevice>& inputDevice);

    void RemoveSeat(const std::shared_ptr<ISeat>& seat);
    void RemoveSeat(const std::shared_ptr<ITouchScreenSeat>& seat);

private:
    const IInputContext* const context_;
    std::list<std::shared_ptr<ISeat>> seats_;
    std::list<std::shared_ptr<ITouchScreenSeat>> touchScreenSeats_;
};
} // namespace MMI
} // namespace OHOS
#endif // SEAT_MANAGER_H