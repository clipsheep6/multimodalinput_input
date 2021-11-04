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
 /**
 * @addtogroup multiInput
 * @{
 *
 * @brief Defines multiInput-related tests, including case data and functions for auto test,
 *
 * @since 1.0
 * @version 2.0
 */
#include <gtest/gtest.h>
#include "libmmi_util.h"
#include "st_data_process.h"
#include "st_helper.h"
#include "st_client_helper.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

struct CaseInfo {
    std::vector<std::string> eventList = {""};
    std::vector<std::string> deviceMsg = {""};
    std::vector<std::string> libinputExpectList = {""};
    std::vector<std::string> standardExpectList = {""};
    std::vector<std::string> expectL3Event = {""};
    std::vector<std::string> dispatcherList = {""};
};

class multiHandouthandUnRegRTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    
    void SetUp() override;
    void TearDown() override;
    
    void DoCompareAndExpect(const struct CaseInfo &ci);
private:
    void ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList, const size_t i);
private:
	std::string ReplaceExpectXY(std::string str, struct AutoTestCoordinate autoTestCoordinate);		
private:
    static StClientHelper st_client_helper_;
};

StClientHelper multiHandouthandUnRegRTest::st_client_helper_;

void multiHandouthandUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandouthandUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandouthandUnRegRTest::SetUp()
{
}

void multiHandouthandUnRegRTest::TearDown()
{
}

