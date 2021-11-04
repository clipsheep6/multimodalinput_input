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

class multiHandoutremotecontrolUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutremotecontrolUnRegRTest::st_client_helper_;

void multiHandoutremotecontrolUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutremotecontrolUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutremotecontrolUnRegRTest::SetUp()
{
}

void multiHandoutremotecontrolUnRegRTest::TearDown()
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
std::string multiHandoutremotecontrolUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutremotecontrolUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_021_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_021_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_021_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CHANNELDOWN_press.json", "remote_CHANNELDOWN_release.json", "remote_enter_press.json", "remote_enter_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|403|1", "eventKeyboard|403|0", "eventKeyboard|28|1",
                             "eventKeyboard|28|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2691|1|", "eventKeyboard|2691|0|", "eventKeyboard|2054|1|",
                        "eventKeyboard|2054|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_026_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_026_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_026_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CHANNELDOWN_press.json", "remote_CHANNELDOWN_release.json", "remote_enter_long_press.json", "remote_enter_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|403|1", "eventKeyboard|403|0", "eventKeyboard|28|1",
                             "eventKeyboard|28|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2691|1|", "eventKeyboard|2691|0|", "eventKeyboard|2054|1|",
                        "eventKeyboard|2054|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_036_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_036_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_036_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"non_remote_press.json", "non_remote_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"NULL|NULL|NULL", "NULL|NULL|NULL"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"NULL|NULL|NULL|", "NULL|NULL|NULL|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_040_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_040_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_040_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_RESTART_press.json", "remote_RESTART_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|408|1", "eventKeyboard|408|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2693|1|", "eventKeyboard|2693|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_042_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_042_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_042_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SELECT_press.json", "remote_SELECT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|353|1", "eventKeyboard|353|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2016|1|", "eventKeyboard|2016|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_043_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_043_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_043_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ESC_press.json", "remote_ESC_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|1|1", "eventKeyboard|1|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2070|1|", "eventKeyboard|2070|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_044_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_044_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_044_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KPPLUS_press.json", "remote_KPPLUS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|78|1", "eventKeyboard|78|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2116|1|", "eventKeyboard|2116|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_045_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_045_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_045_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KPMINUS_press.json", "remote_KPMINUS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|74|1", "eventKeyboard|74|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2115|1|", "eventKeyboard|2115|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_046_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_046_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_046_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SUBTITLE_press.json", "remote_SUBTITLE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|370|1", "eventKeyboard|370|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2667|1|", "eventKeyboard|2667|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_048_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_048_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_048_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_RED_press.json", "remote_RED_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|398|1", "eventKeyboard|398|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2686|1|", "eventKeyboard|2686|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_049_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_049_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_049_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_GREEN_press.json", "remote_GREEN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|399|1", "eventKeyboard|399|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2687|1|", "eventKeyboard|2687|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_050_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_050_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_050_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BLUE_press.json", "remote_BLUE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|401|1", "eventKeyboard|401|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2689|1|", "eventKeyboard|2689|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_051_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_051_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_051_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_YELLOW_press.json", "remote_YELLOW_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|400|1", "eventKeyboard|400|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2688|1|", "eventKeyboard|2688|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_052_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_052_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_052_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ASPECT_RATIO_press.json", "remote_ASPECT_RATIO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|375|1", "eventKeyboard|375|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2670|1|", "eventKeyboard|2670|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_055_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_055_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_055_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BRIGHTNESS_TOGGLE_press.json", "remote_BRIGHTNESS_TOGGLE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|431|1", "eventKeyboard|431|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2711|1|", "eventKeyboard|2711|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_056_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_056_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_056_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BRIGHTNESS_MIN_press.json", "remote_BRIGHTNESS_MIN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|592|1", "eventKeyboard|592|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2724|1|", "eventKeyboard|2724|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_057_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_057_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_057_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BRIGHTNESS_MAX_press.json", "remote_BRIGHTNESS_MAX_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|593|1", "eventKeyboard|593|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2725|1|", "eventKeyboard|2725|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_058_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_058_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_058_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BRIGHTNESS_AUTO_press.json", "remote_BRIGHTNESS_AUTO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|244|1", "eventKeyboard|244|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2660|1|", "eventKeyboard|2660|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_059_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_059_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_059_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDILLUMUP_press.json", "remote_KBDILLUMUP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|230|1", "eventKeyboard|230|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2651|1|", "eventKeyboard|2651|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_060_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_060_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_060_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDILLUMDOWN_press.json", "remote_KBDILLUMDOWN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|229|1", "eventKeyboard|229|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2650|1|", "eventKeyboard|2650|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_061_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_061_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_061_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDILLUMTOGGLE_press.json", "remote_KBDILLUMTOGGLE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|228|1", "eventKeyboard|228|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2649|1|", "eventKeyboard|2649|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_062_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_062_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_062_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VIDEO_NEXT_press.json", "remote_VIDEO_NEXT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|241|1", "eventKeyboard|241|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2657|1|", "eventKeyboard|2657|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_063_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_063_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_063_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_LAST_press.json", "remote_LAST_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|405|1", "eventKeyboard|405|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2692|1|", "eventKeyboard|2692|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_065_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_065_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_065_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PC_press.json", "remote_PC_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|376|1", "eventKeyboard|376|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2671|1|", "eventKeyboard|2671|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_066_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_066_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_066_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_TV_press.json", "remote_TV_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|377|1", "eventKeyboard|377|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2672|1|", "eventKeyboard|2672|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_067_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_067_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_067_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_DVD_press.json", "remote_DVD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|389|1", "eventKeyboard|389|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2681|1|", "eventKeyboard|2681|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_068_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_068_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_068_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PHONE_press.json", "remote_PHONE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|169|1", "eventKeyboard|169|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|3|1|", "eventKeyboard|3|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_069_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_069_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_069_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PROGRAM_press.json", "remote_PROGRAM_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|362|1", "eventKeyboard|362|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2665|1|", "eventKeyboard|2665|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_070_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_070_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_070_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VIDEOPHONE_press.json", "remote_VIDEOPHONE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|416|1", "eventKeyboard|416|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2696|1|", "eventKeyboard|2696|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_071_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_071_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_071_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_GAMES_press.json", "remote_GAMES_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|417|1", "eventKeyboard|417|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2697|1|", "eventKeyboard|2697|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_072_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_072_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_072_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MEMO_press.json", "remote_MEMO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|396|1", "eventKeyboard|396|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2684|1|", "eventKeyboard|2684|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_073_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_073_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_073_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CD_press.json", "remote_CD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|383|1", "eventKeyboard|383|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2677|1|", "eventKeyboard|2677|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_074_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_074_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_074_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VCR_press.json", "remote_VCR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|379|1", "eventKeyboard|379|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2674|1|", "eventKeyboard|2674|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_075_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_075_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_075_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_TUNER_press.json", "remote_TUNER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|386|1", "eventKeyboard|386|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2679|1|", "eventKeyboard|2679|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_076_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_076_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_076_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_TAPE_press.json", "remote_TAPE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|384|1", "eventKeyboard|384|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2678|1|", "eventKeyboard|2678|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_077_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_077_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_077_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_TV2_press.json", "remote_TV2_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|378|1", "eventKeyboard|378|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2673|1|", "eventKeyboard|2673|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_078_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_078_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_078_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SAT_press.json", "remote_SAT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|381|1", "eventKeyboard|381|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2676|1|", "eventKeyboard|2676|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_079_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_079_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_079_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PVR_press.json", "remote_PVR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|366|1", "eventKeyboard|366|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2666|1|", "eventKeyboard|2666|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_082_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_082_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_082_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VCR2_press.json", "remote_VCR2_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|380|1", "eventKeyboard|380|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2675|1|", "eventKeyboard|2675|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_085_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_085_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_085_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_RECORD_press.json", "remote_RECORD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|167|1", "eventKeyboard|167|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2089|1|", "eventKeyboard|2089|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_086_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_086_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_086_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FASTFORWARD_press.json", "remote_FASTFORWARD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|208|1", "eventKeyboard|208|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|15|1|", "eventKeyboard|15|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_087_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_087_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_087_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_REWIND_press.json", "remote_REWIND_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|168|1", "eventKeyboard|168|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|14|1|", "eventKeyboard|14|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_088_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_088_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_088_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_NEXTSONG_press.json", "remote_NEXTSONG_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|163|1", "eventKeyboard|163|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|12|1|", "eventKeyboard|12|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_089_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_089_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_089_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PREVIOUSSONG_press.json", "remote_PREVIOUSSONG_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|165|1", "eventKeyboard|165|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|13|1|", "eventKeyboard|13|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_090_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_090_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_090_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_STOPCD_press.json", "remote_STOPCD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|166|1", "eventKeyboard|166|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|11|1|", "eventKeyboard|11|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_091_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_091_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_091_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EJECTCD_press.json", "remote_EJECTCD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|161|1", "eventKeyboard|161|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2088|1|", "eventKeyboard|2088|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_092_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_092_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_092_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MEDIA_REPEAT_press.json", "remote_MEDIA_REPEAT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|439|1", "eventKeyboard|439|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2714|1|", "eventKeyboard|2714|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_093_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_093_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_093_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SHUFFLE_press.json", "remote_SHUFFLE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|410|1", "eventKeyboard|410|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2695|1|", "eventKeyboard|2695|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_094_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_094_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_094_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SLOW_press.json", "remote_SLOW_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|499|1", "eventKeyboard|499|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|3203|1|", "eventKeyboard|3203|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_095_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_095_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_095_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PLAYPAUSE_press.json", "remote_PLAYPAUSE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|164|1", "eventKeyboard|164|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|10|1|", "eventKeyboard|10|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_096_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_096_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_096_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"ABS_VOLUME_press.json", "ABS_VOLUME_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|32|1", "eventKeyboard|32|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2020|1|", "eventKeyboard|2020|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_097_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_097_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_097_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MUTE_press.json", "remote_MUTE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|113|1", "eventKeyboard|113|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|22|1|", "eventKeyboard|22|0|"};
    ci.dispatcherList = {"|0|1|0,|1|0|0", "|0|1|0,|1|0|0"};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_098_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_098_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_098_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BASSBOOST_press.json", "remote_BASSBOOST_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|209|1", "eventKeyboard|209|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2644|1|", "eventKeyboard|2644|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_101_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_101_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_101_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SLOW_press.json", "remote_SLOW_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|409|1", "eventKeyboard|409|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2694|1|", "eventKeyboard|2694|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_102_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_102_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_102_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BUTTONCONFIG_press.json", "remote_BUTTONCONFIG_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|576|1", "eventKeyboard|576|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2716|1|", "eventKeyboard|2716|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_103_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_103_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_103_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CONFIG_press.json", "remote_CONFIG_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|171|1", "eventKeyboard|171|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2634|1|", "eventKeyboard|2634|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_104_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_104_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_104_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_WORDPROCESSOR_press.json", "remote_WORDPROCESSOR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|421|1", "eventKeyboard|421|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2701|1|", "eventKeyboard|2701|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_105_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_105_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_105_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EDITOR_press.json", "remote_EDITOR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|422|1", "eventKeyboard|422|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2702|1|", "eventKeyboard|2702|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_106_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_106_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_106_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SPREADSHEET_press.json", "remote_SPREADSHEET_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|423|1", "eventKeyboard|423|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2703|1|", "eventKeyboard|2703|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_107_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_107_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_107_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_GRAPHICSEDITOR_press.json", "remote_GRAPHICSEDITOR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|424|1", "eventKeyboard|424|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2704|1|", "eventKeyboard|2704|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_108_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_108_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_108_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PRESENTATION_press.json", "remote_PRESENTATION_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|425|1", "eventKeyboard|425|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2705|1|", "eventKeyboard|2705|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_109_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_109_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_109_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_DATABASE_press.json", "remote_DATABASE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|426|1", "eventKeyboard|426|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2706|1|", "eventKeyboard|2706|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_110_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_110_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_110_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MAIL_press.json", "remote_MAIL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|155|1", "eventKeyboard|155|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2053|1|", "eventKeyboard|2053|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_111_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_111_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_111_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_NEWS_press.json", "remote_NEWS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|427|1", "eventKeyboard|427|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2707|1|", "eventKeyboard|2707|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_112_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_112_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_112_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VOICEMAIL_press.json", "remote_VOICEMAIL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|428|1", "eventKeyboard|428|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2708|1|", "eventKeyboard|2708|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_113_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_113_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_113_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ADDRESSBOOK_press.json", "remote_ADDRESSBOOK_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|429|1", "eventKeyboard|429|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2709|1|", "eventKeyboard|2709|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_114_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_114_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_114_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CALENDAR_press.json", "remote_CALENDAR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|397|1", "eventKeyboard|397|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2685|1|", "eventKeyboard|2685|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_115_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_115_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_115_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_TASKMANAGER_press.json", "remote_TASKMANAGER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|577|1", "eventKeyboard|577|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2717|1|", "eventKeyboard|2717|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_116_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_116_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_116_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_JOURNAL_press.json", "remote_JOURNAL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|578|1", "eventKeyboard|578|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2718|1|", "eventKeyboard|2718|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_117_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_117_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_117_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FINANCE_press.json", "remote_FINANCE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|219|1", "eventKeyboard|219|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2647|1|", "eventKeyboard|2647|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_118_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_118_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_118_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CALC_press.json", "remote_CALC_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|140|1", "eventKeyboard|140|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2626|1|", "eventKeyboard|2626|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_119_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_119_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_119_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PLAYER_press.json", "remote_PLAYER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|387|1", "eventKeyboard|387|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2680|1|", "eventKeyboard|2680|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_120_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_120_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_120_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FILE_press.json", "remote_FILE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|144|1", "eventKeyboard|144|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2627|1|", "eventKeyboard|2627|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_121_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_121_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_121_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EXPLORER_press.json", "remote_EXPLORER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|150|1", "eventKeyboard|150|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2052|1|", "eventKeyboard|2052|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_122_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_122_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_122_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_COFFEE_press.json", "remote_COFFEE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|433|1", "eventKeyboard|433|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2713|1|", "eventKeyboard|2713|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_123_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_123_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_123_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CONTROLPANEL_press.json", "remote_CONTROLPANEL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|579|1", "eventKeyboard|579|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2719|1|", "eventKeyboard|2719|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_124_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_124_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_124_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_APPSELECT_press.json", "remote_APPSELECT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|580|1", "eventKeyboard|580|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2720|1|", "eventKeyboard|2720|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_125_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_125_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_125_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_NEXT_press.json", "remote_NEXT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|407|1", "eventKeyboard|407|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2629|1|", "eventKeyboard|2629|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_126_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_126_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_126_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PREVIOUS_press.json", "remote_PREVIOUS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|412|1", "eventKeyboard|412|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2631|1|", "eventKeyboard|2631|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_127_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_127_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_127_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_HELP_press.json", "remote_HELP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|138|1", "eventKeyboard|138|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2625|1|", "eventKeyboard|2625|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_128_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_128_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_128_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_DOCUMENTS_press.json", "remote_DOCUMENTS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|235|1", "eventKeyboard|235|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2656|1|", "eventKeyboard|2656|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_129_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_129_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_129_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SPELLCHECK_press.json", "remote_SPELLCHECK_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|432|1", "eventKeyboard|432|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2712|1|", "eventKeyboard|2712|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_130_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_130_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_130_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_remoteBOARD_press.json", "remote_remoteBOARD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|374|1", "eventKeyboard|374|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2669|1|", "eventKeyboard|2669|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_131_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_131_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_131_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SCREENSAVER_press.json", "remote_SCREENSAVER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|581|1", "eventKeyboard|581|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2721|1|", "eventKeyboard|2721|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_132_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_132_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_132_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FILE_press.json", "remote_FILE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|144|1", "eventKeyboard|144|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2627|1|", "eventKeyboard|2627|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_133_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_133_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_133_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_IMAGES_press.json", "remote_IMAGES_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|442|1", "eventKeyboard|442|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2715|1|", "eventKeyboard|2715|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_134_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_134_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_134_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_AUDIO_press.json", "remote_AUDIO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|392|1", "eventKeyboard|392|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2682|1|", "eventKeyboard|2682|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_135_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_135_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_135_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VIDEO_press.json", "remote_VIDEO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|393|1", "eventKeyboard|393|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2683|1|", "eventKeyboard|2683|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_136_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_136_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_136_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MESSENGER_press.json", "remote_MESSENGER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|430|1", "eventKeyboard|430|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2710|1|", "eventKeyboard|2710|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_137_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_137_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_137_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_INFO_press.json", "remote_INFO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|358|1", "eventKeyboard|358|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2664|1|", "eventKeyboard|2664|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_139_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_139_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_139_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_NEW_press.json", "remote_NEW_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|181|1", "eventKeyboard|181|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2640|1|", "eventKeyboard|2640|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_140_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_140_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_140_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_OPEN_press.json", "remote_OPEN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|134|1", "eventKeyboard|134|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2621|1|", "eventKeyboard|2621|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_141_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_141_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_141_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CLOSE_press.json", "remote_CLOSE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|206|1", "eventKeyboard|206|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2642|1|", "eventKeyboard|2642|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_142_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_142_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_142_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EXIT_press.json", "remote_EXIT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|174|1", "eventKeyboard|174|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2636|1|", "eventKeyboard|2636|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_143_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_143_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_143_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SAVE_press.json", "remote_SAVE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|234|1", "eventKeyboard|234|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2655|1|", "eventKeyboard|2655|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_144_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_144_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_144_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PRINT_press.json", "remote_PRINT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|210|1", "eventKeyboard|210|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2645|1|", "eventKeyboard|2645|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_145_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_145_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_145_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PROPS_press.json", "remote_PROPS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|130|1", "eventKeyboard|130|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2618|1|", "eventKeyboard|2618|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_146_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_146_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_146_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_UNDO_press.json", "remote_UNDO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|131|1", "eventKeyboard|131|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2619|1|", "eventKeyboard|2619|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_147_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_147_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_147_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_COPY_press.json", "remote_COPY_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|133|1", "eventKeyboard|133|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2620|1|", "eventKeyboard|2620|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_148_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_148_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_148_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CUT_press.json", "remote_CUT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|137|1", "eventKeyboard|137|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2624|1|", "eventKeyboard|2624|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_149_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_149_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_149_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_PASTE_press.json", "remote_PASTE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|135|1", "eventKeyboard|135|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2622|1|", "eventKeyboard|2622|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_150_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_150_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_150_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FIND_press.json", "remote_FIND_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|136|1", "eventKeyboard|136|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2623|1|", "eventKeyboard|2623|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_151_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_151_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_151_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_GOTO_press.json", "remote_GOTO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|354|1", "eventKeyboard|354|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2663|1|", "eventKeyboard|2663|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_152_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_152_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_152_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FORWARD_press.json", "remote_FORWARD_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|159|1", "eventKeyboard|159|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2084|1|", "eventKeyboard|2084|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_153_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_153_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_153_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_STOP_press.json", "remote_STOP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|128|1", "eventKeyboard|128|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2616|1|", "eventKeyboard|2616|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_154_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_154_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_154_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_REFRESH_press.json", "remote_REFRESH_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|173|1", "eventKeyboard|173|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2635|1|", "eventKeyboard|2635|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_155_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_155_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_155_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BOOKMARKS_press.json", "remote_BOOKMARKS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|156|1", "eventKeyboard|156|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2628|1|", "eventKeyboard|2628|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_156_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_156_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_156_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ZOOMIN_press.json", "remote_ZOOMIN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|418|1", "eventKeyboard|418|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2698|1|", "eventKeyboard|2698|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_157_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_157_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_157_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ZOOMOUT_press.json", "remote_ZOOMOUT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|419|1", "eventKeyboard|419|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2699|1|", "eventKeyboard|2699|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_158_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_158_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_158_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_ZOOMRESET_press.json", "remote_ZOOMRESET_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|420|1", "eventKeyboard|420|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2700|1|", "eventKeyboard|2700|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_159_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_159_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_159_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FULL_SCREEN_press.json", "remote_FULL_SCREEN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|372|1", "eventKeyboard|372|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2668|1|", "eventKeyboard|2668|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_160_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_160_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_160_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SCROLLUP_press.json", "remote_SCROLLUP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|177|1", "eventKeyboard|177|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2638|1|", "eventKeyboard|2638|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_161_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_161_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_161_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SCROLLDOWN_press.json", "remote_SCROLLDOWN_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|178|1", "eventKeyboard|178|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2639|1|", "eventKeyboard|2639|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_162_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_162_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_162_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_MINUS_press.json", "remote_MINUS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|12|1", "eventKeyboard|12|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2057|1|", "eventKeyboard|2057|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_163_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_163_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_163_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EDIT_press.json", "remote_EDIT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|176|1", "eventKeyboard|176|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2637|1|", "eventKeyboard|2637|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_164_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_164_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_164_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CANCEL_press.json", "remote_CANCEL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|223|1", "eventKeyboard|223|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2648|1|", "eventKeyboard|2648|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_165_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_165_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_165_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_INSERT_press.json", "remote_INSERT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|110|1", "eventKeyboard|110|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2083|1|", "eventKeyboard|2083|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_166_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_166_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_166_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FORWARD_DEL_press.json", "remote_FORWARD_DEL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|111|1", "eventKeyboard|111|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2071|1|", "eventKeyboard|2071|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_167_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_167_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_167_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_REDO_press.json", "remote_REDO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|182|1", "eventKeyboard|182|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2641|1|", "eventKeyboard|2641|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_168_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_168_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_168_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_REPLY_press.json", "remote_REPLY_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|232|1", "eventKeyboard|232|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2653|1|", "eventKeyboard|2653|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_169_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_169_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_169_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_FORWARDMAIL_press.json", "remote_FORWARDMAIL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|233|1", "eventKeyboard|233|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2654|1|", "eventKeyboard|2654|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_170_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_170_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_170_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SEND_press.json", "remote_SEND_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|231|1", "eventKeyboard|231|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2652|1|", "eventKeyboard|2652|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_171_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_171_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_171_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBD_LAYOUT_NEXT_press.json", "remote_KBD_LAYOUT_NEXT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|584|1", "eventKeyboard|584|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2723|1|", "eventKeyboard|2723|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_172_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_172_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_172_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_PREV_press.json", "remote_KBDINPUTASSIST_PREV_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|608|1", "eventKeyboard|608|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2726|1|", "eventKeyboard|2726|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_173_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_173_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_173_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_NEXT_press.json", "remote_KBDINPUTASSIST_NEXT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|609|1", "eventKeyboard|609|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2727|1|", "eventKeyboard|2727|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_174_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_174_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_174_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_PREVGROUP_press.json", "remote_KBDINPUTASSIST_PREVGROUP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|610|1", "eventKeyboard|610|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2728|1|", "eventKeyboard|2728|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_175_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_175_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_175_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_NEXTGROUP_press.json", "remote_KBDINPUTASSIST_NEXTGROUP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|611|1", "eventKeyboard|611|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2729|1|", "eventKeyboard|2729|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_176_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_176_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_176_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_ACCEPT_press.json", "remote_KBDINPUTASSIST_ACCEPT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|612|1", "eventKeyboard|612|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2730|1|", "eventKeyboard|2730|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_177_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_177_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_177_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDINPUTASSIST_CANCEL_press.json", "remote_KBDINPUTASSIST_CANCEL_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|613|1", "eventKeyboard|613|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2731|1|", "eventKeyboard|2731|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_178_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_178_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_178_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SCALE_press.json", "remote_SCALE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|120|1", "eventKeyboard|120|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2612|1|", "eventKeyboard|2612|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_179_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_179_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_179_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_SLEEP_press.json", "remote_SLEEP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|142|1", "eventKeyboard|142|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2600|1|", "eventKeyboard|2600|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_180_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_180_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_180_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_KBDILLUMTOGGLE_press.json", "remote_KBDILLUMTOGGLE_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|228|1", "eventKeyboard|228|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2649|1|", "eventKeyboard|2649|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_181_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_181_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_181_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_INFO_press.json", "remote_INFO_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|358|1", "eventKeyboard|358|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2664|1|", "eventKeyboard|2664|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_182_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_182_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_182_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_VCR_press.json", "remote_VCR_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|379|1", "eventKeyboard|379|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2674|1|", "eventKeyboard|2674|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_183_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_183_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_183_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EXPLORER_press.json", "remote_EXPLORER_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|150|1", "eventKeyboard|150|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2052|1|", "eventKeyboard|2052|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_184_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_184_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_184_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_EXIT_press.json", "remote_EXIT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|174|1", "eventKeyboard|174|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2636|1|", "eventKeyboard|2636|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_185_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_185_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_185_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_HELP_press.json", "remote_HELP_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|138|1", "eventKeyboard|138|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2625|1|", "eventKeyboard|2625|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_186_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_186_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_186_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_BOOKMARKS_press.json", "remote_BOOKMARKS_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|156|1", "eventKeyboard|156|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2628|1|", "eventKeyboard|2628|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_RemoteControl_Unreg_Untouch_187_R
 * Function: Multi_HandOut_RemoteControl_Unreg_Untouch_187_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutremotecontrolUnRegRTest, Multi_HandOut_RemoteControl_Unreg_Untouch_187_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"remote_CHAT_press.json", "remote_CHAT_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventKeyboard|216|1", "eventKeyboard|216|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventKeyboard|2646|1|", "eventKeyboard|2646|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}
}