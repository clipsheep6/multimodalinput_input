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

class multiHandoutsystemRegCTest : public testing::Test {
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

StClientHelper multiHandoutsystemRegCTest::st_client_helper_;

void multiHandoutsystemRegCTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_C");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutsystemRegCTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutsystemRegCTest::SetUp()
{
}

void multiHandoutsystemRegCTest::TearDown()
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
void multiHandoutsystemRegCTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
std::string multiHandoutsystemRegCTest::ReplaceExpectXY(std::string str,
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
void multiHandoutsystemRegCTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_System_press_001	_C
 * Function: Multi_System_press_001	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_001	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_116_press.json", "systemkey_116_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|116|1", "eventKeyboard|116|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|18|1|", "eventKeyboard|18|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_001	_C");
}

/*
 * Feature: Multi_System_press_008	_C
 * Function: Multi_System_press_008	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_008	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_115_press.json", "systemkey_115_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|115|1", "eventKeyboard|115|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|16|1|", "eventKeyboard|16|0|"};
    ci.combKeyList = {{{"1112","onNext	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_008	_C");
}

/*
 * Feature: Multi_System_press_011	_C
 * Function: Multi_System_press_011	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_011	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_114_press.json", "systemkey_114_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|114|1", "eventKeyboard|114|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|17|1|", "eventKeyboard|17|0|"};
    ci.combKeyList = {{{"1111","onPrevious"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_011	_C");
}

/*
 * Feature: Multi_System_press_014	_C
 * Function: Multi_System_press_014	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_014	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_158_press.json", "systemkey_158_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|158|1", "eventKeyboard|158|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2|1|", "eventKeyboard|2|0|"};
    ci.combKeyList = {{{"&&"},{"4010","onClosePageand1113","onBack"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_014	_C");
}

/*
 * Feature: Multi_System_press_015	_C
 * Function: Multi_System_press_015	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_015	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_139_press.json", "systemkey_139_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|139|1", "eventKeyboard|139|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2067|1|", "eventKeyboard|2067|0|"};
    ci.combKeyList = {{{"1101","onShowMenu"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_015	_C");
}

/*
 * Feature: Multi_System_press_016	_C
 * Function: Multi_System_press_016	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_016	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_172_press.json", "systemkey_172_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|172|1", "eventKeyboard|172|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|1|1|", "eventKeyboard|1|0|"};
    ci.combKeyList = {{{""}}, {{""}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_016	_C");
}

/*
 * Feature: Multi_System_press_017	_C
 * Function: Multi_System_press_017	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_017	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_169_press.json", "systemkey_169_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|169|1", "eventKeyboard|169|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|3|1|", "eventKeyboard|3|0|"};
    ci.combKeyList = {{{""}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_017	_C");
}

/*
 * Feature: Multi_System_press_018	_C
 * Function: Multi_System_press_018	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_018	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_744_press.json", "systemkey_744_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|744|1", "eventKeyboard|744|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|744|1|", "eventKeyboard|744|0|"};
    ci.combKeyList = {{{""}}, {{""}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_018	_C");
}

/*
 * Feature: Multi_System_press_019	_C
 * Function: Multi_System_press_019	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_019	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_113_press.json", "systemkey_113_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|113|1", "eventKeyboard|113|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|22|1|", "eventKeyboard|22|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_019	_C");
}

/*
 * Feature: Multi_System_press_020	_C
 * Function: Multi_System_press_020	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_020	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_248_press.json", "systemkey_248_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|248|1", "eventKeyboard|248|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|23|1|", "eventKeyboard|23|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_020	_C");
}

/*
 * Feature: Multi_System_press_021	_C
 * Function: Multi_System_press_021	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_021	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_226_press.json", "systemkey_226_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|226|1", "eventKeyboard|226|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|6|1|", "eventKeyboard|6|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_021	_C");
}

/*
 * Feature: Multi_System_press_022	_C
 * Function: Multi_System_press_022	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_022	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_207_press.json", "systemkey_207_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|207|1", "eventKeyboard|207|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2085|1|", "eventKeyboard|2085|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_022	_C");
}

/*
 * Feature: Multi_System_press_023	_C
 * Function: Multi_System_press_023	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_023	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_201_press.json", "systemkey_201_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|201|1", "eventKeyboard|201|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2086|1|", "eventKeyboard|2086|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_023	_C");
}

/*
 * Feature: Multi_System_press_024	_C
 * Function: Multi_System_press_024	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_024	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_164_press.json", "systemkey_164_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|164|1", "eventKeyboard|164|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|10|1|", "eventKeyboard|10|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_024	_C");
}

/*
 * Feature: Multi_System_press_025	_C
 * Function: Multi_System_press_025	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_025	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_166_press.json", "systemkey_166_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|166|1", "eventKeyboard|166|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|11|1|", "eventKeyboard|11|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_025	_C");
}

/*
 * Feature: Multi_System_press_026	_C
 * Function: Multi_System_press_026	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_026	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_163_press.json", "systemkey_163_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|163|1", "eventKeyboard|163|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|12|1|", "eventKeyboard|12|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_026	_C");
}

/*
 * Feature: Multi_System_press_027	_C
 * Function: Multi_System_press_027	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_027	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_165_press.json", "systemkey_165_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|165|1", "eventKeyboard|165|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|13|1|", "eventKeyboard|13|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_027	_C");
}

/*
 * Feature: Multi_System_press_028	_C
 * Function: Multi_System_press_028	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_028	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_168_press.json", "systemkey_168_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|168|1", "eventKeyboard|168|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|14|1|", "eventKeyboard|14|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_028	_C");
}

/*
 * Feature: Multi_System_press_029	_C
 * Function: Multi_System_press_029	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_029	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_167_press.json", "systemkey_167_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|167|1", "eventKeyboard|167|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2089|1|", "eventKeyboard|2089|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_029	_C");
}

/*
 * Feature: Multi_System_press_030	_C
 * Function: Multi_System_press_030	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_030	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_208_press.json", "systemkey_208_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|208|1", "eventKeyboard|208|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|15|1|", "eventKeyboard|15|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_030	_C");
}

/*
 * Feature: Multi_System_press_031	_C
 * Function: Multi_System_press_031	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_031	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_212_press.json", "systemkey_212_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|212|1", "eventKeyboard|212|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|19|1|", "eventKeyboard|19|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|0|1", "|0|0|1"};
    DoCompareAndExpect(ci, "Multi_System_press_031	_C");
}

/*
 * Feature: Multi_System_press_032	_C
 * Function: Multi_System_press_032	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_032	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_528_press.json", "systemkey_528_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|528|1", "eventKeyboard|528|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|7|1|", "eventKeyboard|7|0|"};
    ci.combKeyList = {{{"	"}}, {{""}}};
    ci.dispatcherList = {"|0|0|1", "|0|0|1"};
    DoCompareAndExpect(ci, "Multi_System_press_032	_C");
}

/*
 * Feature: Multi_System_press_033	_C
 * Function: Multi_System_press_033	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_033	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_217_press.json", "systemkey_217_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|217|1", "eventKeyboard|217|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|9|1|", "eventKeyboard|9|0|"};
    ci.combKeyList = {{{"4009","onSearch"}}, {{""}}};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_033	_C");
}

/*
 * Feature: Multi_System_press_034	_C
 * Function: Multi_System_press_034	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_034	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_224_press.json", "systemkey_224_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|224|1", "eventKeyboard|224|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|41|1|", "eventKeyboard|41|0|"};
    ci.combKeyList = {{{""}}, {{""}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_034	_C");
}

/*
 * Feature: Multi_System_press_035	_C
 * Function: Multi_System_press_035	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_035	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_225_press.json", "systemkey_225_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|225|1", "eventKeyboard|225|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|40|1|", "eventKeyboard|40|0|"};
    ci.combKeyList = {{{""}}, {{"	"}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_035	_C");
}

/*
 * Feature: Multi_System_press_036	_C
 * Function: Multi_System_press_036	_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutsystemRegCTest, Multi_System_press_036	_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"systemkey_582_press.json", "systemkey_582_release.json"};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventKeyboard|582|1", "eventKeyboard|582|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|20|1|", "eventKeyboard|20|0|"};
    ci.combKeyList = {{{""}}, {{"	"}}};
    ci.dispatcherList = {"|1|0|0", "|1|0|0"};
    DoCompareAndExpect(ci, "Multi_System_press_036	_C");
}
}