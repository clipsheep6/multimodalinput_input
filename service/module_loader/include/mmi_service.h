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

#ifndef MMI_SERVICE_H
#define MMI_SERVICE_H

#ifdef OHOS_RSS_CLIENT
#include <atomic>
#endif
#include <mutex>
#include <thread>

#include "iremote_object.h"
#include "nocopyable.h"
#include "singleton.h"
#include "system_ability.h"

#include "input_event_handler.h"
#include "multimodal_input_connect_stub.h"
#include "libinput_adapter.h"
#include "server_msg_handler.h"
#include "uds_server.h"

#ifdef OHOS_BUILD_HDF
    #include "hdf_event_manager.h"
#endif

namespace OHOS {
namespace MMI {

enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class MMIService : public UDSServer, public SystemAbility, public MultimodalInputConnectStub {
    DECLARE_DELAYED_SINGLETON(MMIService);
    DECLEAR_SYSTEM_ABILITY(MMIService);
    DISALLOW_COPY_AND_MOVE(MMIService);

public:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual void OnDump() override;
    virtual int32_t AllocSocketFd(const std::string &programName, const int32_t moduleType, int32_t &socketFd) override;
    virtual int32_t AddInputEventFilter(sptr<IEventFilter> filter) override;
    virtual int32_t SetPointerVisible(bool visible) override;
    virtual int32_t IsPointerVisible(bool &visible) override;
    virtual int32_t SetPointerLocation(int32_t x, int32_t y) override;

#if (defined OHOS_RSS_CLIENT) || (defined OHOS_DISTRIBUTED_INPUT_MODEL)
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
#endif
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    virtual int32_t GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity) override;
    virtual int32_t PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput) override;
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

protected:
    virtual void OnConnected(SessionPtr s) override;
    virtual void OnDisconnected(SessionPtr s) override;
    virtual int32_t StubHandleAllocSocketFd(MessageParcel &data, MessageParcel &reply) override;

    virtual int32_t AddEpoll(EpollEventType type, int32_t fd) override;

    bool InitLibinputService();
    bool InitService();
    bool InitSignalHandler();
    int32_t Init();

    void OnTimer();
    void OnThread();
    void OnSignalEvent(int32_t signalFd);

private:
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    int32_t mmiFd_ = -1;
    std::mutex mu_;
    std::thread t_;
#ifdef OHOS_RSS_CLIENT
    std::atomic<uint64_t> tid_ = 0;
#endif

    LibinputAdapter libinputAdapter_;
    ServerMsgHandler sMsgHandler_;
};
} // namespace MMI
} // namespace OHOS
#endif // MMI_SERVICE_H
