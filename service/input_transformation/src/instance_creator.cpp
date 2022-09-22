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

// #include "EventLooper.h"
#include "device_manager.h"
#include "input_context.h"
#include "seat_manager.h"
#include "seat.h"
// #include "WindowStateManager.h"
// #include "EventDispatcher.h"
#include "touch_screen_seat.h"

namespace OHOS {
namespace MMI {
// std::unique_ptr<IEventLooper> IEventLooper::CreateInstance(IInputContext* context)
// {
//     return EventLooper::CreateInstance(context);
// }

std::unique_ptr<IDeviceManager> IDeviceManager::CreateInstance(IInputContext* context) 
{
    return DeviceManager::CreateInstance(context);
}

std::unique_ptr<IInputContext> IInputContext::CreateInstance() 
{
    return InputContext::CreateInstance();
}

std::unique_ptr<ISeatManager> ISeatManager::CreateInstance(const IInputContext* context) 
{
    return SeatManager::CreateInstance(context);
}

std::unique_ptr<ISeat> ISeat::CreateInstance(const IInputContext* context, const std::string& seatId)
{
    return Seat::CreateInstance(context, seatId);
}

// std::unique_ptr<IWindowStateManager> IWindowStateManager::CreateInstance(const IInputContext* context) {
//     return WindowStateManager::CreateInstance(context);
// }

// std::unique_ptr<IEventDispatcher> IEventDispatcher::CreateInstance(const IInputContext* context) {
//     return EventDispatcher::CreateInstance(context);
// }

std::unique_ptr<ITouchScreenSeat> ITouchScreenSeat::CreateInstance(const IInputContext* context, const std::string& seatId, const std::string& seatName) {
    return TouchScreenSeat::CreateInstance(context, seatId, seatName);
}
} // namespace MMI
} // namespace OHOS