/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "client_msg_handler.h"
#include "event_factory.h"
#include "mmi_client.h"
#include "proto.h"
#include "struct_multimodal.h"

namespace {
using namespace testing::ext;
using namespace OHOS::MMI;
using namespace OHOS;
using TestMMIClient = OHOS::MMI::MMIClient;

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ClientMsgHandlerTest" }; // namepace

class ClientMsgHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

class ClientMsgHandlerSelf : public OHOS::MMI::ClientMsgHandler {
public:

    bool OnKeyUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnKey(udsClient, pkt);
    }
    bool OnTouchUnitTest(INPUT_DEVICE_TYPE msgId)
    {
        TestMMIClient clientObj;
        NetPacket pkt(MmiMessageId::ON_TOUCH);
        pkt << msgId;
        const uint32_t bitLength = 2;

        if (msgId == INPUT_DEVICE_CAP_POINTER) {
            pkt << bitLength;
        }
        return OnTouch(clientObj, pkt);
    }
    bool OnTouchStandardUnitTest(INPUT_DEVICE_TYPE msgId, int32_t  curReventType)
    {
        TestMMIClient clientObj;
        NetPacket pkt(MmiMessageId::ON_TOUCH);
        pkt << msgId << curReventType;
        const uint32_t bitLength = 2;

        if (msgId == INPUT_DEVICE_CAP_POINTER) {
            pkt << bitLength;
        }
        return OnTouch(clientObj, pkt);
    }

    bool OnTouchUnitTest2(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnTouch(udsClient, pkt);
    }

    bool OnCopyUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnCopy(udsClient, pkt);
    }

    bool OnShowMenuUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnShowMenu(udsClient, pkt);
    }

    bool OnSendUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnSend(udsClient, pkt);
    }

    bool OnPasteUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnPaste(udsClient, pkt);
    }

    bool OnCutUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnCut(udsClient, pkt);
    }

    bool OnUndoUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnUndo(udsClient, pkt);
    }

    bool OnRefreshUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnRefresh(udsClient, pkt);
    }

    bool OnStartDragUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnStartDrag(udsClient, pkt);
    }

    bool OnCancelUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnCancel(udsClient, pkt);
    }

    bool OnEnterUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnEnter(udsClient, pkt);
    }

    bool OnPreviousUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnPrevious(udsClient, pkt);
    }

    bool OnNextUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnNext(udsClient, pkt);
    }

    bool OnBackUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnBack(udsClient, pkt);
    }

    bool OnPrintUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnPrint(udsClient, pkt);
    }

    bool OnPlayUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnPlay(udsClient, pkt);
    }

    bool OnPauseUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnPause(udsClient, pkt);
    }

    bool OnMediaControlUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnMediaControl(udsClient, pkt);
    }

    bool OnScreenShotUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnScreenShot(udsClient, pkt);
    }

    bool OnScreenSplitUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnScreenSplit(udsClient, pkt);
    }

    bool OnStartScreenRecordUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnStartScreenRecord(udsClient, pkt);
    }

    bool OnStopScreenRecordUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnStopScreenRecord(udsClient, pkt);
    }

    bool OnGotoDesktopUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnGotoDesktop(udsClient, pkt);
    }

    bool OnRecentUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnRecent(udsClient, pkt);
    }

    bool OnShowNotificationUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnShowNotification(udsClient, pkt);
    }

    bool OnLockScreenUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnLockScreen(udsClient, pkt);
    }

    bool OnSearchUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnSearch(udsClient, pkt);
    }

    bool OnClosePageUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnClosePage(udsClient, pkt);
    }

    bool OnLaunchVoiceAssistantUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnLaunchVoiceAssistant(udsClient, pkt);
    }

    bool OnMuteUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnMute(udsClient, pkt);
    }

    bool OnAnswerUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnAnswer(udsClient, pkt);
    }

    bool OnRefuseUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnRefuse(udsClient, pkt);
    }

    bool OnHangupUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnHangup(udsClient, pkt);
    }

    bool OnTelephoneControlUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return OnTelephoneControl(udsClient, pkt);
    }

    bool GetMultimodeInputInfoUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return GetMultimodeInputInfo(udsClient, pkt);
    }

    bool DeviceAddUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return DeviceAdd(udsClient, pkt);
    }

    bool DeviceRemoveUnitTest(const UDSClient& udsClient, NetPacket& pkt)
    {
        return DeviceRemove(udsClient, pkt);
    }
};

