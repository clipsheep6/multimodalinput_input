/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "config_multimodal.h"
#include "define_multimodal.h"
#include "input_manager.h"

namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext;
} // namespace

class PointerDrawingManagerTest : public ::testing::Test {
public:
    static void TearDownTestSuite()
    {
#ifdef MMI_COVERAGE
        inputManager_->DumpCoverage();
#endif
    }

    inline static InputManager *inputManager_ = InputManager::GetInstance();
};

HWTEST_F(PointerDrawingManagerTest, TestPointerSize, TestSize.Level1)
{
    int32_t size;

    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, DEFAULT_POINTER_SIZE);

    EXPECT_EQ(inputManager_->SetPointerSize(40), RET_OK);
    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, 40);

    EXPECT_EQ(inputManager_->SetPointerSize(50), RET_OK);
    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, 50);

    EXPECT_EQ(inputManager_->SetPointerSize(0), RET_OK);
    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, MIN_POINTER_SIZE);

    EXPECT_EQ(inputManager_->SetPointerSize(100000), RET_OK);
    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, MAX_POINTER_SIZE);

    // Restore default size
    EXPECT_EQ(inputManager_->SetPointerSize(DEFAULT_POINTER_SIZE), RET_OK);
    EXPECT_EQ(inputManager_->GetPointerSize(size), RET_OK);
    EXPECT_EQ(size, DEFAULT_POINTER_SIZE);
}

HWTEST_F(PointerDrawingManagerTest, TestPointerImages, TestSize.Level1)
{
    std::map<int32_t, std::string> images;
    EXPECT_EQ(inputManager_->SetPointerImages(images), RET_OK);
    images[DEFAULT] = "/system/etc/multimodalinput/mouse_icon/Default.svg";
    EXPECT_EQ(inputManager_->SetPointerImages(images), RET_OK);
    images[DEFAULT] = "";
    EXPECT_EQ(inputManager_->SetPointerImages(images), INVALID_PARAMETER_FILE_PATH);
    images = {};
    images[-1] = "/system/etc/multimodalinput/mouse_icon/Default.svg";
    EXPECT_EQ(inputManager_->SetPointerImages(images), INVALID_PARAMETER_CURSOR_STYLE);
}
} // namespace MMI
} // namespace OHOS
