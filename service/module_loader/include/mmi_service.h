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

#include <mutex>
#include <thread>

#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "nocopyable.h"
#include "singleton.h"
#include "system_ability.h"

#include "entrust_tasks.h"
#include "input_event_handler.h"
#include "i_multimodal_input_connect.h"
#include "libinput_adapter.h"
#include "multimodal_input_connect_define.h"
#include "remote_msg_handler.h"
#include "server_msg_handler.h"
#include "uds_server.h"

#ifdef OHOS_BUILD_HDF
    #include "hdf_event_manager.h"
#endif

namespace OHOS {
namespace MMI {

enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class MMIService : public UDSServer, public SystemAbility, public IRemoteStub<IMultimodalInputConnect> {
    DECLARE_DELAYED_SINGLETON(MMIService);
    DECLEAR_SYSTEM_ABILITY(MMIService);
    DISALLOW_COPY_AND_MOVE(MMIService);

public:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual void OnDump() override;
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& options) override;

protected:
    virtual void OnConnected(SessionPtr s) override;
    virtual void OnDisconnected(SessionPtr s) override;
    virtual int32_t AddEpoll(EpollEventType type, int32_t fd) override;

    bool InitLibinputService();
    bool InitService();
    bool InitSignalHandler();
    bool InitEntrustTasks();
    int32_t Init();

    void OnTimer();
    void OnThread();
    void OnSignalEvent(int32_t signalFd);
    void OnEntrustTask(epoll_event& ev);

private:
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    int32_t mmiFd_ = -1;
    std::mutex mu_;
    std::thread t_;

    EntrustTasks entrustTasks_;
    LibinputAdapter libinputAdapter_;
    ServerMsgHandler sMsgHandler_;
    RemoteMsgHandler rMsgHandler_;
};
} // namespace MMI
} // namespace OHOS
#endif // MMI_SERVICE_H