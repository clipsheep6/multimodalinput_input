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
#include "event_dispatch.h"
#include "event_package.h"
#include "i_event_filter.h"
#include "mouse_event_handler.h"
#include "msg_handler.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {
using EventFun = std::function<int32_t(const multimodal_libinput_event& ev)>;
using NotifyDeviceChange = std::function<void(int32_t, int32_t, char *)>;
class InputEventHandler : public MsgHandler<EventFun>, public DelayedSingleton<InputEventHandler> {
public:
    InputEventHandler();
    virtual ~InputEventHandler() override;
    bool Init(UDSServer& udsServer);
    void OnEvent(void *event);
    void OnCheckEventReport();
    int32_t OnMouseEventEndTimerHandler(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent);
    UDSServer *GetUDSServer();
    int32_t AddInputEventFilter(sptr<IEventFilter> filter);
protected:
    int32_t OnEventDeviceAdded(const multimodal_libinput_event& event);
    int32_t OnEventDeviceRemoved(const multimodal_libinput_event& event);
    int32_t OnEventKeyboard(const multimodal_libinput_event& event);
    int32_t OnEventPointer(const multimodal_libinput_event& event);
    int32_t OnEventTouch(const multimodal_libinput_event& event);
    int32_t OnEventTouchSecond(struct libinput_event *event);
    int32_t OnEventTouchPadSecond(struct libinput_event *event);
    int32_t OnEventGesture(const multimodal_libinput_event& event);
    int32_t OnEventTouchpad(const multimodal_libinput_event& event);
    int32_t OnGestureEvent(struct libinput_event *event);
    int32_t OnEventTabletTool(const multimodal_libinput_event& event);
    int32_t OnEventTabletPad(const multimodal_libinput_event& event);
    int32_t OnEventSwitchToggle(const multimodal_libinput_event& event);
    int32_t OnEventJoyStickKey(const multimodal_libinput_event& event, const uint64_t time);
    int32_t OnEventTabletPadKey(const multimodal_libinput_event& event);
    int32_t OnEventJoyStickAxis(const multimodal_libinput_event& event, const uint64_t time);
    int32_t OnKeyboardEvent(struct libinput_event *event);
    int32_t OnKeyEventDispatch(const multimodal_libinput_event& event);
    
    int32_t OnMouseEventHandler(struct libinput_event *event);
    bool SendMsg(const int32_t fd, NetPacket& pkt) const;
#ifdef OHOS_WESTEN_MODEL
    bool OnSystemEvent(const KeyEventValueTransformations& temp, const enum KEY_STATE state) const;
#endif

private:
    int32_t OnEventHandler(const multimodal_libinput_event& ev);
    int32_t OnEventKey(struct libinput_event *event);
    std::mutex mu_;
    UDSServer *udsServer_ = nullptr;
    EventDispatch eventDispatch_;
    EventPackage eventPackage_;
    KeyEventValueTransformation xkbKeyboardHandlerKey_;
    NotifyDeviceChange notifyDeviceChange_;
    std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent_;

    uint64_t idSeed_ = 0;
    int32_t eventType_ = 0;
    uint64_t initSysClock_ = 0;
    uint64_t lastSysClock_ = 0;
};
} // namespace MMI
} // namespace OHOS
#define InputHandler OHOS::MMI::InputEventHandler::GetInstance()
#endif // INPUT_EVENT_HANDLER_H