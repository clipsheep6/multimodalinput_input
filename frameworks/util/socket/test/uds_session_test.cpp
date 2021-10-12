/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "proto.h"
#include "uds_session.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class UDSSessionTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    const int myfd_ = -1;
};

HWTEST_F(UDSSessionTest, construct, TestSize.Level1)
{
    printf("myfd_ = %d\n", myfd_);
    int32_t fd = 0;
    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;

    UDSSession udsSession(fd, addr);
    udsSession.Close();
}

HWTEST_F(UDSSessionTest, SendMsg_type1_001, TestSize.Level1)
{
    int32_t fd = 0;
    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    const char *buf = "1234";
    size_t size = 4;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_002, TestSize.Level1)
{
    int32_t fd = 0;
    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    const char *buf = nullptr;
    size_t size = 4;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_003, TestSize.Level1)
{
    int32_t fd = 333;
    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    const char *buf = nullptr;
    size_t size = 0;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_004, TestSize.Level1)
{
    int32_t fd = -10001;
    sockaddr_un addr = {};
    const char *buf = "this unit data";
    size_t size = 14;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type1_005, TestSize.Level1)
{
    int32_t fd = -10001;
    sockaddr_un addr = {};
    const char *buf = "this unit data";
    size_t size = -1001;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(buf, size);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_001, TestSize.Level1)
{
    int32_t fd = -1;
    sockaddr_un addr = {};
    NetPacket newPacket(ID_MSG_INVALID);
    addr.sun_family = AF_LOCAL;

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_002, TestSize.Level1)
{
    int32_t fd = 2002;
    sockaddr_un addr = {};
    NetPacket newPacket(ID_MSG_BEGIN);

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}

HWTEST_F(UDSSessionTest, SendMsg_type2_003, TestSize.Level1)
{
    int32_t fd = -65535;
    sockaddr_un addr = {};
    NetPacket newPacket(ID_MSG_BEGIN);

    UDSSession sesObj(fd, addr);
    bool retResult = sesObj.SendMsg(newPacket);
    EXPECT_FALSE(retResult);
}
} // namespace
