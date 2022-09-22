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

#include "seat_manager.h"

#include <algorithm>

#include "i_input_context.h"
#include "i_input_device.h"
#include "i_seat.h"
#include "i_touch_screen_seat.h"
// #include "Log.h"
#include "mmi_log.h"
// #include "RelEvent.h"
// #include "KernelKeyEvent.h"
#include "kernel_event_handler_bridge.h"
// #include "PhysicalDisplayState.h"
// #include "LogicalDisplayState.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "SeatManager" };
};
std::unique_ptr<SeatManager> SeatManager::CreateInstance(const IInputContext* context) {
    if (context == nullptr) {
        return nullptr;
    }

    return std::unique_ptr<SeatManager>(new SeatManager(context));
}

    SeatManager::SeatManager(const IInputContext* context) 
        : context_(context) {
    }

std::shared_ptr<ISeat> SeatManager::FindSeat(std::string seatId, bool createIfNotExist) {
    MMI_HILOGD("Enter seatId:%{public}s createIfNotExist:%{public}d", seatId.c_str(), createIfNotExist);

    for (auto& seat : seats_) {
        if (seat->GetSeatId() == seatId) {
            return seat;
        }
    }

    std::shared_ptr<ISeat> result;
    if (!createIfNotExist) {
        MMI_HILOGD("Leave seatId:%{public}s createIfNotExist:%{public}d, Not Exist", seatId.c_str(), createIfNotExist);
        return result;
    }

    result = ISeat::CreateInstance(context_, seatId);
    if (!result) {
        MMI_HILOGE("Leave seatId:$%{public}s createIfNotExist:%{public}d, Create Failed", seatId.c_str(), createIfNotExist);
        return result;
    }

    seats_.push_back(result);
    MMI_HILOGD("Leave seatId:%{public}s createIfNotExist:%{public}d", seatId.c_str(), createIfNotExist);
    return result;
}

// std::shared_ptr<ITouchScreenSeat> SeatManager::FindTouchScreenSeat(const std::shared_ptr<PhysicalDisplayState>& display,
//         bool createIfNotExist)
// {
//     if (!display) {
//         MMI_HILOGE("Leave, null display");
//         return nullptr;
//     }

//     return FindTouchScreenSeat(display->GetSeatId(), display->GetSeatName(), createIfNotExist);
// }

std::shared_ptr<ITouchScreenSeat> SeatManager::FindTouchScreenSeat(const std::string& seatId, 
        const std::string& seatName, bool createIfNotExist)
{
    MMI_HILOGD("Enter seatId:%{public}s seatName:%{public}s createIfNotExist:%{public}d", seatId.c_str(), seatName.c_str(), createIfNotExist);

    for (auto& seat : touchScreenSeats_) {
        if (seat->GetSeatId() == seatId && seat->GetSeatName() == seatName) {
            return seat;
        }
    }

    std::shared_ptr<ITouchScreenSeat> result;
    if (!createIfNotExist) {
        MMI_HILOGD("Leave seatId:%{public}s seatName:%{public}s createIfNotExist:%{public}d", seatId.c_str(), seatName.c_str(), createIfNotExist);
        return result;
    }

    ITouchScreenSeat::CreateInstance(context_, seatId, seatName);
    if (!result) {
        MMI_HILOGE("Leave seatId:%{public}s seatName:%{public}s createIfNotExist:%{public}d, Create Failed", seatId.c_str(), seatName.c_str(), createIfNotExist);
        return result;
    }

    touchScreenSeats_.push_back(result);
    MMI_HILOGD("Leave seatId:%{public}s seatName:%{public}s createIfNotExist:%{public}d", seatId.c_str(), seatName.c_str(), createIfNotExist);
    return result;
}

void SeatManager::OnInputDeviceAdded(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (!inputDevice) {
        MMI_HILOGE("Leave, null inputDevice");
        return;
    }

    const auto& seatId = inputDevice->GetSeatId();
    const auto& seatName = inputDevice->GetSeatName();

    std::shared_ptr<ITouchScreenSeat> touchScreenSeat;
    if (inputDevice->HasCapability(IInputDevice::CAPABILITY_TOUCHSCREEN)) {
        touchScreenSeat = FindTouchScreenSeat(seatId, seatName, true);
        if (!touchScreenSeat) {
            MMI_HILOGE("Leave, null touchScreenSeat");
            return;
        }

        auto retCode = touchScreenSeat->BindInputDevice(inputDevice);
        if (retCode < 0) {
            RemoveSeat(touchScreenSeat);
            MMI_HILOGE("Leave, BindInputDevice Failed");
            return;
        }
    }

    auto seat = FindSeat(inputDevice->GetSeatId(), true);
    if (!seat) {
        if (touchScreenSeat) {
            touchScreenSeat->UnbindInputDevice(inputDevice);
            RemoveSeat(touchScreenSeat);
        }
        MMI_HILOGE("Leave, null seat");
        return;
    }

    auto retCode = seat->AddInputDevice(inputDevice);
    if (retCode < 0) {
        if (touchScreenSeat) {
            touchScreenSeat->UnbindInputDevice(inputDevice);
            RemoveSeat(touchScreenSeat);
        }
        MMI_HILOGE("Leave, seat AddInputDevice Failed");
        return;
    }

    auto handler = KernelEventHandlerBridge::CreateInstance(seat, touchScreenSeat);
    if (!handler) {
        if (touchScreenSeat) {
            touchScreenSeat->UnbindInputDevice(inputDevice);
            RemoveSeat(touchScreenSeat);
        }

        seat->RemoveInputDevice(inputDevice);
        RemoveSeat(seat);
        MMI_HILOGE("Leave, null bridge handler");
        return;
    }

    inputDevice->StartReceiveEvents(handler);

    MMI_HILOGD("Leave");
}

