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

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_handler_manager.h"
#include "input_manager.h"
#include "multimodal_event_handler.h"
#include "nativetoken_kit.h"
#include "pointer_event.h"
#include "proto.h"
#include "token_setproc.h"

namespace OHOS {
namespace MMI {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
using namespace testing::ext;
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
constexpr int32_t TIME_WAIT_FOR_OP = 500;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerManualTest" };
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

HapInfoParams infoManagerTestInfoParms = {
    .bundleName = "accesstoken_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test"
};

PermissionDef infoManagerTestPermDef = {
    .permissionName = "ohos.permission.test",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test input event filter",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_CORE
};

PermissionStateFull infoManagerTestState = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.test",
    .resDeviceID = {"local"}
};

HapPolicyParams infoManagerTestPolicyPrams = {
    .apl = APL_SYSTEM_CORE,
    .domain = "test.domain",
    .permList = {infoManagerTestPermDef},
    .permStateList = {infoManagerTestState}
};
} // namespace

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(infoManagerTestInfoParms, infoManagerTestPolicyPrams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(accessID_);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

class InputManagerManualTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}

    void SetUp();
    void TearDown() {}

protected:
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void AddInputEventFilter();
    void SimulateInputEventHelper(int32_t physicalX, int32_t physicalY, int32_t expectVal);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    int32_t callbackRet = 0;
};

void InputManagerManualTest::SetUp()
{
    callbackRet = 0;
}

/**
 * @tc.name: StartBytrace_001
 * @tc.desc: Verify keyevent start bytrace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerManualTest, StartBytrace_001, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->SetId(0);
    BytraceAdapter::StartBytrace(keyEvent);
    ASSERT_EQ(keyEvent->GetId(), 0);
}

/**
 * @tc.name: StartBytrace_002
 * @tc.desc: Verify keyevent start bytrace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerManualTest, StartBytrace_002, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->SetId(0);
    keyEvent->SetKeyCode(0);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_INTERCEPT_EVENT);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_LAUNCH_EVENT);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_SUBSCRIBE_EVENT);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_DISPATCH_EVENT);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::POINT_INTERCEPT_EVENT);
    ASSERT_EQ(keyEvent->GetKeyCode(), 0);
}

/**
 * @tc.name: StartBytrace_003
 * @tc.desc: Verify pointerEvent start bytrace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerManualTest, StartBytrace_003, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_STOP);
    ASSERT_EQ(pointerEvent->GetSourceType(), PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
}

/**
 * @tc.name: StartBytrace_004
 * @tc.desc: Verify pointerEvent start bytrace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerManualTest, StartBytrace_004, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::START_EVENT);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::LAUNCH_EVENT);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_START, BytraceAdapter::STOP_EVENT);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::START_EVENT);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::LAUNCH_EVENT);
    BytraceAdapter::StartBytrace(BytraceAdapter::TRACE_STOP, BytraceAdapter::STOP_EVENT);
    ASSERT_EQ(pointerEvent->GetSourceType(), PointerEvent::SOURCE_TYPE_MOUSE);
}

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
void InputManagerManualTest::AddInputEventFilter()
{
    CALL_DEBUG_ENTER;
    auto callback = [this](std::shared_ptr<PointerEvent> pointer) -> bool {
        MMI_HILOGD("Callback enter");
        CHKPF(pointer);
        const std::vector<int32_t> ids = pointer->GetPointerIds();
        if (ids.empty()) {
            MMI_HILOGE("The ids is empty");
            return false;
        }

        const int32_t firstPointerId = ids[0];
        PointerEvent::PointerItem item;
        if (!pointer->GetPointerItem(firstPointerId, item)) {
            MMI_HILOGE("GetPointerItem:%{public}d fail", firstPointerId);
            return false;
        }

        const int32_t x = item.GetDisplayX();
        const int32_t y = item.GetDisplayY();
        if (x == 10 && y == 10) {
            MMI_HILOGI("The values of X and y are both 10, which meets the expectation and callbackRet is set to 1");
            callbackRet = 1;
            return false;
        }

        MMI_HILOGI("The values of X and y are not 10, which meets the expectation and callbackRet is set to 2");
        callbackRet = 2;
        return false;
    };
    AccessToken accessToken;
    int32_t ret = InputManager::GetInstance()->AddInputEventFilter(callback);
    ASSERT_EQ(ret, RET_OK);
}

void InputManagerManualTest::SimulateInputEventHelper(int32_t physicalX, int32_t physicalY, int32_t expectVal)
{
    CALL_DEBUG_ENTER;
    const int32_t pointerId = 0;
    PointerEvent::PointerItem item;
    item.SetPointerId(pointerId);
    item.SetDisplayX(physicalX);
    item.SetDisplayY(physicalY);

    auto pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerId(pointerId);

    MMI_HILOGI("Call InputManager::SimulateInputEvent");
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    EXPECT_EQ(callbackRet, expectVal);
}

/**
 * @tc.name: HandlePointerEventFilter_001
 * @tc.desc: Verify pointer event filter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputManagerManualTest, HandlePointerEventFilter_001, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    AddInputEventFilter();
    SimulateInputEventHelper(10, 10, 1); // set physical x and physical y are 10, will expect value is 1
    SimulateInputEventHelper(0, 0, 2); // set physical x and physical y are not 10, will expect value is 2
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
} // namespace MMI
} // namespace OHOS