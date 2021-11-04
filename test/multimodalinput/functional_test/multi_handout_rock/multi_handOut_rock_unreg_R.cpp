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

class multiHandoutrockUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutrockUnRegRTest::st_client_helper_;

void multiHandoutrockUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutrockUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutrockUnRegRTest::SetUp()
{
}

void multiHandoutrockUnRegRTest::TearDown()
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
std::string multiHandoutrockUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutrockUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_001_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_288_press.json", "rock_key_288_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|288|1", "eventJoyStickKey|288|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2401|1|", "eventKeyboard|2401|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_006_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_290_press.json", "rock_key_290_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|290|1", "eventJoyStickKey|290|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2403|1|", "eventKeyboard|2403|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_011_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_011_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_011_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_291_press.json", "rock_key_291_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|291|1", "eventJoyStickKey|291|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2404|1|", "eventKeyboard|2404|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_016_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_016_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_016_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_289_press.json", "rock_key_289_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|289|1", "eventJoyStickKey|289|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2402|1|", "eventKeyboard|2402|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_021_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_292_press.json", "rock_key_292_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|292|1", "eventJoyStickKey|292|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2405|1|", "eventKeyboard|2405|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_026_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_293_press.json", "rock_key_293_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|293|1", "eventJoyStickKey|293|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2406|1|", "eventKeyboard|2406|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_031_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_031_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_031_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_294_press.json", "rock_key_294_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|294|1", "eventJoyStickKey|294|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2407|1|", "eventKeyboard|2407|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_036_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_036_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_036_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_297_press.json", "rock_key_297_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|297|1", "eventJoyStickKey|297|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2410|1|", "eventKeyboard|2410|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_041_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_041_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_041_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_296_press.json", "rock_key_296_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|296|1", "eventJoyStickKey|296|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2409|1|", "eventKeyboard|2409|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_046_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_046_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_046_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_295_press.json", "rock_key_295_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|295|1", "eventJoyStickKey|295|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2408|1|", "eventKeyboard|2408|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_051_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_051_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_051_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_300_press.json", "rock_key_300_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|300|1", "eventJoyStickKey|300|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2413|1|", "eventKeyboard|2413|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_056_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_056_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_056_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_299_press.json", "rock_key_299_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|299|1", "eventJoyStickKey|299|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2412|1|", "eventKeyboard|2412|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_061_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_061_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_061_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_298_press.json", "rock_key_298_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|298|1", "eventJoyStickKey|298|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2411|1|", "eventKeyboard|2411|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_066_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_066_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_066_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_301_press.json", "rock_key_301_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|301|1", "eventJoyStickKey|301|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2414|1|", "eventKeyboard|2414|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_071_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_071_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_071_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_302_press.json", "rock_key_302_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|302|1", "eventJoyStickKey|302|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2415|1|", "eventKeyboard|2415|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_076_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_076_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_076_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_key_303_press.json", "rock_key_303_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventJoyStickKey|303|1", "eventJoyStickKey|303|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2416|1|", "eventKeyboard|2416|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_081_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_081_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_081_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_kuli_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_086_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_086_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_086_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_kuli_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_091_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_091_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_091_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_kuli_left.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_096_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_096_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_096_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_kuli_right.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_101_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_101_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_101_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_106_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_106_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_106_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_111_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_111_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_111_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_left.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_116_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_116_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_116_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_right.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_121_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_121_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_121_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_youmen_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_126_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_126_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_126_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_youmen_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_131_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_131_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_131_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_leftturn.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_136_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_136_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_136_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"rock_yaogan_rightturn.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventJoyStickAxis"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventJoyStickAxis|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Rock_UnReg_Untouch_Unusual_001_R
 * Function: Multi_HandOut_Rock_UnReg_Untouch_Unusual_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutrockUnRegRTest, Multi_HandOut_Rock_UnReg_Untouch_Unusual_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"non_rock_press.json", "non_rock_realease.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"NULL|NULL|NULL", "NULL|NULL|NULL"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"NULL|NULL|NULL|", "NULL|NULL|NULL|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}
}