void SeatManager::OnInputDeviceRemoved(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (!inputDevice) {
        MMI_HILOGE("Leave");
        return;
    }

    auto seat = FindSeat(inputDevice->GetSeatId(), false);
    if (!seat) {
        MMI_HILOGW("Leave, null seat");
    } else {
        auto retCode = seat->RemoveInputDevice(inputDevice);
        if (retCode < 0) {
            MMI_HILOGW("Leave, seat RemoveInputDevice Failed");
        }
        RemoveSeat(seat);
    }

    auto retCode = inputDevice->StopReceiveEvents();
    if (retCode < 0) {
        MMI_HILOGW("Leave, inputDevice StopReceiveEvents Failed");
    }

    MMI_HILOGD("Leave");
}

// void SeatManager::OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     auto seat = FindTouchScreenSeat(display, true);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }
//     auto retCode = seat->BindDisplay(display);
//     if (retCode < 0) {
//         RemoveSeat(seat);
//         MMI_HILOGE("Leave, BindDisplay Failed");
//         return;
//     }

//     MMI_HILOGD("Leave");
// }

// void SeatManager::OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     auto seat = FindTouchScreenSeat(display, false);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }

//     auto retCode = seat->UnbindDisplay(display);
//     if (retCode < 0) {
//         MMI_HILOGE("Leave, BindDisplay Failed");
//         return;
//     }

//     RemoveSeat(seat);

//     MMI_HILOGD("Leave");
// }

// void SeatManager::OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     auto seat = FindTouchScreenSeat(display, false);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }

//     auto retCode = seat->UpdateDisplay(display);
//     if (retCode < 0) {
//         MMI_HILOGE("Leave, UpdateDisplay Failed");
//         return;
//     }

//     MMI_HILOGD("Leave");
// }

// void SeatManager::OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     if (!display) {
//         MMI_HILOGE("Leave, null display");
//         return;
//     }

//     for (const auto& item : touchScreenSeats_) {
//         item->OnDisplayAdded(display);
//     }

//     auto seat = FindSeat(display->GetSeatId(), true);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }

//     auto retCode = seat->AddDisplay(display);
//     if (retCode < 0) {
//         RemoveSeat(seat);
//         MMI_HILOGE("Leave, seat AddDisplay Failed");
//         return;
//     }

//     MMI_HILOGD("Leave");
// }

// void SeatManager::OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     if (!display) {
//         MMI_HILOGE("Leave, null display");
//         return;
//     }

//     for (const auto& item : touchScreenSeats_) {
//         item->OnDisplayRemoved(display);
//     }

//     auto seat = FindSeat(display->GetSeatId(), false);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }

//     auto retCode = seat->RemoveDisplay(display);
//     if (retCode < 0) {
//         MMI_HILOGE("Leave, seat RemoveDisplay Failed");
//         return;
//     }

//     MMI_HILOGD("Leave");
// }

// void SeatManager::OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display)
// {
//     MMI_HILOGD("Enter display:$s", display);
//     if (!display) {
//         MMI_HILOGE("Leave, null display");
//         return;
//     }

//     for (const auto& item : touchScreenSeats_) {
//         item->OnDisplayChanged(display);
//     }

//     auto seat = FindSeat(display->GetSeatId(), false);
//     if (!seat) {
//         MMI_HILOGE("Leave, null seat");
//         return;
//     }

//     auto retCode = seat->UpdateDisplay(display);
//     if (retCode < 0) {
//         MMI_HILOGE("Leave, seat AddDisplay Failed");
//         return;
//     }

//     MMI_HILOGD("Leave");
// }

void SeatManager::OnTouchScreenRemoved(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (!inputDevice) {
        MMI_HILOGE("Leave, null inputDevice");
        return;
    }

    const auto& seatId = inputDevice->GetSeatId();
    const auto& seatName = inputDevice->GetSeatName();

    auto seat = FindTouchScreenSeat(seatId, seatName, false);
    if (!seat) {
        MMI_HILOGE("Leave, no seat");
        return;
    }

    auto retCode = seat->UnbindInputDevice(inputDevice);
    if (retCode < 0) {
        MMI_HILOGE("Leave, BindInputDevice Failed");
        return;
    }

    MMI_HILOGD("Leave");
}

void SeatManager::RemoveSeat(const std::shared_ptr<ISeat>& seat)
{
    if (!seat)  {
        return;
    }

    if (!seat->IsEmpty()) {
        return;
    }

    seats_.remove(seat);
}

void SeatManager::RemoveSeat(const std::shared_ptr<ITouchScreenSeat>& seat)
{
    if (!seat)  {
        return;
    }

    if (!seat->IsEmpty()) {
        return;
    }

    touchScreenSeats_.remove(seat);
}
} // namespace MMI
} // namespace OHOS