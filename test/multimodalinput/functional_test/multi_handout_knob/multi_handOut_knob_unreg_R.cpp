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

class multiHandoutknobUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutknobUnRegRTest::st_client_helper_;

void multiHandoutknobUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutknobUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutknobUnRegRTest::SetUp()
{
}

void multiHandoutknobUnRegRTest::TearDown()
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
std::string multiHandoutknobUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutknobUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_Knob_UnReg_Untouch_006_R
 * Function: Multi_HandOut_Knob_UnReg_Untouch_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_UnReg_Untouch_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_leftturnpress.json", "knob_model1_leftturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|114|1", "eventKeyboard|114|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|17|1|", "eventKeyboard|17|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_001_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_press.json", "knob_model1_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|113|1", "eventKeyboard|113|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|22|1|", "eventKeyboard|22|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_011_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_011_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_011_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_rightturnpress.json", "knob_model1_rightturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|115|1", "eventKeyboard|115|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|16|1|", "eventKeyboard|16|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_016_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_016_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_016_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model2_leftctrl_press.json", "knob_model2_leftalt_press.json", "knob_model2_n_press.json",
                    "knob_model2_leftctrl_release.json","knob_model2_leftalt_release.json", "knob_model2_n_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventKeyboard|56|1", "eventKeyboard|49|1",
                             "eventKeyboard|29|0", "eventKeyboard|56|0", "eventKeyboard|49|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventKeyboard|2045|1|", "eventKeyboard|2030|1|",
                        "eventKeyboard|2072|0|", "eventKeyboard|2045|0|", "eventKeyboard|2030|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_021_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model2_leftturnpress.json", "knob_model2_leftturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|114|1", "eventKeyboard|114|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|17|1|", "eventKeyboard|17|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_026_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model2_rightturnpress.json", "knob_model2_rightturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|115|1", "eventKeyboard|115|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|16|1|", "eventKeyboard|16|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_031_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_031_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_031_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model3_press.json", "knob_model3_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|274|1", "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|4|1|", "eventPointer|4|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_036_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_036_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_036_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model3_leftturn.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|0|-15"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|0|-15|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_041_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_041_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_041_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model3_rightturn.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|0|15"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|0|15|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_046_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_046_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_046_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_down_leftturnpress.json", "knob_model1_down_leftturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|165|1", "eventKeyboard|165|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|13|1|", "eventKeyboard|13|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_051_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_051_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_051_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_down_rightturnpress.json", "knob_model1_down_rightturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|163|1", "eventKeyboard|163|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|12|1|", "eventKeyboard|12|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_056_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_056_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_056_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model2_down_leftturnpress.json", "knob_model2_down_leftturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|105|1", "eventKeyboard|105|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2014|1|", "eventKeyboard|2014|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_061_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_061_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_061_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model2_down_rightturnpress.json", "knob_model2_down_rightturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|106|1", "eventKeyboard|106|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2015|1|", "eventKeyboard|2015|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_066_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_066_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_066_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model3_down_leftturnpress.json", "knob_model3_down_leftturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|275|1", "eventPointer|275|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|32|1|", "eventPointer|32|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_Unreg_Untouch_071_R
 * Function: Multi_HandOut_Knob_Unreg_Untouch_071_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_Unreg_Untouch_071_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model3_down_rightturnpress.json", "knob_model3_down_rightturnrelease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|276|1", "eventPointer|276|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|64|1|", "eventPointer|64|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}


/*
 * Feature: Multi_HandOut_Knob_UnReg_Unusual_002_R
 * Function: Multi_HandOut_Knob_UnReg_Unusual_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_UnReg_Unusual_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"non_knob_press.json", "non_knob_realease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"NULL|NULL|NULL ", "NULL|NULL|NULL "};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"NULL|NULL|NULL|", "NULL|NULL|NULL|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Knob_UnReg_Unusual_001_R
 * Function: Multi_HandOut_Knob_UnReg_Unusual_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutknobUnRegRTest, Multi_HandOut_Knob_UnReg_Unusual_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json", "knob_model1_press.json", "knob_model1_release.json",
                    "knob_model1_press.json", "knob_model1_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0",
                             "eventKeyboard|113|1", "eventKeyboard|113|0", "eventKeyboard|113|1",
                             "eventKeyboard|113|0", "eventKeyboard|113|1", "eventKeyboard|113|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|",
                        "eventKeyboard|22|1|", "eventKeyboard|22|0|", "eventKeyboard|22|1|",
                        "eventKeyboard|22|0|", "eventKeyboard|22|1|", "eventKeyboard|22|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0", "|0|1|0,|1|0|0",
                         "|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}
}