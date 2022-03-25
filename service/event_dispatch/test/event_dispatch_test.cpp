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

class EventDispatchTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(){
        key = KeyEvent::Create();  
        // std::shared_ptr<MockInputWindowsManager> temp(&mockInputWindowsManager_);
        // InputWindowsManager::instance_ = temp;
        // MMI_LOGD("mockInputWindowsManager_::ptr = %{public}p", (void*)&mockInputWindowsManager_);
        // MMI_LOGD("InputWindowsManager::instance_ = %{public}p", (void*)InputWindowsManager::instance_ .get());
        // MMI_LOGD("WinMgr.get() = %{public}p", (void*)(WinMgr.get()));
    }

    // class MockInputWindowsManager : public InputWindowsManager{
    // public:
    //     MOCK_METHOD1(UpdateTarget, int32_t(std::shared_ptr<InputEvent>));
    // }mockInputWindowsManager_;

    class MockEventDispatch : public EventDispatch{
    protected:
        bool IsNeedFilterOut(const std::string& deviceId, const OHOS::DistributedHardware::DistributedInput::BusinessEvent& businessEvent) override{
            return isNeedFilterOut_;
        }
        OHOS::DistributedHardware::DistributedInput::DInputServerType IsStartDistributedInput() override{
            return type_;
        }
       
    public:
        bool isNeedFilterOut_;
        OHOS::DistributedHardware::DistributedInput::DInputServerType type_;
    }eventDispatch_;

public:
    UDSServer udsServer_;
    std::shared_ptr<KeyEvent> key;
};
} // namespace
