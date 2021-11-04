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

class multiHandoutmouseUnRegRTest : public testing::Test {
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

StClientHelper multiHandoutmouseUnRegRTest::st_client_helper_;

void multiHandoutmouseUnRegRTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_R");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandoutmouseUnRegRTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandoutmouseUnRegRTest::SetUp()
{
}

void multiHandoutmouseUnRegRTest::TearDown()
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
std::string multiHandoutmouseUnRegRTest::ReplaceExpectXY(std::string str,
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
void multiHandoutmouseUnRegRTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_001_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Ctrl_press.json", "mouse_btn_left_press.json", "mouse_btn_left_release.json", "keyboard_Ctrl_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventPointer|272|1", "eventPointer|272|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "402|500|272|1|0|0", "402|501|272|0|0|0", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventKeyboard|2072|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_002_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Shift_press.json", "mouse_btn_left_press.json", "mouse_btn_left_release.json", "keyboard_Shift_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|42|1", "eventPointer|272|1", "eventPointer|272|0",
                             "eventKeyboard|42|0"};
    ci.standardExpectList = {"/", "402|500|272|1|0|0", "402|501|272|0|0|0", "/"};
    ci.expectL3Event = {"eventKeyboard|2047|1|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventKeyboard|2047|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_003_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_003_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_003_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_left_longpress.json", "mouse_btn_right_press.json", "mouse_btn_right_release.json", "mouse_btn_left_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "/", "/", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|1|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_004_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_004_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_004_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_left_longpress.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_left_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|274|1", "eventPointer|274|0",
                             "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "/", "/", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|4|1|", "eventPointer|4|0|",
                        "eventPointer|1|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_005_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_005_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_005_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_right_longpress.json", "mouse_btn_left_press.json", "mouse_btn_left_release.json", "mouse_btn_right_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|272|1", "eventPointer|272|0",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"/", "402|500|272|1|0|0", "402|501|272|0|0|0", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|1|1|", "eventPointer|1|0|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_006_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_006_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_006_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_left_press.json", "mouse_btn_right_press.json", "mouse_btn_left_release.json", "mouse_btn_right_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|273|1", "eventPointer|272|0",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "/", "402|501|272|0|0|0", "/"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|2|1|", "eventPointer|1|0|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_007_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_007_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_007_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_left_press.json", "mouse_btn_middle_press.json", "mouse_btn_left_release.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|274|1", "eventPointer|272|0",
                             "eventPointer|274|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "/", "402|501|272|0|0|0", "/"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|4|1|", "eventPointer|1|0|",
                        "eventPointer|4|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_008_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_008_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_008_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_left_press.json", "mouse_btn_right_press.json", "mouse_btn_middle_press.json", "mouse_btn_left_release.json",
                    "mouse_btn_right_release.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|273|1", "eventPointer|274|1",
                             "eventPointer|272|0", "eventPointer|273|0", "eventPointer|274|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "/", "/", "402|501|272|0|0|0",
                             "/", "/"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|2|1|", "eventPointer|4|1|",
                        "eventPointer|1|0|", "eventPointer|2|0|", "eventPointer|4|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_022_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_022_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_022_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_middle_longpress.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|274|1", "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|4|1|", "eventPointer|4|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_027_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_027_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_027_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_move_1.json", "mouse_move_2.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|-35|40", "eventPointer|-15|60"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|-35|40|", "eventPointer|-15|60|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_032_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_032_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_032_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Ctrl_press.json", "mouse_wheel_up.json", "keyboard_Ctrl_release.json"};
    ci.deviceMsg = {"", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventPointer|0|-30", "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventPointer|0|-30|", "eventKeyboard|2072|0|"};
    ci.dispatcherList = {"", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_037_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_037_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_037_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Ctrl_press.json", "mouse_wheel_down.json", "keyboard_Ctrl_release.json"};
    ci.deviceMsg = {"", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventPointer|0|30", "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventPointer|0|30|", "eventKeyboard|2072|0|"};
    ci.dispatcherList = {"", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_042_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_042_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_042_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_side_press.json", "mouse_btn_side_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|275|1", "eventPointer|275|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|32|1|", "eventPointer|32|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_047_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_047_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_047_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_extra_press.json", "mouse_btn_extra_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|276|1", "eventPointer|276|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|64|1|", "eventPointer|64|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_052_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_052_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_052_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_forward_press.json", "mouse_btn_forward_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|277|1", "eventPointer|277|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|16|1|", "eventPointer|16|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_057_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_057_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_057_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_back_press.json", "mouse_btn_back_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|278|1", "eventPointer|278|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|8|1|", "eventPointer|8|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_067_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_067_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_067_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json",
                    "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json",
                    "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json",
                    "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json",
                    "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", "",
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|274|1", "eventPointer|274|0", "eventPointer|274|1",
                             "eventPointer|274|0", "eventPointer|274|1", "eventPointer|274|0",
                             "eventPointer|274|1", "eventPointer|274|0", "eventPointer|274|1",
                             "eventPointer|274|0", "eventPointer|274|1", "eventPointer|274|0",
                             "eventPointer|274|1", "eventPointer|274|0", "eventPointer|274|1",
                             "eventPointer|274|0", "eventPointer|274|1", "eventPointer|274|0",
                             "eventPointer|274|1", "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/",
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|4|1|", "eventPointer|4|0|", "eventPointer|4|1|",
                        "eventPointer|4|0|", "eventPointer|4|1|", "eventPointer|4|0|",
                        "eventPointer|4|1|", "eventPointer|4|0|", "eventPointer|4|1|",
                        "eventPointer|4|0|", "eventPointer|4|1|", "eventPointer|4|0|",
                        "eventPointer|4|1|", "eventPointer|4|0|", "eventPointer|4|1|",
                        "eventPointer|4|0|", "eventPointer|4|1|", "eventPointer|4|0|",
                        "eventPointer|4|1|", "eventPointer|4|0|"};
    ci.dispatcherList = {"", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", "",
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_082_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_082_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_082_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_middle_longpress.json", "mouse_move_1.json", "mouse_move_2.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|274|1", "eventPointer|-35|40", "eventPointer|-15|60",
                             "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|4|1|", "eventPointer|-35|40|", "eventPointer|-15|60|",
                        "eventPointer|4|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_107_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_107_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_107_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_middle_press.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventPointer|274|1", "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"eventPointer|4|1|", "eventPointer|4|0|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_112_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_112_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_112_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_wheel_up.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|0|-30"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|0|-30|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_117_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_117_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_117_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_wheel_down.json"};
    ci.deviceMsg = {""};    
    ci.libinputExpectList = {"eventPointer|0|30"};
    ci.standardExpectList = {"/"};
    ci.expectL3Event = {"eventPointer|0|30|"};
    ci.dispatcherList = {""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_172_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_172_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_172_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Ctrl_press.json", "mouse_btn_right_press.json", "mouse_btn_right_release.json", "keyboard_Ctrl_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|29|1", "eventPointer|273|1", "eventPointer|273|0",
                             "eventKeyboard|29|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2072|1|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventKeyboard|2072|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_173_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_173_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_173_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"keyboard_Shift_press.json", "mouse_btn_right_press.json", "mouse_btn_right_release.json", "keyboard_Shift_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventKeyboard|42|1", "eventPointer|273|1", "eventPointer|273|0",
                             "eventKeyboard|42|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventKeyboard|2047|1|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventKeyboard|2047|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_174_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_174_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_174_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_right_longpress.json", "mouse_btn_middle_press.json", "mouse_btn_middle_release.json", "mouse_btn_right_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|274|1", "eventPointer|274|0",
                             "eventPointer|273|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|4|1|", "eventPointer|4|0|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_Unreg_Untouch_175_R
 * Function: Multi_HandOut_Mouse_Unreg_Untouch_175_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_Unreg_Untouch_175_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_btn_right_press.json", "mouse_btn_middle_press.json", "mouse_btn_right_release.json", "mouse_btn_middle_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|274|1", "eventPointer|273|0",
                             "eventPointer|274|0"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|4|1|", "eventPointer|2|0|",
                        "eventPointer|4|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_UnReg_Unusual_001_R
 * Function: Multi_HandOut_Mouse_UnReg_Unusual_001_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_UnReg_Unusual_001_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"non_mouse_press.json", "non_mouse_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"NULL|NULL|NULL ", "NULL|NULL|NULL "};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"NULL|NULL|NULL |", "NULL|NULL|NULL |"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Mouse_UnReg_Unusual_002_R
 * Function: Multi_HandOut_Mouse_UnReg_Unusual_002_R
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandoutmouseUnRegRTest, Multi_HandOut_Mouse_UnReg_Unusual_002_R, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json",
                    "mouse_right_btn_press.json", "mouse_right_btn_release.json", "mouse_right_btn_press.json", "mouse_right_btn_release.json"};
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
                             "", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0", "eventPointer|273|1", "eventPointer|273|0",
                             "eventPointer|273|1", "eventPointer|273|0", "eventPointer|273|1",
                             "eventPointer|273|0"};
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
                             "/", "/", "/", "/"};
    ci.expectL3Event = {"eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|", "eventPointer|2|1|", "eventPointer|2|0|",
                        "eventPointer|2|1|", "eventPointer|2|0|", "eventPointer|2|1|",
                        "eventPointer|2|0|"};
    ci.dispatcherList = {"", "", "", "",
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
                         "", "", "", ""};
    DoCompareAndExpect(ci);
}
}