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
#ifndef INPUT_WINDOW_TRANSFER_H
#define INPUT_WINDOW_TRANSFER_H

#include "nocopyable.h"

#include "display_info.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
class InputWindowTransfer final {
public:
    InputWindowTransfer() = default;
    DISALLOW_MOVE(InputWindowTransfer);
    ~InputWindowTransfer() = default;
    void UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo);
    void OnConnected();
private:
    void SendDisplayInfo();
    int32_t PackDisplayData(NetPacket &pkt);
    int32_t PackWindowInfo(NetPacket &pkt);
    int32_t PackDisplayInfo(NetPacket &pkt);
    void PrintDisplayInfo();
private:
    std::mutex mtx_;
    DisplayGroupInfo displayGroupInfo_ {};
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_WINDOW_TRANSFER_H