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

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

#include "input_windows_manager.h"
#include "i_pointer_drawing_manager.h"
#include "mmi_log.h"
#include "proto.h"
#include "scene_board_judgement.h"
#include "struct_multimodal.h"
#include "uds_server.h"
#include "window_info.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class InputWindowsManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void)
    {
        // 创建displayGroupInfo_
        DisplayGroupInfo displayGroupInfo;
        displayGroupInfo.width = 20;
        displayGroupInfo.height = 20;
        displayGroupInfo.focusWindowId = 1;
        uint32_t num = 1;
        for (uint32_t i = 0; i < num; i++) {
            WindowInfo info;
            info.id = 1;
            info.pid = 1;
            info.uid = 1;
            info.area = {1, 1, 1, 1};
            info.defaultHotAreas = { info.area };
            info.pointerHotAreas = { info.area };
            info.agentWindowId = 1;
            info.flags = 1;
            info.transform = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
            info.pointerChangeAreas = { 1, 2, 1, 2, 1, 2, 1, 2, 1 };
            displayGroupInfo.windowsInfo.push_back(info);
        }
        for (uint32_t i = 0; i < num; i++) {
            DisplayInfo info;
            info.id = 1;
            info.x =1;
            info.y = 1;
            info.width = 2;
            info.height = 2;
            info.dpi = 240;
            info.name = "pp";
            info.uniq = "pp";
            info.direction = DIRECTION0;
            displayGroupInfo.displaysInfo.push_back(info);
        }
        WinMgr->UpdateDisplayInfo(displayGroupInfo);
        preHoverScrollState_ = WinMgr->GetHoverScrollState();
    } // void SetUp(void)

    void TearDown(void)
    {
        WinMgr->SetHoverScrollState(preHoverScrollState_);
    }

private:
    bool preHoverScrollState_ { true };
};


