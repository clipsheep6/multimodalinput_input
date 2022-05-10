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

#ifndef I_UDS_SERVER_H
#define I_UDS_SERVER_H

#include "iremote_broker.h"

namespace OHOS {
namespace MMI {
class IUdsServer : public RefBase {
public:
    static IUdsServer *GetInstance();
    virtual bool SendMsg(int32_t fd, NetPacket& pkt) = 0;
    virtual int32_t AddSocketPairInfo(const std::string& programName, const int32_t moduleType, const int32_t uid,
        const int32_t pid, int32_t& serverFd, int32_t& toReturnClientFd) = 0;
    virtual int32_t GetClientFd(int32_t pid) = 0;
    virtual SessionPtr GetSession(int32_t fd) const = 0;
    virtual void AddSessionDeletedCallback(std::function<void(SessionPtr)> callback) = 0;
    virtual int32_t HandleNonConsumedTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;
    virtual int32_t HandleTimerPointerEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;
    virtual std::shared_ptr<KeyEvent> GetKeyEvent() const = 0;
    virtual void Dump(int32_t fd) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_UDS_SERVER_H