/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef MMI_SERVICE_H
#define MMI_SERVICE_H

#include <atomic>
#include <mutex>
#include <thread>

#include "iremote_object.h"
#include "singleton.h"
#include "system_ability.h"

#include "app_debug_listener.h"
#include "delegate_tasks.h"
#include "input_event_handler.h"
#include "libinput_adapter.h"
#include "multimodal_input_connect_stub.h"
#include "server_msg_handler.h"
#include "uds_server.h"
#include "nap_process.h"

namespace OHOS {
namespace MMI {

enum class ServiceRunningState {STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class MMIService final : public UDSServer, public SystemAbility, public MultimodalInputConnectStub {
    DECLARE_DELAYED_SINGLETON(MMIService);
    DECLARE_SYSTEM_ABILITY(MMIService);
    DISALLOW_COPY_AND_MOVE(MMIService);

public:
    void OnStart() override;
    void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    int32_t AllocSocketFd(const std::string &programName, const int32_t moduleType,
        int32_t &toReturnClientFd, int32_t &tokenType) override;
    int32_t AddInputEventFilter(sptr<IEventFilter> filter, int32_t filterId, int32_t priority,
        uint32_t deviceTags) override;
    int32_t RemoveInputEventFilter(int32_t filterId) override;
    int32_t SetPointerSize(int32_t size) override;
    int32_t GetPointerSize(int32_t &size) override;
    int32_t SetMouseScrollRows(int32_t rows) override;
    int32_t GetMouseScrollRows(int32_t &rows) override;
    int32_t SetCustomCursor(int32_t pid, int32_t windowId, int32_t focusX, int32_t focusY, void* pixelMap) override;
    int32_t SetMouseIcon(int32_t pid, int32_t windowId, void* pixelMap) override;
    int32_t ClearWindowPointerStyle(int32_t pid, int32_t windowId) override;
    int32_t SetMouseHotSpot(int32_t pid, int32_t windowId, int32_t hotSpotX, int32_t hotSpotY) override;
    int32_t SetNapStatus(int32_t pid, int32_t uid, std::string bundleName, int32_t napState) override;
    int32_t SetMousePrimaryButton(int32_t primaryButton) override;
    int32_t GetMousePrimaryButton(int32_t &primaryButton) override;
    int32_t SetHoverScrollState(bool state) override;
    int32_t GetHoverScrollState(bool &state) override;
    int32_t SetPointerVisible(bool visible) override;
    int32_t IsPointerVisible(bool &visible) override;
    int32_t MarkProcessed(int32_t eventType, int32_t eventId) override;
    int32_t SetPointerColor(int32_t color) override;
    int32_t GetPointerColor(int32_t &color) override;
    int32_t EnableCombineKey(bool enable) override;
    int32_t SetPointerSpeed(int32_t speed) override;
    int32_t GetPointerSpeed(int32_t &speed) override;
    int32_t SetPointerStyle(int32_t windowId, PointerStyle pointerStyle) override;
    int32_t NotifyNapOnline() override;
    int32_t RemoveInputEventObserver() override;
    int32_t GetPointerStyle(int32_t windowId, PointerStyle &pointerStyle) override;
    int32_t SupportKeys(int32_t deviceId, std::vector<int32_t> &keys, std::vector<bool> &keystroke) override;
    int32_t GetDeviceIds(std::vector<int32_t> &ids) override;
    int32_t GetDevice(int32_t deviceId, std::shared_ptr<InputDevice> &inputDevice) override;
    int32_t RegisterDevListener() override;
    int32_t UnregisterDevListener() override;
    int32_t GetKeyboardType(int32_t deviceId, int32_t &keyboardType) override;
    int32_t SetKeyboardRepeatDelay(int32_t delay) override;
    int32_t SetKeyboardRepeatRate(int32_t rate) override;
    int32_t GetKeyboardRepeatDelay(int32_t &delay) override;
    int32_t GetKeyboardRepeatRate(int32_t &rate) override;
    int32_t AddInputHandler(InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags) override;
    int32_t RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags) override;
    int32_t MarkEventConsumed(int32_t eventId) override;
    int32_t MoveMouseEvent(int32_t offsetX, int32_t offsetY) override;
    int32_t InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent) override;
    int32_t SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option) override;
    int32_t UnsubscribeKeyEvent(int32_t subscribeId) override;
    int32_t SubscribeSwitchEvent(int32_t subscribeId) override;
    int32_t UnsubscribeSwitchEvent(int32_t subscribeId) override;
    int32_t InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent) override;
    int32_t SetAnrObserver() override;
    int32_t GetDisplayBindInfo(DisplayBindInfos &infos) override;
    int32_t GetAllMmiSubscribedEvents(std::map<std::tuple<int32_t, int32_t, std::string>,
        int32_t> &datas) override;
    int32_t SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg) override;
    int32_t GetFunctionKeyState(int32_t funcKey, bool &state) override;
    int32_t SetFunctionKeyState(int32_t funcKey, bool enable) override;
    int32_t SetPointerLocation(int32_t x, int32_t y) override;
    virtual int32_t SetMouseCaptureMode(int32_t windowId, bool isCaptureMode) override;
    int32_t GetWindowPid(int32_t windowId) override;
    int32_t AppendExtraData(const ExtraData& extraData) override;
    int32_t EnableInputDevice(bool enable) override;
    int32_t SetKeyDownDuration(const std::string &businessId, int32_t delay) override;
    int32_t SetTouchpadScrollSwitch(bool switchFlag) override;
    int32_t GetTouchpadScrollSwitch(bool &switchFlag) override;
    int32_t SetTouchpadScrollDirection(bool state) override;
    int32_t GetTouchpadScrollDirection(bool &state) override;
    int32_t SetTouchpadTapSwitch(bool switchFlag) override;
    int32_t GetTouchpadTapSwitch(bool &switchFlag) override;
    int32_t SetTouchpadPointerSpeed(int32_t speed) override;
    int32_t GetTouchpadPointerSpeed(int32_t &speed) override;
    int32_t SetTouchpadPinchSwitch(bool switchFlag) override;
    int32_t GetTouchpadPinchSwitch(bool &switchFlag) override;
    int32_t SetTouchpadSwipeSwitch(bool switchFlag) override;
    int32_t GetTouchpadSwipeSwitch(bool &switchFlag) override;
    int32_t SetTouchpadRightClickType(int32_t type) override;
    int32_t GetTouchpadRightClickType(int32_t &type) override;
    int32_t SetShieldStatus(int32_t shieldMode, bool isShield) override;
    int32_t GetShieldStatus(int32_t shieldMode, bool &isShield) override;
