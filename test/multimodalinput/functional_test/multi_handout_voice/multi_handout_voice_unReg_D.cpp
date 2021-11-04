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

class multiHandoutvoiceUnRegDTest : public testing::Test {
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

StClientHelper multiHandoutvoiceUnRegDTest::st_client_helper_;

void multiHandoutvoiceUnRegDTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_D");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutvoiceUnRegDTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutvoiceUnRegDTest::SetUp()
{
}

void multiHandoutvoiceUnRegDTest::TearDown()
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
void multiHandoutvoiceUnRegDTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
std::string multiHandoutvoiceUnRegDTest::ReplaceExpectXY(std::string str,
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
void multiHandoutvoiceUnRegDTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_HandOut_Voice_Reg_APP_002_D
 * Function: Multi_HandOut_Voice_Reg_APP_002_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_002_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"30021"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_002_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_007_D
 * Function: Multi_HandOut_Voice_Reg_APP_007_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_007_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"30011"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_007_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_012_D
 * Function: Multi_HandOut_Voice_Reg_APP_012_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_012_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50011"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_012_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_017_D
 * Function: Multi_HandOut_Voice_Reg_APP_017_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_017_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50021"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_017_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_022_D
 * Function: Multi_HandOut_Voice_Reg_APP_022_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_022_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50031"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_022_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_027_D
 * Function: Multi_HandOut_Voice_Reg_APP_027_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_027_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11111"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_027_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_032_D
 * Function: Multi_HandOut_Voice_Reg_APP_032_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_032_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11121"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_032_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_037_D
 * Function: Multi_HandOut_Voice_Reg_APP_037_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_037_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11071"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_037_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_042_D
 * Function: Multi_HandOut_Voice_Reg_APP_042_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_042_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11031"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_042_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_047_D
 * Function: Multi_HandOut_Voice_Reg_APP_047_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_047_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11041"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_047_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_052_D
 * Function: Multi_HandOut_Voice_Reg_APP_052_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_052_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11051"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_052_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_057_D
 * Function: Multi_HandOut_Voice_Reg_APP_057_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_057_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11061"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_057_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_062_D
 * Function: Multi_HandOut_Voice_Reg_APP_062_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_062_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11011"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_062_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_067_D
 * Function: Multi_HandOut_Voice_Reg_APP_067_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_067_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11101"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_067_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_072_D
 * Function: Multi_HandOut_Voice_Reg_APP_072_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_072_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11091"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_072_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_077_D
 * Function: Multi_HandOut_Voice_Reg_APP_077_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_077_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11021"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_077_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_082_D
 * Function: Multi_HandOut_Voice_Reg_APP_082_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_082_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11141"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_082_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_087_D
 * Function: Multi_HandOut_Voice_Reg_APP_087_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_APP_087_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11131"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_087_D");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_Unusual_002_D
 * Function: Multi_HandOut_Voice_Reg_Unusual_002_D
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceUnRegDTest, Multi_HandOut_Voice_Reg_Unusual_002_D, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"aaaa1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{""}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_Unusual_002_D");
}
}