/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <memory>
#include <gtest/gtest.h>

#include "event_normalize_handler.h"
#include "event_filter_service.h"
#include "i_input_event_handler.h"
#include "mmi_log.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class EventFilterServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: EventFilterServiceTest_HandleKeyEvent_01
 * @tc.desc: Test HandleKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventFilterServiceTest, EventFilterServiceTest_HandleKeyEvent_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    const std::shared_ptr<IInputEventFilter> filter_;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    EventFilterService event;
    filter_ = nullptr;
    bool ret = event.HandleKeyEvent(keyEvent);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: EventFilterServiceTest_HandleKeyEvent_02
 * @tc.desc: Test HandleKeyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventFilterServiceTest, EventFilterServiceTest_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    const std::shared_ptr<IInputEventFilter> filter_;
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    EventFilterService event;
    ASSERT_NO_FATAL_FAILURE(filter_->OnInputEvent(event));
}

/**
 * @tc.name: EventFilterServiceTest_HandlePointerEvent_01
 * @tc.desc: Test HandlePointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventFilterServiceTest, EventFilterServiceTest_HandlePointerEvent_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    const std::shared_ptr<IInputEventFilter> filter_;
    std::shared_ptr<PointerEvent> pointerEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);
    EventFilterService event;
    filter_ = nullptr;
    bool ret = event.HandlePointerEvent(pointerEvent);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: EventFilterServiceTest_HandlePointerEvent_02
 * @tc.desc: Test HandlePointerEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventFilterServiceTest, EventFilterServiceTest_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    const std::shared_ptr<IInputEventFilter> filter_;
    std::shared_ptr<PointerEvent> keyEvent = PointerEvent::Create();
    EXPECT_NE(pointerEvent, nullptr);
    EventFilterService event;
    ASSERT_NO_FATAL_FAILURE(filter_->OnInputEvent(event));
}
} // namespace MMI
} // namespace OHOS