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

class multiHandouttrackpadoldUnRegHTest : public testing::Test {
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

StClientHelper multiHandouttrackpadoldUnRegHTest::st_client_helper_;

void multiHandouttrackpadoldUnRegHTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_H");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandouttrackpadoldUnRegHTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandouttrackpadoldUnRegHTest::SetUp()
{
}

void multiHandouttrackpadoldUnRegHTest::TearDown()
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
std::string multiHandouttrackpadoldUnRegHTest::ReplaceExpectXY(std::string str,
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
void multiHandouttrackpadoldUnRegHTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_TrackPad_Reg_067_H
 * Function: Multi_HandOut_TrackPad_Reg_067_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_067_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|1|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_068_H
 * Function: Multi_HandOut_TrackPad_Reg_068_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_068_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|272|0", "eventPointer|272|1",
                             "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|1|0|", "eventPointer|1|1|",
                        "eventPointer|1|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_069_H
 * Function: Multi_HandOut_TrackPad_Reg_069_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_069_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|1|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_092_H
 * Function: Multi_HandOut_TrackPad_Reg_092_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_092_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|272|0", "eventPointer|272|1",
                             "eventPointer|272|0", "eventPointer|272|1", "eventPointer|272|0",
                             "eventPointer|272|1", "eventPointer|272|0", "eventPointer|272|1",
                             "eventPointer|272|0", "eventPointer|272|1", "eventPointer|272|0",
                             "eventPointer|272|1", "eventPointer|272|0", "eventPointer|272|1",
                             "eventPointer|272|0", "eventPointer|272|1", "eventPointer|272|0",
                             "eventPointer|272|1", "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0",
                             "402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0",
                             "402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0",
                             "402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0",
                             "402|500|272|1|0|0", "402|501|272|0|0|0", "402|500|272|1|0|0", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|1|0|", "eventPointer|1|1|",
                        "eventPointer|1|0|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventPointer|1|1|", "eventPointer|1|0|", "eventPointer|1|1|",
                        "eventPointer|1|0|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventPointer|1|1|", "eventPointer|1|0|", "eventPointer|1|1|",
                        "eventPointer|1|0|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventPointer|1|1|", "eventPointer|1|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_095_H
 * Function: Multi_HandOut_TrackPad_Reg_095_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_095_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {""};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|-35|40"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|-35|40|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_096_H
 * Function: Multi_HandOut_TrackPad_Reg_096_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_096_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {""};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|350|40"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|350|40|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_097_H
 * Function: Multi_HandOut_TrackPad_Reg_097_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_097_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {""};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|-3|540"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|-3|540|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_105_H
 * Function: Multi_HandOut_TrackPad_Reg_105_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_105_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {""};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|350|40"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|350|40|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_106_H
 * Function: Multi_HandOut_TrackPad_Reg_106_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_106_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventPointer|-35|-38", "eventPointer|-35|-38", "eventPointer|-35|-38",
                             "eventPointer|-35|-38", "eventPointer|-35|-38", "eventPointer|-35|-38"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventPointer|-35|-38|", "eventPointer|-35|-38|", "eventPointer|-35|-38|",
                        "eventPointer|-35|-38|", "eventPointer|-35|-38|", "eventPointer|-35|-38|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_107_H
 * Function: Multi_HandOut_TrackPad_Reg_107_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_107_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventPointer|-35|38", "eventPointer|-35|38", "eventPointer|-35|38",
                             "eventPointer|-35|38", "eventPointer|-35|38", "eventPointer|-35|38"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventPointer|-35|38|", "eventPointer|-35|38|", "eventPointer|-35|38|",
                        "eventPointer|-35|38|", "eventPointer|-35|38|", "eventPointer|-35|38|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_108_H
 * Function: Multi_HandOut_TrackPad_Reg_108_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_108_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventPointer|35|-38", "eventPointer|35|-38", "eventPointer|35|-38",
                             "eventPointer|35|-38", "eventPointer|35|-38", "eventPointer|35|-38"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventPointer|35|-38|", "eventPointer|35|-38|", "eventPointer|35|-38|",
                        "eventPointer|35|-38|", "eventPointer|35|-38|", "eventPointer|35|-38|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_109_H
 * Function: Multi_HandOut_TrackPad_Reg_109_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_109_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventPointer|35|38", "eventPointer|35|38", "eventPointer|35|38",
                             "eventPointer|35|38", "eventPointer|35|38", "eventPointer|35|38"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventPointer|35|38|", "eventPointer|35|38|", "eventPointer|35|38|",
                        "eventPointer|35|38|", "eventPointer|35|38|", "eventPointer|35|38|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_164_H
 * Function: Multi_HandOut_TrackPad_Reg_164_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_164_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_174_H
 * Function: Multi_HandOut_TrackPad_Reg_174_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_174_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_184_H
 * Function: Multi_HandOut_TrackPad_Reg_184_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_184_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_194_H
 * Function: Multi_HandOut_TrackPad_Reg_194_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_194_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_204_H
 * Function: Multi_HandOut_TrackPad_Reg_204_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_204_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|-35|40", "eventPointer|-15|60",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|-35|40|", "eventPointer|-15|60|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_224_H
 * Function: Multi_HandOut_TrackPad_Reg_224_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_224_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_225_H
 * Function: Multi_HandOut_TrackPad_Reg_225_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_225_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_226_H
 * Function: Multi_HandOut_TrackPad_Reg_226_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_226_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", "",
                    "", "", "", ""};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_TrackPad_Reg_227_H
 * Function: Multi_HandOut_TrackPad_Reg_227_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackpadoldUnRegHTest, Multi_HandOut_TrackPad_Reg_227_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", ""};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}
}