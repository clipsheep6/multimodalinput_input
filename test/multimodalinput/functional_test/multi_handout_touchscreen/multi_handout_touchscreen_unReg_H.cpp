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

class multiHandouttouchscreenUnRegHTest : public testing::Test {
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

StClientHelper multiHandouttouchscreenUnRegHTest::st_client_helper_;

void multiHandouttouchscreenUnRegHTest::SetUpTestCase()
{
    GetSTHelper().InitClient("Scene_H");
    st_client_helper_.StartStClient();
    sleep(1);
}

void multiHandouttouchscreenUnRegHTest::TearDownTestCase(void)
{
    st_client_helper_.StopStClient();
}

void multiHandouttouchscreenUnRegHTest::SetUp()
{
}

void multiHandouttouchscreenUnRegHTest::TearDown()
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
std::string multiHandouttouchscreenUnRegHTest::ReplaceExpectXY(std::string str,
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
void multiHandouttouchscreenUnRegHTest::DoCompareAndExpect(const struct CaseInfo &ci)
{
    for (unsigned int i = 0; i < ci.eventList.size(); i++) {
        GetSTHelper().InJectionEvent(ci.eventList[i], ci.deviceMsg[i]);
        std::string ExpectServerResult = "NULL|NULL|NULL |";
        std::string ExpectClientResult = "NULL|NULL|NULL |";
		struct AutoTestCoordinate autoTestCoordinate = GetDataProcess().GetCoordinate();
        if (ci.libinputExpectList[i] != "eventJoyStickAxis") {
            EXPECT_EQ(GetDataProcess().GetLibinputValue_touchScreen(), ReplaceExpectXY(ci.libinputExpectList[i],
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
        EXPECT_EQ(GetDataProcess().GetActualServerResult_touchScreen(), ReplaceExpectXY(ExpectServerResult, autoTestCoordinate));
        EXPECT_EQ(GetDataProcess().GetActualClientResult(), ReplaceExpectXY(ExpectClientResult, autoTestCoordinate));
        EXPECT_EQ((ci.dispatcherList[i] == GetDataProcess().GetDispatcherStr()) ||
                  (ci.dispatcherList[i].find(GetDataProcess().GetDispatcherStr()) != ci.dispatcherList[i].npos),
                  true);
        GetDataProcess().ClearAll();
    }
}

/*
 * Feature: Multi_HandOut_touch_001_H
 * Function: Multi_HandOut_touch_001_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_001_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_singlefinger_press.json", "touch_singlefinger_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|0|<X>|<Y>|", "501|0|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_002_H
 * Function: Multi_HandOut_touch_002_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_002_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_singlefinger_longpress.json", "touch_singlefinger_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|0|<X>|<Y>|", "501|0|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_003_H
 * Function: Multi_HandOut_touch_003_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_003_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_singlefinger_up_press.json", "touch_singlefinger_up_move_1.json", "touch_singlefinger_up_move_2.json", "touch_singlefinger_up_move_3.json",
                    "touch_singlefinger_up_move_4.json", "touch_singlefinger_up_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"500|0|<X>|<Y>|", "502|0|<X>|<Y>|", "502|0|<X>|<Y>|",
                        "502|0|<X>|<Y>|", "502|0|<X>|<Y>|", "501|0|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_004_H
 * Function: Multi_HandOut_touch_004_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_004_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_singlefinger_down_press.json", "touch_singlefinger_down_move_1.json", "touch_singlefinger_down_move_2.json", "touch_singlefinger_down_move_3.json",
                    "touch_singlefinger_down_move_4.json", "touch_singlefinger_down_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"500|0|<X>|<Y>|", "502|0|<X>|<Y>|", "502|0|<X>|<Y>|",
                        "502|0|<X>|<Y>|", "502|0|<X>|<Y>|", "501|0|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_005_H
 * Function: Multi_HandOut_touch_005_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_005_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_singlefinger_longpress.json", "touch_singlefinger_move_1.json", "touch_singlefinger_move_2.json", "touch_singlefinger_move_3.json",
                    "touch_singlefinger_move_4.json", "touch_singlefinger_release.json"};
    ci.deviceMsg = {"", "", "", "",
                             "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/",
                             "/", "/"};
    ci.expectL3Event = {"500|0|<X>|<Y>|", "502|0|<X>|<Y>|", "502|0|<X>|<Y>|",
                        "502|0|<X>|<Y>|", "502|0|<X>|<Y>|", "501|0|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", "",
                         "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_006_H
 * Function: Multi_HandOut_touch_006_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_006_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_press.json", "touch_twofingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_007_H
 * Function: Multi_HandOut_touch_007_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_007_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_longpress.json", "touch_twofingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_008_H
 * Function: Multi_HandOut_touch_008_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_008_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_press.json", "touch_twofingers_move.json", "touch_twofingers_release.json"};
    ci.deviceMsg = {"", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "502|1|<X>|<Y>|", "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_009_H
 * Function: Multi_HandOut_touch_009_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_009_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_press.json", "touch_twofingers_move.json", "touch_twofingers_release.json"};
    ci.deviceMsg = {"", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "502|1|<X>|<Y>|", "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_010_H
 * Function: Multi_HandOut_touch_010_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_010_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_down_press.json", "touch_twofingers_down_move1.json", "touch_twofingers_down_move2.json", "touch_twofingers_down_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "502|1|<X>|<Y>|", "502|1|<X>|<Y>|",
                        "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_011_H
 * Function: Multi_HandOut_touch_011_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_011_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_twofingers_long_press.json", "touch_twofingers_move1.json", "touch_twofingers_move2.json", "touch_twofingers_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"500|1|<X>|<Y>|", "502|1|<X>|<Y>|", "502|1|<X>|<Y>|",
                        "501|1|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_012_H
 * Function: Multi_HandOut_touch_012_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_012_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_threefingers_press.json", "touch_threefingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|2|<X>|<Y>|", "501|2|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_013_H
 * Function: Multi_HandOut_touch_013_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_013_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_threefingers_long_press.json", "touch_threefingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|2|<X>|<Y>|", "501|2|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_014_H
 * Function: Multi_HandOut_touch_014_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_014_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_threefingers_up_press.json", "touch_threefingers_up_move.json", "touch_threefingers_up_release.json"};
    ci.deviceMsg = {"", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/"};
    ci.expectL3Event = {"500|2|<X>|<Y>|", "502|2|<X>|<Y>|", "501|2|<X>|<Y>|"};
    ci.dispatcherList = {"", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_015_H
 * Function: Multi_HandOut_touch_015_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_015_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_fourfingers_press.json", "touch_fourfingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|3|<X>|<Y>|", "501|3|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_016_H
 * Function: Multi_HandOut_touch_016_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_016_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_fivefingers_press.json", "touch_fivefingers_release.json"};
    ci.deviceMsg = {"", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/"};
    ci.expectL3Event = {"500|4|<X>|<Y>|", "501|4|<X>|<Y>|"};
    ci.dispatcherList = {"", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_017_H
 * Function: Multi_HandOut_touch_017_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_017_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_fourfingers_up_press.json", "touch_fourfingers_up_move1.json", "touch_fourfingers_up_move2.json", "touch_fourfingers_up_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"500|3|<X>|<Y>|", "502|3|<X>|<Y>|", "502|3|<X>|<Y>|",
                        "501|3|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}

/*
 * Feature: Multi_HandOut_touch_018_H
 * Function: Multi_HandOut_touch_018_H
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: set log print level
 */
HWTEST_F(multiHandouttouchscreenUnRegHTest, Multi_HandOut_touch_018_H, TestSize.Level0)
{
    struct CaseInfo ci = {{""}, {""}, {""}, {""}, {""}, {""}};
    ci.eventList = {"touch_fivefingers_up_press.json", "touch_fivefingers_up_move1.json", "touch_fivefingers_up_move2.json", "touch_fivefingers_up_release.json"};
    ci.deviceMsg = {"", "", "", ""};    
    ci.libinputExpectList = {"eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>", "eventTouch|<X>|<Y>",
                             "eventTouch|<X>|<Y>"};
    ci.standardExpectList = {"/", "/", "/", "/"};
    ci.expectL3Event = {"500|4|<X>|<Y>|", "502|4|<X>|<Y>|", "502|4|<X>|<Y>|",
                        "501|4|<X>|<Y>|"};
    ci.dispatcherList = {"", "", "", ""};
    DoCompareAndExpect(ci);
}
}