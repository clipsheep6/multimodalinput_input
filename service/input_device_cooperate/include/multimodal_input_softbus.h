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

#ifndef MULTIMODAL_INPUT_CONNECT_REMOTER_H
#define MULTIMODAL_INPUT_CONNECT_REMOTER_H

#include <map>
#include <memory>
#include <string>

#include "nlohmann/json.hpp"
#include "nocopyable.h"
#include "session.h"

#include "i_multimodal_input_connect.h"

namespace OHOS {
namespace MMI {
class MultimodalInputSoftbus {
public:
    virtual ~MultimodalInputSoftbus() = default;
    static std::shared_ptr<MultimodalInputSoftbus> GetInstance();
    int32_t StartRemoteCooperate(const std::string &localDeviceId, const std::string &remoteDeviceId);
    int32_t StartRemoteCooperateResult(const std::string &remoteDeviceId, bool isSuccess,
        const std::string &startDhid, int32_t xPercent, int32_t yPercent);
    int32_t StopRemoteCooperate(const std::string &remoteDeviceId);
    int32_t StopRemoteCooperateResult(const std::string &remoteDeviceId, bool isSuccess);
    int32_t StartCooperateOtherResult(const std::string &remoteDeviceId, const std::string &srcNetworkId);

    int32_t Init();
    void Release();
    int32_t OpenInputSoftbus(const std::string &remoteDevId);
    void CloseInputSoftbus(const std::string &remoteDevId);

    int32_t OnSessionOpened(int32_t sessionId, int32_t result);
    void OnSessionClosed(int32_t sessionId);
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);

private:
    MultimodalInputSoftbus() = default;
    DISALLOW_COPY_AND_MOVE(MultimodalInputSoftbus);
    std::string FindDeviceBySession(int32_t sessionId);
    int32_t SendMsg(int32_t sessionId, std::string &message);
    int32_t CheckDeviceSessionState(const std::string &remoteDevId);
    void HandleSessionData(int32_t sessionId, const std::string& messageData);

    void NotifyResponseStartRemoteCooperate(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStartRemoteCooperateResult(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStopRemoteCooperate(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStopRemoteCooperateResult(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStartCooperateOtherResult(int32_t sessionId, const nlohmann::json &recMsg);

    std::map<std::string, int32_t> sessionDevMap_;
    std::map<std::string, bool> channelStatusMap_;
    std::mutex operationMutex_;
    std::string mySessionName_ = "";
    std::condition_variable openSessionWaitCond_;
    ISessionListener sessListener_;
};
} // namespace MMI
} // namespace OHOS
#define SoftbusMgr MultimodalInputSoftbus::GetInstance()
#endif // MULTIMODAL_INPUT_CONNECT_REMOTER_H
