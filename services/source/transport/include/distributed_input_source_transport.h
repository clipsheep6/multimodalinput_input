/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H
#define DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H

#include <condition_variable>
#include <string>
#include <mutex>
#include <set>
#include <map>
#include <vector>
#include "securec.h"
#include "nlohmann/json.hpp"

#include "constants.h"
#include "event_handler.h"
#include "dinput_source_trans_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceTransport {
public:
    static DistributedInputSourceTransport &GetInstance();
    ~DistributedInputSourceTransport();

    int32_t Init();
    void Release();
    int32_t OpenInputSoftbus(const std::string &remoteDevId);
    void CloseInputSoftbus(const std::string &remoteDevId);
    void RegisterSourceRespCallback(std::shared_ptr<DInputSourceTransCallback> callback);

    int32_t PrepareRemoteInput(const std::string& deviceId);
    int32_t UnprepareRemoteInput(const std::string& deviceId);
    int32_t StartRemoteInput(const std::string& deviceId, const uint32_t& inputTypes);
    int32_t StopRemoteInput(const std::string& deviceId, const uint32_t& inputTypes);

    int32_t OnSessionOpened(int32_t sessionId, int32_t result);
    void OnSessionClosed(int32_t sessionId);
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);

private:
    std::string FindDeviceBySession(int32_t sessionId);
    int32_t SendMsg(int32_t sessionId, std::string &message);
    int32_t CheckDeviceSessionState(const std::string &remoteDevId);
    void HandleSessionData(int32_t sessionId, const std::string& messageData);
    void NotifyResponsePrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStartRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStopRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyReceivedEventRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);

private:
    std::map<std::string, int32_t> sessionDevMap_;
    std::map<std::string, bool> channelStatusMap_;
    std::mutex operationMutex_;
    std::set<int32_t> sessionIdSet_;
    std::shared_ptr<DInputSourceTransCallback> callback_;
    std::string mySessionName_ = "";
    std::condition_variable openSessionWaitCond_;
};
}  // namespace DistributedInput
}  // namespace DistributedHardware
}  // namespace OHOS

#endif  // DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H
