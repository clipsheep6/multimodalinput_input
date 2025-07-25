/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef MULTIMODAL_INPUT_CONNECT_MANAGER_H
#define MULTIMODAL_INPUT_CONNECT_MANAGER_H

#include <system_ability_definition.h>

#include "i_input_service_watcher.h"
#include "imultimodal_input_connect.h"

namespace OHOS {
namespace MMI {
class MultimodalInputConnectManager final : public std::enable_shared_from_this<MultimodalInputConnectManager> {
public:
    enum {
        CONNECT_MODULE_TYPE_MMI_CLIENT = 0,
    };
    static constexpr int32_t INVALID_SOCKET_FD = -1;
    static constexpr int32_t MULTIMODAL_INPUT_CONNECT_SERVICE_ID = MULTIMODAL_INPUT_SERVICE_ID;
    ~MultimodalInputConnectManager() = default;
    static std::shared_ptr<MultimodalInputConnectManager> GetInstance();
    int32_t AllocSocketPair(const int32_t moduleType);
    int32_t GetClientSocketFdOfAllocedSocketPair() const;
    int32_t GetTokenType() const
    {
        return tokenType_;
    }
    int32_t AddInputEventFilter(sptr<IEventFilter> filter, int32_t filterId, int32_t priority, uint32_t deviceTags);
    int32_t RemoveInputEventFilter(int32_t filterId);
    int32_t NotifyNapOnline();
    int32_t RemoveInputEventObserver();
    int32_t SetNapStatus(int32_t pid, int32_t uid, const std::string &bundleName, int32_t napStatus);
    int32_t GetDisplayBindInfo(DisplayBindInfos &infos);
    int32_t GetAllMmiSubscribedEvents(std::map<std::tuple<int32_t, int32_t, std::string>, int32_t> &datas);
    int32_t SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg);
    int32_t SetMouseScrollRows(int32_t rows);
    int32_t GetMouseScrollRows(int32_t &rows);
    int32_t SetPointerSize(int32_t size);
    int32_t GetPointerSize(int32_t &size);
    int32_t GetCursorSurfaceId(uint64_t &surfaceId);
    int32_t SetCustomCursor(int32_t windowId, int32_t focusX, int32_t focusY, void* pixelMap);
    int32_t SetCustomCursor(int32_t windowId, CustomCursor cursor, CursorOptions options);
    int32_t SetMouseIcon(int32_t windowId, void* pixelMap);
    int32_t SetMouseHotSpot(int32_t pid, int32_t windowId, int32_t hotSpotX, int32_t hotSpotY);
    int32_t SetMousePrimaryButton(int32_t primaryButton);
    int32_t GetMousePrimaryButton(int32_t &primaryButton);
    int32_t SetHoverScrollState(bool state);
    int32_t GetHoverScrollState(bool &state);
    int32_t SetPointerVisible(bool visible, int32_t priority);
    int32_t IsPointerVisible(bool &visible);
    int32_t MarkProcessed(int32_t eventType, int32_t eventId);
    int32_t SetPointerColor(int32_t color);
    int32_t GetPointerColor(int32_t &color);
    int32_t EnableCombineKey(bool enable);
    int32_t SetPointerSpeed(int32_t speed);
    int32_t GetPointerSpeed(int32_t &speed);
    int32_t SetPointerStyle(int32_t windowId, PointerStyle pointerStyle, bool isUiExtension = false);
    int32_t GetPointerStyle(int32_t windowId, PointerStyle &pointerStyle, bool isUiExtension = false);
    int32_t ClearWindowPointerStyle(int32_t pid, int32_t windowId);
    int32_t SupportKeys(int32_t deviceId, std::vector<int32_t> &keys, std::vector<bool> &keystroke);
    int32_t GetDeviceIds(std::vector<int32_t> &ids);
    int32_t GetDevice(int32_t deviceId, std::shared_ptr<InputDevice> &inputDevice);
    int32_t RegisterDevListener();
    int32_t UnregisterDevListener();
    int32_t GetKeyboardType(int32_t deviceId, int32_t &keyboardType);
    int32_t SetKeyboardRepeatDelay(int32_t delay);
    int32_t SetKeyboardRepeatRate(int32_t rate);
    int32_t GetKeyboardRepeatDelay(int32_t &delay);
    int32_t GetKeyboardRepeatRate(int32_t &rate);
    int32_t AddInputHandler(InputHandlerType handlerType, HandleEventType eventType, int32_t priority,
        uint32_t deviceTags, std::vector<int32_t> actionsType = std::vector<int32_t>());
    int32_t RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType, int32_t priority,
        uint32_t deviceTags, std::vector<int32_t> actionsType = std::vector<int32_t>());
    int32_t AddPreInputHandler(int32_t handlerId, HandleEventType eventType, std::vector<int32_t> keys);
    int32_t RemovePreInputHandler(int32_t handlerId);
    int32_t AddGestureMonitor(InputHandlerType handlerType,
        HandleEventType eventType, TouchGestureType gestureType, int32_t fingers);
    int32_t RemoveGestureMonitor(InputHandlerType handlerType,
        HandleEventType eventType, TouchGestureType gestureType, int32_t fingers);
    int32_t MarkEventConsumed(int32_t eventId);
    int32_t MoveMouseEvent(int32_t offsetX, int32_t offsetY);
    int32_t InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent, bool isNativeInject);
    int32_t SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option);
    int32_t UnsubscribeKeyEvent(int32_t subscribeId);
    int32_t SubscribeHotkey(int32_t subscribeId, const std::shared_ptr<KeyOption> option);
    int32_t UnsubscribeHotkey(int32_t subscribeId);
