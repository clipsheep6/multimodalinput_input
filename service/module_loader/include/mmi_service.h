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

#include "system_ability.h"

#include "app_debug_listener.h"
#include "input_event_handler.h"
#include "libinput_adapter.h"
#include "multimodal_input_connect_stub.h"
#include "server_msg_handler.h"

namespace OHOS {
namespace MMI {
#if defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)
class TouchGestureManager;
#endif // defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)

enum class ServiceRunningState {STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class MMIService final : public UDSServer, public SystemAbility, public MultimodalInputConnectStub {
    DECLARE_SYSTEM_ABILITY(MMIService);

public:
    void OnStart() override;
    void OnStop() override;
    static MMIService* GetInstance();
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    int32_t AllocSocketFd(const std::string &programName, const int32_t moduleType,
        int32_t &toReturnClientFd, int32_t &tokenType) override;
    int32_t AddInputEventFilter(sptr<IEventFilter> filter, int32_t filterId, int32_t priority,
        uint32_t deviceTags) override;
    int32_t RemoveInputEventFilter(int32_t filterId) override;
    int32_t SetPointerSize(int32_t size) override;
    int32_t GetPointerSize(int32_t &size) override;
    int32_t GetCursorSurfaceId(uint64_t &surfaceId) override;
    int32_t SetMouseScrollRows(int32_t rows) override;
    int32_t GetMouseScrollRows(int32_t &rows) override;
    int32_t SetCustomCursor(int32_t windowId, int32_t focusX, int32_t focusY, void* pixelMap) override;
    int32_t SetCustomCursor(int32_t windowId, CustomCursor cursor, CursorOptions options) override;
    int32_t SetMouseIcon(int32_t windowId, void* pixelMap) override;
    int32_t ClearWindowPointerStyle(int32_t pid, int32_t windowId) override;
    int32_t SetMouseHotSpot(int32_t pid, int32_t windowId, int32_t hotSpotX, int32_t hotSpotY) override;
    int32_t SetNapStatus(int32_t pid, int32_t uid, std::string bundleName, int32_t napState) override;
    int32_t SetMousePrimaryButton(int32_t primaryButton) override;
    int32_t GetMousePrimaryButton(int32_t &primaryButton) override;
    int32_t SetHoverScrollState(bool state) override;
    int32_t GetHoverScrollState(bool &state) override;
    int32_t SetPointerVisible(bool visible, int32_t priority) override;
    int32_t IsPointerVisible(bool &visible) override;
    int32_t MarkProcessed(int32_t eventType, int32_t eventId) override;
    int32_t SetPointerColor(int32_t color) override;
    int32_t GetPointerColor(int32_t &color) override;
    int32_t EnableCombineKey(bool enable) override;
    int32_t SetPointerSpeed(int32_t speed) override;
    int32_t GetPointerSpeed(int32_t &speed) override;
    int32_t SetPointerStyle(int32_t windowId, PointerStyle pointerStyle, bool isUiExtension = false) override;
    int32_t NotifyNapOnline() override;
    int32_t RemoveInputEventObserver() override;
    int32_t GetPointerStyle(int32_t windowId, PointerStyle &pointerStyle, bool isUiExtension = false) override;
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
        int32_t priority, uint32_t deviceTags, std::vector<int32_t> actionsType = std::vector<int32_t>()) override;
    int32_t RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags, std::vector<int32_t> actionsType = std::vector<int32_t>()) override;
    int32_t AddPreInputHandler(int32_t handlerId, HandleEventType eventType, std::vector<int32_t> keys) override;
    int32_t RemovePreInputHandler(int32_t handlerId) override;
    int32_t AddGestureMonitor(InputHandlerType handlerType,
        HandleEventType eventType, TouchGestureType gestureType, int32_t fingers) override;
    int32_t RemoveGestureMonitor(InputHandlerType handlerType,
        HandleEventType eventType, TouchGestureType gestureType, int32_t fingers) override;
    int32_t MarkEventConsumed(int32_t eventId) override;
    int32_t MoveMouseEvent(int32_t offsetX, int32_t offsetY) override;
    int32_t InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent, bool isNativeInject) override;
    int32_t SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option) override;
    int32_t UnsubscribeKeyEvent(int32_t subscribeId) override;
    int32_t SubscribeHotkey(int32_t subscribeId, const std::shared_ptr<KeyOption> option) override;
    int32_t UnsubscribeHotkey(int32_t subscribeId) override;
