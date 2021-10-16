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
#include "send_message.h"
#include "proto.h"
#include "test_aux_tool_client.h"
#include "sys/file.h"

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "SendMessage" };
}

/*
 * It is used for the distribution of
 * write command, including keyboard,
 * mouse, rocker, touch pad, etc...
 */
SendMessage::SendMessage()
{
    Init();
}

void SendMessage::Init()
{
    WriteDeviceFunMap funs[] = {
        {KEY_EVENT, bind(&SendMessage::SimulateKeyboard, this, std::placeholders::_1)},
        {KEY_LONG_EVENT, bind(&SendMessage::SimulateKeyboardLongPress, this, std::placeholders::_1)},
        {MOUSE_MOVE_EVENT, bind(&SendMessage::SimulateMouseMove, this, std::placeholders::_1)},
        {MOUSE_MOVE_WHEEL_EVENT, bind(&SendMessage::SimulateMouseWheel, this, std::placeholders::_1)},
        {MOUSE_MOVE_HWHEEL_EVENT, bind(&SendMessage::SimulateMouseHwheel, this, std::placeholders::_1)},
        {XBOX_RIGHT_KEY, bind(&SendMessage::xBoxKey, this, std::placeholders::_1)},
        {XBOX_ABS_KEY, bind(&SendMessage::xBoxHat0y, this, std::placeholders::_1)},
        {XBOX_LOWERRIGHT_ABSRXY, bind(&SendMessage::xBoxAbsRxy, this, std::placeholders::_1)},
        {XBOX_TOPLEFT_ABSRXY, bind(&SendMessage::xBoxAbs, this, std::placeholders::_1)},
        {XBOX_ABSRZ, bind(&SendMessage::xBoxAbsRz, this, std::placeholders::_1)},
        {ROCKER_KEY, bind(&SendMessage::SimulateRockerKey, this, std::placeholders::_1)},
        {ROCKER_HAT0Y, bind(&SendMessage::SimulateRockerHat0y, this, std::placeholders::_1)},
        {ROCKER_ABSRZ, bind(&SendMessage::SimulateRockerAbsrz, this, std::placeholders::_1)},
        {ROCKER_ABS, bind(&SendMessage::SimulateRockerAbs, this, std::placeholders::_1)},
        {DELAY_KEY_EVENT, bind(&SendMessage::SimulateKeyDelayEvent, this, std::placeholders::_1)},
        {TOUCH_PRESS_EVENT, bind(&SendMessage::SimulateTouchPress, this, std::placeholders::_1)},
        {TOUCH_RELEASE_EVENT, bind(&SendMessage::SimulateTouchReless, this, std::placeholders::_1)},
        {TOUCH_MOVE_EVENT, bind(&SendMessage::SimulateTouchMove, this, std::placeholders::_1)},
        {TOUCH_PAD_KEY_EVENT, bind(&SendMessage::SimulateTouchPadKeyEvent, this, std::placeholders::_1)},
        {TOUCH_PAD_RING_PRESS_EVENT, bind(&SendMessage::SimulateTouchPadRingPressEvent, this, std::placeholders::_1)},
        {TOUCH_PAD_RING_MOVE_EVENT, bind(&SendMessage::SimulateTouchPadRingMoveEvent, this, std::placeholders::_1)},
        {TOUCH_PAD_RING_RELEASE_EVENT, bind(&SendMessage::SimulateTouchPadRingReleaseEvent, this,
                                            std::placeholders::_1)},
    };

    for (auto& it : funs) {
        CHKC(RegistSendEvent(it), EVENT_REG_FAIL);
    }
}

int32_t SendMessage::WriteDevice(InputEvent& inputEvent)
{
    auto iter = mapFuns_.find(inputEvent.type);
    if (iter == mapFuns_.end()) {
        MMI_LOGE("writeDevice not find target function: error targetType = %{public}d", inputEvent.type);
        return RET_ERR;
    }

    WriteDeviceFun fun = iter->second;
    return fun(inputEvent);
}

void SendMessage::SetTimeToLibinputEvent()
{
    timeval tm;
    gettimeofday(&tm, 0);
    event_.input_event_sec = tm.tv_sec;
    event_.input_event_usec = tm.tv_usec;
}

