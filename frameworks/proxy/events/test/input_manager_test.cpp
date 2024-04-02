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

#include <semaphore.h>

#include "event_log_helper.h"
#include "event_util_test.h"
#include "input_manager_util.h"
#include "multimodal_event_handler.h"
#include "system_info.h"
#include "input_manager.h"
#include "pixel_map.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputManagerTest"};
constexpr int32_t TUPLE_PID = 0;
constexpr int32_t TUPLE_UID = 1;
constexpr int32_t TUPLE_NAME = 2;
constexpr int32_t TIME_WAIT_FOR_OP = 100;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t SLEEP_MILLISECONDS = 1000;
constexpr int32_t DEFAULT_DEVICE_ID = 0;
constexpr int32_t KEY_REPEAT_DELAY = 350;
constexpr int32_t KEY_REPEAT_RATE = 60;
constexpr int32_t POINTER_ITEM_DISPLAY_X_ONE = 147;
constexpr int32_t POINTER_ITEM_DISPLAY_X_TWO = 456;
constexpr int32_t POINTER_ITEM_DISPLAY_Y_ONE = 123;
constexpr int32_t POINTER_ITEM_DISPLAY_Y_TWO = 258;
constexpr int32_t KEY_DOWN_DURATION = 300;
constexpr int32_t FINAL_KEY_DOWN_DURATION_ONE = 10;
constexpr int32_t FINAL_KEY_DOWN_DURATION_TWO = 2000;
constexpr int32_t POINTER_SENSOR_INPUT_TIME = 2000;
constexpr int32_t BUFFER_SIZE = 512;
constexpr int32_t KEYBOARD_TYPE_SIZE = 20;
constexpr int32_t PARAMETER_ERROR = 401;
constexpr int32_t INVAID_VALUE = -1;
constexpr uint32_t MAX_WINDOW_NUMS = 15;
#ifdef OHOS_BUILD_ENABLE_ANCO
constexpr uint32_t SHELL_FLAGS_VALUE = 2;
#endif // OHOS_BUILD_ENABLE_ANCO

constexpr double POINTER_ITEM_PRESSURE = 5.0;
} // namespace

class InputManagerTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    std::string GetEventDump();
    std::shared_ptr<OHOS::Media::PixelMap> MatrixToPixelmap(const std::vector<std::vector<uint32_t>>& windowMask);
    std::vector<std::vector<uint32_t>> CreateMatrix(int32_t width, int32_t height);

private:
    int32_t keyboardRepeatRate_ { 50 };
    int32_t keyboardRepeatDelay_ { 500 };
};

class MMIWindowChecker : public MMI::IWindowChecker {
public:
    int32_t CheckWindowId(int32_t windowId) const override;
};

class IEventObserver : public MMI::MMIEventObserver {
public:
    void SyncBundleName(int32_t pid, int32_t uid, std::string bundleName, int32_t syncStatus) override;
};

void IEventObserver::SyncBundleName(int32_t pid, int32_t uid, std::string bundleName, int32_t syncStatus)
{
    int32_t getPid = pid;
    int32_t getUid = uid;
    std::string getName = bundleName;
    int32_t getStatus = syncStatus;
    MMI_HILOGD("SyncBundleName info is : %{public}d, %{public}d, %{public}s, %{public}d",
        getPid, getUid, getName.c_str(), getStatus);
}

int32_t MMIWindowChecker::CheckWindowId(int32_t windowId) const
{
    return getpid();
}

void InputManagerTest::SetUpTestCase()
{
    ASSERT_TRUE(TestUtil->Init());
}

void InputManagerTest::SetUp()
{
    TestUtil->SetRecvFlag(RECV_FLAG::RECV_FOCUS);
}

void InputManagerTest::TearDown()
{
    TestUtil->AddEventDump("");
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    InputManager::GetInstance()->SetKeyboardRepeatDelay(keyboardRepeatDelay_);
    InputManager::GetInstance()->SetKeyboardRepeatRate(keyboardRepeatRate_);
}

std::string InputManagerTest::GetEventDump()
{
    return TestUtil->GetEventDump();
}

std::shared_ptr<OHOS::Media::PixelMap> InputManagerTest::MatrixToPixelmap(
    const std::vector<std::vector<uint32_t>>& windowMask)
{
    uint32_t maskHeight = windowMask.size();
    uint32_t maskWidth = windowMask[0].size();

    OHOS::Media::InitializationOptions ops;
    ops.size.width = maskWidth;
    ops.size.height = maskHeight;
    ops.pixelFormat = OHOS::Media::PixelFormat::ALPHA_8;
    ops.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    ops.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    uint32_t length = maskWidth * maskHeight;
    uint32_t *data = new (std::nothrow) uint32_t[length];
    for (uint32_t i = 0; i < maskHeight; i++) {
        for (uint32_t j = 0; j < maskWidth; j++) {
            uint32_t idx = i * maskWidth + j;
            data[idx] = windowMask[i][j];
        }
    }
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = OHOS::Media::PixelMap::Create(data, length, ops);
    delete[] data;
    if (pixelMap == nullptr) {
        MMI_HILOGE("Failed to create a pixelMap");
        return nullptr;
    }
    return pixelMap;
}

std::vector<std::vector<uint32_t>> InputManagerTest::CreateMatrix(int32_t width, int32_t height)
{
    std::vector<std::vector<uint32_t>> matrix(height, std::vector<uint32_t>(width, 0));
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            matrix[i][j] = (i % 3 == 0)? 0 : 1;
        }
    }
    return matrix;
}

/**
 * @tc.name: InputManager_NotResponse_001
 * @tc.desc: detection of not response
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_NotResponse_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPressure(POINTER_ITEM_PRESSURE);
    item.SetPointerId(0);
    item.SetDisplayX(POINTER_ITEM_DISPLAY_X_TWO);
    item.SetDisplayY(POINTER_ITEM_DISPLAY_Y_ONE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_NotResponse_002
 * @tc.desc: detection of not response
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_NotResponse_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetDisplayY(POINTER_ITEM_DISPLAY_Y_TWO);
    item.SetDisplayX(POINTER_ITEM_DISPLAY_X_ONE);
    item.SetPressure(POINTER_ITEM_PRESSURE);
    item.SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
/**
 * @tc.name: InputManagerTest_InterceptTabletToolEvent_001
 * @tc.desc: Verify intercepting tablet tool event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_InterceptTabletToolEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto interceptor = GetPtr<InputEventCallback>();
    int32_t interceptorId{InputManager::GetInstance()->AddInterceptor(interceptor)};
    EXPECT_TRUE(IsValidHandlerId(interceptorId));
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));

#ifdef OHOS_BUILD_ENABLE_TOUCH
    auto pointerEvent = InputManagerUtil::SetupTabletToolEvent001();
    ASSERT_NE(pointerEvent, nullptr);
    TestSimulateInputEvent(pointerEvent);

    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    TestSimulateInputEvent(pointerEvent);
#endif // OHOS_BUILD_ENABLE_TOUCH

    if (IsValidHandlerId(interceptorId)) {
        InputManager::GetInstance()->RemoveInterceptor(interceptorId);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    }
}
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR

#ifdef OHOS_BUILD_ENABLE_TOUCH
HWTEST_F(InputManagerTest, AppendExtraData_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto consumer = GetPtr<InputEventConsumer>();
    ASSERT_TRUE(consumer != nullptr);
    const std::string threadTest = "EventUtilTest";
    auto runner = AppExecFwk::EventRunner::Create(threadTest);
    ASSERT_TRUE(runner != nullptr);
    auto eventHandler = std::make_shared<AppExecFwk::EventHandler>(runner);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(consumer, eventHandler);
    std::vector<uint8_t> buffer(BUFFER_SIZE, 1);
    ExtraData extraData;
    extraData.appended = true;
    extraData.buffer = buffer;
    extraData.sourceType = PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
    extraData.pointerId = 1;
    InputManager::GetInstance()->AppendExtraData(extraData);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    auto pointerEvent = InputManagerUtil::SetupPointerEvent001();
    pointerEvent->AddFlag(PointerEvent::EVENT_FLAG_NO_INTERCEPT);
    ASSERT_TRUE(pointerEvent != nullptr);
    TestSimulateInputEvent(pointerEvent, TestScene::EXCEPTION_TEST);

    extraData.appended = false;
    extraData.buffer.clear();
    extraData.pointerId = INVAID_VALUE;
    InputManager::GetInstance()->AppendExtraData(extraData);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    ASSERT_TRUE(pointerEvent != nullptr);
    SimulateInputEventUtilTest(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_POINTER
HWTEST_F(InputManagerTest, AppendExtraData_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::vector<uint8_t> buffer(BUFFER_SIZE, 1);
    ExtraData extraData;
    extraData.appended = true;
    extraData.buffer = buffer;
    extraData.sourceType = PointerEvent::SOURCE_TYPE_MOUSE;
    InputManager::GetInstance()->AppendExtraData(extraData);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    auto pointerEvent = InputManagerUtil::SetupPointerEvent006();
    pointerEvent->AddFlag(PointerEvent::EVENT_FLAG_NO_INTERCEPT);
    ASSERT_TRUE(pointerEvent != nullptr);
    TestSimulateInputEvent(pointerEvent, TestScene::EXCEPTION_TEST);

    extraData.appended = false;
    extraData.buffer.clear();
    InputManager::GetInstance()->AppendExtraData(extraData);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    ASSERT_TRUE(pointerEvent != nullptr);
    SimulateInputEventUtilTest(pointerEvent);
}
#endif // OHOS_BUILD_ENABLE_POINTER

/**
 * @tc.name: InputManagerTest_SubscribeKeyEvent_001
 * @tc.desc: Verify invalid parameter.
 * @tc.type: FUNC
 * @tc.require:SR000GGQL4  AR000GJNGN
 * @tc.author: yangguang
 */
