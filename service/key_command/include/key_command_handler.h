/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef KEY_COMMAND_HANDLER_H
#define KEY_COMMAND_HANDLER_H

#include "old_display_info.h"

#include "i_input_event_handler.h"
#include "input_handler_type.h"
#include <mutex>

namespace OHOS {
namespace MMI {
enum KeyCommandType : int32_t {
    TYPE_SHORTKEY = 0,
    TYPE_SEQUENCE = 1,
    TYPE_FINGERSCENE = 2,
    TYPE_REPEAT_KEY = 3,
    TYPE_MULTI_FINGERS = 4,
};

enum class KnuckleType : int32_t {
    KNUCKLE_TYPE_SINGLE = 0,
    KNUCKLE_TYPE_DOUBLE = 1,
};

enum class NotifyType : int32_t {
    CANCEL,
    INCONSISTENTGESTURE,
    REGIONGESTURE,
    LETTERGESTURE,
    OTHER
};

struct Ability {
    std::string bundleName;
    std::string abilityName;
    std::string action;
    std::string type;
    std::string deviceId;
    std::string uri;
    std::string abilityType;
    std::vector<std::string> entities;
    std::map<std::string, std::string> params;
};

struct ShortcutKey {
    std::set<int32_t> preKeys;
    std::string businessId;
    std::string statusConfig;
    bool statusConfigValue { true };
    int32_t finalKey { -1 };
    int32_t keyDownDuration { 0 };
    int32_t triggerType { KeyEvent::KEY_ACTION_DOWN };
    int32_t timerId { -1 };
#ifdef SHORTCUT_KEY_MANAGER_ENABLED
    int32_t shortcutId { -1 };
#endif // SHORTCUT_KEY_MANAGER_ENABLED
    Ability ability;
    void Print() const;
};

struct SequenceKey {
    int32_t keyCode { -1 };
    int32_t keyAction { 0 };
    int64_t actionTime { 0 };
    int64_t delay { 0 };
    bool operator!=(const SequenceKey &sequenceKey)
    {
        return (keyCode != sequenceKey.keyCode) || (keyAction != sequenceKey.keyAction);
    }
};

struct ExcludeKey {
    int32_t keyCode { -1 };
    int32_t keyAction { -1 };
    int64_t delay { 0 };
};

struct Sequence {
    std::vector<SequenceKey> sequenceKeys;
    std::string statusConfig;
    bool statusConfigValue { true };
    int64_t abilityStartDelay { 0 };
    int32_t timerId { -1 };
    Ability ability;
    friend std::ostream& operator<<(std::ostream&, const Sequence&);
};

struct TwoFingerGesture {
    inline static constexpr auto MAX_TOUCH_NUM = 2;
    bool active = false;
    int32_t timerId = -1;
    int64_t abilityStartDelay = 0;
    int64_t startTime = 0;
    int32_t windowId = -1;
    int32_t windowPid = -1;
    bool longPressFlag = false;
    std::shared_ptr<PointerEvent> touchEvent = nullptr;
    Ability ability;
    struct {
        int32_t id { 0 };
        int32_t x { 0 };
        int32_t y { 0 };
        int64_t downTime { 0 };
    } touches[MAX_TOUCH_NUM];
};

struct KnuckleGesture {
    std::shared_ptr<PointerEvent> lastPointerDownEvent { nullptr };
    bool state { false };
    int64_t lastPointerUpTime { 0 };
    int64_t downToPrevUpTime { 0 };
    float doubleClickDistance { 0.0f };
    Ability ability;
    struct {
        int32_t id { 0 };
        int32_t x { 0 };
        int32_t y { 0 };
    } lastDownPointer;
};

struct RepeatKey {
    int32_t keyCode { -1 };
    int32_t keyAction { 0 };
    int32_t times { 0 };
    int64_t actionTime { 0 };
    int64_t delay { 0 };
    std::string statusConfig;
    bool statusConfigValue { true };
    Ability ability;
};

struct MultiFingersTap {
    Ability ability;
};

struct KnuckleSwitch {
    std::string statusConfig { "" };
    bool statusConfigValue { false };
};

using MistouchPreventionCallbackFunc = std::function<void(int32_t)>;
class IMistouchPrevention  {
public:
    IMistouchPrevention() = default;
    virtual ~IMistouchPrevention() = default;
 