int32_t SendMessage::SimulateKeyboard(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t SendMessage::SimulateKeyboardLongPress(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    SimulateKeyboard(inputEvent);
    if (inputEvent.value == EVENT_RELEASE) {
        return RET_OK;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);

    int32_t keyEventNum = (inputEvent.blockTime / EVENT_REPROT_RATE) + 1;
    int16_t count = 0;
    while (count++ < keyEventNum) {
        CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
        SetTimeToLibinputEvent();

        event_.type = EV_KEY;
        event_.code = inputEvent.code;
        event_.value = LONG_PRESS;
        SendEvent(eventTarget_, event_);

        event_.type = EV_SYN;
        event_.code = SYN_REPORT;
        event_.value = SYN_CONFIG;
        SendEvent(eventTarget_, event_);
        std::this_thread::sleep_for(std::chrono::milliseconds(inputEvent.blockTime));
    }
    return RET_OK;
}

/*
 * Used to Simulate mouse movement events
 */
int32_t SendMessage::SimulateMouseMove(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }

    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_REL;
    event_.code = REL_X;
    event_.value = inputEvent.x;
    SendEvent(eventTarget_, event_);

    event_.type = EV_REL;
    event_.code = REL_Y;
    event_.value = inputEvent.y;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate right-click events
 */
int32_t SendMessage::SimulateMouseRightClick(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_RIGHT;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate mouse double click events
 */
int32_t SendMessage::SimulateMouseDoubleClick(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate mouse wheel sliding events
 */
int32_t SendMessage::SimulateMouseWheel(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_REL;
    event_.code = REL_WHEEL;
    event_.value = ~inputEvent.value + 1;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t SendMessage::SimulateMouseHwheel(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_REL;
    event_.code = REL_HWHEEL;
    event_.value = ~inputEvent.value + 1;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate left mouse click events
 */
int32_t SendMessage::SimulateMouseLeftClick(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(memset_s(&event_, sizeof(event_), 0, sizeof(event_) == EOK), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the left mouse button down event
 */
int32_t SendMessage::SimulateMouseLeftClickPress(int32_t eventTarget_)
{
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the left mouse button up event
 */
int32_t SendMessage::SimulateMouseLeftClickRelease(int32_t eventTarget_)
{
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_LEFT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate keyboard press events
 */
int32_t SendMessage::KeyboardPress(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }

    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate keyboard release events
 */
int32_t SendMessage::KeyboardRelease(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate rocker key events
 */
int32_t SendMessage::SimulateRockerKey(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();
    event_.type = EV_MSC;
    event_.code = MSC_SCAN;
    event_.value = OBFUSCATED;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_MSC;
    event_.code = MSC_SCAN;
    event_.value = OBFUSCATED;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the coolie cap
 * event at the top of the rocker
 */
int32_t SendMessage::SimulateRockerHat0y(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the throttle
 * event at the bottom of the rocker
 */
int32_t SendMessage::SimulateRockerAbsrz(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate rocker main control axis events
 */
int32_t SendMessage::SimulateRockerAbs(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate handle key events
 */
int32_t SendMessage::xBoxKey(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate handle direction key events
 */
int32_t SendMessage::xBoxHat0y(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the event of rocker
 * 2 at the bottom right of handle
 */
int32_t SendMessage::xBoxAbsRxy(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the event of rocker
 * 1 at the bottom right of handle
 */
int32_t SendMessage::xBoxAbs(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Used to Simulate the trigger event behind the handle
 */
int32_t SendMessage::xBoxAbsRz(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

/*
 * Delay key events
 */
int32_t SendMessage::SimulateKeyDelayEvent(const InputEvent& inputEvent)
{
    KeyboardPress(inputEvent);
    sleep(inputEvent.value);
    KeyboardRelease(inputEvent);
    return RET_OK;
}

int32_t SendMessage::SimulateTouchPress(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_MT_POSITION_X;
    event_.value = inputEvent.x;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MT_POSITION_Y;
    event_.value = inputEvent.y;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MT_TRACKING_ID;
    event_.value = inputEvent.track;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_TOUCH;
    event_.value = EVENT_PRESS;
    SendEvent(eventTarget_, event_);

    ReportTouchEvent(eventTarget_, inputEvent.multiReprot, event_);
    return RET_OK;
}

int32_t SendMessage::SimulateTouchMove(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_MT_POSITION_X;
    event_.value = inputEvent.x;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MT_POSITION_Y;
    event_.value = inputEvent.y;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MT_TRACKING_ID;
    event_.value = inputEvent.track;
    SendEvent(eventTarget_, event_);

    ReportTouchEvent(eventTarget_, inputEvent.multiReprot, event_);
    return RET_OK;
}

int32_t SendMessage::SimulateTouchReless(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_MT_TRACKING_ID;
    event_.value = inputEvent.track;
    SendEvent(eventTarget_, event_);

    event_.type = EV_KEY;
    event_.code = BTN_TOUCH;
    event_.value = EVENT_RELEASE;
    SendEvent(eventTarget_, event_);

    ReportTouchEvent(eventTarget_, inputEvent.multiReprot, event_);
    if (inputEvent.multiReprot > 0) {
        ReportTouchEvent(eventTarget_, inputEvent.multiReprot, event_);
    }
    return RET_OK;
}

int32_t SendMessage::ReportTouchEvent(int32_t& eventTarget_, int32_t multiReprot, input_event& event_)
{
    event_.type = EV_SYN;
    event_.code = SYN_MT_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    if (multiReprot > 0) {
        event_.type = EV_SYN;
        event_.code = SYN_REPORT;
        event_.value = 0;
        SendEvent(eventTarget_, event_);
    }
    return RET_OK;
}

int32_t SendMessage::SimulateTouchPadKeyEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_KEY;
    event_.code = inputEvent.code;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MISC;
    event_.value = (inputEvent.value == EVENT_PRESS) ? (TOUCH_PAD_PRESS) : (0);
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t OHOS::MMI::SendMessage::SimulateTouchPadRingPressEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MISC;
    event_.value = TOUCH_PAD_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t OHOS::MMI::SendMessage::SimulateTouchPadRingMoveEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t OHOS::MMI::SendMessage::SimulateTouchPadRingReleaseEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MISC;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}


int32_t OHOS::MMI::SendMessage::SimulateTouchFingerPressEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MISC;
    event_.value = TOUCH_PAD_PRESS;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t OHOS::MMI::SendMessage::SimulateTouchFingerMoveEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t OHOS::MMI::SendMessage::SimulateTouchFingerReleaseEvent(const InputEvent& inputEvent)
{
    eventTarget_ = inputEvent.fp;
    if (eventTarget_ < 0) {
        MMI_LOGE("eventTarget_ open fail! errCode:%{public}d", FILE_OPEN_FAIL);
        return RET_ERR;
    }
    CHKR(EOK == memset_s(&event_, sizeof(event_), 0, sizeof(event_)), STRSET_SEC_FUN_FAIL, RET_ERR);
    SetTimeToLibinputEvent();

    event_.type = EV_ABS;
    event_.code = ABS_WHEEL;
    event_.value = inputEvent.value;
    SendEvent(eventTarget_, event_);

    event_.type = EV_ABS;
    event_.code = ABS_MISC;
    event_.value = 0;
    SendEvent(eventTarget_, event_);

    event_.type = EV_SYN;
    event_.code = SYN_REPORT;
    event_.value = 0;
    SendEvent(eventTarget_, event_);
    return RET_OK;
}

int32_t SendMessage::GetDevIndexByType(const int32_t devType)
{
    static const CLMAP<INPUT_DEVICE_TYPE, INPUT_DEVICE_INDEX> deviceTypeToIndexMap = {
        {INPUT_DEVICE_CAP_KEYBOARD, INPUT_DEVICE_KEYBOARD_INDEX},
        {INPUT_DEVICE_CAP_POINTER, INPUT_DEVICE_POINTER_INDEX},
        {INPUT_DEVICE_CAP_TOUCH, INPUT_DEVICE_TOUCH_INDEX},
        {INPUT_DEVICE_CAP_TABLET_TOOL, INPUT_DEVICE_TABLET_TOOL_INDEX},
        {INPUT_DEVICE_CAP_TABLET_PAD, INPUT_DEVICE_TABLET_PAD_INDEX},
        {INPUT_DEVICE_CAP_GESTURE, INPUT_DEVICE_GESTURE_INDEX},
        {INPUT_DEVICE_CAP_SWITCH, INPUT_DEVICE_SWITCH_INDEX},
        {INPUT_DEVICE_CAP_JOYSTICK, INPUT_DEVICE_JOYSTICK_INDEX},
        {INPUT_DEVICE_CAP_REMOTE_CONTROL, INPUT_DEVICE_REMOTE_CONTROL},
        {INPUT_DEVICE_CAP_TRACKPAD5, INPUT_DEVICE_TRACKPAD5_INDEX},
    };

    auto iter = deviceTypeToIndexMap.find(static_cast<INPUT_DEVICE_TYPE>(devType));
    if (iter == deviceTypeToIndexMap.end()) {
        return RET_ERR;
    }
    return static_cast<int32_t>(iter->second);
}

int32_t SendMessage::GetDevIndexByName(const std::string& deviceName)
{
    static const CLMAP<std::string, INPUT_DEVICE_INDEX> deviceTypeToIndexMap = {
        {"keyboard model1", INPUT_DEVICE_KEYBOARD_INDEX},
        {"keyboard model2", INPUT_DEVICE_KEYBOARD_INDEX},
        {"keyboard model3", INPUT_DEVICE_KEYBOARD_INDEX},
        {"mouse", INPUT_DEVICE_POINTER_INDEX},
        {"trackball", INPUT_DEVICE_POINTER_INDEX},
        {"touch", INPUT_DEVICE_TOUCH_INDEX},
        {"pen", INPUT_DEVICE_TABLET_TOOL_INDEX},
        {"pad", INPUT_DEVICE_TABLET_PAD_INDEX},
        {"finger", INPUT_DEVICE_FINGER_INDEX},
        {"switch", INPUT_DEVICE_SWITCH_INDEX},
        {"joystick", INPUT_DEVICE_JOYSTICK_INDEX},
        {"gamePad", INPUT_DEVICE_GAMEPAD_INDEX},
        {"finger", INPUT_DEVICE_FINGER_INDEX},
        {"knob model1", INPUT_DEVICE_SWITCH_INDEX},
        {"knob model2", INPUT_DEVICE_SWITCH_INDEX},
        {"knob model3", INPUT_DEVICE_SWITCH_INDEX},
        {"remoteControl", INPUT_DEVICE_REMOTE_CONTROL},
        {"trackpad model1", INPUT_DEVICE_TRACKPAD5_INDEX},
        {"trackpad model2", INPUT_DEVICE_TRACKPAD5_INDEX},
    };

    auto iter = deviceTypeToIndexMap.find(deviceName);
    if (iter == deviceTypeToIndexMap.end()) {
        return RET_ERR;
    }
    return static_cast<int32_t>(iter->second);
}

int32_t SendMessage::SendToHdi(const InputEventArray& inputEventArray)
{
    int32_t devIndex = GetDevIndexByName(inputEventArray.deviceName);
    if (devIndex == RET_ERR) {
        MMI_LOGE("Get devIndex error by name:%{public}s.", inputEventArray.deviceName.c_str());
        return RET_ERR;
    }
    RawInputEvent speechEvent = {};
    for (InjectEvent event : inputEventArray.events) {
        TransitionHdiEvent(event.event, speechEvent);
        SendToHdi(devIndex, speechEvent);
        int32_t blockTime = (event.blockTime == 0) ? INJECT_SLEEP_TIMES : event.blockTime;
        std::this_thread::sleep_for(std::chrono::milliseconds(blockTime));
    }

    return RET_OK;
}

int32_t SendMessage::SendEvent(int32_t& eventTarget_, input_event& event)
{
    if (injectToHdf_) {
        int32_t devIndex = GetDevIndexByType(eventTarget_);
        if (devIndex == RET_ERR) {
            MMI_LOGE("send event get dev index error:devIndex = %{public}d", devIndex);
            return RET_ERR;
        }
        RawInputEvent speechEvent = {};
        TransitionHdiEvent(event, speechEvent);
        SendToHdi(devIndex, speechEvent);
    } else {
        write(eventTarget_, &event, sizeof(event));
    }
    return RET_OK;
}

/*
 * Sending Event Injection information to HDI
 */
int32_t SendMessage::SendToHdi(const int32_t& devIndex, const RawInputEvent& event)
{
    int32_t sendType = static_cast<int32_t>(SET_EVENT_INJECT);
    NetPacket cktToHdf(ID_MSG_HDI_INJECT);
    cktToHdf << sendType << devIndex << event;

    if (!(SendMsg(cktToHdf))) {
        MMI_LOGE("inject hdi send event to server error");
        return RET_ERR;
    }
    return RET_OK;
}

bool SendMessage::SendMsg(const NetPacket& ckt)
{
    return TestAuxToolClient::GetInstance().SendMsg(ckt);
}

void SendMessage::TransitionHdiEvent(const input_event& event, RawInputEvent& speechEvent)
{
    speechEvent.ev_type = event.type;
    speechEvent.ev_code = event.code;
    speechEvent.ev_value = event.value;
    speechEvent.stamp = static_cast<unsigned int>(event.input_event_usec);
}