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

#ifndef SERVER_MSG_HANDLER_H
#define SERVER_MSG_HANDLER_H

#include "nocopyable.h"

#include "event_dispatch.h"
#include "msg_handler.h"

namespace OHOS {
namespace MMI {
typedef std::function<int32_t(SessionPtr sess, NetPacket& pkt)> ServerMsgFun;
class ServerMsgHandler : public MsgHandler<MmiMessageId, ServerMsgFun> {
public:
    ServerMsgHandler();
    DISALLOW_COPY_AND_MOVE(ServerMsgHandler);
    virtual ~ServerMsgHandler() override;

    void Init(UDSServer& udsServer);
    void OnMsgHandler(SessionPtr sess, NetPacket& pkt);

protected:
    int32_t OnRegisterMsgHandler(SessionPtr sess, NetPacket& pkt);
#ifdef OHOS_BUILD_HDF
    int32_t OnHdiInject(SessionPtr sess, NetPacket& pkt);
#endif
    int32_t MarkProcessed(SessionPtr sess, NetPacket& pkt);
    int32_t OnInjectKeyEvent(SessionPtr sess, NetPacket& pkt);
    int32_t OnInjectPointerEvent(SessionPtr sess, NetPacket& pkt);
    int32_t OnDisplayInfo(SessionPtr sess, NetPacket& pkt);
    int32_t OnAddInputHandler(SessionPtr sess, NetPacket& pkt);
    int32_t OnRemoveInputHandler(SessionPtr sess, NetPacket& pkt);
    int32_t OnMarkConsumed(SessionPtr sess, NetPacket& pkt);
    int32_t OnInputDevice(SessionPtr sess, NetPacket& pkt);
    int32_t OnInputDeviceIds(SessionPtr sess, NetPacket& pkt);
    int32_t OnSupportKeys(SessionPtr sess, NetPacket& pkt);
    int32_t OnInputKeyboardType(SessionPtr sess, NetPacket& pkt);
    int32_t OnAddInputDeviceMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnRemoveInputDeviceMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnAddInputEventMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnRemoveInputEventMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnAddInputEventTouchpadMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnRemoveInputEventTouchpadMontior(SessionPtr sess, NetPacket& pkt);
    int32_t OnSubscribeKeyEvent(SessionPtr sess, NetPacket& pkt);
    int32_t OnUnSubscribeKeyEvent(SessionPtr sess, NetPacket& pkt);
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    int32_t OnMoveMouse(SessionPtr sess, NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
#ifdef OHOS_BUILD_MMI_DEBUG
    int32_t OnBigPacketTest(SessionPtr sess, NetPacket& pkt);
#endif // OHOS_BUILD_MMI_DEBUG
private:
    UDSServer *udsServer_ = nullptr;
    EventDispatch eventDispatch_;
    std::shared_ptr<KeyEvent> keyEvent_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // SERVER_MSG_HANDLER_H