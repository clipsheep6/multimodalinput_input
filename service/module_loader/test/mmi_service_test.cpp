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

#include <gtest/gtest.h>

#include "proto.h"

#define protected public
#define private public
#include "mmi_service.h"

#include "udp_wrap.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class MMIServerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: AddEpollAndDelEpoll_001
 * @tc.desc: Test the function AddEpoll and DelEpoll
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AddEpollAndDelEpoll_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t fd = -1;
    int32_t ret = RET_ERR;
    ret = mmiService.AddEpoll(EPOLL_EVENT_INPUT, fd);
    EXPECT_EQ(ret, RET_ERR);
    ret = mmiService.DelEpoll(EPOLL_EVENT_INPUT, fd);
    EXPECT_EQ(ret, RET_ERR);

    fd = 1;
    ret = mmiService.AddEpoll(EPOLL_EVENT_INPUT, fd);
    EXPECT_EQ(ret, RET_ERR);
    ret = mmiService.DelEpoll(EPOLL_EVENT_INPUT, fd);
    EXPECT_EQ(ret, RET_ERR);

    ret = mmiService.AddEpoll(EPOLL_EVENT_END, fd);
    EXPECT_EQ(ret, RET_ERR);
    ret = mmiService.DelEpoll(EPOLL_EVENT_END, fd);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: InitLibinputService_001
 * @tc.desc: Test the function InitLibinputService
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, InitLibinputService_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t returnCode = 65142793;
    bool ret = mmiService.InitLibinputService();
    EXPECT_EQ(ret, false);

    ret = mmiService.InitService();
    EXPECT_EQ(ret, false);

    ret = mmiService.InitDelegateTasks();
    EXPECT_EQ(ret, false);

    int32_t retCode = mmiService.Init();
    EXPECT_EQ(retCode, returnCode);
}

