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

#include <cstdio>
#include <gtest/gtest.h>

#include "define_multimodal.h"
#include "input_device_manager.h"
#include "input_windows_manager_mock.h"
#include "libinput_mock.h"
#include "input_windows_manager.h"
#include "event_filter_handler.h"
#include "input_event_handler.h"
#include "i_pointer_drawing_manager.h"
#include "fingersense_wrapper.h"
#include "mmi_log.h"
#include "pointer_drawing_manager.h"
#include "proto.h"
#include "scene_board_judgement.h"
#include "struct_multimodal.h"
#include "uds_server.h"
#include "window_info.h"

namespace OHOS {
namespace MMI {
using namespace testing;
using namespace testing::ext;

class InputWindowsManagerTestWithMock : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void InputWindowsManagerTestWithMock::SetUpTestCase()
{}

void InputWindowsManagerTestWithMock::TearDownTestCase()
{}

void InputWindowsManagerTestWithMock::SetUp()
{}

void InputWindowsManagerTestWithMock::TearDown()
{}

/**
 * @tc.name: InputWindowsManagerTestWithMock_UpdateTarget_01
 * @tc.desc: Test UpdateTarget
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTestWithMock, InputWindowsManagerTestWithMock_UpdateTarget_01, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    InputWindowsManagerMock inputwindowsMock;
    EXPECT_CALL(inputwindowsMock, GetPidAndUpdateTarget).WillRepeatedly(Return(-1));

    int32_t ret = WIN_MGR->UpdateTarget(keyEvent);
    EXPECT_EQ(ret, INVALID_FD);

}

/**
 * @tc.name: InputWindowsManagerTestWithMock_UpdateTarget_02
 * @tc.desc: Test UpdateTarget
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTestWithMock, InputWindowsManagerTestWithMock_UpdateTarget_02, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    NiceMock<InputWindowsManagerMock> inputwindowsMock;
    EXPECT_CALL(inputwindowsMock, GetClientFd).WillRepeatedly(Return(-1));

    int32_t ret = WIN_MGR->UpdateTarget(keyEvent);
    EXPECT_EQ(ret, INVALID_FD);

}
} // namespace MMI
} // namespace OHOS