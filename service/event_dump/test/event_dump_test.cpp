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

#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>

#include "event_dump.h"
#include "event_log_helper.h"
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "input_manager.h"
#include "input_manager_util.h"
#include "input_windows_manager.h"
#include "mouse_event_normalize.h"
#include "multimodal_event_handler.h"
#include "system_info.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventDumpTest" };
using namespace testing::ext;
const char *testFileName = "/data/log.log";
} // namespace

class EventDumpTest : public testing::Test {
public:
void SetUp() override
{
    fd_ = open(testFileName, O_WRONLY);
}

void TearDown() override
{
    close(fd_);
}

int32_t fd_;
};

/**
 * @tc.name: EventDumpTest_CheckCount_001
 * @tc.desc: Event dump CheckCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest001, TestSize.Level1)
{
    std::vector<std::string> args;
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    EXPECT_EQ(count, 0);
}

/**
 * @tc.name: EventDumpTest_CheckCount_002
 * @tc.desc: Event dump CheckCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest002, TestSize.Level1)
{
    std::vector<std::string> args = {"--help"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    EXPECT_EQ(count, 1);
}

/**
 * @tc.name: EventDumpTest_CheckCount_003
 * @tc.desc: Event dump CheckCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest003, TestSize.Level1)
{
    std::vector<std::string> args = {"-h"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    EXPECT_EQ(count, 1);
}

/**
 * @tc.name: EventDumpTest_CheckCount_004
 * @tc.desc: Event dump CheckCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest004, TestSize.Level1) {
    std::vector<std::string> args = {"-abc"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    EXPECT_EQ(count, 3);
}

/**
 * @tc.name: EventDumpTest_CheckCount_005
 * @tc.desc: Event dump CheckCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest005, TestSize.Level1) {

    std::vector<std::string> args = {"-a", "--help", "foo", "-bc", "bar"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    MMIEventDump->DumpEventHelp(fd_, args);
    EXPECT_EQ(count, 4);
}

/**
 * @tc.name: EventDumpTest_006
 * @tc.desc: Event dump InputDeviceManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest006, TestSize.Level1)
{
    std::vector<std::string> args = {"-d"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(InputDevMgr->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_007
 * @tc.desc: Event dump DeviceList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest007, TestSize.Level1)
{
    std::vector<std::string> args = {"-l"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(InputDevMgr->DumpDeviceList(fd_, args));
}

/**
 * @tc.name: EventDumpTest_008
 * @tc.desc: Event dump WindowsManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest008, TestSize.Level1)
{
    std::vector<std::string> args = {"-w"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(WinMgr->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_009
 * @tc.desc: Event dump UDSServer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest009, TestSize.Level1)
{
    std::vector<std::string> args = {"-u"};
    size_t count = 0;
    auto udsServer = InputHandler->GetUDSServer();
    CHKPV(udsServer);
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(udsServer->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_010
 * @tc.desc: Event dump SubscriberHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest010, TestSize.Level1)
{
    std::vector<std::string> args = {"-s"};
    size_t count = 0;
    auto subscriberHandler = InputHandler->GetSubscriberHandler();
    CHKPV(subscriberHandler);
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(subscriberHandler->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_011
 * @tc.desc: Event dump MonitorHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest011, TestSize.Level1)
{
    std::vector<std::string> args = {"-o"};
    size_t count = 0;
    auto monitorHandler = InputHandler->GetMonitorHandler();
    CHKPV(monitorHandler);
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(monitorHandler->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_012
 * @tc.desc: Event dump InterceptorHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest012, TestSize.Level1)
{
    std::vector<std::string> args = {"-i"};
    size_t count = 0;
    auto interceptorHandler = InputHandler->GetInterceptorHandler();
    CHKPV(interceptorHandler);
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(interceptorHandler->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_013
 * @tc.desc: Event dump FilterHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest013, TestSize.Level1)
{
    std::vector<std::string> args = {"-f"};
    size_t count = 0;
    auto filterHandler = InputHandler->GetFilterHandler();
    CHKPV(filterHandler);
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(filterHandler->Dump(fd_, args));
}

/**
 * @tc.name: EventDumpTest_014
 * @tc.desc: Event dump MouseEventHandler
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EventDumpTest, EventDumpTest014, TestSize.Level1)
{
    std::vector<std::string> args = {"-m"};
    size_t count = 0;
    MMIEventDump->CheckCount(fd_, args, count);
    MMIEventDump->ParseCommand(fd_, args);
    ASSERT_NO_FATAL_FAILURE(MouseEventHdr->Dump(fd_, args));
}
} // namespace MMI
} // namespace OHOS