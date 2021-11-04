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

class multiHandouttrackpadRegBTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void) {};
    
    void SetUp() override;
    void TearDown() override;
    
    void DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum);
private:
    void ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList, const size_t i);
};

void multiHandouttrackpadRegBTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_B");
    sleep(1);
}

void multiHandouttrackpadRegBTest::SetUp()
{
}

void multiHandouttrackpadRegBTest::TearDown()
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
void multiHandouttrackpadRegBTest::ExpectCombKey(const std::vector<std::vector<std::vector<std::string>>> &combKeyList,
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
void multiHandouttrackpadRegBTest::DoCompareAndExpect(const struct CaseInfo &ci, std::string caseNum)
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
 * Feature: Multi_HandOut_TrackPad_Reg_App_021_B
 * Function: Multi_HandOut_TrackPad_Reg_App_021_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadRegBTest, Multi_HandOut_TrackPad_Reg_App_021_B, TestSize.Level0)
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
    ci.eventList = {"trackpad_twofingers_up_move.json"};
    ci.flagList = {"1"};
    ci.deviceMsg = {""};
    ci.libinputExpectList = {"eventPointer|-30|0"};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {<combKeyList>};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_TrackPad_Reg_App_021_B");
}
/*
 * Feature: Multi_HandOut_TrackPad_Reg_App_026_B
 * Function: Multi_HandOut_TrackPad_Reg_App_026_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadRegBTest_Test, Multi_HandOut_TrackPad_Reg_App_026_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"trackpad_twofingers_down_move.json"};
    ci.flagList = {"1"};
    ci.deviceMsg = {""};
    ci.libinputExpectList = {"eventPointer|30|0"};
    ci.standardExpectList = {"/"};
    ci.combKeyList = {{{"1112","OnNext"}}};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci, "Multi_HandOut_TrackPad_Reg_App_026_B");
}
/*
 * Feature: Multi_HandOut_TrackPad_Reg_App_031_B
 * Function: Multi_HandOut_TrackPad_Reg_App_031_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadRegBTest_Test, Multi_HandOut_TrackPad_Reg_App_031_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"trackpad_btn_right_press.json", "trackpad_btn_right_release.json"};
    ci.flagList = {"1", ""};
    ci.deviceMsg = {"", ""};
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/"};
    ci.combKeyList = {{{"1101","OnShowMenu"}}, {{""}}};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_TrackPad_Reg_App_031_B");
}
/*
 * Feature: Multi_HandOut_TrackPad_Reg_App_036_B
 * Function: Multi_HandOut_TrackPad_Reg_App_036_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadRegBTest_Test, Multi_HandOut_TrackPad_Reg_App_036_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json",
                    "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json",
                    "trackpad_twofingers_down_move.json", "trackpad_twofingers_down_move.json"};
    ci.flagList = {"1", "1", "1", "1", "1", "1", "1", "1", "1", "1"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", ""};
    ci.libinputExpectList = {"eventPointer|30|0", "eventPointer|30|0", "eventPointer|30|0",
                             "eventPointer|30|0", "eventPointer|30|0", "eventPointer|30|0",
                             "eventPointer|30|0", "eventPointer|30|0", "eventPointer|30|0",
                             "eventPointer|30|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/"};
    ci.combKeyList = {{{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}, {{"1112","OnNext"}}};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_TrackPad_Reg_App_036_B");
}
/*
 * Feature: Multi_HandOut_TrackPad_Reg_App_041_B
 * Function: Multi_HandOut_TrackPad_Reg_App_041_B
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadRegBTest_Test, Multi_HandOut_TrackPad_Reg_App_041_B, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {{{""}}}, {""}};
    ci.eventList = {"trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json",
                    "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json",
                    "trackpad_twofingers_up_move.json", "trackpad_twofingers_up_move.json"};
    ci.flagList = {"1", "1", "1", "1", "1", "1", "1", "1", "1", "1"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", ""};
    ci.libinputExpectList = {"eventPointer|-30|0", "eventPointer|-30|0", "eventPointer|-30|0",
                             "eventPointer|-30|0", "eventPointer|-30|0", "eventPointer|-30|0",
                             "eventPointer|-30|0", "eventPointer|-30|0", "eventPointer|-30|0",
                             "eventPointer|-30|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/"};
    ci.combKeyList = {{{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}, {{"1111","OnPrevious"}}};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci, "Multi_HandOut_TrackPad_Reg_App_041_B");
}
}