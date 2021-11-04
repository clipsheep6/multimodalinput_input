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

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;

struct CaseInfo {
    std::vector<std::string> eventList = {""};
    std::vector<std::string> flagList = {""};
    std::vector<std::string> deviceMsg = {""};
    std::vector<std::string> libinputExpectList = {""};
    std::vector<std::string> standardExpectList = {""};
    std::vector<std::vector<std::vector<std::string>>> combKeyList = {{{""}}};
    std::vector<std::string> dispatcherList = {""};
};

class multiHandoutvoiceRegBTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void) {};
    
    void SetUp() override;
    void TearDown() override;
    
    void DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum);
private:
    void ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList, const size_t i);
};

void multiHandoutvoiceRegBTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_B");
    sleep(1);
}

void multiHandoutvoiceRegBTest::SetUp()
{
}

void multiHandoutvoiceRegBTest::TearDown()
{
    GetDataProcess().ClearAll();
}

/*
 * Feature: ExpectCombKey
 * Function: ExpectCombKey
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
void multiHandoutvoiceRegBTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
 * Feature: DoCompareAndExpect
 * Function: DoCompareAndExpect
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
void multiHandoutvoiceRegBTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
{
    for (unsigned int i = 0; i < ci.eventList.size(); i++) {
        GetSTHelper().InJectionEvent(ci.eventList[i], ci.deviceMsg[i]);
        if (caseNum.find("_Voice_") == caseNum.npos && caseNum.find("_Phalangeal_") == caseNum.npos) {
            std::string ExpectServerResult = "NULL|NULL|NULL |";
            std::string ExpectClientResult = "NULL|NULL|NULL |";
            EXPECT_EQ(GetDataProcess().GetLibinputValue(), ci.libinputExpectList[i]);
            EXPECT_EQ(GetDataProcess().GetStandardResult(), ci.standardExpectList[i]);
            if (ci.libinputExpectList[i] != "NULL|NULL|NULL " && ci.flagList[i] == "1") {
                ExpectServerResult += GetSTHelper().valueServer;
                ExpectClientResult += GetSTHelper().valueClient;
            }
            EXPECT_EQ(GetDataProcess().GetActualServerResult(), ExpectServerResult);
            EXPECT_EQ(GetDataProcess().GetActualClientResult(), ExpectClientResult);
            EXPECT_EQ((ci.dispatcherList[i] == GetDataProcess().GetDispatcherStr())
                     || (ci.dispatcherList[i].find(GetDataProcess().GetDispatcherStr())!= ci.dispatcherList[i].npos),
                    true);
        }
        ExpectCombKey(ci.combKeyList, i);
        //GetDataProcess().ClearAll();
    }
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_001_B
 * Function: Multi_HandOut_Voice_Reg_APP_001_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest, Multi_HandOut_Voice_Reg_APP_001_B, TestSize.Level0)
{
    std::vector<ClientList> clientList = GetDataProcess().GetClientList();
    if (clientList.size() > 0) {
        GetSTHelper().ReplaceServerValue(std::regex("fd1"), std::to_string(clientList[0].fd));
        GetSTHelper().ReplaceServerValue(std::regex("surfaceId1"), std::to_string(clientList[0].surfaceId));
        GetSTHelper().ReplaceServerValue(std::regex("abilityId1"), std::to_string(clientList[0].abilityId));
        GetSTHelper().ReplaceServerValue(std::regex("fd1"), std::to_string(clientList[0].fd));
        GetSTHelper().ReplaceServerValue(std::regex("surfaceId1"), std::to_string(clientList[0].surfaceId));
        GetSTHelper().ReplaceServerValue(std::regex("abilityId1"), std::to_string(clientList[0].abilityId));
    }	
	struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"30021"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {<combKeyList>};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_001_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_006_B
 * Function: Multi_HandOut_Voice_Reg_APP_006_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_006_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"30011"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"3001","OnPlay"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_006_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_011_B
 * Function: Multi_HandOut_Voice_Reg_APP_011_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_011_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50011"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"5001","OnAnswer"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_011_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_016_B
 * Function: Multi_HandOut_Voice_Reg_APP_016_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_016_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50021"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"5002","OnRefuse"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_016_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_021_B
 * Function: Multi_HandOut_Voice_Reg_APP_021_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_021_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"50031"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"5003","OnHangup"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_021_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_026_B
 * Function: Multi_HandOut_Voice_Reg_APP_026_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_026_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11111"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1111","OnPrevious"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_026_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_031_B
 * Function: Multi_HandOut_Voice_Reg_APP_031_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_031_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11121"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1112","OnNext"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_031_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_036_B
 * Function: Multi_HandOut_Voice_Reg_APP_036_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_036_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11071"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1107","OnRefresh"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_036_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_041_B
 * Function: Multi_HandOut_Voice_Reg_APP_041_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_041_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11031"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1103","OnCopy"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_041_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_046_B
 * Function: Multi_HandOut_Voice_Reg_APP_046_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_046_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11041"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1104","OnPaste"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_046_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_051_B
 * Function: Multi_HandOut_Voice_Reg_APP_051_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_051_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11051"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1105","OnCut"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_051_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_056_B
 * Function: Multi_HandOut_Voice_Reg_APP_056_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_056_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11061"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1106","OnUndo"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_056_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_061_B
 * Function: Multi_HandOut_Voice_Reg_APP_061_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_061_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11011"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1101","OnShowMenu"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_061_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_066_B
 * Function: Multi_HandOut_Voice_Reg_APP_066_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_066_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11101"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1110","OnEnter"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_066_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_071_B
 * Function: Multi_HandOut_Voice_Reg_APP_071_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_071_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11091"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1109","OnCancel"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_071_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_076_B
 * Function: Multi_HandOut_Voice_Reg_APP_076_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_076_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11021"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1102","OnSend"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_076_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_081_B
 * Function: Multi_HandOut_Voice_Reg_APP_081_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_081_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11141"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1114","OnPrint"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_081_B");
}
/*
 * Feature: Multi_HandOut_Voice_Reg_APP_086_B
 * Function: Multi_HandOut_Voice_Reg_APP_086_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegBTest_Test, Multi_HandOut_Voice_Reg_APP_086_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"11131"};
    ci.flagList = {"1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1113","OnBack"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_086_B");
}
}