HWTEST_F(InputManagerTest, InputManagerTest_SubscribeKeyEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::set<int32_t> preKeys;
    std::shared_ptr<KeyOption> keyOption =
        InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_VOLUME_MUTE, true, 0);
    int32_t response = INVAID_VALUE;
    response = InputManager::GetInstance()->SubscribeKeyEvent(keyOption, nullptr);
    EXPECT_TRUE(response < 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    InputManager::GetInstance()->UnsubscribeKeyEvent(response);
}

/**
 * @tc.name: InputManagerTest_SubscribeKeyEvent_02
 * @tc.desc: Verify subscribe power key event.
 * @tc.type: FUNC
 * @tc.require:SR000GGQL4  AR000GJNGN
 * @tc.author: zhaoxueyuan
 */
HWTEST_F(InputManagerTest, InputManagerTest_SubscribeKeyEvent_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_TRUE(MMIEventHdl.InitClient());
    // 电源键长按按下订阅
    std::set<int32_t> preKeys;
    std::shared_ptr<KeyOption> keyOption =
        InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_POWER, true, FINAL_KEY_DOWN_DURATION_TWO);
    int32_t subscribeId1 = INVAID_VALUE;
    subscribeId1 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_POWER down trigger callback");
    });
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    EXPECT_TRUE(subscribeId1 >= 0);
#else
    EXPECT_TRUE(subscribeId1 < 0);
#endif // OHOS_BUILD_ENABLE_KEYBOARD

    // 电源键抬起订阅
    std::shared_ptr<KeyOption> keyOption2 = InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_POWER, false, 0);
    int32_t subscribeId2 = INVAID_VALUE;
    subscribeId2 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption2, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_POWER up trigger callback");
    });
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    EXPECT_TRUE(subscribeId2 >= 0);
#else
    EXPECT_TRUE(subscribeId2 < 0);
#endif // OHOS_BUILD_ENABLE_KEYBOARD

    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId1);
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId2);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
}

/**
 * @tc.name: InputManagerTest_SubscribeKeyEvent_03
 * @tc.desc: Verify subscribe volume up key event.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SubscribeKeyEvent_03, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_TRUE(MMIEventHdl.InitClient());
    std::set<int32_t> preKeys;
    std::shared_ptr<KeyOption> keyOption1 =
        InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_VOLUME_UP, true, FINAL_KEY_DOWN_DURATION_ONE);
    int32_t subscribeId1 = INVAID_VALUE;
    subscribeId1 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption1, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_VOLUME_UP down trigger callback");
    });
    std::shared_ptr<KeyOption> keyOption2 =
        InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_VOLUME_UP, false, 0);
    int32_t subscribeId2 = INVAID_VALUE;
    subscribeId2 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption2, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_VOLUME_UP up trigger callback");
    });
    std::shared_ptr<KeyOption> keyOption3 = InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_VOLUME_UP, true, 0);
    int32_t subscribeId3 = INVAID_VALUE;
    subscribeId3 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption3, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_VOLUME_UP down trigger callback");
    });
    std::shared_ptr<KeyOption> keyOption4 =
        InputManagerUtil::InitOption(preKeys, KeyEvent::KEYCODE_VOLUME_UP, false, 0);
    int32_t subscribeId4 = INVAID_VALUE;
    subscribeId4 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption4, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_VOLUME_UP up trigger callback");
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId1);
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId2);
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId3);
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId4);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));
}

/**
 * @tc.name: InputManagerTest_SubscribeKeyEvent_04
 * @tc.desc: Verify subscribe key event.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SubscribeKeyEvent_04, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::set<int32_t> preKeys;
    std::shared_ptr<KeyOption> keyOption = std::make_shared<KeyOption>();
    keyOption->SetPreKeys(preKeys);
    keyOption->SetFinalKey(KeyEvent::KEYCODE_VOLUME_DOWN);
    keyOption->SetFinalKeyDown(true);
    keyOption->SetFinalKeyDownDuration(INVAID_VALUE);
    int32_t subscribeId = INVAID_VALUE;
    subscribeId = InputManager::GetInstance()->SubscribeKeyEvent(keyOption, [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_POWER down trigger callback");
    });
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    EXPECT_TRUE(subscribeId >= 0);
#else
    EXPECT_TRUE(subscribeId < 0);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_VOLUME_DOWN);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_VOLUME_DOWN);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
    InputManager::GetInstance()->SimulateInputEvent(injectDownEvent);
    ASSERT_EQ(injectDownEvent->GetKeyAction(), KeyEvent::KEY_ACTION_DOWN);
}

/**
 * @tc.name: TestGetKeystrokeAbility_001
 * @tc.desc: Verify SupportKeys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, TestGetKeystrokeAbility_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::vector<int32_t> keyCodes = {
        KeyEvent::KEYCODE_VOLUME_DOWN, KeyEvent::KEYCODE_VOLUME_MUTE, KeyEvent::KEYCODE_DEL};
    int32_t result = InputManager::GetInstance()->SupportKeys(
        0, keyCodes, [](std::vector<bool> keystrokeAbility) { MMI_HILOGD("TestGetKeystrokeAbility_001 callback ok"); });
    ASSERT_EQ(result, 0);
    MMI_HILOGD("Stop TestGetKeystrokeAbility_001");
}

/**
 * @tc.name: InputManagerTest_RemoteControlAutoRepeat
 * @tc.desc: After the key is pressed, repeatedly trigger the key to press the input
 * @tc.type: FUNC
 * @tc.require: I530XB
 */
