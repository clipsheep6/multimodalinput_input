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

#include "stream_buffer.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class StreamBufferTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name:read_Type1_001
 * @tc.desc:Verify stream buffer read
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(StreamBufferTest, read_Type1_001, TestSize.Level1)
{
    char *buf = nullptr;
    size_t size = 4;

    StreamBuffer bufObj;
    bool retResult = bufObj.Read(buf, size);
    EXPECT_FALSE(retResult);
}

/**
 * @tc.name:read_Type2_002
 * @tc.desc:Verify stream buffer read
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(StreamBufferTest, read_Type2_002, TestSize.Level1)
{
    char *buf = nullptr;
    size_t size = 0;

    StreamBuffer bufObj;
    bool retResult = bufObj.Read(buf, size);
    EXPECT_FALSE(retResult);
}

/**
 * @tc.name:write_Type1_001
 * @tc.desc:Verify stream buffer write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(StreamBufferTest, write_Type1_001, TestSize.Level1)
{
    char *buf = nullptr;
    size_t size = 10;

    StreamBuffer bufObj;
    bool retResult = bufObj.Write(buf, size);
    EXPECT_FALSE(retResult);
}
}
}