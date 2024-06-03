/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "libinput.h"
#include "libinput_interface.h"
#include "define_multimodal.h"
#include "tablet_tool_tranform_processor.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "TabletToolTranformProcessorTest"

namespace OHOS {
namespace MMI {
using namespace testing;
using namespace testing::ext;

class LibinputInterfaceMock : public LibinputInterface {
public:
    LibinputInterfaceMock() = default;
    ~LibinputInterfaceMock() override = default;

    MOCK_METHOD1(GetEventType, enum libinput_event_type (struct libinput_event *event));
    MOCK_METHOD1(GetGestureEvent, struct libinput_event_gesture* (struct libinput_event *event));
    MOCK_METHOD1(GestureEventGetTime, uint32_t (struct libinput_event_gesture *event));
    MOCK_METHOD1(GestureEventGetFingerCount, int (struct libinput_event_gesture *event));
    MOCK_METHOD2(GestureEventGetDevCoordsX, int (struct libinput_event_gesture *, uint32_t));
    MOCK_METHOD2(GestureEventGetDevCoordsY, int (struct libinput_event_gesture *, uint32_t));
};

class TabletToolTranformProcessorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void TabletToolTranformProcessorTest::SetUpTestCase(void)
{
}

void TabletToolTranformProcessorTest::TearDownTestCase(void)
{
}

void TabletToolTranformProcessorTest::SetUp()
{
}

void TabletToolTranformProcessorTest::TearDown()
{
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnEvent_001
 * @tc.desc: Verify that TabletToolTranformProcessor can correctly handle events when receive
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnEvent_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 6;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event *event = nullptr;
    std::shared_ptr<PointerEvent> ret = processor.OnEvent(event);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnEvent_002
 * @tc.desc: Test the funcation OnEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnEvent_002, TestSize.Level1)
{
    int32_t deviceId = 2;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event_tablet_tool event {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetEventType).WillOnce(Return(LIBINPUT_EVENT_TABLET_TOOL_AXIS));

    auto pointerEvent = processor.OnEvent(&event.base);
    ASSERT_TRUE(pointerEvent == nullptr);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnEvent_003
 * @tc.desc: Test the funcation OnEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnEvent_003, TestSize.Level1)
{
    int32_t deviceId = 2;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event_tablet_tool event {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetEventType).WillOnce(Return(LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY));

    auto pointerEvent = processor.OnEvent(&event.base);
    ASSERT_TRUE(pointerEvent == nullptr);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnEvent_004
 * @tc.desc: Test the funcation OnEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnEvent_004, TestSize.Level1)
{
    int32_t deviceId = 2;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event_tablet_tool event {};

    NiceMock<LibinputInterfaceMock> libinputMock;
    EXPECT_CALL(libinputMock, GetEventType).WillOnce(Return(LIBINPUT_EVENT_TABLET_TOOL_TIP));

    auto pointerEvent = processor.OnEvent(&event.base);
    ASSERT_TRUE(pointerEvent == nullptr);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnTip_001
 * @tc.desc: Tablet tool transformation processor test, testing under the tip function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnTip_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 6;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event *event = nullptr;
    bool ret = processor.OnTip(event);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnTipDown_001
 * @tc.desc: Test the OnTipDown method in the TabletToolTranformProcessor class
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnTipDown_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 6;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event_tablet_tool *event = nullptr;
    bool ret = processor.OnTipDown(event);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnTipMotion_001
 * @tc.desc: Test the response of TabletToolTranformProcessor when the tip is moving
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnTipMotion_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 6;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event *event = nullptr;
    bool ret = processor.OnTipMotion(event);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: TabletToolTranformProcessorTest_OnTipUp_001
 * @tc.desc: Test case for the OnTipUp method of the TabletToolTranformProcessor class
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TabletToolTranformProcessorTest, TabletToolTranformProcessorTest_OnTipUp_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t deviceId = 6;
    TabletToolTransformProcessor processor(deviceId);
    libinput_event_tablet_tool *event = nullptr;
    bool ret = processor.OnTipUp(event);
    ASSERT_FALSE(ret);
}
}
}