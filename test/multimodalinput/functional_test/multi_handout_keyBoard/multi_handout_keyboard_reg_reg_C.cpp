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
    std::vector<std::vector<std::vector<std::string>>> combKeyList = {{{""}}};
    std::vector<std::string> dispatcherList = {""};
};

class multiHandoutkeyboardregRegCTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    
    void SetUp() override;
    void TearDown() override;
    
    void DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum);
private:
    void ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList, const size_t i);
private:
	  std::string ReplaceExpectXY(std::string str, struct AutoTestCoordinate autoTestCoordinate);	
private:
    static StClientHelper st_client_helper_;
};

StClientHelper multiHandoutkeyboardregRegCTest::st_client_helper_;

void multiHandoutkeyboardregRegCTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_C");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutkeyboardregRegCTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutkeyboardregRegCTest::SetUp()
{
}

void multiHandoutkeyboardregRegCTest::TearDown()
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
void multiHandoutkeyboardregRegCTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
                            const size_t i)
{
    std::string mix_server = GetDataProcess().MixedKeyServer();
    std::string mix_client = GetDataProcess().MixedKeyClient();
    if (combKeyList[i][0][0] != "") {
        if (combKeyList[i].size() == 1) {
            EXPECT_EQ(mix_server , combKeyList[i][0][0]);
            EXPECT_EQ(mix_client , combKeyList[i][0][1]);
        } else if (combKeyList[i].size() > 1) {
            if (mix_server == "") EXPECT_EQ(mix_server , combKeyList[i][1][0]);
            if (mix_client == "") EXPECT_EQ(mix_client , combKeyList[i][1][1]);
            else {
                bool expectMixServerResult = true;
                bool expectMixClientResult = true;
                for(unsigned int j=1; j<combKeyList[i].size(); j++) {
                    if (combKeyList[i][0][0] == "||") {
                        expectMixServerResult = expectMixServerResult ||
                                                mix_server.find(combKeyList[i][j][0]) != mix_server.npos;
                        expectMixClientResult = expectMixServerResult ||
                                                mix_client.find(combKeyList[i][j][1]) != mix_client.npos;
                        if (expectMixServerResult) break;
                        if (expectMixClientResult) break;
                    } else if (combKeyList[i][0][0] == "&&") {
                        expectMixServerResult = expectMixServerResult &&
                                                mix_server.find(combKeyList[i][j][0]) != mix_server.npos;
                        expectMixClientResult = expectMixServerResult &&
                                                mix_client.find(combKeyList[i][j][1]) != mix_client.npos;
                        if (!expectMixServerResult) break;
                        if (!expectMixClientResult) break;
                    }
                }
                EXPECT_EQ(expectMixServerResult , true);
                EXPECT_EQ(expectMixClientResult , true);
            }
        }
    } else {
        EXPECT_EQ(mix_server , "");
        EXPECT_EQ(mix_client , "");
    }
}