/*
 * Feature: ExpectCombKey
 * Function: ExpectCombKey
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
std::string multiHandouthandUnRegRTest::ReplaceExpectXY(std::string str,
                                                             struct AutoTestCoordinate autoTestCoordinate) {
	str = GetSTHelper().ReplaceString(str, std::regex("<X>"), autoTestCoordinate.focusWindowRawX);
	return GetSTHelper().ReplaceString(str, std::regex("<Y>"), autoTestCoordinate.focusWindowRawY);
}

/*
 * Feature: DoCompareAndExpect
 * Function: DoCompareAndExpect
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
void multiHandouthandUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
{
    for (unsigned int i = 0; i < ci.eventList.size(); i++) {
        GetSTHelper().InJectionEvent(ci.eventList[i], ci.deviceMsg[i]);
        std::string ExpectServerResult = "NULL|NULL|NULL |";
        std::string ExpectClientResult = "NULL|NULL|NULL |";
		struct AutoTestCoordinate autoTestCoordinate = GetDataProcess().GetCoordinate();
        if (ci.libinputExpectList[i] != "eventJoyStickAxis") {
            EXPECT_EQ(GetDataProcess().GetLibinputValue(), ReplaceExpectXY(ci.libinputExpectList[i],
				                                                               autoTestCoordinate));
        } else {
            bool expectAxisResult = true;
            std::vector<float> axisStrList = GetDataProcess().GetAxisStrList();
            for (unsigned int j = 0; j < axisStrList.size(); j++) {
                if (axisStrList[i] < -1 || axisStrList[i] > 1) {
                    expectAxisResult = false;
                    break;
                }
            }
            EXPECT_EQ(expectAxisResult , true);
        }
        EXPECT_EQ(GetDataProcess().GetStandardResult(), ci.standardExpectList[i]);
        //dispatcher FocusWindow
			  GetSTHelper().ReplaceServerValue(std::regex("windowlist"), GetDataProcess().GetWindowsList());
			  struct ClientList clientList = GetDataProcess().GetClientMsg(GetDataProcess().GetFocusId());
			  GetSTHelper().ReplaceServerValue(std::regex("fd"), std::to_string(clientList.fd));
			  GetSTHelper().ReplaceServerValue(std::regex("surfaceId"), std::to_string(clientList.surfaceId));
			  GetSTHelper().ReplaceServerValue(std::regex("abilityId"), std::to_string(clientList.abilityId));
			  GetSTHelper().ReplaceClientValue(std::regex("fd"), std::to_string(clientList.fd));
			  GetSTHelper().ReplaceClientValue(std::regex("surfaceId"), std::to_string(clientList.surfaceId));
			  GetSTHelper().ReplaceClientValue(std::regex("abilityId"), std::to_string(clientList.abilityId));
        if (ci.libinputExpectList[i] != "NULL|NULL|NULL " && ci.dispatcherList[i] != "|1|0|0" &&
            ci.dispatcherList[i] != "|0|0|1") {
            ExpectServerResult = ci.expectL3Event[i] +
                      GetSTHelper().valueServer;
            ExpectClientResult = ci.expectL3Event[i] +
                      GetSTHelper().valueClient;
        }
        EXPECT_EQ(GetDataProcess().GetActualServerResult(), ReplaceExpectXY(ExpectServerResult, autoTestCoordinate));
        EXPECT_EQ(GetDataProcess().GetActualClientResult(), ReplaceExpectXY(ExpectClientResult, autoTestCoordinate));
        EXPECT_EQ((ci.dispatcherList[i] == GetDataProcess().GetDispatcherStr()) ||
                  (ci.dispatcherList[i].find(GetDataProcess().GetDispatcherStr()) != ci.dispatcherList[i].npos),
                  true);
        GetDataProcess().ClearAll();
    }
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_002_R
 * Function: Multi_HandOut_Hand_UnReg_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_314_press.json", "gamepad_key_314_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|314|1", "eventJoyStickKey|314|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2311|1|", "eventKeyboard|2311|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_001_R
 * Function: Multi_HandOut_Hand_UnReg_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_316_press.json", "gamepad_key_316_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|316|1", "eventJoyStickKey|316|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2313|1|", "eventKeyboard|2313|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_004_R
 * Function: Multi_HandOut_Hand_UnReg_004_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_004_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_315_press.json", "gamepad_key_315_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|315|1", "eventJoyStickKey|315|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2312|1|", "eventKeyboard|2312|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_006_R
 * Function: Multi_HandOut_Hand_UnReg_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_307_press.json", "gamepad_key_307_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|307|1", "eventJoyStickKey|307|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2304|1|", "eventKeyboard|2304|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_007_R
 * Function: Multi_HandOut_Hand_UnReg_007_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_007_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_308_press.json", "gamepad_key_308_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|308|1", "eventJoyStickKey|308|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2305|1|", "eventKeyboard|2305|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_008_R
 * Function: Multi_HandOut_Hand_UnReg_008_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_008_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_304_press.json", "gamepad_key_304_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|304|1", "eventJoyStickKey|304|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2301|1|", "eventKeyboard|2301|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_009_R
 * Function: Multi_HandOut_Hand_UnReg_009_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_009_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_305_press.json", "gamepad_key_305_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|305|1", "eventJoyStickKey|305|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2302|1|", "eventKeyboard|2302|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_013_R
 * Function: Multi_HandOut_Hand_UnReg_013_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_013_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_310_press.json", "gamepad_key_310_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|310|1", "eventJoyStickKey|310|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2307|1|", "eventKeyboard|2307|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_014_R
 * Function: Multi_HandOut_Hand_UnReg_014_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_014_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_311_press.json", "gamepad_key_311_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|311|1", "eventJoyStickKey|311|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2308|1|", "eventKeyboard|2308|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_011_R
 * Function: Multi_HandOut_Hand_UnReg_011_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_011_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_317_press.json", "gamepad_key_317_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|317|1", "eventJoyStickKey|317|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2314|1|", "eventKeyboard|2314|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_012_R
 * Function: Multi_HandOut_Hand_UnReg_012_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_012_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_318_press.json", "gamepad_key_318_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|318|1", "eventJoyStickKey|318|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2315|1|", "eventKeyboard|2315|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_019_R
 * Function: Multi_HandOut_Hand_UnReg_019_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_019_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_shizi_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_020_R
 * Function: Multi_HandOut_Hand_UnReg_020_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_020_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_shizi_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_021_R
 * Function: Multi_HandOut_Hand_UnReg_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_shizi_left.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_022_R
 * Function: Multi_HandOut_Hand_UnReg_022_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_022_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_shizi_right.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_027_R
 * Function: Multi_HandOut_Hand_UnReg_027_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_027_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_LT.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"NULL|NULL|NULL"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"NULL|NULL|NULL|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_028_R
 * Function: Multi_HandOut_Hand_UnReg_028_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_028_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_RT.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"NULL|NULL|NULL"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"NULL|NULL|NULL|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_015_R
 * Function: Multi_HandOut_Hand_UnReg_015_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_015_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock1_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_016_R
 * Function: Multi_HandOut_Hand_UnReg_016_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_016_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock1_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_017_R
 * Function: Multi_HandOut_Hand_UnReg_017_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_017_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock1_left.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_018_R
 * Function: Multi_HandOut_Hand_UnReg_018_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_018_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock1_right.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_023_R
 * Function: Multi_HandOut_Hand_UnReg_023_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_023_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock2_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_024_R
 * Function: Multi_HandOut_Hand_UnReg_024_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_024_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock2_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_025_R
 * Function: Multi_HandOut_Hand_UnReg_025_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_025_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock2_left.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_026_R
 * Function: Multi_HandOut_Hand_UnReg_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_rock2_right.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_Unusual_001_R
 * Function: Multi_HandOut_Hand_UnReg_Unusual_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_Unusual_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_172_press.json", "gamepad_key_172_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"NULL|NULL|NULL", "NULL|NULL|NULL"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"NULL|NULL|NULL|", "NULL|NULL|NULL|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Hand_UnReg_Unusual_002_R
 * Function: Multi_HandOut_Hand_UnReg_Unusual_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouthandUnRegRTest, Multi_HandOut_Hand_UnReg_Unusual_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json", "gamepad_key_316_press.json", "gamepad_key_316_release.json",
                    "gamepad_key_316_press.json", "gamepad_key_316_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0",
                             "eventJoyStickKey|316|1", "eventJoyStickKey|316|0", "eventJoyStickKey|316|1",
                             "eventJoyStickKey|316|0", "eventJoyStickKey|316|1", "eventJoyStickKey|316|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|",
                        "eventKeyboard|2313|1|", "eventKeyboard|2313|0|", "eventKeyboard|2313|1|",
                        "eventKeyboard|2313|0|", "eventKeyboard|2313|1|", "eventKeyboard|2313|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}
}