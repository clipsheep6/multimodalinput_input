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

class multiHandoutremotecontrolRegCTest : public testing::Test {
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

StClientHelper multiHandoutremotecontrolRegCTest::st_client_helper_;

void multiHandoutremotecontrolRegCTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_C");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutremotecontrolRegCTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutremotecontrolRegCTest::SetUp()
{
}

void multiHandoutremotecontrolRegCTest::TearDown()
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
void multiHandoutremotecontrolRegCTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
std::string multiHandoutremotecontrolRegCTest::ReplaceExpectXY(std::string str,
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
void multiHandoutremotecontrolRegCTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_HandOut_RemoteControl_Reg_App_003_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_003_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_003_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_onPause_press.json", "remote_onPause_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|119|1", "eventKeyboard|119|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2080|1|", "eventKeyboard|2080|0|"};
    ci.combKeyList = {{{"3002","OnPause"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_003_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_008_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_008_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_008_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_onPlay_press.json", "remote_onPlay_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|207|1", "eventKeyboard|207|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2085|1|", "eventKeyboard|2085|0|"};
    ci.combKeyList = {{{"3001","OnPlay"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_008_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_013_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_013_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_013_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_CHANNELUP_press.json", "remote_CHANNELUP_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|402|1", "eventKeyboard|402|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2690|1|", "eventKeyboard|2690|0|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_013_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_018_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_018_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_018_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_CHANNELDOWN_press.json", "remote_CHANNELDOWN_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|403|1", "eventKeyboard|403|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2691|1|", "eventKeyboard|2691|0|"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_018_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_023_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_023_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_023_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_VOLUMEUP_press.json", "remote_VOLUMEUP_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|115|1", "eventKeyboard|115|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|16|1|", "eventKeyboard|16|0|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_023_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_028_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_028_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_028_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_VOLUMEDOWN_press.json", "remote_VOLUMEDOWN_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|114|1", "eventKeyboard|114|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|17|1|", "eventKeyboard|17|0|"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_028_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_033_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_033_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_033_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_menu_press.json", "remote_menu_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|139|1", "eventKeyboard|139|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2067|1|", "eventKeyboard|2067|0|"};
    ci.combKeyList = {{{"1101","OnShowMenu"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_033_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_038_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_038_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_038_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_enter_press.json", "remote_enter_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|28|1", "eventKeyboard|28|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2054|1|", "eventKeyboard|2054|0|"};
    ci.combKeyList = {{{"&&"},{"1110","OnEnterand1102","OnSend"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_038_C");
}

/*
 * Feature: Multi_HandOut_RemoteControl_Reg_App_048_C
 * Function: Multi_HandOut_RemoteControl_Reg_App_048_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolRegCTest, Multi_HandOut_RemoteControl_Reg_App_048_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"remote_Back_press.json", "remote_Back_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|158|1", "eventKeyboard|158|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2|1|", "eventKeyboard|2|0|"};
    ci.combKeyList = {{{"&&"},{"1113","OnBackand4010","OnClosePage"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_HandOut_RemoteControl_Reg_App_048_C");
}
}