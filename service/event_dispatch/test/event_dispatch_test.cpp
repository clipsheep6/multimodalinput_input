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

#include <gtest/gtest.h>

#include "anr_manager.h"
#include "define_multimodal.h"
#include "event_dispatch_handler.h"
#include "i_input_windows_manager.h"
#include "input_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
constexpr int32_t UID_ROOT { 0 };
static constexpr char PROGRAM_NAME[] = "uds_sesion_test";
int32_t g_moduleType = 3;
int32_t g_pid = 0;
int32_t g_writeFd = -1;
} // namespace

class EventDispatchTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: EventDispatchTest_HandleTouchEvent_001
 * @tc.desc: Test the function HandleTouchEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleTouchEvent_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleTouchEvent(sharedPointerEvent));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_001
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_002
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(sharedPointerEvent, nullptr);

    std::vector<int32_t> pointerIdList;
    pointerIdList.push_back(1);
    pointerIdList.push_back(2);
    EXPECT_TRUE(pointerIdList.size() > 1);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_003
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_003, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(sharedPointerEvent, nullptr);

    std::vector<int32_t> pointerIdList;
    pointerIdList.push_back(1);
    pointerIdList.push_back(2);
    pointerIdList.push_back(3);
    EXPECT_TRUE(pointerIdList.size() > 1);

    int32_t itemPid = 5;
    EXPECT_TRUE(itemPid >= 0);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_001
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(10);
    pointerItem.SetWindowY(20);
    pointerItem.SetTargetWindowId(2);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    EXPECT_TRUE(windowInfo == std::nullopt);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_002
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 2;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(20);
    pointerItem.SetWindowY(30);
    pointerItem.SetTargetWindowId(3);

    std::optional<WindowInfo> windowInfo;
    EXPECT_TRUE(windowInfo->transform.empty());
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_003
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_003, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 5;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_UP;

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(30);
    pointerItem.SetWindowY(40);
    pointerItem.SetTargetWindowId(5);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    int32_t windowId = 2;
    bool ret = eventdispatchhandler.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(ret);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_004
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_004, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 6;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    point->pointerId_ = 2;

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(40);
    pointerItem.SetWindowY(50);
    pointerItem.SetTargetWindowId(5);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_005
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_005, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 6;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_PULL_UP;

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(45);
    pointerItem.SetWindowY(55);
    pointerItem.SetTargetWindowId(3);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_HandleMultiWindowPointerEvent_006
 * @tc.desc: Test HandleMultiWindowPointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandleMultiWindowPointerEvent_006, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 6;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_CANCEL;

    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowIds.push_back(3);

    PointerEvent::PointerItem pointerItem;
    pointerItem.SetWindowX(35);
    pointerItem.SetWindowY(50);
    pointerItem.SetTargetWindowId(2);

    std::optional<WindowInfo> windowInfo;
    windowInfo = std::nullopt;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandleMultiWindowPointerEvent(point, pointerItem));
}

