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

#include "interceptor_handler_global.h"

#include "define_multimodal.h"
#include "event_dispatch.h"
#include "input_event_data_transformation.h"
#include "input_event_handler.h"
#include "mmi_log.h"
#include "net_packet.h"
#include "proto.h"
#include "util_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InterceptorHandlerGlobal" };
} // namespace

InterceptorHandlerGlobal::InterceptorHandlerGlobal() {}

int32_t InterceptorHandlerGlobal::AddInputHandler(int32_t handlerId,
    InputHandlerType handlerType, HandleEventType eventType, SessionPtr session)
{
    CHKPR(session, RET_ERR);
    if (!IsValidHandlerId(handlerId)) {
        MMI_HILOGE("Invalid handler");
        return RET_ERR;
    }
    if (handlerType != InputHandlerType::INTERCEPTOR) {
        MMI_HILOGW("Invalid handler type:%{public}d", handlerType);
        return RET_ERR;
    }
    InitSessionLostCallback();
    MMI_HILOGD("Register interceptor:%{public}d", handlerId);
    SessionHandler interceptor { handlerId, handlerType, eventType, session };
    return interceptors_.AddInterceptor(interceptor);
}

void InterceptorHandlerGlobal::RemoveInputHandler(int32_t handlerId,
    InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(session);
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        MMI_HILOGD("Unregister interceptor:%{public}d", handlerId);
        SessionHandler interceptor { handlerId, handlerType, HandleEventType::ALL, session };
        interceptors_.RemoveInterceptor(interceptor);
    }
}

bool InterceptorHandlerGlobal::HandleEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    MMI_HILOGD("Handle KeyEvent");
    CHKPF(keyEvent);
    if (keyEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT)) {
        MMI_HILOGW("This event has been tagged as not to be intercepted");
        return false;
    }
    return interceptors_.HandleEvent(keyEvent);
}

bool InterceptorHandlerGlobal::HandleEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPF(pointerEvent);
    if (pointerEvent->HasFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT)) {
        MMI_HILOGW("This event has been tagged as not to be intercepted");
        return false;
    }
    return interceptors_.HandleEvent(pointerEvent);
}

void InterceptorHandlerGlobal::InitSessionLostCallback()
{
    if (sessionLostCallbackInitialized_)  {
        MMI_HILOGE("Init session is failed");
        return;
    }
    auto udsServerPtr = InputHandler->GetUDSServer();
    CHKPV(udsServerPtr);
    udsServerPtr->AddSessionDeletedCallback(std::bind(
        &InterceptorHandlerGlobal::OnSessionLost, this, std::placeholders::_1));
    sessionLostCallbackInitialized_ = true;
    MMI_HILOGD("The callback on session deleted is registered successfully");
}

void InterceptorHandlerGlobal::OnSessionLost(SessionPtr session)
{
    interceptors_.OnSessionLost(session);
}

void InterceptorHandlerGlobal::SessionHandler::SendToClient(std::shared_ptr<KeyEvent> keyEvent) const
{
    CHKPV(keyEvent);
    NetPacket pkt(MmiMessageId::REPORT_KEY_EVENT);
    pkt << id_;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write key event failed");
        return;
    }
    if (InputEventDataTransformation::KeyEventToNetPacket(keyEvent, pkt) != RET_OK) {
        MMI_HILOGE("Packet key event failed, errCode:%{public}d", STREAM_BUF_WRITE_FAIL);
        return;
    }
    if (!session_->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}

void InterceptorHandlerGlobal::SessionHandler::SendToClient(std::shared_ptr<PointerEvent> pointerEvent) const
{
    CHKPV(pointerEvent);
    NetPacket pkt(MmiMessageId::REPORT_POINTER_EVENT);
    MMI_HILOGD("Service SendToClient id:%{public}d,InputHandlerType:%{public}d", id_, handlerType_);
    pkt << id_ << handlerType_;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write pointer event failed");
        return;
    }
    if (InputEventDataTransformation::Marshalling(pointerEvent, pkt) != RET_OK) {
        MMI_HILOGE("Marshalling pointer event failed, errCode:%{public}d", STREAM_BUF_WRITE_FAIL);
        return;
    }
    if (!session_->SendMsg(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
        return;
    }
}

