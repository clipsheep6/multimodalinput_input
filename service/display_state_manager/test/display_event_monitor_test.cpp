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

#include "libinput.h"
#include "define_multimodal.h"
#include "display_event_monitor.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class DisplayEventMonitorTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: DisplayEventMonitorTest
 * @tc.desc: Test DisplayEventMonitorTest_SCREEN_ON
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayEventMonitorTest, DisplayEventMonitorTest_SCREEN_ON, TestSize.Level1)
{
    OHOS::EventFwk::CommonEventSubscribeInfo subscribeInfo;
    DisplyChangedReceiver receiver(subscribeInfo);
    EventFwk::CommonEventData eventData;
    std::string action = eventData.GetWant().GetAction();
    action = EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON;
    receiver.OnReceiveEvent(eventData);
    EXPECT_TRUE(FINGERSENSE_WRAPPER->enableFingersense_ != nullptr);
}

/**
 * @tc.name: DisplayEventMonitorTest
 * @tc.desc: Test DisplayEventMonitorTest_SCREEN_OFF
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayEventMonitorTest, DisplayEventMonitorTest_SCREEN_OFF, TestSize.Level1)
{
    OHOS::EventFwk::CommonEventSubscribeInfo subscribeInfo;
    DisplyChangedReceiver receiver(subscribeInfo);
    EventFwk::CommonEventData eventData;
    std::string action = eventData.GetWant().GetAction();
    action = EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF;
    receiver.OnReceiveEvent(eventData);
    EXPECT_TRUE(FINGERSENSE_WRAPPER->disableFingerSense_ != nullptr);
}

/**
 * @tc.name: DisplayEventMonitorTest
 * @tc.desc: Test DisplayEventMonitorTest_SCREEN_UNKNOW
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayEventMonitorTest, DisplayEventMonitorTest_SCREEN_UNKNOW, TestSize.Level1)
{
    OHOS::EventFwk::CommonEventSubscribeInfo subscribeInfo;
    DisplyChangedReceiver receiver(subscribeInfo);
    EventFwk::CommonEventData eventData;
    std::string action = eventData.GetWant().GetAction();
    receiver.OnReceiveEvent(eventData);
    EXPECT_TRUE(FINGERSENSE_WRAPPER->disableFingerSense_ != nullptr);
}
}
}