    virtual int32_t MistouchPreventionConnector(MistouchPreventionCallbackFunc callbackFunc) = 0;

    virtual int32_t MistouchPreventionClose() = 0;
};

class KeyCommandHandler final : public IInputEventHandler, public std::enable_shared_from_this<KeyCommandHandler> {
public:
    KeyCommandHandler() = default;
    DISALLOW_COPY_AND_MOVE(KeyCommandHandler);
    ~KeyCommandHandler() override;
    int32_t UpdateSettingsXml(const std::string &businessId, int32_t delay);
    int32_t EnableCombineKey(bool enable);
    KnuckleGesture GetSingleKnuckleGesture() const;
    KnuckleGesture GetDoubleKnuckleGesture() const;
    void Dump(int32_t fd, const std::vector<std::string> &args);
    void PrintGestureInfo(int32_t fd);
    std::string ConvertKeyActionToString(int32_t keyAction);
    int32_t RegisterKnuckleSwitchByUserId(int32_t userId);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent) override;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    void HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    void HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent) override;
    void HandlePointerActionDownEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void HandlePointerActionMoveEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void HandlePointerActionUpEvent(const std::shared_ptr<PointerEvent> touchEvent);
#endif // OHOS_BUILD_ENABLE_TOUCH
    void SetKnuckleDoubleTapDistance(float distance);
    bool GetKnuckleSwitchValue();
    bool SkipKnuckleDetect();
    bool CheckInputMethodArea(const std::shared_ptr<PointerEvent> touchEvent);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    bool OnHandleEvent(const std::shared_ptr<KeyEvent> keyEvent);
    int32_t SetIsFreezePowerKey(const std::string pageName);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    bool OnHandleEvent(const std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
    void InitKeyObserver();
    bool PreHandleEvent();
    int32_t SetKnuckleSwitch(bool knuckleSwitch);
    void RegisterProximitySensor();
    void UnregisterProximitySensor();
    int32_t LaunchAiScreenAbility(int32_t pid);
    int32_t SwitchScreenCapturePermission(uint32_t permissionType, bool enable);
#ifdef OHOS_BUILD_ENABLE_MISTOUCH_PREVENTION
    void UnregisterMistouchPrevention();
#endif // OHOS_BUILD_ENABLE_MISTOUCH_PREVENTION

private:
    void Print();
    void PrintSeq();
    void PrintExcludeKeys();
    bool ParseConfig();
    bool ParseExcludeConfig();
    bool ParseJson(const std::string &configFile);
    bool ParseExcludeJson(const std::string &configFile);
    void ParseRepeatKeyMaxCount();
    void ParseStatusConfigObserver();
    bool ParseLongPressConfig();
    bool ParseLongPressJson(const std::string &configFile);
    void LaunchAbility(const Ability &ability);
    void LaunchAbility(const Ability &ability, int64_t delay);
    void LaunchAbility(const ShortcutKey &key);
    void LaunchAbility(const Sequence &sequence);
    void LaunchRepeatKeyAbility(const RepeatKey &item, const std::shared_ptr<KeyEvent> keyEvent);
    bool IsKeyMatch(const ShortcutKey &shortcutKey, const std::shared_ptr<KeyEvent> &key);
    bool IsRepeatKeyEvent(const SequenceKey &sequenceKey);
    bool HandleKeyUp(const std::shared_ptr<KeyEvent> &keyEvent, const ShortcutKey &shortcutKey);
    bool HandleKeyDown(ShortcutKey &shortcutKey);
    bool HandleKeyCancel(ShortcutKey &shortcutKey);
    bool PreHandleEvent(const std::shared_ptr<KeyEvent> key);
    bool HandleEvent(const std::shared_ptr<KeyEvent> key);
    bool HandleKeyUpCancel(const RepeatKey &item, const std::shared_ptr<KeyEvent> keyEvent);
    bool HandleRepeatKeyCount(const RepeatKey &item, const std::shared_ptr<KeyEvent> keyEvent);
    void HandleRepeatKeyOwnCount(const RepeatKey &item);
    bool HandleRepeatKey(const RepeatKey& item, const std::shared_ptr<KeyEvent> keyEvent);
    bool HandleRepeatKeys(const std::shared_ptr<KeyEvent> keyEvent);
    bool HandleRepeatKeyAbility(const RepeatKey &item, const std::shared_ptr<KeyEvent> keyEvent, bool isMaxTimes);
    bool HandleSequence(Sequence& sequence, bool &isLaunchAbility);
    bool HandleNormalSequence(Sequence& sequence, bool &isLaunchAbility);
    bool HandleMatchedSequence(Sequence& sequence, bool &isLaunchAbility);
    bool HandleScreenLocked(Sequence& sequence, bool &isLaunchAbility);
    bool IsActiveSequenceRepeating(std::shared_ptr<KeyEvent> keyEvent) const;
    void MarkActiveSequence(bool active);
    bool HandleSequences(const std::shared_ptr<KeyEvent> keyEvent);
    bool HandleShortKeys(const std::shared_ptr<KeyEvent> keyEvent);
    bool MatchShortcutKeys(const std::shared_ptr<KeyEvent> keyEvent);
    bool MatchShortcutKey(std::shared_ptr<KeyEvent> keyEvent,
        ShortcutKey &shortcutKey, std::vector<ShortcutKey> &upAbilities);
    bool HandleConsumedKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
    bool HandleMulFingersTap(const std::shared_ptr<PointerEvent> pointerEvent);
    bool AddSequenceKey(const std::shared_ptr<KeyEvent> keyEvent);
    std::shared_ptr<KeyEvent> CreateKeyEvent(int32_t keyCode, int32_t keyAction, bool isPressed);
    bool IsEnableCombineKey(const std::shared_ptr<KeyEvent> key);
    bool IsExcludeKey(const std::shared_ptr<KeyEvent> key);
    void RemoveSubscribedTimer(int32_t keyCode);
    void HandleSpecialKeys(int32_t keyCode, int32_t keyAction);
    void InterruptTimers();
    void HandlePointerVisibleKeys(const std::shared_ptr<KeyEvent> &keyEvent);
    int32_t GetKeyDownDurationFromXml(const std::string &businessId);
    void SendKeyEvent();
    bool CheckSpecialRepeatKey(RepeatKey& item, const std::shared_ptr<KeyEvent> keyEvent);
    bool IsMusicActivate();
    template <class T>
    void CreateStatusConfigObserver(T& item);
    template <class T>
    void CreateKnuckleConfigObserver(T& item);
    void ResetLastMatchedKey()
    {
        lastMatchedKey_.preKeys.clear();
        lastMatchedKey_.finalKey = -1;
        lastMatchedKey_.timerId = -1;
        lastMatchedKey_.keyDownDuration = 0;
    }
    void ResetCurrentLaunchAbilityKey()
    {
        currentLaunchAbilityKey_.preKeys.clear();
        currentLaunchAbilityKey_.finalKey = -1;
        currentLaunchAbilityKey_.timerId = -1;
        currentLaunchAbilityKey_.keyDownDuration = 0;
    }

    void ResetSequenceKeys()
    {
        keys_.clear();
        filterSequences_.clear();
    }
    bool SkipFinalKey(const int32_t keyCode, const std::shared_ptr<KeyEvent> &key);
#ifdef OHOS_BUILD_ENABLE_TOUCH
    void OnHandleTouchEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void InitializeLongPressConfigurations();
#endif // OHOS_BUILD_ENABLE_TOUCH
#ifdef OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    void StartTwoFingerGesture();
    void StopTwoFingerGesture();
    bool CheckTwoFingerGestureAction() const;
#endif // OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
#ifdef OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    void HandleFingerGestureDownEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void HandleFingerGestureUpEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void HandleKnuckleGestureDownEvent(const std::shared_ptr<PointerEvent> touchEvent);
    void HandleKnuckleGestureUpEvent(const std::shared_ptr<PointerEvent> touchEvent);
    std::pair<int32_t, int32_t> CalcDrawCoordinate(const OLD::DisplayInfo& displayInfo,
        PointerEvent::PointerItem pointerItem);
    void SingleKnuckleGestureProcesser(const std::shared_ptr<PointerEvent> touchEvent);
    void DoubleKnuckleGestureProcesser(const std::shared_ptr<PointerEvent> touchEvent);
    void ReportKnuckleScreenCapture(const std::shared_ptr<PointerEvent> touchEvent);
    void KnuckleGestureProcessor(std::shared_ptr<PointerEvent> touchEvent,
        KnuckleGesture &knuckleGesture, KnuckleType type);
    void UpdateKnuckleGestureInfo(const std::shared_ptr<PointerEvent> touchEvent, KnuckleGesture &knuckleGesture);
    void AdjustDistanceConfigIfNeed(float distance);
    bool CheckKnuckleCondition(std::shared_ptr<PointerEvent> touchEvent);
#endif // OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    int32_t ConvertVPToPX(int32_t vp) const;
#endif // OHOS_BUILD_ENABLE_TOUCH
#ifdef OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    void HandleKnuckleGestureEvent(std::shared_ptr<PointerEvent> touchEvent);
    void HandleKnuckleGestureTouchDown(std::shared_ptr<PointerEvent> touchEvent);
    void HandleKnuckleGestureTouchMove(std::shared_ptr<PointerEvent> touchEvent);
    void HandleKnuckleGestureTouchUp(std::shared_ptr<PointerEvent> touchEvent);
    void ProcessKnuckleGestureTouchUp(NotifyType type);
    void ResetKnuckleGesture();
    std::string GesturePointsToStr() const;
    bool IsValidAction(int32_t action);
    void ReportIfNeed();
    void ReportRegionGesture();
    void ReportLetterGesture();
    void ReportGestureInfo();
    bool IsMatchedAbility(std::vector<float> gesturePoints_, float gestureLastX, float gestureLastY);
#endif // OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    void CheckAndUpdateTappingCountAtDown(std::shared_ptr<PointerEvent> touchEvent);
    bool TouchPadKnuckleDoubleClickHandle(std::shared_ptr<KeyEvent> event);
    void TouchPadKnuckleDoubleClickProcess(const std::string bundleName, const std::string abilityName,
        const std::string action);
    void SendNotSupportMsg(std::shared_ptr<PointerEvent> touchEvent);
    bool CheckBundleName(const std::shared_ptr<PointerEvent> touchEvent);
    void OnKunckleSwitchStatusChange(const std::string switchName);
    void SendSaveEvent(std::shared_ptr<KeyEvent> keyEvent);
    bool MenuClickHandle(std::shared_ptr<KeyEvent> event);
    void MenuClickProcess(const std::string bundleName, const std::string abilityName, const std::string action);
    int32_t CheckTwoFingerGesture(int32_t pid);
    bool HasScreenCapturePermission(uint32_t permissionType);
#ifdef OHOS_BUILD_ENABLE_MISTOUCH_PREVENTION
    void CallMistouchPrevention();
#endif // OHOS_BUILD_ENABLE_MISTOUCH_PREVENTION

private:
    Sequence matchedSequence_;
    ShortcutKey lastMatchedKey_;
    ShortcutKey currentLaunchAbilityKey_;
    std::map<std::string, ShortcutKey> shortcutKeys_;
    std::set<std::string> appWhiteList_;
    std::vector<Sequence> sequences_;
    std::vector<ExcludeKey> excludeKeys_;
    std::vector<Sequence> filterSequences_;
    std::vector<SequenceKey> keys_;
    std::vector<RepeatKey> repeatKeys_;
    std::vector<std::string> businessIds_;
    bool isParseConfig_ { false };
    bool isParseExcludeConfig_ { false };
    bool isParseLongPressConfig_ { false };
    std::map<int32_t, int32_t> specialKeys_;
    std::map<int32_t, std::list<int32_t>> specialTimers_;
    std::map<int32_t, int32_t> repeatKeyMaxTimes_;
    std::map<std::string, int32_t> repeatKeyTimerIds_;
    std::map<std::string, int32_t> repeatKeyCountMap_;
    TwoFingerGesture twoFingerGesture_;
    KnuckleGesture singleKnuckleGesture_;
    KnuckleGesture doubleKnuckleGesture_;
    MultiFingersTap threeFingersTap_;
    bool isDistanceConfig_ { false };
    bool isKnuckleSwitchConfig_ { false };
    struct KnuckleSwitch screenshotSwitch_;
    struct KnuckleSwitch recordSwitch_;
    int32_t checkAdjustIntervalTimeCount_ { 0 };
    int32_t checkAdjustDistanceCount_ { 0 };
    int64_t downToPrevUpTimeConfig_ { 0 };
    float downToPrevDownDistanceConfig_ { 0.0f };
    float distanceDefaultConfig_ { 0.0f };
    float distanceLongConfig_ { 0.0f };
    bool enableCombineKey_ { true };
    RepeatKey repeatKey_;
    int32_t maxCount_ { 0 };
    int32_t count_ { 0 };
    int32_t repeatTimerId_ { -1 };
    int32_t knuckleCount_ { 0 };
    int32_t sosDelayTimerId_ { -1 };
    int64_t downActionTime_ { 0 };
    int64_t lastDownActionTime_ { 0 };
    int64_t lastVolumeDownActionTime_ { 0 };
    int64_t upActionTime_ { 0 };
    int32_t launchAbilityCount_ { 0 };
    int64_t intervalTime_ { 120000 };
    std::atomic<bool> isFreezePowerKey_ { false };
    bool isDownStart_ { false };
    bool isKeyCancel_ { false };
    bool sequenceOccurred_ { false };
    bool isHandleSequence_ { false };
    bool isParseMaxCount_ { false };
    bool isParseStatusConfig_ { false };
    bool isDoubleClick_ { false };
    int32_t lastKeyEventCode_ { -1 };
    std::string sessionKey_ { };
    bool isStartBase_ { false };
#ifdef OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    bool isGesturing_ { false };
    bool isLetterGesturing_ { false };
    bool isLastGestureSucceed_ { false };
    float gestureLastX_ { 0.0f };
    float gestureLastY_ { 0.0f };
    float gestureTrackLength_ { 0.0f };
    std::vector<float> gesturePoints_;
    std::vector<int64_t> gestureTimeStamps_;
    int64_t drawOFailTimestamp_ { 0 };
    int64_t drawOSuccTimestamp_ { 0 };
    Direction lastDirection_ { DIRECTION0 };
#endif // OHOS_BUILD_ENABLE_GESTURESENSE_WRAPPER
    int64_t lastDownTime_ { 0 };
    int64_t previousUpTime_ { 0 };
    int32_t tappingCount_ { 0 };
    std::mutex mutex_;
    std::map<int32_t, int64_t> lastPointerDownTime_;
    int64_t walletLaunchDelayTimes_ { 0 };
    int64_t sosLaunchTime_ { -1 };
    int64_t powerUpTime_ { 0 };
    int32_t currentUserId_ { -1 };
    int64_t lastMenuDownTime_ {0};
    bool existMenuDown_ { false };
    std::shared_ptr<KeyEvent> tmpkeyEvent_ {nullptr};
    bool gameForbidFingerKnuckle_ { false };
    bool hasRegisteredSensor_ { false };
    uint32_t screenCapturePermission_ { ScreenCapturePermissionType::DEFAULT_PERMISSIONS };
    void *mistouchLibHandle_ {nullptr};
    IMistouchPrevention *mistouchPrevention_ {nullptr};
    std::atomic<int32_t> ret_ { 5 };
    std::mutex dataMutex_;
};
} // namespace MMI
} // namespace OHOS
#endif // KEY_COMMAND_HANDLER_H