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

#include "server_msg_handler.h"

#include <cinttypes>

#include "anr_manager.h"
#include "event_dump.h"
#include "event_interceptor_handler.h"
#include "event_monitor_handler.h"
#include "hos_key_event.h"
#include "input_device_manager.h"
#include "input_event.h"
#include "input_event_data_transformation.h"
#include "input_event_handler.h"
#include "input_windows_manager.h"
#include "i_pointer_drawing_manager.h"
#include "key_event_normalize.h"
#include "key_subscriber_handler.h"
#include "libinput_adapter.h"
#include "mmi_func_callback.h"
#include "mouse_event_normalize.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ServerMsgHandler" };
} // namespace

void ServerMsgHandler::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
}

void ServerMsgHandler::OnMsgHandler(SessionPtr sess, NetPacket& pkt)
{
    CHKPV(sess);
    auto id = pkt.GetMsgId();
    TimeCostChk chk("ServerMsgHandler::OnMsgHandler", "overtime 300(us)", MAX_OVER_TIME, id);
    auto callback = GetMsgCallback(id);
    if (callback == nullptr) {
        MMI_HILOGE("Unknown msg id:%{public}d,errCode:%{public}d", id, UNKNOWN_MSG_ID);
        return;
    }
    auto ret = (*callback)(sess, pkt);
    if (ret < 0) {
        MMI_HILOGE("Msg handling failed. id:%{public}d,errCode:%{public}d", id, ret);
    }
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t ServerMsgHandler::OnInjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
    inputEventNormalizeHandler->HandleKeyEvent(keyEvent);
    MMI_HILOGD("Inject keyCode:%{public}d, action:%{public}d", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
    return RET_OK;
}

int32_t ServerMsgHandler::OnGetFunctionKeyState(int32_t funcKey, bool &state)
{
    CALL_INFO_TRACE;
    const auto &keyEvent = KeyEventHdr->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    state = keyEvent->GetFunctionKey(funcKey);
    MMI_HILOGD("Get the function key:%{public}d status as %{public}s", funcKey, state ? "open" : "close");
    return RET_OK;
}

int32_t ServerMsgHandler::OnSetFunctionKeyState(int32_t funcKey, bool enable)
{
    CALL_INFO_TRACE;
    auto device = InputDevMgr->GetKeyboardDevice();
    CHKPR(device, ERROR_NULL_POINTER);
    if (LibinputAdapter::DeviceLedUpdate(device, funcKey, enable) != RET_OK) {
        MMI_HILOGE("Failed to set the keyboard led");
        return RET_ERR;
    }
    int32_t state = libinput_get_funckey_state(device, funcKey);

    auto keyEvent = KeyEventHdr->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    int32_t ret = keyEvent->SetFunctionKey(funcKey, state);
    if (ret != funcKey) {
        MMI_HILOGE("Failed to enable the function key");
        return RET_ERR;
    }
    MMI_HILOGD("Update function key:%{public}d succeed", funcKey);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t ServerMsgHandler::OnInjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    pointerEvent->UpdateId();
    int32_t action = pointerEvent->GetPointerAction();
    auto source = pointerEvent->GetSourceType();
    switch (source) {
        case PointerEvent::SOURCE_TYPE_TOUCHSCREEN: {
#ifdef OHOS_BUILD_ENABLE_TOUCH
            auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
            CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
            if (!FixTargetWindowId(pointerEvent, action)) {
                return RET_ERR;
            }
            inputEventNormalizeHandler->HandleTouchEvent(pointerEvent);
#endif // OHOS_BUILD_ENABLE_TOUCH
            break;
        }
        case PointerEvent::SOURCE_TYPE_MOUSE:
#ifdef OHOS_BUILD_ENABLE_JOYSTICK
        case PointerEvent::SOURCE_TYPE_JOYSTICK:
#endif // OHOS_BUILD_ENABLE_JOYSTICK
        case PointerEvent::SOURCE_TYPE_TOUCHPAD: {
#ifdef OHOS_BUILD_ENABLE_POINTER
            auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
            CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
            if (!IPointerDrawingManager::GetInstance()->IsPointerVisible()) {
                IPointerDrawingManager::GetInstance()->SetPointerVisible(getpid(), true);
            }
            inputEventNormalizeHandler->HandlePointerEvent(pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER
            break;
        }
        default: {
            MMI_HILOGW("Source type is unknown, source:%{public}d", source);
            break;
        }
    }
    if (source == PointerEvent::SOURCE_TYPE_TOUCHSCREEN && action == PointerEvent::POINTER_ACTION_DOWN) {
        targetWindowId_ = pointerEvent->GetTargetWindowId();
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_TOUCH
bool ServerMsgHandler::FixTargetWindowId(std::shared_ptr<PointerEvent> pointerEvent, int32_t action)
{
    if (action == PointerEvent::POINTER_ACTION_DOWN || targetWindowId_ < 0) {
        MMI_HILOGD("Down event or targetWindowId_ less 0 is not need fix window id");
        return true;
    }
    pointerEvent->SetTargetWindowId(targetWindowId_);
    PointerEvent::PointerItem pointerItem;
    auto pointerIds = pointerEvent->GetPointerIds();
    if (pointerIds.empty()) {
        MMI_HILOGE("GetPointerIds is empty");
        return false;
    }
    auto id = pointerIds.front();
    if (!pointerEvent->GetPointerItem(id, pointerItem)) {
        MMI_HILOGE("Can't find pointer item");
        return false;
    }
    pointerItem.SetTargetWindowId(targetWindowId_);
    pointerEvent->UpdatePointerItem(id, pointerItem);
    return true;
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#if defined(OHOS_BUILD_ENABLE_INTERCEPTOR) || defined(OHOS_BUILD_ENABLE_MONITOR)
int32_t ServerMsgHandler::OnAddInputHandler(SessionPtr sess, InputHandlerType handlerType,
    HandleEventType eventType, int32_t priority, uint32_t deviceTags)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    MMI_HILOGD("handlerType:%{public}d", handlerType);
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        auto interceptorHandler = InputHandler->GetInterceptorHandler();
        CHKPR(interceptorHandler, ERROR_NULL_POINTER);
        return interceptorHandler->AddInputHandler(handlerType, eventType, priority, deviceTags, sess);
    }
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
#ifdef OHOS_BUILD_ENABLE_MONITOR
    if (handlerType == InputHandlerType::MONITOR) {
        auto monitorHandler = InputHandler->GetMonitorHandler();
        CHKPR(monitorHandler, ERROR_NULL_POINTER);
        return monitorHandler->AddInputHandler(handlerType, eventType, sess);
    }
#endif // OHOS_BUILD_ENABLE_MONITOR
    return RET_OK;
}

int32_t ServerMsgHandler::OnRemoveInputHandler(SessionPtr sess, InputHandlerType handlerType,
    HandleEventType eventType, int32_t priority, uint32_t deviceTags)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    MMI_HILOGD("OnRemoveInputHandler handlerType:%{public}d eventType:%{public}u", handlerType, eventType);
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    if (handlerType == InputHandlerType::INTERCEPTOR) {
        auto interceptorHandler = InputHandler->GetInterceptorHandler();
        CHKPR(interceptorHandler, ERROR_NULL_POINTER);
        interceptorHandler->RemoveInputHandler(handlerType, eventType, priority, deviceTags, sess);
    }
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR
#ifdef OHOS_BUILD_ENABLE_MONITOR
    if (handlerType == InputHandlerType::MONITOR) {
        auto monitorHandler = InputHandler->GetMonitorHandler();
        CHKPR(monitorHandler, ERROR_NULL_POINTER);
        monitorHandler->RemoveInputHandler(handlerType, eventType, sess);
    }
#endif // OHOS_BUILD_ENABLE_MONITOR
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR || OHOS_BUILD_ENABLE_MONITOR

#ifdef OHOS_BUILD_ENABLE_MONITOR
int32_t ServerMsgHandler::OnMarkConsumed(SessionPtr sess, int32_t eventId)
{
    CHKPR(sess, ERROR_NULL_POINTER);
    auto monitorHandler = InputHandler->GetMonitorHandler();
    CHKPR(monitorHandler, ERROR_NULL_POINTER);
    monitorHandler->MarkConsumed(eventId, sess);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_MONITOR

#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
int32_t ServerMsgHandler::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    CALL_DEBUG_ENTER;
    if (MouseEventHdr->NormalizeMoveMouse(offsetX, offsetY)) {
        auto pointerEvent = MouseEventHdr->GetPointerEvent();
        CHKPR(pointerEvent, ERROR_NULL_POINTER);
        auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
        CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
        inputEventNormalizeHandler->HandlePointerEvent(pointerEvent);
        MMI_HILOGD("Mouse movement message processed successfully");
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t ServerMsgHandler::OnSubscribeKeyEvent(IUdsServer *server, int32_t pid,
    int32_t subscribeId, const std::shared_ptr<KeyOption> option)
{
    CALL_DEBUG_ENTER;
    CHKPR(server, ERROR_NULL_POINTER);
    auto sess = server->GetSessionByPid(pid);
    CHKPR(sess, ERROR_NULL_POINTER);
    auto subscriberHandler = InputHandler->GetSubscriberHandler();
    CHKPR(subscriberHandler, ERROR_NULL_POINTER);
    return subscriberHandler->SubscribeKeyEvent(sess, subscribeId, option);
}

int32_t ServerMsgHandler::OnUnsubscribeKeyEvent(IUdsServer *server, int32_t pid, int32_t subscribeId)
{
    CALL_DEBUG_ENTER;
    CHKPR(server, ERROR_NULL_POINTER);
    auto sess = server->GetSessionByPid(pid);
    CHKPR(sess, ERROR_NULL_POINTER);
    auto subscriberHandler = InputHandler->GetSubscriberHandler();
    CHKPR(subscriberHandler, ERROR_NULL_POINTER);
    return subscriberHandler->UnsubscribeKeyEvent(sess, subscribeId);
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t ServerMsgHandler::AddInputEventFilter(sptr<IEventFilter> filter,
    int32_t filterId, int32_t priority, uint32_t deviceTags, int32_t clientPid)
{
    auto filterHandler = InputHandler->GetFilterHandler();
    CHKPR(filterHandler, ERROR_NULL_POINTER);
    return filterHandler->AddInputEventFilter(filter, filterId, priority, deviceTags, clientPid);
}

int32_t ServerMsgHandler::RemoveInputEventFilter(int32_t clientPid, int32_t filterId)
{
    auto filterHandler = InputHandler->GetFilterHandler();
    CHKPR(filterHandler, ERROR_NULL_POINTER);
    return filterHandler->RemoveInputEventFilter(clientPid, filterId);   
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS
