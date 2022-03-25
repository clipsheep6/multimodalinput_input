/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "event_dispatch.h"
#include <gtest/gtest.h>
#include "libmmi_util.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class MockEventDispatch : public EventDispatch
{
public:
    bool IsNeedFilterOut(const std::string& deviceId, 
                            const OHOS::DistributedHardware::DistributedInput::BusinessEvent& businessEvent) override
    {
        return isNeedFilterOut_;
    }
    OHOS::DistributedHardware::DistributedInput::DInputServerType IsStartDistributedInput() override
    {
        return type_;
    }
    
public:
    bool isNeedFilterOut_;
    OHOS::DistributedHardware::DistributedInput::DInputServerType type_;
}eventDispatch_;

class EventDispatchTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp()
    {
        key = KeyEvent::Create();
    }
public:
    UDSServer udsServer_;
    std::shared_ptr<KeyEvent> key;
};
HWTEST_F(EventDispatchTest, Test_DispatchKeyEventPid_1, TestSize.Level1)
{

    eventDispatch_.type_ = OHOS::DistributedHardware::DistributedInput::DInputServerType::NULL_SERVER_TYPE;
    int32_t actual = eventDispatch_.DispatchKeyEventPid(udsServer_, key, 0);
    ASSERT_EQ(RET_ERR, actual);

}

HWTEST_F(EventDispatchTest, Test_DispatchKeyEventPid_2, TestSize.Level1)
{
    eventDispatch_.type_ = OHOS::DistributedHardware::DistributedInput::DInputServerType::SOURCE_SERVER_TYPE;
    int32_t actual = eventDispatch_.DispatchKeyEventPid(udsServer_, key, 0);
    ASSERT_EQ(RET_ERR, actual);
}

HWTEST_F(EventDispatchTest, Test_DispatchKeyEventPid_3, TestSize.Level1)
{
    eventDispatch_.type_ = OHOS::DistributedHardware::DistributedInput::DInputServerType::SINK_SERVER_TYPE;
    eventDispatch_.isNeedFilterOut_ = false;

    int32_t actual = eventDispatch_.DispatchKeyEventPid(udsServer_, key, 0);
    ASSERT_EQ(RET_OK, actual);
}

HWTEST_F(EventDispatchTest, Test_DispatchKeyEventPid_4, TestSize.Level1)
{
    eventDispatch_.type_ = OHOS::DistributedHardware::DistributedInput::DInputServerType::SINK_SERVER_TYPE;
    eventDispatch_.isNeedFilterOut_ = true;

    int32_t actual = eventDispatch_.DispatchKeyEventPid(udsServer_, key, 0);
    ASSERT_EQ(RET_ERR, actual);
}
} // namespace
