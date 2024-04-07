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

#include <gtest/gtest.h>

#include "event_filter_death_recipient.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class EventFilterDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: EventFilterDeathRecipientTest_EventFilterDeathRecipient_001
 * @tc.desc: Verify the EventFilterDeathRecipient
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventFilterDeathRecipientTest, EventFilterDeathRecipientTest_EventFilterDeathRecipient_001, TestSize.Level1)
{
    using FunEventFilter = std::function<void(const wptr<IRemoteObject> &object)>;
    auto deathCallback = FunEventFilter;
    EventFilterDeathRecipient::EventFilterDeathRecipient(deathCallback);


}

} // namespace MMI
} // namespace OHOS