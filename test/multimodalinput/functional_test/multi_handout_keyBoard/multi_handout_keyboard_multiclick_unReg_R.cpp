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

class multiHandoutkeyboardmulticlickUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutkeyboardmulticlickUnRegRTest::st_client_helper_;

void multiHandoutkeyboardmulticlickUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutkeyboardmulticlickUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutkeyboardmulticlickUnRegRTest::SetUp()
{
}

void multiHandoutkeyboardmulticlickUnRegRTest::TearDown()
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
std::string multiHandoutkeyboardmulticlickUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutkeyboardmulticlickUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_001_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json", "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json",
                    "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json", "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json",
                    "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json", "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json",
                    "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json", "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json",
                    "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json", "keyboard_KEY_GRAVE_press.json", "keyboard_KEY_GRAVE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|41|1", "eventKeyboard|41|0", "eventKeyboard|41|1",
                             "eventKeyboard|41|0", "eventKeyboard|41|1", "eventKeyboard|41|0",
                             "eventKeyboard|41|1", "eventKeyboard|41|0", "eventKeyboard|41|1",
                             "eventKeyboard|41|0", "eventKeyboard|41|1", "eventKeyboard|41|0",
                             "eventKeyboard|41|1", "eventKeyboard|41|0", "eventKeyboard|41|1",
                             "eventKeyboard|41|0", "eventKeyboard|41|1", "eventKeyboard|41|0",
                             "eventKeyboard|41|1", "eventKeyboard|41|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2056|1|", "eventKeyboard|2056|0|", "eventKeyboard|2056|1|",
                        "eventKeyboard|2056|0|", "eventKeyboard|2056|1|", "eventKeyboard|2056|0|",
                        "eventKeyboard|2056|1|", "eventKeyboard|2056|0|", "eventKeyboard|2056|1|",
                        "eventKeyboard|2056|0|", "eventKeyboard|2056|1|", "eventKeyboard|2056|0|",
                        "eventKeyboard|2056|1|", "eventKeyboard|2056|0|", "eventKeyboard|2056|1|",
                        "eventKeyboard|2056|0|", "eventKeyboard|2056|1|", "eventKeyboard|2056|0|",
                        "eventKeyboard|2056|1|", "eventKeyboard|2056|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_002_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json", "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json",
                    "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json", "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json",
                    "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json", "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json",
                    "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json", "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json",
                    "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json", "keyboard_KEY_1_press.json", "keyboard_KEY_1_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|2|1", "eventKeyboard|2|0", "eventKeyboard|2|1",
                             "eventKeyboard|2|0", "eventKeyboard|2|1", "eventKeyboard|2|0",
                             "eventKeyboard|2|1", "eventKeyboard|2|0", "eventKeyboard|2|1",
                             "eventKeyboard|2|0", "eventKeyboard|2|1", "eventKeyboard|2|0",
                             "eventKeyboard|2|1", "eventKeyboard|2|0", "eventKeyboard|2|1",
                             "eventKeyboard|2|0", "eventKeyboard|2|1", "eventKeyboard|2|0",
                             "eventKeyboard|2|1", "eventKeyboard|2|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2001|1|", "eventKeyboard|2001|0|", "eventKeyboard|2001|1|",
                        "eventKeyboard|2001|0|", "eventKeyboard|2001|1|", "eventKeyboard|2001|0|",
                        "eventKeyboard|2001|1|", "eventKeyboard|2001|0|", "eventKeyboard|2001|1|",
                        "eventKeyboard|2001|0|", "eventKeyboard|2001|1|", "eventKeyboard|2001|0|",
                        "eventKeyboard|2001|1|", "eventKeyboard|2001|0|", "eventKeyboard|2001|1|",
                        "eventKeyboard|2001|0|", "eventKeyboard|2001|1|", "eventKeyboard|2001|0|",
                        "eventKeyboard|2001|1|", "eventKeyboard|2001|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_003_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_003_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_003_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json", "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json",
                    "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json", "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json",
                    "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json", "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json",
                    "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json", "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json",
                    "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json", "keyboard_KEY_2_press.json", "keyboard_KEY_2_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|3|1", "eventKeyboard|3|0", "eventKeyboard|3|1",
                             "eventKeyboard|3|0", "eventKeyboard|3|1", "eventKeyboard|3|0",
                             "eventKeyboard|3|1", "eventKeyboard|3|0", "eventKeyboard|3|1",
                             "eventKeyboard|3|0", "eventKeyboard|3|1", "eventKeyboard|3|0",
                             "eventKeyboard|3|1", "eventKeyboard|3|0", "eventKeyboard|3|1",
                             "eventKeyboard|3|0", "eventKeyboard|3|1", "eventKeyboard|3|0",
                             "eventKeyboard|3|1", "eventKeyboard|3|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2002|1|", "eventKeyboard|2002|0|", "eventKeyboard|2002|1|",
                        "eventKeyboard|2002|0|", "eventKeyboard|2002|1|", "eventKeyboard|2002|0|",
                        "eventKeyboard|2002|1|", "eventKeyboard|2002|0|", "eventKeyboard|2002|1|",
                        "eventKeyboard|2002|0|", "eventKeyboard|2002|1|", "eventKeyboard|2002|0|",
                        "eventKeyboard|2002|1|", "eventKeyboard|2002|0|", "eventKeyboard|2002|1|",
                        "eventKeyboard|2002|0|", "eventKeyboard|2002|1|", "eventKeyboard|2002|0|",
                        "eventKeyboard|2002|1|", "eventKeyboard|2002|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_004_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_004_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_004_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json", "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json",
                    "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json", "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json",
                    "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json", "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json",
                    "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json", "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json",
                    "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json", "keyboard_KEY_3_press.json", "keyboard_KEY_3_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|4|1", "eventKeyboard|4|0", "eventKeyboard|4|1",
                             "eventKeyboard|4|0", "eventKeyboard|4|1", "eventKeyboard|4|0",
                             "eventKeyboard|4|1", "eventKeyboard|4|0", "eventKeyboard|4|1",
                             "eventKeyboard|4|0", "eventKeyboard|4|1", "eventKeyboard|4|0",
                             "eventKeyboard|4|1", "eventKeyboard|4|0", "eventKeyboard|4|1",
                             "eventKeyboard|4|0", "eventKeyboard|4|1", "eventKeyboard|4|0",
                             "eventKeyboard|4|1", "eventKeyboard|4|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2003|1|", "eventKeyboard|2003|0|", "eventKeyboard|2003|1|",
                        "eventKeyboard|2003|0|", "eventKeyboard|2003|1|", "eventKeyboard|2003|0|",
                        "eventKeyboard|2003|1|", "eventKeyboard|2003|0|", "eventKeyboard|2003|1|",
                        "eventKeyboard|2003|0|", "eventKeyboard|2003|1|", "eventKeyboard|2003|0|",
                        "eventKeyboard|2003|1|", "eventKeyboard|2003|0|", "eventKeyboard|2003|1|",
                        "eventKeyboard|2003|0|", "eventKeyboard|2003|1|", "eventKeyboard|2003|0|",
                        "eventKeyboard|2003|1|", "eventKeyboard|2003|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_006_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json", "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json",
                    "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json", "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json",
                    "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json", "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json",
                    "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json", "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json",
                    "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json", "keyboard_KEY_5_press.json", "keyboard_KEY_5_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|6|1", "eventKeyboard|6|0", "eventKeyboard|6|1",
                             "eventKeyboard|6|0", "eventKeyboard|6|1", "eventKeyboard|6|0",
                             "eventKeyboard|6|1", "eventKeyboard|6|0", "eventKeyboard|6|1",
                             "eventKeyboard|6|0", "eventKeyboard|6|1", "eventKeyboard|6|0",
                             "eventKeyboard|6|1", "eventKeyboard|6|0", "eventKeyboard|6|1",
                             "eventKeyboard|6|0", "eventKeyboard|6|1", "eventKeyboard|6|0",
                             "eventKeyboard|6|1", "eventKeyboard|6|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2005|1|", "eventKeyboard|2005|0|", "eventKeyboard|2005|1|",
                        "eventKeyboard|2005|0|", "eventKeyboard|2005|1|", "eventKeyboard|2005|0|",
                        "eventKeyboard|2005|1|", "eventKeyboard|2005|0|", "eventKeyboard|2005|1|",
                        "eventKeyboard|2005|0|", "eventKeyboard|2005|1|", "eventKeyboard|2005|0|",
                        "eventKeyboard|2005|1|", "eventKeyboard|2005|0|", "eventKeyboard|2005|1|",
                        "eventKeyboard|2005|0|", "eventKeyboard|2005|1|", "eventKeyboard|2005|0|",
                        "eventKeyboard|2005|1|", "eventKeyboard|2005|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_007_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_007_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_007_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json", "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json",
                    "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json", "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json",
                    "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json", "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json",
                    "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json", "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json",
                    "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json", "keyboard_KEY_6_press.json", "keyboard_KEY_6_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|7|1", "eventKeyboard|7|0", "eventKeyboard|7|1",
                             "eventKeyboard|7|0", "eventKeyboard|7|1", "eventKeyboard|7|0",
                             "eventKeyboard|7|1", "eventKeyboard|7|0", "eventKeyboard|7|1",
                             "eventKeyboard|7|0", "eventKeyboard|7|1", "eventKeyboard|7|0",
                             "eventKeyboard|7|1", "eventKeyboard|7|0", "eventKeyboard|7|1",
                             "eventKeyboard|7|0", "eventKeyboard|7|1", "eventKeyboard|7|0",
                             "eventKeyboard|7|1", "eventKeyboard|7|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2006|1|", "eventKeyboard|2006|0|", "eventKeyboard|2006|1|",
                        "eventKeyboard|2006|0|", "eventKeyboard|2006|1|", "eventKeyboard|2006|0|",
                        "eventKeyboard|2006|1|", "eventKeyboard|2006|0|", "eventKeyboard|2006|1|",
                        "eventKeyboard|2006|0|", "eventKeyboard|2006|1|", "eventKeyboard|2006|0|",
                        "eventKeyboard|2006|1|", "eventKeyboard|2006|0|", "eventKeyboard|2006|1|",
                        "eventKeyboard|2006|0|", "eventKeyboard|2006|1|", "eventKeyboard|2006|0|",
                        "eventKeyboard|2006|1|", "eventKeyboard|2006|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_008_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_008_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_008_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json", "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json",
                    "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json", "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json",
                    "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json", "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json",
                    "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json", "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json",
                    "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json", "keyboard_KEY_7_press.json", "keyboard_KEY_7_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|8|1", "eventKeyboard|8|0", "eventKeyboard|8|1",
                             "eventKeyboard|8|0", "eventKeyboard|8|1", "eventKeyboard|8|0",
                             "eventKeyboard|8|1", "eventKeyboard|8|0", "eventKeyboard|8|1",
                             "eventKeyboard|8|0", "eventKeyboard|8|1", "eventKeyboard|8|0",
                             "eventKeyboard|8|1", "eventKeyboard|8|0", "eventKeyboard|8|1",
                             "eventKeyboard|8|0", "eventKeyboard|8|1", "eventKeyboard|8|0",
                             "eventKeyboard|8|1", "eventKeyboard|8|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2007|1|", "eventKeyboard|2007|0|", "eventKeyboard|2007|1|",
                        "eventKeyboard|2007|0|", "eventKeyboard|2007|1|", "eventKeyboard|2007|0|",
                        "eventKeyboard|2007|1|", "eventKeyboard|2007|0|", "eventKeyboard|2007|1|",
                        "eventKeyboard|2007|0|", "eventKeyboard|2007|1|", "eventKeyboard|2007|0|",
                        "eventKeyboard|2007|1|", "eventKeyboard|2007|0|", "eventKeyboard|2007|1|",
                        "eventKeyboard|2007|0|", "eventKeyboard|2007|1|", "eventKeyboard|2007|0|",
                        "eventKeyboard|2007|1|", "eventKeyboard|2007|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_009_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_009_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_009_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json", "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json",
                    "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json", "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json",
                    "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json", "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json",
                    "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json", "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json",
                    "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json", "keyboard_KEY_8_press.json", "keyboard_KEY_8_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|9|1", "eventKeyboard|9|0", "eventKeyboard|9|1",
                             "eventKeyboard|9|0", "eventKeyboard|9|1", "eventKeyboard|9|0",
                             "eventKeyboard|9|1", "eventKeyboard|9|0", "eventKeyboard|9|1",
                             "eventKeyboard|9|0", "eventKeyboard|9|1", "eventKeyboard|9|0",
                             "eventKeyboard|9|1", "eventKeyboard|9|0", "eventKeyboard|9|1",
                             "eventKeyboard|9|0", "eventKeyboard|9|1", "eventKeyboard|9|0",
                             "eventKeyboard|9|1", "eventKeyboard|9|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2008|1|", "eventKeyboard|2008|0|", "eventKeyboard|2008|1|",
                        "eventKeyboard|2008|0|", "eventKeyboard|2008|1|", "eventKeyboard|2008|0|",
                        "eventKeyboard|2008|1|", "eventKeyboard|2008|0|", "eventKeyboard|2008|1|",
                        "eventKeyboard|2008|0|", "eventKeyboard|2008|1|", "eventKeyboard|2008|0|",
                        "eventKeyboard|2008|1|", "eventKeyboard|2008|0|", "eventKeyboard|2008|1|",
                        "eventKeyboard|2008|0|", "eventKeyboard|2008|1|", "eventKeyboard|2008|0|",
                        "eventKeyboard|2008|1|", "eventKeyboard|2008|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_010_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_010_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_010_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json", "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json",
                    "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json", "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json",
                    "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json", "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json",
                    "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json", "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json",
                    "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json", "keyboard_KEY_9_press.json", "keyboard_KEY_9_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|10|1", "eventKeyboard|10|0", "eventKeyboard|10|1",
                             "eventKeyboard|10|0", "eventKeyboard|10|1", "eventKeyboard|10|0",
                             "eventKeyboard|10|1", "eventKeyboard|10|0", "eventKeyboard|10|1",
                             "eventKeyboard|10|0", "eventKeyboard|10|1", "eventKeyboard|10|0",
                             "eventKeyboard|10|1", "eventKeyboard|10|0", "eventKeyboard|10|1",
                             "eventKeyboard|10|0", "eventKeyboard|10|1", "eventKeyboard|10|0",
                             "eventKeyboard|10|1", "eventKeyboard|10|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2009|1|", "eventKeyboard|2009|0|", "eventKeyboard|2009|1|",
                        "eventKeyboard|2009|0|", "eventKeyboard|2009|1|", "eventKeyboard|2009|0|",
                        "eventKeyboard|2009|1|", "eventKeyboard|2009|0|", "eventKeyboard|2009|1|",
                        "eventKeyboard|2009|0|", "eventKeyboard|2009|1|", "eventKeyboard|2009|0|",
                        "eventKeyboard|2009|1|", "eventKeyboard|2009|0|", "eventKeyboard|2009|1|",
                        "eventKeyboard|2009|0|", "eventKeyboard|2009|1|", "eventKeyboard|2009|0|",
                        "eventKeyboard|2009|1|", "eventKeyboard|2009|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_011_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_011_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_011_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json", "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json",
                    "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json", "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json",
                    "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json", "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json",
                    "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json", "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json",
                    "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json", "keyboard_KEY_0_press.json", "keyboard_KEY_0_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|11|1", "eventKeyboard|11|0", "eventKeyboard|11|1",
                             "eventKeyboard|11|0", "eventKeyboard|11|1", "eventKeyboard|11|0",
                             "eventKeyboard|11|1", "eventKeyboard|11|0", "eventKeyboard|11|1",
                             "eventKeyboard|11|0", "eventKeyboard|11|1", "eventKeyboard|11|0",
                             "eventKeyboard|11|1", "eventKeyboard|11|0", "eventKeyboard|11|1",
                             "eventKeyboard|11|0", "eventKeyboard|11|1", "eventKeyboard|11|0",
                             "eventKeyboard|11|1", "eventKeyboard|11|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2000|1|", "eventKeyboard|2000|0|", "eventKeyboard|2000|1|",
                        "eventKeyboard|2000|0|", "eventKeyboard|2000|1|", "eventKeyboard|2000|0|",
                        "eventKeyboard|2000|1|", "eventKeyboard|2000|0|", "eventKeyboard|2000|1|",
                        "eventKeyboard|2000|0|", "eventKeyboard|2000|1|", "eventKeyboard|2000|0|",
                        "eventKeyboard|2000|1|", "eventKeyboard|2000|0|", "eventKeyboard|2000|1|",
                        "eventKeyboard|2000|0|", "eventKeyboard|2000|1|", "eventKeyboard|2000|0|",
                        "eventKeyboard|2000|1|", "eventKeyboard|2000|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_012_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_012_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_012_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json", "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json",
                    "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json", "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json",
                    "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json", "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json",
                    "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json", "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json",
                    "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json", "keyboard_KEY_MINUS_press.json", "keyboard_KEY_MINUS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|12|1", "eventKeyboard|12|0", "eventKeyboard|12|1",
                             "eventKeyboard|12|0", "eventKeyboard|12|1", "eventKeyboard|12|0",
                             "eventKeyboard|12|1", "eventKeyboard|12|0", "eventKeyboard|12|1",
                             "eventKeyboard|12|0", "eventKeyboard|12|1", "eventKeyboard|12|0",
                             "eventKeyboard|12|1", "eventKeyboard|12|0", "eventKeyboard|12|1",
                             "eventKeyboard|12|0", "eventKeyboard|12|1", "eventKeyboard|12|0",
                             "eventKeyboard|12|1", "eventKeyboard|12|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2057|1|", "eventKeyboard|2057|0|", "eventKeyboard|2057|1|",
                        "eventKeyboard|2057|0|", "eventKeyboard|2057|1|", "eventKeyboard|2057|0|",
                        "eventKeyboard|2057|1|", "eventKeyboard|2057|0|", "eventKeyboard|2057|1|",
                        "eventKeyboard|2057|0|", "eventKeyboard|2057|1|", "eventKeyboard|2057|0|",
                        "eventKeyboard|2057|1|", "eventKeyboard|2057|0|", "eventKeyboard|2057|1|",
                        "eventKeyboard|2057|0|", "eventKeyboard|2057|1|", "eventKeyboard|2057|0|",
                        "eventKeyboard|2057|1|", "eventKeyboard|2057|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_013_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_013_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_013_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json", "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json",
                    "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json", "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json",
                    "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json", "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json",
                    "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json", "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json",
                    "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json", "keyboard_KEY_EQUALS_press.json", "keyboard_KEY_EQUALS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|13|1", "eventKeyboard|13|0", "eventKeyboard|13|1",
                             "eventKeyboard|13|0", "eventKeyboard|13|1", "eventKeyboard|13|0",
                             "eventKeyboard|13|1", "eventKeyboard|13|0", "eventKeyboard|13|1",
                             "eventKeyboard|13|0", "eventKeyboard|13|1", "eventKeyboard|13|0",
                             "eventKeyboard|13|1", "eventKeyboard|13|0", "eventKeyboard|13|1",
                             "eventKeyboard|13|0", "eventKeyboard|13|1", "eventKeyboard|13|0",
                             "eventKeyboard|13|1", "eventKeyboard|13|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2058|1|", "eventKeyboard|2058|0|", "eventKeyboard|2058|1|",
                        "eventKeyboard|2058|0|", "eventKeyboard|2058|1|", "eventKeyboard|2058|0|",
                        "eventKeyboard|2058|1|", "eventKeyboard|2058|0|", "eventKeyboard|2058|1|",
                        "eventKeyboard|2058|0|", "eventKeyboard|2058|1|", "eventKeyboard|2058|0|",
                        "eventKeyboard|2058|1|", "eventKeyboard|2058|0|", "eventKeyboard|2058|1|",
                        "eventKeyboard|2058|0|", "eventKeyboard|2058|1|", "eventKeyboard|2058|0|",
                        "eventKeyboard|2058|1|", "eventKeyboard|2058|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_014_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_014_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_014_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json", "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json",
                    "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json", "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json",
                    "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json", "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json",
                    "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json", "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json",
                    "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json", "keyboard_KEY_DEL_press.json", "keyboard_KEY_DEL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|14|1", "eventKeyboard|14|0", "eventKeyboard|14|1",
                             "eventKeyboard|14|0", "eventKeyboard|14|1", "eventKeyboard|14|0",
                             "eventKeyboard|14|1", "eventKeyboard|14|0", "eventKeyboard|14|1",
                             "eventKeyboard|14|0", "eventKeyboard|14|1", "eventKeyboard|14|0",
                             "eventKeyboard|14|1", "eventKeyboard|14|0", "eventKeyboard|14|1",
                             "eventKeyboard|14|0", "eventKeyboard|14|1", "eventKeyboard|14|0",
                             "eventKeyboard|14|1", "eventKeyboard|14|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2055|1|", "eventKeyboard|2055|0|", "eventKeyboard|2055|1|",
                        "eventKeyboard|2055|0|", "eventKeyboard|2055|1|", "eventKeyboard|2055|0|",
                        "eventKeyboard|2055|1|", "eventKeyboard|2055|0|", "eventKeyboard|2055|1|",
                        "eventKeyboard|2055|0|", "eventKeyboard|2055|1|", "eventKeyboard|2055|0|",
                        "eventKeyboard|2055|1|", "eventKeyboard|2055|0|", "eventKeyboard|2055|1|",
                        "eventKeyboard|2055|0|", "eventKeyboard|2055|1|", "eventKeyboard|2055|0|",
                        "eventKeyboard|2055|1|", "eventKeyboard|2055|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_015_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_015_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_015_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json", "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json",
                    "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json", "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json",
                    "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json", "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json",
                    "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json", "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json",
                    "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json", "keyboard_KEY_Q_press.json", "keyboard_KEY_Q_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|16|1", "eventKeyboard|16|0", "eventKeyboard|16|1",
                             "eventKeyboard|16|0", "eventKeyboard|16|1", "eventKeyboard|16|0",
                             "eventKeyboard|16|1", "eventKeyboard|16|0", "eventKeyboard|16|1",
                             "eventKeyboard|16|0", "eventKeyboard|16|1", "eventKeyboard|16|0",
                             "eventKeyboard|16|1", "eventKeyboard|16|0", "eventKeyboard|16|1",
                             "eventKeyboard|16|0", "eventKeyboard|16|1", "eventKeyboard|16|0",
                             "eventKeyboard|16|1", "eventKeyboard|16|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2033|1|", "eventKeyboard|2033|0|", "eventKeyboard|2033|1|",
                        "eventKeyboard|2033|0|", "eventKeyboard|2033|1|", "eventKeyboard|2033|0|",
                        "eventKeyboard|2033|1|", "eventKeyboard|2033|0|", "eventKeyboard|2033|1|",
                        "eventKeyboard|2033|0|", "eventKeyboard|2033|1|", "eventKeyboard|2033|0|",
                        "eventKeyboard|2033|1|", "eventKeyboard|2033|0|", "eventKeyboard|2033|1|",
                        "eventKeyboard|2033|0|", "eventKeyboard|2033|1|", "eventKeyboard|2033|0|",
                        "eventKeyboard|2033|1|", "eventKeyboard|2033|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_016_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_016_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_016_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json",
                    "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json",
                    "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json",
                    "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json",
                    "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json", "keyboard_KEY_W_press.json", "keyboard_KEY_W_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|17|1", "eventKeyboard|17|0", "eventKeyboard|17|1",
                             "eventKeyboard|17|0", "eventKeyboard|17|1", "eventKeyboard|17|0",
                             "eventKeyboard|17|1", "eventKeyboard|17|0", "eventKeyboard|17|1",
                             "eventKeyboard|17|0", "eventKeyboard|17|1", "eventKeyboard|17|0",
                             "eventKeyboard|17|1", "eventKeyboard|17|0", "eventKeyboard|17|1",
                             "eventKeyboard|17|0", "eventKeyboard|17|1", "eventKeyboard|17|0",
                             "eventKeyboard|17|1", "eventKeyboard|17|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2039|1|", "eventKeyboard|2039|0|", "eventKeyboard|2039|1|",
                        "eventKeyboard|2039|0|", "eventKeyboard|2039|1|", "eventKeyboard|2039|0|",
                        "eventKeyboard|2039|1|", "eventKeyboard|2039|0|", "eventKeyboard|2039|1|",
                        "eventKeyboard|2039|0|", "eventKeyboard|2039|1|", "eventKeyboard|2039|0|",
                        "eventKeyboard|2039|1|", "eventKeyboard|2039|0|", "eventKeyboard|2039|1|",
                        "eventKeyboard|2039|0|", "eventKeyboard|2039|1|", "eventKeyboard|2039|0|",
                        "eventKeyboard|2039|1|", "eventKeyboard|2039|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_017_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_017_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_017_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json", "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json",
                    "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json", "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json",
                    "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json", "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json",
                    "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json", "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json",
                    "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json", "keyboard_KEY_E_press.json", "keyboard_KEY_E_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|18|1", "eventKeyboard|18|0", "eventKeyboard|18|1",
                             "eventKeyboard|18|0", "eventKeyboard|18|1", "eventKeyboard|18|0",
                             "eventKeyboard|18|1", "eventKeyboard|18|0", "eventKeyboard|18|1",
                             "eventKeyboard|18|0", "eventKeyboard|18|1", "eventKeyboard|18|0",
                             "eventKeyboard|18|1", "eventKeyboard|18|0", "eventKeyboard|18|1",
                             "eventKeyboard|18|0", "eventKeyboard|18|1", "eventKeyboard|18|0",
                             "eventKeyboard|18|1", "eventKeyboard|18|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2021|1|", "eventKeyboard|2021|0|", "eventKeyboard|2021|1|",
                        "eventKeyboard|2021|0|", "eventKeyboard|2021|1|", "eventKeyboard|2021|0|",
                        "eventKeyboard|2021|1|", "eventKeyboard|2021|0|", "eventKeyboard|2021|1|",
                        "eventKeyboard|2021|0|", "eventKeyboard|2021|1|", "eventKeyboard|2021|0|",
                        "eventKeyboard|2021|1|", "eventKeyboard|2021|0|", "eventKeyboard|2021|1|",
                        "eventKeyboard|2021|0|", "eventKeyboard|2021|1|", "eventKeyboard|2021|0|",
                        "eventKeyboard|2021|1|", "eventKeyboard|2021|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_018_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_018_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_018_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json",
                    "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json",
                    "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json",
                    "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json",
                    "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json", "keyboard_KEY_R_press.json", "keyboard_KEY_R_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|19|1", "eventKeyboard|19|0", "eventKeyboard|19|1",
                             "eventKeyboard|19|0", "eventKeyboard|19|1", "eventKeyboard|19|0",
                             "eventKeyboard|19|1", "eventKeyboard|19|0", "eventKeyboard|19|1",
                             "eventKeyboard|19|0", "eventKeyboard|19|1", "eventKeyboard|19|0",
                             "eventKeyboard|19|1", "eventKeyboard|19|0", "eventKeyboard|19|1",
                             "eventKeyboard|19|0", "eventKeyboard|19|1", "eventKeyboard|19|0",
                             "eventKeyboard|19|1", "eventKeyboard|19|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2034|1|", "eventKeyboard|2034|0|", "eventKeyboard|2034|1|",
                        "eventKeyboard|2034|0|", "eventKeyboard|2034|1|", "eventKeyboard|2034|0|",
                        "eventKeyboard|2034|1|", "eventKeyboard|2034|0|", "eventKeyboard|2034|1|",
                        "eventKeyboard|2034|0|", "eventKeyboard|2034|1|", "eventKeyboard|2034|0|",
                        "eventKeyboard|2034|1|", "eventKeyboard|2034|0|", "eventKeyboard|2034|1|",
                        "eventKeyboard|2034|0|", "eventKeyboard|2034|1|", "eventKeyboard|2034|0|",
                        "eventKeyboard|2034|1|", "eventKeyboard|2034|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_019_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_019_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_019_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json", "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json",
                    "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json", "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json",
                    "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json", "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json",
                    "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json", "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json",
                    "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json", "keyboard_KEY_T_press.json", "keyboard_KEY_T_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|20|1", "eventKeyboard|20|0", "eventKeyboard|20|1",
                             "eventKeyboard|20|0", "eventKeyboard|20|1", "eventKeyboard|20|0",
                             "eventKeyboard|20|1", "eventKeyboard|20|0", "eventKeyboard|20|1",
                             "eventKeyboard|20|0", "eventKeyboard|20|1", "eventKeyboard|20|0",
                             "eventKeyboard|20|1", "eventKeyboard|20|0", "eventKeyboard|20|1",
                             "eventKeyboard|20|0", "eventKeyboard|20|1", "eventKeyboard|20|0",
                             "eventKeyboard|20|1", "eventKeyboard|20|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2036|1|", "eventKeyboard|2036|0|", "eventKeyboard|2036|1|",
                        "eventKeyboard|2036|0|", "eventKeyboard|2036|1|", "eventKeyboard|2036|0|",
                        "eventKeyboard|2036|1|", "eventKeyboard|2036|0|", "eventKeyboard|2036|1|",
                        "eventKeyboard|2036|0|", "eventKeyboard|2036|1|", "eventKeyboard|2036|0|",
                        "eventKeyboard|2036|1|", "eventKeyboard|2036|0|", "eventKeyboard|2036|1|",
                        "eventKeyboard|2036|0|", "eventKeyboard|2036|1|", "eventKeyboard|2036|0|",
                        "eventKeyboard|2036|1|", "eventKeyboard|2036|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_020_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_020_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_020_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json", "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json",
                    "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json", "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json",
                    "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json", "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json",
                    "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json", "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json",
                    "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json", "keyboard_KEY_Y_press.json", "keyboard_KEY_Y_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|21|1", "eventKeyboard|21|0", "eventKeyboard|21|1",
                             "eventKeyboard|21|0", "eventKeyboard|21|1", "eventKeyboard|21|0",
                             "eventKeyboard|21|1", "eventKeyboard|21|0", "eventKeyboard|21|1",
                             "eventKeyboard|21|0", "eventKeyboard|21|1", "eventKeyboard|21|0",
                             "eventKeyboard|21|1", "eventKeyboard|21|0", "eventKeyboard|21|1",
                             "eventKeyboard|21|0", "eventKeyboard|21|1", "eventKeyboard|21|0",
                             "eventKeyboard|21|1", "eventKeyboard|21|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2041|1|", "eventKeyboard|2041|0|", "eventKeyboard|2041|1|",
                        "eventKeyboard|2041|0|", "eventKeyboard|2041|1|", "eventKeyboard|2041|0|",
                        "eventKeyboard|2041|1|", "eventKeyboard|2041|0|", "eventKeyboard|2041|1|",
                        "eventKeyboard|2041|0|", "eventKeyboard|2041|1|", "eventKeyboard|2041|0|",
                        "eventKeyboard|2041|1|", "eventKeyboard|2041|0|", "eventKeyboard|2041|1|",
                        "eventKeyboard|2041|0|", "eventKeyboard|2041|1|", "eventKeyboard|2041|0|",
                        "eventKeyboard|2041|1|", "eventKeyboard|2041|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_021_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json", "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json",
                    "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json", "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json",
                    "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json", "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json",
                    "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json", "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json",
                    "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json", "keyboard_KEY_U_press.json", "keyboard_KEY_U_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|22|1", "eventKeyboard|22|0", "eventKeyboard|22|1",
                             "eventKeyboard|22|0", "eventKeyboard|22|1", "eventKeyboard|22|0",
                             "eventKeyboard|22|1", "eventKeyboard|22|0", "eventKeyboard|22|1",
                             "eventKeyboard|22|0", "eventKeyboard|22|1", "eventKeyboard|22|0",
                             "eventKeyboard|22|1", "eventKeyboard|22|0", "eventKeyboard|22|1",
                             "eventKeyboard|22|0", "eventKeyboard|22|1", "eventKeyboard|22|0",
                             "eventKeyboard|22|1", "eventKeyboard|22|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2037|1|", "eventKeyboard|2037|0|", "eventKeyboard|2037|1|",
                        "eventKeyboard|2037|0|", "eventKeyboard|2037|1|", "eventKeyboard|2037|0|",
                        "eventKeyboard|2037|1|", "eventKeyboard|2037|0|", "eventKeyboard|2037|1|",
                        "eventKeyboard|2037|0|", "eventKeyboard|2037|1|", "eventKeyboard|2037|0|",
                        "eventKeyboard|2037|1|", "eventKeyboard|2037|0|", "eventKeyboard|2037|1|",
                        "eventKeyboard|2037|0|", "eventKeyboard|2037|1|", "eventKeyboard|2037|0|",
                        "eventKeyboard|2037|1|", "eventKeyboard|2037|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_022_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_022_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_022_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json", "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json",
                    "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json", "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json",
                    "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json", "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json",
                    "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json", "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json",
                    "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json", "keyboard_KEY_I_press.json", "keyboard_KEY_I_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|23|1", "eventKeyboard|23|0", "eventKeyboard|23|1",
                             "eventKeyboard|23|0", "eventKeyboard|23|1", "eventKeyboard|23|0",
                             "eventKeyboard|23|1", "eventKeyboard|23|0", "eventKeyboard|23|1",
                             "eventKeyboard|23|0", "eventKeyboard|23|1", "eventKeyboard|23|0",
                             "eventKeyboard|23|1", "eventKeyboard|23|0", "eventKeyboard|23|1",
                             "eventKeyboard|23|0", "eventKeyboard|23|1", "eventKeyboard|23|0",
                             "eventKeyboard|23|1", "eventKeyboard|23|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2025|1|", "eventKeyboard|2025|0|", "eventKeyboard|2025|1|",
                        "eventKeyboard|2025|0|", "eventKeyboard|2025|1|", "eventKeyboard|2025|0|",
                        "eventKeyboard|2025|1|", "eventKeyboard|2025|0|", "eventKeyboard|2025|1|",
                        "eventKeyboard|2025|0|", "eventKeyboard|2025|1|", "eventKeyboard|2025|0|",
                        "eventKeyboard|2025|1|", "eventKeyboard|2025|0|", "eventKeyboard|2025|1|",
                        "eventKeyboard|2025|0|", "eventKeyboard|2025|1|", "eventKeyboard|2025|0|",
                        "eventKeyboard|2025|1|", "eventKeyboard|2025|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_023_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_023_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_023_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json", "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json",
                    "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json", "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json",
                    "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json", "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json",
                    "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json", "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json",
                    "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json", "keyboard_KEY_O_press.json", "keyboard_KEY_O_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|24|1", "eventKeyboard|24|0", "eventKeyboard|24|1",
                             "eventKeyboard|24|0", "eventKeyboard|24|1", "eventKeyboard|24|0",
                             "eventKeyboard|24|1", "eventKeyboard|24|0", "eventKeyboard|24|1",
                             "eventKeyboard|24|0", "eventKeyboard|24|1", "eventKeyboard|24|0",
                             "eventKeyboard|24|1", "eventKeyboard|24|0", "eventKeyboard|24|1",
                             "eventKeyboard|24|0", "eventKeyboard|24|1", "eventKeyboard|24|0",
                             "eventKeyboard|24|1", "eventKeyboard|24|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2031|1|", "eventKeyboard|2031|0|", "eventKeyboard|2031|1|",
                        "eventKeyboard|2031|0|", "eventKeyboard|2031|1|", "eventKeyboard|2031|0|",
                        "eventKeyboard|2031|1|", "eventKeyboard|2031|0|", "eventKeyboard|2031|1|",
                        "eventKeyboard|2031|0|", "eventKeyboard|2031|1|", "eventKeyboard|2031|0|",
                        "eventKeyboard|2031|1|", "eventKeyboard|2031|0|", "eventKeyboard|2031|1|",
                        "eventKeyboard|2031|0|", "eventKeyboard|2031|1|", "eventKeyboard|2031|0|",
                        "eventKeyboard|2031|1|", "eventKeyboard|2031|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_024_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_024_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_024_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json",
                    "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json",
                    "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json",
                    "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json",
                    "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json", "keyboard_KEY_P_press.json", "keyboard_KEY_P_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|25|1", "eventKeyboard|25|0", "eventKeyboard|25|1",
                             "eventKeyboard|25|0", "eventKeyboard|25|1", "eventKeyboard|25|0",
                             "eventKeyboard|25|1", "eventKeyboard|25|0", "eventKeyboard|25|1",
                             "eventKeyboard|25|0", "eventKeyboard|25|1", "eventKeyboard|25|0",
                             "eventKeyboard|25|1", "eventKeyboard|25|0", "eventKeyboard|25|1",
                             "eventKeyboard|25|0", "eventKeyboard|25|1", "eventKeyboard|25|0",
                             "eventKeyboard|25|1", "eventKeyboard|25|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2032|1|", "eventKeyboard|2032|0|", "eventKeyboard|2032|1|",
                        "eventKeyboard|2032|0|", "eventKeyboard|2032|1|", "eventKeyboard|2032|0|",
                        "eventKeyboard|2032|1|", "eventKeyboard|2032|0|", "eventKeyboard|2032|1|",
                        "eventKeyboard|2032|0|", "eventKeyboard|2032|1|", "eventKeyboard|2032|0|",
                        "eventKeyboard|2032|1|", "eventKeyboard|2032|0|", "eventKeyboard|2032|1|",
                        "eventKeyboard|2032|0|", "eventKeyboard|2032|1|", "eventKeyboard|2032|0|",
                        "eventKeyboard|2032|1|", "eventKeyboard|2032|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_025_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_025_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_025_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json", "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json",
                    "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json", "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json",
                    "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json", "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json",
                    "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json", "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json",
                    "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json", "keyboard_KEY_LEFT_BRACKET_press.json", "keyboard_KEY_LEFT_BRACKET_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|26|1", "eventKeyboard|26|0", "eventKeyboard|26|1",
                             "eventKeyboard|26|0", "eventKeyboard|26|1", "eventKeyboard|26|0",
                             "eventKeyboard|26|1", "eventKeyboard|26|0", "eventKeyboard|26|1",
                             "eventKeyboard|26|0", "eventKeyboard|26|1", "eventKeyboard|26|0",
                             "eventKeyboard|26|1", "eventKeyboard|26|0", "eventKeyboard|26|1",
                             "eventKeyboard|26|0", "eventKeyboard|26|1", "eventKeyboard|26|0",
                             "eventKeyboard|26|1", "eventKeyboard|26|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2059|1|", "eventKeyboard|2059|0|", "eventKeyboard|2059|1|",
                        "eventKeyboard|2059|0|", "eventKeyboard|2059|1|", "eventKeyboard|2059|0|",
                        "eventKeyboard|2059|1|", "eventKeyboard|2059|0|", "eventKeyboard|2059|1|",
                        "eventKeyboard|2059|0|", "eventKeyboard|2059|1|", "eventKeyboard|2059|0|",
                        "eventKeyboard|2059|1|", "eventKeyboard|2059|0|", "eventKeyboard|2059|1|",
                        "eventKeyboard|2059|0|", "eventKeyboard|2059|1|", "eventKeyboard|2059|0|",
                        "eventKeyboard|2059|1|", "eventKeyboard|2059|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_026_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json", "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json",
                    "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json", "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json",
                    "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json", "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json",
                    "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json", "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json",
                    "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json", "keyboard_KEY_RIGHT_BRACKET_press.json", "keyboard_KEY_RIGHT_BRACKET_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|27|1", "eventKeyboard|27|0", "eventKeyboard|27|1",
                             "eventKeyboard|27|0", "eventKeyboard|27|1", "eventKeyboard|27|0",
                             "eventKeyboard|27|1", "eventKeyboard|27|0", "eventKeyboard|27|1",
                             "eventKeyboard|27|0", "eventKeyboard|27|1", "eventKeyboard|27|0",
                             "eventKeyboard|27|1", "eventKeyboard|27|0", "eventKeyboard|27|1",
                             "eventKeyboard|27|0", "eventKeyboard|27|1", "eventKeyboard|27|0",
                             "eventKeyboard|27|1", "eventKeyboard|27|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2060|1|", "eventKeyboard|2060|0|", "eventKeyboard|2060|1|",
                        "eventKeyboard|2060|0|", "eventKeyboard|2060|1|", "eventKeyboard|2060|0|",
                        "eventKeyboard|2060|1|", "eventKeyboard|2060|0|", "eventKeyboard|2060|1|",
                        "eventKeyboard|2060|0|", "eventKeyboard|2060|1|", "eventKeyboard|2060|0|",
                        "eventKeyboard|2060|1|", "eventKeyboard|2060|0|", "eventKeyboard|2060|1|",
                        "eventKeyboard|2060|0|", "eventKeyboard|2060|1|", "eventKeyboard|2060|0|",
                        "eventKeyboard|2060|1|", "eventKeyboard|2060|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_027_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_027_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_027_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json", "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json",
                    "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json", "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json",
                    "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json", "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json",
                    "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json", "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json",
                    "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json", "keyboard_KEY_CAPS_LOCK_press.json", "keyboard_KEY_CAPS_LOCK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|58|1", "eventKeyboard|58|0", "eventKeyboard|58|1",
                             "eventKeyboard|58|0", "eventKeyboard|58|1", "eventKeyboard|58|0",
                             "eventKeyboard|58|1", "eventKeyboard|58|0", "eventKeyboard|58|1",
                             "eventKeyboard|58|0", "eventKeyboard|58|1", "eventKeyboard|58|0",
                             "eventKeyboard|58|1", "eventKeyboard|58|0", "eventKeyboard|58|1",
                             "eventKeyboard|58|0", "eventKeyboard|58|1", "eventKeyboard|58|0",
                             "eventKeyboard|58|1", "eventKeyboard|58|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2074|1|", "eventKeyboard|2074|0|", "eventKeyboard|2074|1|",
                        "eventKeyboard|2074|0|", "eventKeyboard|2074|1|", "eventKeyboard|2074|0|",
                        "eventKeyboard|2074|1|", "eventKeyboard|2074|0|", "eventKeyboard|2074|1|",
                        "eventKeyboard|2074|0|", "eventKeyboard|2074|1|", "eventKeyboard|2074|0|",
                        "eventKeyboard|2074|1|", "eventKeyboard|2074|0|", "eventKeyboard|2074|1|",
                        "eventKeyboard|2074|0|", "eventKeyboard|2074|1|", "eventKeyboard|2074|0|",
                        "eventKeyboard|2074|1|", "eventKeyboard|2074|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_028_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_028_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_028_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json", "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json",
                    "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json", "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json",
                    "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json", "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json",
                    "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json", "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json",
                    "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json", "keyboard_KEY_A_press.json", "keyboard_KEY_A_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|30|1", "eventKeyboard|30|0", "eventKeyboard|30|1",
                             "eventKeyboard|30|0", "eventKeyboard|30|1", "eventKeyboard|30|0",
                             "eventKeyboard|30|1", "eventKeyboard|30|0", "eventKeyboard|30|1",
                             "eventKeyboard|30|0", "eventKeyboard|30|1", "eventKeyboard|30|0",
                             "eventKeyboard|30|1", "eventKeyboard|30|0", "eventKeyboard|30|1",
                             "eventKeyboard|30|0", "eventKeyboard|30|1", "eventKeyboard|30|0",
                             "eventKeyboard|30|1", "eventKeyboard|30|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2017|1|", "eventKeyboard|2017|0|", "eventKeyboard|2017|1|",
                        "eventKeyboard|2017|0|", "eventKeyboard|2017|1|", "eventKeyboard|2017|0|",
                        "eventKeyboard|2017|1|", "eventKeyboard|2017|0|", "eventKeyboard|2017|1|",
                        "eventKeyboard|2017|0|", "eventKeyboard|2017|1|", "eventKeyboard|2017|0|",
                        "eventKeyboard|2017|1|", "eventKeyboard|2017|0|", "eventKeyboard|2017|1|",
                        "eventKeyboard|2017|0|", "eventKeyboard|2017|1|", "eventKeyboard|2017|0|",
                        "eventKeyboard|2017|1|", "eventKeyboard|2017|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_029_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_029_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_029_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json",
                    "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json",
                    "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json",
                    "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json",
                    "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json", "keyboard_KEY_S_press.json", "keyboard_KEY_S_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|31|1", "eventKeyboard|31|0", "eventKeyboard|31|1",
                             "eventKeyboard|31|0", "eventKeyboard|31|1", "eventKeyboard|31|0",
                             "eventKeyboard|31|1", "eventKeyboard|31|0", "eventKeyboard|31|1",
                             "eventKeyboard|31|0", "eventKeyboard|31|1", "eventKeyboard|31|0",
                             "eventKeyboard|31|1", "eventKeyboard|31|0", "eventKeyboard|31|1",
                             "eventKeyboard|31|0", "eventKeyboard|31|1", "eventKeyboard|31|0",
                             "eventKeyboard|31|1", "eventKeyboard|31|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2035|1|", "eventKeyboard|2035|0|", "eventKeyboard|2035|1|",
                        "eventKeyboard|2035|0|", "eventKeyboard|2035|1|", "eventKeyboard|2035|0|",
                        "eventKeyboard|2035|1|", "eventKeyboard|2035|0|", "eventKeyboard|2035|1|",
                        "eventKeyboard|2035|0|", "eventKeyboard|2035|1|", "eventKeyboard|2035|0|",
                        "eventKeyboard|2035|1|", "eventKeyboard|2035|0|", "eventKeyboard|2035|1|",
                        "eventKeyboard|2035|0|", "eventKeyboard|2035|1|", "eventKeyboard|2035|0|",
                        "eventKeyboard|2035|1|", "eventKeyboard|2035|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_030_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_030_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_030_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json",
                    "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json",
                    "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json",
                    "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json",
                    "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json", "keyboard_KEY_D_press.json", "keyboard_KEY_D_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|32|1", "eventKeyboard|32|0", "eventKeyboard|32|1",
                             "eventKeyboard|32|0", "eventKeyboard|32|1", "eventKeyboard|32|0",
                             "eventKeyboard|32|1", "eventKeyboard|32|0", "eventKeyboard|32|1",
                             "eventKeyboard|32|0", "eventKeyboard|32|1", "eventKeyboard|32|0",
                             "eventKeyboard|32|1", "eventKeyboard|32|0", "eventKeyboard|32|1",
                             "eventKeyboard|32|0", "eventKeyboard|32|1", "eventKeyboard|32|0",
                             "eventKeyboard|32|1", "eventKeyboard|32|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2020|1|", "eventKeyboard|2020|0|", "eventKeyboard|2020|1|",
                        "eventKeyboard|2020|0|", "eventKeyboard|2020|1|", "eventKeyboard|2020|0|",
                        "eventKeyboard|2020|1|", "eventKeyboard|2020|0|", "eventKeyboard|2020|1|",
                        "eventKeyboard|2020|0|", "eventKeyboard|2020|1|", "eventKeyboard|2020|0|",
                        "eventKeyboard|2020|1|", "eventKeyboard|2020|0|", "eventKeyboard|2020|1|",
                        "eventKeyboard|2020|0|", "eventKeyboard|2020|1|", "eventKeyboard|2020|0|",
                        "eventKeyboard|2020|1|", "eventKeyboard|2020|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_031_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_031_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_031_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json",
                    "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json",
                    "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json",
                    "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json",
                    "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json", "keyboard_KEY_F_press.json", "keyboard_KEY_F_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|33|1", "eventKeyboard|33|0", "eventKeyboard|33|1",
                             "eventKeyboard|33|0", "eventKeyboard|33|1", "eventKeyboard|33|0",
                             "eventKeyboard|33|1", "eventKeyboard|33|0", "eventKeyboard|33|1",
                             "eventKeyboard|33|0", "eventKeyboard|33|1", "eventKeyboard|33|0",
                             "eventKeyboard|33|1", "eventKeyboard|33|0", "eventKeyboard|33|1",
                             "eventKeyboard|33|0", "eventKeyboard|33|1", "eventKeyboard|33|0",
                             "eventKeyboard|33|1", "eventKeyboard|33|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2022|1|", "eventKeyboard|2022|0|", "eventKeyboard|2022|1|",
                        "eventKeyboard|2022|0|", "eventKeyboard|2022|1|", "eventKeyboard|2022|0|",
                        "eventKeyboard|2022|1|", "eventKeyboard|2022|0|", "eventKeyboard|2022|1|",
                        "eventKeyboard|2022|0|", "eventKeyboard|2022|1|", "eventKeyboard|2022|0|",
                        "eventKeyboard|2022|1|", "eventKeyboard|2022|0|", "eventKeyboard|2022|1|",
                        "eventKeyboard|2022|0|", "eventKeyboard|2022|1|", "eventKeyboard|2022|0|",
                        "eventKeyboard|2022|1|", "eventKeyboard|2022|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_032_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_032_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_032_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json", "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json",
                    "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json", "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json",
                    "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json", "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json",
                    "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json", "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json",
                    "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json", "keyboard_KEY_G_press.json", "keyboard_KEY_G_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|34|1", "eventKeyboard|34|0", "eventKeyboard|34|1",
                             "eventKeyboard|34|0", "eventKeyboard|34|1", "eventKeyboard|34|0",
                             "eventKeyboard|34|1", "eventKeyboard|34|0", "eventKeyboard|34|1",
                             "eventKeyboard|34|0", "eventKeyboard|34|1", "eventKeyboard|34|0",
                             "eventKeyboard|34|1", "eventKeyboard|34|0", "eventKeyboard|34|1",
                             "eventKeyboard|34|0", "eventKeyboard|34|1", "eventKeyboard|34|0",
                             "eventKeyboard|34|1", "eventKeyboard|34|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2023|1|", "eventKeyboard|2023|0|", "eventKeyboard|2023|1|",
                        "eventKeyboard|2023|0|", "eventKeyboard|2023|1|", "eventKeyboard|2023|0|",
                        "eventKeyboard|2023|1|", "eventKeyboard|2023|0|", "eventKeyboard|2023|1|",
                        "eventKeyboard|2023|0|", "eventKeyboard|2023|1|", "eventKeyboard|2023|0|",
                        "eventKeyboard|2023|1|", "eventKeyboard|2023|0|", "eventKeyboard|2023|1|",
                        "eventKeyboard|2023|0|", "eventKeyboard|2023|1|", "eventKeyboard|2023|0|",
                        "eventKeyboard|2023|1|", "eventKeyboard|2023|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_033_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_033_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_033_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json",
                    "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json",
                    "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json",
                    "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json",
                    "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json", "keyboard_KEY_H_press.json", "keyboard_KEY_H_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|35|1", "eventKeyboard|35|0", "eventKeyboard|35|1",
                             "eventKeyboard|35|0", "eventKeyboard|35|1", "eventKeyboard|35|0",
                             "eventKeyboard|35|1", "eventKeyboard|35|0", "eventKeyboard|35|1",
                             "eventKeyboard|35|0", "eventKeyboard|35|1", "eventKeyboard|35|0",
                             "eventKeyboard|35|1", "eventKeyboard|35|0", "eventKeyboard|35|1",
                             "eventKeyboard|35|0", "eventKeyboard|35|1", "eventKeyboard|35|0",
                             "eventKeyboard|35|1", "eventKeyboard|35|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2024|1|", "eventKeyboard|2024|0|", "eventKeyboard|2024|1|",
                        "eventKeyboard|2024|0|", "eventKeyboard|2024|1|", "eventKeyboard|2024|0|",
                        "eventKeyboard|2024|1|", "eventKeyboard|2024|0|", "eventKeyboard|2024|1|",
                        "eventKeyboard|2024|0|", "eventKeyboard|2024|1|", "eventKeyboard|2024|0|",
                        "eventKeyboard|2024|1|", "eventKeyboard|2024|0|", "eventKeyboard|2024|1|",
                        "eventKeyboard|2024|0|", "eventKeyboard|2024|1|", "eventKeyboard|2024|0|",
                        "eventKeyboard|2024|1|", "eventKeyboard|2024|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_034_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_034_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_034_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json", "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json",
                    "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json", "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json",
                    "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json", "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json",
                    "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json", "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json",
                    "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json", "keyboard_KEY_J_press.json", "keyboard_KEY_J_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|36|1", "eventKeyboard|36|0", "eventKeyboard|36|1",
                             "eventKeyboard|36|0", "eventKeyboard|36|1", "eventKeyboard|36|0",
                             "eventKeyboard|36|1", "eventKeyboard|36|0", "eventKeyboard|36|1",
                             "eventKeyboard|36|0", "eventKeyboard|36|1", "eventKeyboard|36|0",
                             "eventKeyboard|36|1", "eventKeyboard|36|0", "eventKeyboard|36|1",
                             "eventKeyboard|36|0", "eventKeyboard|36|1", "eventKeyboard|36|0",
                             "eventKeyboard|36|1", "eventKeyboard|36|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2026|1|", "eventKeyboard|2026|0|", "eventKeyboard|2026|1|",
                        "eventKeyboard|2026|0|", "eventKeyboard|2026|1|", "eventKeyboard|2026|0|",
                        "eventKeyboard|2026|1|", "eventKeyboard|2026|0|", "eventKeyboard|2026|1|",
                        "eventKeyboard|2026|0|", "eventKeyboard|2026|1|", "eventKeyboard|2026|0|",
                        "eventKeyboard|2026|1|", "eventKeyboard|2026|0|", "eventKeyboard|2026|1|",
                        "eventKeyboard|2026|0|", "eventKeyboard|2026|1|", "eventKeyboard|2026|0|",
                        "eventKeyboard|2026|1|", "eventKeyboard|2026|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_035_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_035_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_035_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json", "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json",
                    "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json", "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json",
                    "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json", "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json",
                    "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json", "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json",
                    "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json", "keyboard_KEY_K_press.json", "keyboard_KEY_K_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|37|1", "eventKeyboard|37|0", "eventKeyboard|37|1",
                             "eventKeyboard|37|0", "eventKeyboard|37|1", "eventKeyboard|37|0",
                             "eventKeyboard|37|1", "eventKeyboard|37|0", "eventKeyboard|37|1",
                             "eventKeyboard|37|0", "eventKeyboard|37|1", "eventKeyboard|37|0",
                             "eventKeyboard|37|1", "eventKeyboard|37|0", "eventKeyboard|37|1",
                             "eventKeyboard|37|0", "eventKeyboard|37|1", "eventKeyboard|37|0",
                             "eventKeyboard|37|1", "eventKeyboard|37|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2027|1|", "eventKeyboard|2027|0|", "eventKeyboard|2027|1|",
                        "eventKeyboard|2027|0|", "eventKeyboard|2027|1|", "eventKeyboard|2027|0|",
                        "eventKeyboard|2027|1|", "eventKeyboard|2027|0|", "eventKeyboard|2027|1|",
                        "eventKeyboard|2027|0|", "eventKeyboard|2027|1|", "eventKeyboard|2027|0|",
                        "eventKeyboard|2027|1|", "eventKeyboard|2027|0|", "eventKeyboard|2027|1|",
                        "eventKeyboard|2027|0|", "eventKeyboard|2027|1|", "eventKeyboard|2027|0|",
                        "eventKeyboard|2027|1|", "eventKeyboard|2027|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_036_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_036_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_036_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json",
                    "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json",
                    "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json",
                    "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json",
                    "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json", "keyboard_KEY_L_press.json", "keyboard_KEY_L_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|38|1", "eventKeyboard|38|0", "eventKeyboard|38|1",
                             "eventKeyboard|38|0", "eventKeyboard|38|1", "eventKeyboard|38|0",
                             "eventKeyboard|38|1", "eventKeyboard|38|0", "eventKeyboard|38|1",
                             "eventKeyboard|38|0", "eventKeyboard|38|1", "eventKeyboard|38|0",
                             "eventKeyboard|38|1", "eventKeyboard|38|0", "eventKeyboard|38|1",
                             "eventKeyboard|38|0", "eventKeyboard|38|1", "eventKeyboard|38|0",
                             "eventKeyboard|38|1", "eventKeyboard|38|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2028|1|", "eventKeyboard|2028|0|", "eventKeyboard|2028|1|",
                        "eventKeyboard|2028|0|", "eventKeyboard|2028|1|", "eventKeyboard|2028|0|",
                        "eventKeyboard|2028|1|", "eventKeyboard|2028|0|", "eventKeyboard|2028|1|",
                        "eventKeyboard|2028|0|", "eventKeyboard|2028|1|", "eventKeyboard|2028|0|",
                        "eventKeyboard|2028|1|", "eventKeyboard|2028|0|", "eventKeyboard|2028|1|",
                        "eventKeyboard|2028|0|", "eventKeyboard|2028|1|", "eventKeyboard|2028|0|",
                        "eventKeyboard|2028|1|", "eventKeyboard|2028|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_037_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_037_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_037_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json", "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json",
                    "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json", "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json",
                    "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json", "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json",
                    "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json", "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json",
                    "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json", "keyboard_KEY_SEMICOLON_press.json", "keyboard_KEY_SEMICOLON_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|39|1", "eventKeyboard|39|0", "eventKeyboard|39|1",
                             "eventKeyboard|39|0", "eventKeyboard|39|1", "eventKeyboard|39|0",
                             "eventKeyboard|39|1", "eventKeyboard|39|0", "eventKeyboard|39|1",
                             "eventKeyboard|39|0", "eventKeyboard|39|1", "eventKeyboard|39|0",
                             "eventKeyboard|39|1", "eventKeyboard|39|0", "eventKeyboard|39|1",
                             "eventKeyboard|39|0", "eventKeyboard|39|1", "eventKeyboard|39|0",
                             "eventKeyboard|39|1", "eventKeyboard|39|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2062|1|", "eventKeyboard|2062|0|", "eventKeyboard|2062|1|",
                        "eventKeyboard|2062|0|", "eventKeyboard|2062|1|", "eventKeyboard|2062|0|",
                        "eventKeyboard|2062|1|", "eventKeyboard|2062|0|", "eventKeyboard|2062|1|",
                        "eventKeyboard|2062|0|", "eventKeyboard|2062|1|", "eventKeyboard|2062|0|",
                        "eventKeyboard|2062|1|", "eventKeyboard|2062|0|", "eventKeyboard|2062|1|",
                        "eventKeyboard|2062|0|", "eventKeyboard|2062|1|", "eventKeyboard|2062|0|",
                        "eventKeyboard|2062|1|", "eventKeyboard|2062|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_038_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_038_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_038_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json", "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json",
                    "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json", "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json",
                    "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json", "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json",
                    "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json", "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json",
                    "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json", "keyboard_KEY_APOSTROPHE_press.json", "keyboard_KEY_APOSTROPHE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|40|1", "eventKeyboard|40|0", "eventKeyboard|40|1",
                             "eventKeyboard|40|0", "eventKeyboard|40|1", "eventKeyboard|40|0",
                             "eventKeyboard|40|1", "eventKeyboard|40|0", "eventKeyboard|40|1",
                             "eventKeyboard|40|0", "eventKeyboard|40|1", "eventKeyboard|40|0",
                             "eventKeyboard|40|1", "eventKeyboard|40|0", "eventKeyboard|40|1",
                             "eventKeyboard|40|0", "eventKeyboard|40|1", "eventKeyboard|40|0",
                             "eventKeyboard|40|1", "eventKeyboard|40|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2063|1|", "eventKeyboard|2063|0|", "eventKeyboard|2063|1|",
                        "eventKeyboard|2063|0|", "eventKeyboard|2063|1|", "eventKeyboard|2063|0|",
                        "eventKeyboard|2063|1|", "eventKeyboard|2063|0|", "eventKeyboard|2063|1|",
                        "eventKeyboard|2063|0|", "eventKeyboard|2063|1|", "eventKeyboard|2063|0|",
                        "eventKeyboard|2063|1|", "eventKeyboard|2063|0|", "eventKeyboard|2063|1|",
                        "eventKeyboard|2063|0|", "eventKeyboard|2063|1|", "eventKeyboard|2063|0|",
                        "eventKeyboard|2063|1|", "eventKeyboard|2063|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_039_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_039_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_039_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json", "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json",
                    "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json", "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json",
                    "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json", "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json",
                    "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json", "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json",
                    "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json", "keyboard_KEY_BACKSLASH_press.json", "keyboard_KEY_BACKSLASH_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|43|1", "eventKeyboard|43|0", "eventKeyboard|43|1",
                             "eventKeyboard|43|0", "eventKeyboard|43|1", "eventKeyboard|43|0",
                             "eventKeyboard|43|1", "eventKeyboard|43|0", "eventKeyboard|43|1",
                             "eventKeyboard|43|0", "eventKeyboard|43|1", "eventKeyboard|43|0",
                             "eventKeyboard|43|1", "eventKeyboard|43|0", "eventKeyboard|43|1",
                             "eventKeyboard|43|0", "eventKeyboard|43|1", "eventKeyboard|43|0",
                             "eventKeyboard|43|1", "eventKeyboard|43|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2061|1|", "eventKeyboard|2061|0|", "eventKeyboard|2061|1|",
                        "eventKeyboard|2061|0|", "eventKeyboard|2061|1|", "eventKeyboard|2061|0|",
                        "eventKeyboard|2061|1|", "eventKeyboard|2061|0|", "eventKeyboard|2061|1|",
                        "eventKeyboard|2061|0|", "eventKeyboard|2061|1|", "eventKeyboard|2061|0|",
                        "eventKeyboard|2061|1|", "eventKeyboard|2061|0|", "eventKeyboard|2061|1|",
                        "eventKeyboard|2061|0|", "eventKeyboard|2061|1|", "eventKeyboard|2061|0|",
                        "eventKeyboard|2061|1|", "eventKeyboard|2061|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_040_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_040_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_040_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json",
                    "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json",
                    "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json",
                    "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json",
                    "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json", "keyboard_KEY_SHIFT_LEFT_press.json", "keyboard_KEY_SHIFT_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|42|1", "eventKeyboard|42|0", "eventKeyboard|42|1",
                             "eventKeyboard|42|0", "eventKeyboard|42|1", "eventKeyboard|42|0",
                             "eventKeyboard|42|1", "eventKeyboard|42|0", "eventKeyboard|42|1",
                             "eventKeyboard|42|0", "eventKeyboard|42|1", "eventKeyboard|42|0",
                             "eventKeyboard|42|1", "eventKeyboard|42|0", "eventKeyboard|42|1",
                             "eventKeyboard|42|0", "eventKeyboard|42|1", "eventKeyboard|42|0",
                             "eventKeyboard|42|1", "eventKeyboard|42|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2047|1|", "eventKeyboard|2047|0|", "eventKeyboard|2047|1|",
                        "eventKeyboard|2047|0|", "eventKeyboard|2047|1|", "eventKeyboard|2047|0|",
                        "eventKeyboard|2047|1|", "eventKeyboard|2047|0|", "eventKeyboard|2047|1|",
                        "eventKeyboard|2047|0|", "eventKeyboard|2047|1|", "eventKeyboard|2047|0|",
                        "eventKeyboard|2047|1|", "eventKeyboard|2047|0|", "eventKeyboard|2047|1|",
                        "eventKeyboard|2047|0|", "eventKeyboard|2047|1|", "eventKeyboard|2047|0|",
                        "eventKeyboard|2047|1|", "eventKeyboard|2047|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_041_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_041_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_041_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json",
                    "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json",
                    "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json",
                    "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json",
                    "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json", "keyboard_KEY_Z_press.json", "keyboard_KEY_Z_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|44|1", "eventKeyboard|44|0", "eventKeyboard|44|1",
                             "eventKeyboard|44|0", "eventKeyboard|44|1", "eventKeyboard|44|0",
                             "eventKeyboard|44|1", "eventKeyboard|44|0", "eventKeyboard|44|1",
                             "eventKeyboard|44|0", "eventKeyboard|44|1", "eventKeyboard|44|0",
                             "eventKeyboard|44|1", "eventKeyboard|44|0", "eventKeyboard|44|1",
                             "eventKeyboard|44|0", "eventKeyboard|44|1", "eventKeyboard|44|0",
                             "eventKeyboard|44|1", "eventKeyboard|44|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2042|1|", "eventKeyboard|2042|0|", "eventKeyboard|2042|1|",
                        "eventKeyboard|2042|0|", "eventKeyboard|2042|1|", "eventKeyboard|2042|0|",
                        "eventKeyboard|2042|1|", "eventKeyboard|2042|0|", "eventKeyboard|2042|1|",
                        "eventKeyboard|2042|0|", "eventKeyboard|2042|1|", "eventKeyboard|2042|0|",
                        "eventKeyboard|2042|1|", "eventKeyboard|2042|0|", "eventKeyboard|2042|1|",
                        "eventKeyboard|2042|0|", "eventKeyboard|2042|1|", "eventKeyboard|2042|0|",
                        "eventKeyboard|2042|1|", "eventKeyboard|2042|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_042_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_042_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_042_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json",
                    "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json",
                    "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json",
                    "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json",
                    "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json", "keyboard_KEY_X_press.json", "keyboard_KEY_X_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|45|1", "eventKeyboard|45|0", "eventKeyboard|45|1",
                             "eventKeyboard|45|0", "eventKeyboard|45|1", "eventKeyboard|45|0",
                             "eventKeyboard|45|1", "eventKeyboard|45|0", "eventKeyboard|45|1",
                             "eventKeyboard|45|0", "eventKeyboard|45|1", "eventKeyboard|45|0",
                             "eventKeyboard|45|1", "eventKeyboard|45|0", "eventKeyboard|45|1",
                             "eventKeyboard|45|0", "eventKeyboard|45|1", "eventKeyboard|45|0",
                             "eventKeyboard|45|1", "eventKeyboard|45|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2040|1|", "eventKeyboard|2040|0|", "eventKeyboard|2040|1|",
                        "eventKeyboard|2040|0|", "eventKeyboard|2040|1|", "eventKeyboard|2040|0|",
                        "eventKeyboard|2040|1|", "eventKeyboard|2040|0|", "eventKeyboard|2040|1|",
                        "eventKeyboard|2040|0|", "eventKeyboard|2040|1|", "eventKeyboard|2040|0|",
                        "eventKeyboard|2040|1|", "eventKeyboard|2040|0|", "eventKeyboard|2040|1|",
                        "eventKeyboard|2040|0|", "eventKeyboard|2040|1|", "eventKeyboard|2040|0|",
                        "eventKeyboard|2040|1|", "eventKeyboard|2040|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_043_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_043_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_043_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json",
                    "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json",
                    "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json",
                    "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json",
                    "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json", "keyboard_KEY_C_press.json", "keyboard_KEY_C_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|46|1", "eventKeyboard|46|0", "eventKeyboard|46|1",
                             "eventKeyboard|46|0", "eventKeyboard|46|1", "eventKeyboard|46|0",
                             "eventKeyboard|46|1", "eventKeyboard|46|0", "eventKeyboard|46|1",
                             "eventKeyboard|46|0", "eventKeyboard|46|1", "eventKeyboard|46|0",
                             "eventKeyboard|46|1", "eventKeyboard|46|0", "eventKeyboard|46|1",
                             "eventKeyboard|46|0", "eventKeyboard|46|1", "eventKeyboard|46|0",
                             "eventKeyboard|46|1", "eventKeyboard|46|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2019|1|", "eventKeyboard|2019|0|", "eventKeyboard|2019|1|",
                        "eventKeyboard|2019|0|", "eventKeyboard|2019|1|", "eventKeyboard|2019|0|",
                        "eventKeyboard|2019|1|", "eventKeyboard|2019|0|", "eventKeyboard|2019|1|",
                        "eventKeyboard|2019|0|", "eventKeyboard|2019|1|", "eventKeyboard|2019|0|",
                        "eventKeyboard|2019|1|", "eventKeyboard|2019|0|", "eventKeyboard|2019|1|",
                        "eventKeyboard|2019|0|", "eventKeyboard|2019|1|", "eventKeyboard|2019|0|",
                        "eventKeyboard|2019|1|", "eventKeyboard|2019|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_044_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_044_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_044_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json",
                    "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json",
                    "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json",
                    "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json",
                    "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json", "keyboard_KEY_V_press.json", "keyboard_KEY_V_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|47|1", "eventKeyboard|47|0", "eventKeyboard|47|1",
                             "eventKeyboard|47|0", "eventKeyboard|47|1", "eventKeyboard|47|0",
                             "eventKeyboard|47|1", "eventKeyboard|47|0", "eventKeyboard|47|1",
                             "eventKeyboard|47|0", "eventKeyboard|47|1", "eventKeyboard|47|0",
                             "eventKeyboard|47|1", "eventKeyboard|47|0", "eventKeyboard|47|1",
                             "eventKeyboard|47|0", "eventKeyboard|47|1", "eventKeyboard|47|0",
                             "eventKeyboard|47|1", "eventKeyboard|47|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2038|1|", "eventKeyboard|2038|0|", "eventKeyboard|2038|1|",
                        "eventKeyboard|2038|0|", "eventKeyboard|2038|1|", "eventKeyboard|2038|0|",
                        "eventKeyboard|2038|1|", "eventKeyboard|2038|0|", "eventKeyboard|2038|1|",
                        "eventKeyboard|2038|0|", "eventKeyboard|2038|1|", "eventKeyboard|2038|0|",
                        "eventKeyboard|2038|1|", "eventKeyboard|2038|0|", "eventKeyboard|2038|1|",
                        "eventKeyboard|2038|0|", "eventKeyboard|2038|1|", "eventKeyboard|2038|0|",
                        "eventKeyboard|2038|1|", "eventKeyboard|2038|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_045_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_045_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_045_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json", "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json",
                    "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json", "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json",
                    "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json", "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json",
                    "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json", "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json",
                    "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json", "keyboard_KEY_B_press.json", "keyboard_KEY_B_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|48|1", "eventKeyboard|48|0", "eventKeyboard|48|1",
                             "eventKeyboard|48|0", "eventKeyboard|48|1", "eventKeyboard|48|0",
                             "eventKeyboard|48|1", "eventKeyboard|48|0", "eventKeyboard|48|1",
                             "eventKeyboard|48|0", "eventKeyboard|48|1", "eventKeyboard|48|0",
                             "eventKeyboard|48|1", "eventKeyboard|48|0", "eventKeyboard|48|1",
                             "eventKeyboard|48|0", "eventKeyboard|48|1", "eventKeyboard|48|0",
                             "eventKeyboard|48|1", "eventKeyboard|48|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2018|1|", "eventKeyboard|2018|0|", "eventKeyboard|2018|1|",
                        "eventKeyboard|2018|0|", "eventKeyboard|2018|1|", "eventKeyboard|2018|0|",
                        "eventKeyboard|2018|1|", "eventKeyboard|2018|0|", "eventKeyboard|2018|1|",
                        "eventKeyboard|2018|0|", "eventKeyboard|2018|1|", "eventKeyboard|2018|0|",
                        "eventKeyboard|2018|1|", "eventKeyboard|2018|0|", "eventKeyboard|2018|1|",
                        "eventKeyboard|2018|0|", "eventKeyboard|2018|1|", "eventKeyboard|2018|0|",
                        "eventKeyboard|2018|1|", "eventKeyboard|2018|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_046_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_046_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_046_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json",
                    "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json",
                    "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json",
                    "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json",
                    "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json", "keyboard_KEY_N_press.json", "keyboard_KEY_N_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|49|1", "eventKeyboard|49|0", "eventKeyboard|49|1",
                             "eventKeyboard|49|0", "eventKeyboard|49|1", "eventKeyboard|49|0",
                             "eventKeyboard|49|1", "eventKeyboard|49|0", "eventKeyboard|49|1",
                             "eventKeyboard|49|0", "eventKeyboard|49|1", "eventKeyboard|49|0",
                             "eventKeyboard|49|1", "eventKeyboard|49|0", "eventKeyboard|49|1",
                             "eventKeyboard|49|0", "eventKeyboard|49|1", "eventKeyboard|49|0",
                             "eventKeyboard|49|1", "eventKeyboard|49|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2030|1|", "eventKeyboard|2030|0|", "eventKeyboard|2030|1|",
                        "eventKeyboard|2030|0|", "eventKeyboard|2030|1|", "eventKeyboard|2030|0|",
                        "eventKeyboard|2030|1|", "eventKeyboard|2030|0|", "eventKeyboard|2030|1|",
                        "eventKeyboard|2030|0|", "eventKeyboard|2030|1|", "eventKeyboard|2030|0|",
                        "eventKeyboard|2030|1|", "eventKeyboard|2030|0|", "eventKeyboard|2030|1|",
                        "eventKeyboard|2030|0|", "eventKeyboard|2030|1|", "eventKeyboard|2030|0|",
                        "eventKeyboard|2030|1|", "eventKeyboard|2030|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_047_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_047_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_047_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json", "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json",
                    "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json", "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json",
                    "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json", "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json",
                    "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json", "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json",
                    "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json", "keyboard_KEY_M_press.json", "keyboard_KEY_M_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|50|1", "eventKeyboard|50|0", "eventKeyboard|50|1",
                             "eventKeyboard|50|0", "eventKeyboard|50|1", "eventKeyboard|50|0",
                             "eventKeyboard|50|1", "eventKeyboard|50|0", "eventKeyboard|50|1",
                             "eventKeyboard|50|0", "eventKeyboard|50|1", "eventKeyboard|50|0",
                             "eventKeyboard|50|1", "eventKeyboard|50|0", "eventKeyboard|50|1",
                             "eventKeyboard|50|0", "eventKeyboard|50|1", "eventKeyboard|50|0",
                             "eventKeyboard|50|1", "eventKeyboard|50|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2029|1|", "eventKeyboard|2029|0|", "eventKeyboard|2029|1|",
                        "eventKeyboard|2029|0|", "eventKeyboard|2029|1|", "eventKeyboard|2029|0|",
                        "eventKeyboard|2029|1|", "eventKeyboard|2029|0|", "eventKeyboard|2029|1|",
                        "eventKeyboard|2029|0|", "eventKeyboard|2029|1|", "eventKeyboard|2029|0|",
                        "eventKeyboard|2029|1|", "eventKeyboard|2029|0|", "eventKeyboard|2029|1|",
                        "eventKeyboard|2029|0|", "eventKeyboard|2029|1|", "eventKeyboard|2029|0|",
                        "eventKeyboard|2029|1|", "eventKeyboard|2029|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_048_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_048_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_048_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json", "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json",
                    "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json", "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json",
                    "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json", "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json",
                    "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json", "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json",
                    "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json", "keyboard_KEY_COMMA_press.json", "keyboard_KEY_COMMA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|51|1", "eventKeyboard|51|0", "eventKeyboard|51|1",
                             "eventKeyboard|51|0", "eventKeyboard|51|1", "eventKeyboard|51|0",
                             "eventKeyboard|51|1", "eventKeyboard|51|0", "eventKeyboard|51|1",
                             "eventKeyboard|51|0", "eventKeyboard|51|1", "eventKeyboard|51|0",
                             "eventKeyboard|51|1", "eventKeyboard|51|0", "eventKeyboard|51|1",
                             "eventKeyboard|51|0", "eventKeyboard|51|1", "eventKeyboard|51|0",
                             "eventKeyboard|51|1", "eventKeyboard|51|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2043|1|", "eventKeyboard|2043|0|", "eventKeyboard|2043|1|",
                        "eventKeyboard|2043|0|", "eventKeyboard|2043|1|", "eventKeyboard|2043|0|",
                        "eventKeyboard|2043|1|", "eventKeyboard|2043|0|", "eventKeyboard|2043|1|",
                        "eventKeyboard|2043|0|", "eventKeyboard|2043|1|", "eventKeyboard|2043|0|",
                        "eventKeyboard|2043|1|", "eventKeyboard|2043|0|", "eventKeyboard|2043|1|",
                        "eventKeyboard|2043|0|", "eventKeyboard|2043|1|", "eventKeyboard|2043|0|",
                        "eventKeyboard|2043|1|", "eventKeyboard|2043|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_049_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_049_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_049_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json", "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json",
                    "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json", "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json",
                    "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json", "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json",
                    "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json", "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json",
                    "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json", "keyboard_KEY_PERIOD_press.json", "keyboard_KEY_PERIOD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|52|1", "eventKeyboard|52|0", "eventKeyboard|52|1",
                             "eventKeyboard|52|0", "eventKeyboard|52|1", "eventKeyboard|52|0",
                             "eventKeyboard|52|1", "eventKeyboard|52|0", "eventKeyboard|52|1",
                             "eventKeyboard|52|0", "eventKeyboard|52|1", "eventKeyboard|52|0",
                             "eventKeyboard|52|1", "eventKeyboard|52|0", "eventKeyboard|52|1",
                             "eventKeyboard|52|0", "eventKeyboard|52|1", "eventKeyboard|52|0",
                             "eventKeyboard|52|1", "eventKeyboard|52|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2044|1|", "eventKeyboard|2044|0|", "eventKeyboard|2044|1|",
                        "eventKeyboard|2044|0|", "eventKeyboard|2044|1|", "eventKeyboard|2044|0|",
                        "eventKeyboard|2044|1|", "eventKeyboard|2044|0|", "eventKeyboard|2044|1|",
                        "eventKeyboard|2044|0|", "eventKeyboard|2044|1|", "eventKeyboard|2044|0|",
                        "eventKeyboard|2044|1|", "eventKeyboard|2044|0|", "eventKeyboard|2044|1|",
                        "eventKeyboard|2044|0|", "eventKeyboard|2044|1|", "eventKeyboard|2044|0|",
                        "eventKeyboard|2044|1|", "eventKeyboard|2044|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_050_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_050_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_050_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json", "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json",
                    "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json", "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json",
                    "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json", "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json",
                    "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json", "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json",
                    "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json", "keyboard_KEY_SLASH_press.json", "keyboard_KEY_SLASH_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|53|1", "eventKeyboard|53|0", "eventKeyboard|53|1",
                             "eventKeyboard|53|0", "eventKeyboard|53|1", "eventKeyboard|53|0",
                             "eventKeyboard|53|1", "eventKeyboard|53|0", "eventKeyboard|53|1",
                             "eventKeyboard|53|0", "eventKeyboard|53|1", "eventKeyboard|53|0",
                             "eventKeyboard|53|1", "eventKeyboard|53|0", "eventKeyboard|53|1",
                             "eventKeyboard|53|0", "eventKeyboard|53|1", "eventKeyboard|53|0",
                             "eventKeyboard|53|1", "eventKeyboard|53|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2064|1|", "eventKeyboard|2064|0|", "eventKeyboard|2064|1|",
                        "eventKeyboard|2064|0|", "eventKeyboard|2064|1|", "eventKeyboard|2064|0|",
                        "eventKeyboard|2064|1|", "eventKeyboard|2064|0|", "eventKeyboard|2064|1|",
                        "eventKeyboard|2064|0|", "eventKeyboard|2064|1|", "eventKeyboard|2064|0|",
                        "eventKeyboard|2064|1|", "eventKeyboard|2064|0|", "eventKeyboard|2064|1|",
                        "eventKeyboard|2064|0|", "eventKeyboard|2064|1|", "eventKeyboard|2064|0|",
                        "eventKeyboard|2064|1|", "eventKeyboard|2064|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_051_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_051_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_051_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json", "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json",
                    "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json", "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json",
                    "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json", "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json",
                    "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json", "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json",
                    "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json", "keyboard_KEY_SHIFT_RIGHT_press.json", "keyboard_KEY_SHIFT_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|54|1", "eventKeyboard|54|0", "eventKeyboard|54|1",
                             "eventKeyboard|54|0", "eventKeyboard|54|1", "eventKeyboard|54|0",
                             "eventKeyboard|54|1", "eventKeyboard|54|0", "eventKeyboard|54|1",
                             "eventKeyboard|54|0", "eventKeyboard|54|1", "eventKeyboard|54|0",
                             "eventKeyboard|54|1", "eventKeyboard|54|0", "eventKeyboard|54|1",
                             "eventKeyboard|54|0", "eventKeyboard|54|1", "eventKeyboard|54|0",
                             "eventKeyboard|54|1", "eventKeyboard|54|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2048|1|", "eventKeyboard|2048|0|", "eventKeyboard|2048|1|",
                        "eventKeyboard|2048|0|", "eventKeyboard|2048|1|", "eventKeyboard|2048|0|",
                        "eventKeyboard|2048|1|", "eventKeyboard|2048|0|", "eventKeyboard|2048|1|",
                        "eventKeyboard|2048|0|", "eventKeyboard|2048|1|", "eventKeyboard|2048|0|",
                        "eventKeyboard|2048|1|", "eventKeyboard|2048|0|", "eventKeyboard|2048|1|",
                        "eventKeyboard|2048|0|", "eventKeyboard|2048|1|", "eventKeyboard|2048|0|",
                        "eventKeyboard|2048|1|", "eventKeyboard|2048|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_052_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_052_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_052_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json", "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json",
                    "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json", "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json",
                    "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json", "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json",
                    "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json", "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json",
                    "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json", "keyboard_KEY_CTRL_LEFT_press.json", "keyboard_KEY_CTRL_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|29|0", "eventKeyboard|29|1",
                             "eventKeyboard|29|0", "eventKeyboard|29|1", "eventKeyboard|29|0",
                             "eventKeyboard|29|1", "eventKeyboard|29|0", "eventKeyboard|29|1",
                             "eventKeyboard|29|0", "eventKeyboard|29|1", "eventKeyboard|29|0",
                             "eventKeyboard|29|1", "eventKeyboard|29|0", "eventKeyboard|29|1",
                             "eventKeyboard|29|0", "eventKeyboard|29|1", "eventKeyboard|29|0",
                             "eventKeyboard|29|1", "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2072|0|", "eventKeyboard|2072|1|",
                        "eventKeyboard|2072|0|", "eventKeyboard|2072|1|", "eventKeyboard|2072|0|",
                        "eventKeyboard|2072|1|", "eventKeyboard|2072|0|", "eventKeyboard|2072|1|",
                        "eventKeyboard|2072|0|", "eventKeyboard|2072|1|", "eventKeyboard|2072|0|",
                        "eventKeyboard|2072|1|", "eventKeyboard|2072|0|", "eventKeyboard|2072|1|",
                        "eventKeyboard|2072|0|", "eventKeyboard|2072|1|", "eventKeyboard|2072|0|",
                        "eventKeyboard|2072|1|", "eventKeyboard|2072|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_053_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_053_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_053_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json", "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json",
                    "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json", "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json",
                    "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json", "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json",
                    "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json", "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json",
                    "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json", "keyboard_KEY_META_LEFT_press.json", "keyboard_KEY_META_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|125|1", "eventKeyboard|125|0", "eventKeyboard|125|1",
                             "eventKeyboard|125|0", "eventKeyboard|125|1", "eventKeyboard|125|0",
                             "eventKeyboard|125|1", "eventKeyboard|125|0", "eventKeyboard|125|1",
                             "eventKeyboard|125|0", "eventKeyboard|125|1", "eventKeyboard|125|0",
                             "eventKeyboard|125|1", "eventKeyboard|125|0", "eventKeyboard|125|1",
                             "eventKeyboard|125|0", "eventKeyboard|125|1", "eventKeyboard|125|0",
                             "eventKeyboard|125|1", "eventKeyboard|125|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2076|1|", "eventKeyboard|2076|0|", "eventKeyboard|2076|1|",
                        "eventKeyboard|2076|0|", "eventKeyboard|2076|1|", "eventKeyboard|2076|0|",
                        "eventKeyboard|2076|1|", "eventKeyboard|2076|0|", "eventKeyboard|2076|1|",
                        "eventKeyboard|2076|0|", "eventKeyboard|2076|1|", "eventKeyboard|2076|0|",
                        "eventKeyboard|2076|1|", "eventKeyboard|2076|0|", "eventKeyboard|2076|1|",
                        "eventKeyboard|2076|0|", "eventKeyboard|2076|1|", "eventKeyboard|2076|0|",
                        "eventKeyboard|2076|1|", "eventKeyboard|2076|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_054_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_054_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_054_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json", "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json",
                    "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json", "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json",
                    "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json", "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json",
                    "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json", "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json",
                    "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json", "keyboard_KEY_ALT_LEFT_press.json", "keyboard_KEY_ALT_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|56|1", "eventKeyboard|56|0", "eventKeyboard|56|1",
                             "eventKeyboard|56|0", "eventKeyboard|56|1", "eventKeyboard|56|0",
                             "eventKeyboard|56|1", "eventKeyboard|56|0", "eventKeyboard|56|1",
                             "eventKeyboard|56|0", "eventKeyboard|56|1", "eventKeyboard|56|0",
                             "eventKeyboard|56|1", "eventKeyboard|56|0", "eventKeyboard|56|1",
                             "eventKeyboard|56|0", "eventKeyboard|56|1", "eventKeyboard|56|0",
                             "eventKeyboard|56|1", "eventKeyboard|56|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2045|1|", "eventKeyboard|2045|0|", "eventKeyboard|2045|1|",
                        "eventKeyboard|2045|0|", "eventKeyboard|2045|1|", "eventKeyboard|2045|0|",
                        "eventKeyboard|2045|1|", "eventKeyboard|2045|0|", "eventKeyboard|2045|1|",
                        "eventKeyboard|2045|0|", "eventKeyboard|2045|1|", "eventKeyboard|2045|0|",
                        "eventKeyboard|2045|1|", "eventKeyboard|2045|0|", "eventKeyboard|2045|1|",
                        "eventKeyboard|2045|0|", "eventKeyboard|2045|1|", "eventKeyboard|2045|0|",
                        "eventKeyboard|2045|1|", "eventKeyboard|2045|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_055_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_055_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_055_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json", "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json",
                    "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json", "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json",
                    "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json", "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json",
                    "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json", "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json",
                    "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json", "keyboard_KEY_SPACE_press.json", "keyboard_KEY_SPACE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|57|1", "eventKeyboard|57|0", "eventKeyboard|57|1",
                             "eventKeyboard|57|0", "eventKeyboard|57|1", "eventKeyboard|57|0",
                             "eventKeyboard|57|1", "eventKeyboard|57|0", "eventKeyboard|57|1",
                             "eventKeyboard|57|0", "eventKeyboard|57|1", "eventKeyboard|57|0",
                             "eventKeyboard|57|1", "eventKeyboard|57|0", "eventKeyboard|57|1",
                             "eventKeyboard|57|0", "eventKeyboard|57|1", "eventKeyboard|57|0",
                             "eventKeyboard|57|1", "eventKeyboard|57|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2050|1|", "eventKeyboard|2050|0|", "eventKeyboard|2050|1|",
                        "eventKeyboard|2050|0|", "eventKeyboard|2050|1|", "eventKeyboard|2050|0|",
                        "eventKeyboard|2050|1|", "eventKeyboard|2050|0|", "eventKeyboard|2050|1|",
                        "eventKeyboard|2050|0|", "eventKeyboard|2050|1|", "eventKeyboard|2050|0|",
                        "eventKeyboard|2050|1|", "eventKeyboard|2050|0|", "eventKeyboard|2050|1|",
                        "eventKeyboard|2050|0|", "eventKeyboard|2050|1|", "eventKeyboard|2050|0|",
                        "eventKeyboard|2050|1|", "eventKeyboard|2050|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_056_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_056_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_056_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json", "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json",
                    "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json", "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json",
                    "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json", "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json",
                    "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json", "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json",
                    "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json", "keyboard_KEY_ALT_RIGHT_press.json", "keyboard_KEY_ALT_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|100|1", "eventKeyboard|100|0", "eventKeyboard|100|1",
                             "eventKeyboard|100|0", "eventKeyboard|100|1", "eventKeyboard|100|0",
                             "eventKeyboard|100|1", "eventKeyboard|100|0", "eventKeyboard|100|1",
                             "eventKeyboard|100|0", "eventKeyboard|100|1", "eventKeyboard|100|0",
                             "eventKeyboard|100|1", "eventKeyboard|100|0", "eventKeyboard|100|1",
                             "eventKeyboard|100|0", "eventKeyboard|100|1", "eventKeyboard|100|0",
                             "eventKeyboard|100|1", "eventKeyboard|100|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2046|1|", "eventKeyboard|2046|0|", "eventKeyboard|2046|1|",
                        "eventKeyboard|2046|0|", "eventKeyboard|2046|1|", "eventKeyboard|2046|0|",
                        "eventKeyboard|2046|1|", "eventKeyboard|2046|0|", "eventKeyboard|2046|1|",
                        "eventKeyboard|2046|0|", "eventKeyboard|2046|1|", "eventKeyboard|2046|0|",
                        "eventKeyboard|2046|1|", "eventKeyboard|2046|0|", "eventKeyboard|2046|1|",
                        "eventKeyboard|2046|0|", "eventKeyboard|2046|1|", "eventKeyboard|2046|0|",
                        "eventKeyboard|2046|1|", "eventKeyboard|2046|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_058_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_058_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_058_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json", "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json",
                    "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json", "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json",
                    "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json", "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json",
                    "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json", "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json",
                    "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json", "keyboard_KEY_CTRL_RIGHT_press.json", "keyboard_KEY_CTRL_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|97|1", "eventKeyboard|97|0", "eventKeyboard|97|1",
                             "eventKeyboard|97|0", "eventKeyboard|97|1", "eventKeyboard|97|0",
                             "eventKeyboard|97|1", "eventKeyboard|97|0", "eventKeyboard|97|1",
                             "eventKeyboard|97|0", "eventKeyboard|97|1", "eventKeyboard|97|0",
                             "eventKeyboard|97|1", "eventKeyboard|97|0", "eventKeyboard|97|1",
                             "eventKeyboard|97|0", "eventKeyboard|97|1", "eventKeyboard|97|0",
                             "eventKeyboard|97|1", "eventKeyboard|97|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2073|1|", "eventKeyboard|2073|0|", "eventKeyboard|2073|1|",
                        "eventKeyboard|2073|0|", "eventKeyboard|2073|1|", "eventKeyboard|2073|0|",
                        "eventKeyboard|2073|1|", "eventKeyboard|2073|0|", "eventKeyboard|2073|1|",
                        "eventKeyboard|2073|0|", "eventKeyboard|2073|1|", "eventKeyboard|2073|0|",
                        "eventKeyboard|2073|1|", "eventKeyboard|2073|0|", "eventKeyboard|2073|1|",
                        "eventKeyboard|2073|0|", "eventKeyboard|2073|1|", "eventKeyboard|2073|0|",
                        "eventKeyboard|2073|1|", "eventKeyboard|2073|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_059_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_059_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_059_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json", "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json",
                    "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json", "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json",
                    "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json", "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json",
                    "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json", "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json",
                    "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json", "keyboard_KEY_F1_press.json", "keyboard_KEY_F1_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|59|1", "eventKeyboard|59|0", "eventKeyboard|59|1",
                             "eventKeyboard|59|0", "eventKeyboard|59|1", "eventKeyboard|59|0",
                             "eventKeyboard|59|1", "eventKeyboard|59|0", "eventKeyboard|59|1",
                             "eventKeyboard|59|0", "eventKeyboard|59|1", "eventKeyboard|59|0",
                             "eventKeyboard|59|1", "eventKeyboard|59|0", "eventKeyboard|59|1",
                             "eventKeyboard|59|0", "eventKeyboard|59|1", "eventKeyboard|59|0",
                             "eventKeyboard|59|1", "eventKeyboard|59|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2090|1|", "eventKeyboard|2090|0|", "eventKeyboard|2090|1|",
                        "eventKeyboard|2090|0|", "eventKeyboard|2090|1|", "eventKeyboard|2090|0|",
                        "eventKeyboard|2090|1|", "eventKeyboard|2090|0|", "eventKeyboard|2090|1|",
                        "eventKeyboard|2090|0|", "eventKeyboard|2090|1|", "eventKeyboard|2090|0|",
                        "eventKeyboard|2090|1|", "eventKeyboard|2090|0|", "eventKeyboard|2090|1|",
                        "eventKeyboard|2090|0|", "eventKeyboard|2090|1|", "eventKeyboard|2090|0|",
                        "eventKeyboard|2090|1|", "eventKeyboard|2090|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_060_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_060_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_060_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json", "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json",
                    "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json", "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json",
                    "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json", "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json",
                    "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json", "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json",
                    "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json", "keyboard_KEY_F2_press.json", "keyboard_KEY_F2_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|60|1", "eventKeyboard|60|0", "eventKeyboard|60|1",
                             "eventKeyboard|60|0", "eventKeyboard|60|1", "eventKeyboard|60|0",
                             "eventKeyboard|60|1", "eventKeyboard|60|0", "eventKeyboard|60|1",
                             "eventKeyboard|60|0", "eventKeyboard|60|1", "eventKeyboard|60|0",
                             "eventKeyboard|60|1", "eventKeyboard|60|0", "eventKeyboard|60|1",
                             "eventKeyboard|60|0", "eventKeyboard|60|1", "eventKeyboard|60|0",
                             "eventKeyboard|60|1", "eventKeyboard|60|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2091|1|", "eventKeyboard|2091|0|", "eventKeyboard|2091|1|",
                        "eventKeyboard|2091|0|", "eventKeyboard|2091|1|", "eventKeyboard|2091|0|",
                        "eventKeyboard|2091|1|", "eventKeyboard|2091|0|", "eventKeyboard|2091|1|",
                        "eventKeyboard|2091|0|", "eventKeyboard|2091|1|", "eventKeyboard|2091|0|",
                        "eventKeyboard|2091|1|", "eventKeyboard|2091|0|", "eventKeyboard|2091|1|",
                        "eventKeyboard|2091|0|", "eventKeyboard|2091|1|", "eventKeyboard|2091|0|",
                        "eventKeyboard|2091|1|", "eventKeyboard|2091|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_061_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_061_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_061_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json", "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json",
                    "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json", "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json",
                    "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json", "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json",
                    "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json", "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json",
                    "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json", "keyboard_KEY_F3_press.json", "keyboard_KEY_F3_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|61|1", "eventKeyboard|61|0", "eventKeyboard|61|1",
                             "eventKeyboard|61|0", "eventKeyboard|61|1", "eventKeyboard|61|0",
                             "eventKeyboard|61|1", "eventKeyboard|61|0", "eventKeyboard|61|1",
                             "eventKeyboard|61|0", "eventKeyboard|61|1", "eventKeyboard|61|0",
                             "eventKeyboard|61|1", "eventKeyboard|61|0", "eventKeyboard|61|1",
                             "eventKeyboard|61|0", "eventKeyboard|61|1", "eventKeyboard|61|0",
                             "eventKeyboard|61|1", "eventKeyboard|61|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2092|1|", "eventKeyboard|2092|0|", "eventKeyboard|2092|1|",
                        "eventKeyboard|2092|0|", "eventKeyboard|2092|1|", "eventKeyboard|2092|0|",
                        "eventKeyboard|2092|1|", "eventKeyboard|2092|0|", "eventKeyboard|2092|1|",
                        "eventKeyboard|2092|0|", "eventKeyboard|2092|1|", "eventKeyboard|2092|0|",
                        "eventKeyboard|2092|1|", "eventKeyboard|2092|0|", "eventKeyboard|2092|1|",
                        "eventKeyboard|2092|0|", "eventKeyboard|2092|1|", "eventKeyboard|2092|0|",
                        "eventKeyboard|2092|1|", "eventKeyboard|2092|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_062_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_062_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_062_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json", "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json",
                    "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json", "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json",
                    "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json", "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json",
                    "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json", "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json",
                    "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json", "keyboard_KEY_F5_press.json", "keyboard_KEY_F5_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|63|1", "eventKeyboard|63|0", "eventKeyboard|63|1",
                             "eventKeyboard|63|0", "eventKeyboard|63|1", "eventKeyboard|63|0",
                             "eventKeyboard|63|1", "eventKeyboard|63|0", "eventKeyboard|63|1",
                             "eventKeyboard|63|0", "eventKeyboard|63|1", "eventKeyboard|63|0",
                             "eventKeyboard|63|1", "eventKeyboard|63|0", "eventKeyboard|63|1",
                             "eventKeyboard|63|0", "eventKeyboard|63|1", "eventKeyboard|63|0",
                             "eventKeyboard|63|1", "eventKeyboard|63|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2094|1|", "eventKeyboard|2094|0|", "eventKeyboard|2094|1|",
                        "eventKeyboard|2094|0|", "eventKeyboard|2094|1|", "eventKeyboard|2094|0|",
                        "eventKeyboard|2094|1|", "eventKeyboard|2094|0|", "eventKeyboard|2094|1|",
                        "eventKeyboard|2094|0|", "eventKeyboard|2094|1|", "eventKeyboard|2094|0|",
                        "eventKeyboard|2094|1|", "eventKeyboard|2094|0|", "eventKeyboard|2094|1|",
                        "eventKeyboard|2094|0|", "eventKeyboard|2094|1|", "eventKeyboard|2094|0|",
                        "eventKeyboard|2094|1|", "eventKeyboard|2094|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_063_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_063_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_063_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json", "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json",
                    "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json", "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json",
                    "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json", "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json",
                    "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json", "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json",
                    "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json", "keyboard_KEY_F6_press.json", "keyboard_KEY_F6_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|64|1", "eventKeyboard|64|0", "eventKeyboard|64|1",
                             "eventKeyboard|64|0", "eventKeyboard|64|1", "eventKeyboard|64|0",
                             "eventKeyboard|64|1", "eventKeyboard|64|0", "eventKeyboard|64|1",
                             "eventKeyboard|64|0", "eventKeyboard|64|1", "eventKeyboard|64|0",
                             "eventKeyboard|64|1", "eventKeyboard|64|0", "eventKeyboard|64|1",
                             "eventKeyboard|64|0", "eventKeyboard|64|1", "eventKeyboard|64|0",
                             "eventKeyboard|64|1", "eventKeyboard|64|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2095|1|", "eventKeyboard|2095|0|", "eventKeyboard|2095|1|",
                        "eventKeyboard|2095|0|", "eventKeyboard|2095|1|", "eventKeyboard|2095|0|",
                        "eventKeyboard|2095|1|", "eventKeyboard|2095|0|", "eventKeyboard|2095|1|",
                        "eventKeyboard|2095|0|", "eventKeyboard|2095|1|", "eventKeyboard|2095|0|",
                        "eventKeyboard|2095|1|", "eventKeyboard|2095|0|", "eventKeyboard|2095|1|",
                        "eventKeyboard|2095|0|", "eventKeyboard|2095|1|", "eventKeyboard|2095|0|",
                        "eventKeyboard|2095|1|", "eventKeyboard|2095|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_064_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_064_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_064_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json", "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json",
                    "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json", "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json",
                    "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json", "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json",
                    "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json", "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json",
                    "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json", "keyboard_KEY_F7_press.json", "keyboard_KEY_F7_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|65|1", "eventKeyboard|65|0", "eventKeyboard|65|1",
                             "eventKeyboard|65|0", "eventKeyboard|65|1", "eventKeyboard|65|0",
                             "eventKeyboard|65|1", "eventKeyboard|65|0", "eventKeyboard|65|1",
                             "eventKeyboard|65|0", "eventKeyboard|65|1", "eventKeyboard|65|0",
                             "eventKeyboard|65|1", "eventKeyboard|65|0", "eventKeyboard|65|1",
                             "eventKeyboard|65|0", "eventKeyboard|65|1", "eventKeyboard|65|0",
                             "eventKeyboard|65|1", "eventKeyboard|65|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2096|1|", "eventKeyboard|2096|0|", "eventKeyboard|2096|1|",
                        "eventKeyboard|2096|0|", "eventKeyboard|2096|1|", "eventKeyboard|2096|0|",
                        "eventKeyboard|2096|1|", "eventKeyboard|2096|0|", "eventKeyboard|2096|1|",
                        "eventKeyboard|2096|0|", "eventKeyboard|2096|1|", "eventKeyboard|2096|0|",
                        "eventKeyboard|2096|1|", "eventKeyboard|2096|0|", "eventKeyboard|2096|1|",
                        "eventKeyboard|2096|0|", "eventKeyboard|2096|1|", "eventKeyboard|2096|0|",
                        "eventKeyboard|2096|1|", "eventKeyboard|2096|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_065_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_065_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_065_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json", "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json",
                    "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json", "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json",
                    "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json", "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json",
                    "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json", "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json",
                    "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json", "keyboard_KEY_F8_press.json", "keyboard_KEY_F8_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|66|1", "eventKeyboard|66|0", "eventKeyboard|66|1",
                             "eventKeyboard|66|0", "eventKeyboard|66|1", "eventKeyboard|66|0",
                             "eventKeyboard|66|1", "eventKeyboard|66|0", "eventKeyboard|66|1",
                             "eventKeyboard|66|0", "eventKeyboard|66|1", "eventKeyboard|66|0",
                             "eventKeyboard|66|1", "eventKeyboard|66|0", "eventKeyboard|66|1",
                             "eventKeyboard|66|0", "eventKeyboard|66|1", "eventKeyboard|66|0",
                             "eventKeyboard|66|1", "eventKeyboard|66|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2097|1|", "eventKeyboard|2097|0|", "eventKeyboard|2097|1|",
                        "eventKeyboard|2097|0|", "eventKeyboard|2097|1|", "eventKeyboard|2097|0|",
                        "eventKeyboard|2097|1|", "eventKeyboard|2097|0|", "eventKeyboard|2097|1|",
                        "eventKeyboard|2097|0|", "eventKeyboard|2097|1|", "eventKeyboard|2097|0|",
                        "eventKeyboard|2097|1|", "eventKeyboard|2097|0|", "eventKeyboard|2097|1|",
                        "eventKeyboard|2097|0|", "eventKeyboard|2097|1|", "eventKeyboard|2097|0|",
                        "eventKeyboard|2097|1|", "eventKeyboard|2097|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_066_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_066_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_066_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json", "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json",
                    "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json", "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json",
                    "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json", "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json",
                    "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json", "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json",
                    "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json", "keyboard_KEY_F9_press.json", "keyboard_KEY_F9_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|67|1", "eventKeyboard|67|0", "eventKeyboard|67|1",
                             "eventKeyboard|67|0", "eventKeyboard|67|1", "eventKeyboard|67|0",
                             "eventKeyboard|67|1", "eventKeyboard|67|0", "eventKeyboard|67|1",
                             "eventKeyboard|67|0", "eventKeyboard|67|1", "eventKeyboard|67|0",
                             "eventKeyboard|67|1", "eventKeyboard|67|0", "eventKeyboard|67|1",
                             "eventKeyboard|67|0", "eventKeyboard|67|1", "eventKeyboard|67|0",
                             "eventKeyboard|67|1", "eventKeyboard|67|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2098|1|", "eventKeyboard|2098|0|", "eventKeyboard|2098|1|",
                        "eventKeyboard|2098|0|", "eventKeyboard|2098|1|", "eventKeyboard|2098|0|",
                        "eventKeyboard|2098|1|", "eventKeyboard|2098|0|", "eventKeyboard|2098|1|",
                        "eventKeyboard|2098|0|", "eventKeyboard|2098|1|", "eventKeyboard|2098|0|",
                        "eventKeyboard|2098|1|", "eventKeyboard|2098|0|", "eventKeyboard|2098|1|",
                        "eventKeyboard|2098|0|", "eventKeyboard|2098|1|", "eventKeyboard|2098|0|",
                        "eventKeyboard|2098|1|", "eventKeyboard|2098|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_067_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_067_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_067_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json",
                    "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json",
                    "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json",
                    "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json",
                    "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json", "keyboard_KEY_F10_press.json", "keyboard_KEY_F10_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|68|1", "eventKeyboard|68|0", "eventKeyboard|68|1",
                             "eventKeyboard|68|0", "eventKeyboard|68|1", "eventKeyboard|68|0",
                             "eventKeyboard|68|1", "eventKeyboard|68|0", "eventKeyboard|68|1",
                             "eventKeyboard|68|0", "eventKeyboard|68|1", "eventKeyboard|68|0",
                             "eventKeyboard|68|1", "eventKeyboard|68|0", "eventKeyboard|68|1",
                             "eventKeyboard|68|0", "eventKeyboard|68|1", "eventKeyboard|68|0",
                             "eventKeyboard|68|1", "eventKeyboard|68|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2099|1|", "eventKeyboard|2099|0|", "eventKeyboard|2099|1|",
                        "eventKeyboard|2099|0|", "eventKeyboard|2099|1|", "eventKeyboard|2099|0|",
                        "eventKeyboard|2099|1|", "eventKeyboard|2099|0|", "eventKeyboard|2099|1|",
                        "eventKeyboard|2099|0|", "eventKeyboard|2099|1|", "eventKeyboard|2099|0|",
                        "eventKeyboard|2099|1|", "eventKeyboard|2099|0|", "eventKeyboard|2099|1|",
                        "eventKeyboard|2099|0|", "eventKeyboard|2099|1|", "eventKeyboard|2099|0|",
                        "eventKeyboard|2099|1|", "eventKeyboard|2099|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_068_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_068_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_068_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json", "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json",
                    "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json", "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json",
                    "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json", "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json",
                    "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json", "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json",
                    "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json", "keyboard_KEY_F12_press.json", "keyboard_KEY_F12_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|88|1", "eventKeyboard|88|0", "eventKeyboard|88|1",
                             "eventKeyboard|88|0", "eventKeyboard|88|1", "eventKeyboard|88|0",
                             "eventKeyboard|88|1", "eventKeyboard|88|0", "eventKeyboard|88|1",
                             "eventKeyboard|88|0", "eventKeyboard|88|1", "eventKeyboard|88|0",
                             "eventKeyboard|88|1", "eventKeyboard|88|0", "eventKeyboard|88|1",
                             "eventKeyboard|88|0", "eventKeyboard|88|1", "eventKeyboard|88|0",
                             "eventKeyboard|88|1", "eventKeyboard|88|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2101|1|", "eventKeyboard|2101|0|", "eventKeyboard|2101|1|",
                        "eventKeyboard|2101|0|", "eventKeyboard|2101|1|", "eventKeyboard|2101|0|",
                        "eventKeyboard|2101|1|", "eventKeyboard|2101|0|", "eventKeyboard|2101|1|",
                        "eventKeyboard|2101|0|", "eventKeyboard|2101|1|", "eventKeyboard|2101|0|",
                        "eventKeyboard|2101|1|", "eventKeyboard|2101|0|", "eventKeyboard|2101|1|",
                        "eventKeyboard|2101|0|", "eventKeyboard|2101|1|", "eventKeyboard|2101|0|",
                        "eventKeyboard|2101|1|", "eventKeyboard|2101|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_069_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_069_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_069_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json", "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json",
                    "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json", "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json",
                    "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json", "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json",
                    "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json", "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json",
                    "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json", "keyboard_KEY_SYSRQ_press.json", "keyboard_KEY_SYSRQ_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|99|1", "eventKeyboard|99|0", "eventKeyboard|99|1",
                             "eventKeyboard|99|0", "eventKeyboard|99|1", "eventKeyboard|99|0",
                             "eventKeyboard|99|1", "eventKeyboard|99|0", "eventKeyboard|99|1",
                             "eventKeyboard|99|0", "eventKeyboard|99|1", "eventKeyboard|99|0",
                             "eventKeyboard|99|1", "eventKeyboard|99|0", "eventKeyboard|99|1",
                             "eventKeyboard|99|0", "eventKeyboard|99|1", "eventKeyboard|99|0",
                             "eventKeyboard|99|1", "eventKeyboard|99|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2079|1|", "eventKeyboard|2079|0|", "eventKeyboard|2079|1|",
                        "eventKeyboard|2079|0|", "eventKeyboard|2079|1|", "eventKeyboard|2079|0|",
                        "eventKeyboard|2079|1|", "eventKeyboard|2079|0|", "eventKeyboard|2079|1|",
                        "eventKeyboard|2079|0|", "eventKeyboard|2079|1|", "eventKeyboard|2079|0|",
                        "eventKeyboard|2079|1|", "eventKeyboard|2079|0|", "eventKeyboard|2079|1|",
                        "eventKeyboard|2079|0|", "eventKeyboard|2079|1|", "eventKeyboard|2079|0|",
                        "eventKeyboard|2079|1|", "eventKeyboard|2079|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_070_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_070_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_070_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json", "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json",
                    "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json", "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json",
                    "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json", "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json",
                    "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json", "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json",
                    "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json", "keyboard_KEY_SCROLL_LOCK_press.json", "keyboard_KEY_SCROLL_LOCK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|70|1", "eventKeyboard|70|0", "eventKeyboard|70|1",
                             "eventKeyboard|70|0", "eventKeyboard|70|1", "eventKeyboard|70|0",
                             "eventKeyboard|70|1", "eventKeyboard|70|0", "eventKeyboard|70|1",
                             "eventKeyboard|70|0", "eventKeyboard|70|1", "eventKeyboard|70|0",
                             "eventKeyboard|70|1", "eventKeyboard|70|0", "eventKeyboard|70|1",
                             "eventKeyboard|70|0", "eventKeyboard|70|1", "eventKeyboard|70|0",
                             "eventKeyboard|70|1", "eventKeyboard|70|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2075|1|", "eventKeyboard|2075|0|", "eventKeyboard|2075|1|",
                        "eventKeyboard|2075|0|", "eventKeyboard|2075|1|", "eventKeyboard|2075|0|",
                        "eventKeyboard|2075|1|", "eventKeyboard|2075|0|", "eventKeyboard|2075|1|",
                        "eventKeyboard|2075|0|", "eventKeyboard|2075|1|", "eventKeyboard|2075|0|",
                        "eventKeyboard|2075|1|", "eventKeyboard|2075|0|", "eventKeyboard|2075|1|",
                        "eventKeyboard|2075|0|", "eventKeyboard|2075|1|", "eventKeyboard|2075|0|",
                        "eventKeyboard|2075|1|", "eventKeyboard|2075|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_071_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_071_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_071_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json", "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json",
                    "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json", "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json",
                    "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json", "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json",
                    "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json", "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json",
                    "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json", "keyboard_KEY_BREAK_press.json", "keyboard_KEY_BREAK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|119|1", "eventKeyboard|119|0", "eventKeyboard|119|1",
                             "eventKeyboard|119|0", "eventKeyboard|119|1", "eventKeyboard|119|0",
                             "eventKeyboard|119|1", "eventKeyboard|119|0", "eventKeyboard|119|1",
                             "eventKeyboard|119|0", "eventKeyboard|119|1", "eventKeyboard|119|0",
                             "eventKeyboard|119|1", "eventKeyboard|119|0", "eventKeyboard|119|1",
                             "eventKeyboard|119|0", "eventKeyboard|119|1", "eventKeyboard|119|0",
                             "eventKeyboard|119|1", "eventKeyboard|119|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2080|1|", "eventKeyboard|2080|0|", "eventKeyboard|2080|1|",
                        "eventKeyboard|2080|0|", "eventKeyboard|2080|1|", "eventKeyboard|2080|0|",
                        "eventKeyboard|2080|1|", "eventKeyboard|2080|0|", "eventKeyboard|2080|1|",
                        "eventKeyboard|2080|0|", "eventKeyboard|2080|1|", "eventKeyboard|2080|0|",
                        "eventKeyboard|2080|1|", "eventKeyboard|2080|0|", "eventKeyboard|2080|1|",
                        "eventKeyboard|2080|0|", "eventKeyboard|2080|1|", "eventKeyboard|2080|0|",
                        "eventKeyboard|2080|1|", "eventKeyboard|2080|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_072_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_072_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_072_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json", "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json",
                    "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json", "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json",
                    "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json", "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json",
                    "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json", "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json",
                    "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json", "keyboard_KEY_INSERT_press.json", "keyboard_KEY_INSERT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|110|1", "eventKeyboard|110|0", "eventKeyboard|110|1",
                             "eventKeyboard|110|0", "eventKeyboard|110|1", "eventKeyboard|110|0",
                             "eventKeyboard|110|1", "eventKeyboard|110|0", "eventKeyboard|110|1",
                             "eventKeyboard|110|0", "eventKeyboard|110|1", "eventKeyboard|110|0",
                             "eventKeyboard|110|1", "eventKeyboard|110|0", "eventKeyboard|110|1",
                             "eventKeyboard|110|0", "eventKeyboard|110|1", "eventKeyboard|110|0",
                             "eventKeyboard|110|1", "eventKeyboard|110|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2083|1|", "eventKeyboard|2083|0|", "eventKeyboard|2083|1|",
                        "eventKeyboard|2083|0|", "eventKeyboard|2083|1|", "eventKeyboard|2083|0|",
                        "eventKeyboard|2083|1|", "eventKeyboard|2083|0|", "eventKeyboard|2083|1|",
                        "eventKeyboard|2083|0|", "eventKeyboard|2083|1|", "eventKeyboard|2083|0|",
                        "eventKeyboard|2083|1|", "eventKeyboard|2083|0|", "eventKeyboard|2083|1|",
                        "eventKeyboard|2083|0|", "eventKeyboard|2083|1|", "eventKeyboard|2083|0|",
                        "eventKeyboard|2083|1|", "eventKeyboard|2083|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_073_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_073_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_073_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json", "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json",
                    "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json", "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json",
                    "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json", "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json",
                    "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json", "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json",
                    "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json", "keyboard_KEY_MOVE_HOME_press.json", "keyboard_KEY_MOVE_HOME_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|102|1", "eventKeyboard|102|0", "eventKeyboard|102|1",
                             "eventKeyboard|102|0", "eventKeyboard|102|1", "eventKeyboard|102|0",
                             "eventKeyboard|102|1", "eventKeyboard|102|0", "eventKeyboard|102|1",
                             "eventKeyboard|102|0", "eventKeyboard|102|1", "eventKeyboard|102|0",
                             "eventKeyboard|102|1", "eventKeyboard|102|0", "eventKeyboard|102|1",
                             "eventKeyboard|102|0", "eventKeyboard|102|1", "eventKeyboard|102|0",
                             "eventKeyboard|102|1", "eventKeyboard|102|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2081|1|", "eventKeyboard|2081|0|", "eventKeyboard|2081|1|",
                        "eventKeyboard|2081|0|", "eventKeyboard|2081|1|", "eventKeyboard|2081|0|",
                        "eventKeyboard|2081|1|", "eventKeyboard|2081|0|", "eventKeyboard|2081|1|",
                        "eventKeyboard|2081|0|", "eventKeyboard|2081|1|", "eventKeyboard|2081|0|",
                        "eventKeyboard|2081|1|", "eventKeyboard|2081|0|", "eventKeyboard|2081|1|",
                        "eventKeyboard|2081|0|", "eventKeyboard|2081|1|", "eventKeyboard|2081|0|",
                        "eventKeyboard|2081|1|", "eventKeyboard|2081|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_074_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_074_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_074_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json", "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json",
                    "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json", "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json",
                    "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json", "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json",
                    "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json", "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json",
                    "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json", "keyboard_KEY_FORWARD_DEL_press.json", "keyboard_KEY_FORWARD_DEL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|111|1", "eventKeyboard|111|0", "eventKeyboard|111|1",
                             "eventKeyboard|111|0", "eventKeyboard|111|1", "eventKeyboard|111|0",
                             "eventKeyboard|111|1", "eventKeyboard|111|0", "eventKeyboard|111|1",
                             "eventKeyboard|111|0", "eventKeyboard|111|1", "eventKeyboard|111|0",
                             "eventKeyboard|111|1", "eventKeyboard|111|0", "eventKeyboard|111|1",
                             "eventKeyboard|111|0", "eventKeyboard|111|1", "eventKeyboard|111|0",
                             "eventKeyboard|111|1", "eventKeyboard|111|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2071|1|", "eventKeyboard|2071|0|", "eventKeyboard|2071|1|",
                        "eventKeyboard|2071|0|", "eventKeyboard|2071|1|", "eventKeyboard|2071|0|",
                        "eventKeyboard|2071|1|", "eventKeyboard|2071|0|", "eventKeyboard|2071|1|",
                        "eventKeyboard|2071|0|", "eventKeyboard|2071|1|", "eventKeyboard|2071|0|",
                        "eventKeyboard|2071|1|", "eventKeyboard|2071|0|", "eventKeyboard|2071|1|",
                        "eventKeyboard|2071|0|", "eventKeyboard|2071|1|", "eventKeyboard|2071|0|",
                        "eventKeyboard|2071|1|", "eventKeyboard|2071|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_075_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_075_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_075_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json", "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json",
                    "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json", "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json",
                    "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json", "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json",
                    "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json", "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json",
                    "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json", "keyboard_KEY_MOVE_END_press.json", "keyboard_KEY_MOVE_END_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|107|1", "eventKeyboard|107|0", "eventKeyboard|107|1",
                             "eventKeyboard|107|0", "eventKeyboard|107|1", "eventKeyboard|107|0",
                             "eventKeyboard|107|1", "eventKeyboard|107|0", "eventKeyboard|107|1",
                             "eventKeyboard|107|0", "eventKeyboard|107|1", "eventKeyboard|107|0",
                             "eventKeyboard|107|1", "eventKeyboard|107|0", "eventKeyboard|107|1",
                             "eventKeyboard|107|0", "eventKeyboard|107|1", "eventKeyboard|107|0",
                             "eventKeyboard|107|1", "eventKeyboard|107|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2082|1|", "eventKeyboard|2082|0|", "eventKeyboard|2082|1|",
                        "eventKeyboard|2082|0|", "eventKeyboard|2082|1|", "eventKeyboard|2082|0|",
                        "eventKeyboard|2082|1|", "eventKeyboard|2082|0|", "eventKeyboard|2082|1|",
                        "eventKeyboard|2082|0|", "eventKeyboard|2082|1|", "eventKeyboard|2082|0|",
                        "eventKeyboard|2082|1|", "eventKeyboard|2082|0|", "eventKeyboard|2082|1|",
                        "eventKeyboard|2082|0|", "eventKeyboard|2082|1|", "eventKeyboard|2082|0|",
                        "eventKeyboard|2082|1|", "eventKeyboard|2082|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_076_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_076_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_076_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json", "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json",
                    "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json", "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json",
                    "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json", "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json",
                    "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json", "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json",
                    "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json", "keyboard_KEY_NUM_LOCK_press.json", "keyboard_KEY_NUM_LOCK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|69|1", "eventKeyboard|69|0", "eventKeyboard|69|1",
                             "eventKeyboard|69|0", "eventKeyboard|69|1", "eventKeyboard|69|0",
                             "eventKeyboard|69|1", "eventKeyboard|69|0", "eventKeyboard|69|1",
                             "eventKeyboard|69|0", "eventKeyboard|69|1", "eventKeyboard|69|0",
                             "eventKeyboard|69|1", "eventKeyboard|69|0", "eventKeyboard|69|1",
                             "eventKeyboard|69|0", "eventKeyboard|69|1", "eventKeyboard|69|0",
                             "eventKeyboard|69|1", "eventKeyboard|69|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2102|1|", "eventKeyboard|2102|0|", "eventKeyboard|2102|1|",
                        "eventKeyboard|2102|0|", "eventKeyboard|2102|1|", "eventKeyboard|2102|0|",
                        "eventKeyboard|2102|1|", "eventKeyboard|2102|0|", "eventKeyboard|2102|1|",
                        "eventKeyboard|2102|0|", "eventKeyboard|2102|1|", "eventKeyboard|2102|0|",
                        "eventKeyboard|2102|1|", "eventKeyboard|2102|0|", "eventKeyboard|2102|1|",
                        "eventKeyboard|2102|0|", "eventKeyboard|2102|1|", "eventKeyboard|2102|0|",
                        "eventKeyboard|2102|1|", "eventKeyboard|2102|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_077_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_077_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_077_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json", "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json",
                    "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json", "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json",
                    "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json", "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json",
                    "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json", "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json",
                    "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json", "keyboard_KEY_NUMPAD_DIVIDE_press.json", "keyboard_KEY_NUMPAD_DIVIDE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|98|1", "eventKeyboard|98|0", "eventKeyboard|98|1",
                             "eventKeyboard|98|0", "eventKeyboard|98|1", "eventKeyboard|98|0",
                             "eventKeyboard|98|1", "eventKeyboard|98|0", "eventKeyboard|98|1",
                             "eventKeyboard|98|0", "eventKeyboard|98|1", "eventKeyboard|98|0",
                             "eventKeyboard|98|1", "eventKeyboard|98|0", "eventKeyboard|98|1",
                             "eventKeyboard|98|0", "eventKeyboard|98|1", "eventKeyboard|98|0",
                             "eventKeyboard|98|1", "eventKeyboard|98|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2113|1|", "eventKeyboard|2113|0|", "eventKeyboard|2113|1|",
                        "eventKeyboard|2113|0|", "eventKeyboard|2113|1|", "eventKeyboard|2113|0|",
                        "eventKeyboard|2113|1|", "eventKeyboard|2113|0|", "eventKeyboard|2113|1|",
                        "eventKeyboard|2113|0|", "eventKeyboard|2113|1|", "eventKeyboard|2113|0|",
                        "eventKeyboard|2113|1|", "eventKeyboard|2113|0|", "eventKeyboard|2113|1|",
                        "eventKeyboard|2113|0|", "eventKeyboard|2113|1|", "eventKeyboard|2113|0|",
                        "eventKeyboard|2113|1|", "eventKeyboard|2113|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_078_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_078_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_078_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json", "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json",
                    "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json", "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json",
                    "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json", "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json",
                    "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json", "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json",
                    "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json", "keyboard_KEY_NUMPAD_MULTIPLY_press.json", "keyboard_KEY_NUMPAD_MULTIPLY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|55|1", "eventKeyboard|55|0", "eventKeyboard|55|1",
                             "eventKeyboard|55|0", "eventKeyboard|55|1", "eventKeyboard|55|0",
                             "eventKeyboard|55|1", "eventKeyboard|55|0", "eventKeyboard|55|1",
                             "eventKeyboard|55|0", "eventKeyboard|55|1", "eventKeyboard|55|0",
                             "eventKeyboard|55|1", "eventKeyboard|55|0", "eventKeyboard|55|1",
                             "eventKeyboard|55|0", "eventKeyboard|55|1", "eventKeyboard|55|0",
                             "eventKeyboard|55|1", "eventKeyboard|55|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2114|1|", "eventKeyboard|2114|0|", "eventKeyboard|2114|1|",
                        "eventKeyboard|2114|0|", "eventKeyboard|2114|1|", "eventKeyboard|2114|0|",
                        "eventKeyboard|2114|1|", "eventKeyboard|2114|0|", "eventKeyboard|2114|1|",
                        "eventKeyboard|2114|0|", "eventKeyboard|2114|1|", "eventKeyboard|2114|0|",
                        "eventKeyboard|2114|1|", "eventKeyboard|2114|0|", "eventKeyboard|2114|1|",
                        "eventKeyboard|2114|0|", "eventKeyboard|2114|1|", "eventKeyboard|2114|0|",
                        "eventKeyboard|2114|1|", "eventKeyboard|2114|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_079_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_079_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_079_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json", "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json",
                    "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json", "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json",
                    "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json", "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json",
                    "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json", "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json",
                    "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json", "keyboard_KEY_NUMPAD_SUBTRACT_press.json", "keyboard_KEY_NUMPAD_SUBTRACT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|74|1", "eventKeyboard|74|0", "eventKeyboard|74|1",
                             "eventKeyboard|74|0", "eventKeyboard|74|1", "eventKeyboard|74|0",
                             "eventKeyboard|74|1", "eventKeyboard|74|0", "eventKeyboard|74|1",
                             "eventKeyboard|74|0", "eventKeyboard|74|1", "eventKeyboard|74|0",
                             "eventKeyboard|74|1", "eventKeyboard|74|0", "eventKeyboard|74|1",
                             "eventKeyboard|74|0", "eventKeyboard|74|1", "eventKeyboard|74|0",
                             "eventKeyboard|74|1", "eventKeyboard|74|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2115|1|", "eventKeyboard|2115|0|", "eventKeyboard|2115|1|",
                        "eventKeyboard|2115|0|", "eventKeyboard|2115|1|", "eventKeyboard|2115|0|",
                        "eventKeyboard|2115|1|", "eventKeyboard|2115|0|", "eventKeyboard|2115|1|",
                        "eventKeyboard|2115|0|", "eventKeyboard|2115|1|", "eventKeyboard|2115|0|",
                        "eventKeyboard|2115|1|", "eventKeyboard|2115|0|", "eventKeyboard|2115|1|",
                        "eventKeyboard|2115|0|", "eventKeyboard|2115|1|", "eventKeyboard|2115|0|",
                        "eventKeyboard|2115|1|", "eventKeyboard|2115|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_080_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_080_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_080_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json", "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json",
                    "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json", "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json",
                    "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json", "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json",
                    "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json", "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json",
                    "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json", "keyboard_KEY_NUMPAD_7_press.json", "keyboard_KEY_NUMPAD_7_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|71|1", "eventKeyboard|71|0", "eventKeyboard|71|1",
                             "eventKeyboard|71|0", "eventKeyboard|71|1", "eventKeyboard|71|0",
                             "eventKeyboard|71|1", "eventKeyboard|71|0", "eventKeyboard|71|1",
                             "eventKeyboard|71|0", "eventKeyboard|71|1", "eventKeyboard|71|0",
                             "eventKeyboard|71|1", "eventKeyboard|71|0", "eventKeyboard|71|1",
                             "eventKeyboard|71|0", "eventKeyboard|71|1", "eventKeyboard|71|0",
                             "eventKeyboard|71|1", "eventKeyboard|71|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2110|1|", "eventKeyboard|2110|0|", "eventKeyboard|2110|1|",
                        "eventKeyboard|2110|0|", "eventKeyboard|2110|1|", "eventKeyboard|2110|0|",
                        "eventKeyboard|2110|1|", "eventKeyboard|2110|0|", "eventKeyboard|2110|1|",
                        "eventKeyboard|2110|0|", "eventKeyboard|2110|1|", "eventKeyboard|2110|0|",
                        "eventKeyboard|2110|1|", "eventKeyboard|2110|0|", "eventKeyboard|2110|1|",
                        "eventKeyboard|2110|0|", "eventKeyboard|2110|1|", "eventKeyboard|2110|0|",
                        "eventKeyboard|2110|1|", "eventKeyboard|2110|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_081_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_081_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_081_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json", "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json",
                    "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json", "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json",
                    "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json", "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json",
                    "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json", "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json",
                    "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json", "keyboard_KEY_NUMPAD_8_press.json", "keyboard_KEY_NUMPAD_8_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|72|1", "eventKeyboard|72|0", "eventKeyboard|72|1",
                             "eventKeyboard|72|0", "eventKeyboard|72|1", "eventKeyboard|72|0",
                             "eventKeyboard|72|1", "eventKeyboard|72|0", "eventKeyboard|72|1",
                             "eventKeyboard|72|0", "eventKeyboard|72|1", "eventKeyboard|72|0",
                             "eventKeyboard|72|1", "eventKeyboard|72|0", "eventKeyboard|72|1",
                             "eventKeyboard|72|0", "eventKeyboard|72|1", "eventKeyboard|72|0",
                             "eventKeyboard|72|1", "eventKeyboard|72|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2111|1|", "eventKeyboard|2111|0|", "eventKeyboard|2111|1|",
                        "eventKeyboard|2111|0|", "eventKeyboard|2111|1|", "eventKeyboard|2111|0|",
                        "eventKeyboard|2111|1|", "eventKeyboard|2111|0|", "eventKeyboard|2111|1|",
                        "eventKeyboard|2111|0|", "eventKeyboard|2111|1|", "eventKeyboard|2111|0|",
                        "eventKeyboard|2111|1|", "eventKeyboard|2111|0|", "eventKeyboard|2111|1|",
                        "eventKeyboard|2111|0|", "eventKeyboard|2111|1|", "eventKeyboard|2111|0|",
                        "eventKeyboard|2111|1|", "eventKeyboard|2111|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_082_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_082_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_082_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json", "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json",
                    "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json", "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json",
                    "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json", "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json",
                    "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json", "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json",
                    "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json", "keyboard_KEY_NUMPAD_9_press.json", "keyboard_KEY_NUMPAD_9_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|73|1", "eventKeyboard|73|0", "eventKeyboard|73|1",
                             "eventKeyboard|73|0", "eventKeyboard|73|1", "eventKeyboard|73|0",
                             "eventKeyboard|73|1", "eventKeyboard|73|0", "eventKeyboard|73|1",
                             "eventKeyboard|73|0", "eventKeyboard|73|1", "eventKeyboard|73|0",
                             "eventKeyboard|73|1", "eventKeyboard|73|0", "eventKeyboard|73|1",
                             "eventKeyboard|73|0", "eventKeyboard|73|1", "eventKeyboard|73|0",
                             "eventKeyboard|73|1", "eventKeyboard|73|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2112|1|", "eventKeyboard|2112|0|", "eventKeyboard|2112|1|",
                        "eventKeyboard|2112|0|", "eventKeyboard|2112|1|", "eventKeyboard|2112|0|",
                        "eventKeyboard|2112|1|", "eventKeyboard|2112|0|", "eventKeyboard|2112|1|",
                        "eventKeyboard|2112|0|", "eventKeyboard|2112|1|", "eventKeyboard|2112|0|",
                        "eventKeyboard|2112|1|", "eventKeyboard|2112|0|", "eventKeyboard|2112|1|",
                        "eventKeyboard|2112|0|", "eventKeyboard|2112|1|", "eventKeyboard|2112|0|",
                        "eventKeyboard|2112|1|", "eventKeyboard|2112|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_083_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_083_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_083_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json", "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json",
                    "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json", "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json",
                    "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json", "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json",
                    "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json", "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json",
                    "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json", "keyboard_KEY_NUMPAD_4_press.json", "keyboard_KEY_NUMPAD_4_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|75|1", "eventKeyboard|75|0", "eventKeyboard|75|1",
                             "eventKeyboard|75|0", "eventKeyboard|75|1", "eventKeyboard|75|0",
                             "eventKeyboard|75|1", "eventKeyboard|75|0", "eventKeyboard|75|1",
                             "eventKeyboard|75|0", "eventKeyboard|75|1", "eventKeyboard|75|0",
                             "eventKeyboard|75|1", "eventKeyboard|75|0", "eventKeyboard|75|1",
                             "eventKeyboard|75|0", "eventKeyboard|75|1", "eventKeyboard|75|0",
                             "eventKeyboard|75|1", "eventKeyboard|75|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2107|1|", "eventKeyboard|2107|0|", "eventKeyboard|2107|1|",
                        "eventKeyboard|2107|0|", "eventKeyboard|2107|1|", "eventKeyboard|2107|0|",
                        "eventKeyboard|2107|1|", "eventKeyboard|2107|0|", "eventKeyboard|2107|1|",
                        "eventKeyboard|2107|0|", "eventKeyboard|2107|1|", "eventKeyboard|2107|0|",
                        "eventKeyboard|2107|1|", "eventKeyboard|2107|0|", "eventKeyboard|2107|1|",
                        "eventKeyboard|2107|0|", "eventKeyboard|2107|1|", "eventKeyboard|2107|0|",
                        "eventKeyboard|2107|1|", "eventKeyboard|2107|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_084_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_084_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_084_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json", "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json",
                    "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json", "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json",
                    "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json", "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json",
                    "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json", "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json",
                    "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json", "keyboard_KEY_NUMPAD_5_press.json", "keyboard_KEY_NUMPAD_5_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|76|1", "eventKeyboard|76|0", "eventKeyboard|76|1",
                             "eventKeyboard|76|0", "eventKeyboard|76|1", "eventKeyboard|76|0",
                             "eventKeyboard|76|1", "eventKeyboard|76|0", "eventKeyboard|76|1",
                             "eventKeyboard|76|0", "eventKeyboard|76|1", "eventKeyboard|76|0",
                             "eventKeyboard|76|1", "eventKeyboard|76|0", "eventKeyboard|76|1",
                             "eventKeyboard|76|0", "eventKeyboard|76|1", "eventKeyboard|76|0",
                             "eventKeyboard|76|1", "eventKeyboard|76|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2108|1|", "eventKeyboard|2108|0|", "eventKeyboard|2108|1|",
                        "eventKeyboard|2108|0|", "eventKeyboard|2108|1|", "eventKeyboard|2108|0|",
                        "eventKeyboard|2108|1|", "eventKeyboard|2108|0|", "eventKeyboard|2108|1|",
                        "eventKeyboard|2108|0|", "eventKeyboard|2108|1|", "eventKeyboard|2108|0|",
                        "eventKeyboard|2108|1|", "eventKeyboard|2108|0|", "eventKeyboard|2108|1|",
                        "eventKeyboard|2108|0|", "eventKeyboard|2108|1|", "eventKeyboard|2108|0|",
                        "eventKeyboard|2108|1|", "eventKeyboard|2108|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_085_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_085_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_085_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json", "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json",
                    "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json", "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json",
                    "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json", "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json",
                    "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json", "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json",
                    "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json", "keyboard_KEY_NUMPAD_6_press.json", "keyboard_KEY_NUMPAD_6_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|77|1", "eventKeyboard|77|0", "eventKeyboard|77|1",
                             "eventKeyboard|77|0", "eventKeyboard|77|1", "eventKeyboard|77|0",
                             "eventKeyboard|77|1", "eventKeyboard|77|0", "eventKeyboard|77|1",
                             "eventKeyboard|77|0", "eventKeyboard|77|1", "eventKeyboard|77|0",
                             "eventKeyboard|77|1", "eventKeyboard|77|0", "eventKeyboard|77|1",
                             "eventKeyboard|77|0", "eventKeyboard|77|1", "eventKeyboard|77|0",
                             "eventKeyboard|77|1", "eventKeyboard|77|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2109|1|", "eventKeyboard|2109|0|", "eventKeyboard|2109|1|",
                        "eventKeyboard|2109|0|", "eventKeyboard|2109|1|", "eventKeyboard|2109|0|",
                        "eventKeyboard|2109|1|", "eventKeyboard|2109|0|", "eventKeyboard|2109|1|",
                        "eventKeyboard|2109|0|", "eventKeyboard|2109|1|", "eventKeyboard|2109|0|",
                        "eventKeyboard|2109|1|", "eventKeyboard|2109|0|", "eventKeyboard|2109|1|",
                        "eventKeyboard|2109|0|", "eventKeyboard|2109|1|", "eventKeyboard|2109|0|",
                        "eventKeyboard|2109|1|", "eventKeyboard|2109|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_086_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_086_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_086_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json", "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json",
                    "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json", "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json",
                    "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json", "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json",
                    "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json", "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json",
                    "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json", "keyboard_KEY_NUMPAD_1_press.json", "keyboard_KEY_NUMPAD_1_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|79|1", "eventKeyboard|79|0", "eventKeyboard|79|1",
                             "eventKeyboard|79|0", "eventKeyboard|79|1", "eventKeyboard|79|0",
                             "eventKeyboard|79|1", "eventKeyboard|79|0", "eventKeyboard|79|1",
                             "eventKeyboard|79|0", "eventKeyboard|79|1", "eventKeyboard|79|0",
                             "eventKeyboard|79|1", "eventKeyboard|79|0", "eventKeyboard|79|1",
                             "eventKeyboard|79|0", "eventKeyboard|79|1", "eventKeyboard|79|0",
                             "eventKeyboard|79|1", "eventKeyboard|79|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2104|1|", "eventKeyboard|2104|0|", "eventKeyboard|2104|1|",
                        "eventKeyboard|2104|0|", "eventKeyboard|2104|1|", "eventKeyboard|2104|0|",
                        "eventKeyboard|2104|1|", "eventKeyboard|2104|0|", "eventKeyboard|2104|1|",
                        "eventKeyboard|2104|0|", "eventKeyboard|2104|1|", "eventKeyboard|2104|0|",
                        "eventKeyboard|2104|1|", "eventKeyboard|2104|0|", "eventKeyboard|2104|1|",
                        "eventKeyboard|2104|0|", "eventKeyboard|2104|1|", "eventKeyboard|2104|0|",
                        "eventKeyboard|2104|1|", "eventKeyboard|2104|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_087_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_087_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_087_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json", "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json",
                    "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json", "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json",
                    "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json", "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json",
                    "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json", "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json",
                    "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json", "keyboard_KEY_NUMPAD_2_press.json", "keyboard_KEY_NUMPAD_2_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|80|1", "eventKeyboard|80|0", "eventKeyboard|80|1",
                             "eventKeyboard|80|0", "eventKeyboard|80|1", "eventKeyboard|80|0",
                             "eventKeyboard|80|1", "eventKeyboard|80|0", "eventKeyboard|80|1",
                             "eventKeyboard|80|0", "eventKeyboard|80|1", "eventKeyboard|80|0",
                             "eventKeyboard|80|1", "eventKeyboard|80|0", "eventKeyboard|80|1",
                             "eventKeyboard|80|0", "eventKeyboard|80|1", "eventKeyboard|80|0",
                             "eventKeyboard|80|1", "eventKeyboard|80|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2105|1|", "eventKeyboard|2105|0|", "eventKeyboard|2105|1|",
                        "eventKeyboard|2105|0|", "eventKeyboard|2105|1|", "eventKeyboard|2105|0|",
                        "eventKeyboard|2105|1|", "eventKeyboard|2105|0|", "eventKeyboard|2105|1|",
                        "eventKeyboard|2105|0|", "eventKeyboard|2105|1|", "eventKeyboard|2105|0|",
                        "eventKeyboard|2105|1|", "eventKeyboard|2105|0|", "eventKeyboard|2105|1|",
                        "eventKeyboard|2105|0|", "eventKeyboard|2105|1|", "eventKeyboard|2105|0|",
                        "eventKeyboard|2105|1|", "eventKeyboard|2105|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_088_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_088_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_088_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json", "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json",
                    "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json", "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json",
                    "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json", "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json",
                    "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json", "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json",
                    "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json", "keyboard_KEY_NUMPAD_3_press.json", "keyboard_KEY_NUMPAD_3_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|81|1", "eventKeyboard|81|0", "eventKeyboard|81|1",
                             "eventKeyboard|81|0", "eventKeyboard|81|1", "eventKeyboard|81|0",
                             "eventKeyboard|81|1", "eventKeyboard|81|0", "eventKeyboard|81|1",
                             "eventKeyboard|81|0", "eventKeyboard|81|1", "eventKeyboard|81|0",
                             "eventKeyboard|81|1", "eventKeyboard|81|0", "eventKeyboard|81|1",
                             "eventKeyboard|81|0", "eventKeyboard|81|1", "eventKeyboard|81|0",
                             "eventKeyboard|81|1", "eventKeyboard|81|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2106|1|", "eventKeyboard|2106|0|", "eventKeyboard|2106|1|",
                        "eventKeyboard|2106|0|", "eventKeyboard|2106|1|", "eventKeyboard|2106|0|",
                        "eventKeyboard|2106|1|", "eventKeyboard|2106|0|", "eventKeyboard|2106|1|",
                        "eventKeyboard|2106|0|", "eventKeyboard|2106|1|", "eventKeyboard|2106|0|",
                        "eventKeyboard|2106|1|", "eventKeyboard|2106|0|", "eventKeyboard|2106|1|",
                        "eventKeyboard|2106|0|", "eventKeyboard|2106|1|", "eventKeyboard|2106|0|",
                        "eventKeyboard|2106|1|", "eventKeyboard|2106|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_089_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_089_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_089_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json", "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json",
                    "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json", "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json",
                    "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json", "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json",
                    "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json", "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json",
                    "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json", "keyboard_KEY_NUMPAD_ADD_press.json", "keyboard_KEY_NUMPAD_ADD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|78|1", "eventKeyboard|78|0", "eventKeyboard|78|1",
                             "eventKeyboard|78|0", "eventKeyboard|78|1", "eventKeyboard|78|0",
                             "eventKeyboard|78|1", "eventKeyboard|78|0", "eventKeyboard|78|1",
                             "eventKeyboard|78|0", "eventKeyboard|78|1", "eventKeyboard|78|0",
                             "eventKeyboard|78|1", "eventKeyboard|78|0", "eventKeyboard|78|1",
                             "eventKeyboard|78|0", "eventKeyboard|78|1", "eventKeyboard|78|0",
                             "eventKeyboard|78|1", "eventKeyboard|78|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2116|1|", "eventKeyboard|2116|0|", "eventKeyboard|2116|1|",
                        "eventKeyboard|2116|0|", "eventKeyboard|2116|1|", "eventKeyboard|2116|0|",
                        "eventKeyboard|2116|1|", "eventKeyboard|2116|0|", "eventKeyboard|2116|1|",
                        "eventKeyboard|2116|0|", "eventKeyboard|2116|1|", "eventKeyboard|2116|0|",
                        "eventKeyboard|2116|1|", "eventKeyboard|2116|0|", "eventKeyboard|2116|1|",
                        "eventKeyboard|2116|0|", "eventKeyboard|2116|1|", "eventKeyboard|2116|0|",
                        "eventKeyboard|2116|1|", "eventKeyboard|2116|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_090_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_090_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_090_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json", "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json",
                    "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json", "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json",
                    "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json", "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json",
                    "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json", "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json",
                    "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json", "keyboard_KEY_NUMPAD_0_press.json", "keyboard_KEY_NUMPAD_0_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|82|1", "eventKeyboard|82|0", "eventKeyboard|82|1",
                             "eventKeyboard|82|0", "eventKeyboard|82|1", "eventKeyboard|82|0",
                             "eventKeyboard|82|1", "eventKeyboard|82|0", "eventKeyboard|82|1",
                             "eventKeyboard|82|0", "eventKeyboard|82|1", "eventKeyboard|82|0",
                             "eventKeyboard|82|1", "eventKeyboard|82|0", "eventKeyboard|82|1",
                             "eventKeyboard|82|0", "eventKeyboard|82|1", "eventKeyboard|82|0",
                             "eventKeyboard|82|1", "eventKeyboard|82|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2103|1|", "eventKeyboard|2103|0|", "eventKeyboard|2103|1|",
                        "eventKeyboard|2103|0|", "eventKeyboard|2103|1|", "eventKeyboard|2103|0|",
                        "eventKeyboard|2103|1|", "eventKeyboard|2103|0|", "eventKeyboard|2103|1|",
                        "eventKeyboard|2103|0|", "eventKeyboard|2103|1|", "eventKeyboard|2103|0|",
                        "eventKeyboard|2103|1|", "eventKeyboard|2103|0|", "eventKeyboard|2103|1|",
                        "eventKeyboard|2103|0|", "eventKeyboard|2103|1|", "eventKeyboard|2103|0|",
                        "eventKeyboard|2103|1|", "eventKeyboard|2103|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_091_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_091_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_091_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json", "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json",
                    "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json", "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json",
                    "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json", "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json",
                    "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json", "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json",
                    "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json", "keyboard_KEY_NUMPAD_DOT_press.json", "keyboard_KEY_NUMPAD_DOT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|83|1", "eventKeyboard|83|0", "eventKeyboard|83|1",
                             "eventKeyboard|83|0", "eventKeyboard|83|1", "eventKeyboard|83|0",
                             "eventKeyboard|83|1", "eventKeyboard|83|0", "eventKeyboard|83|1",
                             "eventKeyboard|83|0", "eventKeyboard|83|1", "eventKeyboard|83|0",
                             "eventKeyboard|83|1", "eventKeyboard|83|0", "eventKeyboard|83|1",
                             "eventKeyboard|83|0", "eventKeyboard|83|1", "eventKeyboard|83|0",
                             "eventKeyboard|83|1", "eventKeyboard|83|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2117|1|", "eventKeyboard|2117|0|", "eventKeyboard|2117|1|",
                        "eventKeyboard|2117|0|", "eventKeyboard|2117|1|", "eventKeyboard|2117|0|",
                        "eventKeyboard|2117|1|", "eventKeyboard|2117|0|", "eventKeyboard|2117|1|",
                        "eventKeyboard|2117|0|", "eventKeyboard|2117|1|", "eventKeyboard|2117|0|",
                        "eventKeyboard|2117|1|", "eventKeyboard|2117|0|", "eventKeyboard|2117|1|",
                        "eventKeyboard|2117|0|", "eventKeyboard|2117|1|", "eventKeyboard|2117|0|",
                        "eventKeyboard|2117|1|", "eventKeyboard|2117|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_093_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_093_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_093_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json", "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json",
                    "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json", "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json",
                    "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json", "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json",
                    "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json", "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json",
                    "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json", "keyboard_KEY_ZENKAKUHANKAKU_press.json", "keyboard_KEY_ZENKAKUHANKAKU_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|85|1", "eventKeyboard|85|0", "eventKeyboard|85|1",
                             "eventKeyboard|85|0", "eventKeyboard|85|1", "eventKeyboard|85|0",
                             "eventKeyboard|85|1", "eventKeyboard|85|0", "eventKeyboard|85|1",
                             "eventKeyboard|85|0", "eventKeyboard|85|1", "eventKeyboard|85|0",
                             "eventKeyboard|85|1", "eventKeyboard|85|0", "eventKeyboard|85|1",
                             "eventKeyboard|85|0", "eventKeyboard|85|1", "eventKeyboard|85|0",
                             "eventKeyboard|85|1", "eventKeyboard|85|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2601|1|", "eventKeyboard|2601|0|", "eventKeyboard|2601|1|",
                        "eventKeyboard|2601|0|", "eventKeyboard|2601|1|", "eventKeyboard|2601|0|",
                        "eventKeyboard|2601|1|", "eventKeyboard|2601|0|", "eventKeyboard|2601|1|",
                        "eventKeyboard|2601|0|", "eventKeyboard|2601|1|", "eventKeyboard|2601|0|",
                        "eventKeyboard|2601|1|", "eventKeyboard|2601|0|", "eventKeyboard|2601|1|",
                        "eventKeyboard|2601|0|", "eventKeyboard|2601|1|", "eventKeyboard|2601|0|",
                        "eventKeyboard|2601|1|", "eventKeyboard|2601|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_094_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_094_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_094_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json", "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json",
                    "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json", "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json",
                    "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json", "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json",
                    "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json", "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json",
                    "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json", "keyboard_KEY_102ND_press.json", "keyboard_KEY_102ND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|86|1", "eventKeyboard|86|0", "eventKeyboard|86|1",
                             "eventKeyboard|86|0", "eventKeyboard|86|1", "eventKeyboard|86|0",
                             "eventKeyboard|86|1", "eventKeyboard|86|0", "eventKeyboard|86|1",
                             "eventKeyboard|86|0", "eventKeyboard|86|1", "eventKeyboard|86|0",
                             "eventKeyboard|86|1", "eventKeyboard|86|0", "eventKeyboard|86|1",
                             "eventKeyboard|86|0", "eventKeyboard|86|1", "eventKeyboard|86|0",
                             "eventKeyboard|86|1", "eventKeyboard|86|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2602|1|", "eventKeyboard|2602|0|", "eventKeyboard|2602|1|",
                        "eventKeyboard|2602|0|", "eventKeyboard|2602|1|", "eventKeyboard|2602|0|",
                        "eventKeyboard|2602|1|", "eventKeyboard|2602|0|", "eventKeyboard|2602|1|",
                        "eventKeyboard|2602|0|", "eventKeyboard|2602|1|", "eventKeyboard|2602|0|",
                        "eventKeyboard|2602|1|", "eventKeyboard|2602|0|", "eventKeyboard|2602|1|",
                        "eventKeyboard|2602|0|", "eventKeyboard|2602|1|", "eventKeyboard|2602|0|",
                        "eventKeyboard|2602|1|", "eventKeyboard|2602|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_095_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_095_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_095_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json", "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json",
                    "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json", "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json",
                    "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json", "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json",
                    "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json", "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json",
                    "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json", "keyboard_KEY_RO_press.json", "keyboard_KEY_RO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|89|1", "eventKeyboard|89|0", "eventKeyboard|89|1",
                             "eventKeyboard|89|0", "eventKeyboard|89|1", "eventKeyboard|89|0",
                             "eventKeyboard|89|1", "eventKeyboard|89|0", "eventKeyboard|89|1",
                             "eventKeyboard|89|0", "eventKeyboard|89|1", "eventKeyboard|89|0",
                             "eventKeyboard|89|1", "eventKeyboard|89|0", "eventKeyboard|89|1",
                             "eventKeyboard|89|0", "eventKeyboard|89|1", "eventKeyboard|89|0",
                             "eventKeyboard|89|1", "eventKeyboard|89|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2603|1|", "eventKeyboard|2603|0|", "eventKeyboard|2603|1|",
                        "eventKeyboard|2603|0|", "eventKeyboard|2603|1|", "eventKeyboard|2603|0|",
                        "eventKeyboard|2603|1|", "eventKeyboard|2603|0|", "eventKeyboard|2603|1|",
                        "eventKeyboard|2603|0|", "eventKeyboard|2603|1|", "eventKeyboard|2603|0|",
                        "eventKeyboard|2603|1|", "eventKeyboard|2603|0|", "eventKeyboard|2603|1|",
                        "eventKeyboard|2603|0|", "eventKeyboard|2603|1|", "eventKeyboard|2603|0|",
                        "eventKeyboard|2603|1|", "eventKeyboard|2603|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_096_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_096_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_096_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json", "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json",
                    "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json", "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json",
                    "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json", "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json",
                    "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json", "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json",
                    "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json", "keyboard_KEY_KATAKANA_press.json", "keyboard_KEY_KATAKANA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|90|1", "eventKeyboard|90|0", "eventKeyboard|90|1",
                             "eventKeyboard|90|0", "eventKeyboard|90|1", "eventKeyboard|90|0",
                             "eventKeyboard|90|1", "eventKeyboard|90|0", "eventKeyboard|90|1",
                             "eventKeyboard|90|0", "eventKeyboard|90|1", "eventKeyboard|90|0",
                             "eventKeyboard|90|1", "eventKeyboard|90|0", "eventKeyboard|90|1",
                             "eventKeyboard|90|0", "eventKeyboard|90|1", "eventKeyboard|90|0",
                             "eventKeyboard|90|1", "eventKeyboard|90|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2604|1|", "eventKeyboard|2604|0|", "eventKeyboard|2604|1|",
                        "eventKeyboard|2604|0|", "eventKeyboard|2604|1|", "eventKeyboard|2604|0|",
                        "eventKeyboard|2604|1|", "eventKeyboard|2604|0|", "eventKeyboard|2604|1|",
                        "eventKeyboard|2604|0|", "eventKeyboard|2604|1|", "eventKeyboard|2604|0|",
                        "eventKeyboard|2604|1|", "eventKeyboard|2604|0|", "eventKeyboard|2604|1|",
                        "eventKeyboard|2604|0|", "eventKeyboard|2604|1|", "eventKeyboard|2604|0|",
                        "eventKeyboard|2604|1|", "eventKeyboard|2604|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_097_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_097_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_097_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json", "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json",
                    "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json", "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json",
                    "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json", "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json",
                    "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json", "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json",
                    "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json", "keyboard_KEY_HIRAGANA_press.json", "keyboard_KEY_HIRAGANA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|91|1", "eventKeyboard|91|0", "eventKeyboard|91|1",
                             "eventKeyboard|91|0", "eventKeyboard|91|1", "eventKeyboard|91|0",
                             "eventKeyboard|91|1", "eventKeyboard|91|0", "eventKeyboard|91|1",
                             "eventKeyboard|91|0", "eventKeyboard|91|1", "eventKeyboard|91|0",
                             "eventKeyboard|91|1", "eventKeyboard|91|0", "eventKeyboard|91|1",
                             "eventKeyboard|91|0", "eventKeyboard|91|1", "eventKeyboard|91|0",
                             "eventKeyboard|91|1", "eventKeyboard|91|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2605|1|", "eventKeyboard|2605|0|", "eventKeyboard|2605|1|",
                        "eventKeyboard|2605|0|", "eventKeyboard|2605|1|", "eventKeyboard|2605|0|",
                        "eventKeyboard|2605|1|", "eventKeyboard|2605|0|", "eventKeyboard|2605|1|",
                        "eventKeyboard|2605|0|", "eventKeyboard|2605|1|", "eventKeyboard|2605|0|",
                        "eventKeyboard|2605|1|", "eventKeyboard|2605|0|", "eventKeyboard|2605|1|",
                        "eventKeyboard|2605|0|", "eventKeyboard|2605|1|", "eventKeyboard|2605|0|",
                        "eventKeyboard|2605|1|", "eventKeyboard|2605|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_098_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_098_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_098_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json", "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json",
                    "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json", "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json",
                    "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json", "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json",
                    "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json", "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json",
                    "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json", "keyboard_KEY_HENKAN_press.json", "keyboard_KEY_HENKAN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|92|1", "eventKeyboard|92|0", "eventKeyboard|92|1",
                             "eventKeyboard|92|0", "eventKeyboard|92|1", "eventKeyboard|92|0",
                             "eventKeyboard|92|1", "eventKeyboard|92|0", "eventKeyboard|92|1",
                             "eventKeyboard|92|0", "eventKeyboard|92|1", "eventKeyboard|92|0",
                             "eventKeyboard|92|1", "eventKeyboard|92|0", "eventKeyboard|92|1",
                             "eventKeyboard|92|0", "eventKeyboard|92|1", "eventKeyboard|92|0",
                             "eventKeyboard|92|1", "eventKeyboard|92|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2606|1|", "eventKeyboard|2606|0|", "eventKeyboard|2606|1|",
                        "eventKeyboard|2606|0|", "eventKeyboard|2606|1|", "eventKeyboard|2606|0|",
                        "eventKeyboard|2606|1|", "eventKeyboard|2606|0|", "eventKeyboard|2606|1|",
                        "eventKeyboard|2606|0|", "eventKeyboard|2606|1|", "eventKeyboard|2606|0|",
                        "eventKeyboard|2606|1|", "eventKeyboard|2606|0|", "eventKeyboard|2606|1|",
                        "eventKeyboard|2606|0|", "eventKeyboard|2606|1|", "eventKeyboard|2606|0|",
                        "eventKeyboard|2606|1|", "eventKeyboard|2606|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_099_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_099_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_099_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json", "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json",
                    "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json", "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json",
                    "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json", "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json",
                    "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json", "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json",
                    "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json", "keyboard_KEY_KATAKANAHIRAGANA_press.json", "keyboard_KEY_KATAKANAHIRAGANA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|93|1", "eventKeyboard|93|0", "eventKeyboard|93|1",
                             "eventKeyboard|93|0", "eventKeyboard|93|1", "eventKeyboard|93|0",
                             "eventKeyboard|93|1", "eventKeyboard|93|0", "eventKeyboard|93|1",
                             "eventKeyboard|93|0", "eventKeyboard|93|1", "eventKeyboard|93|0",
                             "eventKeyboard|93|1", "eventKeyboard|93|0", "eventKeyboard|93|1",
                             "eventKeyboard|93|0", "eventKeyboard|93|1", "eventKeyboard|93|0",
                             "eventKeyboard|93|1", "eventKeyboard|93|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2607|1|", "eventKeyboard|2607|0|", "eventKeyboard|2607|1|",
                        "eventKeyboard|2607|0|", "eventKeyboard|2607|1|", "eventKeyboard|2607|0|",
                        "eventKeyboard|2607|1|", "eventKeyboard|2607|0|", "eventKeyboard|2607|1|",
                        "eventKeyboard|2607|0|", "eventKeyboard|2607|1|", "eventKeyboard|2607|0|",
                        "eventKeyboard|2607|1|", "eventKeyboard|2607|0|", "eventKeyboard|2607|1|",
                        "eventKeyboard|2607|0|", "eventKeyboard|2607|1|", "eventKeyboard|2607|0|",
                        "eventKeyboard|2607|1|", "eventKeyboard|2607|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_100_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_100_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_100_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json", "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json",
                    "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json", "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json",
                    "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json", "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json",
                    "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json", "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json",
                    "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json", "keyboard_KEY_MUHENKAN_press.json", "keyboard_KEY_MUHENKAN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|94|1", "eventKeyboard|94|0", "eventKeyboard|94|1",
                             "eventKeyboard|94|0", "eventKeyboard|94|1", "eventKeyboard|94|0",
                             "eventKeyboard|94|1", "eventKeyboard|94|0", "eventKeyboard|94|1",
                             "eventKeyboard|94|0", "eventKeyboard|94|1", "eventKeyboard|94|0",
                             "eventKeyboard|94|1", "eventKeyboard|94|0", "eventKeyboard|94|1",
                             "eventKeyboard|94|0", "eventKeyboard|94|1", "eventKeyboard|94|0",
                             "eventKeyboard|94|1", "eventKeyboard|94|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2608|1|", "eventKeyboard|2608|0|", "eventKeyboard|2608|1|",
                        "eventKeyboard|2608|0|", "eventKeyboard|2608|1|", "eventKeyboard|2608|0|",
                        "eventKeyboard|2608|1|", "eventKeyboard|2608|0|", "eventKeyboard|2608|1|",
                        "eventKeyboard|2608|0|", "eventKeyboard|2608|1|", "eventKeyboard|2608|0|",
                        "eventKeyboard|2608|1|", "eventKeyboard|2608|0|", "eventKeyboard|2608|1|",
                        "eventKeyboard|2608|0|", "eventKeyboard|2608|1|", "eventKeyboard|2608|0|",
                        "eventKeyboard|2608|1|", "eventKeyboard|2608|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_101_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_101_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_101_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json", "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json",
                    "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json", "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json",
                    "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json", "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json",
                    "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json", "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json",
                    "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json", "keyboard_KEY_KPJPCOMMA_press.json", "keyboard_KEY_KPJPCOMMA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|95|1", "eventKeyboard|95|0", "eventKeyboard|95|1",
                             "eventKeyboard|95|0", "eventKeyboard|95|1", "eventKeyboard|95|0",
                             "eventKeyboard|95|1", "eventKeyboard|95|0", "eventKeyboard|95|1",
                             "eventKeyboard|95|0", "eventKeyboard|95|1", "eventKeyboard|95|0",
                             "eventKeyboard|95|1", "eventKeyboard|95|0", "eventKeyboard|95|1",
                             "eventKeyboard|95|0", "eventKeyboard|95|1", "eventKeyboard|95|0",
                             "eventKeyboard|95|1", "eventKeyboard|95|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_102_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_102_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_102_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json", "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json",
                    "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json", "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json",
                    "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json", "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json",
                    "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json", "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json",
                    "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json", "keyboard_KEY_LINEFEED_press.json", "keyboard_KEY_LINEFEED_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|101|1", "eventKeyboard|101|0", "eventKeyboard|101|1",
                             "eventKeyboard|101|0", "eventKeyboard|101|1", "eventKeyboard|101|0",
                             "eventKeyboard|101|1", "eventKeyboard|101|0", "eventKeyboard|101|1",
                             "eventKeyboard|101|0", "eventKeyboard|101|1", "eventKeyboard|101|0",
                             "eventKeyboard|101|1", "eventKeyboard|101|0", "eventKeyboard|101|1",
                             "eventKeyboard|101|0", "eventKeyboard|101|1", "eventKeyboard|101|0",
                             "eventKeyboard|101|1", "eventKeyboard|101|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2609|1|", "eventKeyboard|2609|0|", "eventKeyboard|2609|1|",
                        "eventKeyboard|2609|0|", "eventKeyboard|2609|1|", "eventKeyboard|2609|0|",
                        "eventKeyboard|2609|1|", "eventKeyboard|2609|0|", "eventKeyboard|2609|1|",
                        "eventKeyboard|2609|0|", "eventKeyboard|2609|1|", "eventKeyboard|2609|0|",
                        "eventKeyboard|2609|1|", "eventKeyboard|2609|0|", "eventKeyboard|2609|1|",
                        "eventKeyboard|2609|0|", "eventKeyboard|2609|1|", "eventKeyboard|2609|0|",
                        "eventKeyboard|2609|1|", "eventKeyboard|2609|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_103_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_103_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_103_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json", "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json",
                    "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json", "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json",
                    "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json", "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json",
                    "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json", "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json",
                    "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json", "keyboard_KEY_MACRO_press.json", "keyboard_KEY_MACRO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|112|1", "eventKeyboard|112|0", "eventKeyboard|112|1",
                             "eventKeyboard|112|0", "eventKeyboard|112|1", "eventKeyboard|112|0",
                             "eventKeyboard|112|1", "eventKeyboard|112|0", "eventKeyboard|112|1",
                             "eventKeyboard|112|0", "eventKeyboard|112|1", "eventKeyboard|112|0",
                             "eventKeyboard|112|1", "eventKeyboard|112|0", "eventKeyboard|112|1",
                             "eventKeyboard|112|0", "eventKeyboard|112|1", "eventKeyboard|112|0",
                             "eventKeyboard|112|1", "eventKeyboard|112|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2610|1|", "eventKeyboard|2610|0|", "eventKeyboard|2610|1|",
                        "eventKeyboard|2610|0|", "eventKeyboard|2610|1|", "eventKeyboard|2610|0|",
                        "eventKeyboard|2610|1|", "eventKeyboard|2610|0|", "eventKeyboard|2610|1|",
                        "eventKeyboard|2610|0|", "eventKeyboard|2610|1|", "eventKeyboard|2610|0|",
                        "eventKeyboard|2610|1|", "eventKeyboard|2610|0|", "eventKeyboard|2610|1|",
                        "eventKeyboard|2610|0|", "eventKeyboard|2610|1|", "eventKeyboard|2610|0|",
                        "eventKeyboard|2610|1|", "eventKeyboard|2610|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_104_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_104_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_104_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json", "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json",
                    "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json", "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json",
                    "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json", "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json",
                    "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json", "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json",
                    "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json", "keyboard_KEY_MUTE_press.json", "keyboard_KEY_MUTE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_105_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_105_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_105_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json", "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json",
                    "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json", "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json",
                    "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json", "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json",
                    "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json", "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json",
                    "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json", "keyboard_KEY_VOLUMEDOWN_press.json", "keyboard_KEY_VOLUMEDOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|114|1", "eventKeyboard|114|0", "eventKeyboard|114|1",
                             "eventKeyboard|114|0", "eventKeyboard|114|1", "eventKeyboard|114|0",
                             "eventKeyboard|114|1", "eventKeyboard|114|0", "eventKeyboard|114|1",
                             "eventKeyboard|114|0", "eventKeyboard|114|1", "eventKeyboard|114|0",
                             "eventKeyboard|114|1", "eventKeyboard|114|0", "eventKeyboard|114|1",
                             "eventKeyboard|114|0", "eventKeyboard|114|1", "eventKeyboard|114|0",
                             "eventKeyboard|114|1", "eventKeyboard|114|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|17|1|", "eventKeyboard|17|0|", "eventKeyboard|17|1|",
                        "eventKeyboard|17|0|", "eventKeyboard|17|1|", "eventKeyboard|17|0|",
                        "eventKeyboard|17|1|", "eventKeyboard|17|0|", "eventKeyboard|17|1|",
                        "eventKeyboard|17|0|", "eventKeyboard|17|1|", "eventKeyboard|17|0|",
                        "eventKeyboard|17|1|", "eventKeyboard|17|0|", "eventKeyboard|17|1|",
                        "eventKeyboard|17|0|", "eventKeyboard|17|1|", "eventKeyboard|17|0|",
                        "eventKeyboard|17|1|", "eventKeyboard|17|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_106_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_106_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_106_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json", "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json",
                    "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json", "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json",
                    "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json", "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json",
                    "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json", "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json",
                    "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json", "keyboard_KEY_VOLUMEUP_press.json", "keyboard_KEY_VOLUMEUP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|115|1", "eventKeyboard|115|0", "eventKeyboard|115|1",
                             "eventKeyboard|115|0", "eventKeyboard|115|1", "eventKeyboard|115|0",
                             "eventKeyboard|115|1", "eventKeyboard|115|0", "eventKeyboard|115|1",
                             "eventKeyboard|115|0", "eventKeyboard|115|1", "eventKeyboard|115|0",
                             "eventKeyboard|115|1", "eventKeyboard|115|0", "eventKeyboard|115|1",
                             "eventKeyboard|115|0", "eventKeyboard|115|1", "eventKeyboard|115|0",
                             "eventKeyboard|115|1", "eventKeyboard|115|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|16|1|", "eventKeyboard|16|0|", "eventKeyboard|16|1|",
                        "eventKeyboard|16|0|", "eventKeyboard|16|1|", "eventKeyboard|16|0|",
                        "eventKeyboard|16|1|", "eventKeyboard|16|0|", "eventKeyboard|16|1|",
                        "eventKeyboard|16|0|", "eventKeyboard|16|1|", "eventKeyboard|16|0|",
                        "eventKeyboard|16|1|", "eventKeyboard|16|0|", "eventKeyboard|16|1|",
                        "eventKeyboard|16|0|", "eventKeyboard|16|1|", "eventKeyboard|16|0|",
                        "eventKeyboard|16|1|", "eventKeyboard|16|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_108_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_108_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_108_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json", "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json",
                    "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json", "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json",
                    "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json", "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json",
                    "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json", "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json",
                    "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json", "keyboard_KEY_KPEQUAL_press.json", "keyboard_KEY_KPEQUAL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|117|1", "eventKeyboard|117|0", "eventKeyboard|117|1",
                             "eventKeyboard|117|0", "eventKeyboard|117|1", "eventKeyboard|117|0",
                             "eventKeyboard|117|1", "eventKeyboard|117|0", "eventKeyboard|117|1",
                             "eventKeyboard|117|0", "eventKeyboard|117|1", "eventKeyboard|117|0",
                             "eventKeyboard|117|1", "eventKeyboard|117|0", "eventKeyboard|117|1",
                             "eventKeyboard|117|0", "eventKeyboard|117|1", "eventKeyboard|117|0",
                             "eventKeyboard|117|1", "eventKeyboard|117|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2120|1|", "eventKeyboard|2120|0|", "eventKeyboard|2120|1|",
                        "eventKeyboard|2120|0|", "eventKeyboard|2120|1|", "eventKeyboard|2120|0|",
                        "eventKeyboard|2120|1|", "eventKeyboard|2120|0|", "eventKeyboard|2120|1|",
                        "eventKeyboard|2120|0|", "eventKeyboard|2120|1|", "eventKeyboard|2120|0|",
                        "eventKeyboard|2120|1|", "eventKeyboard|2120|0|", "eventKeyboard|2120|1|",
                        "eventKeyboard|2120|0|", "eventKeyboard|2120|1|", "eventKeyboard|2120|0|",
                        "eventKeyboard|2120|1|", "eventKeyboard|2120|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_109_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_109_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_109_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json", "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json",
                    "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json", "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json",
                    "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json", "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json",
                    "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json", "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json",
                    "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json", "keyboard_KEY_KPPLUSMINUS_press.json", "keyboard_KEY_KPPLUSMINUS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|118|1", "eventKeyboard|118|0", "eventKeyboard|118|1",
                             "eventKeyboard|118|0", "eventKeyboard|118|1", "eventKeyboard|118|0",
                             "eventKeyboard|118|1", "eventKeyboard|118|0", "eventKeyboard|118|1",
                             "eventKeyboard|118|0", "eventKeyboard|118|1", "eventKeyboard|118|0",
                             "eventKeyboard|118|1", "eventKeyboard|118|0", "eventKeyboard|118|1",
                             "eventKeyboard|118|0", "eventKeyboard|118|1", "eventKeyboard|118|0",
                             "eventKeyboard|118|1", "eventKeyboard|118|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2611|1|", "eventKeyboard|2611|0|", "eventKeyboard|2611|1|",
                        "eventKeyboard|2611|0|", "eventKeyboard|2611|1|", "eventKeyboard|2611|0|",
                        "eventKeyboard|2611|1|", "eventKeyboard|2611|0|", "eventKeyboard|2611|1|",
                        "eventKeyboard|2611|0|", "eventKeyboard|2611|1|", "eventKeyboard|2611|0|",
                        "eventKeyboard|2611|1|", "eventKeyboard|2611|0|", "eventKeyboard|2611|1|",
                        "eventKeyboard|2611|0|", "eventKeyboard|2611|1|", "eventKeyboard|2611|0|",
                        "eventKeyboard|2611|1|", "eventKeyboard|2611|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_110_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_110_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_110_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json", "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json",
                    "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json", "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json",
                    "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json", "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json",
                    "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json", "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json",
                    "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json", "keyboard_KEY_SCALE_press.json", "keyboard_KEY_SCALE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|120|1", "eventKeyboard|120|0", "eventKeyboard|120|1",
                             "eventKeyboard|120|0", "eventKeyboard|120|1", "eventKeyboard|120|0",
                             "eventKeyboard|120|1", "eventKeyboard|120|0", "eventKeyboard|120|1",
                             "eventKeyboard|120|0", "eventKeyboard|120|1", "eventKeyboard|120|0",
                             "eventKeyboard|120|1", "eventKeyboard|120|0", "eventKeyboard|120|1",
                             "eventKeyboard|120|0", "eventKeyboard|120|1", "eventKeyboard|120|0",
                             "eventKeyboard|120|1", "eventKeyboard|120|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2612|1|", "eventKeyboard|2612|0|", "eventKeyboard|2612|1|",
                        "eventKeyboard|2612|0|", "eventKeyboard|2612|1|", "eventKeyboard|2612|0|",
                        "eventKeyboard|2612|1|", "eventKeyboard|2612|0|", "eventKeyboard|2612|1|",
                        "eventKeyboard|2612|0|", "eventKeyboard|2612|1|", "eventKeyboard|2612|0|",
                        "eventKeyboard|2612|1|", "eventKeyboard|2612|0|", "eventKeyboard|2612|1|",
                        "eventKeyboard|2612|0|", "eventKeyboard|2612|1|", "eventKeyboard|2612|0|",
                        "eventKeyboard|2612|1|", "eventKeyboard|2612|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_111_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_111_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_111_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json", "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json",
                    "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json", "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json",
                    "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json", "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json",
                    "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json", "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json",
                    "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json", "keyboard_KEY_KPCOMMA_press.json", "keyboard_KEY_KPCOMMA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|121|1", "eventKeyboard|121|0", "eventKeyboard|121|1",
                             "eventKeyboard|121|0", "eventKeyboard|121|1", "eventKeyboard|121|0",
                             "eventKeyboard|121|1", "eventKeyboard|121|0", "eventKeyboard|121|1",
                             "eventKeyboard|121|0", "eventKeyboard|121|1", "eventKeyboard|121|0",
                             "eventKeyboard|121|1", "eventKeyboard|121|0", "eventKeyboard|121|1",
                             "eventKeyboard|121|0", "eventKeyboard|121|1", "eventKeyboard|121|0",
                             "eventKeyboard|121|1", "eventKeyboard|121|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|", "eventKeyboard|2118|1|",
                        "eventKeyboard|2118|0|", "eventKeyboard|2118|1|", "eventKeyboard|2118|0|",
                        "eventKeyboard|2118|1|", "eventKeyboard|2118|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_112_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_112_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_112_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json", "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json",
                    "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json", "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json",
                    "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json", "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json",
                    "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json", "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json",
                    "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json", "keyboard_KEY_HANGEUL_press.json", "keyboard_KEY_HANGEUL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|122|1", "eventKeyboard|122|0", "eventKeyboard|122|1",
                             "eventKeyboard|122|0", "eventKeyboard|122|1", "eventKeyboard|122|0",
                             "eventKeyboard|122|1", "eventKeyboard|122|0", "eventKeyboard|122|1",
                             "eventKeyboard|122|0", "eventKeyboard|122|1", "eventKeyboard|122|0",
                             "eventKeyboard|122|1", "eventKeyboard|122|0", "eventKeyboard|122|1",
                             "eventKeyboard|122|0", "eventKeyboard|122|1", "eventKeyboard|122|0",
                             "eventKeyboard|122|1", "eventKeyboard|122|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2613|1|", "eventKeyboard|2613|0|", "eventKeyboard|2613|1|",
                        "eventKeyboard|2613|0|", "eventKeyboard|2613|1|", "eventKeyboard|2613|0|",
                        "eventKeyboard|2613|1|", "eventKeyboard|2613|0|", "eventKeyboard|2613|1|",
                        "eventKeyboard|2613|0|", "eventKeyboard|2613|1|", "eventKeyboard|2613|0|",
                        "eventKeyboard|2613|1|", "eventKeyboard|2613|0|", "eventKeyboard|2613|1|",
                        "eventKeyboard|2613|0|", "eventKeyboard|2613|1|", "eventKeyboard|2613|0|",
                        "eventKeyboard|2613|1|", "eventKeyboard|2613|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_113_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_113_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_113_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json", "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json",
                    "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json", "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json",
                    "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json", "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json",
                    "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json", "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json",
                    "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json", "keyboard_KEY_HANJA_press.json", "keyboard_KEY_HANJA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|123|1", "eventKeyboard|123|0", "eventKeyboard|123|1",
                             "eventKeyboard|123|0", "eventKeyboard|123|1", "eventKeyboard|123|0",
                             "eventKeyboard|123|1", "eventKeyboard|123|0", "eventKeyboard|123|1",
                             "eventKeyboard|123|0", "eventKeyboard|123|1", "eventKeyboard|123|0",
                             "eventKeyboard|123|1", "eventKeyboard|123|0", "eventKeyboard|123|1",
                             "eventKeyboard|123|0", "eventKeyboard|123|1", "eventKeyboard|123|0",
                             "eventKeyboard|123|1", "eventKeyboard|123|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2614|1|", "eventKeyboard|2614|0|", "eventKeyboard|2614|1|",
                        "eventKeyboard|2614|0|", "eventKeyboard|2614|1|", "eventKeyboard|2614|0|",
                        "eventKeyboard|2614|1|", "eventKeyboard|2614|0|", "eventKeyboard|2614|1|",
                        "eventKeyboard|2614|0|", "eventKeyboard|2614|1|", "eventKeyboard|2614|0|",
                        "eventKeyboard|2614|1|", "eventKeyboard|2614|0|", "eventKeyboard|2614|1|",
                        "eventKeyboard|2614|0|", "eventKeyboard|2614|1|", "eventKeyboard|2614|0|",
                        "eventKeyboard|2614|1|", "eventKeyboard|2614|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_114_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_114_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_114_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json", "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json",
                    "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json", "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json",
                    "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json", "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json",
                    "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json", "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json",
                    "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json", "keyboard_KEY_YEN_press.json", "keyboard_KEY_YEN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|124|1", "eventKeyboard|124|0", "eventKeyboard|124|1",
                             "eventKeyboard|124|0", "eventKeyboard|124|1", "eventKeyboard|124|0",
                             "eventKeyboard|124|1", "eventKeyboard|124|0", "eventKeyboard|124|1",
                             "eventKeyboard|124|0", "eventKeyboard|124|1", "eventKeyboard|124|0",
                             "eventKeyboard|124|1", "eventKeyboard|124|0", "eventKeyboard|124|1",
                             "eventKeyboard|124|0", "eventKeyboard|124|1", "eventKeyboard|124|0",
                             "eventKeyboard|124|1", "eventKeyboard|124|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2615|1|", "eventKeyboard|2615|0|", "eventKeyboard|2615|1|",
                        "eventKeyboard|2615|0|", "eventKeyboard|2615|1|", "eventKeyboard|2615|0|",
                        "eventKeyboard|2615|1|", "eventKeyboard|2615|0|", "eventKeyboard|2615|1|",
                        "eventKeyboard|2615|0|", "eventKeyboard|2615|1|", "eventKeyboard|2615|0|",
                        "eventKeyboard|2615|1|", "eventKeyboard|2615|0|", "eventKeyboard|2615|1|",
                        "eventKeyboard|2615|0|", "eventKeyboard|2615|1|", "eventKeyboard|2615|0|",
                        "eventKeyboard|2615|1|", "eventKeyboard|2615|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_115_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_115_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_115_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json", "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json",
                    "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json", "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json",
                    "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json", "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json",
                    "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json", "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json",
                    "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json", "keyboard_KEY_RIGHTMETA_press.json", "keyboard_KEY_RIGHTMETA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|126|1", "eventKeyboard|126|0", "eventKeyboard|126|1",
                             "eventKeyboard|126|0", "eventKeyboard|126|1", "eventKeyboard|126|0",
                             "eventKeyboard|126|1", "eventKeyboard|126|0", "eventKeyboard|126|1",
                             "eventKeyboard|126|0", "eventKeyboard|126|1", "eventKeyboard|126|0",
                             "eventKeyboard|126|1", "eventKeyboard|126|0", "eventKeyboard|126|1",
                             "eventKeyboard|126|0", "eventKeyboard|126|1", "eventKeyboard|126|0",
                             "eventKeyboard|126|1", "eventKeyboard|126|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2077|1|", "eventKeyboard|2077|0|", "eventKeyboard|2077|1|",
                        "eventKeyboard|2077|0|", "eventKeyboard|2077|1|", "eventKeyboard|2077|0|",
                        "eventKeyboard|2077|1|", "eventKeyboard|2077|0|", "eventKeyboard|2077|1|",
                        "eventKeyboard|2077|0|", "eventKeyboard|2077|1|", "eventKeyboard|2077|0|",
                        "eventKeyboard|2077|1|", "eventKeyboard|2077|0|", "eventKeyboard|2077|1|",
                        "eventKeyboard|2077|0|", "eventKeyboard|2077|1|", "eventKeyboard|2077|0|",
                        "eventKeyboard|2077|1|", "eventKeyboard|2077|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_116_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_116_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_116_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json", "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json",
                    "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json", "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json",
                    "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json", "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json",
                    "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json", "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json",
                    "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json", "keyboard_KEY_STOP_press.json", "keyboard_KEY_STOP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|128|1", "eventKeyboard|128|0", "eventKeyboard|128|1",
                             "eventKeyboard|128|0", "eventKeyboard|128|1", "eventKeyboard|128|0",
                             "eventKeyboard|128|1", "eventKeyboard|128|0", "eventKeyboard|128|1",
                             "eventKeyboard|128|0", "eventKeyboard|128|1", "eventKeyboard|128|0",
                             "eventKeyboard|128|1", "eventKeyboard|128|0", "eventKeyboard|128|1",
                             "eventKeyboard|128|0", "eventKeyboard|128|1", "eventKeyboard|128|0",
                             "eventKeyboard|128|1", "eventKeyboard|128|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2616|1|", "eventKeyboard|2616|0|", "eventKeyboard|2616|1|",
                        "eventKeyboard|2616|0|", "eventKeyboard|2616|1|", "eventKeyboard|2616|0|",
                        "eventKeyboard|2616|1|", "eventKeyboard|2616|0|", "eventKeyboard|2616|1|",
                        "eventKeyboard|2616|0|", "eventKeyboard|2616|1|", "eventKeyboard|2616|0|",
                        "eventKeyboard|2616|1|", "eventKeyboard|2616|0|", "eventKeyboard|2616|1|",
                        "eventKeyboard|2616|0|", "eventKeyboard|2616|1|", "eventKeyboard|2616|0|",
                        "eventKeyboard|2616|1|", "eventKeyboard|2616|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_117_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_117_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_117_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json", "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json",
                    "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json", "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json",
                    "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json", "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json",
                    "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json", "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json",
                    "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json", "keyboard_KEY_AGAIN_press.json", "keyboard_KEY_AGAIN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|129|1", "eventKeyboard|129|0", "eventKeyboard|129|1",
                             "eventKeyboard|129|0", "eventKeyboard|129|1", "eventKeyboard|129|0",
                             "eventKeyboard|129|1", "eventKeyboard|129|0", "eventKeyboard|129|1",
                             "eventKeyboard|129|0", "eventKeyboard|129|1", "eventKeyboard|129|0",
                             "eventKeyboard|129|1", "eventKeyboard|129|0", "eventKeyboard|129|1",
                             "eventKeyboard|129|0", "eventKeyboard|129|1", "eventKeyboard|129|0",
                             "eventKeyboard|129|1", "eventKeyboard|129|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2617|1|", "eventKeyboard|2617|0|", "eventKeyboard|2617|1|",
                        "eventKeyboard|2617|0|", "eventKeyboard|2617|1|", "eventKeyboard|2617|0|",
                        "eventKeyboard|2617|1|", "eventKeyboard|2617|0|", "eventKeyboard|2617|1|",
                        "eventKeyboard|2617|0|", "eventKeyboard|2617|1|", "eventKeyboard|2617|0|",
                        "eventKeyboard|2617|1|", "eventKeyboard|2617|0|", "eventKeyboard|2617|1|",
                        "eventKeyboard|2617|0|", "eventKeyboard|2617|1|", "eventKeyboard|2617|0|",
                        "eventKeyboard|2617|1|", "eventKeyboard|2617|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_118_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_118_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_118_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json", "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json",
                    "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json", "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json",
                    "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json", "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json",
                    "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json", "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json",
                    "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json", "keyboard_KEY_PROPS_press.json", "keyboard_KEY_PROPS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|130|1", "eventKeyboard|130|0", "eventKeyboard|130|1",
                             "eventKeyboard|130|0", "eventKeyboard|130|1", "eventKeyboard|130|0",
                             "eventKeyboard|130|1", "eventKeyboard|130|0", "eventKeyboard|130|1",
                             "eventKeyboard|130|0", "eventKeyboard|130|1", "eventKeyboard|130|0",
                             "eventKeyboard|130|1", "eventKeyboard|130|0", "eventKeyboard|130|1",
                             "eventKeyboard|130|0", "eventKeyboard|130|1", "eventKeyboard|130|0",
                             "eventKeyboard|130|1", "eventKeyboard|130|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2618|1|", "eventKeyboard|2618|0|", "eventKeyboard|2618|1|",
                        "eventKeyboard|2618|0|", "eventKeyboard|2618|1|", "eventKeyboard|2618|0|",
                        "eventKeyboard|2618|1|", "eventKeyboard|2618|0|", "eventKeyboard|2618|1|",
                        "eventKeyboard|2618|0|", "eventKeyboard|2618|1|", "eventKeyboard|2618|0|",
                        "eventKeyboard|2618|1|", "eventKeyboard|2618|0|", "eventKeyboard|2618|1|",
                        "eventKeyboard|2618|0|", "eventKeyboard|2618|1|", "eventKeyboard|2618|0|",
                        "eventKeyboard|2618|1|", "eventKeyboard|2618|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_119_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_119_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_119_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json", "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json",
                    "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json", "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json",
                    "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json", "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json",
                    "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json", "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json",
                    "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json", "keyboard_KEY_UNDO_press.json", "keyboard_KEY_UNDO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|131|1", "eventKeyboard|131|0", "eventKeyboard|131|1",
                             "eventKeyboard|131|0", "eventKeyboard|131|1", "eventKeyboard|131|0",
                             "eventKeyboard|131|1", "eventKeyboard|131|0", "eventKeyboard|131|1",
                             "eventKeyboard|131|0", "eventKeyboard|131|1", "eventKeyboard|131|0",
                             "eventKeyboard|131|1", "eventKeyboard|131|0", "eventKeyboard|131|1",
                             "eventKeyboard|131|0", "eventKeyboard|131|1", "eventKeyboard|131|0",
                             "eventKeyboard|131|1", "eventKeyboard|131|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2619|1|", "eventKeyboard|2619|0|", "eventKeyboard|2619|1|",
                        "eventKeyboard|2619|0|", "eventKeyboard|2619|1|", "eventKeyboard|2619|0|",
                        "eventKeyboard|2619|1|", "eventKeyboard|2619|0|", "eventKeyboard|2619|1|",
                        "eventKeyboard|2619|0|", "eventKeyboard|2619|1|", "eventKeyboard|2619|0|",
                        "eventKeyboard|2619|1|", "eventKeyboard|2619|0|", "eventKeyboard|2619|1|",
                        "eventKeyboard|2619|0|", "eventKeyboard|2619|1|", "eventKeyboard|2619|0|",
                        "eventKeyboard|2619|1|", "eventKeyboard|2619|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_120_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_120_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_120_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json", "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json",
                    "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json", "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json",
                    "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json", "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json",
                    "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json", "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json",
                    "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json", "keyboard_KEY_FRONT_press.json", "keyboard_KEY_FRONT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|132|1", "eventKeyboard|132|0", "eventKeyboard|132|1",
                             "eventKeyboard|132|0", "eventKeyboard|132|1", "eventKeyboard|132|0",
                             "eventKeyboard|132|1", "eventKeyboard|132|0", "eventKeyboard|132|1",
                             "eventKeyboard|132|0", "eventKeyboard|132|1", "eventKeyboard|132|0",
                             "eventKeyboard|132|1", "eventKeyboard|132|0", "eventKeyboard|132|1",
                             "eventKeyboard|132|0", "eventKeyboard|132|1", "eventKeyboard|132|0",
                             "eventKeyboard|132|1", "eventKeyboard|132|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2800|1|", "eventKeyboard|2800|0|", "eventKeyboard|2800|1|",
                        "eventKeyboard|2800|0|", "eventKeyboard|2800|1|", "eventKeyboard|2800|0|",
                        "eventKeyboard|2800|1|", "eventKeyboard|2800|0|", "eventKeyboard|2800|1|",
                        "eventKeyboard|2800|0|", "eventKeyboard|2800|1|", "eventKeyboard|2800|0|",
                        "eventKeyboard|2800|1|", "eventKeyboard|2800|0|", "eventKeyboard|2800|1|",
                        "eventKeyboard|2800|0|", "eventKeyboard|2800|1|", "eventKeyboard|2800|0|",
                        "eventKeyboard|2800|1|", "eventKeyboard|2800|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_121_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_121_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_121_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json", "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json",
                    "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json", "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json",
                    "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json", "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json",
                    "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json", "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json",
                    "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json", "keyboard_KEY_COPY_press.json", "keyboard_KEY_COPY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|133|1", "eventKeyboard|133|0", "eventKeyboard|133|1",
                             "eventKeyboard|133|0", "eventKeyboard|133|1", "eventKeyboard|133|0",
                             "eventKeyboard|133|1", "eventKeyboard|133|0", "eventKeyboard|133|1",
                             "eventKeyboard|133|0", "eventKeyboard|133|1", "eventKeyboard|133|0",
                             "eventKeyboard|133|1", "eventKeyboard|133|0", "eventKeyboard|133|1",
                             "eventKeyboard|133|0", "eventKeyboard|133|1", "eventKeyboard|133|0",
                             "eventKeyboard|133|1", "eventKeyboard|133|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2620|1|", "eventKeyboard|2620|0|", "eventKeyboard|2620|1|",
                        "eventKeyboard|2620|0|", "eventKeyboard|2620|1|", "eventKeyboard|2620|0|",
                        "eventKeyboard|2620|1|", "eventKeyboard|2620|0|", "eventKeyboard|2620|1|",
                        "eventKeyboard|2620|0|", "eventKeyboard|2620|1|", "eventKeyboard|2620|0|",
                        "eventKeyboard|2620|1|", "eventKeyboard|2620|0|", "eventKeyboard|2620|1|",
                        "eventKeyboard|2620|0|", "eventKeyboard|2620|1|", "eventKeyboard|2620|0|",
                        "eventKeyboard|2620|1|", "eventKeyboard|2620|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_122_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_122_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_122_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json", "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json",
                    "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json", "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json",
                    "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json", "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json",
                    "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json", "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json",
                    "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json", "keyboard_KEY_OPEN_press.json", "keyboard_KEY_OPEN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|134|1", "eventKeyboard|134|0", "eventKeyboard|134|1",
                             "eventKeyboard|134|0", "eventKeyboard|134|1", "eventKeyboard|134|0",
                             "eventKeyboard|134|1", "eventKeyboard|134|0", "eventKeyboard|134|1",
                             "eventKeyboard|134|0", "eventKeyboard|134|1", "eventKeyboard|134|0",
                             "eventKeyboard|134|1", "eventKeyboard|134|0", "eventKeyboard|134|1",
                             "eventKeyboard|134|0", "eventKeyboard|134|1", "eventKeyboard|134|0",
                             "eventKeyboard|134|1", "eventKeyboard|134|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2621|1|", "eventKeyboard|2621|0|", "eventKeyboard|2621|1|",
                        "eventKeyboard|2621|0|", "eventKeyboard|2621|1|", "eventKeyboard|2621|0|",
                        "eventKeyboard|2621|1|", "eventKeyboard|2621|0|", "eventKeyboard|2621|1|",
                        "eventKeyboard|2621|0|", "eventKeyboard|2621|1|", "eventKeyboard|2621|0|",
                        "eventKeyboard|2621|1|", "eventKeyboard|2621|0|", "eventKeyboard|2621|1|",
                        "eventKeyboard|2621|0|", "eventKeyboard|2621|1|", "eventKeyboard|2621|0|",
                        "eventKeyboard|2621|1|", "eventKeyboard|2621|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_123_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_123_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_123_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json", "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json",
                    "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json", "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json",
                    "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json", "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json",
                    "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json", "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json",
                    "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json", "keyboard_KEY_PASTE_press.json", "keyboard_KEY_PASTE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|135|1", "eventKeyboard|135|0", "eventKeyboard|135|1",
                             "eventKeyboard|135|0", "eventKeyboard|135|1", "eventKeyboard|135|0",
                             "eventKeyboard|135|1", "eventKeyboard|135|0", "eventKeyboard|135|1",
                             "eventKeyboard|135|0", "eventKeyboard|135|1", "eventKeyboard|135|0",
                             "eventKeyboard|135|1", "eventKeyboard|135|0", "eventKeyboard|135|1",
                             "eventKeyboard|135|0", "eventKeyboard|135|1", "eventKeyboard|135|0",
                             "eventKeyboard|135|1", "eventKeyboard|135|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2622|1|", "eventKeyboard|2622|0|", "eventKeyboard|2622|1|",
                        "eventKeyboard|2622|0|", "eventKeyboard|2622|1|", "eventKeyboard|2622|0|",
                        "eventKeyboard|2622|1|", "eventKeyboard|2622|0|", "eventKeyboard|2622|1|",
                        "eventKeyboard|2622|0|", "eventKeyboard|2622|1|", "eventKeyboard|2622|0|",
                        "eventKeyboard|2622|1|", "eventKeyboard|2622|0|", "eventKeyboard|2622|1|",
                        "eventKeyboard|2622|0|", "eventKeyboard|2622|1|", "eventKeyboard|2622|0|",
                        "eventKeyboard|2622|1|", "eventKeyboard|2622|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_124_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_124_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_124_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json", "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json",
                    "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json", "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json",
                    "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json", "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json",
                    "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json", "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json",
                    "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json", "keyboard_KEY_FIND_press.json", "keyboard_KEY_FIND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|136|1", "eventKeyboard|136|0", "eventKeyboard|136|1",
                             "eventKeyboard|136|0", "eventKeyboard|136|1", "eventKeyboard|136|0",
                             "eventKeyboard|136|1", "eventKeyboard|136|0", "eventKeyboard|136|1",
                             "eventKeyboard|136|0", "eventKeyboard|136|1", "eventKeyboard|136|0",
                             "eventKeyboard|136|1", "eventKeyboard|136|0", "eventKeyboard|136|1",
                             "eventKeyboard|136|0", "eventKeyboard|136|1", "eventKeyboard|136|0",
                             "eventKeyboard|136|1", "eventKeyboard|136|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2623|1|", "eventKeyboard|2623|0|", "eventKeyboard|2623|1|",
                        "eventKeyboard|2623|0|", "eventKeyboard|2623|1|", "eventKeyboard|2623|0|",
                        "eventKeyboard|2623|1|", "eventKeyboard|2623|0|", "eventKeyboard|2623|1|",
                        "eventKeyboard|2623|0|", "eventKeyboard|2623|1|", "eventKeyboard|2623|0|",
                        "eventKeyboard|2623|1|", "eventKeyboard|2623|0|", "eventKeyboard|2623|1|",
                        "eventKeyboard|2623|0|", "eventKeyboard|2623|1|", "eventKeyboard|2623|0|",
                        "eventKeyboard|2623|1|", "eventKeyboard|2623|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_125_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_125_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_125_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json", "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json",
                    "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json", "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json",
                    "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json", "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json",
                    "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json", "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json",
                    "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json", "keyboard_KEY_CUT_press.json", "keyboard_KEY_CUT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|137|1", "eventKeyboard|137|0", "eventKeyboard|137|1",
                             "eventKeyboard|137|0", "eventKeyboard|137|1", "eventKeyboard|137|0",
                             "eventKeyboard|137|1", "eventKeyboard|137|0", "eventKeyboard|137|1",
                             "eventKeyboard|137|0", "eventKeyboard|137|1", "eventKeyboard|137|0",
                             "eventKeyboard|137|1", "eventKeyboard|137|0", "eventKeyboard|137|1",
                             "eventKeyboard|137|0", "eventKeyboard|137|1", "eventKeyboard|137|0",
                             "eventKeyboard|137|1", "eventKeyboard|137|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2624|1|", "eventKeyboard|2624|0|", "eventKeyboard|2624|1|",
                        "eventKeyboard|2624|0|", "eventKeyboard|2624|1|", "eventKeyboard|2624|0|",
                        "eventKeyboard|2624|1|", "eventKeyboard|2624|0|", "eventKeyboard|2624|1|",
                        "eventKeyboard|2624|0|", "eventKeyboard|2624|1|", "eventKeyboard|2624|0|",
                        "eventKeyboard|2624|1|", "eventKeyboard|2624|0|", "eventKeyboard|2624|1|",
                        "eventKeyboard|2624|0|", "eventKeyboard|2624|1|", "eventKeyboard|2624|0|",
                        "eventKeyboard|2624|1|", "eventKeyboard|2624|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_126_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_126_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_126_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json", "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json",
                    "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json", "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json",
                    "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json", "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json",
                    "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json", "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json",
                    "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json", "keyboard_KEY_HELP_press.json", "keyboard_KEY_HELP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|138|1", "eventKeyboard|138|0", "eventKeyboard|138|1",
                             "eventKeyboard|138|0", "eventKeyboard|138|1", "eventKeyboard|138|0",
                             "eventKeyboard|138|1", "eventKeyboard|138|0", "eventKeyboard|138|1",
                             "eventKeyboard|138|0", "eventKeyboard|138|1", "eventKeyboard|138|0",
                             "eventKeyboard|138|1", "eventKeyboard|138|0", "eventKeyboard|138|1",
                             "eventKeyboard|138|0", "eventKeyboard|138|1", "eventKeyboard|138|0",
                             "eventKeyboard|138|1", "eventKeyboard|138|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2625|1|", "eventKeyboard|2625|0|", "eventKeyboard|2625|1|",
                        "eventKeyboard|2625|0|", "eventKeyboard|2625|1|", "eventKeyboard|2625|0|",
                        "eventKeyboard|2625|1|", "eventKeyboard|2625|0|", "eventKeyboard|2625|1|",
                        "eventKeyboard|2625|0|", "eventKeyboard|2625|1|", "eventKeyboard|2625|0|",
                        "eventKeyboard|2625|1|", "eventKeyboard|2625|0|", "eventKeyboard|2625|1|",
                        "eventKeyboard|2625|0|", "eventKeyboard|2625|1|", "eventKeyboard|2625|0|",
                        "eventKeyboard|2625|1|", "eventKeyboard|2625|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_128_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_128_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_128_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json", "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json",
                    "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json", "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json",
                    "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json", "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json",
                    "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json", "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json",
                    "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json", "keyboard_KEY_CALC_press.json", "keyboard_KEY_CALC_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|140|1", "eventKeyboard|140|0", "eventKeyboard|140|1",
                             "eventKeyboard|140|0", "eventKeyboard|140|1", "eventKeyboard|140|0",
                             "eventKeyboard|140|1", "eventKeyboard|140|0", "eventKeyboard|140|1",
                             "eventKeyboard|140|0", "eventKeyboard|140|1", "eventKeyboard|140|0",
                             "eventKeyboard|140|1", "eventKeyboard|140|0", "eventKeyboard|140|1",
                             "eventKeyboard|140|0", "eventKeyboard|140|1", "eventKeyboard|140|0",
                             "eventKeyboard|140|1", "eventKeyboard|140|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2626|1|", "eventKeyboard|2626|0|", "eventKeyboard|2626|1|",
                        "eventKeyboard|2626|0|", "eventKeyboard|2626|1|", "eventKeyboard|2626|0|",
                        "eventKeyboard|2626|1|", "eventKeyboard|2626|0|", "eventKeyboard|2626|1|",
                        "eventKeyboard|2626|0|", "eventKeyboard|2626|1|", "eventKeyboard|2626|0|",
                        "eventKeyboard|2626|1|", "eventKeyboard|2626|0|", "eventKeyboard|2626|1|",
                        "eventKeyboard|2626|0|", "eventKeyboard|2626|1|", "eventKeyboard|2626|0|",
                        "eventKeyboard|2626|1|", "eventKeyboard|2626|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_129_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_129_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_129_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json", "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json",
                    "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json", "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json",
                    "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json", "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json",
                    "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json", "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json",
                    "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json", "keyboard_KEY_SETUP_press.json", "keyboard_KEY_SETUP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|141|1", "eventKeyboard|141|0", "eventKeyboard|141|1",
                             "eventKeyboard|141|0", "eventKeyboard|141|1", "eventKeyboard|141|0",
                             "eventKeyboard|141|1", "eventKeyboard|141|0", "eventKeyboard|141|1",
                             "eventKeyboard|141|0", "eventKeyboard|141|1", "eventKeyboard|141|0",
                             "eventKeyboard|141|1", "eventKeyboard|141|0", "eventKeyboard|141|1",
                             "eventKeyboard|141|0", "eventKeyboard|141|1", "eventKeyboard|141|0",
                             "eventKeyboard|141|1", "eventKeyboard|141|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2801|1|", "eventKeyboard|2801|0|", "eventKeyboard|2801|1|",
                        "eventKeyboard|2801|0|", "eventKeyboard|2801|1|", "eventKeyboard|2801|0|",
                        "eventKeyboard|2801|1|", "eventKeyboard|2801|0|", "eventKeyboard|2801|1|",
                        "eventKeyboard|2801|0|", "eventKeyboard|2801|1|", "eventKeyboard|2801|0|",
                        "eventKeyboard|2801|1|", "eventKeyboard|2801|0|", "eventKeyboard|2801|1|",
                        "eventKeyboard|2801|0|", "eventKeyboard|2801|1|", "eventKeyboard|2801|0|",
                        "eventKeyboard|2801|1|", "eventKeyboard|2801|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_130_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_130_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_130_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json", "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json",
                    "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json", "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json",
                    "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json", "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json",
                    "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json", "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json",
                    "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json", "keyboard_KEY_SLEEP_press.json", "keyboard_KEY_SLEEP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|142|1", "eventKeyboard|142|0", "eventKeyboard|142|1",
                             "eventKeyboard|142|0", "eventKeyboard|142|1", "eventKeyboard|142|0",
                             "eventKeyboard|142|1", "eventKeyboard|142|0", "eventKeyboard|142|1",
                             "eventKeyboard|142|0", "eventKeyboard|142|1", "eventKeyboard|142|0",
                             "eventKeyboard|142|1", "eventKeyboard|142|0", "eventKeyboard|142|1",
                             "eventKeyboard|142|0", "eventKeyboard|142|1", "eventKeyboard|142|0",
                             "eventKeyboard|142|1", "eventKeyboard|142|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2600|1|", "eventKeyboard|2600|0|", "eventKeyboard|2600|1|",
                        "eventKeyboard|2600|0|", "eventKeyboard|2600|1|", "eventKeyboard|2600|0|",
                        "eventKeyboard|2600|1|", "eventKeyboard|2600|0|", "eventKeyboard|2600|1|",
                        "eventKeyboard|2600|0|", "eventKeyboard|2600|1|", "eventKeyboard|2600|0|",
                        "eventKeyboard|2600|1|", "eventKeyboard|2600|0|", "eventKeyboard|2600|1|",
                        "eventKeyboard|2600|0|", "eventKeyboard|2600|1|", "eventKeyboard|2600|0|",
                        "eventKeyboard|2600|1|", "eventKeyboard|2600|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_131_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_131_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_131_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json", "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json",
                    "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json", "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json",
                    "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json", "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json",
                    "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json", "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json",
                    "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json", "keyboard_KEY_WAKEUP_press.json", "keyboard_KEY_WAKEUP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|143|1", "eventKeyboard|143|0", "eventKeyboard|143|1",
                             "eventKeyboard|143|0", "eventKeyboard|143|1", "eventKeyboard|143|0",
                             "eventKeyboard|143|1", "eventKeyboard|143|0", "eventKeyboard|143|1",
                             "eventKeyboard|143|0", "eventKeyboard|143|1", "eventKeyboard|143|0",
                             "eventKeyboard|143|1", "eventKeyboard|143|0", "eventKeyboard|143|1",
                             "eventKeyboard|143|0", "eventKeyboard|143|1", "eventKeyboard|143|0",
                             "eventKeyboard|143|1", "eventKeyboard|143|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2802|1|", "eventKeyboard|2802|0|", "eventKeyboard|2802|1|",
                        "eventKeyboard|2802|0|", "eventKeyboard|2802|1|", "eventKeyboard|2802|0|",
                        "eventKeyboard|2802|1|", "eventKeyboard|2802|0|", "eventKeyboard|2802|1|",
                        "eventKeyboard|2802|0|", "eventKeyboard|2802|1|", "eventKeyboard|2802|0|",
                        "eventKeyboard|2802|1|", "eventKeyboard|2802|0|", "eventKeyboard|2802|1|",
                        "eventKeyboard|2802|0|", "eventKeyboard|2802|1|", "eventKeyboard|2802|0|",
                        "eventKeyboard|2802|1|", "eventKeyboard|2802|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_132_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_132_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_132_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json", "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json",
                    "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json", "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json",
                    "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json", "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json",
                    "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json", "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json",
                    "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json", "keyboard_KEY_FILE_press.json", "keyboard_KEY_FILE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|144|1", "eventKeyboard|144|0", "eventKeyboard|144|1",
                             "eventKeyboard|144|0", "eventKeyboard|144|1", "eventKeyboard|144|0",
                             "eventKeyboard|144|1", "eventKeyboard|144|0", "eventKeyboard|144|1",
                             "eventKeyboard|144|0", "eventKeyboard|144|1", "eventKeyboard|144|0",
                             "eventKeyboard|144|1", "eventKeyboard|144|0", "eventKeyboard|144|1",
                             "eventKeyboard|144|0", "eventKeyboard|144|1", "eventKeyboard|144|0",
                             "eventKeyboard|144|1", "eventKeyboard|144|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2627|1|", "eventKeyboard|2627|0|", "eventKeyboard|2627|1|",
                        "eventKeyboard|2627|0|", "eventKeyboard|2627|1|", "eventKeyboard|2627|0|",
                        "eventKeyboard|2627|1|", "eventKeyboard|2627|0|", "eventKeyboard|2627|1|",
                        "eventKeyboard|2627|0|", "eventKeyboard|2627|1|", "eventKeyboard|2627|0|",
                        "eventKeyboard|2627|1|", "eventKeyboard|2627|0|", "eventKeyboard|2627|1|",
                        "eventKeyboard|2627|0|", "eventKeyboard|2627|1|", "eventKeyboard|2627|0|",
                        "eventKeyboard|2627|1|", "eventKeyboard|2627|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_133_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_133_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_133_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json", "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json",
                    "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json", "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json",
                    "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json", "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json",
                    "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json", "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json",
                    "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json", "keyboard_KEY_SENDFILE_press.json", "keyboard_KEY_SENDFILE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|145|1", "eventKeyboard|145|0", "eventKeyboard|145|1",
                             "eventKeyboard|145|0", "eventKeyboard|145|1", "eventKeyboard|145|0",
                             "eventKeyboard|145|1", "eventKeyboard|145|0", "eventKeyboard|145|1",
                             "eventKeyboard|145|0", "eventKeyboard|145|1", "eventKeyboard|145|0",
                             "eventKeyboard|145|1", "eventKeyboard|145|0", "eventKeyboard|145|1",
                             "eventKeyboard|145|0", "eventKeyboard|145|1", "eventKeyboard|145|0",
                             "eventKeyboard|145|1", "eventKeyboard|145|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2803|1|", "eventKeyboard|2803|0|", "eventKeyboard|2803|1|",
                        "eventKeyboard|2803|0|", "eventKeyboard|2803|1|", "eventKeyboard|2803|0|",
                        "eventKeyboard|2803|1|", "eventKeyboard|2803|0|", "eventKeyboard|2803|1|",
                        "eventKeyboard|2803|0|", "eventKeyboard|2803|1|", "eventKeyboard|2803|0|",
                        "eventKeyboard|2803|1|", "eventKeyboard|2803|0|", "eventKeyboard|2803|1|",
                        "eventKeyboard|2803|0|", "eventKeyboard|2803|1|", "eventKeyboard|2803|0|",
                        "eventKeyboard|2803|1|", "eventKeyboard|2803|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_134_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_134_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_134_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json", "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json",
                    "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json", "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json",
                    "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json", "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json",
                    "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json", "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json",
                    "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json", "keyboard_KEY_DELETEFILE_press.json", "keyboard_KEY_DELETEFILE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|146|1", "eventKeyboard|146|0", "eventKeyboard|146|1",
                             "eventKeyboard|146|0", "eventKeyboard|146|1", "eventKeyboard|146|0",
                             "eventKeyboard|146|1", "eventKeyboard|146|0", "eventKeyboard|146|1",
                             "eventKeyboard|146|0", "eventKeyboard|146|1", "eventKeyboard|146|0",
                             "eventKeyboard|146|1", "eventKeyboard|146|0", "eventKeyboard|146|1",
                             "eventKeyboard|146|0", "eventKeyboard|146|1", "eventKeyboard|146|0",
                             "eventKeyboard|146|1", "eventKeyboard|146|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2804|1|", "eventKeyboard|2804|0|", "eventKeyboard|2804|1|",
                        "eventKeyboard|2804|0|", "eventKeyboard|2804|1|", "eventKeyboard|2804|0|",
                        "eventKeyboard|2804|1|", "eventKeyboard|2804|0|", "eventKeyboard|2804|1|",
                        "eventKeyboard|2804|0|", "eventKeyboard|2804|1|", "eventKeyboard|2804|0|",
                        "eventKeyboard|2804|1|", "eventKeyboard|2804|0|", "eventKeyboard|2804|1|",
                        "eventKeyboard|2804|0|", "eventKeyboard|2804|1|", "eventKeyboard|2804|0|",
                        "eventKeyboard|2804|1|", "eventKeyboard|2804|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_135_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_135_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_135_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json", "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json",
                    "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json", "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json",
                    "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json", "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json",
                    "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json", "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json",
                    "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json", "keyboard_KEY_XFER_press.json", "keyboard_KEY_XFER_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|147|1", "eventKeyboard|147|0", "eventKeyboard|147|1",
                             "eventKeyboard|147|0", "eventKeyboard|147|1", "eventKeyboard|147|0",
                             "eventKeyboard|147|1", "eventKeyboard|147|0", "eventKeyboard|147|1",
                             "eventKeyboard|147|0", "eventKeyboard|147|1", "eventKeyboard|147|0",
                             "eventKeyboard|147|1", "eventKeyboard|147|0", "eventKeyboard|147|1",
                             "eventKeyboard|147|0", "eventKeyboard|147|1", "eventKeyboard|147|0",
                             "eventKeyboard|147|1", "eventKeyboard|147|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2805|1|", "eventKeyboard|2805|0|", "eventKeyboard|2805|1|",
                        "eventKeyboard|2805|0|", "eventKeyboard|2805|1|", "eventKeyboard|2805|0|",
                        "eventKeyboard|2805|1|", "eventKeyboard|2805|0|", "eventKeyboard|2805|1|",
                        "eventKeyboard|2805|0|", "eventKeyboard|2805|1|", "eventKeyboard|2805|0|",
                        "eventKeyboard|2805|1|", "eventKeyboard|2805|0|", "eventKeyboard|2805|1|",
                        "eventKeyboard|2805|0|", "eventKeyboard|2805|1|", "eventKeyboard|2805|0|",
                        "eventKeyboard|2805|1|", "eventKeyboard|2805|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_136_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_136_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_136_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json", "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json",
                    "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json", "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json",
                    "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json", "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json",
                    "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json", "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json",
                    "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json", "keyboard_KEY_PROG1_press.json", "keyboard_KEY_PROG1_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|148|1", "eventKeyboard|148|0", "eventKeyboard|148|1",
                             "eventKeyboard|148|0", "eventKeyboard|148|1", "eventKeyboard|148|0",
                             "eventKeyboard|148|1", "eventKeyboard|148|0", "eventKeyboard|148|1",
                             "eventKeyboard|148|0", "eventKeyboard|148|1", "eventKeyboard|148|0",
                             "eventKeyboard|148|1", "eventKeyboard|148|0", "eventKeyboard|148|1",
                             "eventKeyboard|148|0", "eventKeyboard|148|1", "eventKeyboard|148|0",
                             "eventKeyboard|148|1", "eventKeyboard|148|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2806|1|", "eventKeyboard|2806|0|", "eventKeyboard|2806|1|",
                        "eventKeyboard|2806|0|", "eventKeyboard|2806|1|", "eventKeyboard|2806|0|",
                        "eventKeyboard|2806|1|", "eventKeyboard|2806|0|", "eventKeyboard|2806|1|",
                        "eventKeyboard|2806|0|", "eventKeyboard|2806|1|", "eventKeyboard|2806|0|",
                        "eventKeyboard|2806|1|", "eventKeyboard|2806|0|", "eventKeyboard|2806|1|",
                        "eventKeyboard|2806|0|", "eventKeyboard|2806|1|", "eventKeyboard|2806|0|",
                        "eventKeyboard|2806|1|", "eventKeyboard|2806|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_137_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_137_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_137_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json", "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json",
                    "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json", "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json",
                    "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json", "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json",
                    "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json", "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json",
                    "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json", "keyboard_KEY_PROG2_press.json", "keyboard_KEY_PROG2_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|149|1", "eventKeyboard|149|0", "eventKeyboard|149|1",
                             "eventKeyboard|149|0", "eventKeyboard|149|1", "eventKeyboard|149|0",
                             "eventKeyboard|149|1", "eventKeyboard|149|0", "eventKeyboard|149|1",
                             "eventKeyboard|149|0", "eventKeyboard|149|1", "eventKeyboard|149|0",
                             "eventKeyboard|149|1", "eventKeyboard|149|0", "eventKeyboard|149|1",
                             "eventKeyboard|149|0", "eventKeyboard|149|1", "eventKeyboard|149|0",
                             "eventKeyboard|149|1", "eventKeyboard|149|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2807|1|", "eventKeyboard|2807|0|", "eventKeyboard|2807|1|",
                        "eventKeyboard|2807|0|", "eventKeyboard|2807|1|", "eventKeyboard|2807|0|",
                        "eventKeyboard|2807|1|", "eventKeyboard|2807|0|", "eventKeyboard|2807|1|",
                        "eventKeyboard|2807|0|", "eventKeyboard|2807|1|", "eventKeyboard|2807|0|",
                        "eventKeyboard|2807|1|", "eventKeyboard|2807|0|", "eventKeyboard|2807|1|",
                        "eventKeyboard|2807|0|", "eventKeyboard|2807|1|", "eventKeyboard|2807|0|",
                        "eventKeyboard|2807|1|", "eventKeyboard|2807|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_138_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_138_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_138_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json", "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json",
                    "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json", "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json",
                    "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json", "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json",
                    "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json", "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json",
                    "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json", "keyboard_KEY_WWW_press.json", "keyboard_KEY_WWW_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|150|1", "eventKeyboard|150|0", "eventKeyboard|150|1",
                             "eventKeyboard|150|0", "eventKeyboard|150|1", "eventKeyboard|150|0",
                             "eventKeyboard|150|1", "eventKeyboard|150|0", "eventKeyboard|150|1",
                             "eventKeyboard|150|0", "eventKeyboard|150|1", "eventKeyboard|150|0",
                             "eventKeyboard|150|1", "eventKeyboard|150|0", "eventKeyboard|150|1",
                             "eventKeyboard|150|0", "eventKeyboard|150|1", "eventKeyboard|150|0",
                             "eventKeyboard|150|1", "eventKeyboard|150|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2052|1|", "eventKeyboard|2052|0|", "eventKeyboard|2052|1|",
                        "eventKeyboard|2052|0|", "eventKeyboard|2052|1|", "eventKeyboard|2052|0|",
                        "eventKeyboard|2052|1|", "eventKeyboard|2052|0|", "eventKeyboard|2052|1|",
                        "eventKeyboard|2052|0|", "eventKeyboard|2052|1|", "eventKeyboard|2052|0|",
                        "eventKeyboard|2052|1|", "eventKeyboard|2052|0|", "eventKeyboard|2052|1|",
                        "eventKeyboard|2052|0|", "eventKeyboard|2052|1|", "eventKeyboard|2052|0|",
                        "eventKeyboard|2052|1|", "eventKeyboard|2052|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_139_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_139_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_139_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json", "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json",
                    "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json", "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json",
                    "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json", "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json",
                    "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json", "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json",
                    "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json", "keyboard_KEY_MSDOS_press.json", "keyboard_KEY_MSDOS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|151|1", "eventKeyboard|151|0", "eventKeyboard|151|1",
                             "eventKeyboard|151|0", "eventKeyboard|151|1", "eventKeyboard|151|0",
                             "eventKeyboard|151|1", "eventKeyboard|151|0", "eventKeyboard|151|1",
                             "eventKeyboard|151|0", "eventKeyboard|151|1", "eventKeyboard|151|0",
                             "eventKeyboard|151|1", "eventKeyboard|151|0", "eventKeyboard|151|1",
                             "eventKeyboard|151|0", "eventKeyboard|151|1", "eventKeyboard|151|0",
                             "eventKeyboard|151|1", "eventKeyboard|151|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2808|1|", "eventKeyboard|2808|0|", "eventKeyboard|2808|1|",
                        "eventKeyboard|2808|0|", "eventKeyboard|2808|1|", "eventKeyboard|2808|0|",
                        "eventKeyboard|2808|1|", "eventKeyboard|2808|0|", "eventKeyboard|2808|1|",
                        "eventKeyboard|2808|0|", "eventKeyboard|2808|1|", "eventKeyboard|2808|0|",
                        "eventKeyboard|2808|1|", "eventKeyboard|2808|0|", "eventKeyboard|2808|1|",
                        "eventKeyboard|2808|0|", "eventKeyboard|2808|1|", "eventKeyboard|2808|0|",
                        "eventKeyboard|2808|1|", "eventKeyboard|2808|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_140_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_140_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_140_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json", "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json",
                    "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json", "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json",
                    "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json", "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json",
                    "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json", "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json",
                    "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json", "keyboard_KEY_SCREENLOCK_press.json", "keyboard_KEY_SCREENLOCK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|152|1", "eventKeyboard|152|0", "eventKeyboard|152|1",
                             "eventKeyboard|152|0", "eventKeyboard|152|1", "eventKeyboard|152|0",
                             "eventKeyboard|152|1", "eventKeyboard|152|0", "eventKeyboard|152|1",
                             "eventKeyboard|152|0", "eventKeyboard|152|1", "eventKeyboard|152|0",
                             "eventKeyboard|152|1", "eventKeyboard|152|0", "eventKeyboard|152|1",
                             "eventKeyboard|152|0", "eventKeyboard|152|1", "eventKeyboard|152|0",
                             "eventKeyboard|152|1", "eventKeyboard|152|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2809|1|", "eventKeyboard|2809|0|", "eventKeyboard|2809|1|",
                        "eventKeyboard|2809|0|", "eventKeyboard|2809|1|", "eventKeyboard|2809|0|",
                        "eventKeyboard|2809|1|", "eventKeyboard|2809|0|", "eventKeyboard|2809|1|",
                        "eventKeyboard|2809|0|", "eventKeyboard|2809|1|", "eventKeyboard|2809|0|",
                        "eventKeyboard|2809|1|", "eventKeyboard|2809|0|", "eventKeyboard|2809|1|",
                        "eventKeyboard|2809|0|", "eventKeyboard|2809|1|", "eventKeyboard|2809|0|",
                        "eventKeyboard|2809|1|", "eventKeyboard|2809|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_141_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_141_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_141_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json", "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json",
                    "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json", "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json",
                    "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json", "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json",
                    "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json", "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json",
                    "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json", "keyboard_KEY_ROTATE_DISPLAY_press.json", "keyboard_KEY_ROTATE_DISPLAY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|153|1", "eventKeyboard|153|0", "eventKeyboard|153|1",
                             "eventKeyboard|153|0", "eventKeyboard|153|1", "eventKeyboard|153|0",
                             "eventKeyboard|153|1", "eventKeyboard|153|0", "eventKeyboard|153|1",
                             "eventKeyboard|153|0", "eventKeyboard|153|1", "eventKeyboard|153|0",
                             "eventKeyboard|153|1", "eventKeyboard|153|0", "eventKeyboard|153|1",
                             "eventKeyboard|153|0", "eventKeyboard|153|1", "eventKeyboard|153|0",
                             "eventKeyboard|153|1", "eventKeyboard|153|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2810|1|", "eventKeyboard|2810|0|", "eventKeyboard|2810|1|",
                        "eventKeyboard|2810|0|", "eventKeyboard|2810|1|", "eventKeyboard|2810|0|",
                        "eventKeyboard|2810|1|", "eventKeyboard|2810|0|", "eventKeyboard|2810|1|",
                        "eventKeyboard|2810|0|", "eventKeyboard|2810|1|", "eventKeyboard|2810|0|",
                        "eventKeyboard|2810|1|", "eventKeyboard|2810|0|", "eventKeyboard|2810|1|",
                        "eventKeyboard|2810|0|", "eventKeyboard|2810|1|", "eventKeyboard|2810|0|",
                        "eventKeyboard|2810|1|", "eventKeyboard|2810|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_142_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_142_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_142_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json", "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json",
                    "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json", "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json",
                    "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json", "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json",
                    "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json", "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json",
                    "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json", "keyboard_KEY_CYCLEWINDOWS_press.json", "keyboard_KEY_CYCLEWINDOWS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|154|1", "eventKeyboard|154|0", "eventKeyboard|154|1",
                             "eventKeyboard|154|0", "eventKeyboard|154|1", "eventKeyboard|154|0",
                             "eventKeyboard|154|1", "eventKeyboard|154|0", "eventKeyboard|154|1",
                             "eventKeyboard|154|0", "eventKeyboard|154|1", "eventKeyboard|154|0",
                             "eventKeyboard|154|1", "eventKeyboard|154|0", "eventKeyboard|154|1",
                             "eventKeyboard|154|0", "eventKeyboard|154|1", "eventKeyboard|154|0",
                             "eventKeyboard|154|1", "eventKeyboard|154|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2811|1|", "eventKeyboard|2811|0|", "eventKeyboard|2811|1|",
                        "eventKeyboard|2811|0|", "eventKeyboard|2811|1|", "eventKeyboard|2811|0|",
                        "eventKeyboard|2811|1|", "eventKeyboard|2811|0|", "eventKeyboard|2811|1|",
                        "eventKeyboard|2811|0|", "eventKeyboard|2811|1|", "eventKeyboard|2811|0|",
                        "eventKeyboard|2811|1|", "eventKeyboard|2811|0|", "eventKeyboard|2811|1|",
                        "eventKeyboard|2811|0|", "eventKeyboard|2811|1|", "eventKeyboard|2811|0|",
                        "eventKeyboard|2811|1|", "eventKeyboard|2811|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_143_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_143_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_143_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json", "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json",
                    "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json", "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json",
                    "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json", "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json",
                    "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json", "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json",
                    "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json", "keyboard_KEY_MAIL_press.json", "keyboard_KEY_MAIL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|155|1", "eventKeyboard|155|0", "eventKeyboard|155|1",
                             "eventKeyboard|155|0", "eventKeyboard|155|1", "eventKeyboard|155|0",
                             "eventKeyboard|155|1", "eventKeyboard|155|0", "eventKeyboard|155|1",
                             "eventKeyboard|155|0", "eventKeyboard|155|1", "eventKeyboard|155|0",
                             "eventKeyboard|155|1", "eventKeyboard|155|0", "eventKeyboard|155|1",
                             "eventKeyboard|155|0", "eventKeyboard|155|1", "eventKeyboard|155|0",
                             "eventKeyboard|155|1", "eventKeyboard|155|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2053|1|", "eventKeyboard|2053|0|", "eventKeyboard|2053|1|",
                        "eventKeyboard|2053|0|", "eventKeyboard|2053|1|", "eventKeyboard|2053|0|",
                        "eventKeyboard|2053|1|", "eventKeyboard|2053|0|", "eventKeyboard|2053|1|",
                        "eventKeyboard|2053|0|", "eventKeyboard|2053|1|", "eventKeyboard|2053|0|",
                        "eventKeyboard|2053|1|", "eventKeyboard|2053|0|", "eventKeyboard|2053|1|",
                        "eventKeyboard|2053|0|", "eventKeyboard|2053|1|", "eventKeyboard|2053|0|",
                        "eventKeyboard|2053|1|", "eventKeyboard|2053|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_144_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_144_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_144_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json", "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json",
                    "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json", "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json",
                    "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json", "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json",
                    "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json", "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json",
                    "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json", "keyboard_KEY_BOOKMARKS_press.json", "keyboard_KEY_BOOKMARKS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|156|1", "eventKeyboard|156|0", "eventKeyboard|156|1",
                             "eventKeyboard|156|0", "eventKeyboard|156|1", "eventKeyboard|156|0",
                             "eventKeyboard|156|1", "eventKeyboard|156|0", "eventKeyboard|156|1",
                             "eventKeyboard|156|0", "eventKeyboard|156|1", "eventKeyboard|156|0",
                             "eventKeyboard|156|1", "eventKeyboard|156|0", "eventKeyboard|156|1",
                             "eventKeyboard|156|0", "eventKeyboard|156|1", "eventKeyboard|156|0",
                             "eventKeyboard|156|1", "eventKeyboard|156|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2628|1|", "eventKeyboard|2628|0|", "eventKeyboard|2628|1|",
                        "eventKeyboard|2628|0|", "eventKeyboard|2628|1|", "eventKeyboard|2628|0|",
                        "eventKeyboard|2628|1|", "eventKeyboard|2628|0|", "eventKeyboard|2628|1|",
                        "eventKeyboard|2628|0|", "eventKeyboard|2628|1|", "eventKeyboard|2628|0|",
                        "eventKeyboard|2628|1|", "eventKeyboard|2628|0|", "eventKeyboard|2628|1|",
                        "eventKeyboard|2628|0|", "eventKeyboard|2628|1|", "eventKeyboard|2628|0|",
                        "eventKeyboard|2628|1|", "eventKeyboard|2628|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_145_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_145_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_145_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json", "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json",
                    "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json", "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json",
                    "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json", "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json",
                    "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json", "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json",
                    "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json", "keyboard_KEY_COMPUTER_press.json", "keyboard_KEY_COMPUTER_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|157|1", "eventKeyboard|157|0", "eventKeyboard|157|1",
                             "eventKeyboard|157|0", "eventKeyboard|157|1", "eventKeyboard|157|0",
                             "eventKeyboard|157|1", "eventKeyboard|157|0", "eventKeyboard|157|1",
                             "eventKeyboard|157|0", "eventKeyboard|157|1", "eventKeyboard|157|0",
                             "eventKeyboard|157|1", "eventKeyboard|157|0", "eventKeyboard|157|1",
                             "eventKeyboard|157|0", "eventKeyboard|157|1", "eventKeyboard|157|0",
                             "eventKeyboard|157|1", "eventKeyboard|157|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2812|1|", "eventKeyboard|2812|0|", "eventKeyboard|2812|1|",
                        "eventKeyboard|2812|0|", "eventKeyboard|2812|1|", "eventKeyboard|2812|0|",
                        "eventKeyboard|2812|1|", "eventKeyboard|2812|0|", "eventKeyboard|2812|1|",
                        "eventKeyboard|2812|0|", "eventKeyboard|2812|1|", "eventKeyboard|2812|0|",
                        "eventKeyboard|2812|1|", "eventKeyboard|2812|0|", "eventKeyboard|2812|1|",
                        "eventKeyboard|2812|0|", "eventKeyboard|2812|1|", "eventKeyboard|2812|0|",
                        "eventKeyboard|2812|1|", "eventKeyboard|2812|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_146_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_146_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_146_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json", "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json",
                    "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json", "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json",
                    "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json", "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json",
                    "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json", "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json",
                    "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json", "keyboard_KEY_BACK_press.json", "keyboard_KEY_BACK_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|158|1", "eventKeyboard|158|0", "eventKeyboard|158|1",
                             "eventKeyboard|158|0", "eventKeyboard|158|1", "eventKeyboard|158|0",
                             "eventKeyboard|158|1", "eventKeyboard|158|0", "eventKeyboard|158|1",
                             "eventKeyboard|158|0", "eventKeyboard|158|1", "eventKeyboard|158|0",
                             "eventKeyboard|158|1", "eventKeyboard|158|0", "eventKeyboard|158|1",
                             "eventKeyboard|158|0", "eventKeyboard|158|1", "eventKeyboard|158|0",
                             "eventKeyboard|158|1", "eventKeyboard|158|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2|1|", "eventKeyboard|2|0|", "eventKeyboard|2|1|",
                        "eventKeyboard|2|0|", "eventKeyboard|2|1|", "eventKeyboard|2|0|",
                        "eventKeyboard|2|1|", "eventKeyboard|2|0|", "eventKeyboard|2|1|",
                        "eventKeyboard|2|0|", "eventKeyboard|2|1|", "eventKeyboard|2|0|",
                        "eventKeyboard|2|1|", "eventKeyboard|2|0|", "eventKeyboard|2|1|",
                        "eventKeyboard|2|0|", "eventKeyboard|2|1|", "eventKeyboard|2|0|",
                        "eventKeyboard|2|1|", "eventKeyboard|2|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_147_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_147_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_147_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json", "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json",
                    "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json", "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json",
                    "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json", "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json",
                    "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json", "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json",
                    "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json", "keyboard_KEY_FORWARD_press.json", "keyboard_KEY_FORWARD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|159|1", "eventKeyboard|159|0", "eventKeyboard|159|1",
                             "eventKeyboard|159|0", "eventKeyboard|159|1", "eventKeyboard|159|0",
                             "eventKeyboard|159|1", "eventKeyboard|159|0", "eventKeyboard|159|1",
                             "eventKeyboard|159|0", "eventKeyboard|159|1", "eventKeyboard|159|0",
                             "eventKeyboard|159|1", "eventKeyboard|159|0", "eventKeyboard|159|1",
                             "eventKeyboard|159|0", "eventKeyboard|159|1", "eventKeyboard|159|0",
                             "eventKeyboard|159|1", "eventKeyboard|159|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2084|1|", "eventKeyboard|2084|0|", "eventKeyboard|2084|1|",
                        "eventKeyboard|2084|0|", "eventKeyboard|2084|1|", "eventKeyboard|2084|0|",
                        "eventKeyboard|2084|1|", "eventKeyboard|2084|0|", "eventKeyboard|2084|1|",
                        "eventKeyboard|2084|0|", "eventKeyboard|2084|1|", "eventKeyboard|2084|0|",
                        "eventKeyboard|2084|1|", "eventKeyboard|2084|0|", "eventKeyboard|2084|1|",
                        "eventKeyboard|2084|0|", "eventKeyboard|2084|1|", "eventKeyboard|2084|0|",
                        "eventKeyboard|2084|1|", "eventKeyboard|2084|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_148_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_148_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_148_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json", "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json",
                    "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json", "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json",
                    "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json", "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json",
                    "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json", "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json",
                    "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json", "keyboard_KEY_CLOSECD_press.json", "keyboard_KEY_CLOSECD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|160|1", "eventKeyboard|160|0", "eventKeyboard|160|1",
                             "eventKeyboard|160|0", "eventKeyboard|160|1", "eventKeyboard|160|0",
                             "eventKeyboard|160|1", "eventKeyboard|160|0", "eventKeyboard|160|1",
                             "eventKeyboard|160|0", "eventKeyboard|160|1", "eventKeyboard|160|0",
                             "eventKeyboard|160|1", "eventKeyboard|160|0", "eventKeyboard|160|1",
                             "eventKeyboard|160|0", "eventKeyboard|160|1", "eventKeyboard|160|0",
                             "eventKeyboard|160|1", "eventKeyboard|160|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2087|1|", "eventKeyboard|2087|0|", "eventKeyboard|2087|1|",
                        "eventKeyboard|2087|0|", "eventKeyboard|2087|1|", "eventKeyboard|2087|0|",
                        "eventKeyboard|2087|1|", "eventKeyboard|2087|0|", "eventKeyboard|2087|1|",
                        "eventKeyboard|2087|0|", "eventKeyboard|2087|1|", "eventKeyboard|2087|0|",
                        "eventKeyboard|2087|1|", "eventKeyboard|2087|0|", "eventKeyboard|2087|1|",
                        "eventKeyboard|2087|0|", "eventKeyboard|2087|1|", "eventKeyboard|2087|0|",
                        "eventKeyboard|2087|1|", "eventKeyboard|2087|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_149_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_149_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_149_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json", "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json",
                    "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json", "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json",
                    "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json", "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json",
                    "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json", "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json",
                    "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json", "keyboard_KEY_EJECTCD_press.json", "keyboard_KEY_EJECTCD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|161|1", "eventKeyboard|161|0", "eventKeyboard|161|1",
                             "eventKeyboard|161|0", "eventKeyboard|161|1", "eventKeyboard|161|0",
                             "eventKeyboard|161|1", "eventKeyboard|161|0", "eventKeyboard|161|1",
                             "eventKeyboard|161|0", "eventKeyboard|161|1", "eventKeyboard|161|0",
                             "eventKeyboard|161|1", "eventKeyboard|161|0", "eventKeyboard|161|1",
                             "eventKeyboard|161|0", "eventKeyboard|161|1", "eventKeyboard|161|0",
                             "eventKeyboard|161|1", "eventKeyboard|161|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2088|1|", "eventKeyboard|2088|0|", "eventKeyboard|2088|1|",
                        "eventKeyboard|2088|0|", "eventKeyboard|2088|1|", "eventKeyboard|2088|0|",
                        "eventKeyboard|2088|1|", "eventKeyboard|2088|0|", "eventKeyboard|2088|1|",
                        "eventKeyboard|2088|0|", "eventKeyboard|2088|1|", "eventKeyboard|2088|0|",
                        "eventKeyboard|2088|1|", "eventKeyboard|2088|0|", "eventKeyboard|2088|1|",
                        "eventKeyboard|2088|0|", "eventKeyboard|2088|1|", "eventKeyboard|2088|0|",
                        "eventKeyboard|2088|1|", "eventKeyboard|2088|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_150_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_150_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_150_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json", "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json",
                    "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json", "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json",
                    "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json", "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json",
                    "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json", "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json",
                    "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json", "keyboard_KEY_EJECTCLOSECD_press.json", "keyboard_KEY_EJECTCLOSECD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|162|1", "eventKeyboard|162|0", "eventKeyboard|162|1",
                             "eventKeyboard|162|0", "eventKeyboard|162|1", "eventKeyboard|162|0",
                             "eventKeyboard|162|1", "eventKeyboard|162|0", "eventKeyboard|162|1",
                             "eventKeyboard|162|0", "eventKeyboard|162|1", "eventKeyboard|162|0",
                             "eventKeyboard|162|1", "eventKeyboard|162|0", "eventKeyboard|162|1",
                             "eventKeyboard|162|0", "eventKeyboard|162|1", "eventKeyboard|162|0",
                             "eventKeyboard|162|1", "eventKeyboard|162|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2813|1|", "eventKeyboard|2813|0|", "eventKeyboard|2813|1|",
                        "eventKeyboard|2813|0|", "eventKeyboard|2813|1|", "eventKeyboard|2813|0|",
                        "eventKeyboard|2813|1|", "eventKeyboard|2813|0|", "eventKeyboard|2813|1|",
                        "eventKeyboard|2813|0|", "eventKeyboard|2813|1|", "eventKeyboard|2813|0|",
                        "eventKeyboard|2813|1|", "eventKeyboard|2813|0|", "eventKeyboard|2813|1|",
                        "eventKeyboard|2813|0|", "eventKeyboard|2813|1|", "eventKeyboard|2813|0|",
                        "eventKeyboard|2813|1|", "eventKeyboard|2813|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_151_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_151_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_151_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json", "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json",
                    "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json", "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json",
                    "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json", "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json",
                    "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json", "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json",
                    "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json", "keyboard_KEY_NEXTSONG_press.json", "keyboard_KEY_NEXTSONG_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|163|1", "eventKeyboard|163|0", "eventKeyboard|163|1",
                             "eventKeyboard|163|0", "eventKeyboard|163|1", "eventKeyboard|163|0",
                             "eventKeyboard|163|1", "eventKeyboard|163|0", "eventKeyboard|163|1",
                             "eventKeyboard|163|0", "eventKeyboard|163|1", "eventKeyboard|163|0",
                             "eventKeyboard|163|1", "eventKeyboard|163|0", "eventKeyboard|163|1",
                             "eventKeyboard|163|0", "eventKeyboard|163|1", "eventKeyboard|163|0",
                             "eventKeyboard|163|1", "eventKeyboard|163|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|12|1|", "eventKeyboard|12|0|", "eventKeyboard|12|1|",
                        "eventKeyboard|12|0|", "eventKeyboard|12|1|", "eventKeyboard|12|0|",
                        "eventKeyboard|12|1|", "eventKeyboard|12|0|", "eventKeyboard|12|1|",
                        "eventKeyboard|12|0|", "eventKeyboard|12|1|", "eventKeyboard|12|0|",
                        "eventKeyboard|12|1|", "eventKeyboard|12|0|", "eventKeyboard|12|1|",
                        "eventKeyboard|12|0|", "eventKeyboard|12|1|", "eventKeyboard|12|0|",
                        "eventKeyboard|12|1|", "eventKeyboard|12|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_152_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_152_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_152_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json", "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json",
                    "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json", "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json",
                    "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json", "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json",
                    "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json", "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json",
                    "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json", "keyboard_KEY_PLAYPAUSE_press.json", "keyboard_KEY_PLAYPAUSE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|164|1", "eventKeyboard|164|0", "eventKeyboard|164|1",
                             "eventKeyboard|164|0", "eventKeyboard|164|1", "eventKeyboard|164|0",
                             "eventKeyboard|164|1", "eventKeyboard|164|0", "eventKeyboard|164|1",
                             "eventKeyboard|164|0", "eventKeyboard|164|1", "eventKeyboard|164|0",
                             "eventKeyboard|164|1", "eventKeyboard|164|0", "eventKeyboard|164|1",
                             "eventKeyboard|164|0", "eventKeyboard|164|1", "eventKeyboard|164|0",
                             "eventKeyboard|164|1", "eventKeyboard|164|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|10|1|", "eventKeyboard|10|0|", "eventKeyboard|10|1|",
                        "eventKeyboard|10|0|", "eventKeyboard|10|1|", "eventKeyboard|10|0|",
                        "eventKeyboard|10|1|", "eventKeyboard|10|0|", "eventKeyboard|10|1|",
                        "eventKeyboard|10|0|", "eventKeyboard|10|1|", "eventKeyboard|10|0|",
                        "eventKeyboard|10|1|", "eventKeyboard|10|0|", "eventKeyboard|10|1|",
                        "eventKeyboard|10|0|", "eventKeyboard|10|1|", "eventKeyboard|10|0|",
                        "eventKeyboard|10|1|", "eventKeyboard|10|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_153_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_153_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_153_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json", "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json",
                    "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json", "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json",
                    "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json", "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json",
                    "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json", "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json",
                    "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json", "keyboard_KEY_PREVIOUSSONG_press.json", "keyboard_KEY_PREVIOUSSONG_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|165|1", "eventKeyboard|165|0", "eventKeyboard|165|1",
                             "eventKeyboard|165|0", "eventKeyboard|165|1", "eventKeyboard|165|0",
                             "eventKeyboard|165|1", "eventKeyboard|165|0", "eventKeyboard|165|1",
                             "eventKeyboard|165|0", "eventKeyboard|165|1", "eventKeyboard|165|0",
                             "eventKeyboard|165|1", "eventKeyboard|165|0", "eventKeyboard|165|1",
                             "eventKeyboard|165|0", "eventKeyboard|165|1", "eventKeyboard|165|0",
                             "eventKeyboard|165|1", "eventKeyboard|165|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|13|1|", "eventKeyboard|13|0|", "eventKeyboard|13|1|",
                        "eventKeyboard|13|0|", "eventKeyboard|13|1|", "eventKeyboard|13|0|",
                        "eventKeyboard|13|1|", "eventKeyboard|13|0|", "eventKeyboard|13|1|",
                        "eventKeyboard|13|0|", "eventKeyboard|13|1|", "eventKeyboard|13|0|",
                        "eventKeyboard|13|1|", "eventKeyboard|13|0|", "eventKeyboard|13|1|",
                        "eventKeyboard|13|0|", "eventKeyboard|13|1|", "eventKeyboard|13|0|",
                        "eventKeyboard|13|1|", "eventKeyboard|13|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_154_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_154_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_154_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json", "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json",
                    "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json", "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json",
                    "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json", "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json",
                    "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json", "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json",
                    "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json", "keyboard_KEY_STOPCD_press.json", "keyboard_KEY_STOPCD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|166|1", "eventKeyboard|166|0", "eventKeyboard|166|1",
                             "eventKeyboard|166|0", "eventKeyboard|166|1", "eventKeyboard|166|0",
                             "eventKeyboard|166|1", "eventKeyboard|166|0", "eventKeyboard|166|1",
                             "eventKeyboard|166|0", "eventKeyboard|166|1", "eventKeyboard|166|0",
                             "eventKeyboard|166|1", "eventKeyboard|166|0", "eventKeyboard|166|1",
                             "eventKeyboard|166|0", "eventKeyboard|166|1", "eventKeyboard|166|0",
                             "eventKeyboard|166|1", "eventKeyboard|166|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|11|1|", "eventKeyboard|11|0|", "eventKeyboard|11|1|",
                        "eventKeyboard|11|0|", "eventKeyboard|11|1|", "eventKeyboard|11|0|",
                        "eventKeyboard|11|1|", "eventKeyboard|11|0|", "eventKeyboard|11|1|",
                        "eventKeyboard|11|0|", "eventKeyboard|11|1|", "eventKeyboard|11|0|",
                        "eventKeyboard|11|1|", "eventKeyboard|11|0|", "eventKeyboard|11|1|",
                        "eventKeyboard|11|0|", "eventKeyboard|11|1|", "eventKeyboard|11|0|",
                        "eventKeyboard|11|1|", "eventKeyboard|11|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_155_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_155_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_155_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json", "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json",
                    "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json", "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json",
                    "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json", "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json",
                    "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json", "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json",
                    "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json", "keyboard_KEY_RECORD_press.json", "keyboard_KEY_RECORD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|167|1", "eventKeyboard|167|0", "eventKeyboard|167|1",
                             "eventKeyboard|167|0", "eventKeyboard|167|1", "eventKeyboard|167|0",
                             "eventKeyboard|167|1", "eventKeyboard|167|0", "eventKeyboard|167|1",
                             "eventKeyboard|167|0", "eventKeyboard|167|1", "eventKeyboard|167|0",
                             "eventKeyboard|167|1", "eventKeyboard|167|0", "eventKeyboard|167|1",
                             "eventKeyboard|167|0", "eventKeyboard|167|1", "eventKeyboard|167|0",
                             "eventKeyboard|167|1", "eventKeyboard|167|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2089|1|", "eventKeyboard|2089|0|", "eventKeyboard|2089|1|",
                        "eventKeyboard|2089|0|", "eventKeyboard|2089|1|", "eventKeyboard|2089|0|",
                        "eventKeyboard|2089|1|", "eventKeyboard|2089|0|", "eventKeyboard|2089|1|",
                        "eventKeyboard|2089|0|", "eventKeyboard|2089|1|", "eventKeyboard|2089|0|",
                        "eventKeyboard|2089|1|", "eventKeyboard|2089|0|", "eventKeyboard|2089|1|",
                        "eventKeyboard|2089|0|", "eventKeyboard|2089|1|", "eventKeyboard|2089|0|",
                        "eventKeyboard|2089|1|", "eventKeyboard|2089|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_156_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_156_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_156_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json", "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json",
                    "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json", "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json",
                    "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json", "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json",
                    "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json", "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json",
                    "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json", "keyboard_KEY_REWIND_press.json", "keyboard_KEY_REWIND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|168|1", "eventKeyboard|168|0", "eventKeyboard|168|1",
                             "eventKeyboard|168|0", "eventKeyboard|168|1", "eventKeyboard|168|0",
                             "eventKeyboard|168|1", "eventKeyboard|168|0", "eventKeyboard|168|1",
                             "eventKeyboard|168|0", "eventKeyboard|168|1", "eventKeyboard|168|0",
                             "eventKeyboard|168|1", "eventKeyboard|168|0", "eventKeyboard|168|1",
                             "eventKeyboard|168|0", "eventKeyboard|168|1", "eventKeyboard|168|0",
                             "eventKeyboard|168|1", "eventKeyboard|168|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|14|1|", "eventKeyboard|14|0|", "eventKeyboard|14|1|",
                        "eventKeyboard|14|0|", "eventKeyboard|14|1|", "eventKeyboard|14|0|",
                        "eventKeyboard|14|1|", "eventKeyboard|14|0|", "eventKeyboard|14|1|",
                        "eventKeyboard|14|0|", "eventKeyboard|14|1|", "eventKeyboard|14|0|",
                        "eventKeyboard|14|1|", "eventKeyboard|14|0|", "eventKeyboard|14|1|",
                        "eventKeyboard|14|0|", "eventKeyboard|14|1|", "eventKeyboard|14|0|",
                        "eventKeyboard|14|1|", "eventKeyboard|14|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_157_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_157_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_157_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json", "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json",
                    "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json", "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json",
                    "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json", "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json",
                    "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json", "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json",
                    "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json", "keyboard_KEY_PHONE_press.json", "keyboard_KEY_PHONE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|169|1", "eventKeyboard|169|0", "eventKeyboard|169|1",
                             "eventKeyboard|169|0", "eventKeyboard|169|1", "eventKeyboard|169|0",
                             "eventKeyboard|169|1", "eventKeyboard|169|0", "eventKeyboard|169|1",
                             "eventKeyboard|169|0", "eventKeyboard|169|1", "eventKeyboard|169|0",
                             "eventKeyboard|169|1", "eventKeyboard|169|0", "eventKeyboard|169|1",
                             "eventKeyboard|169|0", "eventKeyboard|169|1", "eventKeyboard|169|0",
                             "eventKeyboard|169|1", "eventKeyboard|169|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|3|1|", "eventKeyboard|3|0|", "eventKeyboard|3|1|",
                        "eventKeyboard|3|0|", "eventKeyboard|3|1|", "eventKeyboard|3|0|",
                        "eventKeyboard|3|1|", "eventKeyboard|3|0|", "eventKeyboard|3|1|",
                        "eventKeyboard|3|0|", "eventKeyboard|3|1|", "eventKeyboard|3|0|",
                        "eventKeyboard|3|1|", "eventKeyboard|3|0|", "eventKeyboard|3|1|",
                        "eventKeyboard|3|0|", "eventKeyboard|3|1|", "eventKeyboard|3|0|",
                        "eventKeyboard|3|1|", "eventKeyboard|3|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_158_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_158_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_158_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json", "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json",
                    "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json", "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json",
                    "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json", "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json",
                    "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json", "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json",
                    "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json", "keyboard_KEY_ISO_press.json", "keyboard_KEY_ISO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|170|1", "eventKeyboard|170|0", "eventKeyboard|170|1",
                             "eventKeyboard|170|0", "eventKeyboard|170|1", "eventKeyboard|170|0",
                             "eventKeyboard|170|1", "eventKeyboard|170|0", "eventKeyboard|170|1",
                             "eventKeyboard|170|0", "eventKeyboard|170|1", "eventKeyboard|170|0",
                             "eventKeyboard|170|1", "eventKeyboard|170|0", "eventKeyboard|170|1",
                             "eventKeyboard|170|0", "eventKeyboard|170|1", "eventKeyboard|170|0",
                             "eventKeyboard|170|1", "eventKeyboard|170|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2814|1|", "eventKeyboard|2814|0|", "eventKeyboard|2814|1|",
                        "eventKeyboard|2814|0|", "eventKeyboard|2814|1|", "eventKeyboard|2814|0|",
                        "eventKeyboard|2814|1|", "eventKeyboard|2814|0|", "eventKeyboard|2814|1|",
                        "eventKeyboard|2814|0|", "eventKeyboard|2814|1|", "eventKeyboard|2814|0|",
                        "eventKeyboard|2814|1|", "eventKeyboard|2814|0|", "eventKeyboard|2814|1|",
                        "eventKeyboard|2814|0|", "eventKeyboard|2814|1|", "eventKeyboard|2814|0|",
                        "eventKeyboard|2814|1|", "eventKeyboard|2814|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_159_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_159_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_159_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json", "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json",
                    "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json", "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json",
                    "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json", "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json",
                    "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json", "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json",
                    "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json", "keyboard_KEY_CONFIG_press.json", "keyboard_KEY_CONFIG_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|171|1", "eventKeyboard|171|0", "eventKeyboard|171|1",
                             "eventKeyboard|171|0", "eventKeyboard|171|1", "eventKeyboard|171|0",
                             "eventKeyboard|171|1", "eventKeyboard|171|0", "eventKeyboard|171|1",
                             "eventKeyboard|171|0", "eventKeyboard|171|1", "eventKeyboard|171|0",
                             "eventKeyboard|171|1", "eventKeyboard|171|0", "eventKeyboard|171|1",
                             "eventKeyboard|171|0", "eventKeyboard|171|1", "eventKeyboard|171|0",
                             "eventKeyboard|171|1", "eventKeyboard|171|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2634|1|", "eventKeyboard|2634|0|", "eventKeyboard|2634|1|",
                        "eventKeyboard|2634|0|", "eventKeyboard|2634|1|", "eventKeyboard|2634|0|",
                        "eventKeyboard|2634|1|", "eventKeyboard|2634|0|", "eventKeyboard|2634|1|",
                        "eventKeyboard|2634|0|", "eventKeyboard|2634|1|", "eventKeyboard|2634|0|",
                        "eventKeyboard|2634|1|", "eventKeyboard|2634|0|", "eventKeyboard|2634|1|",
                        "eventKeyboard|2634|0|", "eventKeyboard|2634|1|", "eventKeyboard|2634|0|",
                        "eventKeyboard|2634|1|", "eventKeyboard|2634|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_161_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_161_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_161_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json", "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json",
                    "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json", "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json",
                    "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json", "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json",
                    "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json", "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json",
                    "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json", "keyboard_KEY_REFRESH_press.json", "keyboard_KEY_REFRESH_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|173|1", "eventKeyboard|173|0", "eventKeyboard|173|1",
                             "eventKeyboard|173|0", "eventKeyboard|173|1", "eventKeyboard|173|0",
                             "eventKeyboard|173|1", "eventKeyboard|173|0", "eventKeyboard|173|1",
                             "eventKeyboard|173|0", "eventKeyboard|173|1", "eventKeyboard|173|0",
                             "eventKeyboard|173|1", "eventKeyboard|173|0", "eventKeyboard|173|1",
                             "eventKeyboard|173|0", "eventKeyboard|173|1", "eventKeyboard|173|0",
                             "eventKeyboard|173|1", "eventKeyboard|173|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2635|1|", "eventKeyboard|2635|0|", "eventKeyboard|2635|1|",
                        "eventKeyboard|2635|0|", "eventKeyboard|2635|1|", "eventKeyboard|2635|0|",
                        "eventKeyboard|2635|1|", "eventKeyboard|2635|0|", "eventKeyboard|2635|1|",
                        "eventKeyboard|2635|0|", "eventKeyboard|2635|1|", "eventKeyboard|2635|0|",
                        "eventKeyboard|2635|1|", "eventKeyboard|2635|0|", "eventKeyboard|2635|1|",
                        "eventKeyboard|2635|0|", "eventKeyboard|2635|1|", "eventKeyboard|2635|0|",
                        "eventKeyboard|2635|1|", "eventKeyboard|2635|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_162_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_162_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_162_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json", "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json",
                    "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json", "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json",
                    "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json", "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json",
                    "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json", "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json",
                    "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json", "keyboard_KEY_EXIT_press.json", "keyboard_KEY_EXIT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|174|1", "eventKeyboard|174|0", "eventKeyboard|174|1",
                             "eventKeyboard|174|0", "eventKeyboard|174|1", "eventKeyboard|174|0",
                             "eventKeyboard|174|1", "eventKeyboard|174|0", "eventKeyboard|174|1",
                             "eventKeyboard|174|0", "eventKeyboard|174|1", "eventKeyboard|174|0",
                             "eventKeyboard|174|1", "eventKeyboard|174|0", "eventKeyboard|174|1",
                             "eventKeyboard|174|0", "eventKeyboard|174|1", "eventKeyboard|174|0",
                             "eventKeyboard|174|1", "eventKeyboard|174|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2636|1|", "eventKeyboard|2636|0|", "eventKeyboard|2636|1|",
                        "eventKeyboard|2636|0|", "eventKeyboard|2636|1|", "eventKeyboard|2636|0|",
                        "eventKeyboard|2636|1|", "eventKeyboard|2636|0|", "eventKeyboard|2636|1|",
                        "eventKeyboard|2636|0|", "eventKeyboard|2636|1|", "eventKeyboard|2636|0|",
                        "eventKeyboard|2636|1|", "eventKeyboard|2636|0|", "eventKeyboard|2636|1|",
                        "eventKeyboard|2636|0|", "eventKeyboard|2636|1|", "eventKeyboard|2636|0|",
                        "eventKeyboard|2636|1|", "eventKeyboard|2636|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_163_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_163_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_163_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json", "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json",
                    "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json", "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json",
                    "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json", "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json",
                    "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json", "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json",
                    "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json", "keyboard_KEY_MOVE_press.json", "keyboard_KEY_MOVE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|175|1", "eventKeyboard|175|0", "eventKeyboard|175|1",
                             "eventKeyboard|175|0", "eventKeyboard|175|1", "eventKeyboard|175|0",
                             "eventKeyboard|175|1", "eventKeyboard|175|0", "eventKeyboard|175|1",
                             "eventKeyboard|175|0", "eventKeyboard|175|1", "eventKeyboard|175|0",
                             "eventKeyboard|175|1", "eventKeyboard|175|0", "eventKeyboard|175|1",
                             "eventKeyboard|175|0", "eventKeyboard|175|1", "eventKeyboard|175|0",
                             "eventKeyboard|175|1", "eventKeyboard|175|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2815|1|", "eventKeyboard|2815|0|", "eventKeyboard|2815|1|",
                        "eventKeyboard|2815|0|", "eventKeyboard|2815|1|", "eventKeyboard|2815|0|",
                        "eventKeyboard|2815|1|", "eventKeyboard|2815|0|", "eventKeyboard|2815|1|",
                        "eventKeyboard|2815|0|", "eventKeyboard|2815|1|", "eventKeyboard|2815|0|",
                        "eventKeyboard|2815|1|", "eventKeyboard|2815|0|", "eventKeyboard|2815|1|",
                        "eventKeyboard|2815|0|", "eventKeyboard|2815|1|", "eventKeyboard|2815|0|",
                        "eventKeyboard|2815|1|", "eventKeyboard|2815|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_164_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_164_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_164_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json", "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json",
                    "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json", "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json",
                    "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json", "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json",
                    "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json", "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json",
                    "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json", "keyboard_KEY_EDIT_press.json", "keyboard_KEY_EDIT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|176|1", "eventKeyboard|176|0", "eventKeyboard|176|1",
                             "eventKeyboard|176|0", "eventKeyboard|176|1", "eventKeyboard|176|0",
                             "eventKeyboard|176|1", "eventKeyboard|176|0", "eventKeyboard|176|1",
                             "eventKeyboard|176|0", "eventKeyboard|176|1", "eventKeyboard|176|0",
                             "eventKeyboard|176|1", "eventKeyboard|176|0", "eventKeyboard|176|1",
                             "eventKeyboard|176|0", "eventKeyboard|176|1", "eventKeyboard|176|0",
                             "eventKeyboard|176|1", "eventKeyboard|176|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2637|1|", "eventKeyboard|2637|0|", "eventKeyboard|2637|1|",
                        "eventKeyboard|2637|0|", "eventKeyboard|2637|1|", "eventKeyboard|2637|0|",
                        "eventKeyboard|2637|1|", "eventKeyboard|2637|0|", "eventKeyboard|2637|1|",
                        "eventKeyboard|2637|0|", "eventKeyboard|2637|1|", "eventKeyboard|2637|0|",
                        "eventKeyboard|2637|1|", "eventKeyboard|2637|0|", "eventKeyboard|2637|1|",
                        "eventKeyboard|2637|0|", "eventKeyboard|2637|1|", "eventKeyboard|2637|0|",
                        "eventKeyboard|2637|1|", "eventKeyboard|2637|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_165_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_165_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_165_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json", "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json",
                    "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json", "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json",
                    "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json", "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json",
                    "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json", "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json",
                    "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json", "keyboard_KEY_SCROLLUP_press.json", "keyboard_KEY_SCROLLUP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|177|1", "eventKeyboard|177|0", "eventKeyboard|177|1",
                             "eventKeyboard|177|0", "eventKeyboard|177|1", "eventKeyboard|177|0",
                             "eventKeyboard|177|1", "eventKeyboard|177|0", "eventKeyboard|177|1",
                             "eventKeyboard|177|0", "eventKeyboard|177|1", "eventKeyboard|177|0",
                             "eventKeyboard|177|1", "eventKeyboard|177|0", "eventKeyboard|177|1",
                             "eventKeyboard|177|0", "eventKeyboard|177|1", "eventKeyboard|177|0",
                             "eventKeyboard|177|1", "eventKeyboard|177|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2638|1|", "eventKeyboard|2638|0|", "eventKeyboard|2638|1|",
                        "eventKeyboard|2638|0|", "eventKeyboard|2638|1|", "eventKeyboard|2638|0|",
                        "eventKeyboard|2638|1|", "eventKeyboard|2638|0|", "eventKeyboard|2638|1|",
                        "eventKeyboard|2638|0|", "eventKeyboard|2638|1|", "eventKeyboard|2638|0|",
                        "eventKeyboard|2638|1|", "eventKeyboard|2638|0|", "eventKeyboard|2638|1|",
                        "eventKeyboard|2638|0|", "eventKeyboard|2638|1|", "eventKeyboard|2638|0|",
                        "eventKeyboard|2638|1|", "eventKeyboard|2638|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_166_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_166_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_166_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json", "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json",
                    "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json", "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json",
                    "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json", "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json",
                    "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json", "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json",
                    "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json", "keyboard_KEY_SCROLLDOWN_press.json", "keyboard_KEY_SCROLLDOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|178|1", "eventKeyboard|178|0", "eventKeyboard|178|1",
                             "eventKeyboard|178|0", "eventKeyboard|178|1", "eventKeyboard|178|0",
                             "eventKeyboard|178|1", "eventKeyboard|178|0", "eventKeyboard|178|1",
                             "eventKeyboard|178|0", "eventKeyboard|178|1", "eventKeyboard|178|0",
                             "eventKeyboard|178|1", "eventKeyboard|178|0", "eventKeyboard|178|1",
                             "eventKeyboard|178|0", "eventKeyboard|178|1", "eventKeyboard|178|0",
                             "eventKeyboard|178|1", "eventKeyboard|178|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2639|1|", "eventKeyboard|2639|0|", "eventKeyboard|2639|1|",
                        "eventKeyboard|2639|0|", "eventKeyboard|2639|1|", "eventKeyboard|2639|0|",
                        "eventKeyboard|2639|1|", "eventKeyboard|2639|0|", "eventKeyboard|2639|1|",
                        "eventKeyboard|2639|0|", "eventKeyboard|2639|1|", "eventKeyboard|2639|0|",
                        "eventKeyboard|2639|1|", "eventKeyboard|2639|0|", "eventKeyboard|2639|1|",
                        "eventKeyboard|2639|0|", "eventKeyboard|2639|1|", "eventKeyboard|2639|0|",
                        "eventKeyboard|2639|1|", "eventKeyboard|2639|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_167_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_167_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_167_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json", "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json",
                    "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json", "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json",
                    "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json", "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json",
                    "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json", "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json",
                    "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json", "keyboard_KEY_KPLEFTPAREN_press.json", "keyboard_KEY_KPLEFTPAREN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|179|1", "eventKeyboard|179|0", "eventKeyboard|179|1",
                             "eventKeyboard|179|0", "eventKeyboard|179|1", "eventKeyboard|179|0",
                             "eventKeyboard|179|1", "eventKeyboard|179|0", "eventKeyboard|179|1",
                             "eventKeyboard|179|0", "eventKeyboard|179|1", "eventKeyboard|179|0",
                             "eventKeyboard|179|1", "eventKeyboard|179|0", "eventKeyboard|179|1",
                             "eventKeyboard|179|0", "eventKeyboard|179|1", "eventKeyboard|179|0",
                             "eventKeyboard|179|1", "eventKeyboard|179|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2121|1|", "eventKeyboard|2121|0|", "eventKeyboard|2121|1|",
                        "eventKeyboard|2121|0|", "eventKeyboard|2121|1|", "eventKeyboard|2121|0|",
                        "eventKeyboard|2121|1|", "eventKeyboard|2121|0|", "eventKeyboard|2121|1|",
                        "eventKeyboard|2121|0|", "eventKeyboard|2121|1|", "eventKeyboard|2121|0|",
                        "eventKeyboard|2121|1|", "eventKeyboard|2121|0|", "eventKeyboard|2121|1|",
                        "eventKeyboard|2121|0|", "eventKeyboard|2121|1|", "eventKeyboard|2121|0|",
                        "eventKeyboard|2121|1|", "eventKeyboard|2121|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_168_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_168_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_168_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json", "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json",
                    "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json", "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json",
                    "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json", "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json",
                    "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json", "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json",
                    "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json", "keyboard_KEY_KPRIGHTPAREN_press.json", "keyboard_KEY_KPRIGHTPAREN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|180|1", "eventKeyboard|180|0", "eventKeyboard|180|1",
                             "eventKeyboard|180|0", "eventKeyboard|180|1", "eventKeyboard|180|0",
                             "eventKeyboard|180|1", "eventKeyboard|180|0", "eventKeyboard|180|1",
                             "eventKeyboard|180|0", "eventKeyboard|180|1", "eventKeyboard|180|0",
                             "eventKeyboard|180|1", "eventKeyboard|180|0", "eventKeyboard|180|1",
                             "eventKeyboard|180|0", "eventKeyboard|180|1", "eventKeyboard|180|0",
                             "eventKeyboard|180|1", "eventKeyboard|180|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2122|1|", "eventKeyboard|2122|0|", "eventKeyboard|2122|1|",
                        "eventKeyboard|2122|0|", "eventKeyboard|2122|1|", "eventKeyboard|2122|0|",
                        "eventKeyboard|2122|1|", "eventKeyboard|2122|0|", "eventKeyboard|2122|1|",
                        "eventKeyboard|2122|0|", "eventKeyboard|2122|1|", "eventKeyboard|2122|0|",
                        "eventKeyboard|2122|1|", "eventKeyboard|2122|0|", "eventKeyboard|2122|1|",
                        "eventKeyboard|2122|0|", "eventKeyboard|2122|1|", "eventKeyboard|2122|0|",
                        "eventKeyboard|2122|1|", "eventKeyboard|2122|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_169_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_169_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_169_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json", "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json",
                    "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json", "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json",
                    "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json", "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json",
                    "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json", "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json",
                    "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json", "keyboard_KEY_NEW_press.json", "keyboard_KEY_NEW_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|181|1", "eventKeyboard|181|0", "eventKeyboard|181|1",
                             "eventKeyboard|181|0", "eventKeyboard|181|1", "eventKeyboard|181|0",
                             "eventKeyboard|181|1", "eventKeyboard|181|0", "eventKeyboard|181|1",
                             "eventKeyboard|181|0", "eventKeyboard|181|1", "eventKeyboard|181|0",
                             "eventKeyboard|181|1", "eventKeyboard|181|0", "eventKeyboard|181|1",
                             "eventKeyboard|181|0", "eventKeyboard|181|1", "eventKeyboard|181|0",
                             "eventKeyboard|181|1", "eventKeyboard|181|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2640|1|", "eventKeyboard|2640|0|", "eventKeyboard|2640|1|",
                        "eventKeyboard|2640|0|", "eventKeyboard|2640|1|", "eventKeyboard|2640|0|",
                        "eventKeyboard|2640|1|", "eventKeyboard|2640|0|", "eventKeyboard|2640|1|",
                        "eventKeyboard|2640|0|", "eventKeyboard|2640|1|", "eventKeyboard|2640|0|",
                        "eventKeyboard|2640|1|", "eventKeyboard|2640|0|", "eventKeyboard|2640|1|",
                        "eventKeyboard|2640|0|", "eventKeyboard|2640|1|", "eventKeyboard|2640|0|",
                        "eventKeyboard|2640|1|", "eventKeyboard|2640|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_170_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_170_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_170_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json", "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json",
                    "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json", "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json",
                    "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json", "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json",
                    "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json", "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json",
                    "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json", "keyboard_KEY_REDO_press.json", "keyboard_KEY_REDO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|182|1", "eventKeyboard|182|0", "eventKeyboard|182|1",
                             "eventKeyboard|182|0", "eventKeyboard|182|1", "eventKeyboard|182|0",
                             "eventKeyboard|182|1", "eventKeyboard|182|0", "eventKeyboard|182|1",
                             "eventKeyboard|182|0", "eventKeyboard|182|1", "eventKeyboard|182|0",
                             "eventKeyboard|182|1", "eventKeyboard|182|0", "eventKeyboard|182|1",
                             "eventKeyboard|182|0", "eventKeyboard|182|1", "eventKeyboard|182|0",
                             "eventKeyboard|182|1", "eventKeyboard|182|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2641|1|", "eventKeyboard|2641|0|", "eventKeyboard|2641|1|",
                        "eventKeyboard|2641|0|", "eventKeyboard|2641|1|", "eventKeyboard|2641|0|",
                        "eventKeyboard|2641|1|", "eventKeyboard|2641|0|", "eventKeyboard|2641|1|",
                        "eventKeyboard|2641|0|", "eventKeyboard|2641|1|", "eventKeyboard|2641|0|",
                        "eventKeyboard|2641|1|", "eventKeyboard|2641|0|", "eventKeyboard|2641|1|",
                        "eventKeyboard|2641|0|", "eventKeyboard|2641|1|", "eventKeyboard|2641|0|",
                        "eventKeyboard|2641|1|", "eventKeyboard|2641|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_173_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_173_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_173_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json", "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json",
                    "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json", "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json",
                    "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json", "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json",
                    "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json", "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json",
                    "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json", "keyboard_KEY_F15_press.json", "keyboard_KEY_F15_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|185|1", "eventKeyboard|185|0", "eventKeyboard|185|1",
                             "eventKeyboard|185|0", "eventKeyboard|185|1", "eventKeyboard|185|0",
                             "eventKeyboard|185|1", "eventKeyboard|185|0", "eventKeyboard|185|1",
                             "eventKeyboard|185|0", "eventKeyboard|185|1", "eventKeyboard|185|0",
                             "eventKeyboard|185|1", "eventKeyboard|185|0", "eventKeyboard|185|1",
                             "eventKeyboard|185|0", "eventKeyboard|185|1", "eventKeyboard|185|0",
                             "eventKeyboard|185|1", "eventKeyboard|185|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2818|1|", "eventKeyboard|2818|0|", "eventKeyboard|2818|1|",
                        "eventKeyboard|2818|0|", "eventKeyboard|2818|1|", "eventKeyboard|2818|0|",
                        "eventKeyboard|2818|1|", "eventKeyboard|2818|0|", "eventKeyboard|2818|1|",
                        "eventKeyboard|2818|0|", "eventKeyboard|2818|1|", "eventKeyboard|2818|0|",
                        "eventKeyboard|2818|1|", "eventKeyboard|2818|0|", "eventKeyboard|2818|1|",
                        "eventKeyboard|2818|0|", "eventKeyboard|2818|1|", "eventKeyboard|2818|0|",
                        "eventKeyboard|2818|1|", "eventKeyboard|2818|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_174_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_174_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_174_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json", "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json",
                    "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json", "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json",
                    "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json", "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json",
                    "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json", "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json",
                    "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json", "keyboard_KEY_F16_press.json", "keyboard_KEY_F16_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|186|1", "eventKeyboard|186|0", "eventKeyboard|186|1",
                             "eventKeyboard|186|0", "eventKeyboard|186|1", "eventKeyboard|186|0",
                             "eventKeyboard|186|1", "eventKeyboard|186|0", "eventKeyboard|186|1",
                             "eventKeyboard|186|0", "eventKeyboard|186|1", "eventKeyboard|186|0",
                             "eventKeyboard|186|1", "eventKeyboard|186|0", "eventKeyboard|186|1",
                             "eventKeyboard|186|0", "eventKeyboard|186|1", "eventKeyboard|186|0",
                             "eventKeyboard|186|1", "eventKeyboard|186|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2819|1|", "eventKeyboard|2819|0|", "eventKeyboard|2819|1|",
                        "eventKeyboard|2819|0|", "eventKeyboard|2819|1|", "eventKeyboard|2819|0|",
                        "eventKeyboard|2819|1|", "eventKeyboard|2819|0|", "eventKeyboard|2819|1|",
                        "eventKeyboard|2819|0|", "eventKeyboard|2819|1|", "eventKeyboard|2819|0|",
                        "eventKeyboard|2819|1|", "eventKeyboard|2819|0|", "eventKeyboard|2819|1|",
                        "eventKeyboard|2819|0|", "eventKeyboard|2819|1|", "eventKeyboard|2819|0|",
                        "eventKeyboard|2819|1|", "eventKeyboard|2819|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_175_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_175_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_175_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json", "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json",
                    "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json", "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json",
                    "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json", "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json",
                    "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json", "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json",
                    "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json", "keyboard_KEY_F17_press.json", "keyboard_KEY_F17_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|187|1", "eventKeyboard|187|0", "eventKeyboard|187|1",
                             "eventKeyboard|187|0", "eventKeyboard|187|1", "eventKeyboard|187|0",
                             "eventKeyboard|187|1", "eventKeyboard|187|0", "eventKeyboard|187|1",
                             "eventKeyboard|187|0", "eventKeyboard|187|1", "eventKeyboard|187|0",
                             "eventKeyboard|187|1", "eventKeyboard|187|0", "eventKeyboard|187|1",
                             "eventKeyboard|187|0", "eventKeyboard|187|1", "eventKeyboard|187|0",
                             "eventKeyboard|187|1", "eventKeyboard|187|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2820|1|", "eventKeyboard|2820|0|", "eventKeyboard|2820|1|",
                        "eventKeyboard|2820|0|", "eventKeyboard|2820|1|", "eventKeyboard|2820|0|",
                        "eventKeyboard|2820|1|", "eventKeyboard|2820|0|", "eventKeyboard|2820|1|",
                        "eventKeyboard|2820|0|", "eventKeyboard|2820|1|", "eventKeyboard|2820|0|",
                        "eventKeyboard|2820|1|", "eventKeyboard|2820|0|", "eventKeyboard|2820|1|",
                        "eventKeyboard|2820|0|", "eventKeyboard|2820|1|", "eventKeyboard|2820|0|",
                        "eventKeyboard|2820|1|", "eventKeyboard|2820|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_176_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_176_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_176_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json", "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json",
                    "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json", "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json",
                    "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json", "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json",
                    "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json", "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json",
                    "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json", "keyboard_KEY_F18_press.json", "keyboard_KEY_F18_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|188|1", "eventKeyboard|188|0", "eventKeyboard|188|1",
                             "eventKeyboard|188|0", "eventKeyboard|188|1", "eventKeyboard|188|0",
                             "eventKeyboard|188|1", "eventKeyboard|188|0", "eventKeyboard|188|1",
                             "eventKeyboard|188|0", "eventKeyboard|188|1", "eventKeyboard|188|0",
                             "eventKeyboard|188|1", "eventKeyboard|188|0", "eventKeyboard|188|1",
                             "eventKeyboard|188|0", "eventKeyboard|188|1", "eventKeyboard|188|0",
                             "eventKeyboard|188|1", "eventKeyboard|188|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2821|1|", "eventKeyboard|2821|0|", "eventKeyboard|2821|1|",
                        "eventKeyboard|2821|0|", "eventKeyboard|2821|1|", "eventKeyboard|2821|0|",
                        "eventKeyboard|2821|1|", "eventKeyboard|2821|0|", "eventKeyboard|2821|1|",
                        "eventKeyboard|2821|0|", "eventKeyboard|2821|1|", "eventKeyboard|2821|0|",
                        "eventKeyboard|2821|1|", "eventKeyboard|2821|0|", "eventKeyboard|2821|1|",
                        "eventKeyboard|2821|0|", "eventKeyboard|2821|1|", "eventKeyboard|2821|0|",
                        "eventKeyboard|2821|1|", "eventKeyboard|2821|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_177_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_177_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_177_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json", "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json",
                    "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json", "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json",
                    "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json", "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json",
                    "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json", "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json",
                    "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json", "keyboard_KEY_F19_press.json", "keyboard_KEY_F19_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|189|1", "eventKeyboard|189|0", "eventKeyboard|189|1",
                             "eventKeyboard|189|0", "eventKeyboard|189|1", "eventKeyboard|189|0",
                             "eventKeyboard|189|1", "eventKeyboard|189|0", "eventKeyboard|189|1",
                             "eventKeyboard|189|0", "eventKeyboard|189|1", "eventKeyboard|189|0",
                             "eventKeyboard|189|1", "eventKeyboard|189|0", "eventKeyboard|189|1",
                             "eventKeyboard|189|0", "eventKeyboard|189|1", "eventKeyboard|189|0",
                             "eventKeyboard|189|1", "eventKeyboard|189|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2822|1|", "eventKeyboard|2822|0|", "eventKeyboard|2822|1|",
                        "eventKeyboard|2822|0|", "eventKeyboard|2822|1|", "eventKeyboard|2822|0|",
                        "eventKeyboard|2822|1|", "eventKeyboard|2822|0|", "eventKeyboard|2822|1|",
                        "eventKeyboard|2822|0|", "eventKeyboard|2822|1|", "eventKeyboard|2822|0|",
                        "eventKeyboard|2822|1|", "eventKeyboard|2822|0|", "eventKeyboard|2822|1|",
                        "eventKeyboard|2822|0|", "eventKeyboard|2822|1|", "eventKeyboard|2822|0|",
                        "eventKeyboard|2822|1|", "eventKeyboard|2822|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_178_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_178_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_178_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json", "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json",
                    "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json", "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json",
                    "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json", "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json",
                    "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json", "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json",
                    "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json", "keyboard_KEY_F20_press.json", "keyboard_KEY_F20_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|190|1", "eventKeyboard|190|0", "eventKeyboard|190|1",
                             "eventKeyboard|190|0", "eventKeyboard|190|1", "eventKeyboard|190|0",
                             "eventKeyboard|190|1", "eventKeyboard|190|0", "eventKeyboard|190|1",
                             "eventKeyboard|190|0", "eventKeyboard|190|1", "eventKeyboard|190|0",
                             "eventKeyboard|190|1", "eventKeyboard|190|0", "eventKeyboard|190|1",
                             "eventKeyboard|190|0", "eventKeyboard|190|1", "eventKeyboard|190|0",
                             "eventKeyboard|190|1", "eventKeyboard|190|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2823|1|", "eventKeyboard|2823|0|", "eventKeyboard|2823|1|",
                        "eventKeyboard|2823|0|", "eventKeyboard|2823|1|", "eventKeyboard|2823|0|",
                        "eventKeyboard|2823|1|", "eventKeyboard|2823|0|", "eventKeyboard|2823|1|",
                        "eventKeyboard|2823|0|", "eventKeyboard|2823|1|", "eventKeyboard|2823|0|",
                        "eventKeyboard|2823|1|", "eventKeyboard|2823|0|", "eventKeyboard|2823|1|",
                        "eventKeyboard|2823|0|", "eventKeyboard|2823|1|", "eventKeyboard|2823|0|",
                        "eventKeyboard|2823|1|", "eventKeyboard|2823|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_179_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_179_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_179_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json", "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json",
                    "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json", "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json",
                    "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json", "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json",
                    "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json", "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json",
                    "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json", "keyboard_KEY_F21_press.json", "keyboard_KEY_F21_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|191|1", "eventKeyboard|191|0", "eventKeyboard|191|1",
                             "eventKeyboard|191|0", "eventKeyboard|191|1", "eventKeyboard|191|0",
                             "eventKeyboard|191|1", "eventKeyboard|191|0", "eventKeyboard|191|1",
                             "eventKeyboard|191|0", "eventKeyboard|191|1", "eventKeyboard|191|0",
                             "eventKeyboard|191|1", "eventKeyboard|191|0", "eventKeyboard|191|1",
                             "eventKeyboard|191|0", "eventKeyboard|191|1", "eventKeyboard|191|0",
                             "eventKeyboard|191|1", "eventKeyboard|191|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2824|1|", "eventKeyboard|2824|0|", "eventKeyboard|2824|1|",
                        "eventKeyboard|2824|0|", "eventKeyboard|2824|1|", "eventKeyboard|2824|0|",
                        "eventKeyboard|2824|1|", "eventKeyboard|2824|0|", "eventKeyboard|2824|1|",
                        "eventKeyboard|2824|0|", "eventKeyboard|2824|1|", "eventKeyboard|2824|0|",
                        "eventKeyboard|2824|1|", "eventKeyboard|2824|0|", "eventKeyboard|2824|1|",
                        "eventKeyboard|2824|0|", "eventKeyboard|2824|1|", "eventKeyboard|2824|0|",
                        "eventKeyboard|2824|1|", "eventKeyboard|2824|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_180_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_180_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_180_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json", "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json",
                    "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json", "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json",
                    "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json", "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json",
                    "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json", "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json",
                    "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json", "keyboard_KEY_F22_press.json", "keyboard_KEY_F22_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|192|1", "eventKeyboard|192|0", "eventKeyboard|192|1",
                             "eventKeyboard|192|0", "eventKeyboard|192|1", "eventKeyboard|192|0",
                             "eventKeyboard|192|1", "eventKeyboard|192|0", "eventKeyboard|192|1",
                             "eventKeyboard|192|0", "eventKeyboard|192|1", "eventKeyboard|192|0",
                             "eventKeyboard|192|1", "eventKeyboard|192|0", "eventKeyboard|192|1",
                             "eventKeyboard|192|0", "eventKeyboard|192|1", "eventKeyboard|192|0",
                             "eventKeyboard|192|1", "eventKeyboard|192|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2825|1|", "eventKeyboard|2825|0|", "eventKeyboard|2825|1|",
                        "eventKeyboard|2825|0|", "eventKeyboard|2825|1|", "eventKeyboard|2825|0|",
                        "eventKeyboard|2825|1|", "eventKeyboard|2825|0|", "eventKeyboard|2825|1|",
                        "eventKeyboard|2825|0|", "eventKeyboard|2825|1|", "eventKeyboard|2825|0|",
                        "eventKeyboard|2825|1|", "eventKeyboard|2825|0|", "eventKeyboard|2825|1|",
                        "eventKeyboard|2825|0|", "eventKeyboard|2825|1|", "eventKeyboard|2825|0|",
                        "eventKeyboard|2825|1|", "eventKeyboard|2825|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_181_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_181_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_181_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json", "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json",
                    "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json", "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json",
                    "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json", "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json",
                    "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json", "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json",
                    "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json", "keyboard_KEY_F23_press.json", "keyboard_KEY_F23_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|193|1", "eventKeyboard|193|0", "eventKeyboard|193|1",
                             "eventKeyboard|193|0", "eventKeyboard|193|1", "eventKeyboard|193|0",
                             "eventKeyboard|193|1", "eventKeyboard|193|0", "eventKeyboard|193|1",
                             "eventKeyboard|193|0", "eventKeyboard|193|1", "eventKeyboard|193|0",
                             "eventKeyboard|193|1", "eventKeyboard|193|0", "eventKeyboard|193|1",
                             "eventKeyboard|193|0", "eventKeyboard|193|1", "eventKeyboard|193|0",
                             "eventKeyboard|193|1", "eventKeyboard|193|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2826|1|", "eventKeyboard|2826|0|", "eventKeyboard|2826|1|",
                        "eventKeyboard|2826|0|", "eventKeyboard|2826|1|", "eventKeyboard|2826|0|",
                        "eventKeyboard|2826|1|", "eventKeyboard|2826|0|", "eventKeyboard|2826|1|",
                        "eventKeyboard|2826|0|", "eventKeyboard|2826|1|", "eventKeyboard|2826|0|",
                        "eventKeyboard|2826|1|", "eventKeyboard|2826|0|", "eventKeyboard|2826|1|",
                        "eventKeyboard|2826|0|", "eventKeyboard|2826|1|", "eventKeyboard|2826|0|",
                        "eventKeyboard|2826|1|", "eventKeyboard|2826|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_182_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_182_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_182_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json", "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json",
                    "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json", "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json",
                    "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json", "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json",
                    "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json", "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json",
                    "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json", "keyboard_KEY_F24_press.json", "keyboard_KEY_F24_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|194|1", "eventKeyboard|194|0", "eventKeyboard|194|1",
                             "eventKeyboard|194|0", "eventKeyboard|194|1", "eventKeyboard|194|0",
                             "eventKeyboard|194|1", "eventKeyboard|194|0", "eventKeyboard|194|1",
                             "eventKeyboard|194|0", "eventKeyboard|194|1", "eventKeyboard|194|0",
                             "eventKeyboard|194|1", "eventKeyboard|194|0", "eventKeyboard|194|1",
                             "eventKeyboard|194|0", "eventKeyboard|194|1", "eventKeyboard|194|0",
                             "eventKeyboard|194|1", "eventKeyboard|194|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2827|1|", "eventKeyboard|2827|0|", "eventKeyboard|2827|1|",
                        "eventKeyboard|2827|0|", "eventKeyboard|2827|1|", "eventKeyboard|2827|0|",
                        "eventKeyboard|2827|1|", "eventKeyboard|2827|0|", "eventKeyboard|2827|1|",
                        "eventKeyboard|2827|0|", "eventKeyboard|2827|1|", "eventKeyboard|2827|0|",
                        "eventKeyboard|2827|1|", "eventKeyboard|2827|0|", "eventKeyboard|2827|1|",
                        "eventKeyboard|2827|0|", "eventKeyboard|2827|1|", "eventKeyboard|2827|0|",
                        "eventKeyboard|2827|1|", "eventKeyboard|2827|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_183_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_183_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_183_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json", "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json",
                    "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json", "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json",
                    "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json", "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json",
                    "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json", "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json",
                    "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json", "keyboard_KEY_PLAYCD_press.json", "keyboard_KEY_PLAYCD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|200|1", "eventKeyboard|200|0", "eventKeyboard|200|1",
                             "eventKeyboard|200|0", "eventKeyboard|200|1", "eventKeyboard|200|0",
                             "eventKeyboard|200|1", "eventKeyboard|200|0", "eventKeyboard|200|1",
                             "eventKeyboard|200|0", "eventKeyboard|200|1", "eventKeyboard|200|0",
                             "eventKeyboard|200|1", "eventKeyboard|200|0", "eventKeyboard|200|1",
                             "eventKeyboard|200|0", "eventKeyboard|200|1", "eventKeyboard|200|0",
                             "eventKeyboard|200|1", "eventKeyboard|200|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2643|1|", "eventKeyboard|2643|0|", "eventKeyboard|2643|1|",
                        "eventKeyboard|2643|0|", "eventKeyboard|2643|1|", "eventKeyboard|2643|0|",
                        "eventKeyboard|2643|1|", "eventKeyboard|2643|0|", "eventKeyboard|2643|1|",
                        "eventKeyboard|2643|0|", "eventKeyboard|2643|1|", "eventKeyboard|2643|0|",
                        "eventKeyboard|2643|1|", "eventKeyboard|2643|0|", "eventKeyboard|2643|1|",
                        "eventKeyboard|2643|0|", "eventKeyboard|2643|1|", "eventKeyboard|2643|0|",
                        "eventKeyboard|2643|1|", "eventKeyboard|2643|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_184_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_184_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_184_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json", "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json",
                    "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json", "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json",
                    "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json", "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json",
                    "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json", "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json",
                    "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json", "keyboard_KEY_PAUSECD_press.json", "keyboard_KEY_PAUSECD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|201|1", "eventKeyboard|201|0", "eventKeyboard|201|1",
                             "eventKeyboard|201|0", "eventKeyboard|201|1", "eventKeyboard|201|0",
                             "eventKeyboard|201|1", "eventKeyboard|201|0", "eventKeyboard|201|1",
                             "eventKeyboard|201|0", "eventKeyboard|201|1", "eventKeyboard|201|0",
                             "eventKeyboard|201|1", "eventKeyboard|201|0", "eventKeyboard|201|1",
                             "eventKeyboard|201|0", "eventKeyboard|201|1", "eventKeyboard|201|0",
                             "eventKeyboard|201|1", "eventKeyboard|201|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2086|1|", "eventKeyboard|2086|0|", "eventKeyboard|2086|1|",
                        "eventKeyboard|2086|0|", "eventKeyboard|2086|1|", "eventKeyboard|2086|0|",
                        "eventKeyboard|2086|1|", "eventKeyboard|2086|0|", "eventKeyboard|2086|1|",
                        "eventKeyboard|2086|0|", "eventKeyboard|2086|1|", "eventKeyboard|2086|0|",
                        "eventKeyboard|2086|1|", "eventKeyboard|2086|0|", "eventKeyboard|2086|1|",
                        "eventKeyboard|2086|0|", "eventKeyboard|2086|1|", "eventKeyboard|2086|0|",
                        "eventKeyboard|2086|1|", "eventKeyboard|2086|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_185_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_185_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_185_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json", "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json",
                    "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json", "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json",
                    "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json", "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json",
                    "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json", "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json",
                    "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json", "keyboard_KEY_PROG3_press.json", "keyboard_KEY_PROG3_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|202|1", "eventKeyboard|202|0", "eventKeyboard|202|1",
                             "eventKeyboard|202|0", "eventKeyboard|202|1", "eventKeyboard|202|0",
                             "eventKeyboard|202|1", "eventKeyboard|202|0", "eventKeyboard|202|1",
                             "eventKeyboard|202|0", "eventKeyboard|202|1", "eventKeyboard|202|0",
                             "eventKeyboard|202|1", "eventKeyboard|202|0", "eventKeyboard|202|1",
                             "eventKeyboard|202|0", "eventKeyboard|202|1", "eventKeyboard|202|0",
                             "eventKeyboard|202|1", "eventKeyboard|202|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2828|1|", "eventKeyboard|2828|0|", "eventKeyboard|2828|1|",
                        "eventKeyboard|2828|0|", "eventKeyboard|2828|1|", "eventKeyboard|2828|0|",
                        "eventKeyboard|2828|1|", "eventKeyboard|2828|0|", "eventKeyboard|2828|1|",
                        "eventKeyboard|2828|0|", "eventKeyboard|2828|1|", "eventKeyboard|2828|0|",
                        "eventKeyboard|2828|1|", "eventKeyboard|2828|0|", "eventKeyboard|2828|1|",
                        "eventKeyboard|2828|0|", "eventKeyboard|2828|1|", "eventKeyboard|2828|0|",
                        "eventKeyboard|2828|1|", "eventKeyboard|2828|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_186_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_186_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_186_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json", "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json",
                    "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json", "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json",
                    "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json", "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json",
                    "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json", "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json",
                    "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json", "keyboard_KEY_PROG4_press.json", "keyboard_KEY_PROG4_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|203|1", "eventKeyboard|203|0", "eventKeyboard|203|1",
                             "eventKeyboard|203|0", "eventKeyboard|203|1", "eventKeyboard|203|0",
                             "eventKeyboard|203|1", "eventKeyboard|203|0", "eventKeyboard|203|1",
                             "eventKeyboard|203|0", "eventKeyboard|203|1", "eventKeyboard|203|0",
                             "eventKeyboard|203|1", "eventKeyboard|203|0", "eventKeyboard|203|1",
                             "eventKeyboard|203|0", "eventKeyboard|203|1", "eventKeyboard|203|0",
                             "eventKeyboard|203|1", "eventKeyboard|203|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2829|1|", "eventKeyboard|2829|0|", "eventKeyboard|2829|1|",
                        "eventKeyboard|2829|0|", "eventKeyboard|2829|1|", "eventKeyboard|2829|0|",
                        "eventKeyboard|2829|1|", "eventKeyboard|2829|0|", "eventKeyboard|2829|1|",
                        "eventKeyboard|2829|0|", "eventKeyboard|2829|1|", "eventKeyboard|2829|0|",
                        "eventKeyboard|2829|1|", "eventKeyboard|2829|0|", "eventKeyboard|2829|1|",
                        "eventKeyboard|2829|0|", "eventKeyboard|2829|1|", "eventKeyboard|2829|0|",
                        "eventKeyboard|2829|1|", "eventKeyboard|2829|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_187_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_187_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_187_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json", "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json",
                    "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json", "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json",
                    "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json", "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json",
                    "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json", "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json",
                    "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json", "keyboard_KEY_DASHBOARD_press.json", "keyboard_KEY_DASHBOARD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|204|1", "eventKeyboard|204|0", "eventKeyboard|204|1",
                             "eventKeyboard|204|0", "eventKeyboard|204|1", "eventKeyboard|204|0",
                             "eventKeyboard|204|1", "eventKeyboard|204|0", "eventKeyboard|204|1",
                             "eventKeyboard|204|0", "eventKeyboard|204|1", "eventKeyboard|204|0",
                             "eventKeyboard|204|1", "eventKeyboard|204|0", "eventKeyboard|204|1",
                             "eventKeyboard|204|0", "eventKeyboard|204|1", "eventKeyboard|204|0",
                             "eventKeyboard|204|1", "eventKeyboard|204|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2830|1|", "eventKeyboard|2830|0|", "eventKeyboard|2830|1|",
                        "eventKeyboard|2830|0|", "eventKeyboard|2830|1|", "eventKeyboard|2830|0|",
                        "eventKeyboard|2830|1|", "eventKeyboard|2830|0|", "eventKeyboard|2830|1|",
                        "eventKeyboard|2830|0|", "eventKeyboard|2830|1|", "eventKeyboard|2830|0|",
                        "eventKeyboard|2830|1|", "eventKeyboard|2830|0|", "eventKeyboard|2830|1|",
                        "eventKeyboard|2830|0|", "eventKeyboard|2830|1|", "eventKeyboard|2830|0|",
                        "eventKeyboard|2830|1|", "eventKeyboard|2830|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_188_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_188_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_188_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json", "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json",
                    "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json", "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json",
                    "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json", "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json",
                    "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json", "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json",
                    "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json", "keyboard_KEY_SUSPEND_press.json", "keyboard_KEY_SUSPEND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|205|1", "eventKeyboard|205|0", "eventKeyboard|205|1",
                             "eventKeyboard|205|0", "eventKeyboard|205|1", "eventKeyboard|205|0",
                             "eventKeyboard|205|1", "eventKeyboard|205|0", "eventKeyboard|205|1",
                             "eventKeyboard|205|0", "eventKeyboard|205|1", "eventKeyboard|205|0",
                             "eventKeyboard|205|1", "eventKeyboard|205|0", "eventKeyboard|205|1",
                             "eventKeyboard|205|0", "eventKeyboard|205|1", "eventKeyboard|205|0",
                             "eventKeyboard|205|1", "eventKeyboard|205|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2831|1|", "eventKeyboard|2831|0|", "eventKeyboard|2831|1|",
                        "eventKeyboard|2831|0|", "eventKeyboard|2831|1|", "eventKeyboard|2831|0|",
                        "eventKeyboard|2831|1|", "eventKeyboard|2831|0|", "eventKeyboard|2831|1|",
                        "eventKeyboard|2831|0|", "eventKeyboard|2831|1|", "eventKeyboard|2831|0|",
                        "eventKeyboard|2831|1|", "eventKeyboard|2831|0|", "eventKeyboard|2831|1|",
                        "eventKeyboard|2831|0|", "eventKeyboard|2831|1|", "eventKeyboard|2831|0|",
                        "eventKeyboard|2831|1|", "eventKeyboard|2831|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_189_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_189_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_189_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json", "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json",
                    "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json", "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json",
                    "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json", "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json",
                    "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json", "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json",
                    "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json", "keyboard_KEY_CLOSE_press.json", "keyboard_KEY_CLOSE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|206|1", "eventKeyboard|206|0", "eventKeyboard|206|1",
                             "eventKeyboard|206|0", "eventKeyboard|206|1", "eventKeyboard|206|0",
                             "eventKeyboard|206|1", "eventKeyboard|206|0", "eventKeyboard|206|1",
                             "eventKeyboard|206|0", "eventKeyboard|206|1", "eventKeyboard|206|0",
                             "eventKeyboard|206|1", "eventKeyboard|206|0", "eventKeyboard|206|1",
                             "eventKeyboard|206|0", "eventKeyboard|206|1", "eventKeyboard|206|0",
                             "eventKeyboard|206|1", "eventKeyboard|206|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2642|1|", "eventKeyboard|2642|0|", "eventKeyboard|2642|1|",
                        "eventKeyboard|2642|0|", "eventKeyboard|2642|1|", "eventKeyboard|2642|0|",
                        "eventKeyboard|2642|1|", "eventKeyboard|2642|0|", "eventKeyboard|2642|1|",
                        "eventKeyboard|2642|0|", "eventKeyboard|2642|1|", "eventKeyboard|2642|0|",
                        "eventKeyboard|2642|1|", "eventKeyboard|2642|0|", "eventKeyboard|2642|1|",
                        "eventKeyboard|2642|0|", "eventKeyboard|2642|1|", "eventKeyboard|2642|0|",
                        "eventKeyboard|2642|1|", "eventKeyboard|2642|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_190_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_190_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_190_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json", "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json",
                    "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json", "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json",
                    "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json", "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json",
                    "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json", "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json",
                    "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json", "keyboard_KEY_PLAY_press.json", "keyboard_KEY_PLAY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|207|1", "eventKeyboard|207|0", "eventKeyboard|207|1",
                             "eventKeyboard|207|0", "eventKeyboard|207|1", "eventKeyboard|207|0",
                             "eventKeyboard|207|1", "eventKeyboard|207|0", "eventKeyboard|207|1",
                             "eventKeyboard|207|0", "eventKeyboard|207|1", "eventKeyboard|207|0",
                             "eventKeyboard|207|1", "eventKeyboard|207|0", "eventKeyboard|207|1",
                             "eventKeyboard|207|0", "eventKeyboard|207|1", "eventKeyboard|207|0",
                             "eventKeyboard|207|1", "eventKeyboard|207|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2085|1|", "eventKeyboard|2085|0|", "eventKeyboard|2085|1|",
                        "eventKeyboard|2085|0|", "eventKeyboard|2085|1|", "eventKeyboard|2085|0|",
                        "eventKeyboard|2085|1|", "eventKeyboard|2085|0|", "eventKeyboard|2085|1|",
                        "eventKeyboard|2085|0|", "eventKeyboard|2085|1|", "eventKeyboard|2085|0|",
                        "eventKeyboard|2085|1|", "eventKeyboard|2085|0|", "eventKeyboard|2085|1|",
                        "eventKeyboard|2085|0|", "eventKeyboard|2085|1|", "eventKeyboard|2085|0|",
                        "eventKeyboard|2085|1|", "eventKeyboard|2085|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_191_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_191_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_191_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json", "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json",
                    "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json", "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json",
                    "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json", "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json",
                    "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json", "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json",
                    "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json", "keyboard_KEY_FASTFORWARD_press.json", "keyboard_KEY_FASTFORWARD_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|208|1", "eventKeyboard|208|0", "eventKeyboard|208|1",
                             "eventKeyboard|208|0", "eventKeyboard|208|1", "eventKeyboard|208|0",
                             "eventKeyboard|208|1", "eventKeyboard|208|0", "eventKeyboard|208|1",
                             "eventKeyboard|208|0", "eventKeyboard|208|1", "eventKeyboard|208|0",
                             "eventKeyboard|208|1", "eventKeyboard|208|0", "eventKeyboard|208|1",
                             "eventKeyboard|208|0", "eventKeyboard|208|1", "eventKeyboard|208|0",
                             "eventKeyboard|208|1", "eventKeyboard|208|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|15|1|", "eventKeyboard|15|0|", "eventKeyboard|15|1|",
                        "eventKeyboard|15|0|", "eventKeyboard|15|1|", "eventKeyboard|15|0|",
                        "eventKeyboard|15|1|", "eventKeyboard|15|0|", "eventKeyboard|15|1|",
                        "eventKeyboard|15|0|", "eventKeyboard|15|1|", "eventKeyboard|15|0|",
                        "eventKeyboard|15|1|", "eventKeyboard|15|0|", "eventKeyboard|15|1|",
                        "eventKeyboard|15|0|", "eventKeyboard|15|1|", "eventKeyboard|15|0|",
                        "eventKeyboard|15|1|", "eventKeyboard|15|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_192_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_192_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_192_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json", "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json",
                    "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json", "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json",
                    "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json", "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json",
                    "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json", "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json",
                    "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json", "keyboard_KEY_BASSBOOST_press.json", "keyboard_KEY_BASSBOOST_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|209|1", "eventKeyboard|209|0", "eventKeyboard|209|1",
                             "eventKeyboard|209|0", "eventKeyboard|209|1", "eventKeyboard|209|0",
                             "eventKeyboard|209|1", "eventKeyboard|209|0", "eventKeyboard|209|1",
                             "eventKeyboard|209|0", "eventKeyboard|209|1", "eventKeyboard|209|0",
                             "eventKeyboard|209|1", "eventKeyboard|209|0", "eventKeyboard|209|1",
                             "eventKeyboard|209|0", "eventKeyboard|209|1", "eventKeyboard|209|0",
                             "eventKeyboard|209|1", "eventKeyboard|209|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2644|1|", "eventKeyboard|2644|0|", "eventKeyboard|2644|1|",
                        "eventKeyboard|2644|0|", "eventKeyboard|2644|1|", "eventKeyboard|2644|0|",
                        "eventKeyboard|2644|1|", "eventKeyboard|2644|0|", "eventKeyboard|2644|1|",
                        "eventKeyboard|2644|0|", "eventKeyboard|2644|1|", "eventKeyboard|2644|0|",
                        "eventKeyboard|2644|1|", "eventKeyboard|2644|0|", "eventKeyboard|2644|1|",
                        "eventKeyboard|2644|0|", "eventKeyboard|2644|1|", "eventKeyboard|2644|0|",
                        "eventKeyboard|2644|1|", "eventKeyboard|2644|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_193_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_193_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_193_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json", "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json",
                    "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json", "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json",
                    "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json", "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json",
                    "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json", "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json",
                    "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json", "keyboard_KEY_PRINT_press.json", "keyboard_KEY_PRINT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|210|1", "eventKeyboard|210|0", "eventKeyboard|210|1",
                             "eventKeyboard|210|0", "eventKeyboard|210|1", "eventKeyboard|210|0",
                             "eventKeyboard|210|1", "eventKeyboard|210|0", "eventKeyboard|210|1",
                             "eventKeyboard|210|0", "eventKeyboard|210|1", "eventKeyboard|210|0",
                             "eventKeyboard|210|1", "eventKeyboard|210|0", "eventKeyboard|210|1",
                             "eventKeyboard|210|0", "eventKeyboard|210|1", "eventKeyboard|210|0",
                             "eventKeyboard|210|1", "eventKeyboard|210|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2645|1|", "eventKeyboard|2645|0|", "eventKeyboard|2645|1|",
                        "eventKeyboard|2645|0|", "eventKeyboard|2645|1|", "eventKeyboard|2645|0|",
                        "eventKeyboard|2645|1|", "eventKeyboard|2645|0|", "eventKeyboard|2645|1|",
                        "eventKeyboard|2645|0|", "eventKeyboard|2645|1|", "eventKeyboard|2645|0|",
                        "eventKeyboard|2645|1|", "eventKeyboard|2645|0|", "eventKeyboard|2645|1|",
                        "eventKeyboard|2645|0|", "eventKeyboard|2645|1|", "eventKeyboard|2645|0|",
                        "eventKeyboard|2645|1|", "eventKeyboard|2645|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_194_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_194_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_194_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json", "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json",
                    "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json", "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json",
                    "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json", "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json",
                    "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json", "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json",
                    "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json", "keyboard_KEY_HP_press.json", "keyboard_KEY_HP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|211|1", "eventKeyboard|211|0", "eventKeyboard|211|1",
                             "eventKeyboard|211|0", "eventKeyboard|211|1", "eventKeyboard|211|0",
                             "eventKeyboard|211|1", "eventKeyboard|211|0", "eventKeyboard|211|1",
                             "eventKeyboard|211|0", "eventKeyboard|211|1", "eventKeyboard|211|0",
                             "eventKeyboard|211|1", "eventKeyboard|211|0", "eventKeyboard|211|1",
                             "eventKeyboard|211|0", "eventKeyboard|211|1", "eventKeyboard|211|0",
                             "eventKeyboard|211|1", "eventKeyboard|211|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2832|1|", "eventKeyboard|2832|0|", "eventKeyboard|2832|1|",
                        "eventKeyboard|2832|0|", "eventKeyboard|2832|1|", "eventKeyboard|2832|0|",
                        "eventKeyboard|2832|1|", "eventKeyboard|2832|0|", "eventKeyboard|2832|1|",
                        "eventKeyboard|2832|0|", "eventKeyboard|2832|1|", "eventKeyboard|2832|0|",
                        "eventKeyboard|2832|1|", "eventKeyboard|2832|0|", "eventKeyboard|2832|1|",
                        "eventKeyboard|2832|0|", "eventKeyboard|2832|1|", "eventKeyboard|2832|0|",
                        "eventKeyboard|2832|1|", "eventKeyboard|2832|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_196_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_196_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_196_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json", "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json",
                    "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json", "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json",
                    "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json", "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json",
                    "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json", "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json",
                    "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json", "keyboard_KEY_SOUND_press.json", "keyboard_KEY_SOUND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|213|1", "eventKeyboard|213|0", "eventKeyboard|213|1",
                             "eventKeyboard|213|0", "eventKeyboard|213|1", "eventKeyboard|213|0",
                             "eventKeyboard|213|1", "eventKeyboard|213|0", "eventKeyboard|213|1",
                             "eventKeyboard|213|0", "eventKeyboard|213|1", "eventKeyboard|213|0",
                             "eventKeyboard|213|1", "eventKeyboard|213|0", "eventKeyboard|213|1",
                             "eventKeyboard|213|0", "eventKeyboard|213|1", "eventKeyboard|213|0",
                             "eventKeyboard|213|1", "eventKeyboard|213|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2833|1|", "eventKeyboard|2833|0|", "eventKeyboard|2833|1|",
                        "eventKeyboard|2833|0|", "eventKeyboard|2833|1|", "eventKeyboard|2833|0|",
                        "eventKeyboard|2833|1|", "eventKeyboard|2833|0|", "eventKeyboard|2833|1|",
                        "eventKeyboard|2833|0|", "eventKeyboard|2833|1|", "eventKeyboard|2833|0|",
                        "eventKeyboard|2833|1|", "eventKeyboard|2833|0|", "eventKeyboard|2833|1|",
                        "eventKeyboard|2833|0|", "eventKeyboard|2833|1|", "eventKeyboard|2833|0|",
                        "eventKeyboard|2833|1|", "eventKeyboard|2833|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_197_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_197_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_197_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json", "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json",
                    "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json", "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json",
                    "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json", "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json",
                    "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json", "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json",
                    "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json", "keyboard_KEY_QUESTION_press.json", "keyboard_KEY_QUESTION_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|214|1", "eventKeyboard|214|0", "eventKeyboard|214|1",
                             "eventKeyboard|214|0", "eventKeyboard|214|1", "eventKeyboard|214|0",
                             "eventKeyboard|214|1", "eventKeyboard|214|0", "eventKeyboard|214|1",
                             "eventKeyboard|214|0", "eventKeyboard|214|1", "eventKeyboard|214|0",
                             "eventKeyboard|214|1", "eventKeyboard|214|0", "eventKeyboard|214|1",
                             "eventKeyboard|214|0", "eventKeyboard|214|1", "eventKeyboard|214|0",
                             "eventKeyboard|214|1", "eventKeyboard|214|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2834|1|", "eventKeyboard|2834|0|", "eventKeyboard|2834|1|",
                        "eventKeyboard|2834|0|", "eventKeyboard|2834|1|", "eventKeyboard|2834|0|",
                        "eventKeyboard|2834|1|", "eventKeyboard|2834|0|", "eventKeyboard|2834|1|",
                        "eventKeyboard|2834|0|", "eventKeyboard|2834|1|", "eventKeyboard|2834|0|",
                        "eventKeyboard|2834|1|", "eventKeyboard|2834|0|", "eventKeyboard|2834|1|",
                        "eventKeyboard|2834|0|", "eventKeyboard|2834|1|", "eventKeyboard|2834|0|",
                        "eventKeyboard|2834|1|", "eventKeyboard|2834|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_198_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_198_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_198_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json", "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json",
                    "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json", "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json",
                    "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json", "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json",
                    "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json", "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json",
                    "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json", "keyboard_KEY_EMAIL_press.json", "keyboard_KEY_EMAIL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|215|1", "eventKeyboard|215|0", "eventKeyboard|215|1",
                             "eventKeyboard|215|0", "eventKeyboard|215|1", "eventKeyboard|215|0",
                             "eventKeyboard|215|1", "eventKeyboard|215|0", "eventKeyboard|215|1",
                             "eventKeyboard|215|0", "eventKeyboard|215|1", "eventKeyboard|215|0",
                             "eventKeyboard|215|1", "eventKeyboard|215|0", "eventKeyboard|215|1",
                             "eventKeyboard|215|0", "eventKeyboard|215|1", "eventKeyboard|215|0",
                             "eventKeyboard|215|1", "eventKeyboard|215|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2065|1|", "eventKeyboard|2065|0|", "eventKeyboard|2065|1|",
                        "eventKeyboard|2065|0|", "eventKeyboard|2065|1|", "eventKeyboard|2065|0|",
                        "eventKeyboard|2065|1|", "eventKeyboard|2065|0|", "eventKeyboard|2065|1|",
                        "eventKeyboard|2065|0|", "eventKeyboard|2065|1|", "eventKeyboard|2065|0|",
                        "eventKeyboard|2065|1|", "eventKeyboard|2065|0|", "eventKeyboard|2065|1|",
                        "eventKeyboard|2065|0|", "eventKeyboard|2065|1|", "eventKeyboard|2065|0|",
                        "eventKeyboard|2065|1|", "eventKeyboard|2065|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_199_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_199_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_199_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json", "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json",
                    "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json", "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json",
                    "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json", "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json",
                    "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json", "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json",
                    "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json", "keyboard_KEY_CHAT_press.json", "keyboard_KEY_CHAT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|216|1", "eventKeyboard|216|0", "eventKeyboard|216|1",
                             "eventKeyboard|216|0", "eventKeyboard|216|1", "eventKeyboard|216|0",
                             "eventKeyboard|216|1", "eventKeyboard|216|0", "eventKeyboard|216|1",
                             "eventKeyboard|216|0", "eventKeyboard|216|1", "eventKeyboard|216|0",
                             "eventKeyboard|216|1", "eventKeyboard|216|0", "eventKeyboard|216|1",
                             "eventKeyboard|216|0", "eventKeyboard|216|1", "eventKeyboard|216|0",
                             "eventKeyboard|216|1", "eventKeyboard|216|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2646|1|", "eventKeyboard|2646|0|", "eventKeyboard|2646|1|",
                        "eventKeyboard|2646|0|", "eventKeyboard|2646|1|", "eventKeyboard|2646|0|",
                        "eventKeyboard|2646|1|", "eventKeyboard|2646|0|", "eventKeyboard|2646|1|",
                        "eventKeyboard|2646|0|", "eventKeyboard|2646|1|", "eventKeyboard|2646|0|",
                        "eventKeyboard|2646|1|", "eventKeyboard|2646|0|", "eventKeyboard|2646|1|",
                        "eventKeyboard|2646|0|", "eventKeyboard|2646|1|", "eventKeyboard|2646|0|",
                        "eventKeyboard|2646|1|", "eventKeyboard|2646|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_200_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_200_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_200_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json", "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json",
                    "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json", "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json",
                    "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json", "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json",
                    "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json", "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json",
                    "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json", "keyboard_KEY_SEARCH_press.json", "keyboard_KEY_SEARCH_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|217|1", "eventKeyboard|217|0", "eventKeyboard|217|1",
                             "eventKeyboard|217|0", "eventKeyboard|217|1", "eventKeyboard|217|0",
                             "eventKeyboard|217|1", "eventKeyboard|217|0", "eventKeyboard|217|1",
                             "eventKeyboard|217|0", "eventKeyboard|217|1", "eventKeyboard|217|0",
                             "eventKeyboard|217|1", "eventKeyboard|217|0", "eventKeyboard|217|1",
                             "eventKeyboard|217|0", "eventKeyboard|217|1", "eventKeyboard|217|0",
                             "eventKeyboard|217|1", "eventKeyboard|217|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|9|1|", "eventKeyboard|9|0|", "eventKeyboard|9|1|",
                        "eventKeyboard|9|0|", "eventKeyboard|9|1|", "eventKeyboard|9|0|",
                        "eventKeyboard|9|1|", "eventKeyboard|9|0|", "eventKeyboard|9|1|",
                        "eventKeyboard|9|0|", "eventKeyboard|9|1|", "eventKeyboard|9|0|",
                        "eventKeyboard|9|1|", "eventKeyboard|9|0|", "eventKeyboard|9|1|",
                        "eventKeyboard|9|0|", "eventKeyboard|9|1|", "eventKeyboard|9|0|",
                        "eventKeyboard|9|1|", "eventKeyboard|9|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_201_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_201_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_201_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json", "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json",
                    "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json", "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json",
                    "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json", "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json",
                    "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json", "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json",
                    "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json", "keyboard_KEY_CONNECT_press.json", "keyboard_KEY_CONNECT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|218|1", "eventKeyboard|218|0", "eventKeyboard|218|1",
                             "eventKeyboard|218|0", "eventKeyboard|218|1", "eventKeyboard|218|0",
                             "eventKeyboard|218|1", "eventKeyboard|218|0", "eventKeyboard|218|1",
                             "eventKeyboard|218|0", "eventKeyboard|218|1", "eventKeyboard|218|0",
                             "eventKeyboard|218|1", "eventKeyboard|218|0", "eventKeyboard|218|1",
                             "eventKeyboard|218|0", "eventKeyboard|218|1", "eventKeyboard|218|0",
                             "eventKeyboard|218|1", "eventKeyboard|218|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2836|1|", "eventKeyboard|2836|0|", "eventKeyboard|2836|1|",
                        "eventKeyboard|2836|0|", "eventKeyboard|2836|1|", "eventKeyboard|2836|0|",
                        "eventKeyboard|2836|1|", "eventKeyboard|2836|0|", "eventKeyboard|2836|1|",
                        "eventKeyboard|2836|0|", "eventKeyboard|2836|1|", "eventKeyboard|2836|0|",
                        "eventKeyboard|2836|1|", "eventKeyboard|2836|0|", "eventKeyboard|2836|1|",
                        "eventKeyboard|2836|0|", "eventKeyboard|2836|1|", "eventKeyboard|2836|0|",
                        "eventKeyboard|2836|1|", "eventKeyboard|2836|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_202_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_202_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_202_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json", "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json",
                    "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json", "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json",
                    "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json", "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json",
                    "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json", "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json",
                    "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json", "keyboard_KEY_FINANCE_press.json", "keyboard_KEY_FINANCE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|219|1", "eventKeyboard|219|0", "eventKeyboard|219|1",
                             "eventKeyboard|219|0", "eventKeyboard|219|1", "eventKeyboard|219|0",
                             "eventKeyboard|219|1", "eventKeyboard|219|0", "eventKeyboard|219|1",
                             "eventKeyboard|219|0", "eventKeyboard|219|1", "eventKeyboard|219|0",
                             "eventKeyboard|219|1", "eventKeyboard|219|0", "eventKeyboard|219|1",
                             "eventKeyboard|219|0", "eventKeyboard|219|1", "eventKeyboard|219|0",
                             "eventKeyboard|219|1", "eventKeyboard|219|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2647|1|", "eventKeyboard|2647|0|", "eventKeyboard|2647|1|",
                        "eventKeyboard|2647|0|", "eventKeyboard|2647|1|", "eventKeyboard|2647|0|",
                        "eventKeyboard|2647|1|", "eventKeyboard|2647|0|", "eventKeyboard|2647|1|",
                        "eventKeyboard|2647|0|", "eventKeyboard|2647|1|", "eventKeyboard|2647|0|",
                        "eventKeyboard|2647|1|", "eventKeyboard|2647|0|", "eventKeyboard|2647|1|",
                        "eventKeyboard|2647|0|", "eventKeyboard|2647|1|", "eventKeyboard|2647|0|",
                        "eventKeyboard|2647|1|", "eventKeyboard|2647|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_203_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_203_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_203_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json", "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json",
                    "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json", "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json",
                    "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json", "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json",
                    "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json", "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json",
                    "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json", "keyboard_KEY_SPORT_press.json", "keyboard_KEY_SPORT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|220|1", "eventKeyboard|220|0", "eventKeyboard|220|1",
                             "eventKeyboard|220|0", "eventKeyboard|220|1", "eventKeyboard|220|0",
                             "eventKeyboard|220|1", "eventKeyboard|220|0", "eventKeyboard|220|1",
                             "eventKeyboard|220|0", "eventKeyboard|220|1", "eventKeyboard|220|0",
                             "eventKeyboard|220|1", "eventKeyboard|220|0", "eventKeyboard|220|1",
                             "eventKeyboard|220|0", "eventKeyboard|220|1", "eventKeyboard|220|0",
                             "eventKeyboard|220|1", "eventKeyboard|220|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2837|1|", "eventKeyboard|2837|0|", "eventKeyboard|2837|1|",
                        "eventKeyboard|2837|0|", "eventKeyboard|2837|1|", "eventKeyboard|2837|0|",
                        "eventKeyboard|2837|1|", "eventKeyboard|2837|0|", "eventKeyboard|2837|1|",
                        "eventKeyboard|2837|0|", "eventKeyboard|2837|1|", "eventKeyboard|2837|0|",
                        "eventKeyboard|2837|1|", "eventKeyboard|2837|0|", "eventKeyboard|2837|1|",
                        "eventKeyboard|2837|0|", "eventKeyboard|2837|1|", "eventKeyboard|2837|0|",
                        "eventKeyboard|2837|1|", "eventKeyboard|2837|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_204_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_204_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_204_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json", "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json",
                    "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json", "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json",
                    "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json", "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json",
                    "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json", "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json",
                    "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json", "keyboard_KEY_SHOP_press.json", "keyboard_KEY_SHOP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|221|1", "eventKeyboard|221|0", "eventKeyboard|221|1",
                             "eventKeyboard|221|0", "eventKeyboard|221|1", "eventKeyboard|221|0",
                             "eventKeyboard|221|1", "eventKeyboard|221|0", "eventKeyboard|221|1",
                             "eventKeyboard|221|0", "eventKeyboard|221|1", "eventKeyboard|221|0",
                             "eventKeyboard|221|1", "eventKeyboard|221|0", "eventKeyboard|221|1",
                             "eventKeyboard|221|0", "eventKeyboard|221|1", "eventKeyboard|221|0",
                             "eventKeyboard|221|1", "eventKeyboard|221|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2838|1|", "eventKeyboard|2838|0|", "eventKeyboard|2838|1|",
                        "eventKeyboard|2838|0|", "eventKeyboard|2838|1|", "eventKeyboard|2838|0|",
                        "eventKeyboard|2838|1|", "eventKeyboard|2838|0|", "eventKeyboard|2838|1|",
                        "eventKeyboard|2838|0|", "eventKeyboard|2838|1|", "eventKeyboard|2838|0|",
                        "eventKeyboard|2838|1|", "eventKeyboard|2838|0|", "eventKeyboard|2838|1|",
                        "eventKeyboard|2838|0|", "eventKeyboard|2838|1|", "eventKeyboard|2838|0|",
                        "eventKeyboard|2838|1|", "eventKeyboard|2838|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_205_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_205_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_205_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json", "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json",
                    "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json", "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json",
                    "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json", "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json",
                    "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json", "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json",
                    "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json", "keyboard_KEY_ALTERASE_press.json", "keyboard_KEY_ALTERASE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|222|1", "eventKeyboard|222|0", "eventKeyboard|222|1",
                             "eventKeyboard|222|0", "eventKeyboard|222|1", "eventKeyboard|222|0",
                             "eventKeyboard|222|1", "eventKeyboard|222|0", "eventKeyboard|222|1",
                             "eventKeyboard|222|0", "eventKeyboard|222|1", "eventKeyboard|222|0",
                             "eventKeyboard|222|1", "eventKeyboard|222|0", "eventKeyboard|222|1",
                             "eventKeyboard|222|0", "eventKeyboard|222|1", "eventKeyboard|222|0",
                             "eventKeyboard|222|1", "eventKeyboard|222|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2839|1|", "eventKeyboard|2839|0|", "eventKeyboard|2839|1|",
                        "eventKeyboard|2839|0|", "eventKeyboard|2839|1|", "eventKeyboard|2839|0|",
                        "eventKeyboard|2839|1|", "eventKeyboard|2839|0|", "eventKeyboard|2839|1|",
                        "eventKeyboard|2839|0|", "eventKeyboard|2839|1|", "eventKeyboard|2839|0|",
                        "eventKeyboard|2839|1|", "eventKeyboard|2839|0|", "eventKeyboard|2839|1|",
                        "eventKeyboard|2839|0|", "eventKeyboard|2839|1|", "eventKeyboard|2839|0|",
                        "eventKeyboard|2839|1|", "eventKeyboard|2839|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_206_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_206_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_206_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json", "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json",
                    "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json", "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json",
                    "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json", "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json",
                    "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json", "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json",
                    "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json", "keyboard_KEY_CANCEL_press.json", "keyboard_KEY_CANCEL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|223|1", "eventKeyboard|223|0", "eventKeyboard|223|1",
                             "eventKeyboard|223|0", "eventKeyboard|223|1", "eventKeyboard|223|0",
                             "eventKeyboard|223|1", "eventKeyboard|223|0", "eventKeyboard|223|1",
                             "eventKeyboard|223|0", "eventKeyboard|223|1", "eventKeyboard|223|0",
                             "eventKeyboard|223|1", "eventKeyboard|223|0", "eventKeyboard|223|1",
                             "eventKeyboard|223|0", "eventKeyboard|223|1", "eventKeyboard|223|0",
                             "eventKeyboard|223|1", "eventKeyboard|223|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2648|1|", "eventKeyboard|2648|0|", "eventKeyboard|2648|1|",
                        "eventKeyboard|2648|0|", "eventKeyboard|2648|1|", "eventKeyboard|2648|0|",
                        "eventKeyboard|2648|1|", "eventKeyboard|2648|0|", "eventKeyboard|2648|1|",
                        "eventKeyboard|2648|0|", "eventKeyboard|2648|1|", "eventKeyboard|2648|0|",
                        "eventKeyboard|2648|1|", "eventKeyboard|2648|0|", "eventKeyboard|2648|1|",
                        "eventKeyboard|2648|0|", "eventKeyboard|2648|1|", "eventKeyboard|2648|0|",
                        "eventKeyboard|2648|1|", "eventKeyboard|2648|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_209_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_209_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_209_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json", "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json",
                    "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json", "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json",
                    "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json", "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json",
                    "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json", "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json",
                    "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json", "keyboard_KEY_MEDIA_press.json", "keyboard_KEY_MEDIA_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|226|1", "eventKeyboard|226|0", "eventKeyboard|226|1",
                             "eventKeyboard|226|0", "eventKeyboard|226|1", "eventKeyboard|226|0",
                             "eventKeyboard|226|1", "eventKeyboard|226|0", "eventKeyboard|226|1",
                             "eventKeyboard|226|0", "eventKeyboard|226|1", "eventKeyboard|226|0",
                             "eventKeyboard|226|1", "eventKeyboard|226|0", "eventKeyboard|226|1",
                             "eventKeyboard|226|0", "eventKeyboard|226|1", "eventKeyboard|226|0",
                             "eventKeyboard|226|1", "eventKeyboard|226|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|6|1|", "eventKeyboard|6|0|", "eventKeyboard|6|1|",
                        "eventKeyboard|6|0|", "eventKeyboard|6|1|", "eventKeyboard|6|0|",
                        "eventKeyboard|6|1|", "eventKeyboard|6|0|", "eventKeyboard|6|1|",
                        "eventKeyboard|6|0|", "eventKeyboard|6|1|", "eventKeyboard|6|0|",
                        "eventKeyboard|6|1|", "eventKeyboard|6|0|", "eventKeyboard|6|1|",
                        "eventKeyboard|6|0|", "eventKeyboard|6|1|", "eventKeyboard|6|0|",
                        "eventKeyboard|6|1|", "eventKeyboard|6|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_210_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_210_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_210_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json", "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json",
                    "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json", "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json",
                    "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json", "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json",
                    "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json", "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json",
                    "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json", "keyboard_KEY_SWITCHVIDEOMODE_press.json", "keyboard_KEY_SWITCHVIDEOMODE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|227|1", "eventKeyboard|227|0", "eventKeyboard|227|1",
                             "eventKeyboard|227|0", "eventKeyboard|227|1", "eventKeyboard|227|0",
                             "eventKeyboard|227|1", "eventKeyboard|227|0", "eventKeyboard|227|1",
                             "eventKeyboard|227|0", "eventKeyboard|227|1", "eventKeyboard|227|0",
                             "eventKeyboard|227|1", "eventKeyboard|227|0", "eventKeyboard|227|1",
                             "eventKeyboard|227|0", "eventKeyboard|227|1", "eventKeyboard|227|0",
                             "eventKeyboard|227|1", "eventKeyboard|227|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2841|1|", "eventKeyboard|2841|0|", "eventKeyboard|2841|1|",
                        "eventKeyboard|2841|0|", "eventKeyboard|2841|1|", "eventKeyboard|2841|0|",
                        "eventKeyboard|2841|1|", "eventKeyboard|2841|0|", "eventKeyboard|2841|1|",
                        "eventKeyboard|2841|0|", "eventKeyboard|2841|1|", "eventKeyboard|2841|0|",
                        "eventKeyboard|2841|1|", "eventKeyboard|2841|0|", "eventKeyboard|2841|1|",
                        "eventKeyboard|2841|0|", "eventKeyboard|2841|1|", "eventKeyboard|2841|0|",
                        "eventKeyboard|2841|1|", "eventKeyboard|2841|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_211_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_211_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_211_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json", "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json",
                    "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json", "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json",
                    "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json", "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json",
                    "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json", "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json",
                    "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json", "keyboard_KEY_KBDILLUMTOGGLE_press.json", "keyboard_KEY_KBDILLUMTOGGLE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|228|1", "eventKeyboard|228|0", "eventKeyboard|228|1",
                             "eventKeyboard|228|0", "eventKeyboard|228|1", "eventKeyboard|228|0",
                             "eventKeyboard|228|1", "eventKeyboard|228|0", "eventKeyboard|228|1",
                             "eventKeyboard|228|0", "eventKeyboard|228|1", "eventKeyboard|228|0",
                             "eventKeyboard|228|1", "eventKeyboard|228|0", "eventKeyboard|228|1",
                             "eventKeyboard|228|0", "eventKeyboard|228|1", "eventKeyboard|228|0",
                             "eventKeyboard|228|1", "eventKeyboard|228|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2649|1|", "eventKeyboard|2649|0|", "eventKeyboard|2649|1|",
                        "eventKeyboard|2649|0|", "eventKeyboard|2649|1|", "eventKeyboard|2649|0|",
                        "eventKeyboard|2649|1|", "eventKeyboard|2649|0|", "eventKeyboard|2649|1|",
                        "eventKeyboard|2649|0|", "eventKeyboard|2649|1|", "eventKeyboard|2649|0|",
                        "eventKeyboard|2649|1|", "eventKeyboard|2649|0|", "eventKeyboard|2649|1|",
                        "eventKeyboard|2649|0|", "eventKeyboard|2649|1|", "eventKeyboard|2649|0|",
                        "eventKeyboard|2649|1|", "eventKeyboard|2649|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_212_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_212_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_212_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json", "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json",
                    "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json", "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json",
                    "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json", "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json",
                    "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json", "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json",
                    "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json", "keyboard_KEY_KBDILLUMDOWN_press.json", "keyboard_KEY_KBDILLUMDOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|229|1", "eventKeyboard|229|0", "eventKeyboard|229|1",
                             "eventKeyboard|229|0", "eventKeyboard|229|1", "eventKeyboard|229|0",
                             "eventKeyboard|229|1", "eventKeyboard|229|0", "eventKeyboard|229|1",
                             "eventKeyboard|229|0", "eventKeyboard|229|1", "eventKeyboard|229|0",
                             "eventKeyboard|229|1", "eventKeyboard|229|0", "eventKeyboard|229|1",
                             "eventKeyboard|229|0", "eventKeyboard|229|1", "eventKeyboard|229|0",
                             "eventKeyboard|229|1", "eventKeyboard|229|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2650|1|", "eventKeyboard|2650|0|", "eventKeyboard|2650|1|",
                        "eventKeyboard|2650|0|", "eventKeyboard|2650|1|", "eventKeyboard|2650|0|",
                        "eventKeyboard|2650|1|", "eventKeyboard|2650|0|", "eventKeyboard|2650|1|",
                        "eventKeyboard|2650|0|", "eventKeyboard|2650|1|", "eventKeyboard|2650|0|",
                        "eventKeyboard|2650|1|", "eventKeyboard|2650|0|", "eventKeyboard|2650|1|",
                        "eventKeyboard|2650|0|", "eventKeyboard|2650|1|", "eventKeyboard|2650|0|",
                        "eventKeyboard|2650|1|", "eventKeyboard|2650|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_213_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_213_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_213_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json", "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json",
                    "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json", "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json",
                    "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json", "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json",
                    "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json", "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json",
                    "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json", "keyboard_KEY_KBDILLUMUP_press.json", "keyboard_KEY_KBDILLUMUP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|230|1", "eventKeyboard|230|0", "eventKeyboard|230|1",
                             "eventKeyboard|230|0", "eventKeyboard|230|1", "eventKeyboard|230|0",
                             "eventKeyboard|230|1", "eventKeyboard|230|0", "eventKeyboard|230|1",
                             "eventKeyboard|230|0", "eventKeyboard|230|1", "eventKeyboard|230|0",
                             "eventKeyboard|230|1", "eventKeyboard|230|0", "eventKeyboard|230|1",
                             "eventKeyboard|230|0", "eventKeyboard|230|1", "eventKeyboard|230|0",
                             "eventKeyboard|230|1", "eventKeyboard|230|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2651|1|", "eventKeyboard|2651|0|", "eventKeyboard|2651|1|",
                        "eventKeyboard|2651|0|", "eventKeyboard|2651|1|", "eventKeyboard|2651|0|",
                        "eventKeyboard|2651|1|", "eventKeyboard|2651|0|", "eventKeyboard|2651|1|",
                        "eventKeyboard|2651|0|", "eventKeyboard|2651|1|", "eventKeyboard|2651|0|",
                        "eventKeyboard|2651|1|", "eventKeyboard|2651|0|", "eventKeyboard|2651|1|",
                        "eventKeyboard|2651|0|", "eventKeyboard|2651|1|", "eventKeyboard|2651|0|",
                        "eventKeyboard|2651|1|", "eventKeyboard|2651|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_214_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_214_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_214_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json", "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json",
                    "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json", "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json",
                    "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json", "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json",
                    "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json", "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json",
                    "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json", "keyboard_KEY_SEND_press.json", "keyboard_KEY_SEND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|231|1", "eventKeyboard|231|0", "eventKeyboard|231|1",
                             "eventKeyboard|231|0", "eventKeyboard|231|1", "eventKeyboard|231|0",
                             "eventKeyboard|231|1", "eventKeyboard|231|0", "eventKeyboard|231|1",
                             "eventKeyboard|231|0", "eventKeyboard|231|1", "eventKeyboard|231|0",
                             "eventKeyboard|231|1", "eventKeyboard|231|0", "eventKeyboard|231|1",
                             "eventKeyboard|231|0", "eventKeyboard|231|1", "eventKeyboard|231|0",
                             "eventKeyboard|231|1", "eventKeyboard|231|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2652|1|", "eventKeyboard|2652|0|", "eventKeyboard|2652|1|",
                        "eventKeyboard|2652|0|", "eventKeyboard|2652|1|", "eventKeyboard|2652|0|",
                        "eventKeyboard|2652|1|", "eventKeyboard|2652|0|", "eventKeyboard|2652|1|",
                        "eventKeyboard|2652|0|", "eventKeyboard|2652|1|", "eventKeyboard|2652|0|",
                        "eventKeyboard|2652|1|", "eventKeyboard|2652|0|", "eventKeyboard|2652|1|",
                        "eventKeyboard|2652|0|", "eventKeyboard|2652|1|", "eventKeyboard|2652|0|",
                        "eventKeyboard|2652|1|", "eventKeyboard|2652|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_215_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_215_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_215_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json", "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json",
                    "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json", "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json",
                    "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json", "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json",
                    "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json", "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json",
                    "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json", "keyboard_KEY_REPLY_press.json", "keyboard_KEY_REPLY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|232|1", "eventKeyboard|232|0", "eventKeyboard|232|1",
                             "eventKeyboard|232|0", "eventKeyboard|232|1", "eventKeyboard|232|0",
                             "eventKeyboard|232|1", "eventKeyboard|232|0", "eventKeyboard|232|1",
                             "eventKeyboard|232|0", "eventKeyboard|232|1", "eventKeyboard|232|0",
                             "eventKeyboard|232|1", "eventKeyboard|232|0", "eventKeyboard|232|1",
                             "eventKeyboard|232|0", "eventKeyboard|232|1", "eventKeyboard|232|0",
                             "eventKeyboard|232|1", "eventKeyboard|232|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2653|1|", "eventKeyboard|2653|0|", "eventKeyboard|2653|1|",
                        "eventKeyboard|2653|0|", "eventKeyboard|2653|1|", "eventKeyboard|2653|0|",
                        "eventKeyboard|2653|1|", "eventKeyboard|2653|0|", "eventKeyboard|2653|1|",
                        "eventKeyboard|2653|0|", "eventKeyboard|2653|1|", "eventKeyboard|2653|0|",
                        "eventKeyboard|2653|1|", "eventKeyboard|2653|0|", "eventKeyboard|2653|1|",
                        "eventKeyboard|2653|0|", "eventKeyboard|2653|1|", "eventKeyboard|2653|0|",
                        "eventKeyboard|2653|1|", "eventKeyboard|2653|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_216_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_216_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_216_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json", "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json",
                    "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json", "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json",
                    "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json", "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json",
                    "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json", "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json",
                    "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json", "keyboard_KEY_FORWARDMAIL_press.json", "keyboard_KEY_FORWARDMAIL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|233|1", "eventKeyboard|233|0", "eventKeyboard|233|1",
                             "eventKeyboard|233|0", "eventKeyboard|233|1", "eventKeyboard|233|0",
                             "eventKeyboard|233|1", "eventKeyboard|233|0", "eventKeyboard|233|1",
                             "eventKeyboard|233|0", "eventKeyboard|233|1", "eventKeyboard|233|0",
                             "eventKeyboard|233|1", "eventKeyboard|233|0", "eventKeyboard|233|1",
                             "eventKeyboard|233|0", "eventKeyboard|233|1", "eventKeyboard|233|0",
                             "eventKeyboard|233|1", "eventKeyboard|233|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2654|1|", "eventKeyboard|2654|0|", "eventKeyboard|2654|1|",
                        "eventKeyboard|2654|0|", "eventKeyboard|2654|1|", "eventKeyboard|2654|0|",
                        "eventKeyboard|2654|1|", "eventKeyboard|2654|0|", "eventKeyboard|2654|1|",
                        "eventKeyboard|2654|0|", "eventKeyboard|2654|1|", "eventKeyboard|2654|0|",
                        "eventKeyboard|2654|1|", "eventKeyboard|2654|0|", "eventKeyboard|2654|1|",
                        "eventKeyboard|2654|0|", "eventKeyboard|2654|1|", "eventKeyboard|2654|0|",
                        "eventKeyboard|2654|1|", "eventKeyboard|2654|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_217_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_217_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_217_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json", "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json",
                    "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json", "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json",
                    "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json", "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json",
                    "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json", "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json",
                    "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json", "keyboard_KEY_SAVE_press.json", "keyboard_KEY_SAVE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|234|1", "eventKeyboard|234|0", "eventKeyboard|234|1",
                             "eventKeyboard|234|0", "eventKeyboard|234|1", "eventKeyboard|234|0",
                             "eventKeyboard|234|1", "eventKeyboard|234|0", "eventKeyboard|234|1",
                             "eventKeyboard|234|0", "eventKeyboard|234|1", "eventKeyboard|234|0",
                             "eventKeyboard|234|1", "eventKeyboard|234|0", "eventKeyboard|234|1",
                             "eventKeyboard|234|0", "eventKeyboard|234|1", "eventKeyboard|234|0",
                             "eventKeyboard|234|1", "eventKeyboard|234|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2655|1|", "eventKeyboard|2655|0|", "eventKeyboard|2655|1|",
                        "eventKeyboard|2655|0|", "eventKeyboard|2655|1|", "eventKeyboard|2655|0|",
                        "eventKeyboard|2655|1|", "eventKeyboard|2655|0|", "eventKeyboard|2655|1|",
                        "eventKeyboard|2655|0|", "eventKeyboard|2655|1|", "eventKeyboard|2655|0|",
                        "eventKeyboard|2655|1|", "eventKeyboard|2655|0|", "eventKeyboard|2655|1|",
                        "eventKeyboard|2655|0|", "eventKeyboard|2655|1|", "eventKeyboard|2655|0|",
                        "eventKeyboard|2655|1|", "eventKeyboard|2655|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_218_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_218_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_218_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json", "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json",
                    "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json", "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json",
                    "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json", "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json",
                    "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json", "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json",
                    "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json", "keyboard_KEY_DOCUMENTS_press.json", "keyboard_KEY_DOCUMENTS_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|235|1", "eventKeyboard|235|0", "eventKeyboard|235|1",
                             "eventKeyboard|235|0", "eventKeyboard|235|1", "eventKeyboard|235|0",
                             "eventKeyboard|235|1", "eventKeyboard|235|0", "eventKeyboard|235|1",
                             "eventKeyboard|235|0", "eventKeyboard|235|1", "eventKeyboard|235|0",
                             "eventKeyboard|235|1", "eventKeyboard|235|0", "eventKeyboard|235|1",
                             "eventKeyboard|235|0", "eventKeyboard|235|1", "eventKeyboard|235|0",
                             "eventKeyboard|235|1", "eventKeyboard|235|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2656|1|", "eventKeyboard|2656|0|", "eventKeyboard|2656|1|",
                        "eventKeyboard|2656|0|", "eventKeyboard|2656|1|", "eventKeyboard|2656|0|",
                        "eventKeyboard|2656|1|", "eventKeyboard|2656|0|", "eventKeyboard|2656|1|",
                        "eventKeyboard|2656|0|", "eventKeyboard|2656|1|", "eventKeyboard|2656|0|",
                        "eventKeyboard|2656|1|", "eventKeyboard|2656|0|", "eventKeyboard|2656|1|",
                        "eventKeyboard|2656|0|", "eventKeyboard|2656|1|", "eventKeyboard|2656|0|",
                        "eventKeyboard|2656|1|", "eventKeyboard|2656|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_219_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_219_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_219_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json", "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json",
                    "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json", "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json",
                    "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json", "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json",
                    "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json", "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json",
                    "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json", "keyboard_KEY_BATTERY_press.json", "keyboard_KEY_BATTERY_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|236|1", "eventKeyboard|236|0", "eventKeyboard|236|1",
                             "eventKeyboard|236|0", "eventKeyboard|236|1", "eventKeyboard|236|0",
                             "eventKeyboard|236|1", "eventKeyboard|236|0", "eventKeyboard|236|1",
                             "eventKeyboard|236|0", "eventKeyboard|236|1", "eventKeyboard|236|0",
                             "eventKeyboard|236|1", "eventKeyboard|236|0", "eventKeyboard|236|1",
                             "eventKeyboard|236|0", "eventKeyboard|236|1", "eventKeyboard|236|0",
                             "eventKeyboard|236|1", "eventKeyboard|236|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2842|1|", "eventKeyboard|2842|0|", "eventKeyboard|2842|1|",
                        "eventKeyboard|2842|0|", "eventKeyboard|2842|1|", "eventKeyboard|2842|0|",
                        "eventKeyboard|2842|1|", "eventKeyboard|2842|0|", "eventKeyboard|2842|1|",
                        "eventKeyboard|2842|0|", "eventKeyboard|2842|1|", "eventKeyboard|2842|0|",
                        "eventKeyboard|2842|1|", "eventKeyboard|2842|0|", "eventKeyboard|2842|1|",
                        "eventKeyboard|2842|0|", "eventKeyboard|2842|1|", "eventKeyboard|2842|0|",
                        "eventKeyboard|2842|1|", "eventKeyboard|2842|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_220_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_220_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_220_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json", "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json",
                    "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json", "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json",
                    "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json", "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json",
                    "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json", "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json",
                    "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json", "keyboard_KEY_BLUETOOTH_press.json", "keyboard_KEY_BLUETOOTH_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|237|1", "eventKeyboard|237|0", "eventKeyboard|237|1",
                             "eventKeyboard|237|0", "eventKeyboard|237|1", "eventKeyboard|237|0",
                             "eventKeyboard|237|1", "eventKeyboard|237|0", "eventKeyboard|237|1",
                             "eventKeyboard|237|0", "eventKeyboard|237|1", "eventKeyboard|237|0",
                             "eventKeyboard|237|1", "eventKeyboard|237|0", "eventKeyboard|237|1",
                             "eventKeyboard|237|0", "eventKeyboard|237|1", "eventKeyboard|237|0",
                             "eventKeyboard|237|1", "eventKeyboard|237|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2843|1|", "eventKeyboard|2843|0|", "eventKeyboard|2843|1|",
                        "eventKeyboard|2843|0|", "eventKeyboard|2843|1|", "eventKeyboard|2843|0|",
                        "eventKeyboard|2843|1|", "eventKeyboard|2843|0|", "eventKeyboard|2843|1|",
                        "eventKeyboard|2843|0|", "eventKeyboard|2843|1|", "eventKeyboard|2843|0|",
                        "eventKeyboard|2843|1|", "eventKeyboard|2843|0|", "eventKeyboard|2843|1|",
                        "eventKeyboard|2843|0|", "eventKeyboard|2843|1|", "eventKeyboard|2843|0|",
                        "eventKeyboard|2843|1|", "eventKeyboard|2843|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_221_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_221_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_221_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json", "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json",
                    "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json", "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json",
                    "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json", "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json",
                    "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json", "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json",
                    "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json", "keyboard_KEY_WLAN_press.json", "keyboard_KEY_WLAN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|238|1", "eventKeyboard|238|0", "eventKeyboard|238|1",
                             "eventKeyboard|238|0", "eventKeyboard|238|1", "eventKeyboard|238|0",
                             "eventKeyboard|238|1", "eventKeyboard|238|0", "eventKeyboard|238|1",
                             "eventKeyboard|238|0", "eventKeyboard|238|1", "eventKeyboard|238|0",
                             "eventKeyboard|238|1", "eventKeyboard|238|0", "eventKeyboard|238|1",
                             "eventKeyboard|238|0", "eventKeyboard|238|1", "eventKeyboard|238|0",
                             "eventKeyboard|238|1", "eventKeyboard|238|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2844|1|", "eventKeyboard|2844|0|", "eventKeyboard|2844|1|",
                        "eventKeyboard|2844|0|", "eventKeyboard|2844|1|", "eventKeyboard|2844|0|",
                        "eventKeyboard|2844|1|", "eventKeyboard|2844|0|", "eventKeyboard|2844|1|",
                        "eventKeyboard|2844|0|", "eventKeyboard|2844|1|", "eventKeyboard|2844|0|",
                        "eventKeyboard|2844|1|", "eventKeyboard|2844|0|", "eventKeyboard|2844|1|",
                        "eventKeyboard|2844|0|", "eventKeyboard|2844|1|", "eventKeyboard|2844|0|",
                        "eventKeyboard|2844|1|", "eventKeyboard|2844|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_222_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_222_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_222_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json", "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json",
                    "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json", "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json",
                    "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json", "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json",
                    "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json", "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json",
                    "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json", "keyboard_KEY_UWB_press.json", "keyboard_KEY_UWB_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|239|1", "eventKeyboard|239|0", "eventKeyboard|239|1",
                             "eventKeyboard|239|0", "eventKeyboard|239|1", "eventKeyboard|239|0",
                             "eventKeyboard|239|1", "eventKeyboard|239|0", "eventKeyboard|239|1",
                             "eventKeyboard|239|0", "eventKeyboard|239|1", "eventKeyboard|239|0",
                             "eventKeyboard|239|1", "eventKeyboard|239|0", "eventKeyboard|239|1",
                             "eventKeyboard|239|0", "eventKeyboard|239|1", "eventKeyboard|239|0",
                             "eventKeyboard|239|1", "eventKeyboard|239|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2845|1|", "eventKeyboard|2845|0|", "eventKeyboard|2845|1|",
                        "eventKeyboard|2845|0|", "eventKeyboard|2845|1|", "eventKeyboard|2845|0|",
                        "eventKeyboard|2845|1|", "eventKeyboard|2845|0|", "eventKeyboard|2845|1|",
                        "eventKeyboard|2845|0|", "eventKeyboard|2845|1|", "eventKeyboard|2845|0|",
                        "eventKeyboard|2845|1|", "eventKeyboard|2845|0|", "eventKeyboard|2845|1|",
                        "eventKeyboard|2845|0|", "eventKeyboard|2845|1|", "eventKeyboard|2845|0|",
                        "eventKeyboard|2845|1|", "eventKeyboard|2845|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_223_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_223_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_223_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json", "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json",
                    "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json", "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json",
                    "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json", "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json",
                    "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json", "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json",
                    "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json", "keyboard_KEY_UNKNOWN_press.json", "keyboard_KEY_UNKNOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|240|1", "eventKeyboard|240|0", "eventKeyboard|240|1",
                             "eventKeyboard|240|0", "eventKeyboard|240|1", "eventKeyboard|240|0",
                             "eventKeyboard|240|1", "eventKeyboard|240|0", "eventKeyboard|240|1",
                             "eventKeyboard|240|0", "eventKeyboard|240|1", "eventKeyboard|240|0",
                             "eventKeyboard|240|1", "eventKeyboard|240|0", "eventKeyboard|240|1",
                             "eventKeyboard|240|0", "eventKeyboard|240|1", "eventKeyboard|240|0",
                             "eventKeyboard|240|1", "eventKeyboard|240|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|-1|1|", "eventKeyboard|-1|0|", "eventKeyboard|-1|1|",
                        "eventKeyboard|-1|0|", "eventKeyboard|-1|1|", "eventKeyboard|-1|0|",
                        "eventKeyboard|-1|1|", "eventKeyboard|-1|0|", "eventKeyboard|-1|1|",
                        "eventKeyboard|-1|0|", "eventKeyboard|-1|1|", "eventKeyboard|-1|0|",
                        "eventKeyboard|-1|1|", "eventKeyboard|-1|0|", "eventKeyboard|-1|1|",
                        "eventKeyboard|-1|0|", "eventKeyboard|-1|1|", "eventKeyboard|-1|0|",
                        "eventKeyboard|-1|1|", "eventKeyboard|-1|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_224_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_224_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_224_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json", "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json",
                    "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json", "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json",
                    "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json", "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json",
                    "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json", "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json",
                    "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json", "keyboard_KEY_VIDEO_NEXT_press.json", "keyboard_KEY_VIDEO_NEXT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|241|1", "eventKeyboard|241|0", "eventKeyboard|241|1",
                             "eventKeyboard|241|0", "eventKeyboard|241|1", "eventKeyboard|241|0",
                             "eventKeyboard|241|1", "eventKeyboard|241|0", "eventKeyboard|241|1",
                             "eventKeyboard|241|0", "eventKeyboard|241|1", "eventKeyboard|241|0",
                             "eventKeyboard|241|1", "eventKeyboard|241|0", "eventKeyboard|241|1",
                             "eventKeyboard|241|0", "eventKeyboard|241|1", "eventKeyboard|241|0",
                             "eventKeyboard|241|1", "eventKeyboard|241|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2657|1|", "eventKeyboard|2657|0|", "eventKeyboard|2657|1|",
                        "eventKeyboard|2657|0|", "eventKeyboard|2657|1|", "eventKeyboard|2657|0|",
                        "eventKeyboard|2657|1|", "eventKeyboard|2657|0|", "eventKeyboard|2657|1|",
                        "eventKeyboard|2657|0|", "eventKeyboard|2657|1|", "eventKeyboard|2657|0|",
                        "eventKeyboard|2657|1|", "eventKeyboard|2657|0|", "eventKeyboard|2657|1|",
                        "eventKeyboard|2657|0|", "eventKeyboard|2657|1|", "eventKeyboard|2657|0|",
                        "eventKeyboard|2657|1|", "eventKeyboard|2657|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_225_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_225_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_225_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json", "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json",
                    "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json", "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json",
                    "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json", "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json",
                    "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json", "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json",
                    "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json", "keyboard_KEY_VIDEO_PREV_press.json", "keyboard_KEY_VIDEO_PREV_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|242|1", "eventKeyboard|242|0", "eventKeyboard|242|1",
                             "eventKeyboard|242|0", "eventKeyboard|242|1", "eventKeyboard|242|0",
                             "eventKeyboard|242|1", "eventKeyboard|242|0", "eventKeyboard|242|1",
                             "eventKeyboard|242|0", "eventKeyboard|242|1", "eventKeyboard|242|0",
                             "eventKeyboard|242|1", "eventKeyboard|242|0", "eventKeyboard|242|1",
                             "eventKeyboard|242|0", "eventKeyboard|242|1", "eventKeyboard|242|0",
                             "eventKeyboard|242|1", "eventKeyboard|242|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2658|1|", "eventKeyboard|2658|0|", "eventKeyboard|2658|1|",
                        "eventKeyboard|2658|0|", "eventKeyboard|2658|1|", "eventKeyboard|2658|0|",
                        "eventKeyboard|2658|1|", "eventKeyboard|2658|0|", "eventKeyboard|2658|1|",
                        "eventKeyboard|2658|0|", "eventKeyboard|2658|1|", "eventKeyboard|2658|0|",
                        "eventKeyboard|2658|1|", "eventKeyboard|2658|0|", "eventKeyboard|2658|1|",
                        "eventKeyboard|2658|0|", "eventKeyboard|2658|1|", "eventKeyboard|2658|0|",
                        "eventKeyboard|2658|1|", "eventKeyboard|2658|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_226_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_226_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_226_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json", "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json",
                    "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json", "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json",
                    "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json", "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json",
                    "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json", "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json",
                    "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json", "keyboard_KEY_BRIGHTNESS_CYCLE_press.json", "keyboard_KEY_BRIGHTNESS_CYCLE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|243|1", "eventKeyboard|243|0", "eventKeyboard|243|1",
                             "eventKeyboard|243|0", "eventKeyboard|243|1", "eventKeyboard|243|0",
                             "eventKeyboard|243|1", "eventKeyboard|243|0", "eventKeyboard|243|1",
                             "eventKeyboard|243|0", "eventKeyboard|243|1", "eventKeyboard|243|0",
                             "eventKeyboard|243|1", "eventKeyboard|243|0", "eventKeyboard|243|1",
                             "eventKeyboard|243|0", "eventKeyboard|243|1", "eventKeyboard|243|0",
                             "eventKeyboard|243|1", "eventKeyboard|243|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2659|1|", "eventKeyboard|2659|0|", "eventKeyboard|2659|1|",
                        "eventKeyboard|2659|0|", "eventKeyboard|2659|1|", "eventKeyboard|2659|0|",
                        "eventKeyboard|2659|1|", "eventKeyboard|2659|0|", "eventKeyboard|2659|1|",
                        "eventKeyboard|2659|0|", "eventKeyboard|2659|1|", "eventKeyboard|2659|0|",
                        "eventKeyboard|2659|1|", "eventKeyboard|2659|0|", "eventKeyboard|2659|1|",
                        "eventKeyboard|2659|0|", "eventKeyboard|2659|1|", "eventKeyboard|2659|0|",
                        "eventKeyboard|2659|1|", "eventKeyboard|2659|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_227_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_227_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_227_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json", "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json",
                    "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json", "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json",
                    "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json", "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json",
                    "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json", "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json",
                    "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json", "keyboard_KEY_BRIGHTNESS_AUTO_press.json", "keyboard_KEY_BRIGHTNESS_AUTO_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|244|1", "eventKeyboard|244|0", "eventKeyboard|244|1",
                             "eventKeyboard|244|0", "eventKeyboard|244|1", "eventKeyboard|244|0",
                             "eventKeyboard|244|1", "eventKeyboard|244|0", "eventKeyboard|244|1",
                             "eventKeyboard|244|0", "eventKeyboard|244|1", "eventKeyboard|244|0",
                             "eventKeyboard|244|1", "eventKeyboard|244|0", "eventKeyboard|244|1",
                             "eventKeyboard|244|0", "eventKeyboard|244|1", "eventKeyboard|244|0",
                             "eventKeyboard|244|1", "eventKeyboard|244|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2660|1|", "eventKeyboard|2660|0|", "eventKeyboard|2660|1|",
                        "eventKeyboard|2660|0|", "eventKeyboard|2660|1|", "eventKeyboard|2660|0|",
                        "eventKeyboard|2660|1|", "eventKeyboard|2660|0|", "eventKeyboard|2660|1|",
                        "eventKeyboard|2660|0|", "eventKeyboard|2660|1|", "eventKeyboard|2660|0|",
                        "eventKeyboard|2660|1|", "eventKeyboard|2660|0|", "eventKeyboard|2660|1|",
                        "eventKeyboard|2660|0|", "eventKeyboard|2660|1|", "eventKeyboard|2660|0|",
                        "eventKeyboard|2660|1|", "eventKeyboard|2660|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_228_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_228_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_228_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json", "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json",
                    "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json", "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json",
                    "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json", "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json",
                    "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json", "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json",
                    "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json", "keyboard_KEY_DISPLAY_OFF_press.json", "keyboard_KEY_DISPLAY_OFF_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|245|1", "eventKeyboard|245|0", "eventKeyboard|245|1",
                             "eventKeyboard|245|0", "eventKeyboard|245|1", "eventKeyboard|245|0",
                             "eventKeyboard|245|1", "eventKeyboard|245|0", "eventKeyboard|245|1",
                             "eventKeyboard|245|0", "eventKeyboard|245|1", "eventKeyboard|245|0",
                             "eventKeyboard|245|1", "eventKeyboard|245|0", "eventKeyboard|245|1",
                             "eventKeyboard|245|0", "eventKeyboard|245|1", "eventKeyboard|245|0",
                             "eventKeyboard|245|1", "eventKeyboard|245|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2661|1|", "eventKeyboard|2661|0|", "eventKeyboard|2661|1|",
                        "eventKeyboard|2661|0|", "eventKeyboard|2661|1|", "eventKeyboard|2661|0|",
                        "eventKeyboard|2661|1|", "eventKeyboard|2661|0|", "eventKeyboard|2661|1|",
                        "eventKeyboard|2661|0|", "eventKeyboard|2661|1|", "eventKeyboard|2661|0|",
                        "eventKeyboard|2661|1|", "eventKeyboard|2661|0|", "eventKeyboard|2661|1|",
                        "eventKeyboard|2661|0|", "eventKeyboard|2661|1|", "eventKeyboard|2661|0|",
                        "eventKeyboard|2661|1|", "eventKeyboard|2661|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_229_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_229_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_229_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json", "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json",
                    "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json", "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json",
                    "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json", "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json",
                    "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json", "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json",
                    "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json", "keyboard_KEY_WWAN_press.json", "keyboard_KEY_WWAN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|246|1", "eventKeyboard|246|0", "eventKeyboard|246|1",
                             "eventKeyboard|246|0", "eventKeyboard|246|1", "eventKeyboard|246|0",
                             "eventKeyboard|246|1", "eventKeyboard|246|0", "eventKeyboard|246|1",
                             "eventKeyboard|246|0", "eventKeyboard|246|1", "eventKeyboard|246|0",
                             "eventKeyboard|246|1", "eventKeyboard|246|0", "eventKeyboard|246|1",
                             "eventKeyboard|246|0", "eventKeyboard|246|1", "eventKeyboard|246|0",
                             "eventKeyboard|246|1", "eventKeyboard|246|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2846|1|", "eventKeyboard|2846|0|", "eventKeyboard|2846|1|",
                        "eventKeyboard|2846|0|", "eventKeyboard|2846|1|", "eventKeyboard|2846|0|",
                        "eventKeyboard|2846|1|", "eventKeyboard|2846|0|", "eventKeyboard|2846|1|",
                        "eventKeyboard|2846|0|", "eventKeyboard|2846|1|", "eventKeyboard|2846|0|",
                        "eventKeyboard|2846|1|", "eventKeyboard|2846|0|", "eventKeyboard|2846|1|",
                        "eventKeyboard|2846|0|", "eventKeyboard|2846|1|", "eventKeyboard|2846|0|",
                        "eventKeyboard|2846|1|", "eventKeyboard|2846|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_230_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_230_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_230_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json", "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json",
                    "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json", "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json",
                    "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json", "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json",
                    "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json", "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json",
                    "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json", "keyboard_KEY_RFKILL_press.json", "keyboard_KEY_RFKILL_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|247|1", "eventKeyboard|247|0", "eventKeyboard|247|1",
                             "eventKeyboard|247|0", "eventKeyboard|247|1", "eventKeyboard|247|0",
                             "eventKeyboard|247|1", "eventKeyboard|247|0", "eventKeyboard|247|1",
                             "eventKeyboard|247|0", "eventKeyboard|247|1", "eventKeyboard|247|0",
                             "eventKeyboard|247|1", "eventKeyboard|247|0", "eventKeyboard|247|1",
                             "eventKeyboard|247|0", "eventKeyboard|247|1", "eventKeyboard|247|0",
                             "eventKeyboard|247|1", "eventKeyboard|247|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2847|1|", "eventKeyboard|2847|0|", "eventKeyboard|2847|1|",
                        "eventKeyboard|2847|0|", "eventKeyboard|2847|1|", "eventKeyboard|2847|0|",
                        "eventKeyboard|2847|1|", "eventKeyboard|2847|0|", "eventKeyboard|2847|1|",
                        "eventKeyboard|2847|0|", "eventKeyboard|2847|1|", "eventKeyboard|2847|0|",
                        "eventKeyboard|2847|1|", "eventKeyboard|2847|0|", "eventKeyboard|2847|1|",
                        "eventKeyboard|2847|0|", "eventKeyboard|2847|1|", "eventKeyboard|2847|0|",
                        "eventKeyboard|2847|1|", "eventKeyboard|2847|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_231_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_231_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_231_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json", "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json",
                    "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json", "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json",
                    "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json", "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json",
                    "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json", "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json",
                    "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json", "keyboard_KEY_MICMUTE_press.json", "keyboard_KEY_MICMUTE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|248|1", "eventKeyboard|248|0", "eventKeyboard|248|1",
                             "eventKeyboard|248|0", "eventKeyboard|248|1", "eventKeyboard|248|0",
                             "eventKeyboard|248|1", "eventKeyboard|248|0", "eventKeyboard|248|1",
                             "eventKeyboard|248|0", "eventKeyboard|248|1", "eventKeyboard|248|0",
                             "eventKeyboard|248|1", "eventKeyboard|248|0", "eventKeyboard|248|1",
                             "eventKeyboard|248|0", "eventKeyboard|248|1", "eventKeyboard|248|0",
                             "eventKeyboard|248|1", "eventKeyboard|248|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|23|1|", "eventKeyboard|23|0|", "eventKeyboard|23|1|",
                        "eventKeyboard|23|0|", "eventKeyboard|23|1|", "eventKeyboard|23|0|",
                        "eventKeyboard|23|1|", "eventKeyboard|23|0|", "eventKeyboard|23|1|",
                        "eventKeyboard|23|0|", "eventKeyboard|23|1|", "eventKeyboard|23|0|",
                        "eventKeyboard|23|1|", "eventKeyboard|23|0|", "eventKeyboard|23|1|",
                        "eventKeyboard|23|0|", "eventKeyboard|23|1|", "eventKeyboard|23|0|",
                        "eventKeyboard|23|1|", "eventKeyboard|23|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_232_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_232_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_232_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json", "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json",
                    "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json", "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json",
                    "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json", "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json",
                    "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json", "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json",
                    "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json", "keyboard_KEY_PAGE_UP_press.json", "keyboard_KEY_PAGE_UP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|104|1", "eventKeyboard|104|0", "eventKeyboard|104|1",
                             "eventKeyboard|104|0", "eventKeyboard|104|1", "eventKeyboard|104|0",
                             "eventKeyboard|104|1", "eventKeyboard|104|0", "eventKeyboard|104|1",
                             "eventKeyboard|104|0", "eventKeyboard|104|1", "eventKeyboard|104|0",
                             "eventKeyboard|104|1", "eventKeyboard|104|0", "eventKeyboard|104|1",
                             "eventKeyboard|104|0", "eventKeyboard|104|1", "eventKeyboard|104|0",
                             "eventKeyboard|104|1", "eventKeyboard|104|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2068|1|", "eventKeyboard|2068|0|", "eventKeyboard|2068|1|",
                        "eventKeyboard|2068|0|", "eventKeyboard|2068|1|", "eventKeyboard|2068|0|",
                        "eventKeyboard|2068|1|", "eventKeyboard|2068|0|", "eventKeyboard|2068|1|",
                        "eventKeyboard|2068|0|", "eventKeyboard|2068|1|", "eventKeyboard|2068|0|",
                        "eventKeyboard|2068|1|", "eventKeyboard|2068|0|", "eventKeyboard|2068|1|",
                        "eventKeyboard|2068|0|", "eventKeyboard|2068|1|", "eventKeyboard|2068|0|",
                        "eventKeyboard|2068|1|", "eventKeyboard|2068|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_233_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_233_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_233_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json", "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json",
                    "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json", "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json",
                    "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json", "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json",
                    "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json", "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json",
                    "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json", "keyboard_KEY_PAGE_DOWN_press.json", "keyboard_KEY_PAGE_DOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|109|1", "eventKeyboard|109|0", "eventKeyboard|109|1",
                             "eventKeyboard|109|0", "eventKeyboard|109|1", "eventKeyboard|109|0",
                             "eventKeyboard|109|1", "eventKeyboard|109|0", "eventKeyboard|109|1",
                             "eventKeyboard|109|0", "eventKeyboard|109|1", "eventKeyboard|109|0",
                             "eventKeyboard|109|1", "eventKeyboard|109|0", "eventKeyboard|109|1",
                             "eventKeyboard|109|0", "eventKeyboard|109|1", "eventKeyboard|109|0",
                             "eventKeyboard|109|1", "eventKeyboard|109|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2069|1|", "eventKeyboard|2069|0|", "eventKeyboard|2069|1|",
                        "eventKeyboard|2069|0|", "eventKeyboard|2069|1|", "eventKeyboard|2069|0|",
                        "eventKeyboard|2069|1|", "eventKeyboard|2069|0|", "eventKeyboard|2069|1|",
                        "eventKeyboard|2069|0|", "eventKeyboard|2069|1|", "eventKeyboard|2069|0|",
                        "eventKeyboard|2069|1|", "eventKeyboard|2069|0|", "eventKeyboard|2069|1|",
                        "eventKeyboard|2069|0|", "eventKeyboard|2069|1|", "eventKeyboard|2069|0|",
                        "eventKeyboard|2069|1|", "eventKeyboard|2069|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_235_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_235_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_235_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json", "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json",
                    "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json", "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json",
                    "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json", "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json",
                    "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json", "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json",
                    "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json", "keyboard_KEY_DPAD_DOWN_press.json", "keyboard_KEY_DPAD_DOWN_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|108|1", "eventKeyboard|108|0", "eventKeyboard|108|1",
                             "eventKeyboard|108|0", "eventKeyboard|108|1", "eventKeyboard|108|0",
                             "eventKeyboard|108|1", "eventKeyboard|108|0", "eventKeyboard|108|1",
                             "eventKeyboard|108|0", "eventKeyboard|108|1", "eventKeyboard|108|0",
                             "eventKeyboard|108|1", "eventKeyboard|108|0", "eventKeyboard|108|1",
                             "eventKeyboard|108|0", "eventKeyboard|108|1", "eventKeyboard|108|0",
                             "eventKeyboard|108|1", "eventKeyboard|108|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2013|1|", "eventKeyboard|2013|0|", "eventKeyboard|2013|1|",
                        "eventKeyboard|2013|0|", "eventKeyboard|2013|1|", "eventKeyboard|2013|0|",
                        "eventKeyboard|2013|1|", "eventKeyboard|2013|0|", "eventKeyboard|2013|1|",
                        "eventKeyboard|2013|0|", "eventKeyboard|2013|1|", "eventKeyboard|2013|0|",
                        "eventKeyboard|2013|1|", "eventKeyboard|2013|0|", "eventKeyboard|2013|1|",
                        "eventKeyboard|2013|0|", "eventKeyboard|2013|1|", "eventKeyboard|2013|0|",
                        "eventKeyboard|2013|1|", "eventKeyboard|2013|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_236_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_236_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_236_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json", "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json",
                    "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json", "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json",
                    "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json", "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json",
                    "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json", "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json",
                    "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json", "keyboard_KEY_DPAD_LEFT_press.json", "keyboard_KEY_DPAD_LEFT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|105|1", "eventKeyboard|105|0", "eventKeyboard|105|1",
                             "eventKeyboard|105|0", "eventKeyboard|105|1", "eventKeyboard|105|0",
                             "eventKeyboard|105|1", "eventKeyboard|105|0", "eventKeyboard|105|1",
                             "eventKeyboard|105|0", "eventKeyboard|105|1", "eventKeyboard|105|0",
                             "eventKeyboard|105|1", "eventKeyboard|105|0", "eventKeyboard|105|1",
                             "eventKeyboard|105|0", "eventKeyboard|105|1", "eventKeyboard|105|0",
                             "eventKeyboard|105|1", "eventKeyboard|105|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2014|1|", "eventKeyboard|2014|0|", "eventKeyboard|2014|1|",
                        "eventKeyboard|2014|0|", "eventKeyboard|2014|1|", "eventKeyboard|2014|0|",
                        "eventKeyboard|2014|1|", "eventKeyboard|2014|0|", "eventKeyboard|2014|1|",
                        "eventKeyboard|2014|0|", "eventKeyboard|2014|1|", "eventKeyboard|2014|0|",
                        "eventKeyboard|2014|1|", "eventKeyboard|2014|0|", "eventKeyboard|2014|1|",
                        "eventKeyboard|2014|0|", "eventKeyboard|2014|1|", "eventKeyboard|2014|0|",
                        "eventKeyboard|2014|1|", "eventKeyboard|2014|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_237_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_237_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_237_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json", "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json",
                    "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json", "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json",
                    "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json", "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json",
                    "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json", "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json",
                    "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json", "keyboard_KEY_DPAD_RIGHT_press.json", "keyboard_KEY_DPAD_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|106|1", "eventKeyboard|106|0", "eventKeyboard|106|1",
                             "eventKeyboard|106|0", "eventKeyboard|106|1", "eventKeyboard|106|0",
                             "eventKeyboard|106|1", "eventKeyboard|106|0", "eventKeyboard|106|1",
                             "eventKeyboard|106|0", "eventKeyboard|106|1", "eventKeyboard|106|0",
                             "eventKeyboard|106|1", "eventKeyboard|106|0", "eventKeyboard|106|1",
                             "eventKeyboard|106|0", "eventKeyboard|106|1", "eventKeyboard|106|0",
                             "eventKeyboard|106|1", "eventKeyboard|106|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2015|1|", "eventKeyboard|2015|0|", "eventKeyboard|2015|1|",
                        "eventKeyboard|2015|0|", "eventKeyboard|2015|1|", "eventKeyboard|2015|0|",
                        "eventKeyboard|2015|1|", "eventKeyboard|2015|0|", "eventKeyboard|2015|1|",
                        "eventKeyboard|2015|0|", "eventKeyboard|2015|1|", "eventKeyboard|2015|0|",
                        "eventKeyboard|2015|1|", "eventKeyboard|2015|0|", "eventKeyboard|2015|1|",
                        "eventKeyboard|2015|0|", "eventKeyboard|2015|1|", "eventKeyboard|2015|0|",
                        "eventKeyboard|2015|1|", "eventKeyboard|2015|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_239_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_239_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_239_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json", "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json",
                    "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json", "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json",
                    "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json", "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json",
                    "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json", "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json",
                    "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json", "keyboard_KEY_ENTER_press.json", "keyboard_KEY_ENTER_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|28|1", "eventKeyboard|28|0", "eventKeyboard|28|1",
                             "eventKeyboard|28|0", "eventKeyboard|28|1", "eventKeyboard|28|0",
                             "eventKeyboard|28|1", "eventKeyboard|28|0", "eventKeyboard|28|1",
                             "eventKeyboard|28|0", "eventKeyboard|28|1", "eventKeyboard|28|0",
                             "eventKeyboard|28|1", "eventKeyboard|28|0", "eventKeyboard|28|1",
                             "eventKeyboard|28|0", "eventKeyboard|28|1", "eventKeyboard|28|0",
                             "eventKeyboard|28|1", "eventKeyboard|28|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2054|1|", "eventKeyboard|2054|0|", "eventKeyboard|2054|1|",
                        "eventKeyboard|2054|0|", "eventKeyboard|2054|1|", "eventKeyboard|2054|0|",
                        "eventKeyboard|2054|1|", "eventKeyboard|2054|0|", "eventKeyboard|2054|1|",
                        "eventKeyboard|2054|0|", "eventKeyboard|2054|1|", "eventKeyboard|2054|0|",
                        "eventKeyboard|2054|1|", "eventKeyboard|2054|0|", "eventKeyboard|2054|1|",
                        "eventKeyboard|2054|0|", "eventKeyboard|2054|1|", "eventKeyboard|2054|0|",
                        "eventKeyboard|2054|1|", "eventKeyboard|2054|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_240_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_240_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_240_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json", "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json",
                    "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json", "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json",
                    "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json", "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json",
                    "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json", "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json",
                    "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json", "keyboard_KEY_ESCAPE_press.json", "keyboard_KEY_ESCAPE_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|1|1", "eventKeyboard|1|0", "eventKeyboard|1|1",
                             "eventKeyboard|1|0", "eventKeyboard|1|1", "eventKeyboard|1|0",
                             "eventKeyboard|1|1", "eventKeyboard|1|0", "eventKeyboard|1|1",
                             "eventKeyboard|1|0", "eventKeyboard|1|1", "eventKeyboard|1|0",
                             "eventKeyboard|1|1", "eventKeyboard|1|0", "eventKeyboard|1|1",
                             "eventKeyboard|1|0", "eventKeyboard|1|1", "eventKeyboard|1|0",
                             "eventKeyboard|1|1", "eventKeyboard|1|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2070|1|", "eventKeyboard|2070|0|", "eventKeyboard|2070|1|",
                        "eventKeyboard|2070|0|", "eventKeyboard|2070|1|", "eventKeyboard|2070|0|",
                        "eventKeyboard|2070|1|", "eventKeyboard|2070|0|", "eventKeyboard|2070|1|",
                        "eventKeyboard|2070|0|", "eventKeyboard|2070|1|", "eventKeyboard|2070|0|",
                        "eventKeyboard|2070|1|", "eventKeyboard|2070|0|", "eventKeyboard|2070|1|",
                        "eventKeyboard|2070|0|", "eventKeyboard|2070|1|", "eventKeyboard|2070|0|",
                        "eventKeyboard|2070|1|", "eventKeyboard|2070|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_241_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_241_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_241_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json",
                    "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json",
                    "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json",
                    "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json",
                    "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json", "keyboard_KEY_TAB_press.json", "keyboard_KEY_TAB_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|15|1", "eventKeyboard|15|0", "eventKeyboard|15|1",
                             "eventKeyboard|15|0", "eventKeyboard|15|1", "eventKeyboard|15|0",
                             "eventKeyboard|15|1", "eventKeyboard|15|0", "eventKeyboard|15|1",
                             "eventKeyboard|15|0", "eventKeyboard|15|1", "eventKeyboard|15|0",
                             "eventKeyboard|15|1", "eventKeyboard|15|0", "eventKeyboard|15|1",
                             "eventKeyboard|15|0", "eventKeyboard|15|1", "eventKeyboard|15|0",
                             "eventKeyboard|15|1", "eventKeyboard|15|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2049|1|", "eventKeyboard|2049|0|", "eventKeyboard|2049|1|",
                        "eventKeyboard|2049|0|", "eventKeyboard|2049|1|", "eventKeyboard|2049|0|",
                        "eventKeyboard|2049|1|", "eventKeyboard|2049|0|", "eventKeyboard|2049|1|",
                        "eventKeyboard|2049|0|", "eventKeyboard|2049|1|", "eventKeyboard|2049|0|",
                        "eventKeyboard|2049|1|", "eventKeyboard|2049|0|", "eventKeyboard|2049|1|",
                        "eventKeyboard|2049|0|", "eventKeyboard|2049|1|", "eventKeyboard|2049|0|",
                        "eventKeyboard|2049|1|", "eventKeyboard|2049|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_242_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_242_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_242_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json", "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json",
                    "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json", "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json",
                    "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json", "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json",
                    "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json", "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json",
                    "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json", "keyboard_KEY_FUNCTION_press.json", "keyboard_KEY_FUNCTION_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|464|1", "eventKeyboard|464|0", "eventKeyboard|464|1",
                             "eventKeyboard|464|0", "eventKeyboard|464|1", "eventKeyboard|464|0",
                             "eventKeyboard|464|1", "eventKeyboard|464|0", "eventKeyboard|464|1",
                             "eventKeyboard|464|0", "eventKeyboard|464|1", "eventKeyboard|464|0",
                             "eventKeyboard|464|1", "eventKeyboard|464|0", "eventKeyboard|464|1",
                             "eventKeyboard|464|0", "eventKeyboard|464|1", "eventKeyboard|464|0",
                             "eventKeyboard|464|1", "eventKeyboard|464|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2078|1|", "eventKeyboard|2078|0|", "eventKeyboard|2078|1|",
                        "eventKeyboard|2078|0|", "eventKeyboard|2078|1|", "eventKeyboard|2078|0|",
                        "eventKeyboard|2078|1|", "eventKeyboard|2078|0|", "eventKeyboard|2078|1|",
                        "eventKeyboard|2078|0|", "eventKeyboard|2078|1|", "eventKeyboard|2078|0|",
                        "eventKeyboard|2078|1|", "eventKeyboard|2078|0|", "eventKeyboard|2078|1|",
                        "eventKeyboard|2078|0|", "eventKeyboard|2078|1|", "eventKeyboard|2078|0|",
                        "eventKeyboard|2078|1|", "eventKeyboard|2078|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_243_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_243_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_243_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json", "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json",
                    "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json", "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json",
                    "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json", "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json",
                    "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json", "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json",
                    "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json", "keyboard_KEY_STAR_press.json", "keyboard_KEY_STAR_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|522|1", "eventKeyboard|522|0", "eventKeyboard|522|1",
                             "eventKeyboard|522|0", "eventKeyboard|522|1", "eventKeyboard|522|0",
                             "eventKeyboard|522|1", "eventKeyboard|522|0", "eventKeyboard|522|1",
                             "eventKeyboard|522|0", "eventKeyboard|522|1", "eventKeyboard|522|0",
                             "eventKeyboard|522|1", "eventKeyboard|522|0", "eventKeyboard|522|1",
                             "eventKeyboard|522|0", "eventKeyboard|522|1", "eventKeyboard|522|0",
                             "eventKeyboard|522|1", "eventKeyboard|522|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2010|1|", "eventKeyboard|2010|0|", "eventKeyboard|2010|1|",
                        "eventKeyboard|2010|0|", "eventKeyboard|2010|1|", "eventKeyboard|2010|0|",
                        "eventKeyboard|2010|1|", "eventKeyboard|2010|0|", "eventKeyboard|2010|1|",
                        "eventKeyboard|2010|0|", "eventKeyboard|2010|1|", "eventKeyboard|2010|0|",
                        "eventKeyboard|2010|1|", "eventKeyboard|2010|0|", "eventKeyboard|2010|1|",
                        "eventKeyboard|2010|0|", "eventKeyboard|2010|1|", "eventKeyboard|2010|0|",
                        "eventKeyboard|2010|1|", "eventKeyboard|2010|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_244_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_244_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_244_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json", "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json",
                    "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json", "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json",
                    "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json", "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json",
                    "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json", "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json",
                    "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json", "keyboard_KEY_POUND_press.json", "keyboard_KEY_POUND_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|523|1", "eventKeyboard|523|0", "eventKeyboard|523|1",
                             "eventKeyboard|523|0", "eventKeyboard|523|1", "eventKeyboard|523|0",
                             "eventKeyboard|523|1", "eventKeyboard|523|0", "eventKeyboard|523|1",
                             "eventKeyboard|523|0", "eventKeyboard|523|1", "eventKeyboard|523|0",
                             "eventKeyboard|523|1", "eventKeyboard|523|0", "eventKeyboard|523|1",
                             "eventKeyboard|523|0", "eventKeyboard|523|1", "eventKeyboard|523|0",
                             "eventKeyboard|523|1", "eventKeyboard|523|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2011|1|", "eventKeyboard|2011|0|", "eventKeyboard|2011|1|",
                        "eventKeyboard|2011|0|", "eventKeyboard|2011|1|", "eventKeyboard|2011|0|",
                        "eventKeyboard|2011|1|", "eventKeyboard|2011|0|", "eventKeyboard|2011|1|",
                        "eventKeyboard|2011|0|", "eventKeyboard|2011|1|", "eventKeyboard|2011|0|",
                        "eventKeyboard|2011|1|", "eventKeyboard|2011|0|", "eventKeyboard|2011|1|",
                        "eventKeyboard|2011|0|", "eventKeyboard|2011|1|", "eventKeyboard|2011|0|",
                        "eventKeyboard|2011|1|", "eventKeyboard|2011|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_005_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_005_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_005_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json", "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json",
                    "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json", "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json",
                    "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json", "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json",
                    "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json", "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json",
                    "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json", "keyboard_KEY_4_press.json", "keyboard_KEY_4_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|5|1", "eventKeyboard|5|0", "eventKeyboard|5|1",
                             "eventKeyboard|5|0", "eventKeyboard|5|1", "eventKeyboard|5|0",
                             "eventKeyboard|5|1", "eventKeyboard|5|0", "eventKeyboard|5|1",
                             "eventKeyboard|5|0", "eventKeyboard|5|1", "eventKeyboard|5|0",
                             "eventKeyboard|5|1", "eventKeyboard|5|0", "eventKeyboard|5|1",
                             "eventKeyboard|5|0", "eventKeyboard|5|1", "eventKeyboard|5|0",
                             "eventKeyboard|5|1", "eventKeyboard|5|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2004|1|", "eventKeyboard|2004|0|", "eventKeyboard|2004|1|",
                        "eventKeyboard|2004|0|", "eventKeyboard|2004|1|", "eventKeyboard|2004|0|",
                        "eventKeyboard|2004|1|", "eventKeyboard|2004|0|", "eventKeyboard|2004|1|",
                        "eventKeyboard|2004|0|", "eventKeyboard|2004|1|", "eventKeyboard|2004|0|",
                        "eventKeyboard|2004|1|", "eventKeyboard|2004|0|", "eventKeyboard|2004|1|",
                        "eventKeyboard|2004|0|", "eventKeyboard|2004|1|", "eventKeyboard|2004|0|",
                        "eventKeyboard|2004|1|", "eventKeyboard|2004|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_127_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_127_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_127_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json", "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json",
                    "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json", "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json",
                    "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json", "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json",
                    "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json", "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json",
                    "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json", "keyboard_KEY_MENU_RIGHT_press.json", "keyboard_KEY_MENU_RIGHT_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|139|1", "eventKeyboard|139|0", "eventKeyboard|139|1",
                             "eventKeyboard|139|0", "eventKeyboard|139|1", "eventKeyboard|139|0",
                             "eventKeyboard|139|1", "eventKeyboard|139|0", "eventKeyboard|139|1",
                             "eventKeyboard|139|0", "eventKeyboard|139|1", "eventKeyboard|139|0",
                             "eventKeyboard|139|1", "eventKeyboard|139|0", "eventKeyboard|139|1",
                             "eventKeyboard|139|0", "eventKeyboard|139|1", "eventKeyboard|139|0",
                             "eventKeyboard|139|1", "eventKeyboard|139|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_171_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_171_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_171_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json", "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json",
                    "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json", "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json",
                    "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json", "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json",
                    "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json", "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json",
                    "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json", "keyboard_KEY_F13_press.json", "keyboard_KEY_F13_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|183|1", "eventKeyboard|183|0", "eventKeyboard|183|1",
                             "eventKeyboard|183|0", "eventKeyboard|183|1", "eventKeyboard|183|0",
                             "eventKeyboard|183|1", "eventKeyboard|183|0", "eventKeyboard|183|1",
                             "eventKeyboard|183|0", "eventKeyboard|183|1", "eventKeyboard|183|0",
                             "eventKeyboard|183|1", "eventKeyboard|183|0", "eventKeyboard|183|1",
                             "eventKeyboard|183|0", "eventKeyboard|183|1", "eventKeyboard|183|0",
                             "eventKeyboard|183|1", "eventKeyboard|183|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2816|1|", "eventKeyboard|2816|0|", "eventKeyboard|2816|1|",
                        "eventKeyboard|2816|0|", "eventKeyboard|2816|1|", "eventKeyboard|2816|0|",
                        "eventKeyboard|2816|1|", "eventKeyboard|2816|0|", "eventKeyboard|2816|1|",
                        "eventKeyboard|2816|0|", "eventKeyboard|2816|1|", "eventKeyboard|2816|0|",
                        "eventKeyboard|2816|1|", "eventKeyboard|2816|0|", "eventKeyboard|2816|1|",
                        "eventKeyboard|2816|0|", "eventKeyboard|2816|1|", "eventKeyboard|2816|0|",
                        "eventKeyboard|2816|1|", "eventKeyboard|2816|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_172_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_172_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_172_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json", "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json",
                    "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json", "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json",
                    "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json", "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json",
                    "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json", "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json",
                    "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json", "keyboard_KEY_F14_press.json", "keyboard_KEY_F14_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|184|1", "eventKeyboard|184|0", "eventKeyboard|184|1",
                             "eventKeyboard|184|0", "eventKeyboard|184|1", "eventKeyboard|184|0",
                             "eventKeyboard|184|1", "eventKeyboard|184|0", "eventKeyboard|184|1",
                             "eventKeyboard|184|0", "eventKeyboard|184|1", "eventKeyboard|184|0",
                             "eventKeyboard|184|1", "eventKeyboard|184|0", "eventKeyboard|184|1",
                             "eventKeyboard|184|0", "eventKeyboard|184|1", "eventKeyboard|184|0",
                             "eventKeyboard|184|1", "eventKeyboard|184|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2817|1|", "eventKeyboard|2817|0|", "eventKeyboard|2817|1|",
                        "eventKeyboard|2817|0|", "eventKeyboard|2817|1|", "eventKeyboard|2817|0|",
                        "eventKeyboard|2817|1|", "eventKeyboard|2817|0|", "eventKeyboard|2817|1|",
                        "eventKeyboard|2817|0|", "eventKeyboard|2817|1|", "eventKeyboard|2817|0|",
                        "eventKeyboard|2817|1|", "eventKeyboard|2817|0|", "eventKeyboard|2817|1|",
                        "eventKeyboard|2817|0|", "eventKeyboard|2817|1|", "eventKeyboard|2817|0|",
                        "eventKeyboard|2817|1|", "eventKeyboard|2817|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_234_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_234_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_234_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json", "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json",
                    "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json", "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json",
                    "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json", "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json",
                    "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json", "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json",
                    "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json", "keyboard_KEY_DPAD_UP_press.json", "keyboard_KEY_DPAD_UP_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|103|1", "eventKeyboard|103|0", "eventKeyboard|103|1",
                             "eventKeyboard|103|0", "eventKeyboard|103|1", "eventKeyboard|103|0",
                             "eventKeyboard|103|1", "eventKeyboard|103|0", "eventKeyboard|103|1",
                             "eventKeyboard|103|0", "eventKeyboard|103|1", "eventKeyboard|103|0",
                             "eventKeyboard|103|1", "eventKeyboard|103|0", "eventKeyboard|103|1",
                             "eventKeyboard|103|0", "eventKeyboard|103|1", "eventKeyboard|103|0",
                             "eventKeyboard|103|1", "eventKeyboard|103|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2012|1|", "eventKeyboard|2012|0|", "eventKeyboard|2012|1|",
                        "eventKeyboard|2012|0|", "eventKeyboard|2012|1|", "eventKeyboard|2012|0|",
                        "eventKeyboard|2012|1|", "eventKeyboard|2012|0|", "eventKeyboard|2012|1|",
                        "eventKeyboard|2012|0|", "eventKeyboard|2012|1|", "eventKeyboard|2012|0|",
                        "eventKeyboard|2012|1|", "eventKeyboard|2012|0|", "eventKeyboard|2012|1|",
                        "eventKeyboard|2012|0|", "eventKeyboard|2012|1|", "eventKeyboard|2012|0|",
                        "eventKeyboard|2012|1|", "eventKeyboard|2012|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_KeyBoard_UnReg_mult_238_R
 * Function: Multi_HandOut_KeyBoard_UnReg_mult_238_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutkeyboardmulticlickUnRegRTest, Multi_HandOut_KeyBoard_UnReg_mult_238_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json", "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json",
                    "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json", "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json",
                    "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json", "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json",
                    "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json", "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json",
                    "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json", "keyboard_KEY_LEFT_MENU_press.json", "keyboard_KEY_LEFT_MENU_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|127|1", "eventKeyboard|127|0", "eventKeyboard|127|1",
                             "eventKeyboard|127|0", "eventKeyboard|127|1", "eventKeyboard|127|0",
                             "eventKeyboard|127|1", "eventKeyboard|127|0", "eventKeyboard|127|1",
                             "eventKeyboard|127|0", "eventKeyboard|127|1", "eventKeyboard|127|0",
                             "eventKeyboard|127|1", "eventKeyboard|127|0", "eventKeyboard|127|1",
                             "eventKeyboard|127|0", "eventKeyboard|127|1", "eventKeyboard|127|0",
                             "eventKeyboard|127|1", "eventKeyboard|127|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|", "eventKeyboard|2067|1|",
                        "eventKeyboard|2067|0|", "eventKeyboard|2067|1|", "eventKeyboard|2067|0|",
                        "eventKeyboard|2067|1|", "eventKeyboard|2067|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}
}