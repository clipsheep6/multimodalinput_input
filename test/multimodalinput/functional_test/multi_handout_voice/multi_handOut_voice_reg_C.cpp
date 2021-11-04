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

class multiHandoutvoiceRegCTest : public testing::Test {
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

StClientHelper multiHandoutvoiceRegCTest::st_client_helper_;

void multiHandoutvoiceRegCTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_C");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutvoiceRegCTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutvoiceRegCTest::SetUp()
{
}

void multiHandoutvoiceRegCTest::TearDown()
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
void multiHandoutvoiceRegCTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
std::string multiHandoutvoiceRegCTest::ReplaceExpectXY(std::string str,
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
void multiHandoutvoiceRegCTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_HandOut_Voice_Reg_APP_003_C
 * Function: Multi_HandOut_Voice_Reg_APP_003_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_003_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"3002 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"3002","OnPause"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_003_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_008_C
 * Function: Multi_HandOut_Voice_Reg_APP_008_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_008_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"3001 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"3001","OnPlay"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_008_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_013_C
 * Function: Multi_HandOut_Voice_Reg_APP_013_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_013_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"5001 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"5001","OnAnswer"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_013_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_018_C
 * Function: Multi_HandOut_Voice_Reg_APP_018_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_018_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"5002 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"5002","OnRefuse"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_018_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_023_C
 * Function: Multi_HandOut_Voice_Reg_APP_023_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_023_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"5003 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"5003","OnHangup"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_023_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_028_C
 * Function: Multi_HandOut_Voice_Reg_APP_028_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_028_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1111 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1111","OnPrevious"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_028_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_033_C
 * Function: Multi_HandOut_Voice_Reg_APP_033_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_033_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1112 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1112","OnNext"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_033_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_038_C
 * Function: Multi_HandOut_Voice_Reg_APP_038_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_038_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1107 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1107","OnRefresh"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_038_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_043_C
 * Function: Multi_HandOut_Voice_Reg_APP_043_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_043_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1103 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1103","OnCopy"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_043_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_048_C
 * Function: Multi_HandOut_Voice_Reg_APP_048_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_048_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1104 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1104","OnPaste"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_048_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_053_C
 * Function: Multi_HandOut_Voice_Reg_APP_053_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_053_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1105 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1105","OnCut"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_053_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_058_C
 * Function: Multi_HandOut_Voice_Reg_APP_058_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_058_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1106 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1106","OnUndo"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_058_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_063_C
 * Function: Multi_HandOut_Voice_Reg_APP_063_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_063_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1101 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1101","OnShowMenu"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_063_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_068_C
 * Function: Multi_HandOut_Voice_Reg_APP_068_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_068_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1110 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1110","OnEnter"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_068_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_073_C
 * Function: Multi_HandOut_Voice_Reg_APP_073_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_073_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1109 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1109","OnCancel"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_073_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_078_C
 * Function: Multi_HandOut_Voice_Reg_APP_078_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_078_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1102 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1102","OnSend"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_078_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_083_C
 * Function: Multi_HandOut_Voice_Reg_APP_083_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_083_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1114 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1114","OnPrint"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_083_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_APP_088_C
 * Function: Multi_HandOut_Voice_Reg_APP_088_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_APP_088_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"1113 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"1113","OnBack"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_APP_088_C");
}

/*
 * Feature: Multi_HandOut_Voice_Reg_Unusual_001_C
 * Function: Multi_HandOut_Voice_Reg_Unusual_001_C
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutvoiceRegCTest, Multi_HandOut_Voice_Reg_Unusual_001_C, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"3002 1"};
    ci.deviceMsg = {"Voice"};
    ci.libinputExpectList = {""};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"|"};
    ci.combKeyList = {{{"3002","OnPause"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_Voice_Reg_Unusual_001_C");
}
}