HWTEST_F(ClientMsgHandlerTest, Init, TestSize.Level1)
{
    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONKEY_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONKEY_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "key_board" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONKEY_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);
    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "key_board" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONKEY_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONKEY_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = {1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "key_board"};
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTOUCH_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTOUCH_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t typeNum = INPUT_DEVICE_CAP_TOUCH;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 500, 10000, static_cast<DEVICE_TYPE>(0), "touchscreen" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTOUCH_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t typeNum = INPUT_DEVICE_CAP_TOUCH;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 500, 10000, static_cast<DEVICE_TYPE>(0), "touchscreen" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTOUCH_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t typeNum = INPUT_DEVICE_CAP_TOUCH;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTOUCH_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t typeNum = INPUT_DEVICE_CAP_TOUCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 500, 10000, static_cast<DEVICE_TYPE>(0), "touchscreen" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCOPY_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCOPY_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCOPY_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCOPY_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCOPY_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWMENU_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWMENU_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 402, 10000, static_cast<DEVICE_TYPE>(2), "mouse" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWMENU_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWMENU_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWMENU_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 402, 10000, static_cast<DEVICE_TYPE>(2), "mouse" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEND_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEND_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEND_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEND_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEND_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPASTE_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPASTE_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPASTE_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPASTE_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPASTE_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCUT_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCUT_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCUT_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCUT_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCUT_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONUNDO_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONUNDO_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONUNDO_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> mixData >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId << serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONUNDO_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    int32_t typeNum = INPUT_DEVICE_CAP_KEYBOARD;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;
    pkt >> typeNum >> idMsg >> aiDeviceFd >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONUNDO_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONREFRESH, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_REFRESH);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSTARTDRAG, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_START_DRAG);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCANCEL, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CANCEL);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONENTER, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_ENTER);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPREVIOUS, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PREVIOUS);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONNEXT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_NEXT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONBACK, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_BACK);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPRINT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PRINT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPLAY, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PLAY);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONPAUSE, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PAUSE);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONMEDIACONTROL, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_MEDIA_CONTROL);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSCREENSHOT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SCREEN_SHOT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSCREENSPLIT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SCREEN_SPLIT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSTARTSCREENRECORD, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_START_SCREEN_RECORD);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSTOPSCREENRECORD, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_STOP_SCREEN_RECORD);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONGOTODESKTOP, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_GOTO_DESKTOP);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONRECENT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_RECENT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSHOWNOTIFICATION, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_NOTIFICATION);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONLOCKSCREEN, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_LOCK_SCREEN);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONSEARCH, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEARCH);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONCLOSEPAGE, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CLOSE_PAGE);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONLAUNCHVOICEASSISTANT, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_LAUNCH_VOICE_ASSISTANT);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONMUTE, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_MUTE);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONANSWER, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_ANSWER);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONREFUSE, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_REFUSE);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONHANGUP, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_HANG_UP);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONTELEPHONECONTROL, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TELEPHONE_CONTROL);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}
HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_ONJOYSTICK, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_JOYSTICK);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMsgHandler_GETMMIINFO_ACK, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::GET_MMI_INFO_ACK);

    ClientMsgHandler clientHandlerObj;
    clientHandlerObj.Init();
    clientHandlerObj.OnMsgHandler(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnTouch_keyboard, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_KEYBOARD);
}