/**
 * @tc.name: OnStartAndOnStop_001
 * @tc.desc: Test the function OnStart and OnStop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnStartAndOnStop_001, TestSize.Level1)
{
    MMIService mmiService;
    mmiService.OnStart();
    mmiService.OnStop();
}

/**
 * @tc.name: AddAppDebugListener_001
 * @tc.desc: Test the function AddAppDebugListener and RemoveAppDebugListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AddAppDebugListener_001, TestSize.Level1)
{
    MMIService mmiService;
    mmiService.AddAppDebugListener();
    mmiService.RemoveAppDebugListener();
}

/**
 * @tc.name: AllocSocketFd_001
 * @tc.desc: Test the function AllocSocketFd
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AllocSocketFd_001, TestSize.Level1)
{
    MMIService mmiService;
    const std::string programName = "programName";
    const int32_t moduleType = 1;
    int32_t toReturnClientFd = 1;
    int32_t tokenType = 1;

    mmiService.AllocSocketFd(programName, moduleType, toReturnClientFd, tokenType);
}

/**
 * @tc.name: AddInputEventFilter_001
 * @tc.desc: Test the function AddInputEventFilter and RemoveInputEventFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AddInputEventFilter_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t filterId = 1;
    int32_t priority = 1;
    uint32_t deviceTags = 1;
    int32_t ret = RET_ERR;
    int32_t returnCode0 = 65142804;
    int32_t returnCode = 65142786;
    sptr<IEventFilter> filter;

    ret = mmiService.AddInputEventFilter(filter, filterId, priority, deviceTags);
    EXPECT_EQ(ret, returnCode);

    ret = mmiService.RemoveInputEventFilter(filterId);
    EXPECT_EQ(ret, returnCode0);
}

/**
 * @tc.name: OnConnected_001
 * @tc.desc: Test the function OnConnected and OnDisconnected
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnConnected_001, TestSize.Level1)
{
    MMIService mmiService;
    SessionPtr session;
    mmiService.OnConnected(session);
    mmiService.OnDisconnected(session);
}

/**
 * @tc.name: SetCustomCursor_001
 * @tc.desc: Test the function SetCustomCursor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetCustomCursor_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t pid = 1;
    int32_t windowId = 1;
    int32_t focusX = 200; 
    int32_t focusY = 500;
    int32_t ret = RET_ERR;
    void* pixelMap = nullptr;

    ret = mmiService.SetCustomCursor(pid, windowId, focusX, focusY, pixelMap);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SetMouseIcon_001
 * @tc.desc: Test the function SetMouseIcon
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetMouseIcon_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t pid = 1;
    int32_t windowId = 1;
    void* pixelMap = nullptr;
    int32_t ret = RET_ERR;

    ret = mmiService.SetMouseIcon(pid, windowId, pixelMap);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SetMouseHotSpot_001
 * @tc.desc: Test the function SetMouseHotSpot
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetMouseHotSpot_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t pid = 1;
    int32_t windowId = 1;
    int32_t hotSpotX = 100;
    int32_t hotSpotY = 200;
    int32_t ret = RET_ERR;

    ret = mmiService.SetMouseHotSpot(pid, windowId, hotSpotX, hotSpotY);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SetNapStatus_001
 * @tc.desc: Test the function SetNapStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetNapStatus_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t pid = 1;
    int32_t uid = 2;
    std::string bundleName = "bundleName";
    int32_t napStatus = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.SetNapStatus(pid, uid, bundleName, napStatus);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: ReadMouseScrollRows_001
 * @tc.desc: Test the function ReadMouseScrollRows
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, ReadMouseScrollRows_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t rows = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.ReadMouseScrollRows(rows);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: SetMousePrimaryButton_001
 * @tc.desc: Test the function SetMousePrimaryButton
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetMousePrimaryButton_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t primaryButton = 1;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;

    ret = mmiService.SetMousePrimaryButton(primaryButton);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: ReadMousePrimaryButton_001
 * @tc.desc: Test the function ReadMousePrimaryButton
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, ReadMousePrimaryButton_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t primaryButton = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.ReadMousePrimaryButton(primaryButton);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: GetMousePrimaryButton_001
 * @tc.desc: Test the function GetMousePrimaryButton
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetMousePrimaryButton_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t primaryButton = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.GetMousePrimaryButton(primaryButton);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: CheckPointerVisible_001
 * @tc.desc: Test the function CheckPointerVisible
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckPointerVisible_001, TestSize.Level1)
{
    MMIService mmiService;
    bool visible = true;
    int32_t ret = RET_ERR;

    ret = mmiService.CheckPointerVisible(visible);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: MarkProcessed_001
 * @tc.desc: Test the function MarkProcessed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, MarkProcessed_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t eventType = 1;
    int32_t eventId = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.MarkProcessed(eventType, eventId);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: ReadPointerColor_001
 * @tc.desc: Test the function ReadPointerColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, ReadPointerColor_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t color = 1;
    int32_t ret = RET_ERR;

    ret = mmiService.ReadPointerColor(color);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: NotifyNapOnline_001
 * @tc.desc: Test the function NotifyNapOnline
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, NotifyNapOnline_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;

    ret = mmiService.NotifyNapOnline();
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: RemoveInputEventObserver_001
 * @tc.desc: Test the function RemoveInputEventObserver
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, RemoveInputEventObserver_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;

    ret = mmiService.RemoveInputEventObserver();
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: ClearWindowPointerStyle_001
 * @tc.desc: Test the function ClearWindowPointerStyle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, ClearWindowPointerStyle_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t pid = 1;
    int32_t windowId = 2;

    ret = mmiService.ClearWindowPointerStyle(pid,windowId);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: ReadHoverScrollState_001
 * @tc.desc: Test the function ReadHoverScrollState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, ReadHoverScrollState_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    bool state = true;

    ret = mmiService.ReadHoverScrollState(state);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: OnSupportKeys_001
 * @tc.desc: Test the function OnSupportKeys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnSupportKeys_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t deviceId = 1;
    std::vector<int32_t> keys = {1};
    std::vector<bool> keystroke = {true, true};
    std::vector<bool> keystroke1 = {true, true, true, true, true, true};

    ret = mmiService.OnSupportKeys(deviceId, keys, keystroke);
    EXPECT_EQ(ret, RET_OK);

    ret = mmiService.OnSupportKeys(deviceId, keys, keystroke1);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SupportKeys_001
 * @tc.desc: Test the function SupportKeys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SupportKeys_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t deviceId = 1;
    int32_t returnCode = 65142804;
    std::vector<int32_t> keys = {1};
    std::vector<bool> keystroke = {true, true};

    ret = mmiService.SupportKeys(deviceId, keys, keystroke);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: OnGetDeviceIds_001
 * @tc.desc: Test the function OnGetDeviceIds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnGetDeviceIds_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    std::vector<int32_t> ids= {1};

    ret = mmiService.OnGetDeviceIds(ids);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: GetDeviceIds_001
 * @tc.desc: Test the function GetDeviceIds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetDeviceIds_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    std::vector<int32_t> ids= {1};

    ret = mmiService.GetDeviceIds(ids);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: OnGetDevice_001
 * @tc.desc: Test the function OnGetDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnGetDevice_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t deviceId = 1;
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();

    ret = mmiService.OnGetDevice(deviceId, inputDevice);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: GetDevice_001
 * @tc.desc: Test the function GetDevice
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetDevice_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;
    int32_t deviceId = 1;
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();

    ret = mmiService.GetDevice(deviceId, inputDevice);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: OnRegisterDevListener_001
 * @tc.desc: Test the function OnRegisterDevListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnRegisterDevListener_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t pid = 1;

    ret = mmiService.OnRegisterDevListener(pid);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: RegisterDevListener_001
 * @tc.desc: Test the function RegisterDevListener and OnUnregisterDevListener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, RegisterDevListener_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t pid = 1;

    ret = mmiService.RegisterDevListener();
    EXPECT_EQ(ret, RET_ERR);

    ret = mmiService.UnregisterDevListener();
    EXPECT_EQ(ret, RET_ERR);

    ret = mmiService.OnUnregisterDevListener(pid);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: OnGetKeyboardType_001
 * @tc.desc: Test the function OnGetKeyboardType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnGetKeyboardType_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t deviceId = 1;
    int32_t keyboardType = 1;

    ret = mmiService.OnGetKeyboardType(deviceId, keyboardType);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: GetKeyboardType_001
 * @tc.desc: Test the function GetKeyboardType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetKeyboardType_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;
    int32_t deviceId = 1;
    int32_t keyboardType = 1;

    ret = mmiService.GetKeyboardType(deviceId, keyboardType);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: GetKeyboardRepeatDelay_001
 * @tc.desc: Test the function GetKeyboardRepeatDelay
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetKeyboardRepeatDelay_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;
    int32_t delay = 1;

    ret = mmiService.GetKeyboardRepeatDelay(delay);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: GetKeyboardRepeatRate_001
 * @tc.desc: Test the function GetKeyboardRepeatRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetKeyboardRepeatRate_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;
    int32_t rate = 1;

    ret = mmiService.GetKeyboardRepeatRate(rate);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: CheckAddInput_001
 * @tc.desc: Test the function CheckAddInput
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckAddInput_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142786;
    int32_t pid = 1;
    InputHandlerType handlerType = InputHandlerType::MONITOR;
    HandleEventType eventType = 10;
    int32_t priority = 1;
    uint32_t deviceTags = 1;

    ret = mmiService.CheckAddInput(pid, handlerType, eventType, priority, deviceTags);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: AddInputHandler_001
 * @tc.desc: Test the function AddInputHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AddInputHandler_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    InputHandlerType handlerType = InputHandlerType::INTERCEPTOR;
    HandleEventType eventType = 10;
    int32_t priority = 1;
    uint32_t deviceTags = 1;

    ret = mmiService.AddInputHandler(handlerType, eventType, priority, deviceTags);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: CheckRemoveInput_001
 * @tc.desc: Test the function CheckRemoveInput
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckRemoveInput_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142786;
    int32_t pid = 1;
    InputHandlerType handlerType = InputHandlerType::INTERCEPTOR;
    HandleEventType eventType = 1;
    int32_t priority = 1;
    uint32_t deviceTags = 1;

    ret = mmiService.CheckRemoveInput(pid, handlerType, eventType, priority, deviceTags);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: RemoveInputHandler_001
 * @tc.desc: Test the function RemoveInputHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, RemoveInputHandler_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    InputHandlerType handlerType = InputHandlerType::INTERCEPTOR;
    HandleEventType eventType = 1;
    int32_t priority = 1;
    uint32_t deviceTags = 1;

    ret = mmiService.RemoveInputHandler(handlerType, eventType, priority, deviceTags);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: CheckMarkConsumed_001
 * @tc.desc: Test the function CheckMarkConsumed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckMarkConsumed_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142786;
    int32_t pid = 1;
    int32_t eventId = 1;

    ret = mmiService.CheckMarkConsumed(pid, eventId);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: MoveMouseEvent_001
 * @tc.desc: Test the function MoveMouseEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, MoveMouseEvent_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t offsetX = 100;
    int32_t offsetY = 200;

    ret = mmiService.MoveMouseEvent(offsetX, offsetY);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: CheckInjectKeyEvent_001
 * @tc.desc: Test the function CheckInjectKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckInjectKeyEvent_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142786;
    const std::shared_ptr<KeyEvent> Event{ nullptr };
    int32_t pid = 1;
    bool isNativeInject = false;

    ret = mmiService.CheckInjectKeyEvent(Event, pid, isNativeInject);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: OnAddSystemAbility_001
 * @tc.desc: Test the function OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnAddSystemAbility_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t systemAbilityId = 1;
    const std::string deviceId = "device_id";

    systemAbilityId = RES_SCHED_SYS_ABILITY_ID;
    ASSERT_NO_FATAL_FAILURE(mmiService.OnAddSystemAbility(systemAbilityId, deviceId));

    systemAbilityId = COMMON_EVENT_SERVICE_ID;
    ASSERT_NO_FATAL_FAILURE(mmiService.OnAddSystemAbility(systemAbilityId, deviceId));

    systemAbilityId = APP_MGR_SERVICE_ID;
    ASSERT_NO_FATAL_FAILURE(mmiService.OnAddSystemAbility(systemAbilityId, deviceId));
}

/**
 * @tc.name: SubscribeKeyEvent_001
 * @tc.desc: Test the function SubscribeKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SubscribeKeyEvent_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t subscribeId = 1;
    const std::shared_ptr<KeyOption> option = std::make_shared<KeyOption>();

    ret = mmiService.SubscribeKeyEvent(subscribeId, option);
    EXPECT_EQ(ret, RET_ERR);

    ret = mmiService.UnsubscribeKeyEvent(subscribeId);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: GetDisplayBindInfo_001
 * @tc.desc: Test the function GetDisplayBindInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetDisplayBindInfo_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    DisplayBindInfos infos;

    ret = mmiService.GetDisplayBindInfo(infos);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SetDisplayBind_001
 * @tc.desc: Test the function SetDisplayBind
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetDisplayBind_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t deviceId = 1;
    int32_t displayId = 2;
    std::string msg = "test";

    ret = mmiService.SetDisplayBind(deviceId, displayId, msg);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: SetFunctionKeyState_001
 * @tc.desc: Test the function SetFunctionKeyState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetFunctionKeyState_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t funcKey = 1;
    bool enable = true;
    bool state = false;

    ret = mmiService.SetFunctionKeyState(funcKey, enable);
    EXPECT_EQ(ret, RET_ERR);

    ret = mmiService.GetFunctionKeyState(funcKey, state);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: OnDelegateTask_001
 * @tc.desc: Test the function OnDelegateTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnDelegateTask_001, TestSize.Level1)
{
    MMIService mmiService;
    epoll_event ev;

    ev.events = 0;
    ASSERT_NO_FATAL_FAILURE(mmiService.OnDelegateTask(ev));

    ev.events = 1;
    ASSERT_NO_FATAL_FAILURE(mmiService.OnDelegateTask(ev));
}

/**
 * @tc.name: OnThread_001
 * @tc.desc: Test the function OnThread
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnThread_001, TestSize.Level1)
{
    MMIService mmiService;
 
    ASSERT_NO_FATAL_FAILURE(mmiService.OnThread());
}

/**
 * @tc.name: InitSignalHandler_001
 * @tc.desc: Test the function InitSignalHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, InitSignalHandler_001, TestSize.Level1)
{
    MMIService mmiService;
    bool ret = RET_ERR;

    ret = mmiService.InitSignalHandler();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: AddReloadDeviceTimer_001
 * @tc.desc: Test the function AddReloadDeviceTimer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, AddReloadDeviceTimer_001, TestSize.Level1)
{
    MMIService mmiService;
 
    ASSERT_NO_FATAL_FAILURE(mmiService.AddReloadDeviceTimer());
}

/**
 * @tc.name: Dump_001
 * @tc.desc: Test the function Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, Dump_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t fd = -1;
    const std::vector<std::u16string> args;

    ret = mmiService.Dump(fd, args);
    EXPECT_EQ(ret, DUMP_PARAM_ERR);

    fd = 1;
    ret = mmiService.Dump(fd, args);
    EXPECT_EQ(ret, DUMP_PARAM_ERR);
}

/**
 * @tc.name: SetMouseCaptureMode_001
 * @tc.desc: Test the function SetMouseCaptureMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetMouseCaptureMode_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t windowId = 1;
    bool isCaptureMode = false;

    ret = mmiService.SetMouseCaptureMode(windowId, isCaptureMode);
    EXPECT_EQ(ret, RET_ERR);

    isCaptureMode = true;
    ret = mmiService.SetMouseCaptureMode(windowId, isCaptureMode);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: OnGetWindowPid_001
 * @tc.desc: Test the function OnGetWindowPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, OnGetWindowPid_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t windowId = 1;
    int32_t windowPid = 1;

    ret = mmiService.OnGetWindowPid(windowId, windowPid);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: GetWindowPid_001
 * @tc.desc: Test the function GetWindowPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, GetWindowPid_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t windowId = 1;

    ret = mmiService.GetWindowPid(windowId);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: UpdateCombineKeyState_001
 * @tc.desc: Test the function UpdateCombineKeyState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, UpdateCombineKeyState_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    bool enable = false;

    ret = mmiService.UpdateCombineKeyState(enable);
    EXPECT_EQ(ret, RET_OK);

    enable = true;
    ret = mmiService.UpdateCombineKeyState(enable);
    EXPECT_EQ(ret, RET_OK);
}

/**
 * @tc.name: CheckPidPermission_001
 * @tc.desc: Test the function CheckPidPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, CheckPidPermission_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t pid = 10;

    ret = mmiService.CheckPidPermission(pid);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: UpdateSettingsXml_001
 * @tc.desc: Test the function UpdateSettingsXml
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, UpdateSettingsXml_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 401;
    const std::string businessId = "testid";
    int32_t delay = 3;

    ret = mmiService.UpdateSettingsXml(businessId, delay);
    EXPECT_EQ(ret, returnCode);
}

/**
 * @tc.name: SetShieldStatus_001
 * @tc.desc: Test the function SetShieldStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MMIServerTest, SetShieldStatus_001, TestSize.Level1)
{
    MMIService mmiService;
    int32_t ret = RET_ERR;
    int32_t returnCode = 65142804;
    int32_t shieldMode = 1;
    bool isShield = 0;

    ret = mmiService.SetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(ret, returnCode);

    ret = mmiService.GetShieldStatus(shieldMode, isShield);
    EXPECT_EQ(ret, returnCode);
}
} // namespace MMI
} // namespace OHOS
