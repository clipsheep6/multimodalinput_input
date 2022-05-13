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

#ifndef INPUT_EVENT_HANDLER_H
#define INPUT_EVENT_HANDLER_H

#include <memory>

#include "nocopyable.h"
#include "singleton.h"

#include "event_dispatch.h"
#include "i_event_filter.h"
#include "i_input_event_handler.h"
#include "i_interceptor_handler_global.h"
#include "i_input_south_event_handler.h"
#include "input_handler_manager_global.h"
#include "key_event_subscriber.h"
#include "mouse_event_handler.h"
#include "event_filter_wrap.h"
#include "msg_handler.h"
#include "key_event_handler.h"
#include "pointer_event_handler.h"
#include "touch_event_handler.h"

namespace OHOS {
namespace MMI {
using EventFun = std::function<int32_t(libinput_event *event)>;
using NotifyDeviceChange = std::function<void(int32_t, int32_t, char *)>;
class InputEventHandler : public IInputSouthEventHandler, public MsgHandler<EventFun>,
    public DelayedSingleton<InputEventHandler> {
public:
    InputEventHandler();
    DISALLOW_COPY_AND_MOVE(InputEventHandler);
    virtual ~InputEventHandler() override;
    void Init();
    void OnEvent(void *event);
    void OnCheckEventReport();
    std::shared_ptr<KeyEvent> GetKeyEvent() const override;

    int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) override;
    int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
    int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) override;

    int32_t AddKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemoveKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    int32_t AddPointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemovePointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    int32_t AddTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemoveTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;  

    void AddKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemoveKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void AddPointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemovePointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void AddTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemoveTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void TouchMonitorHandlerMarkConsumed(int32_t monitorId, int32_t eventId, SessionPtr sess) override;
    int32_t AddSubscriber(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption) override;
    int32_t RemoveSubscriber(SessionPtr sess, int32_t subscribeId) override;

    int32_t AddFilter(sptr<IEventFilter> filter) override;

protected:
    int32_t OnEventDeviceAdded(libinput_event *event);
    int32_t OnEventDeviceRemoved(libinput_event *event);
    int32_t OnEventPointer(libinput_event *event);
    int32_t OnEventTouch(libinput_event *event);
    int32_t OnEventGesture(libinput_event *event);
    int32_t OnEventTouchpad(libinput_event *event);
    int32_t OnTabletToolEvent(libinput_event *event);
    int32_t OnEventKey(libinput_event *event);

private:
    int32_t OnEventHandler(libinput_event *event);
    void BuildInputHandlerChain();

    NotifyDeviceChange notifyDeviceChange_;
    std::shared_ptr<KeyEvent> keyEvent_ = nullptr;

    std::shared_ptr<IInputEventHandler> keyEventHandler_ = nullptr;
    std::shared_ptr<IInputEventHandler> pointerEventHandler_ = nullptr;
    std::shared_ptr<IInputEventHandler> touchEventHandler_ = nullptr;

    uint64_t idSeed_ = 0;
    int32_t eventType_ = 0;
    int64_t initSysClock_ = 0;
    int64_t lastSysClock_ = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_HANDLER_H