int32_t InterceptorHandlerGlobal::InterceptorCollection::GetPriority() const
{
    return IInputEventHandler::DEFAULT_INTERCEPTOR;
}

bool InterceptorHandlerGlobal::InterceptorCollection::HandleEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPF(keyEvent);
    if (interceptors_.empty()) {
        MMI_HILOGW("Key interceptors is empty");
        return false;
    }
    MMI_HILOGD("There are currently:%{public}zu interceptors", interceptors_.size());
    bool isInterceptor = false;
    for (const auto &interceptor : interceptors_) {
        if (interceptor.eventType_ == HandleEventType::KEY
            || interceptor.eventType_ == HandleEventType::ALL) {
            interceptor.SendToClient(keyEvent);
            MMI_HILOGD("Key event was intercepted");
            isInterceptor = true;
        }
    }
    return isInterceptor;
}

bool InterceptorHandlerGlobal::InterceptorCollection::HandleEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPF(pointerEvent);
    if (interceptors_.empty()) {
        MMI_HILOGE("Pointer interceptors is empty");
        return false;
    }
    MMI_HILOGD("There are currently:%{public}zu interceptors", interceptors_.size());
    bool isInterceptor = false;
    for (const auto &interceptor : interceptors_) {
        if (interceptor.eventType_ == HandleEventType::ALL) {
            interceptor.SendToClient(pointerEvent);
            MMI_HILOGD("Pointer event was intercepted");
            isInterceptor = true;
        }
    }
    return isInterceptor;
}

int32_t InterceptorHandlerGlobal::InterceptorCollection::AddInterceptor(const SessionHandler& interceptor)
{
    if (interceptors_.size() >= MAX_N_INPUT_INTERCEPTORS) {
        MMI_HILOGE("The number of interceptors exceeds limit");
        return RET_ERR;
    }
    auto ret = interceptors_.insert(interceptor);
    if (!ret.second) {
        MMI_HILOGW("Duplicate interceptors");
    }
    MMI_HILOGD("Register interceptor successfully");
    return RET_OK;
}

void InterceptorHandlerGlobal::InterceptorCollection::RemoveInterceptor(const SessionHandler& interceptor)
{
    std::set<SessionHandler>::const_iterator tItr = interceptors_.find(interceptor);
    if (tItr != interceptors_.cend()) {
        interceptors_.erase(tItr);
    }
    MMI_HILOGD("Unregister interceptor successfully");
}

void InterceptorHandlerGlobal::InterceptorCollection::OnSessionLost(SessionPtr session)
{
    std::set<SessionHandler>::const_iterator cItr = interceptors_.cbegin();
    while (cItr != interceptors_.cend()) {
        if (cItr->session_ != session) {
            ++cItr;
        } else {
            cItr = interceptors_.erase(cItr);
        }
    }
}
void InterceptorHandlerGlobal::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    return interceptors_.Dump(fd, args);
}

void InterceptorHandlerGlobal::InterceptorCollection::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    CALL_LOG_ENTER;
    MMI_HILOGI("Interceptor Dump in !");
    mprintf(fd, "--------------------------[interceptor information]-------------------------");
    mprintf(fd, "interceptors: count=%d", interceptors_.size());
    for (const auto &interceptor : interceptors_){
        mprintf(fd,
                "interceptor id:%d | handlerType:%d | Pid:%d | Uid:%d | Fd:%d | EarlistEventTime:%" PRId64 "\t",
                interceptor.id_, interceptor.handlerType_, interceptor.session_->GetPid(), 
                interceptor.session_->GetUid(), interceptor.session_->GetFd(),
                interceptor.session_->GetEarlistEventTime());
    }
}
} // namespace MMI
} // namespace OHOS
