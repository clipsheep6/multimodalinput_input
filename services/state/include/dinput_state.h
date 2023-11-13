/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_STATE_BASE_H
#define DISTRIBUTED_INPUT_STATE_BASE_H

#include <map>
#include <mutex>
#include <string>
#include <linux/input.h>
#include "constants_dinput.h"

#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
/*
 * This enumeration class represents the two states of the peropheral:
 * THROUGH_IN : The state indicates the peripheral takes effect on the local device.
 * THROUGH_OUT : The state indicates that the peripheral takes effect at the remote device.
 */
enum class DhIdState {
    THROUGH_IN = 0,
    THROUGH_OUT,
};

class DInputState {
    DECLARE_SINGLE_INSTANCE_BASE(DInputState);
public:
    int32_t Init();
    int32_t Release();
    int32_t RecordDhIds(const std::vector<std::string> &dhIds, DhIdState state, const int32_t sessionId);
    int32_t RemoveDhIds(const std::vector<std::string> &dhIds);
    DhIdState GetStateByDhid(const std::string &dhId);

    void AddKeyDownState(struct RawEvent event);
    void RemoveKeyDownState(struct RawEvent event);
    /**
     * If user pressed some keys before we prepare distributed input sucess, because the monitor thread not start,
     * we CAN NOT monitor those keys.
     * So, we check all keys state before start input device monitor thread.
     * But the keys state not contain the pressed order, so we use the key REPEAT event to move the corresponding
     * cached key last.
     */
    void CheckAndSetLongPressedKeyOrder(struct RawEvent event);

    /**
     * Clear Device stats if unprepare.
     */
    void ClearDeviceStates();

    void RefreshABSPosition(const std::string &dhId, int32_t absX, int32_t absY);
    std::pair<int32_t, int32_t> GetAndClearABSPosition(const std::string &dhId);
private:
    DInputState() = default;
    ~DInputState();
    // Simulate device state to the pass through target device.
    void SimulateEventInjectToSrc(const int32_t sessionId, const std::vector<std::string> &dhIds);
    void SimulateBtnTouchEvent(const int32_t sessionId, const std::string &dhId, const struct RawEvent &event);
    void SimulateNormalEvent(const int32_t sessionId, const std::string &dhId, const struct RawEvent &event);
private:
    std::mutex operationMutex_;
    std::map<std::string, DhIdState> dhIdStateMap_;

    std::mutex keyDownStateMapMtx_;
    // Record key down state of each device dhid
    std::unordered_map<std::string, std::vector<struct RawEvent>> keyDownStateMap_;

    std::mutex absPosMtx_;
    // Record abs x/y of touchpad
    std::unordered_map<std::string, std::pair<int32_t, int32_t>> absPositionsMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_STATE_BASE_H