/**
 * @tc.name: EventDispatchTest_NotifyPointerEventToRS_001
 * @tc.desc: Test the function NotifyPointerEventToRS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_NotifyPointerEventToRS_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t action = 1;
    std::string name = "ExampleProgram";
    uint32_t processId = 12345;
    int32_t touchCnt = 0;
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.NotifyPointerEventToRS(action, name, processId, touchCnt));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner_001
 * @tc.desc: Test the function HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    PointerEvent* pointerEvent = new PointerEvent(eventType);
    std::shared_ptr<PointerEvent> sharedPointerEvent(pointerEvent);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandlePointerEventInner(sharedPointerEvent));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner_002
 * @tc.desc: Test the function HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner_002, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> point = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(point, nullptr);
    std::vector<int32_t> windowIds;
    windowIds.push_back(1);
    windowIds.push_back(2);
    EXPECT_FALSE(windowIds.empty());
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.HandlePointerEventInner(point));
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_001
 * @tc.desc: Test the function DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_001, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    UDSServer udsServer;
    int32_t keyevent = 3;
    KeyEvent* keyEvent = new KeyEvent(keyevent);
    std::shared_ptr<KeyEvent> sharedKeyEvent(keyEvent);
    int32_t ret = eventdispatchhandler.DispatchKeyEventPid(udsServer, sharedKeyEvent);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: EventDispatchTest_AcquireEnableMark
 * @tc.desc: Test Acquire Enable Mark
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_AcquireEnableMark, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> event = PointerEvent::Create();
    ASSERT_NE(event, nullptr);
    event->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_FALSE(dispatch.AcquireEnableMark(event));
    event->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_TRUE(dispatch.AcquireEnableMark(event));
}

/**
 * @tc.name: EventDispatchTest_HandlePointerEventInner
 * @tc.desc: Test HandlePointerEventInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_HandlePointerEventInner, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    PointerEvent::PointerItem pointerItem;
    pointerEvent->SetPointerId(5);
    pointerItem.SetPointerId(5);
    pointerEvent->AddPointerItem(pointerItem);
    ASSERT_NO_FATAL_FAILURE(dispatch.HandlePointerEventInner(pointerEvent));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_001
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_001, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = -1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    dispatch.eventTime_ = 1000;
    pointerEvent->SetActionTime(5000);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_002
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_002, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = -1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    int32_t currentTime = dispatch.currentTime_;
    int32_t eventTime = dispatch.eventTime_;
    int32_t INTERVAL_TIME = 3000;
    currentTime = 6000;
    eventTime = 1000;
    EXPECT_TRUE(currentTime - eventTime > INTERVAL_TIME);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_003
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_003, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    int32_t fd = 1;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    int32_t pointerAction;
    pointerAction = PointerEvent::POINTER_ACTION_DOWN;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(point, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_004
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_004, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_005
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_005, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 1;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    pointerEvent->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_006
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_006, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 2;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    pointerEvent->pointerAction_ = PointerEvent::POINTER_ACTION_UP;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_007
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_007, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 2;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    pointerEvent->pointerAction_ = PointerEvent::POINTER_ACTION_PULL_DOWN;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_008
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_008, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 2;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    pointerEvent->pointerAction_ = PointerEvent::POINTER_ACTION_PULL_UP;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchPointerEventInner_009
 * @tc.desc: Test Dispatch Pointer Event Inner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchPointerEventInner_009, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    SessionPtr sess = nullptr;
    int32_t fd = 2;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    int32_t type = 0;
    int64_t time = 3000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    pointerEvent->pointerAction_ = PointerEvent::POINTER_ACTION_MOVE;
    ASSERT_NO_FATAL_FAILURE(dispatch.DispatchPointerEventInner(pointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_002
 * @tc.desc: Test Dispatch Key Event Pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_002, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> KeyEvent = KeyEvent::Create();
    ASSERT_NE(KeyEvent, nullptr);
    dispatch.eventTime_ = 1000;
    KeyEvent->SetActionTime(5000);
    ASSERT_EQ(dispatch.DispatchKeyEventPid(udsServer, KeyEvent), RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_003
 * @tc.desc: Test DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_003, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    int32_t currentTime = dispatch.currentTime_;
    int32_t eventTime = dispatch.eventTime_;
    int32_t INTERVAL_TIME = 3000;
    currentTime = 6000;
    eventTime = 1000;
    EXPECT_TRUE(currentTime - eventTime > INTERVAL_TIME);
    int32_t ret = dispatch.DispatchKeyEventPid(udsServer, keyEvent);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_004
 * @tc.desc: Test DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_004, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);

    NetPacket pkt(MmiMessageId::INVALID);
    EXPECT_FALSE(pkt.ChkRWError());
    int32_t ret = dispatch.DispatchKeyEventPid(udsServer, keyEvent);
    EXPECT_EQ(ret, RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_DispatchKeyEventPid_005
 * @tc.desc: Test DispatchKeyEventPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_DispatchKeyEventPid_005, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    UDSServer udsServer;
    SessionPtr sess = nullptr;
    std::shared_ptr<KeyEvent> KeyEvent = KeyEvent::Create();
    ASSERT_NE(KeyEvent, nullptr);
    dispatch.eventTime_ = 1000;
    KeyEvent->SetActionTime(2000);

    int32_t type = 0;
    int64_t time = 2000;
    sess = std::make_shared<UDSSession>(PROGRAM_NAME, g_moduleType, g_writeFd, UID_ROOT, g_pid);
    bool ret = ANRMgr->TriggerANR(type, time, sess);
    EXPECT_FALSE(ret);

    ASSERT_EQ(dispatch.DispatchKeyEventPid(udsServer, KeyEvent), RET_ERR);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_001
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_001, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_UP;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::nullopt;
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_002
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_002, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::nullopt;
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_003
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_003, TestSize.Level1)
{
    EventDispatchHandler handler;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_UP;
    int32_t windowId = 1;
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::optional<WindowInfo> windowInfo = std::nullopt;
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    handler.cancelEventList_[1].insert(windowInfo1);
    bool result = handler.ReissueEvent(point, windowId, windowInfo);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_004
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_004, TestSize.Level1)
{
    EventDispatchHandler handler;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    int32_t windowId = 1;
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::optional<WindowInfo> windowInfo = std::nullopt;
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    handler.cancelEventList_[1].insert(windowInfo1);
    bool result = handler.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_005
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_005, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_006
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_006, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_DOWN;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 5;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_007
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_007, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_UP;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_008
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_008, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_UP;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 5;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_009
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_009, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_CANCEL;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 5;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_010
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_010, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_CANCEL;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: EventDispatchTest_ReissueEvent_011
 * @tc.desc: Test ReissueEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_ReissueEvent_011, TestSize.Level1)
{
    EventDispatchHandler dispatch;
    std::shared_ptr<PointerEvent> point = PointerEvent::Create();
    ASSERT_NE(point, nullptr);
    point->pointerAction_ = PointerEvent::POINTER_ACTION_MOVE;
    int32_t windowId = 100;
    std::optional<WindowInfo> windowInfo = std::make_optional<WindowInfo>();
    ASSERT_NE(windowInfo, std::nullopt);
    point->pointerId_ = 1;
    point->SetPointerId(point->pointerId_);
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    dispatch.cancelEventList_[1].insert(windowInfo1);
    bool result = dispatch.ReissueEvent(point, windowId, windowInfo);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: EventDispatchTest_SearchCancelList_001
 * @tc.desc: Test SearchCancelList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_SearchCancelList_001, TestSize.Level1)
{
    EventDispatchHandler handler;
    int32_t pointerId = 1;
    int32_t windowId = 2;
    std::shared_ptr<WindowInfo> result = handler.SearchCancelList(pointerId, windowId);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: EventDispatchTest_SearchCancelList_002
 * @tc.desc: Test SearchCancelList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_SearchCancelList_002, TestSize.Level1)
{
    EventDispatchHandler handler;
    int32_t pointerId = 5;
    int32_t windowId = 2;
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    handler.cancelEventList_[1].insert(windowInfo1);
    std::shared_ptr<WindowInfo> result = handler.SearchCancelList(pointerId, windowId);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: EventDispatchTest_SearchCancelList_003
 * @tc.desc: Test SearchCancelList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_SearchCancelList_003, TestSize.Level1)
{
    EventDispatchHandler handler;
    int32_t pointerId = 1;
    int32_t windowId = 1;
    std::shared_ptr<WindowInfo> windowInfo1 = std::make_shared<WindowInfo>();
    windowInfo1->id = 1;
    handler.cancelEventList_[1].insert(windowInfo1);
    std::shared_ptr<WindowInfo> result = handler.SearchCancelList(pointerId, windowId);
    ASSERT_NE(result, nullptr);
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_004
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_004, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    int32_t fd = 1;
    int32_t eventType = 3;
    UDSServer* udsServer = new UDSServer();
    InputHandler->udsServer_ = udsServer;
    std::shared_ptr<PointerEvent> sharedPointerEvent = std::make_shared<PointerEvent>(eventType);
    EXPECT_NE(sharedPointerEvent, nullptr);
    std::vector<int32_t> pointerIdList;
    EXPECT_FALSE(pointerIdList.size() > 1);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(sharedPointerEvent, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_005
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_005, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    PointerEvent::PointerItem testPointerItem;
    UDSServer* udsServer = new UDSServer();
    InputHandler->udsServer_ = udsServer;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> event = std::make_shared<PointerEvent>(eventType);
    event->pointers_.push_back(PointerEvent::PointerItem());
    event->pointers_.push_back(PointerEvent::PointerItem());
    std::vector<int32_t> pointerIds = event->GetPointerIds();
    ASSERT_TRUE(pointerIds.size() > 1);
    event->AddPointerItem(testPointerItem);
    int32_t testPointerId = 1;
    testPointerItem.SetPointerId(testPointerId);
    event->AddPointerItem(testPointerItem);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(event, fd));
}

/**
 * @tc.name: EventDispatchTest_FilterInvalidPointerItem_006
 * @tc.desc: Test the function FilterInvalidPointerItem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDispatchTest, EventDispatchTest_FilterInvalidPointerItem_006, TestSize.Level1)
{
    EventDispatchHandler eventdispatchhandler;
    PointerEvent::PointerItem testPointerItem;
    UDSServer* udsServer = new UDSServer();
    InputHandler->udsServer_ = udsServer;
    int32_t fd = 1;
    int32_t eventType = 3;
    std::shared_ptr<PointerEvent> event = std::make_shared<PointerEvent>(eventType);
    event->pointers_.push_back(PointerEvent::PointerItem());
    event->pointers_.push_back(PointerEvent::PointerItem());
    std::vector<int32_t> pointerIds = event->GetPointerIds();
    ASSERT_TRUE(pointerIds.size() > 1);
    event->AddPointerItem(testPointerItem);
    int32_t testPointerId = 1;
    testPointerItem.SetPointerId(testPointerId + 1);
    event->AddPointerItem(testPointerItem);
    ASSERT_NO_FATAL_FAILURE(eventdispatchhandler.FilterInvalidPointerItem(event, fd));
}
} // namespace MMI
} // namespace OHOS
