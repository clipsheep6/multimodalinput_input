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
 
#include <gtest/gtest.h>
 
#include "mmi_log.h"
#include "mouse_event_normalize.h"
 
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "MouseEventNormalizeEXTest"
 
namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
}
class MouseEventNormalizeEXTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
};
 
/**
 * @tc.name: MouseTransformProcessorTest_HandleTouchpadLeftButton_001
 * @tc.desc: Test GetPointerLocation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MouseEventNormalizeEXTest, MouseEventNormalizeEXTest_GetPointerLocation_001, TestSize.Level1)
{
    int32_t displayId = 0;
    double displayX = 0.0;
    double displayY = 0.0;
    EXPECT_EQ(MouseEventHdr->GetPointerLocation(displayId, displayX, displayY), RET_OK);
    EXPECT_EQ(displayId, -1);
    EXPECT_EQ(displayX, 0);
    EXPECT_EQ(displayY, 0);
}
}
}