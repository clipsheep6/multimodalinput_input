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

#include "event_dispatch.h"
#include <gtest/gtest.h>
#include "libmmi_util.h"
#include "run_shell_util.h"
#include "define_multimodal.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
namespace {
   constexpr int32_t SLEEP = 6000;
   constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
   static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventDispatchTest" };
}

class EventDispatchTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

HWTEST_F(EventDispatchTest, EventDispatch_SimulateInputEvent_011, TestSize.Level1)
{
    MMI_LOGT("EventDispatchTest SimulateInputEvent begin");
    RunShellUtil runCommand;
    std::string command { "ServerMsgHandler: in OnInjectPointerEvent, #[[:digit:]]\\{1,\\}, "
        "Unknown source type!" };

    std::vector<std::string> sLogs;
    ASSERT_TRUE(runCommand.RunShellCommand(command, sLogs) == RET_OK);

    auto pointerEvent = PointerEvent::Create();
    int64_t downTime = static_cast<int64_t>(GetNanoTime()/NANOSECOND_TO_MILLISECOND);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerId(1);
    pointerEvent->SetAxis(0);
    pointerEvent->SetAxisValue(0);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    PointerEvent::PointerItem item;
    item.SetDownTime(downTime);
    item.SetPressed(true);

    item.SetGlobalX(200);
    item.SetGlobalY(200);
    item.SetLocalX(300);
    item.SetLocalY(300);

    item.SetWidth(0);
    item.SetHeight(0);
    item.SetPressure(0);
    item.SetDeviceId(0);
    pointerEvent->AddPointerItem(item);
    for (int32_t i = 0; i < 10; i++) {
        InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    }
    MMI_LOGT("EventdispatchTest starts waiting for 6 seconds.");
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP));
    for (int32_t i = 0; i < 10; i++) {
        InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    }

    std::vector<std::string> tLogs;
    ASSERT_TRUE(runCommand.RunShellCommand(command, tLogs) == RET_OK);
    MMI_LOGT("EventDispatchTest SimulateInputEvent end");
}
} // namespace