#ifdef OHOS_BUILD_ENABLE_KEY_PRESSED_HANDLER
    int32_t SubscribeKeyMonitor(const KeyMonitorOption &keyOption) override;
    int32_t UnsubscribeKeyMonitor(const KeyMonitorOption &keyOption) override;
#endif // OHOS_BUILD_ENABLE_KEY_PRESSED_HANDLER
    int32_t SubscribeSwitchEvent(int32_t subscribeId, int32_t switchType) override;
    int32_t UnsubscribeSwitchEvent(int32_t subscribeId) override;
    int32_t QuerySwitchStatus(int32_t switchType, int32_t& state) override;
    int32_t SubscribeTabletProximity(int32_t subscribeId) override;
    int32_t UnsubscribetabletProximity(int32_t subscribeId) override;
    int32_t SubscribeLongPressEvent(int32_t subscribeId, const LongPressRequest &longPressRequest) override;
    int32_t UnsubscribeLongPressEvent(int32_t subscribeId) override;
    int32_t InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent, bool isNativeInject) override;
    int32_t InjectTouchPadEvent(const std::shared_ptr<PointerEvent> pointerEvent,
        const TouchpadCDG &touchpadCDG, bool isNativeInject) override;
    int32_t SetAnrObserver() override;
    int32_t GetDisplayBindInfo(DisplayBindInfos &infos) override;
    int32_t GetAllMmiSubscribedEvents(std::map<std::tuple<int32_t, int32_t, std::string>,
        int32_t> &datas) override;
    int32_t SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg) override;
    int32_t GetFunctionKeyState(int32_t funcKey, bool &state) override;
    int32_t SetFunctionKeyState(int32_t funcKey, bool enable) override;
    int32_t SetPointerLocation(int32_t x, int32_t y, int32_t displayId) override;
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
    int32_t GetTouchpadCDG(TouchpadCDG &touchpadCDG) override;
    int32_t SetTouchpadPinchSwitch(bool switchFlag) override;
    int32_t GetTouchpadPinchSwitch(bool &switchFlag) override;
    int32_t SetTouchpadSwipeSwitch(bool switchFlag) override;
    int32_t GetTouchpadSwipeSwitch(bool &switchFlag) override;
    int32_t SetTouchpadRightClickType(int32_t type) override;
    int32_t GetTouchpadRightClickType(int32_t &type) override;
    int32_t SetTouchpadRotateSwitch(bool rotateSwitch) override;
    int32_t GetTouchpadRotateSwitch(bool &rotateSwitch) override;
    int32_t SetTouchpadDoubleTapAndDragState(bool switchFlag) override;
    int32_t GetTouchpadDoubleTapAndDragState(bool &switchFlag) override;
    int32_t SetShieldStatus(int32_t shieldMode, bool isShield) override;
    int32_t GetShieldStatus(int32_t shieldMode, bool &isShield) override;
    int32_t GetKeyState(std::vector<int32_t> &pressedKeys, std::map<int32_t, int32_t> &specialKeysState) override;
    int32_t Authorize(bool isAuthorize) override;
    int32_t CancelInjection() override;
    int32_t SetMoveEventFilters(bool flag) override;
#if defined(OHOS_RSS_CLIENT) && !defined(OHOS_BUILD_PC_PRIORITY)
    void OnAddResSchedSystemAbility(int32_t systemAbilityId, const std::string &deviceId);
#endif // defined(OHOS_RSS_CLIENT) && !defined(OHOS_BUILD_PC_PRIORITY)
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    int32_t HasIrEmitter(bool &hasIrEmitter) override;
    int32_t GetInfraredFrequencies(std::vector<InfraredFrequency>& frequencies) override;
    int32_t TransmitInfrared(int64_t number, std::vector<int64_t>& pattern) override;
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
    int32_t CreateVKeyboardDevice(sptr<IRemoteObject> &vkeyboardDevice) override;
    int32_t OnCreateVKeyboardDevice(sptr<IRemoteObject> &vkeyboardDevice);
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
    int32_t OnHasIrEmitter(bool &hasIrEmitter);
    int32_t SetPixelMapData(int32_t infoId, void* pixelMap) override;
    int32_t SetCurrentUser(int32_t userId) override;
    int32_t SetTouchpadThreeFingersTapSwitch(bool switchFlag) override;
    int32_t GetTouchpadThreeFingersTapSwitch(bool &switchFlag) override;
    int32_t AddVirtualInputDevice(std::shared_ptr<InputDevice> device, int32_t &deviceId) override;
    int32_t RemoveVirtualInputDevice(int32_t deviceId) override;
    int32_t EnableHardwareCursorStats(bool enable) override;
    int32_t GetHardwareCursorStats(uint32_t &frameCount, uint32_t &vsyncCount) override;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t GetPointerSnapshot(void *pixelMapPtr) override;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t TransferBinderClientSrv(const sptr<IRemoteObject> &binderClientObject) override;
    int32_t SetTouchpadScrollRows(int32_t rows) override;
    int32_t GetTouchpadScrollRows(int32_t &rows) override;
    int32_t SkipPointerLayer(bool isSkip) override;
    void CalculateFuntionRunningTime(std::function<void()> func, const std::string &flag);
    int32_t SetClientInfo(int32_t pid, uint64_t readThreadId) override;
    int32_t GetIntervalSinceLastInput(int64_t &timeInterval) override;