/**
 * @tc.name: OnTouch_pointer
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_POINTER
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_pointer_001, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_POINTER);
}

/**
 * @tc.name: OnTouch_pointer
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_POINTER
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */

HWTEST_F(ClientMsgHandlerTest, OnTouch_pointer_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t abilityId = 0;
    int32_t screenId = 0;
    int32_t fileData = 0;
    int32_t retResult = RET_ERR;
    uint64_t serverStartTime = 0;
    EventPointer pointData = {};
    pkt << INPUT_DEVICE_CAP_POINTER;
    pkt << retResult << pointData << abilityId << screenId << fileData << serverStartTime;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest2(clientObj, pkt);
}

/**
 * @tc.name: OnTouch_touch
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_TOUCH
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_touch, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_TOUCH);
}

/**
 * @tc.name: OnTouch_tabletTool
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_TABLET_TOOL
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_tabletTool, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_TABLET_TOOL);
}

HWTEST_F(ClientMsgHandlerTest, OnTouch_StandardtabletPad, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchStandardUnitTest(INPUT_DEVICE_CAP_TABLET_TOOL, 1);
}

/**
 * @tc.name: OnTouch_tabletPad
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_TABLET_PAD
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_tabletPad, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_TABLET_PAD);
}

/**
 * @tc.name: OnTouch_capGesture
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_GESTURE
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_capGesture, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_GESTURE);
}

/**
 * @tc.name: OnTouch_capSwitch
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_SWITCH
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_capSwitch, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_SWITCH);
}

/**
 * @tc.name: OnTouch_capJoystick
 * @tc.desc: detection create event:  INPUT_DEVICE_CAP_JOYSTICK
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnTouch_capJoystick, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_JOYSTICK);
}

HWTEST_F(ClientMsgHandlerTest, OnTouch2, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TOUCH);

    int32_t typeNum = INPUT_DEVICE_CAP_TOUCH;
    int32_t dataSize = 3;
    int32_t abilityId = 4;
    int32_t screenId = 5;
    int32_t fileData = 6;
    int64_t serverStartTime = 7;
    EventTouch touchData = {};
    int32_t ret = memcpy_s(touchData.deviceName, MAX_DEVICENAME, "name", MAX_DEVICENAME);
    CHK(ret == EOK, MEMCPY_SEC_FUN_FAIL);
    ret = memcpy_s(touchData.physical, MAX_DEVICENAME, "TouchScreen", MAX_DEVICENAME);
    CHK(ret == EOK, MEMCPY_SEC_FUN_FAIL);
    ret = memcpy_s(touchData.uuid, MAX_DEVICENAME, "12345", MAX_DEVICENAME);
    CHK(ret == EOK, MEMCPY_SEC_FUN_FAIL);
    touchData.eventType = 500;
    touchData.time = 500;
    touchData.slot = 500;
    touchData.seatSlot = 500;
    touchData.deviceType = static_cast<DEVICE_TYPE>(0);

    pkt << typeNum;
    pkt << dataSize << abilityId << screenId << fileData << serverStartTime;
    pkt << touchData;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest2(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnTouch_capTouchPad, TestSize.Level1)
{
    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTouchUnitTest(INPUT_DEVICE_CAP_TOUCH_PAD);
}

/**
 * @tc.name: DeviceAdd
 * @tc.desc: detection create event:
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, OnKey_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_KEY);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnKeyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCopy_001, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCopyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCopy_002, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_JOYSTICK;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 403, 10000, static_cast<DEVICE_TYPE>(7), "JOYSTICK" };

    pkt << mixData;
    pkt << typeNum;
    pkt << idMsg << aiDeviceFd << fileData << windowId << abilityId << serverStartTime;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCopyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCopy_003, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_AISENSOR;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    int64_t serverStartTime = 0;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(9), "aisensor" };

    pkt << mixData;
    pkt << typeNum;
    pkt << idMsg << aiDeviceFd << fileData << windowId << abilityId << serverStartTime;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCopyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCopy_004, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_AISENSOR;
    int16_t idMsg = 3;
    int32_t aiDeviceFd = 4;
    int32_t fileData = 5;
    int32_t windowId = -1;
    int32_t abilityId = 7;
    uint64_t serverStartTime = 0;

    pkt << typeNum;
    pkt << idMsg << aiDeviceFd << fileData << windowId << abilityId << serverStartTime;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCopyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCopy_005, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_COPY);

    int32_t typeNum = INPUT_DEVICE_CAP_SWITCH;
    int32_t fileData = 11;
    int32_t windowId = -1;
    int32_t abilityId = 33;
    int64_t serverStartTime = 44;
    RegisteredEvent mixData = { 1, "abc123@34", 300, 10000, static_cast<DEVICE_TYPE>(1), "keyboard" };
    pkt >> typeNum;
    pkt >> mixData >> fileData >> windowId >> abilityId >> serverStartTime;

    pkt << typeNum;
    pkt << mixData << fileData << windowId << abilityId << serverStartTime;

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCopyUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnShowMenu, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_MENU);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnShowMenuUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnSend, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEND);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnSendUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnPaste, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PASTE);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnPasteUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCut, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CUT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCutUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnUndo, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_UNDO);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnUndoUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnRefresh, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_REFRESH);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnRefreshUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnStartDrag, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_START_DRAG);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnStartDragUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnCancel, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CANCEL);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnCancelUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnEnter, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_ENTER);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnEnterUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnPrevious, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PREVIOUS);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnPreviousUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnNext, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_NEXT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnNextUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnBack, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_BACK);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnBackUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnPrint, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PRINT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnPrintUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnPlay, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PLAY);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnPlayUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnPause, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_PAUSE);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnPauseUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMediaControl, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_MEDIA_CONTROL);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnMediaControlUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnScreenShot, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SCREEN_SHOT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnScreenShotUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnScreenSplit, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SCREEN_SPLIT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnScreenSplitUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnStartScreenRecord, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_START_SCREEN_RECORD);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnStartScreenRecordUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnStopScreenRecord, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_STOP_SCREEN_RECORD);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnStopScreenRecordUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnGotoDesktop, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_GOTO_DESKTOP);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnGotoDesktopUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnRecent, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_RECENT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnRecentUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnShowNotification, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SHOW_NOTIFICATION);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnShowNotificationUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnLockScreen, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_LOCK_SCREEN);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnLockScreenUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnSearch, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_SEARCH);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnSearchUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnClosePage, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_CLOSE_PAGE);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnClosePageUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnLaunchVoiceAssistant, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_LAUNCH_VOICE_ASSISTANT);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnLaunchVoiceAssistantUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnMute, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_MUTE);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnMuteUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnAnswer, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_ANSWER);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnAnswerUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnRefuse, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_REFUSE);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnRefuseUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnHangup, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_HANG_UP);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnHangupUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, OnTelephoneControl, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_TELEPHONE_CONTROL);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.OnTelephoneControlUnitTest(clientObj, pkt);
}

HWTEST_F(ClientMsgHandlerTest, GetMultimodeInputInfo, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::GET_MMI_INFO_ACK);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.GetMultimodeInputInfoUnitTest(clientObj, pkt);
}

/**
 * @tc.name: DeviceAdd
 * @tc.desc: detection create event:
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, DeviceAdd, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_DEVICE_ADDED);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.DeviceAddUnitTest(clientObj, pkt);
}

/**
 * @tc.name: DeviceRemove
 * @tc.desc: detection create event:
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(ClientMsgHandlerTest, DeviceRemove, TestSize.Level1)
{
    TestMMIClient clientObj;
    NetPacket pkt(MmiMessageId::ON_DEVICE_REMOVED);

    ClientMsgHandlerSelf clientMsgHandlerSelf;
    clientMsgHandlerSelf.Init();
    clientMsgHandlerSelf.DeviceRemoveUnitTest(clientObj, pkt);
}
} // namespace