HWTEST_F(InputManagerTest, InputManagerTest_RemoteControlAutoRepeat, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    injectDownEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);
    ASSERT_TRUE(injectDownEvent != nullptr);
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_A);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_A);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    SimulateInputEventUtilTest(injectDownEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD

    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MILLISECONDS));

    std::shared_ptr<KeyEvent> injectUpEvent = KeyEvent::Create();
    ASSERT_TRUE(injectUpEvent != nullptr);
    downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitUp;
    kitUp.SetKeyCode(KeyEvent::KEYCODE_A);
    kitUp.SetPressed(false);
    kitUp.SetDownTime(downTime);
    injectUpEvent->SetKeyCode(KeyEvent::KEYCODE_A);
    injectUpEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    injectUpEvent->RemoveReleasedKeyItems(kitUp);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    SimulateInputEventUtilTest(injectUpEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

static int32_t g_deviceIDtest = 0;
static void GetKeyboardTypeCallback(int32_t keyboardType)
{
    switch (keyboardType) {
        case KEYBOARD_TYPE_NONE: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "None");
            break;
        }
        case KEYBOARD_TYPE_UNKNOWN: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "unknown");
            break;
        }
        case KEYBOARD_TYPE_ALPHABETICKEYBOARD: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "alphabetickeyboard");
            break;
        }
        case KEYBOARD_TYPE_DIGITALKEYBOARD: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "digitalkeyboard");
            break;
        }
        case KEYBOARD_TYPE_HANDWRITINGPEN: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "handwritingpen");
            break;
        }
        case KEYBOARD_TYPE_REMOTECONTROL: {
            MMI_HILOGD("g_deviceIDtest:%{public}d-->KeyboardType:%{public}s", g_deviceIDtest, "remotecontrol");
            break;
        }
        default: {
            MMI_HILOGW("Error obtaining keyboard type");
            break;
        }
    }
}

/**
 * @tc.name: InputManagerTest_GetKeyboardType
 * @tc.desc: Verify Get Keyboard Type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetKeyboardType, TestSize.Level1)
{
    MMI_HILOGD("Start InputManagerTest_GetKeyboardType");
    for (int32_t i = 0; i < KEYBOARD_TYPE_SIZE; ++i) {
        g_deviceIDtest = i;
        ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->GetKeyboardType(i, GetKeyboardTypeCallback));
        MMI_HILOGD("i:%{public}d", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    MMI_HILOGD("Stop InputManagerTest_GetKeyboardType");
}

/**
 * @tc.name: InputManagerTest_SetKeyboardRepeatDelay
 * @tc.desc: Verify Set Keyboard Repeat Delay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetKeyboardRepeatDelay, TestSize.Level1)
{
    MMI_HILOGD("Start InputManagerTest_SetKeyboardRepeatDelay");
    int32_t ret = InputManager::GetInstance()->SetKeyboardRepeatDelay(KEY_REPEAT_DELAY);
    ASSERT_EQ(ret, RET_OK);
    MMI_HILOGD("Stop InputManagerTest_SetKeyboardRepeatDelay");
}

/**
 * @tc.name: InputManagerTest_SetKeyboardRepeatRate
 * @tc.desc: Verify Set Keyboard Repeat Rate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetKeyboardRepeatRate, TestSize.Level1)
{
    MMI_HILOGD("Start InputManagerTest_SetKeyboardRepeatRate");
    int32_t ret = InputManager::GetInstance()->SetKeyboardRepeatRate(KEY_REPEAT_RATE);
    ASSERT_EQ(ret, RET_OK);
    MMI_HILOGD("Stop InputManagerTest_SetKeyboardRepeatRate");
}

/**
 * @tc.name: InputManagerTest_GetKeyboardRepeatDelay
 * @tc.desc: Verify Get Keyboard Repeat Delay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetKeyboardRepeatDelay, TestSize.Level1)
{
    MMI_HILOGD("Start InputManagerTest_GetKeyboardRepeatDelay");
    auto callback = [](int32_t delay) {
        ASSERT_TRUE(delay == KEY_REPEAT_DELAY);
        MMI_HILOGD("Get keyboard repeat delay success");
    };
    if (InputManager::GetInstance()->SetKeyboardRepeatDelay(KEY_REPEAT_DELAY) == RET_OK) {
        ASSERT_TRUE(InputManager::GetInstance()->GetKeyboardRepeatDelay(callback) == RET_OK);
    }
    MMI_HILOGD("Stop InputManagerTest_GetKeyboardRepeatDelay");
}

/**
 * @tc.name: InputManagerTest_GetKeyboardRepeatRate
 * @tc.desc: Verify Get Keyboard Repeat Rate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetKeyboardRepeatRate, TestSize.Level1)
{
    MMI_HILOGD("Start InputManagerTest_GetKeyboardRepeatRate");
    auto callback = [](int32_t rate) {
        ASSERT_TRUE(rate == KEY_REPEAT_RATE);
        MMI_HILOGD("Get keyboard repeat rate success");
    };
    if (InputManager::GetInstance()->SetKeyboardRepeatRate(KEY_REPEAT_RATE) == RET_OK) {
        ASSERT_TRUE(InputManager::GetInstance()->GetKeyboardRepeatRate(callback) == RET_OK);
    }
    MMI_HILOGD("Stop InputManagerTest_GetKeyboardRepeatRate");
}

HWTEST_F(InputManagerTest, InputManagerTest_GetProcCpuUsage, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SYSTEM_INFO::CpuInfo cpuInfo;
    const std::string process_name = "multimodalinput";
    auto usage = cpuInfo.GetProcCpuUsage(process_name);
    MMI_HILOGD("The CPU usage of the %{public}s process is %{public}.2f", process_name.c_str(), usage);
    ASSERT_TRUE(usage < SYSTEM_INFO::CPU_USAGE_LOAD && usage != SYSTEM_INFO::CPU_USAGE_UNKNOWN);
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_001
 * @tc.desc: Set NumLock for the keyboard enablement state to true
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::NUM_LOCK_FUNCTION_KEY, true);
    ASSERT_FALSE(InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::NUM_LOCK_FUNCTION_KEY));
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_002
 * @tc.desc: Set NumLock for the keyboard enablement state to false
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::NUM_LOCK_FUNCTION_KEY, false);
    bool result = InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::NUM_LOCK_FUNCTION_KEY);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_003
 * @tc.desc: Set ScrollLock for the keyboard enablement state to true
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::SCROLL_LOCK_FUNCTION_KEY, true);
    ASSERT_FALSE(InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::SCROLL_LOCK_FUNCTION_KEY));
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_004
 * @tc.desc: Set ScrollLock for the keyboard enablement state to false
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::SCROLL_LOCK_FUNCTION_KEY, false);
    bool result = InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::SCROLL_LOCK_FUNCTION_KEY);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_005
 * @tc.desc: Set CapsLock for the keyboard enablement state to true
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::CAPS_LOCK_FUNCTION_KEY, true);
    ASSERT_FALSE(InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::CAPS_LOCK_FUNCTION_KEY));
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_006
 * @tc.desc: Set CapsLock for the keyboard enablement state to false
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_006, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::CAPS_LOCK_FUNCTION_KEY, false);
    bool result = InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::CAPS_LOCK_FUNCTION_KEY);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: InputManagerTest_FunctionKeyState_007
 * @tc.desc: Set other function keys
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_FunctionKeyState_007, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::UNKNOWN_FUNCTION_KEY, true);
    bool result = InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::UNKNOWN_FUNCTION_KEY);
    ASSERT_FALSE(result);

    InputManager::GetInstance()->SetFunctionKeyState(KeyEvent::UNKNOWN_FUNCTION_KEY, false);
    result = InputManager::GetInstance()->GetFunctionKeyState(KeyEvent::UNKNOWN_FUNCTION_KEY);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: InputManagerTest_EnableCombineKey_001
 * @tc.desc: Enable combine key
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_EnableCombineKey_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_EQ(InputManager::GetInstance()->EnableCombineKey(false), RET_OK);
}

/**
 * @tc.name: InputManagerTest_EnableCombineKey_002
 * @tc.desc: Enable combine key
 * @tc.type: FUNC
 * @tc.require: I5HMCX
 */