#ifdef OHOS_BUILD_ENABLE_ANCO
    void InitAncoUds();
    void StopAncoUds();
    int32_t InjectKeyEventExt(const std::shared_ptr<KeyEvent> keyEvent, int32_t pid, bool isNativeInject);
    int32_t InjectPointerEventExt(const std::shared_ptr<PointerEvent> pointerEvent, int32_t pid,
        bool isNativeInject, bool isShell);
    int32_t AncoAddChannel(sptr<IAncoChannel> channel) override;
    int32_t AncoRemoveChannel(sptr<IAncoChannel> channel) override;
    int32_t CheckKnuckleEvent(float pointX, float pointY, bool &isKnuckleType) override;
    int32_t SyncKnuckleStatus();
#endif // OHOS_BUILD_ENABLE_ANCO
#if defined(OHOS_BUILD_ENABLE_MONITOR) && defined(PLAYER_FRAMEWORK_EXISTS)
    static void ScreenCaptureCallback(int32_t pid, bool isStart);
    void RegisterScreenCaptureCallback();
#endif // OHOS_BUILD_ENABLE_MONITOR && PLAYER_FRAMEWORK_EXISTS

    int32_t OnGetAllSystemHotkey(std::vector<std::unique_ptr<KeyOption>> &keyOptions);
    int32_t GetAllSystemHotkeys(std::vector<std::unique_ptr<KeyOption>> &keyOptions) override;
    int32_t SetInputDeviceEnabled(int32_t deviceId, bool enable, int32_t index) override;
    int32_t ShiftAppPointerEvent(const ShiftWindowParam &param, bool autoGenDown) override;
    int32_t SetMultiWindowScreenId(uint64_t screenId, uint64_t displayNodeScreenId) override;
    int32_t SetMultiWindowScreenIdInner(uint64_t screenId, uint64_t displayNodeScreenId);
    int32_t SetKnuckleSwitch(bool knuckleSwitch) override;
    int32_t LaunchAiScreenAbility() override;

protected:
    void OnConnected(SessionPtr s) override;
    void OnDisconnected(SessionPtr s) override;
    int32_t AddEpoll(EpollEventType type, int32_t fd, bool readOnly = false) override;
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
    int32_t ReadTouchpadCDG(TouchpadCDG &touchpadCDG);
    int32_t ReadTouchpadPinchSwitch(bool &switchFlag);
    int32_t ReadTouchpadSwipeSwitch(bool &switchFlag);
    int32_t ReadTouchpadRightMenuType(int32_t &type);
    int32_t ReadTouchpadRotateSwitch(bool &rotateSwitch);
    int32_t ReadTouchpadDoubleTapAndDragState(bool &switchFlag);
    int32_t ReadTouchpadScrollRows(int32_t &rows);
#endif // OHOS_BUILD_ENABLE_POINTER
    int32_t OnRegisterDevListener(int32_t pid);
    int32_t OnUnregisterDevListener(int32_t pid);
    int32_t OnGetDeviceIds(std::vector<int32_t> &ids);
    int32_t OnGetDevice(int32_t deviceId, std::shared_ptr<InputDevice> inputDevice);
    int32_t OnSupportKeys(int32_t deviceId, std::vector<int32_t> &keys, std::vector<bool> &keystroke);
    int32_t OnGetKeyboardType(int32_t deviceId, int32_t &keyboardType);
    int32_t OnGetWindowPid(int32_t windowId, int32_t &windowPid);