#ifdef OHOS_RSS_CLIENT
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
#endif

#ifdef OHOS_BUILD_ENABLE_CONTAINER
    void InitContainer();
    void StopContainer();
    int32_t InjectKeyEventExt(const std::shared_ptr<KeyEvent> keyEvent);
    int32_t InjectPointerEventExt(const std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_CONTAINER

protected:
    void OnConnected(SessionPtr s) override;
    void OnDisconnected(SessionPtr s) override;
    int32_t AddEpoll(EpollEventType type, int32_t fd) override;
    int32_t DelEpoll(EpollEventType type, int32_t fd);
    bool IsRunning() const override;
#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
    int32_t CheckPointerVisible(bool &visible);
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING
#ifdef OHOS_BUILD_ENABLE_POINTER
    int32_t ReadMouseScrollRows(int32_t &rows);
    int32_t ReadPointerSize(int32_t &size);
    int32_t ReadMousePrimaryButton(int32_t &primaryButton);
    int32_t ReadPointerSpeed(int32_t &speed);
    int32_t ReadHoverScrollState(bool &state);
    int32_t ReadPointerColor(int32_t &color);
    int32_t ReadTouchpadScrollSwich(bool &switchFlag);
    int32_t ReadTouchpadScrollDirection(bool &state);
    int32_t ReadTouchpadTapSwitch(bool &switchFlag);
    int32_t ReadTouchpadPointerSpeed(int32_t &speed);
    int32_t ReadTouchpadPinchSwitch(bool &switchFlag);
    int32_t ReadTouchpadSwipeSwitch(bool &switchFlag);
    int32_t ReadTouchpadRightMenuType(int32_t &type);
#endif // OHOS_BUILD_ENABLE_POINTER
    int32_t OnRegisterDevListener(int32_t pid);
    int32_t OnUnregisterDevListener(int32_t pid);
    int32_t OnGetDeviceIds(std::vector<int32_t> &ids);
    int32_t OnGetDevice(int32_t deviceId, std::shared_ptr<InputDevice> &inputDevice);
    int32_t OnSupportKeys(int32_t deviceId, std::vector<int32_t> &keys, std::vector<bool> &keystroke);
    int32_t OnGetKeyboardType(int32_t deviceId, int32_t &keyboardType);
    int32_t OnGetWindowPid(int32_t windowId, int32_t &windowPid);
#if defined(OHOS_BUILD_ENABLE_INTERCEPTOR) || defined(OHOS_BUILD_ENABLE_MONITOR)
    int32_t CheckAddInput(int32_t pid, InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags);
    int32_t CheckRemoveInput(int32_t pid, InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags);
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR || OHOS_BUILD_ENABLE_MONITOR
    int32_t CheckMarkConsumed(int32_t pid, int32_t eventId);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    int32_t CheckInjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    int32_t CheckInjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
    bool InitLibinputService();
    bool InitService();
    bool InitSignalHandler();
    bool InitDelegateTasks();
    int32_t Init();

    void OnThread();
    void OnSignalEvent(int32_t signalFd);
    void OnDelegateTask(epoll_event& ev);

    void AddReloadDeviceTimer();
    int32_t UpdateSettingsXml(const std::string &businessId, int32_t delay);
    void AddAppDebugListener();
    void RemoveAppDebugListener();
    int32_t UpdateCombineKeyState(bool enable);

private:
    std::atomic<ServiceRunningState> state_ = ServiceRunningState::STATE_NOT_START;
    int32_t mmiFd_ { -1 };
    std::mutex mu_;
    std::thread t_;
#ifdef OHOS_RSS_CLIENT
    std::atomic<uint64_t> tid_ = 0;
#endif
    LibinputAdapter libinputAdapter_;
    ServerMsgHandler sMsgHandler_;
    DelegateTasks delegateTasks_;
    sptr<AppDebugListener> appDebugListener_;

    std::atomic_bool threadStatusFlag_ { false };
};
} // namespace MMI
} // namespace OHOS
#endif // MMI_SERVICE_H
