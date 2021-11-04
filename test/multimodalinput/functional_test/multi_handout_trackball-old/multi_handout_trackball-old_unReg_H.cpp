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

class multiHandouttrackballoldUnRegHTest : public testing::Test {
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

StClientHelper multiHandouttrackballoldUnRegHTest::st_client_helper_;

void multiHandouttrackballoldUnRegHTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_H");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandouttrackballoldUnRegHTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandouttrackballoldUnRegHTest::SetUp()
{
}

void multiHandouttrackballoldUnRegHTest::TearDown()
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
std::string multiHandouttrackballoldUnRegHTest::ReplaceExpectXY(std::string str,
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
void multiHandouttrackballoldUnRegHTest::DoCompareAndExpect(const struct CaseInfo &ci)
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
 * Feature: Multi_HandOut_Trackball_UnReg_077_H
 * Function: Multi_HandOut_Trackball_UnReg_077_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_077_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_088_H
 * Function: Multi_HandOut_Trackball_UnReg_088_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_088_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_099_H
 * Function: Multi_HandOut_Trackball_UnReg_099_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_099_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_110_H
 * Function: Multi_HandOut_Trackball_UnReg_110_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_110_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_121_H
 * Function: Multi_HandOut_Trackball_UnReg_121_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_121_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventPointer|272|1", "eventPointer|-45|60", "eventPointer|-15|30",
                             "eventPointer|272|0"};
    ci.standardExpectList = {"402|500|272|1|0|0", "400|502|272|1|-45|60", "400|502|272|1|-15|30", "402|501|272|0|0|0"};
    ci.expectL3Event = {"eventPointer|1|1|", "eventPointer|-45|60|", "eventPointer|-15|30|",
                        "eventPointer|1|0|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_Trackball_UnReg_132_H
 * Function: Multi_HandOut_Trackball_UnReg_132_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_132_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
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
 * Feature: Multi_HandOut_Trackball_UnReg_143_H
 * Function: Multi_HandOut_Trackball_UnReg_143_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_143_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
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
 * Feature: Multi_HandOut_Trackball_UnReg_154_H
 * Function: Multi_HandOut_Trackball_UnReg_154_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_154_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
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
 * Feature: Multi_HandOut_Trackball_UnReg_175_H
 * Function: Multi_HandOut_Trackball_UnReg_175_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_175_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
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
 * Feature: Multi_HandOut_Trackball_UnReg_191_H
 * Function: Multi_HandOut_Trackball_UnReg_191_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_191_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"", "", "", ""};
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
 * Feature: Multi_HandOut_Trackball_UnReg_208_H
 * Function: Multi_HandOut_Trackball_UnReg_208_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_208_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_218_H
 * Function: Multi_HandOut_Trackball_UnReg_218_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_218_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_228_H
 * Function: Multi_HandOut_Trackball_UnReg_228_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_228_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_238_H
 * Function: Multi_HandOut_Trackball_UnReg_238_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_238_H, TestSize.Level0)
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
 * Feature: Multi_HandOut_Trackball_UnReg_248_H
 * Function: Multi_HandOut_Trackball_UnReg_248_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttrackballoldUnRegHTest, Multi_HandOut_Trackball_UnReg_248_H, TestSize.Level0)
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
}