#if defined(OHOS_BUILD_ENABLE_INTERCEPTOR) || defined(OHOS_BUILD_ENABLE_MONITOR)
    int32_t CheckAddInput(int32_t pid, InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags);
    int32_t CheckRemoveInput(int32_t pid, InputHandlerType handlerType, HandleEventType eventType,
        int32_t priority, uint32_t deviceTags);
    int32_t ObserverAddInputHandler(int32_t pid);
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR || OHOS_BUILD_ENABLE_MONITOR
    int32_t CheckMarkConsumed(int32_t pid, int32_t eventId);
    int32_t CheckInjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent, int32_t pid, bool isNativeInject);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    int32_t OnGetKeyState(std::vector<int32_t> &pressedKeys, std::map<int32_t, int32_t> &specialKeysState);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    int32_t CheckInjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent,
        int32_t pid, bool isNativeInject, bool isShell);
    int32_t CheckTouchPadEvent(const std::shared_ptr<PointerEvent> pointerEvent,
        int32_t pid, const TouchpadCDG &touchpadCDG, bool isNativeInject, bool isShell);
    bool InitLibinputService();
    bool InitService();
    bool InitSignalHandler();
    bool InitDelegateTasks();
    int32_t Init();
    void InitPreferences();
#ifdef OHOS_BUILD_PC_PRIORITY
    void SetMmiServicePriority();
#endif // OHOS_BUILD_PC_PRIORITY
    void OnThread();
    void PreEventLoop();
    void OnSignalEvent(int32_t signalFd);
    void OnDelegateTask(epoll_event& ev);

    void AddReloadDeviceTimer();
#if defined(OHOS_BUILD_ENABLE_KEYBOARD) && defined(OHOS_BUILD_ENABLE_COMBINATION_KEY)
    int32_t UpdateSettingsXml(const std::string &businessId, int32_t delay);
#endif // OHOS_BUILD_ENABLE_KEYBOARD && OHOS_BUILD_ENABLE_COMBINATION_KEY
    void AddAppDebugListener();
    void RemoveAppDebugListener();
#if defined(OHOS_BUILD_ENABLE_KEYBOARD) && defined(OHOS_BUILD_ENABLE_COMBINATION_KEY)
    int32_t UpdateCombineKeyState(bool enable);
#endif // OHOS_BUILD_ENABLE_KEYBOARD && OHOS_BUILD_ENABLE_COMBINATION_KEY
    int32_t OnAuthorize(bool isAuthorize);
    int32_t OnCancelInjection(int32_t callPid = 0);
    void InitPrintClientInfo();
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
    void InitVKeyboardFuncHandler();
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
    int32_t SetInputDeviceEnable(int32_t deviceId, bool enable, int32_t index, int32_t pid, SessionPtr sess);
private:
    MMIService();
    ~MMIService();

    int32_t CheckPidPermission(int32_t pid);
    void PrintLog(const std::string &flag, int32_t duration, int32_t pid, int32_t tid);
    void OnSessionDelete(SessionPtr session);
#if defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)
    void SetupTouchGestureHandler();
#endif // defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)

    std::atomic<ServiceRunningState> state_ = ServiceRunningState::STATE_NOT_START;
    int32_t mmiFd_ { -1 };
    std::atomic<bool> isCesStart_ { false };
    std::mutex mu_;
    std::thread t_;
    std::thread eventMonitorThread_;
#ifdef OHOS_BUILD_ENABLE_ANCO
    int32_t shellAssitentPid_ { -1 };
#endif // OHOS_BUILD_ENABLE_ANCO
#ifdef OHOS_RSS_CLIENT
    std::atomic<uint64_t> tid_ = 0;
#endif // OHOS_RSS_CLIENT
    LibinputAdapter libinputAdapter_;
    ServerMsgHandler sMsgHandler_;
    DelegateTasks delegateTasks_;
#if defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)
    std::shared_ptr<TouchGestureManager> touchGestureMgr_ { nullptr };
#endif // defined(OHOS_BUILD_ENABLE_TOUCH) && defined(OHOS_BUILD_ENABLE_MONITOR)
    std::shared_ptr<DelegateInterface> delegateInterface_ { nullptr };
    sptr<AppDebugListener> appDebugListener_;
    std::atomic_bool threadStatusFlag_ { false };
    struct ClientInfo {
        int32_t pid { -1 };
        uint64_t readThreadId { -1 };
    };
    std::map<std::string, ClientInfo> clientInfos_;
    std::mutex mutex_;
#if defined(OHOS_BUILD_ENABLE_MONITOR) && defined(PLAYER_FRAMEWORK_EXISTS)
    bool hasRegisterListener_ { false };
#endif // OHOS_BUILD_ENABLE_MONITOR && PLAYER_FRAMEWORK_EXISTS
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
    std::atomic_bool isHPR_ { false };
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
};
} // namespace MMI
} // namespace OHOS
#endif // MMI_SERVICE_H