HWTEST_F(InputManagerTest, InputManagerTest_EnableCombineKey_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_EQ(InputManager::GetInstance()->EnableCombineKey(true), RET_OK);
}

/**
 * @tc.name: InputManagerTest_TouchScreenHotArea_001
 * @tc.desc: Touch event Search window by defaultHotAreas
 * @tc.type: FUNC
 * @tc.require: I5HMCB
 */
HWTEST_F(InputManagerTest, InputManagerTest_TouchScreenHotArea_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<PointerEvent> pointerEvent{InputManagerUtil::SetupTouchScreenEvent001()};
    ASSERT_TRUE(pointerEvent != nullptr);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    ASSERT_EQ(pointerEvent->GetSourceType(), PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
}

/**
 * @tc.name: InputManagerTest_TouchScreenHotArea_002
 * @tc.desc: Touch event Search window by pointerHotAreas
 * @tc.type: FUNC
 * @tc.require: I5HMCB
 */
HWTEST_F(InputManagerTest, InputManagerTest_TouchScreenHotArea_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<PointerEvent> pointerEvent{InputManagerUtil::SetupTouchScreenEvent002()};
    ASSERT_TRUE(pointerEvent != nullptr);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    ASSERT_EQ(pointerEvent->GetSourceType(), PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 0;
    displayGroupInfo.width = 0;
    displayGroupInfo.height = 0;
    InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
    ASSERT_TRUE(displayGroupInfo.displaysInfo.empty());
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for 1 display and 1 window
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 1;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    displayInfo.id = 0;
    displayInfo.x =1;
    displayInfo.y = 1;
    displayInfo.width = 2;
    displayInfo.height = 2;
    displayInfo.dpi = 240;
    displayInfo.name = "pp";
    displayInfo.uniq = "pp";
    displayInfo.direction = DIRECTION0;
    displayGroupInfo.displaysInfo.push_back(displayInfo);
    WindowInfo info;
    info.id = 1;
    info.pid = 1;
    info.uid = 1;
    info.area = {1, 1, 1, 1};
    info.defaultHotAreas = { info.area };
    info.pointerHotAreas = { info.area };
    info.pointerChangeAreas = {16, 5, 16, 5, 16, 5, 16, 5};
    info.transform = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    info.agentWindowId = 1;
    info.flags = 0;
    info.displayId = 0;
    displayGroupInfo.windowsInfo.push_back(info);
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for 1 display and max-windows
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 0;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    displayInfo.id = 0;
    displayInfo.x =1;
    displayInfo.y = 1;
    displayInfo.width = 2;
    displayInfo.height = 2;
    displayInfo.dpi = 240;
    displayInfo.name = "pp";
    displayInfo.uniq = "pp";
    displayInfo.direction = DIRECTION0;
    displayInfo.displayMode = DisplayMode::FULL;
    displayGroupInfo.displaysInfo.push_back(displayInfo);
    for (uint32_t i = 0; i < MAX_WINDOW_NUMS; i++) {
        WindowInfo info;
        info.id = i + 1;
        info.pid = 1;
        info.uid = 1;
        info.area = {1, 1, 1, 1};
        info.defaultHotAreas = { info.area };
        info.pointerHotAreas = { info.area };
        info.pointerChangeAreas = {16, 5, 16, 5, 16, 5, 16, 5};
        info.transform = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        info.agentWindowId = 1;
        info.flags = 0;
        info.displayId = 0;
        info.zOrder = static_cast<float>(MAX_WINDOW_NUMS - i);
        displayGroupInfo.windowsInfo.push_back(info);
    }
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for max-display and max-window
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 1;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    for (uint32_t i = 0; i < 2; i++) { // one is default-display and another is simulate display
        displayInfo.id = i;
        displayInfo.x =1;
        displayInfo.y = 1;
        displayInfo.width = 2;
        displayInfo.height = 2;
        displayInfo.dpi = 240;
        displayInfo.name = "pp";
        displayInfo.uniq = "pp";
        displayInfo.direction = DIRECTION0;
        displayGroupInfo.displaysInfo.push_back(displayInfo);
    }
    WindowInfo info;
    for (uint32_t i = 0; i < 2; i++) { // 2 widnows for 2 display
        info.id = 1;
        info.pid = 1;
        info.uid = 1;
        info.defaultHotAreas = { {1, 1, 1, 1} };
        info.agentWindowId = 1;
        info.flags = 0;
        info.displayId = i;
        displayGroupInfo.windowsInfo.push_back(info);
    }
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateWindowGroupInfo_001
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateWindowGroupInfo_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    WindowInfo window;
    window.id = 1;
    window.action = WINDOW_UPDATE_ACTION::ADD;
    WindowGroupInfo windowGroupInfo;
    windowGroupInfo.displayId = 0;
    windowGroupInfo.focusWindowId = 1;
    windowGroupInfo.windowsInfo = {window};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateWindowInfo(windowGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateWindowGroupInfo_002
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateWindowGroupInfo_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    WindowInfo window;
    window.id = 1;
    window.action = WINDOW_UPDATE_ACTION::CHANGE;
    WindowGroupInfo windowGroupInfo;
    windowGroupInfo.windowsInfo = {window};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateWindowInfo(windowGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateWindowGroupInfo_003
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateWindowGroupInfo_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    WindowInfo window;
    window.id = 1;
    window.action = WINDOW_UPDATE_ACTION::DEL;
    WindowGroupInfo windowGroupInfo;
    windowGroupInfo.windowsInfo = {window};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateWindowInfo(windowGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateWindowGroupInfo_004
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateWindowGroupInfo_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    WindowInfo window;
    window.id = 1;
    window.action = WINDOW_UPDATE_ACTION::UNKNOWN;

    WindowGroupInfo windowGroupInfo;
    windowGroupInfo.windowsInfo = {window};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateWindowInfo(windowGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateWindowGroupInfo_005
 * @tc.desc: Update window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateWindowGroupInfo_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    WindowInfo window;
    window.id = 1;
    window.action = WINDOW_UPDATE_ACTION::CHANGE;
#ifdef OHOS_BUILD_ENABLE_ANCO
    window.flags |= SHELL_FLAGS_VALUE;
#endif // OHOS_BUILD_ENABLE_ANCO
    WindowGroupInfo windowGroupInfo;
    windowGroupInfo.windowsInfo = {window};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateWindowInfo(windowGroupInfo));
}

#ifdef OHOS_BUILD_ENABLE_SECURITY_COMPONENT
/**
 * @tc.name: InputManagerTest_SetEnhanceConfig_001
 * @tc.desc: Set Secutity component enhance config
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetEnhanceConfig_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    uint8_t cfgData[16] = {0};
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->SetEnhanceConfig(cfgData, 16));
}
#endif // OHOS_BUILD_ENABLE_SECURITY_COMPONENT

/**
 * @tc.name: InputManagerTest_GetDevice_001
 * @tc.desc: Verify the fetch device info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetDevice_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 0;
    auto callback = [](std::shared_ptr<InputDevice> inputDevice) {
        MMI_HILOGD("Get device success");
        ASSERT_TRUE(inputDevice != nullptr);
    };
    InputManager::GetInstance()->GetDevice(deviceId, callback);
}

/**
 * @tc.name: InputManagerTest_GetDevice_002
 * @tc.desc: Verify the fetch device info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetDevice_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = INVAID_VALUE;
    auto callback = [](std::shared_ptr<InputDevice> inputDevice) {
        MMI_HILOGD("Get device success");
        ASSERT_TRUE(inputDevice != nullptr);
    };
    int32_t ret = InputManager::GetInstance()->GetDevice(deviceId, callback);
    ASSERT_NE(ret, RET_OK);
}

/**
 * @tc.name: InputManagerTest_GetDeviceIds
 * @tc.desc: Verify the fetch device list
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetDeviceIds, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto callback = [](std::vector<int32_t> ids) { MMI_HILOGD("Get device success"); };
    int32_t ret = InputManager::GetInstance()->GetDeviceIds(callback);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InputManagerTest_EventTypeToString
 * @tc.desc: Verify inputevent interface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_EventTypeToString, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto inputEvent = InputEvent::Create();
    ASSERT_NE(inputEvent, nullptr);
    auto ret = inputEvent->EventTypeToString(InputEvent::EVENT_TYPE_BASE);
    ASSERT_STREQ(ret, "base");
    ret = inputEvent->EventTypeToString(InputEvent::EVENT_TYPE_KEY);
    ASSERT_STREQ(ret, "key");
    ret = inputEvent->EventTypeToString(InputEvent::EVENT_TYPE_AXIS);
    ASSERT_STREQ(ret, "axis");
    ret = inputEvent->EventTypeToString(INVAID_VALUE);
    ASSERT_STREQ(ret, "unknown");
}

/**
 * @tc.name: InputManagerTest_InputDeviceInterface_001
 * @tc.desc: Verify inputdevice interface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_InputDeviceInterface_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    ASSERT_NE(inputDevice, nullptr);
    inputDevice->SetId(0);
    ASSERT_EQ(inputDevice->GetId(), 0);
    inputDevice->SetName("name");
    ASSERT_STREQ(inputDevice->GetName().c_str(), "name");
    inputDevice->SetType(0);
    ASSERT_EQ(inputDevice->GetType(), 0);
    inputDevice->SetBus(0);
    ASSERT_EQ(inputDevice->GetBus(), 0);
    inputDevice->SetVersion(0);
    ASSERT_EQ(inputDevice->GetVersion(), 0);
    inputDevice->SetProduct(0);
    ASSERT_EQ(inputDevice->GetProduct(), 0);
    inputDevice->SetVendor(0);
    ASSERT_EQ(inputDevice->GetVendor(), 0);
    inputDevice->SetPhys("phys");
    ASSERT_STREQ(inputDevice->GetPhys().c_str(), "phys");
    inputDevice->SetUniq("uniq");
    ASSERT_STREQ(inputDevice->GetUniq().c_str(), "uniq");
}

/**
 * @tc.name: InputManagerTest_InputDeviceInterface_002
 * @tc.desc: Verify inputdevice interface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_InputDeviceInterface_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    ASSERT_NE(inputDevice, nullptr);
    InputDevice::AxisInfo axis;
    axis.SetAxisType(0);
    axis.SetMinimum(0);
    axis.SetMaximum(1);
    axis.SetFuzz(0);
    axis.SetFlat(1);
    axis.SetResolution(0);
    inputDevice->AddAxisInfo(axis);
    auto iter = inputDevice->GetAxisInfo();
    ASSERT_EQ(iter[0].GetAxisType(), 0);
    ASSERT_EQ(iter[0].GetMinimum(), 0);
    ASSERT_EQ(iter[0].GetMaximum(), 1);
    ASSERT_EQ(iter[0].GetFuzz(), 0);
    ASSERT_EQ(iter[0].GetFlat(), 1);
    ASSERT_EQ(iter[0].GetResolution(), 0);
}

/**
 * @tc.name: InputManagerTest_SetAnrObserver
 * @tc.desc: Verify the observer for events
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetAnrObserver, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    class IAnrObserverTest : public IAnrObserver {
    public:
        IAnrObserverTest() : IAnrObserver()
        {}
        virtual ~IAnrObserverTest()
        {}
        void OnAnr(int32_t pid) const override
        {
            MMI_HILOGD("Set anr success");
        };
    };

    std::shared_ptr<IAnrObserverTest> observer = std::make_shared<IAnrObserverTest>();
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->SetAnrObserver(observer));
}

/**
 * @tc.name: InputManagerTest_EnableInputDevice_001
 * @tc.desc: Enable input device
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_EnableInputDevice_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto ret = InputManager::GetInstance()->EnableInputDevice(false);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->EnableInputDevice(true);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InputManagerTest_SensorInputTime_001
 * @tc.desc: Test SensorTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SensorInputTime_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_TRUE(pointerEvent != nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSensorInputTime(POINTER_SENSOR_INPUT_TIME);
    ASSERT_TRUE(pointerEvent->GetSensorInputTime() == POINTER_SENSOR_INPUT_TIME);
}

/**
 * @tc.name: InputManagerTest_GetDisplayBindInfo_001
 * @tc.desc: Get diaplay bind information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetDisplayBindInfo_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    OHOS::MMI::DisplayBindInfos infos;
    int32_t ret = InputManager::GetInstance()->GetDisplayBindInfo(infos);
    ASSERT_TRUE(ret == RET_OK);
    if (ret != RET_OK) {
        MMI_HILOGE("Call GetDisplayBindInfo failed, ret:%{public}d", ret);
    }
}

/**
 * @tc.name: InputManagerTest_SetDisplayBind_001
 * @tc.desc: Set diaplay bind information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetDisplayBind_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = DEFAULT_DEVICE_ID;
    int32_t displayId = INVAID_VALUE;
    std::string msg;
    int32_t ret = InputManager::GetInstance()->SetDisplayBind(deviceId, displayId, msg);
    ASSERT_TRUE(ret == RET_OK);
    if (ret != RET_OK) {
        MMI_HILOGE("Call SetDisplayBind failed, ret:%{public}d", ret);
    }
}

/**
 * @tc.name: InputManagerTest_MarkConsumed_001
 * @tc.desc: Mark Cosumer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_MarkConsumed_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto consumer = GetPtr<InputEventConsumer>();
    ASSERT_TRUE(consumer != nullptr);
    int32_t monitorId = InputManager::GetInstance()->AddMonitor(consumer);
    auto pointerEvent = PointerEvent::Create();
    ASSERT_TRUE(pointerEvent != nullptr);
    auto eventId = pointerEvent->GetId();
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->MarkConsumed(monitorId, eventId));
}

/**
 * @tc.name: InputManagerTest_EnterCaptureMode_001
 * @tc.desc: Entering capture mode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_EnterCaptureMode_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    CHKPV(window);
    uint32_t windowId = window->GetWindowId();
    int32_t ret = InputManager::GetInstance()->EnterCaptureMode(windowId);
    ASSERT_TRUE(ret == RET_OK);
    if (ret != RET_OK) {
        MMI_HILOGE("Call EnterCaptureMode failed, ret:%{public}d", ret);
    }
}

/**
 * @tc.name: InputManagerTest_LeaveCaptureMode_001
 * @tc.desc: Leaving capture mode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_LeaveCaptureMode_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    CHKPV(window);
    uint32_t windowId = window->GetWindowId();
    int32_t ret = InputManager::GetInstance()->LeaveCaptureMode(windowId);
    ASSERT_TRUE(ret == RET_OK);
    if (ret != RET_OK) {
        MMI_HILOGE("Call LeaveCaptureMode failed, ret:%{public}d", ret);
    }
}

/**
 * @tc.name: InputManagerTest_GetWindowPid_001
 * @tc.desc: Get window pid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetWindowPid_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    CHKPV(window);
    uint32_t windowId = window->GetWindowId();
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->GetWindowPid(windowId));
    int32_t ret = InputManager::GetInstance()->GetWindowPid(windowId);
    if (ret == RET_ERR) {
        MMI_HILOGE("Call GetWindowPid failed, ret:%{public}d", ret);
    }
}


/**
 * @tc.name: InputManagerTest_SetCustomCursor
 * @tc.desc: Test set the wrong windowId for SetCustomCursor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetCustomCursor, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t fakeWindowId = 100;
    const std::string iconPath = "/system/etc/multimodalinput/mouse_icon/North_South.svg";
    PointerStyle pointerStyle;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = InputManagerUtil::SetMouseIconTest(iconPath);
    ASSERT_NE(pixelMap, nullptr);
    pointerStyle.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    ASSERT_TRUE(InputManager::GetInstance()->SetCustomCursor(fakeWindowId, (void *)pixelMap.get(), 32, 32) == RET_ERR);
}

/**
 * @tc.name: InputManagerTest_SetMouseIcon
 * @tc.desc: Test set the wrong windowId for SetMouseIcon
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetMouseIcon, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t fakeWindoId = 100;
    const std::string iconPath = "/system/etc/multimodalinput/mouse_icon/North_South.svg";
    PointerStyle pointerStyle;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = InputManagerUtil::SetMouseIconTest(iconPath);
    ASSERT_NE(pixelMap, nullptr);
    pointerStyle.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    ASSERT_TRUE(InputManager::GetInstance()->SetMouseIcon(fakeWindoId, (void *)pixelMap.get()) == RET_ERR);
}

/**
 * @tc.name: InputManagerTest_SetMouseHotSpot
 * @tc.desc: Test set the wrong windowId for SetMouseHotSpot
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetMouseHotSpot, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    PointerStyle pointerStyle;
    pointerStyle.id = MOUSE_ICON::CROSS;
    int32_t fakeWindoId = 100;
    int32_t mouseIcon = 20;
    ASSERT_TRUE(
        InputManager::GetInstance()->SetMouseHotSpot(fakeWindoId, mouseIcon, mouseIcon) == RET_ERR);
}


/**
 * @tc.name: InputManagerTest_SetKeyDownDuration_001
 * @tc.desc: Customize the delay time for starting the ability by using the shortcut key.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetKeyDownDuration_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::string businessId = "";
    int32_t delay = KEY_DOWN_DURATION;
    ASSERT_EQ(PARAMETER_ERROR, InputManager::GetInstance()->SetKeyDownDuration(businessId, delay));
}

/**
 * @tc.name: InputManagerTest_UnsubscribeSwitchEvent_001
 * @tc.desc: Unsubscribes from a switch input event.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UnsubscribeSwitchEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t subscriberId = INVAID_VALUE;
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UnsubscribeSwitchEvent(subscriberId));
}

/**
 * @tc.name: InputManagerTest_ClearWindowPointerStyle_001
 * @tc.desc: Verify invalid parameter.
 * @tc.type: FUNC
 * @tc.require:SR000GGQL4  AR000GJNGN
 * @tc.author: yangguang
 */
HWTEST_F(InputManagerTest, InputManagerTest_ClearWindowPointerStyle_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto mmichecker = std::make_shared<MMIWindowChecker>();
    InputManager::GetInstance()->SetWindowCheckerHandler(mmichecker);
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    CHKPV(window);
    uint32_t windowId = window->GetWindowId();
    PointerStyle pointerStyle;
    pointerStyle.id = MOUSE_ICON::CROSS;
    int32_t ret = InputManager::GetInstance()->SetPointerStyle(windowId, pointerStyle);
    InputManager::GetInstance()->ClearWindowPointerStyle(getpid(), windowId);
    PointerStyle style;
    ret = InputManager::GetInstance()->GetPointerStyle(windowId, style);
    EXPECT_TRUE(ret == RET_OK);
}

HWTEST_F(InputManagerTest, InputManagerTest_SyncBundleName_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto mmiObserver = std::make_shared<IEventObserver>();
    InputManager::GetInstance()->AddInputEventObserver(mmiObserver);
    auto callbackPtr = GetPtr<InputEventCallback>();
    ASSERT_TRUE(callbackPtr != nullptr);
    int32_t monitorId = InputManagerUtil::TestAddMonitor(callbackPtr);
    InputManager::GetInstance()->SetNapStatus(10, 20, "bundleName_test", 0);
    std::map<std::tuple<int32_t, int32_t, std::string>, int32_t> mapBefore;
    InputManager::GetInstance()->GetAllMmiSubscribedEvents(mapBefore);
    for (auto map = mapBefore.begin(); map != mapBefore.end(); ++map) {
        if (std::get<TUPLE_PID>(map->first) == 10) {
            EXPECT_TRUE(std::get<TUPLE_UID>(map->first) == 20);
            EXPECT_TRUE(std::get<TUPLE_NAME>(map->first) == "bundleName_test");
            EXPECT_TRUE(map->second == 0);
        }
    }
    for (const auto& map : mapBefore) {
        MMI_HILOGD("All NapStatus in mapBefore pid:%{public}d, uid:%{public}d, name:%{public}s, status:%{public}d",
            std::get<TUPLE_PID>(map.first), std::get<TUPLE_UID>(map.first), std::get<TUPLE_NAME>(map.first).c_str(),
            map.second);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    InputManagerUtil::TestRemoveMonitor(monitorId);
    InputManager::GetInstance()->SetNapStatus(10, 20, "bundleName_test", 2);
    std::map<std::tuple<int32_t, int32_t, std::string>, int32_t> mapAfter;
    InputManager::GetInstance()->GetAllMmiSubscribedEvents(mapAfter);
    for (const auto& map : mapAfter) {
        EXPECT_FALSE(std::get<TUPLE_PID>(map.first) == 10);
        EXPECT_FALSE(std::get<TUPLE_UID>(map.first) == 20);
        EXPECT_FALSE(std::get<TUPLE_NAME>(map.first) == "bundleName_test");
    }
    for (const auto& map : mapAfter) {
        MMI_HILOGD("All NapStatus in mapAfter pid:%{public}d, uid:%{public}d, name:%{public}s, status:%{public}d",
            std::get<TUPLE_PID>(map.first), std::get<TUPLE_UID>(map.first), std::get<TUPLE_NAME>(map.first).c_str(),
            map.second);
    }
    InputManager::GetInstance()->RemoveInputEventObserver(mmiObserver);
}

/**
 * @tc.name: InputManager_InjectMouseEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectMouseEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectMouseEvent_002
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectMouseEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectMouseEvent_003
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectMouseEvent_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectMouseEvent_004
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectMouseEvent_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHPAD);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectMouseEvent_005
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectMouseEvent_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHPAD);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectTouchEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectTouchEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectTouchEvent_002
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectTouchEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(200);
    item.SetDisplayY(200);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_InjectEvent_003
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectEvent_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    ASSERT_NO_FATAL_FAILURE(keyEvent->SetRepeat(true));
}

/**
 * @tc.name: InputManager_InjectEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);

    KeyEvent::KeyItem item;
    keyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    item.SetKeyCode(2017);
    item.SetPressed(true);
    item.SetDownTime(500);
    keyEvent->AddKeyItem(item);
    InputManager::GetInstance()->SimulateInputEvent(keyEvent);
}

/**
 * @tc.name: InputManager_InjectEvent_002
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_InjectEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::vector<int32_t> downKey;
    downKey.push_back(2072);
    downKey.push_back(2017);

    KeyEvent::KeyItem item[downKey.size()];
    for (size_t i = 0; i < downKey.size(); i++) {
        keyEvent->SetKeyCode(2072);
        keyEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
        item[i].SetKeyCode(downKey[i]);
        item[i].SetPressed(true);
        item[i].SetDownTime(0);
        keyEvent->AddKeyItem(item[i]);
    }
    InputManager::GetInstance()->SimulateInputEvent(keyEvent);
}

/**
 * @tc.name: InputManagerTest_GetPointerColor_001
 * @tc.desc: Obtains the mouse color.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_GetPointerColor_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t setColor = 0x000000;
    InputManager::GetInstance()->SetPointerColor(setColor);
    int32_t getColor = 3;
    ASSERT_TRUE(InputManager::GetInstance()->GetPointerColor(getColor) == RET_OK);
}

/**
 * @tc.name: InputManagerTest_SimulateInputEventExt_001
 * @tc.desc: Obtains the mouse color.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SimulateInputEventExt_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetDisplayY(POINTER_ITEM_DISPLAY_Y_TWO);
    item.SetDisplayX(POINTER_ITEM_DISPLAY_X_ONE);
    item.SetPressure(POINTER_ITEM_PRESSURE);
    item.SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item);

#ifdef OHOS_BUILD_ENABLE_ANCO
    InputManager::GetInstance()->SimulateInputEventExt(pointerEvent);
    InputManager::GetInstance()->SimulateInputEventExt(pointerEvent);
    InputManager::GetInstance()->SimulateInputEventExt(pointerEvent);
#endif // OHOS_BUILD_ENABLE_ANCO
}

/**
 * @tc.name: InputManagerTest_SimulateInputEventExt_002
 * @tc.desc: Obtains the mouse color.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SimulateInputEventExt_002, TestSize.Level1)
{
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_VOLUME_DOWN);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_VOLUME_DOWN);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);

#ifdef OHOS_BUILD_ENABLE_ANCO
    InputManager::GetInstance()->SimulateInputEventExt(injectDownEvent);
    ASSERT_EQ(injectDownEvent->GetKeyAction(), KeyEvent::KEY_ACTION_DOWN);
#endif // OHOS_BUILD_ENABLE_ANCO
}

/**
 * @tc.name: InputManagerTest_SimulateInputEventZorder_001
 * @tc.desc: Simulate input evnet with zOrder.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SimulateInputEventZorder_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    PointerEvent::PointerItem item;
    item.SetDisplayY(POINTER_ITEM_DISPLAY_Y_TWO);
    item.SetDisplayX(POINTER_ITEM_DISPLAY_X_ONE);
    item.SetPressure(POINTER_ITEM_PRESSURE);
    item.SetPointerId(0);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetZOrder(10.0);

    InputManager::GetInstance()->SimulateInputEvent(pointerEvent, 10.0);
}

/**
 * @tc.name: InputManagerTest_SetShieldStatus_001
 * @tc.desc: Test set shield status
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetShieldStatus_001, TestSize.Level1)
{
    bool factoryModeStatus = false;
    bool oobeModeStatus = false;
    int32_t ret = InputManager::GetInstance()->SetShieldStatus(SHIELD_MODE::FACTORY_MODE, true);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::FACTORY_MODE, factoryModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::OOBE_MODE, oobeModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ASSERT_TRUE(factoryModeStatus);
    ASSERT_FALSE(oobeModeStatus);
    ret = InputManager::GetInstance()->SetShieldStatus(SHIELD_MODE::OOBE_MODE, true);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::FACTORY_MODE, factoryModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::OOBE_MODE, oobeModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ASSERT_FALSE(factoryModeStatus);
    ASSERT_TRUE(oobeModeStatus);
    ret = InputManager::GetInstance()->SetShieldStatus(SHIELD_MODE::OOBE_MODE, false);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::FACTORY_MODE, factoryModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->GetShieldStatus(SHIELD_MODE::OOBE_MODE, oobeModeStatus);
    ASSERT_EQ(ret, RET_OK);
    ASSERT_FALSE(factoryModeStatus);
    ASSERT_FALSE(oobeModeStatus);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_001
 * @tc.desc: Set SourceType to SOURCE_TYPE_MOUSE
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_002
 * @tc.desc: Set SourceType to SOURCE_TYPE_TOUCHPAD
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHPAD);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_003
 * @tc.desc: Set SourceType to SOURCE_TYPE_TOUCHSCREEN
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputEvent_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_004
 * @tc.desc: Set SourceType to SOURCE_TYPE_JOYSTICK
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputEvent_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_JOYSTICK);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_005
 * @tc.desc: Set SourceType to invalid
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputEvent_005, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_UNKNOWN);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateInputEvent_001
 * @tc.desc: SimulateInputEvent interface detection
 * @tc.type: FUNC
 * @tc.require:SR000GGN6G
 */
HWTEST_F(InputManagerTest, InputManager_SimulateInputKeyEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    InputManager::GetInstance()->SimulateInputEvent(keyEvent);
}

/**
 * @tc.name: InputManagerTest_SetWindowPointerStyle_001
 * @tc.desc: Verify valid parameter.
 * @tc.type: FUNC
 * @tc.require:SR000GGQL4  AR000GJNGN
 */
HWTEST_F(InputManagerTest, InputManagerTest_SetWindowPointerStyle_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    CHKPV(window);
    uint32_t windowId = window->GetWindowId();
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::ENTER, getpid(), windowId);
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::FOCUS_ON_TOP, getpid(), windowId);
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::FOCUS_ON_RIGHT, getpid(), windowId);
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::FOCUS_ON_BOTTOM_LEFT, getpid(), windowId);
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::TOP_LIMIT, getpid(), windowId);
    InputManager::GetInstance()->SetWindowPointerStyle(WindowArea::BOTTOM_RIGHT_LIMIT, getpid(), windowId);
}

/**
 * @tc.name: InputManagerTest_RemoveInputEventFilter_001
 * @tc.desc: When eventFilterService is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_RemoveInputEventFilter_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t ret = InputManager::GetInstance()->RemoveInputEventFilter(-1);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->RemoveInputEventFilter(0);
    ASSERT_EQ(ret, RET_OK);
    ret = InputManager::GetInstance()->RemoveInputEventFilter(1);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InputManagerTest_RemoveInputEventFilter_002
 * @tc.desc: When the eventFilterService is full
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_RemoveInputEventFilter_002, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    struct KeyFilter : public IInputEventFilter {
        bool OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const override
        {
            MMI_HILOGI("KeyFilter::OnInputEvent enter,pid: %{public}d", getpid());
            return false;
        }
        bool OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override
        {
            return false;
        }
    };
    auto addFilter = []() -> int32_t {
        auto filter = std::make_shared<KeyFilter>();
        uint32_t touchTags = CapabilityToTags(InputDeviceCapability::INPUT_DEV_CAP_MAX);
        const int32_t filterId = InputManager::GetInstance()->AddInputEventFilter(filter, 220, touchTags);
        return filterId;
    };
    const size_t singleClientSuportMaxNum = 4;
    for (size_t i = 0; i < singleClientSuportMaxNum; ++i) {
        int32_t filterId = addFilter();
        ASSERT_NE(filterId, RET_ERR);
    }
    int32_t filterId = addFilter();
    ASSERT_EQ(filterId, RET_ERR);
    auto ret = InputManager::GetInstance()->RemoveInputEventFilter(RET_ERR);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InputManagerTest_RemoveInputEventFilter_003
 * @tc.desc: Verify valid parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_RemoveInputEventFilter_003, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    struct KeyFilter : public IInputEventFilter {
        bool OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const override
        {
            MMI_HILOGI("KeyFilter::OnInputEvent enter,pid: %{public}d", getpid());
            return false;
        }
        bool OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override
        {
            return false;
        }
    };
    auto addFilter = []() -> int32_t {
        auto filter = std::make_shared<KeyFilter>();
        uint32_t touchTags = CapabilityToTags(InputDeviceCapability::INPUT_DEV_CAP_MAX);
        int32_t filterId = InputManager::GetInstance()->AddInputEventFilter(filter, 220, touchTags);
        return filterId;
    };
    int32_t filterId = addFilter();
    ASSERT_NE(filterId, RET_ERR);
    auto ret = InputManager::GetInstance()->RemoveInputEventFilter(filterId);
    ASSERT_EQ(ret, RET_OK);
    filterId = addFilter();
    ASSERT_NE(filterId, RET_ERR);
    ret = InputManager::GetInstance()->RemoveInputEventFilter(filterId);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InputManager_SlideUpBrightScreenUnlockEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR000GJG6G
 */
HWTEST_F(InputManagerTest, InputManager_SlideUpBrightScreenUnlockEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_NE(injectDownEvent, nullptr);
    injectDownEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);

    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_F5);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(500);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_F5);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
    InputManager::GetInstance()->SimulateInputEvent(injectDownEvent);

    std::shared_ptr<KeyEvent> injectUpEvent = KeyEvent::Create();
    ASSERT_NE(injectUpEvent, nullptr);
    injectDownEvent->AddFlag(InputEvent::EVENT_FLAG_NO_INTERCEPT);

    KeyEvent::KeyItem kitUp;
    kitUp.SetKeyCode(KeyEvent::KEYCODE_F5);
    kitUp.SetPressed(false);
    kitUp.SetDownTime(500);
    injectUpEvent->SetKeyCode(KeyEvent::KEYCODE_F5);
    injectUpEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    injectUpEvent->RemoveReleasedKeyItems(kitUp);
    InputManager::GetInstance()->SimulateInputEvent(injectUpEvent);
}