/*
 * Feature: ExpectCombKey
 * Function: ExpectCombKey
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
std::string multiHandoutkeyboardregRegCTest::ReplaceExpectXY(std::string str,
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
void multiHandoutkeyboardregRegCTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
{
    for (unsigned int i = 0; i < ci.eventList.size(); i++) {
        GetSTHelper().InJectionEvent(ci.eventList[i], ci.deviceMsg[i]);
        if (caseNum.find("_Voice_") == caseNum.npos && caseNum.find("_Phalangeal_") == caseNum.npos) {
		    struct AutoTestCoordinate autoTestCoordinate = GetDataProcess().GetCoordinate();
            std::string ExpectServerResult = "NULL|NULL|NULL |";
            std::string ExpectClientResult = "NULL|NULL|NULL |";
			if (ci.libinputExpectList[i] != "eventJoyStickAxis") {
                EXPECT_EQ(GetDataProcess().GetLibinputValue(), ReplaceExpectXY(ci.libinputExpectList[i],
				                                                               autoTestCoordinate));
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
            if (ci.libinputExpectList[i] != "NULL|NULL|NULL " && ci.dispatcherList[i] != "|0|0|1" &&
                ci.dispatcherList[i] != "|1|0|0") {
                ExpectServerResult = ci.expectL3Event[i] + ci.deviceMsg[i] +
                    GetSTHelper().valueServer;
                ExpectClientResult = ci.expectL3Event[i] + ci.deviceMsg[i] +
                    GetSTHelper().valueClient;                         
            }
            if ((ci.dispatcherList[i] == "|0|0|1" || ci.dispatcherList[i] == "|1|0|0") &&
                ci.combKeyList[i][0][0] != "") {
                ExpectServerResult += GetSTHelper().valueServer;
                ExpectClientResult += GetSTHelper().valueClient;
            }
            EXPECT_EQ(GetDataProcess().GetActualServerResult(), ReplaceExpectXY(ExpectServerResult,
                                                                                autoTestCoordinate));
            EXPECT_EQ(GetDataProcess().GetActualClientResult(), ReplaceExpectXY(ExpectClientResult,
                                                                                autoTestCoordinate));
            EXPECT_EQ((ci.dispatcherList[i] == GetDataProcess().GetDispatcherStr())
                     || (ci.dispatcherList[i].find(GetDataProcess().GetDispatcherStr())!=
                     ci.dispatcherList[i].npos), true);
        }
        ExpectCombKey(ci.combKeyList, i);        
        GetDataProcess().ClearAll();
    }
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_003_C
 * Function: Multi_HandOut_Keyboard_Reg_App_003_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_003_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|109|1", "eventKeyboard|109|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2069|1|", "eventKeyboard|2069|0|"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_003_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_008_C
 * Function: Multi_HandOut_Keyboard_Reg_App_008_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_008_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|103|1", "eventKeyboard|103|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2012|1|", "eventKeyboard|2012|0|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_008_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_013_C
 * Function: Multi_HandOut_Keyboard_Reg_App_013_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_013_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|105|1", "eventKeyboard|105|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2014|1|", "eventKeyboard|2014|0|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_013_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_018_C
 * Function: Multi_HandOut_Keyboard_Reg_App_018_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_018_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|104|1", "eventKeyboard|104|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2068|1|", "eventKeyboard|2068|0|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_018_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_023_C
 * Function: Multi_HandOut_Keyboard_Reg_App_023_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_023_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|108|1", "eventKeyboard|108|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2013|1|", "eventKeyboard|2013|0|"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_023_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_028_C
 * Function: Multi_HandOut_Keyboard_Reg_App_028_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_028_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|106|1", "eventKeyboard|106|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2015|1|", "eventKeyboard|2015|0|"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_028_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_033_C
 * Function: Multi_HandOut_Keyboard_Reg_App_033_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_033_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|63|1", "eventKeyboard|63|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2094|1|", "eventKeyboard|2094|0|"};
    ci.combKeyList = {{{"1107","OnRefresh"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_033_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_038_C
 * Function: Multi_HandOut_Keyboard_Reg_App_038_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_038_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|19|1", "eventKeyboard|19|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2034|1|", "eventKeyboard|2034|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"1107","OnRefresh"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_038_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_043_C
 * Function: Multi_HandOut_Keyboard_Reg_App_043_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_043_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_MENU_press.json", "keyboard_KEY_MENU_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|127|1", "eventKeyboard|127|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2067|1|", "eventKeyboard|2067|0|"};
    ci.combKeyList = {{{"1101","OnShowMenu"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_043_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_048_C
 * Function: Multi_HandOut_Keyboard_Reg_App_048_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_048_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_SHIFT_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|42|1", "eventKeyboard|68|1", "eventKeyboard|68|0",
                             "eventKeyboard|42|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2047|1|", "eventKeyboard|2099|1|", "eventKeyboard|2099|0|",
                        "eventKeyboard|2047|0|"};
    ci.combKeyList = {{{""}}, {{"1101","OnShowMenu"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_048_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_053_C
 * Function: Multi_HandOut_Keyboard_Reg_App_053_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_053_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json",
                    "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|42|1", "eventKeyboard|68|1",
                             "eventKeyboard|68|0", "eventKeyboard|42|0", "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2047|1|", "eventKeyboard|2099|1|",
                        "eventKeyboard|2099|0|", "eventKeyboard|2047|0|", "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{""}}, {{"1101","OnShowMenu"}}, {{""}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_053_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_058_C
 * Function: Multi_HandOut_Keyboard_Reg_App_058_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_058_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|46|1", "eventKeyboard|46|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2019|1|", "eventKeyboard|2019|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"1103","OnCopy"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_058_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_063_C
 * Function: Multi_HandOut_Keyboard_Reg_App_063_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_063_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_CTRL_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|97|1", "eventKeyboard|47|1", "eventKeyboard|47|0",
                             "eventKeyboard|97|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2073|1|", "eventKeyboard|2038|1|", "eventKeyboard|2038|0|",
                        "eventKeyboard|2073|0|"};
    ci.combKeyList = {{{""}}, {{"1104","OnPaste"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_063_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_068_C
 * Function: Multi_HandOut_Keyboard_Reg_App_068_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_068_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|45|1", "eventKeyboard|45|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2040|1|", "eventKeyboard|2040|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"1105","OnCut"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_068_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_073_C
 * Function: Multi_HandOut_Keyboard_Reg_App_073_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_073_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|44|1", "eventKeyboard|44|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2042|1|", "eventKeyboard|2042|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"1106","OnUndo"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_073_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_078_C
 * Function: Multi_HandOut_Keyboard_Reg_App_078_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_078_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_ENTER_press.json", "keyboard_KEY_NUMPAD_ENTER_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|96|1", "eventKeyboard|96|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2119|1|", "eventKeyboard|2119|0|"};
    ci.combKeyList = {{{"&&"},{"1110","OnEnter"},{"1102","OnSend"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_078_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_083_C
 * Function: Multi_HandOut_Keyboard_Reg_App_083_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_083_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|1|1", "eventKeyboard|1|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2070|1|", "eventKeyboard|2070|0|"};
    ci.combKeyList = {{{"&&"},{"1109","OnCancel"},{"1113","OnBack"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_083_C");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_App_088_C
 * Function: Multi_HandOut_Keyboard_Reg_App_088_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegCTest, Multi_HandOut_Keyboard_Reg_App_088_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|25|1", "eventKeyboard|25|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2032|1|", "eventKeyboard|2032|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"1114","OnPrint"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_App_088_C");
}
}