#ifdef OHOS_BUILD_ENABLE_KEY_PRESSED_HANDLER
    int32_t SubscribeKeyMonitor(const KeyMonitorOption &keyOption);
    int32_t UnsubscribeKeyMonitor(const KeyMonitorOption &keyOption);
#endif // OHOS_BUILD_ENABLE_KEY_PRESSED_HANDLER
    int32_t SubscribeSwitchEvent(int32_t subscribeId, int32_t switchType);
    int32_t UnsubscribeSwitchEvent(int32_t subscribeId);
    int32_t QuerySwitchStatus(int32_t switchType, int32_t& state);
    int32_t SubscribeTabletProximity(int32_t subscribeId);
    int32_t UnsubscribetabletProximity(int32_t subscribeId);
    int32_t SubscribeLongPressEvent(int32_t subscribeId, const LongPressRequest &longPressRequest);
    int32_t UnsubscribeLongPressEvent(int32_t subscribeId);
    int32_t InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent, bool isNativeInject,
        int32_t useCoordinate);
    int32_t InjectTouchPadEvent(std::shared_ptr<PointerEvent> pointerEvent, const TouchpadCDG &touchpadCDG,
        bool isNativeInject);
    int32_t SetAnrObserver();
    int32_t GetFunctionKeyState(int32_t funcKey, bool &state);
    int32_t SetFunctionKeyState(int32_t funcKey, bool enable);
    int32_t SetPointerLocation(int32_t x, int32_t y, int32_t displayId);
    int32_t GetPointerLocation(int32_t &displayId, double &displayX, double &displayY);
    int32_t SetMouseCaptureMode(int32_t windowId, bool isCaptureMode);
    int32_t GetWindowPid(int32_t windowId);
    int32_t AppendExtraData(const ExtraData& extraData);
    int32_t EnableInputDevice(bool enable);
    int32_t SetKeyDownDuration(const std::string &businessId, int32_t delay);
    int32_t SetTouchpadScrollSwitch(bool switchFlag);
    int32_t GetTouchpadScrollSwitch(bool &switchFlag);
    int32_t SetTouchpadScrollDirection(bool state);
    int32_t GetTouchpadScrollDirection(bool &state);
    int32_t SetTouchpadTapSwitch(bool switchFlag);
    int32_t GetTouchpadTapSwitch(bool &switchFlag);
    int32_t SetTouchpadPointerSpeed(int32_t speed);
    int32_t GetTouchpadPointerSpeed(int32_t &speed);
    int32_t GetTouchpadCDG(TouchpadCDG &touchpadCDG);
    int32_t SetTouchpadPinchSwitch(bool switchFlag);
    int32_t GetTouchpadPinchSwitch(bool &switchFlag);
    int32_t SetTouchpadSwipeSwitch(bool switchFlag);
    int32_t GetTouchpadSwipeSwitch(bool &switchFlag);
    int32_t SetTouchpadRightClickType(int32_t type);
    int32_t GetTouchpadRightClickType(int32_t &type);
    int32_t SetTouchpadRotateSwitch(bool rotateSwitch);
    int32_t GetTouchpadRotateSwitch(bool &rotateSwitch);
    int32_t SetTouchpadDoubleTapAndDragState(bool switchFlag);
    int32_t GetTouchpadDoubleTapAndDragState(bool &switchFlag);
    int32_t SetShieldStatus(int32_t shieldMode, bool isShield);
    int32_t GetShieldStatus(int32_t shieldMode, bool &isShield);
    int32_t GetKeyState(std::vector<int32_t> &pressedKeys, std::map<int32_t, int32_t> &specialKeysState);
    int32_t Authorize(bool isAuthorize);
    int32_t CancelInjection();
    int32_t RequestInjection(int32_t &status, int32_t &reqId);
    int32_t QueryAuthorizedStatus(int32_t &status);
    int32_t HasIrEmitter(bool &hasIrEmitter);
    int32_t GetInfraredFrequencies(std::vector<InfraredFrequency>& requencys);
    int32_t TransmitInfrared(int64_t number, std::vector<int64_t>& pattern);
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
    int32_t CreateVKeyboardDevice(sptr<IRemoteObject> &vkeyboardDevice);
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
    int32_t SetMoveEventFilters(bool flag);
    void AddServiceWatcher(std::shared_ptr<IInputServiceWatcher> watcher);
    void RemoveServiceWatcher(std::shared_ptr<IInputServiceWatcher> watcher);
    int32_t SetPixelMapData(int32_t infoId, void* pixelMap);
    int32_t SetCurrentUser(int32_t userId);
    int32_t SetTouchpadThreeFingersTapSwitch(bool switchFlag);
    int32_t GetTouchpadThreeFingersTapSwitch(bool &switchFlag);
    int32_t EnableHardwareCursorStats(bool enable);
    int32_t GetHardwareCursorStats(uint32_t &frameCount, uint32_t &vsyncCount);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t GetPointerSnapshot(void *pixelMapPtr);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t AddVirtualInputDevice(std::shared_ptr<InputDevice> device, int32_t &deviceId);
    int32_t RemoveVirtualInputDevice(int32_t deviceId);
    int32_t SetTouchpadScrollRows(int32_t rows);
    int32_t GetTouchpadScrollRows(int32_t &rows);
    int32_t SkipPointerLayer(bool isSkip);
    int32_t SetClientInfo(int32_t pid, uint64_t readThreadId);
    int32_t GetIntervalSinceLastInput(int64_t &timeInterval);

