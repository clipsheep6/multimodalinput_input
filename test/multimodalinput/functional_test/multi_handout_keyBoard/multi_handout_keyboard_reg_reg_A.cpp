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

class multiHandoutkeyboardregRegATest : public testing::Test {
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

StClientHelper multiHandoutkeyboardregRegATest::st_client_helper_;

void multiHandoutkeyboardregRegATest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_A");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutkeyboardregRegATest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutkeyboardregRegATest::SetUp()
{
}

void multiHandoutkeyboardregRegATest::TearDown()
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
void multiHandoutkeyboardregRegATest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
                            const size_t i)
{
    std::string mix_server = GetDataProcess().MixedKeyServer();
    std::string mix_client = GetDataProcess().MixedKeyClient();
    if (combKeyList[i][0][0] != "") {
        if (combKeyList[i].size() == 1) {
            EXPECT_EQ(mix_server , combKeyList[i][0][0]);
            EXPECT_EQ(mix_client , combKeyList[i][0][1]);
        } else if (combKeyList[i].size() > 1) {
            if (mix_server == "") {
                EXPECT_EQ(mix_server, combKeyList[i][1][0]);
            }

            if (mix_client == "") {
                EXPECT_EQ(mix_client, combKeyList[i][1][1]);
            } else {
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
std::string multiHandoutkeyboardregRegATest::ReplaceExpectXY(std::string str,
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
void multiHandoutkeyboardregRegATest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_014_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_014_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_014_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_F11_press.json", "keyboard_KEY_F11_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|87|1", "eventKeyboard|87|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2100|1|", "eventKeyboard|2100|0|"};
    ci.combKeyList = {{{"4001","OnScreenShot"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_014_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_015_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_015_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_015_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json",
                    "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|42|1", "eventKeyboard|31|1",
                             "eventKeyboard|31|0", "eventKeyboard|42|0", "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2047|1|", "eventKeyboard|2035|1|",
                        "eventKeyboard|2035|0|", "eventKeyboard|2047|0|", "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{""}}, {{"4001","OnScreenShot"}}, {{""}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_015_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_016_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_016_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_016_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|32|1", "eventKeyboard|32|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2020|1|", "eventKeyboard|2020|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4005","OnGotoDesktop"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_016_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_017_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_017_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_017_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|35|1", "eventKeyboard|35|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2024|1|", "eventKeyboard|2024|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4005","OnGotoDesktop"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_017_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_018_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_018_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_018_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|15|1", "eventKeyboard|15|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2049|1|", "eventKeyboard|2049|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4006","OnRecent"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_018_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_019_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_019_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_019_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_ALT_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|56|1", "eventKeyboard|15|1", "eventKeyboard|15|0",
                             "eventKeyboard|56|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2045|1|", "eventKeyboard|2049|1|", "eventKeyboard|2049|0|",
                        "eventKeyboard|2045|0|"};
    ci.combKeyList = {{{""}}, {{"4006","OnRecent"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_019_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_020_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_020_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_020_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|49|1", "eventKeyboard|49|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2030|1|", "eventKeyboard|2030|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4007","OnShowNotifiCation"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_020_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_021_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_021_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_021_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|38|1", "eventKeyboard|38|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2028|1|", "eventKeyboard|2028|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4008","OnLockScreen"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_021_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_022_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_022_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_022_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|31|1", "eventKeyboard|31|0",
                             "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2035|1|", "eventKeyboard|2035|0|",
                        "eventKeyboard|2076|0|"};
    ci.combKeyList = {{{""}}, {{"4009","OnSearch"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_022_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_023_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_023_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_023_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|33|1", "eventKeyboard|33|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2022|1|", "eventKeyboard|2022|0|",
                        "eventKeyboard|2072|0|"};
    ci.combKeyList = {{{""}}, {{"4009","OnSearch"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_023_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_024_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_024_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_024_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_F4_press.json", "keyboard_KEY_F4_release.json", "keyboard_KEY_ALT_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|56|1", "eventKeyboard|62|1", "eventKeyboard|62|0",
                             "eventKeyboard|56|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2045|1|", "eventKeyboard|2093|1|", "eventKeyboard|2093|0|",
                        "eventKeyboard|2045|0|"};
    ci.combKeyList = {{{""}}, {{"4010","OnClosePage"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_024_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_025_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_025_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_025_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_CTRL_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", ""};
    ci.libinputExpectList = {"eventKeyboard|97|1", "eventKeyboard|17|1", "eventKeyboard|17|0",
                             "eventKeyboard|97|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2073|1|", "eventKeyboard|2039|1|", "eventKeyboard|2039|0|",
                        "eventKeyboard|2073|0|"};
    ci.combKeyList = {{{""}}, {{"4010","OnClosePage"}}, {{""}}, {{""}}};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_025_A");
}

/*
 * Feature: Multi_HandOut_Keyboard_Reg_Sys_026_A
 * Function: Multi_HandOut_Keyboard_Reg_Sys_026_A
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardregRegATest, Multi_HandOut_Keyboard_Reg_Sys_026_A, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"keyboard_KEY_F4_press.json", "keyboard_KEY_F4_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|62|1", "eventKeyboard|62|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2093|1|", "eventKeyboard|2093|0|"};
    ci.combKeyList = {{{"4012","OnMute"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_Keyboard_Reg_Sys_026_A");
}
}