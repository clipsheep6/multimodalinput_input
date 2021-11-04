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

class multiHandoutkeyboardlongclickUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutkeyboardlongclickUnRegRTest::st_client_helper_;

void multiHandoutkeyboardlongclickUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutkeyboardlongclickUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutkeyboardlongclickUnRegRTest::SetUp()
{
}

void multiHandoutkeyboardlongclickUnRegRTest::TearDown()
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
std::string multiHandoutkeyboardlongclickUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutkeyboardlongclickUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
        EXPECT_EQ(GetDataProcess().GetActualClientResult_longClick(), ReplaceExpectXY(ExpectClientResult, autoTestCoordinate));
        EXPECT_EQ((ci.dispatcherList[i] == GetDataProcess().GetDispatcherStr()) ||
                  (ci.dispatcherList[i].find(GetDataProcess().GetDispatcherStr()) != ci.dispatcherList[i].npos),
                  true);
        GetDataProcess().ClearAll();
    }
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_001_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_GRAVE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|41|1 & eventKeyboard|41|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2056|1 & eventKeyboard|2056|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_002_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_1_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|2|1 & eventKeyboard|2|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2001|1 & eventKeyboard|2001|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_003_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_003_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_003_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_2_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|3|1 & eventKeyboard|3|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2002|1 & eventKeyboard|2002|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_004_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_004_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_004_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_3_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|4|1 & eventKeyboard|4|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2003|1 & eventKeyboard|2003|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_005_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_005_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_005_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_4_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|5|1 & eventKeyboard|5|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2004|1 & eventKeyboard|2004|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_006_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_5_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|6|1 & eventKeyboard|6|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2005|1 & eventKeyboard|2005|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_007_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_007_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_007_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_6_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|7|1 & eventKeyboard|7|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2006|1 & eventKeyboard|2006|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_008_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_008_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_008_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_7_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|8|1 & eventKeyboard|8|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2007|1 & eventKeyboard|2007|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_009_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_009_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_009_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_8_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|9|1 & eventKeyboard|9|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2008|1 & eventKeyboard|2008|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_010_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_010_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_010_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_9_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|10|1 & eventKeyboard|10|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2009|1 & eventKeyboard|2009|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_011_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_011_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_011_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_0_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|11|1 & eventKeyboard|11|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2000|1 & eventKeyboard|2000|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_012_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_012_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_012_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MINUS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|12|1 & eventKeyboard|12|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2057|1 & eventKeyboard|2057|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_013_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_013_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_013_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EQUALS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|13|1 & eventKeyboard|13|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2058|1 & eventKeyboard|2058|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_014_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_014_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_014_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DEL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|14|1 & eventKeyboard|14|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2055|1 & eventKeyboard|2055|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_015_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_015_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_015_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Q_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|16|1 & eventKeyboard|16|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2033|1 & eventKeyboard|2033|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_016_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_016_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_016_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_W_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|17|1 & eventKeyboard|17|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2039|1 & eventKeyboard|2039|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_017_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_017_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_017_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_E_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|18|1 & eventKeyboard|18|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2021|1 & eventKeyboard|2021|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_018_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_018_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_018_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_R_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|19|1 & eventKeyboard|19|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2034|1 & eventKeyboard|2034|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_019_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_019_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_019_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_T_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|20|1 & eventKeyboard|20|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2036|1 & eventKeyboard|2036|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_020_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_020_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_020_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Y_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|21|1 & eventKeyboard|21|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2041|1 & eventKeyboard|2041|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_021_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_U_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|22|1 & eventKeyboard|22|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2037|1 & eventKeyboard|2037|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_022_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_022_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_022_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_I_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|23|1 & eventKeyboard|23|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2025|1 & eventKeyboard|2025|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_023_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_023_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_023_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_O_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|24|1 & eventKeyboard|24|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2031|1 & eventKeyboard|2031|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_024_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_024_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_024_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_P_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|25|1 & eventKeyboard|25|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2032|1 & eventKeyboard|2032|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_025_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_025_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_025_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LEFT_BRACKET_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|26|1 & eventKeyboard|26|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2059|1 & eventKeyboard|2059|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_026_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RIGHT_BRACKET_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|27|1 & eventKeyboard|27|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2060|1 & eventKeyboard|2060|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_027_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_027_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_027_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CAPS_LOCK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|58|1 & eventKeyboard|58|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2074|1 & eventKeyboard|2074|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_028_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_028_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_028_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_A_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|30|1 & eventKeyboard|30|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2017|1 & eventKeyboard|2017|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_029_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_029_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_029_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_S_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|31|1 & eventKeyboard|31|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2035|1 & eventKeyboard|2035|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_030_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_030_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_030_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_D_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|32|1 & eventKeyboard|32|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2020|1 & eventKeyboard|2020|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_031_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_031_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_031_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|33|1 & eventKeyboard|33|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2022|1 & eventKeyboard|2022|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_032_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_032_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_032_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_G_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|34|1 & eventKeyboard|34|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2023|1 & eventKeyboard|2023|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_033_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_033_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_033_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_H_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|35|1 & eventKeyboard|35|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2024|1 & eventKeyboard|2024|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_034_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_034_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_034_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_J_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|36|1 & eventKeyboard|36|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2026|1 & eventKeyboard|2026|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_035_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_035_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_035_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_K_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|37|1 & eventKeyboard|37|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2027|1 & eventKeyboard|2027|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_036_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_036_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_036_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_L_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|38|1 & eventKeyboard|38|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2028|1 & eventKeyboard|2028|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_037_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_037_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_037_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEMICOLON_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|39|1 & eventKeyboard|39|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2062|1 & eventKeyboard|2062|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_038_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_038_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_038_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_APOSTROPHE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|40|1 & eventKeyboard|40|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2063|1 & eventKeyboard|2063|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_039_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_039_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_039_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BACKSLASH_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|43|1 & eventKeyboard|43|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2061|1 & eventKeyboard|2061|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_040_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_040_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_040_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHIFT_LEFT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|42|1 & eventKeyboard|42|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2047|1 & eventKeyboard|2047|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_041_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_041_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_041_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Z_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|44|1 & eventKeyboard|44|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2042|1 & eventKeyboard|2042|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_042_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_042_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_042_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_X_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|45|1 & eventKeyboard|45|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2040|1 & eventKeyboard|2040|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_043_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_043_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_043_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_C_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|46|1 & eventKeyboard|46|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2019|1 & eventKeyboard|2019|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_044_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_044_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_044_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_V_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|47|1 & eventKeyboard|47|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2038|1 & eventKeyboard|2038|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_045_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_045_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_045_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_B_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|48|1 & eventKeyboard|48|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2018|1 & eventKeyboard|2018|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_046_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_046_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_046_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_N_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|49|1 & eventKeyboard|49|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2030|1 & eventKeyboard|2030|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_047_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_047_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_047_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_M_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|50|1 & eventKeyboard|50|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2029|1 & eventKeyboard|2029|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_048_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_048_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_048_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COMMA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|51|1 & eventKeyboard|51|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2043|1 & eventKeyboard|2043|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_049_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_049_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_049_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PERIOD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|52|1 & eventKeyboard|52|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2044|1 & eventKeyboard|2044|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_050_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_050_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_050_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SLASH_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|53|1 & eventKeyboard|53|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2064|1 & eventKeyboard|2064|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_051_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_051_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_051_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHIFT_RIGHT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|54|1 & eventKeyboard|54|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2048|1 & eventKeyboard|2048|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_052_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_052_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_052_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|29|1 & eventKeyboard|29|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2072|1 & eventKeyboard|2072|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_053_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_053_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_053_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|125|1 & eventKeyboard|125|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2076|1 & eventKeyboard|2076|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_054_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_054_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_054_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_LEFT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|56|1 & eventKeyboard|56|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2045|1 & eventKeyboard|2045|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_055_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_055_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_055_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SPACE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|57|1 & eventKeyboard|57|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2050|1 & eventKeyboard|2050|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_056_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_056_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_056_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_RIGHT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|100|1 & eventKeyboard|100|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2046|1 & eventKeyboard|2046|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_058_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_058_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_058_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_RIGHT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|97|1 & eventKeyboard|97|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2073|1 & eventKeyboard|2073|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_059_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_059_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_059_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F1_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|59|1 & eventKeyboard|59|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2090|1 & eventKeyboard|2090|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_060_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_060_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_060_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F2_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|60|1 & eventKeyboard|60|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2091|1 & eventKeyboard|2091|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_061_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_061_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_061_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F3_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|61|1 & eventKeyboard|61|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2092|1 & eventKeyboard|2092|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_062_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_062_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_062_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F5_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|63|1 & eventKeyboard|63|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2094|1 & eventKeyboard|2094|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_063_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_063_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_063_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F6_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|64|1 & eventKeyboard|64|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2095|1 & eventKeyboard|2095|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_064_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_064_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_064_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F7_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|65|1 & eventKeyboard|65|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2096|1 & eventKeyboard|2096|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_065_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_065_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_065_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F8_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|66|1 & eventKeyboard|66|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2097|1 & eventKeyboard|2097|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_066_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_066_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_066_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F9_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|67|1 & eventKeyboard|67|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2098|1 & eventKeyboard|2098|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_067_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_067_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_067_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F10_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|68|1 & eventKeyboard|68|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2099|1 & eventKeyboard|2099|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_068_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_068_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_068_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F12_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|88|1 & eventKeyboard|88|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2101|1 & eventKeyboard|2101|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_069_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_069_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_069_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SYSRQ_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|99|1 & eventKeyboard|99|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2079|1 & eventKeyboard|2079|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_070_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_070_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_070_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLL_LOCK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|70|1 & eventKeyboard|70|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2075|1 & eventKeyboard|2075|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_071_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_071_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_071_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BREAK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|119|1 & eventKeyboard|119|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2080|1 & eventKeyboard|2080|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_072_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_072_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_072_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_INSERT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|110|1 & eventKeyboard|110|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2083|1 & eventKeyboard|2083|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_073_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_073_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_073_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_HOME_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|102|1 & eventKeyboard|102|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2081|1 & eventKeyboard|2081|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_074_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_074_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_074_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARD_DEL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|111|1 & eventKeyboard|111|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2071|1 & eventKeyboard|2071|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_075_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_075_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_075_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_END_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|107|1 & eventKeyboard|107|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2082|1 & eventKeyboard|2082|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_076_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_076_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_076_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUM_LOCK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|69|1 & eventKeyboard|69|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2102|1 & eventKeyboard|2102|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_077_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_077_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_077_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_DIVIDE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|98|1 & eventKeyboard|98|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2113|1 & eventKeyboard|2113|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_078_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_078_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_078_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_MULTIPLY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|55|1 & eventKeyboard|55|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2114|1 & eventKeyboard|2114|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_079_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_079_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_079_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_SUBTRACT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|74|1 & eventKeyboard|74|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2115|1 & eventKeyboard|2115|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_080_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_080_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_080_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_7_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|71|1 & eventKeyboard|71|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2110|1 & eventKeyboard|2110|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_081_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_081_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_081_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_8_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|72|1 & eventKeyboard|72|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2111|1 & eventKeyboard|2111|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_082_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_082_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_082_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_9_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|73|1 & eventKeyboard|73|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2112|1 & eventKeyboard|2112|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_083_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_083_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_083_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_4_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|75|1 & eventKeyboard|75|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2107|1 & eventKeyboard|2107|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_084_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_084_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_084_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_5_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|76|1 & eventKeyboard|76|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2108|1 & eventKeyboard|2108|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_085_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_085_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_085_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_6_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|77|1 & eventKeyboard|77|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2109|1 & eventKeyboard|2109|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_086_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_086_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_086_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_1_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|79|1 & eventKeyboard|79|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2104|1 & eventKeyboard|2104|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_087_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_087_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_087_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_2_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|80|1 & eventKeyboard|80|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2105|1 & eventKeyboard|2105|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_088_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_088_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_088_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_3_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|81|1 & eventKeyboard|81|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2106|1 & eventKeyboard|2106|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_089_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_089_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_089_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_ADD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|78|1 & eventKeyboard|78|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2116|1 & eventKeyboard|2116|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_090_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_090_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_090_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_0_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|82|1 & eventKeyboard|82|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2103|1 & eventKeyboard|2103|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_091_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_091_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_091_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_DOT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|83|1 & eventKeyboard|83|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2117|1 & eventKeyboard|2117|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_093_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_093_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_093_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ZENKAKUHANKAKU_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|85|1 & eventKeyboard|85|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2601|1 & eventKeyboard|2601|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_094_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_094_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_094_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_102ND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|86|1 & eventKeyboard|86|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2602|1 & eventKeyboard|2602|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_095_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_095_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_095_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|89|1 & eventKeyboard|89|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2603|1 & eventKeyboard|2603|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_096_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_096_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_096_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KATAKANA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|90|1 & eventKeyboard|90|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2604|1 & eventKeyboard|2604|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_097_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_097_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_097_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HIRAGANA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|91|1 & eventKeyboard|91|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2605|1 & eventKeyboard|2605|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_098_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_098_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_098_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HENKAN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|92|1 & eventKeyboard|92|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2606|1 & eventKeyboard|2606|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_099_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_099_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_099_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KATAKANAHIRAGANA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|93|1 & eventKeyboard|93|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2607|1 & eventKeyboard|2607|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_100_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_100_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_100_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MUHENKAN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|94|1 & eventKeyboard|94|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2608|1 & eventKeyboard|2608|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_101_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_101_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_101_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPJPCOMMA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|95|1 & eventKeyboard|95|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2118|1 & eventKeyboard|2118|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_102_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_102_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_102_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LINEFEED_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|101|1 & eventKeyboard|101|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2609|1 & eventKeyboard|2609|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_103_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_103_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_103_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MACRO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|112|1 & eventKeyboard|112|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2610|1 & eventKeyboard|2610|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_104_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_104_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_104_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MUTE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|113|1 & eventKeyboard|113|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|22|1 & eventKeyboard|22|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_105_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_105_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_105_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VOLUMEDOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|114|1 & eventKeyboard|114|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|17|1 & eventKeyboard|17|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_106_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_106_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_106_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VOLUMEUP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|115|1 & eventKeyboard|115|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|16|1 & eventKeyboard|16|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_108_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_108_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_108_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPEQUAL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|117|1 & eventKeyboard|117|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2120|1 & eventKeyboard|2120|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_109_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_109_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_109_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPPLUSMINUS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|118|1 & eventKeyboard|118|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2611|1 & eventKeyboard|2611|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_110_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_110_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_110_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCALE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|120|1 & eventKeyboard|120|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2612|1 & eventKeyboard|2612|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_111_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_111_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_111_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPCOMMA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|121|1 & eventKeyboard|121|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2118|1 & eventKeyboard|2118|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_112_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_112_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_112_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HANGEUL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|122|1 & eventKeyboard|122|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2613|1 & eventKeyboard|2613|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_113_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_113_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_113_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HANJA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|123|1 & eventKeyboard|123|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2614|1 & eventKeyboard|2614|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_114_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_114_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_114_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_YEN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|124|1 & eventKeyboard|124|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2615|1 & eventKeyboard|2615|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_115_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_115_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_115_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RIGHTMETA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|126|1 & eventKeyboard|126|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2077|1 & eventKeyboard|2077|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_116_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_116_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_116_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STOP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|128|1 & eventKeyboard|128|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2616|1 & eventKeyboard|2616|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_117_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_117_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_117_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_AGAIN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|129|1 & eventKeyboard|129|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2617|1 & eventKeyboard|2617|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_118_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_118_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_118_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROPS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|130|1 & eventKeyboard|130|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2618|1 & eventKeyboard|2618|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_119_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_119_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_119_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UNDO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|131|1 & eventKeyboard|131|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2619|1 & eventKeyboard|2619|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_120_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_120_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_120_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FRONT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|132|1 & eventKeyboard|132|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2800|1 & eventKeyboard|2800|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_121_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_121_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_121_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COPY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|133|1 & eventKeyboard|133|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2620|1 & eventKeyboard|2620|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_122_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_122_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_122_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_OPEN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|134|1 & eventKeyboard|134|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2621|1 & eventKeyboard|2621|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_123_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_123_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_123_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PASTE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|135|1 & eventKeyboard|135|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2622|1 & eventKeyboard|2622|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_124_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_124_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_124_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FIND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|136|1 & eventKeyboard|136|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2623|1 & eventKeyboard|2623|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_125_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_125_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_125_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CUT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|137|1 & eventKeyboard|137|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2624|1 & eventKeyboard|2624|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_126_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_126_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_126_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HELP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|138|1 & eventKeyboard|138|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2625|1 & eventKeyboard|2625|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_127_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_127_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_127_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RIGHT_MENU_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|139|1 & eventKeyboard|139|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2067|1 & eventKeyboard|2067|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_128_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_128_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_128_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CALC_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|140|1 & eventKeyboard|140|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2626|1 & eventKeyboard|2626|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_129_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_129_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_129_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SETUP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|141|1 & eventKeyboard|141|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2801|1 & eventKeyboard|2801|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_130_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_130_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_130_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SLEEP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|142|1 & eventKeyboard|142|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2600|1 & eventKeyboard|2600|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_131_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_131_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_131_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WAKEUP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|143|1 & eventKeyboard|143|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2802|1 & eventKeyboard|2802|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_132_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_132_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_132_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FILE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|144|1 & eventKeyboard|144|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2627|1 & eventKeyboard|2627|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_133_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_133_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_133_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SENDFILE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|145|1 & eventKeyboard|145|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2803|1 & eventKeyboard|2803|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_134_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_134_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_134_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DELETEFILE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|146|1 & eventKeyboard|146|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2804|1 & eventKeyboard|2804|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_135_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_135_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_135_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_XFER_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|147|1 & eventKeyboard|147|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2805|1 & eventKeyboard|2805|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_136_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_136_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_136_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG1_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|148|1 & eventKeyboard|148|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2806|1 & eventKeyboard|2806|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_137_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_137_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_137_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG2_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|149|1 & eventKeyboard|149|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2807|1 & eventKeyboard|2807|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_138_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_138_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_138_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WWW_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|150|1 & eventKeyboard|150|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2052|1 & eventKeyboard|2052|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_139_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_139_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_139_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MSDOS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|151|1 & eventKeyboard|151|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2808|1 & eventKeyboard|2808|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_140_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_140_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_140_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCREENLOCK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|152|1 & eventKeyboard|152|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2809|1 & eventKeyboard|2809|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_141_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_141_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_141_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ROTATE_DISPLAY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|153|1 & eventKeyboard|153|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2810|1 & eventKeyboard|2810|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_142_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_142_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_142_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CYCLEWINDOWS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|154|1 & eventKeyboard|154|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2811|1 & eventKeyboard|2811|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_143_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_143_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_143_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MAIL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|155|1 & eventKeyboard|155|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2053|1 & eventKeyboard|2053|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_144_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_144_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_144_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BOOKMARKS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|156|1 & eventKeyboard|156|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2628|1 & eventKeyboard|2628|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_145_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_145_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_145_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COMPUTER_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|157|1 & eventKeyboard|157|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2812|1 & eventKeyboard|2812|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_146_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_146_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_146_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BACK_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|158|1 & eventKeyboard|158|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2|1 & eventKeyboard|2|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_147_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_147_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_147_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|159|1 & eventKeyboard|159|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2084|1 & eventKeyboard|2084|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_148_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_148_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_148_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CLOSECD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|160|1 & eventKeyboard|160|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2087|1 & eventKeyboard|2087|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_149_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_149_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_149_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EJECTCD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|161|1 & eventKeyboard|161|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2088|1 & eventKeyboard|2088|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_150_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_150_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_150_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EJECTCLOSECD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|162|1 & eventKeyboard|162|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2813|1 & eventKeyboard|2813|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_151_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_151_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_151_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NEXTSONG_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|163|1 & eventKeyboard|163|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|12|1 & eventKeyboard|12|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_152_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_152_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_152_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAYPAUSE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|164|1 & eventKeyboard|164|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|10|1 & eventKeyboard|10|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_153_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_153_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_153_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PREVIOUSSONG_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|165|1 & eventKeyboard|165|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|13|1 & eventKeyboard|13|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_154_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_154_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_154_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STOPCD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|166|1 & eventKeyboard|166|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|11|1 & eventKeyboard|11|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_155_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_155_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_155_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RECORD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|167|1 & eventKeyboard|167|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2089|1 & eventKeyboard|2089|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_156_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_156_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_156_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REWIND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|168|1 & eventKeyboard|168|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|14|1 & eventKeyboard|14|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_157_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_157_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_157_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PHONE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|169|1 & eventKeyboard|169|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|3|1 & eventKeyboard|3|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_158_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_158_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_158_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ISO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|170|1 & eventKeyboard|170|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2814|1 & eventKeyboard|2814|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_159_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_159_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_159_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CONFIG_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|171|1 & eventKeyboard|171|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2634|1 & eventKeyboard|2634|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_161_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_161_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_161_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REFRESH_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|173|1 & eventKeyboard|173|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2635|1 & eventKeyboard|2635|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_162_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_162_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_162_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EXIT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|174|1 & eventKeyboard|174|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2636|1 & eventKeyboard|2636|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_163_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_163_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_163_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|175|1 & eventKeyboard|175|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2815|1 & eventKeyboard|2815|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_164_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_164_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_164_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EDIT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|176|1 & eventKeyboard|176|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2637|1 & eventKeyboard|2637|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_165_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_165_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_165_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLLUP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|177|1 & eventKeyboard|177|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2638|1 & eventKeyboard|2638|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_166_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_166_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_166_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLLDOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|178|1 & eventKeyboard|178|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2639|1 & eventKeyboard|2639|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_167_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_167_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_167_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPLEFTPAREN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|179|1 & eventKeyboard|179|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2121|1 & eventKeyboard|2121|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_168_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_168_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_168_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPRIGHTPAREN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|180|1 & eventKeyboard|180|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2122|1 & eventKeyboard|2122|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_169_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_169_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_169_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NEW_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|181|1 & eventKeyboard|181|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2640|1 & eventKeyboard|2640|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_170_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_170_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_170_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REDO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|182|1 & eventKeyboard|182|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2641|1 & eventKeyboard|2641|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_171_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_171_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_171_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F13_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|183|1 & eventKeyboard|183|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2816|1 & eventKeyboard|2816|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_172_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_172_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_172_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F14_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|184|1 & eventKeyboard|184|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2817|1 & eventKeyboard|2817|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_173_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_173_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_173_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F15_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|185|1 & eventKeyboard|185|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2818|1 & eventKeyboard|2818|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_174_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_174_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_174_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F16_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|186|1 & eventKeyboard|186|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2819|1 & eventKeyboard|2819|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_175_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_175_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_175_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F17_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|187|1 & eventKeyboard|187|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2820|1 & eventKeyboard|2820|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_176_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_176_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_176_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F18_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|188|1 & eventKeyboard|188|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2821|1 & eventKeyboard|2821|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_177_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_177_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_177_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F19_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|189|1 & eventKeyboard|189|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2822|1 & eventKeyboard|2822|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_178_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_178_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_178_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F20_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|190|1 & eventKeyboard|190|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2823|1 & eventKeyboard|2823|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_179_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_179_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_179_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F21_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|191|1 & eventKeyboard|191|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2824|1 & eventKeyboard|2824|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_180_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_180_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_180_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F22_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|192|1 & eventKeyboard|192|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2825|1 & eventKeyboard|2825|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_181_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_181_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_181_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F23_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|193|1 & eventKeyboard|193|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2826|1 & eventKeyboard|2826|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_182_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_182_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_182_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F24_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|194|1 & eventKeyboard|194|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2827|1 & eventKeyboard|2827|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_183_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_183_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_183_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAYCD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|200|1 & eventKeyboard|200|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2643|1 & eventKeyboard|2643|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_184_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_184_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_184_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAUSECD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|201|1 & eventKeyboard|201|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2086|1 & eventKeyboard|2086|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_185_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_185_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_185_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG3_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|202|1 & eventKeyboard|202|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2828|1 & eventKeyboard|2828|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_186_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_186_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_186_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG4_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|203|1 & eventKeyboard|203|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2829|1 & eventKeyboard|2829|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_187_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_187_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_187_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DASHBOARD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|204|1 & eventKeyboard|204|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2830|1 & eventKeyboard|2830|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_188_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_188_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_188_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SUSPEND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|205|1 & eventKeyboard|205|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2831|1 & eventKeyboard|2831|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_189_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_189_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_189_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CLOSE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|206|1 & eventKeyboard|206|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2642|1 & eventKeyboard|2642|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_190_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_190_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_190_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|207|1 & eventKeyboard|207|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2085|1 & eventKeyboard|2085|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_191_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_191_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_191_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FASTFORWARD_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|208|1 & eventKeyboard|208|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|15|1 & eventKeyboard|15|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_192_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_192_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_192_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BASSBOOST_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|209|1 & eventKeyboard|209|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2644|1 & eventKeyboard|2644|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_193_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_193_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_193_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PRINT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|210|1 & eventKeyboard|210|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2645|1 & eventKeyboard|2645|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_194_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_194_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_194_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|211|1 & eventKeyboard|211|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2832|1 & eventKeyboard|2832|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_196_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_196_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_196_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SOUND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|213|1 & eventKeyboard|213|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2833|1 & eventKeyboard|2833|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_197_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_197_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_197_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_QUESTION_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|214|1 & eventKeyboard|214|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2834|1 & eventKeyboard|2834|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_198_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_198_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_198_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EMAIL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|215|1 & eventKeyboard|215|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2065|1 & eventKeyboard|2065|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_199_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_199_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_199_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CHAT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|216|1 & eventKeyboard|216|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2646|1 & eventKeyboard|2646|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_200_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_200_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_200_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEARCH_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|217|1 & eventKeyboard|217|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|9|1 & eventKeyboard|9|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_201_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_201_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_201_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CONNECT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|218|1 & eventKeyboard|218|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2836|1 & eventKeyboard|2836|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_202_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_202_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_202_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FINANCE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|219|1 & eventKeyboard|219|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2647|1 & eventKeyboard|2647|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_203_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_203_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_203_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SPORT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|220|1 & eventKeyboard|220|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2837|1 & eventKeyboard|2837|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_204_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_204_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_204_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHOP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|221|1 & eventKeyboard|221|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2838|1 & eventKeyboard|2838|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_205_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_205_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_205_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALTERASE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|222|1 & eventKeyboard|222|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2839|1 & eventKeyboard|2839|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_206_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_206_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_206_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CANCEL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|223|1 & eventKeyboard|223|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2648|1 & eventKeyboard|2648|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_209_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_209_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_209_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MEDIA_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|226|1 & eventKeyboard|226|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|6|1 & eventKeyboard|6|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_210_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_210_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_210_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SWITCHVIDEOMODE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|227|1 & eventKeyboard|227|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2841|1 & eventKeyboard|2841|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_211_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_211_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_211_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMTOGGLE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|228|1 & eventKeyboard|228|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2649|1 & eventKeyboard|2649|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_212_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_212_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_212_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMDOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|229|1 & eventKeyboard|229|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2650|1 & eventKeyboard|2650|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_213_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_213_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_213_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMUP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|230|1 & eventKeyboard|230|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2651|1 & eventKeyboard|2651|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_214_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_214_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_214_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|231|1 & eventKeyboard|231|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2652|1 & eventKeyboard|2652|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_215_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_215_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_215_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REPLY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|232|1 & eventKeyboard|232|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2653|1 & eventKeyboard|2653|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_216_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_216_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_216_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARDMAIL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|233|1 & eventKeyboard|233|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2654|1 & eventKeyboard|2654|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_217_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_217_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_217_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SAVE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|234|1 & eventKeyboard|234|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2655|1 & eventKeyboard|2655|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_218_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_218_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_218_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DOCUMENTS_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|235|1 & eventKeyboard|235|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2656|1 & eventKeyboard|2656|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_219_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_219_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_219_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BATTERY_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|236|1 & eventKeyboard|236|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2842|1 & eventKeyboard|2842|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_220_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_220_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_220_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BLUETOOTH_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|237|1 & eventKeyboard|237|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2843|1 & eventKeyboard|2843|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_221_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_221_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_221_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WLAN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|238|1 & eventKeyboard|238|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2844|1 & eventKeyboard|2844|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_222_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_222_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_222_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UWB_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|239|1 & eventKeyboard|239|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2845|1 & eventKeyboard|2845|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_223_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_223_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_223_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UNKNOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|240|1 & eventKeyboard|240|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|-1|1 & eventKeyboard|-1|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_224_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_224_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_224_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VIDEO_NEXT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|241|1 & eventKeyboard|241|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2657|1 & eventKeyboard|2657|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_225_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_225_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_225_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VIDEO_PREV_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|242|1 & eventKeyboard|242|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2658|1 & eventKeyboard|2658|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_226_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_226_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_226_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BRIGHTNESS_CYCLE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|243|1 & eventKeyboard|243|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2659|1 & eventKeyboard|2659|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_227_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_227_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_227_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BRIGHTNESS_AUTO_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|244|1 & eventKeyboard|244|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2660|1 & eventKeyboard|2660|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_228_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_228_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_228_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DISPLAY_OFF_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|245|1 & eventKeyboard|245|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2661|1 & eventKeyboard|2661|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_229_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_229_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_229_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WWAN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|246|1 & eventKeyboard|246|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2846|1 & eventKeyboard|2846|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_230_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_230_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_230_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RFKILL_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|247|1 & eventKeyboard|247|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2847|1 & eventKeyboard|2847|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_231_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_231_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_231_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MICMUTE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|248|1 & eventKeyboard|248|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|23|1 & eventKeyboard|23|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_232_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_232_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_232_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_UP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|104|1 & eventKeyboard|104|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2068|1 & eventKeyboard|2068|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_233_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_233_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_233_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_DOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|109|1 & eventKeyboard|109|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2069|1 & eventKeyboard|2069|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_234_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_234_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_234_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_UP_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|103|1 & eventKeyboard|103|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2012|1 & eventKeyboard|2012|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_235_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_235_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_235_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_DOWN_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|108|1 & eventKeyboard|108|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2013|1 & eventKeyboard|2013|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_236_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_236_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_236_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_LEFT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|105|1 & eventKeyboard|105|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2014|1 & eventKeyboard|2014|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_237_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_237_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_237_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_RIGHT_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|106|1 & eventKeyboard|106|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2015|1 & eventKeyboard|2015|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_239_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_239_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_239_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LEFT_MENU_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|127|1 & eventKeyboard|127|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2067|1 & eventKeyboard|2067|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_240_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_240_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_240_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ENTER_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|28|1 & eventKeyboard|28|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2054|1 & eventKeyboard|2054|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_241_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_241_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_241_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ESCAPE_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|1|1 & eventKeyboard|1|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2070|1 & eventKeyboard|2070|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_242_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_242_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_242_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_TAB_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|15|1 & eventKeyboard|15|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2049|1 & eventKeyboard|2049|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_243_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_243_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_243_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FUNCTION_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|464|1 & eventKeyboard|464|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2078|1 & eventKeyboard|2078|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_244_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_244_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_244_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STAR_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|522|1 & eventKeyboard|522|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2010|1 & eventKeyboard|2010|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_LP_245_R
 * Function: Multi_HandOut_KeyBoard_UnReg_LP_245_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardlongclickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_LP_245_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_POUND_longpress.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventKeyboard|523|1 & eventKeyboard|523|0"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventKeyboard|2011|1 & eventKeyboard|2011|0|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}
}