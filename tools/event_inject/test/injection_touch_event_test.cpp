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
#include <gtest/gtest.h>
#include "injection_touch_event.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;

class InjectionTouchEventTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
};

HWTEST_F(InjectionTouchEventTest, GetTouchFile_001, TestSize.Level1)
{
    const std::string path = "/root/projects/bi/deviceconfig.ini";

    InjectionTouchEvent inject;
    inject.GetTouchFile(path);
}

HWTEST_F(InjectionTouchEventTest, GetTouchFile_002, TestSize.Level1)
{
    const std::string path = "";

    InjectionTouchEvent inject;
    inject.GetTouchFile(path);
}

HWTEST_F(InjectionTouchEventTest, GetTouchFile_003, TestSize.Level1)
{
    const std::string path = "./";

    InjectionTouchEvent inject;
    inject.GetTouchFile(path);
}

HWTEST_F(InjectionTouchEventTest, GetTouchFile_004, TestSize.Level1)
{
    const std::string path = "./testconfig.log";

    InjectionTouchEvent inject;
    inject.GetTouchFile(path);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_001, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "123456789123456");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_002, TestSize.Level2)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "222222222222222");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}
HWTEST_F(InjectionTouchEventTest, AnalysisData_003, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "-13456789123456");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_004, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "-1-256789123456");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}
HWTEST_F(InjectionTouchEventTest, AnalysisData_005, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "-1-2-6669123456");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_006, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "-1-2-666-777456");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_007, TestSize.Level1)
{
    InputEvent input;
    char szData[16] = { '0' };
    strcpy_s(szData, sizeof(szData), "-1-2-666-777356");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_008, TestSize.Level1)
{
    InputEvent input;
    char szData[18] = { '0' };
    strcpy_s(szData, sizeof(szData), "-1-2-666-7773-888");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, AnalysisData_009, TestSize.Level1)
{
    InputEvent input;
    char szData[15] = { '0' };
    strcpy_s(szData, sizeof(szData), "-9-8-666-77738");

    InjectionTouchEvent inject;
    inject.AnalysisData(szData, input);
}

HWTEST_F(InjectionTouchEventTest, SetDeviceHandle_002, TestSize.Level2)
{
    const char deviceFile[1] = "";

    InjectionTouchEvent inject;
    inject.SetDeviceHandle(deviceFile);
}

HWTEST_F(InjectionTouchEventTest, SetDeviceHandle_003, TestSize.Level2)
{
    const char deviceFile[] = "./utSimulate.sh";

    InjectionTouchEvent inject;
    inject.SetDeviceHandle(deviceFile);
}
} // namespace
