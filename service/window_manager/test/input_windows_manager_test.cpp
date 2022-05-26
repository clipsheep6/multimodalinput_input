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

#include <gtest/gtest.h>
#define private public
#include "input_windows_manager.h"


namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class InputWindowsManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
#ifdef OHOS_BUILD_KEY_MOUSE
    void SetUp()
    {
        const int32_t TOP_LEFT_X1 = 0;
        const int32_t WIDTH1 = 500;
        const int32_t TOP_LEFT_Y1 = 0;
        const int32_t HEIGHT1 = 200;
        const int32_t ID1 = 1;
        LogicalDisplayInfo info1;
        info1.topLeftX = TOP_LEFT_X1;
        info1.width = WIDTH1;
        info1.topLeftY = TOP_LEFT_Y1;
        info1.height = HEIGHT1;
        info1.id = ID1;
        LogicalDisplayInfo info2 = info1;
        info2.topLeftX = WIDTH1;
        info2.topLeftY = HEIGHT1;
        info2.id = ID1 + 1;
        WinMgr->logicalDisplays_.push_back(info1);
        WinMgr->logicalDisplays_.push_back(info2);
    }
#endif
};

#ifdef OHOS_BUILD_KEY_MOUSE
HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_001, TestSize.Level1)
{
    double x = 0;
    double y = 100;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 0);
    EXPECT_EQ(mouseInfo.globalY, 100);
    EXPECT_EQ(mouseInfo.displayId, 1);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_002, TestSize.Level1)
{
    double x = 200;
    double y = 0;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 200);
    EXPECT_EQ(mouseInfo.globalY, 0);
    EXPECT_EQ(mouseInfo.displayId, 1);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_003, TestSize.Level1)
{
    double x = 100;
    double y = 70;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 100);
    EXPECT_EQ(mouseInfo.globalY, 70);
    EXPECT_EQ(mouseInfo.displayId, 1);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_004, TestSize.Level1)
{
    double x = -100;
    double y = 100;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 0);
    EXPECT_EQ(mouseInfo.globalY, 100);
    EXPECT_EQ(mouseInfo.displayId, 1);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_005, TestSize.Level1)
{
    double x = 500;
    double y = 200;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 500);
    EXPECT_EQ(mouseInfo.globalY, 200);
    EXPECT_EQ(mouseInfo.displayId, 1);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_006, TestSize.Level1)
{
    double x = 600;
    double y = 200;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 600);
    EXPECT_EQ(mouseInfo.globalY, 200);
    EXPECT_EQ(mouseInfo.displayId, 2);
}

HWTEST_F(InputWindowsManagerTest, InputWindowsManagerTest_UpdateAndAdjustMouseLoction_007, TestSize.Level1)
{
    double x = 1100;
    double y = 300;
    WinMgr->UpdateAndAdjustMouseLoction(x, y);
    MouseLocation mouseInfo = WinMgr->GetMouseInfo();
    EXPECT_EQ(mouseInfo.globalX, 1000);
    EXPECT_EQ(mouseInfo.globalY, 300);
    EXPECT_EQ(mouseInfo.displayId, 2);
}
#endif
} // namespace