#ifdef OHOS_BUILD_ENABLE_ANCO
    int32_t AncoAddChannel(sptr<IAncoChannel> channel);
    int32_t AncoRemoveChannel(sptr<IAncoChannel> channel);
    int32_t CheckKnuckleEvent(float pointX, float pointY, bool &isKnuckleType);
#endif // OHOS_BUILD_ENABLE_ANCO

    int32_t GetAllSystemHotkeys(std::vector<std::unique_ptr<KeyOption>> &keyOptions);
    int32_t SetInputDeviceEnabled(int32_t deviceId, bool enable, int32_t index);
    int32_t ShiftAppPointerEvent(const ShiftWindowParam &param, bool autoGenDown);
    int32_t SetMultiWindowScreenId(uint64_t screenId, uint64_t displayNodeScreenId);
    int32_t SetKnuckleSwitch(bool knuckleSwitch);
    int32_t LaunchAiScreenAbility();
    int32_t GetMaxMultiTouchPointNum(int32_t &pointNum);
    int32_t SetInputDeviceConsumer(const std::vector<std::string>& deviceNames);
    int32_t ClearInputDeviceConsumer(const std::vector<std::string>& deviceNames);
    int32_t SubscribeInputActive(int32_t subscribeId, int64_t interval);
    int32_t UnsubscribeInputActive(int32_t subscribeId);
    int32_t SetMouseAccelerateMotionSwitch(int32_t deviceId, bool enable);
    int32_t SwitchScreenCapturePermission(uint32_t permissionType, bool enable);
    int32_t ClearMouseHideFlag(int32_t eventId);
    int32_t QueryPointerRecord(int32_t count, std::vector<std::shared_ptr<PointerEvent>> &pointerList);

private:
    MultimodalInputConnectManager() = default;
    DISALLOW_COPY_AND_MOVE(MultimodalInputConnectManager);

    bool ConnectMultimodalInputService();
    void OnDeath(const wptr<IRemoteObject> &remoteObj);
    void Clean(const wptr<IRemoteObject> &remoteObj);
    void NotifyServiceDeath();
    void NotifyDeath();
    sptr<IMultimodalInputConnect> multimodalInputConnectService_ { nullptr };
    sptr<IRemoteObject::DeathRecipient> multimodalInputConnectRecipient_ { nullptr };
    int32_t socketFd_ { MultimodalInputConnectManager::INVALID_SOCKET_FD };
    int32_t tokenType_ { -1 };
    std::mutex lock_;
    std::set<std::shared_ptr<IInputServiceWatcher>> watchers_;
};
} // namespace MMI
} // namespace OHOS
#define MULTIMODAL_INPUT_CONNECT_MGR MultimodalInputConnectManager::GetInstance()
#endif // MULTIMODAL_INPUT_CONNECT_MANAGER_H