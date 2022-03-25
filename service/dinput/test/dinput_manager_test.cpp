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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "input_manager.h"
#define private public
#include "dinput_manager.h"
namespace {
using namespace testing::ext;
using namespace OHOS;
using namespace MMI;
using namespace OHOS::DistributedHardware::DistributedInput;
namespace {

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerTest" };
}

class MockDInputManager : public DInputManager {
public:
    int32_t PrepareRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalPrepareDInputCallback> callback){
        return StartThread(deviceId, callback);
    }
    int32_t UnPrepareRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalUnPrepareDInputCallback> callback){
        return StartThread(deviceId, callback);

    }
    int32_t StartRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalStartDInputCallback> callback){
        return StartThread(deviceId, callback);

    }
    int32_t StopRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalStopDInputCallback> callback){
        return StartThread(deviceId, callback);
    }

    template<typename T>
    int32_t StartThread(const std::string& deviceId, sptr<T> callback){
        MMI_LOGD("wuwu1");
        if (nullptr == callback)
        {
            return -1;
        }
        std::thread t(
            [callback, deviceId](){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                callback->OnResult(deviceId,0);
            });
        t.detach();
        return 1;
    }
};

class DinputManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp()
    {
        mockDInputManager_ = new MockDInputManager();
        std::shared_ptr<MockDInputManager> temp(mockDInputManager_);
        DInputManager::instance_ = temp;
        sessionPtr = std::make_shared<UDSSession>("programName", 1,-1,1,1);
    }
    void TearDown()
    {
        MMI_LOGD("wuwu5");
        DInputMgr->DestroyInstance();
        mockDInputManager_ = nullptr;
        sessionPtr.reset();
    }
public:
    MockDInputManager* mockDInputManager_;
    SessionPtr sessionPtr;
};

HWTEST_F(DinputManagerTest, InputManagerTest_PrepareRemoteInput_001, TestSize.Level1)
{
    // auto* instance = InputManager::GetInstance();
    EXPECT_EQ(1,mockDInputManager_->PrepareRemoteInput(100 ,"id", sessionPtr));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
HWTEST_F(DinputManagerTest, InputManagerTest_UnprepareRemoteInput_001, TestSize.Level1)
{
    // auto* instance = InputManager::GetInstance();
    EXPECT_EQ(1,mockDInputManager_->UnprepareRemoteInput(90 ,"id", sessionPtr));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
HWTEST_F(DinputManagerTest, InputManagerTest_StopRemoteInput_001, TestSize.Level1)
{
    // auto* instance = InputManager::GetInstance();
    EXPECT_EQ(1,mockDInputManager_->StopRemoteInput(80 ,"id", sessionPtr));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
HWTEST_F(DinputManagerTest, InputManagerTest_StartRemoteInput_001, TestSize.Level1)
{
    // auto* instance = InputManager::GetInstance();
    EXPECT_EQ(1,mockDInputManager_->StartRemoteInput(70 ,"id", sessionPtr));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
} // namespace