/**
 * @tc.name: InputManager_SimulateEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR20240223308600
 */
HWTEST_F(InputManagerTest, InputManager_SimulateEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = InputManagerUtil::SetupSimulateEvent001();
    MMI_HILOGI("Before handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    MMI_HILOGI("After handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR20240223308600
 */
HWTEST_F(InputManagerTest, InputManager_SimulateEvent_002, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = InputManagerUtil::SetupSimulateEvent002();
    MMI_HILOGI("Before handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    MMI_HILOGI("After handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR20240223308600
 */
HWTEST_F(InputManagerTest, InputManager_SimulateEvent_003, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = InputManagerUtil::SetupSimulateEvent003();
    MMI_HILOGI("Before handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    MMI_HILOGI("After handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
}

/**
 * @tc.name: InputManager_SimulateEvent_001
 * @tc.desc: Injection interface detection
 * @tc.type: FUNC
 * @tc.require:AR20240223308600
 */
HWTEST_F(InputManagerTest, InputManager_SimulateEvent_004, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto pointerEvent = InputManagerUtil::SetupSimulateEvent004();
    MMI_HILOGI("Before handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    MMI_HILOGI("After handle SimulateInputEvent");
    InputManagerUtil::PrintPointerEventId(pointerEvent);
}

#ifdef INPUT_MANAGER_TEST_ENABLE_DEMO
class ServiceWatcher final : public IInputServiceWatcher {
public:
    ServiceWatcher() = default;
    ~ServiceWatcher() = default;

    void OnServiceDied() override
    {}
};

/**
 * @tc.name: InputManagerTest_InputServiceWatcher
 * @tc.desc: Verify service watcher.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_InputServiceWatcher, TestSize.Level1)
{
    auto watcher = std::make_shared<ServiceWatcher>();
    InputManager::GetInstance()->AddServiceWatcher(watcher);
    InputManager::GetInstance()->RemoveServiceWatcher(watcher);
}
#endif // INPUT_MANAGER_TEST_ENABLE_DEMO

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for 1 display and 1 window
 * @tc.desc: Update shaped window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo_Shaped01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t width = 200;
    int32_t height = 300;
    OHOS::Media::PixelMap* pixelMap = MatrixToPixelmap(CreateMatrix(width, height)).get();
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 1;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    displayInfo.id = 0;
    displayInfo.x =1;
    displayInfo.y = 1;
    displayInfo.width = 2;
    displayInfo.height = 2;
    displayInfo.dpi = 240;
    displayInfo.name = "pp";
    displayInfo.uniq = "pp";
    displayInfo.direction = DIRECTION0;
    displayGroupInfo.displaysInfo.push_back(displayInfo);
    WindowInfo info;
    info.id = 1;
    info.pid = 1;
    info.uid = 1;
    info.area = {1, 1, 1, 1};
    info.defaultHotAreas = { info.area };
    info.pointerHotAreas = { info.area };
    info.pointerChangeAreas = {16, 5, 16, 5, 16, 5, 16, 5};
    info.transform = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    info.agentWindowId = 1;
    info.flags = 0;
    info.displayId = 0;
    info.pixelMap = pixelMap;
    displayGroupInfo.windowsInfo.push_back(info);
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for 1 display and max-windows
 * @tc.desc: Update shaped window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo_Shaped02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 0;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    displayInfo.id = 0;
    displayInfo.x =1;
    displayInfo.y = 1;
    displayInfo.width = 2;
    displayInfo.height = 2;
    displayInfo.dpi = 240;
    displayInfo.name = "pp";
    displayInfo.uniq = "pp";
    displayInfo.direction = DIRECTION0;
    displayInfo.displayMode = DisplayMode::FULL;
    displayGroupInfo.displaysInfo.push_back(displayInfo);
    for (uint32_t i = 0; i < MAX_WINDOW_NUMS; i++) {
        WindowInfo info;
        info.id = i + 1;
        info.pid = 1;
        info.uid = 1;
        info.area = {1, 1, 1, 1};
        info.defaultHotAreas = { info.area };
        info.pointerHotAreas = { info.area };
        info.pointerChangeAreas = {16, 5, 16, 5, 16, 5, 16, 5};
        info.transform = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        info.agentWindowId = 1;
        info.flags = 0;
        info.displayId = 0;
        info.zOrder = static_cast<float>(MAX_WINDOW_NUMS - i);
        int32_t width = 100;
        int32_t height = 200;
        OHOS::Media::PixelMap* pixelMap = MatrixToPixelmap(CreateMatrix(width, height)).get();
        info.pixelMap = pixelMap;
        displayGroupInfo.windowsInfo.push_back(info);
    }
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputManagerTest_UpdateDisplayInfo for max-display and max-window
 * @tc.desc: Update shaped window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerTest, InputManagerTest_UpdateDisplayInfo_Shaped03, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 1;
    displayGroupInfo.width = 1000;
    displayGroupInfo.height = 2000;
    DisplayInfo displayInfo;
    for (uint32_t i = 0; i < 2; i++) { // one is default-display and another is simulate display
        displayInfo.id = i;
        displayInfo.x =1;
        displayInfo.y = 1;
        displayInfo.width = 2;
        displayInfo.height = 2;
        displayInfo.dpi = 240;
        displayInfo.name = "pp";
        displayInfo.uniq = "pp";
        displayInfo.direction = DIRECTION0;
        displayGroupInfo.displaysInfo.push_back(displayInfo);
    }
    WindowInfo info;
    for (uint32_t i = 0; i < 2; i++) { // 2 widnows for 2 display
        info.id = 1;
        info.pid = 1;
        info.uid = 1;
        info.defaultHotAreas = { {1, 1, 1, 1} };
        info.agentWindowId = 1;
        info.flags = 0;
        info.displayId = i;
        int32_t width = 200;
        int32_t height = 50;
        OHOS::Media::PixelMap* pixelMap = MatrixToPixelmap(CreateMatrix(width, height)).get();
        info.pixelMap = pixelMap;
        displayGroupInfo.windowsInfo.push_back(info);
    }
    ASSERT_NO_FATAL_FAILURE(InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo));
}
} // namespace MMI
} // namespace OHOS
