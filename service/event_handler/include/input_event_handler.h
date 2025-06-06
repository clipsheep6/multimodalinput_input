/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INPUT_EVENT_HANDLER_H
#define INPUT_EVENT_HANDLER_H

#include "event_dispatch_handler.h"
#include "event_filter_handler.h"
#include "event_interceptor_handler.h"
#include "event_monitor_handler.h"
#include "event_normalize_handler.h"
#include "input_active_subscriber_handler.h"
#include "key_command_handler.h"
#include "key_subscriber_handler.h"
#ifdef OHOS_BUILD_ENABLE_POINTER
#include "mouse_event_normalize.h"
#endif // OHOS_BUILD_ENABLE_POINTER
// #ifdef OHOS_BUILD_ENABLE_SWITCH
#include "switch_subscriber_handler.h"
// #endif // OHOS_BUILD_ENABLE_SWITCH
#include "event_pre_monitor_handler.h"

namespace OHOS {
namespace MMI {
using EventFun = std::function<int32_t(libinput_event *event)>;
using NotifyDeviceChange = std::function<void(int32_t, int32_t, char *)>;
class InputEventHandler final : public std::enable_shared_from_this<InputEventHandler> {
    DECLARE_DELAYED_SINGLETON(InputEventHandler);
public:
    DISALLOW_COPY_AND_MOVE(InputEventHandler);
    void Init(UDSServer& udsServer);
    void OnEvent(void *event, int64_t frameTime);
    UDSServer *GetUDSServer() const;
    int32_t SetMoveEventFilters(bool flag);
    int32_t GetIntervalSinceLastInput(int64_t &timeInterval);

    std::shared_ptr<EventNormalizeHandler> GetEventNormalizeHandler() const;
    std::shared_ptr<EventInterceptorHandler> GetInterceptorHandler() const;
    std::shared_ptr<KeySubscriberHandler> GetSubscriberHandler() const;
    std::shared_ptr<SwitchSubscriberHandler> GetSwitchSubscriberHandler() const;
    std::shared_ptr<KeyCommandHandler> GetKeyCommandHandler() const;
    std::shared_ptr<EventMonitorHandler> GetMonitorHandler() const;
    std::shared_ptr<EventFilterHandler> GetFilterHandler() const;
    std::shared_ptr<EventDispatchHandler> GetEventDispatchHandler() const;
    std::shared_ptr<EventPreMonitorHandler> GetEventPreMonitorHandler() const;
    std::shared_ptr<InputActiveSubscriberHandler> GetInputActiveSubscriberHandler() const;
private:
    int32_t BuildInputHandlerChain();

    // disable-while-typing
    void UpdateDwtRecord(libinput_event *event);
    void UpdateDwtTouchpadRecord(libinput_event *event);
    void UpdateDwtKeyboardRecord(libinput_event *event);
    bool IsStandaloneFunctionKey(uint32_t keycode);
    bool IsModifierKey(uint32_t keycode);
    void RefreshDwtActingState();
    bool IsTouchpadMistouch(libinput_event *event);
    bool IsTouchpadButtonMistouch(libinput_event *event);
    bool IsTouchpadTapMistouch(libinput_event *event);
    bool IsTouchpadMotionMistouch(libinput_event *event);
    bool IsTouchpadPointerMotionMistouch(libinput_event *event);

    UDSServer *udsServer_ { nullptr };
    std::shared_ptr<EventNormalizeHandler> eventNormalizeHandler_ { nullptr };
    std::shared_ptr<EventFilterHandler> eventFilterHandler_ { nullptr };
    std::shared_ptr<EventInterceptorHandler> eventInterceptorHandler_ { nullptr };
    std::shared_ptr<KeySubscriberHandler> eventSubscriberHandler_ { nullptr };
    std::shared_ptr<SwitchSubscriberHandler> switchEventSubscriberHandler_ { nullptr };
    std::shared_ptr<KeyCommandHandler> eventKeyCommandHandler_ { nullptr };
    std::shared_ptr<EventMonitorHandler> eventMonitorHandler_ { nullptr };
    std::shared_ptr<EventDispatchHandler> eventDispatchHandler_ { nullptr };
    std::shared_ptr<EventPreMonitorHandler> eventPreMonitorHandler_ { nullptr };
    std::shared_ptr<InputActiveSubscriberHandler> inputActiveSubscriberHandler_ { nullptr };

    uint64_t idSeed_ { 0 };
    
    // assumption:
    // 1. LIBINPUT_EVENT_TOUCHPAD_* and LIBINPUT_EVENT_POINTER_*_TOUCHPAD are in pairs
    // 2. LIBINPUT_EVENT_POINTER_*_TOUCHPAD always comes after LIBINPUT_EVENT_TOUCHPAD_*
    double touchpadEventDownAbsX_ { 0.0 };
    double touchpadEventDownAbsY_ { 0.0 };
    double touchpadEventAbsX_ { 0.0 };
    double touchpadEventAbsY_ { 0.0 };
    // disable-while-typing
    bool isDwtEdgeAreaForTouchpadMotionActing_ { false };
    bool isDwtEdgeAreaForTouchpadButtonActing_ { false };
    bool isDwtEdgeAreaForTouchpadTapActing_ { false };
    bool isButtonMistouch_ { false };
    bool isTapMistouch_ { false };
    int modifierPressedCount_ { 0 };
    static constexpr uint32_t TOUCHPAD_KEY_CNT = 0x300;
    std::vector<bool> isKeyPressedWithAnyModifiers_ = std::vector<bool>(TOUCHPAD_KEY_CNT, false); // keycode as index

    int64_t lastEventBeginTime_ { 0 };

    static constexpr double TOUCHPAD_EDGE_WIDTH = 15.0;
    static constexpr double TOUCHPAD_EDGE_WIDTH_RELEASE = 20.0;
    static constexpr double TOUCHPAD_EDGE_WIDTH_FOR_BUTTON = 8.0;
    static constexpr double TOUCHPAD_EDGE_WIDTH_FOR_TAP = 25.0;
};
#define InputHandler ::OHOS::DelayedSingleton<InputEventHandler>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_HANDLER_H