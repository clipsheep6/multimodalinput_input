/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <libinput.h>

#include "anr_manager.h"

#include "dfx_hisysevent.h"
#include "input_event_handler.h"
#include "input_windows_manager.h"
#include "mmi_log.h"
#include "napi_constants.h"
#include "proto.h"
#include "timer_manager.h"
#include "window_manager.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class AnrManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: AnrManagerTest_Init_001
 * @tc.desc: Init
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_Init_001, TestSize.Level1)
{
    UDSServer udsServer;
    ANRMgr->Init(udsServer);
}

/**
 * @tc.name: AnrManagerTest_MarkProcessed_001
 * @tc.desc: Mark processed
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_MarkProcessed_001, TestSize.Level1)
{
    int32_t pid = 123;
    int32_t eventType = 1;
    int32_t eventId = 456;
    ANRMgr->MarkProcessed(pid, eventType, eventId);
}

/**
 * @tc.name: AnrManagerTest_RemoveTimers_001
 * @tc.desc: Remove timers
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_RemoveTimers_001, TestSize.Level1)
{
    SessionPtr sess;
    ANRMgr->RemoveTimers(sess);
}

/**
 * @tc.name: AnrManagerTest_RemoveTimersByType_001
 * @tc.desc: Remove timers by type
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_RemoveTimersByType_001, TestSize.Level1)
{
    SessionPtr session;
    int32_t dispatchType = -1;
    ANRMgr->RemoveTimersByType(session, dispatchType);
}

/**
 * @tc.name: AnrManagerTest_RemoveTimersByType_002
 * @tc.desc: Remove timers by type
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_RemoveTimersByType_002, TestSize.Level1)
{
    SessionPtr session;
    int32_t monitorType = 0;
    ANRMgr->RemoveTimersByType(session, monitorType);
}

/**
 * @tc.name: AnrManagerTest_RemoveTimersByType_003
 * @tc.desc: Remove timers by type
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_RemoveTimersByType_003, TestSize.Level1)
{
    SessionPtr session;
    int32_t illegalType = 123;
    ANRMgr->RemoveTimersByType(session, illegalType);
}

/**
 * @tc.name: AnrManagerTest_SetANRNoticedPid_001
 * @tc.desc: Set ANR noticed pid
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_SetANRNoticedPid_003, TestSize.Level1)
{
    int32_t pid = 1234;
    int32_t ret = ANRMgr->SetANRNoticedPid(pid);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: AnrManagerTest_SetANRNoticedPid_002
 * @tc.desc: Set noticed pid
 * @tc.type: FUNC
 * @tc.require:SR000HQ0RR
 */
HWTEST_F(AnrManagerTest, AnrManagerTest_SetANRNoticedPid_002, TestSize.Level1)
{
    int32_t pid = -1;
    int32_t ret = ANRMgr->SetANRNoticedPid(pid);
    ASSERT_EQ(ret, RET_OK);
}
} // namespace MMI
} // namespace OHOS