/**
 * @tc.name: InputWindowsManagerTest_GetClientFd_001
 * @tc.desc: Test GetClientFd
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetClientFd_001, TestSize.Level1)
{
    auto pointerEvent = PointerEvent::Create();
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    WinMgr->GetDisplayGroupInfo();
    int32_t idNames = -1;
    ASSERT_EQ(WinMgr->GetClientFd(pointerEvent), idNames);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateTarget_003
 * @tc.desc: Test UpdateTarget
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateTarget_003, TestSize.Level1)
{
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    auto keyEvent = KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    keyEvent->SetDeviceId(1);
    keyEvent->SetTargetWindowId(1);
    keyEvent->SetAgentWindowId(1);
    ASSERT_EQ(WinMgr->UpdateTarget(keyEvent), -1);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateWindow_002
 * @tc.desc: Test UpdateWindow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateWindow_002, TestSize.Level1)
{
    WindowInfo window;
    window.id = 11;
    window.pid = 1221;
    window.uid = 1;
    window.area = {1, 1, 1, 1};
    window.defaultHotAreas = { window.area };
    window.pointerHotAreas = { window.area };
    window.pointerChangeAreas = {1, 2, 1, 2};
    window.displayId = 0;
    window.agentWindowId = 1;
    window.flags = 1;
    window.action = WINDOW_UPDATE_ACTION::UNKNOWN;
    WinMgr->UpdateWindowInfo({0, 11, {window}});
    ASSERT_EQ(WinMgr->GetWindowPid(11), -1);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateTargetPointer_005
 * @tc.desc: Test UpdateTargetPointer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateTargetPointer_005, TestSize.Level1)
{
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    auto pointerEvent = PointerEvent::Create();
    ASSERT_EQ(WinMgr->UpdateTargetPointer(pointerEvent), -1);
}

/**
 * @tc.name: InputWindowsManagerTest_IsNeedRefreshLayer_006
 * @tc.desc: Test IsNeedRefreshLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_IsNeedRefreshLayer_006, TestSize.Level1)
{
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(-1), true);
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(0), true);
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(1), true);
    } else {
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(-1), false);
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(0), false);
        ASSERT_EQ(WinMgr->IsNeedRefreshLayer(1), false);
    }
}

/**
 * @tc.name: InputWindowsManagerTest_SetMouseCaptureMode_008
 * @tc.desc: Test SetMouseCaptureMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_SetMouseCaptureMode_008, TestSize.Level1)
{
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    bool isCaptureMode = false;
    ASSERT_EQ(WinMgr->SetMouseCaptureMode(-1, isCaptureMode), -1);
    ASSERT_EQ(WinMgr->SetMouseCaptureMode(1, isCaptureMode), 0);
    isCaptureMode = true;
    ASSERT_EQ(WinMgr->SetMouseCaptureMode(1, isCaptureMode), 0);
}

/**
 * @tc.name: InputWindowsManagerTest_SetDisplayBind_009
 * @tc.desc: Test SetDisplayBind
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_SetDisplayBind_009, TestSize.Level1)
{
    UDSServer udsServer;
    WinMgr->Init(udsServer);
    std::string sysUid = "james";
    std::string devStatus = "add";
    WinMgr->DeviceStatusChanged(2, sysUid, devStatus);
    devStatus = "remove";
    WinMgr->DeviceStatusChanged(2, sysUid, devStatus);
    std::string msg = "There is in InputWindowsManagerTest_GetDisplayIdNames_009";
    ASSERT_EQ(WinMgr->SetDisplayBind(-1, 1, msg), -1);
}

/**
 * @tc.name: InputWindowsManagerTest_SetHoverScrollState_010
 * @tc.desc: Test SetHoverScrollState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_SetHoverScrollState_010, TestSize.Level1)
{
    ASSERT_TRUE(WinMgr->SetHoverScrollState(false) == RET_OK);
    WinMgr->SetHoverScrollState(true);
}

/**
 * @tc.name: InputWindowsManagerTest_GetHoverScrollState_011
 * @tc.desc: Test GetHoverScrollState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetHoverScrollState_011, TestSize.Level1)
{
    WinMgr->SetHoverScrollState(true);
    ASSERT_TRUE(WinMgr->GetHoverScrollState());
}

/**
 * @tc.name: InputWindowsManagerTest_InitMouseDownInfo_001
 * @tc.desc: Test initializing mouse down information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_InitMouseDownInfo_001, TestSize.Level1)
{
    WinMgr->InitMouseDownInfo();
    EXPECT_EQ(WinMgr->mouseDownInfo_.id, -1);
    EXPECT_EQ(WinMgr->mouseDownInfo_.pid, -1);
    EXPECT_TRUE(WinMgr->mouseDownInfo_.defaultHotAreas.empty());
    EXPECT_TRUE(WinMgr->mouseDownInfo_.pointerHotAreas.empty());
}

/**
 * @tc.name: InputWindowsManagerTest_InitMouseDownInfo_002
 * @tc.desc: Test initializing mouse down information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_InitMouseDownInfo_002, TestSize.Level1)
{
    WinMgr->mouseDownInfo_.id = 1;
    WinMgr->mouseDownInfo_.pid = 123;
    WinMgr->mouseDownInfo_.defaultHotAreas.push_back({0, 0, 100, 100});
    WinMgr->InitMouseDownInfo();
    EXPECT_EQ(WinMgr->mouseDownInfo_.id, -1);
    EXPECT_EQ(WinMgr->mouseDownInfo_.pid, -1);
    EXPECT_TRUE(WinMgr->mouseDownInfo_.defaultHotAreas.empty());
    EXPECT_TRUE(WinMgr->mouseDownInfo_.pointerHotAreas.empty());
}

/**
 * @tc.name: InputWindowsManagerTest_GetWindowGroupInfoByDisplayId_001
 * @tc.desc: Test getting window group information by display ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetWindowGroupInfoByDisplayId_001, TestSize.Level1)
{
    int32_t displayId = -1;
    const std::vector<WindowInfo>& windowGroupInfo = WinMgr->GetWindowGroupInfoByDisplayId(displayId);
    EXPECT_EQ(windowGroupInfo.size(), 1);
}

/**
 * @tc.name: InputWindowsManagerTest_GetWindowGroupInfoByDisplayId_002
 * @tc.desc: Test getting window group information by display ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetWindowGroupInfoByDisplayId_002, TestSize.Level1)
{
    int32_t displayId = 1;
    const std::vector<WindowInfo>& windowGroupInfo = WinMgr->GetWindowGroupInfoByDisplayId(displayId);
    EXPECT_FALSE(windowGroupInfo.empty());
}

/**
 * @tc.name: InputWindowsManagerTest_GetDisplayId_001
 * @tc.desc: Test getting the display ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetDisplayId_001, TestSize.Level1)
{
    int32_t expectedDisplayId = 1;
    std::shared_ptr<InputEvent> inputEvent = InputEvent::Create();
    EXPECT_NE(inputEvent, nullptr);
    inputEvent->SetTargetDisplayId(expectedDisplayId);
    int32_t ret = WinMgr->GetDisplayId(inputEvent);
    EXPECT_EQ(ret, expectedDisplayId);
}

/**
 * @tc.name: InputWindowsManagerTest_GetPidAndUpdateTarget_001
 * @tc.desc: Test getting PID and updating the target
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetPidAndUpdateTarget_001, TestSize.Level1)
{
    std::shared_ptr<KeyEvent> keyEvent = KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    int32_t targetDisplayId = 0;
    keyEvent->SetTargetDisplayId(targetDisplayId);
    int32_t ret = WinMgr->GetPidAndUpdateTarget(keyEvent);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name: InputWindowsManagerTest_GetWindowPid_001
 * @tc.desc: Test getting the process ID of a window
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetWindowPid_001, TestSize.Level1)
{
    int32_t windowId = 100;
    std::vector<WindowInfo> windowsInfo;
    int32_t ret = WinMgr->GetWindowPid(windowId,  windowsInfo);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: InputWindowsManagerTest_CheckFocusWindowChange_001
 * @tc.desc: Test checking focus window changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_CheckFocusWindowChange_001, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 123;
    ASSERT_NO_FATAL_FAILURE(WinMgr->CheckFocusWindowChange(displayGroupInfo));
}

/**
 * @tc.name: InputWindowsManagerTest_CheckFocusWindowChange_002
 * @tc.desc: Test checking focus window changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_CheckFocusWindowChange_002, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    DisplayGroupInfo displayGroupInfo_;
    displayGroupInfo.focusWindowId = 123;
    displayGroupInfo_.focusWindowId = 456;
    ASSERT_NO_FATAL_FAILURE(WinMgr->CheckFocusWindowChange(displayGroupInfo));
    ASSERT_NO_FATAL_FAILURE(WinMgr->CheckFocusWindowChange(displayGroupInfo_));
}

/**
 * @tc.name: InputWindowsManagerTest_CheckZorderWindowChange_001
 * @tc.desc: Test checking Z-order window changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_CheckZorderWindowChange_001, TestSize.Level1)
{
    std::vector<WindowInfo> oldWindowsInfo = {{1}};
    std::vector<WindowInfo> newWindowsInfo = {{2}};
    ASSERT_NO_FATAL_FAILURE(WinMgr->CheckZorderWindowChange(oldWindowsInfo, newWindowsInfo));
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateDisplayIdAndName_001
 * @tc.desc: Test updating display ID and name
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateDisplayIdAndName_001, TestSize.Level1)
{
    ASSERT_NO_FATAL_FAILURE(WinMgr->UpdateDisplayIdAndName());
    assert(WinMgr->GetDisplayIdNames().size() == 2);
    assert(WinMgr->IsDisplayAdd(1, "A"));
    assert(WinMgr->IsDisplayAdd(2, "B"));
    ASSERT_NO_FATAL_FAILURE(WinMgr->UpdateDisplayIdAndName());
    assert(WinMgr->GetDisplayIdNames().size() == 2);
    assert(WinMgr->IsDisplayAdd(1, "A"));
    assert(WinMgr->IsDisplayAdd(3, "C"));
    assert(!WinMgr->IsDisplayAdd(2, "B"));
}

/**
 * @tc.name: InputWindowsManagerTest_GetDisplayBindInfo_001
 * @tc.desc: Test getting display binding information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetDisplayBindInfo_001, TestSize.Level1)
{
    int32_t deviceId = 1;
    int32_t displayId = 2;
    DisplayBindInfos infos;
    std::string msg;
    int32_t ret = WinMgr->SetDisplayBind(deviceId, displayId, msg);
    EXPECT_EQ(ret, -1);
    ret = WinMgr->GetDisplayBindInfo(infos);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateCaptureMode_001
 * @tc.desc: Test updating capture mode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateCaptureMode_001, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 123;
    WinMgr->UpdateCaptureMode(displayGroupInfo);
    EXPECT_FALSE(WinMgr->captureModeInfo_.isCaptureMode);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateDisplayInfoByIncrementalInfo_001
 * @tc.desc: Test updating display information by incremental info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateDisplayInfoByIncrementalInfo_001, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 1;
    WindowInfo window;
    WinMgr->UpdateDisplayInfoByIncrementalInfo(window, displayGroupInfo);
    EXPECT_EQ(displayGroupInfo.windowsInfo.size(), 0);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateWindowsInfoPerDisplay_001
 * @tc.desc: Test updating window information for each display
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateWindowsInfoPerDisplay_001, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    displayGroupInfo.focusWindowId = 2;
    WinMgr->UpdateWindowsInfoPerDisplay(displayGroupInfo);
    WindowInfo window1{1};
    WindowInfo window2{2};
    displayGroupInfo.windowsInfo.push_back(window1);
    displayGroupInfo.windowsInfo.push_back(window2);
    WinMgr->UpdateDisplayInfo(displayGroupInfo);
    ASSERT_EQ(displayGroupInfo.windowsInfo.size(), 2);
    ASSERT_EQ(displayGroupInfo.windowsInfo[0].zOrder, 0);
    ASSERT_EQ(displayGroupInfo.windowsInfo[1].zOrder, 0);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateDisplayInfo_001
 * @tc.desc: Test updating display information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateDisplayInfo_001, TestSize.Level1)
{
    DisplayGroupInfo displayGroupInfo;
    WindowInfo windowInfo1;
    windowInfo1.zOrder = 1;
    windowInfo1.action = WINDOW_UPDATE_ACTION::ADD_END;
    WindowInfo windowInfo2;
    windowInfo2.zOrder = 2;
    windowInfo2.action = WINDOW_UPDATE_ACTION::ADD_END;
    displayGroupInfo.windowsInfo.push_back(windowInfo1);
    displayGroupInfo.windowsInfo.push_back(windowInfo2);
    ASSERT_NO_FATAL_FAILURE(WinMgr->UpdateDisplayInfo(displayGroupInfo));
}

/**
 * @tc.name: InputWindowsManagerTest_NeedUpdatePointDrawFlag_001
 * @tc.desc: Test whether the point draw flag needs to be updated
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_NeedUpdatePointDrawFlag_001, TestSize.Level1)
{
    std::vector<WindowInfo> windows1;
    EXPECT_FALSE(WinMgr->NeedUpdatePointDrawFlag(windows1));
    std::vector<WindowInfo> windows2;
    windows2.push_back(WindowInfo());
    windows2.back().action = OHOS::MMI::WINDOW_UPDATE_ACTION::ADD;
    EXPECT_FALSE(WinMgr->NeedUpdatePointDrawFlag(windows2));
    std::vector<WindowInfo> windows3;
    windows3.push_back(WindowInfo());
    windows3.back().action = OHOS::MMI::WINDOW_UPDATE_ACTION::ADD_END;
    EXPECT_TRUE(WinMgr->NeedUpdatePointDrawFlag(windows3));
}

/**
 * @tc.name: InputWindowsManagerTest_GetPointerStyleByArea_001
 * @tc.desc: Test getting pointer style by area
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetPointerStyleByArea_001, TestSize.Level1)
{
    WindowArea area;
    int32_t pid = 123;
    int32_t winId = 678;
    PointerStyle pointerStyle;
    pointerStyle.size = 1;
    pointerStyle.color = 2;
    pointerStyle.id = 3;
    area = WindowArea::ENTER;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::SOUTH);
    area = WindowArea::EXIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::SOUTH);
    area = WindowArea::FOCUS_ON_TOP_LEFT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST_SOUTH_EAST);
    area = WindowArea::FOCUS_ON_BOTTOM_RIGHT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST_SOUTH_EAST);
    area = WindowArea::FOCUS_ON_TOP_RIGHT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_EAST_SOUTH_WEST);
    area = WindowArea::FOCUS_ON_BOTTOM_LEFT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_EAST_SOUTH_WEST);
    area = WindowArea::FOCUS_ON_TOP;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_SOUTH);
    area = WindowArea::FOCUS_ON_BOTTOM;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_SOUTH);
    area = WindowArea::FOCUS_ON_LEFT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::WEST_EAST);
}

/**
 * @tc.name: InputWindowsManagerTest_GetPointerStyleByArea_002
 * @tc.desc: Test getting pointer style by area
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetPointerStyleByArea_002, TestSize.Level1)
{
    WindowArea area;
    int32_t pid = 123;
    int32_t winId = 678;
    PointerStyle pointerStyle;
    pointerStyle.size = 1;
    pointerStyle.color = 2;
    pointerStyle.id = 3;
    area = WindowArea::FOCUS_ON_RIGHT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::WEST_EAST);
    area = WindowArea::TOP_LEFT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::SOUTH_EAST);
    area = WindowArea::TOP_RIGHT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::SOUTH_WEST);
    area = WindowArea::TOP_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::SOUTH);
    area = WindowArea::LEFT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::EAST);
    area = WindowArea::RIGHT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::WEST);
    area = WindowArea::BOTTOM_LEFT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST);
    area = WindowArea::BOTTOM_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST);
    area = WindowArea::BOTTOM_RIGHT_LIMIT;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST);
    area = WindowArea::FOCUS_ON_INNER;
    WinMgr->GetPointerStyleByArea(area, pid, winId, pointerStyle);
    EXPECT_EQ(pointerStyle.id, MOUSE_ICON::NORTH_WEST);
}

/**
 * @tc.name: InputWindowsManagerTest_SetWindowPointerStyle_001
 * @tc.desc: Test setting window pointer style
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_SetWindowPointerStyle_001, TestSize.Level1)
{
    WindowArea area;
    int32_t pid = 1;
    int32_t windowId = 2;
    IconStyle defaultIconStyle;
    area = WindowArea::ENTER;
    defaultIconStyle.iconPath = "default_icon_path";
    IPointerDrawingManager::GetInstance()->GetMouseIconPath()[MOUSE_ICON::DEFAULT] = defaultIconStyle;
    WinMgr->SetWindowPointerStyle(area, pid, windowId);
    assert(lastPointerStyle_.id == pointerStyle.id);
    assert(windowId != GLOBAL_WINDOW_ID && (pointerStyle.id == MOUSE_ICON::DEFAULT &&
        mouseIcons[MOUSE_ICON(pointerStyle.id)].iconPath != defaultIconPath));
    assert(WinMgr->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style) == RET_OK);
    assert(lastPointerStyle_.id == style.id);
}

/**
 * @tc.name: InputWindowsManagerTest_UpdateWindowPointerVisible_001
 * @tc.desc: Test updating window pointer visibility
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateWindowPointerVisible_001, TestSize.Level1)
{    
    int32_t pid = 123;
    bool visible = true;
    IPointerDrawingManager::GetInstance()->GetPointerVisible(pid);
    IPointerDrawingManager::GetInstance()->SetPointerVisible(pid, visible);
    ASSERT_NO_FATAL_FAILURE(WinMgr->UpdateWindowPointerVisible(pid));
}

/**
 * @tc.name: InputWindowsManagerTest_DispatchPointer_001
 * @tc.desc: Test dispatching pointer events
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_DispatchPointer_001, TestSize.Level1)
{    
    int32_t pointerAction = PointerEvent::POINTER_ACTION_ENTER_WINDOW;
    ASSERT_NO_FATAL_FAILURE(WinMgr->DispatchPointer(pointerAction));
    pointerAction = PointerEvent::POINTER_ACTION_LEAVE_WINDOW;
    ASSERT_NO_FATAL_FAILURE(WinMgr->DispatchPointer(pointerAction));
    pointerAction = PointerEvent::POINTER_ACTION_MOVE;
    ASSERT_NO_FATAL_FAILURE(WinMgr->DispatchPointer(pointerAction));
}

/**
 * @tc.name: InputWindowsManagerTest_NotifyPointerToWindow_001
 * @tc.desc: Test notifying pointer events to window
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_NotifyPointerToWindow_001, TestSize.Level1)
{    
    InputWindowsManager inputWindowsManager;
    auto windowInfo = inputWindowsManager.GetWindowInfo(0, 0);
    inputWindowsManager.lastWindowInfo_ = WindowInfo{1};
    ASSERT_NO_FATAL_FAILURE(WinMgr->NotifyPointerToWindow());
}

/**
 * @tc.name: InputWindowsManagerTest_PrintWindowInfo_001
 * @tc.desc: Test printing window information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_PrintWindowInfo_001, TestSize.Level1)
{    
    WindowInfo windowInfo1;
    windowInfo1.id = 1;
    windowInfo1.pid = 100;
    windowInfo1.uid = 200;
    windowInfo1.area = {0, 0, 800, 600};
    windowInfo1.defaultHotAreas = {{10, 10, 100, 100}, {200, 200, 50, 50}};
    windowInfo1.pointerHotAreas = {{30, 30, 150, 150}, {400, 400, 70, 70}};
    windowInfo1.agentWindowId = 10;
    windowInfo1.flags = 1;
    windowInfo1.displayId = 3;
    windowInfo1.zOrder = 4.0f;
    windowInfo1.pointerChangeAreas = {10, 20, 30};
    windowInfo1.transform = {1.0f, 2.0f, 3.0f};
    WindowInfo windowInfo2;
    windowInfo2.id = 2;
    windowInfo2.pid = 101;
    windowInfo2.uid = 201;
    windowInfo2.area = {800, 600, 1024, 768};
    windowInfo2.defaultHotAreas = {{50, 50, 200, 200}, {600, 600, 100, 100}};
    windowInfo2.pointerHotAreas = {{70, 70, 250, 250}, {800, 800, 120, 120}};
    windowInfo2.agentWindowId = 20;
    windowInfo2.flags = 2;
    windowInfo2.displayId = 4;
    windowInfo2.zOrder = 5.0f;
    windowInfo2.pointerChangeAreas = {40, 50, 60};
    windowInfo2.transform = {4.0f, 5.0f, 6.0f};
    std::vector<WindowInfo> windowsInfo = {windowInfo1, windowInfo2};
    ASSERT_NO_FATAL_FAILURE(WinMgr->PrintWindowInfo(windowsInfo));
}

/**
 * @tc.name: InputWindowsManagerTest_PrintWindowGroupInfo_001
 * @tc.desc: Test printing window group information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_PrintWindowGroupInfo_001, TestSize.Level1)
{
    WindowGroupInfo testData;
    testData.focusWindowId = 1;
    testData.displayId = 2;
    ASSERT_NO_FATAL_FAILURE(WinMgr->PrintWindowGroupInfo(testData));
}

/**
 * @tc.name: InputWindowsManagerTest_PrintDisplayInfo_001
 * @tc.desc: Test printing display information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_PrintDisplayInfo_001, TestSize.Level1)
{    
    InputWindowsManager manager;
    manager.displayGroupInfo_.width = 1920;
    manager.displayGroupInfo_.height = 1080;
    manager.displayGroupInfo_.focusWindowId = 1;
    manager.displayGroupInfo_.windowsInfo.push_back(WindowInfo());
    manager.displayGroupInfo_.displaysInfo.push_back(DisplayInfo());
    ASSERT_NO_FATAL_FAILURE(WinMgr->PrintDisplayInfo());
}

/**
 * @tc.name: InputWindowsManagerTest_GetPhysicalDisplay_001
 * @tc.desc: Test getting physical display information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetPhysicalDisplay_001, TestSize.Level1)
{    
    int32_t id = 1;
    const DisplayInfo* displayInfo = WinMgr->GetPhysicalDisplay(id);
    EXPECT_NE(displayInfo, nullptr);
    EXPECT_EQ(displayInfo->id, id);
}

/**
 * @tc.name: InputWindowsManagerTest_GetPhysicalDisplay_002
 * @tc.desc: Test getting physical display information
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_GetPhysicalDisplay_002, TestSize.Level1)
{    
    int32_t id = -1;
    const DisplayInfo* displayInfo = WinMgr->GetPhysicalDisplay(id);
    EXPECT_EQ(displayInfo, nullptr);
}
} // namespace MMI
} // namespace OHOS
