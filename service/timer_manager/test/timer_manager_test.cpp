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

#include <fstream>

#include <gtest/gtest.h>

#include "mmi_log.h"
#include "uds_server.h"
#include "timer_manager.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class TimerManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

void Lambda()
{
    return;
}

/**
 * @tc.name: TimerManagerTest_ManagerTimer_001
 * @tc.desc: Test The Function AddTimer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TimerManagerTest, TimerManagerTest_ManagerTimer_001, TestSize.Level1)
{
    int32_t repeatCount = 3;
    int32_t intervalMs  = 1000;
    int32_t timerld = TimerMgr->AddTimer(intervalMs, repeatCount, Lambda);
    EXPECT_EQ(timerld, 0);
}

/**
 * @tc.name: TimerManagerTest_ManagerTimer_002
 * @tc.desc: Test The Function RemoveTimer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TimerManagerTest, TimerManagerTest_ManagerTimer_002, TestSize.Level1)
{
    int32_t repeatCount = 3;
    int32_t intervalMs  = 1000;
    int32_t timerld = TimerMgr->AddTimer(intervalMs, repeatCount, Lambda);
    ASSERT_EQ(TimerMgr->RemoveTimer(timerld), 0);
}

/**
 * @tc.name: TimerManagerTest_ManagerTimer_003
 * @tc.desc: Test The Function ResetTimer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TimerManagerTest, TimerManagerTest_ManagerTimer_003, TestSize.Level1)
{
    int32_t repeatCount = 3;
    int32_t intervalMs = 1000;
    int32_t timerld = TimerMgr->AddTimer(intervalMs, repeatCount, Lambda);
    int32_t result = TimerMgr->ResetTimer(timerld);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: TimerManagerTest_ManagerTimer_004
 * @tc.desc: Test The Function IsExist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TimerManagerTest, TimerManagerTest_ManagerTimer_004, TestSize.Level1)
{
    int32_t repeatCount = 3;
    int32_t intervalMs = 1000;
    int32_t timerld = TimerMgr->AddTimer(intervalMs, repeatCount, Lambda);
    ASSERT_TRUE(TimerMgr->IsExist(timerld));
}
} // namespace MMI
} // namespace OHOS
