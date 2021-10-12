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

#ifndef OHOS_SEND_MESSAGE_H
#define OHOS_SEND_MESSAGE_H
#include "msg_head.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
struct WriteDeviceFunMap {
    int32_t id;
    WriteDeviceFun fun;
};
class SendMessage {
public:
    SendMessage();
    ~SendMessage() = default;
    void Init();
    int32_t GetDevIndexByType(const int32_t devType);
    int32_t SendEvent(int32_t& eventTarget_, input_event& event_);
    int32_t WriteDevice(InputEvent& inputEvent);
    /* keyboard event */
    int32_t SimulateKeyboard(const InputEvent& inputEvent);
    int32_t SimulateKeyboardLongPress(const InputEvent& inputEvent);
    int32_t KeyboardPress(const InputEvent& inputEvent);
    int32_t KeyboardRelease(const InputEvent& inputEvent);

    /* Mouse event contain trackBall and TouchPad */
    int32_t SimulateMouseMove(const InputEvent& inputEvent);
    int32_t SimulateMouseLeftClick(const InputEvent& inputEvent);
    int32_t SimulateMouseLeftClickPress(int32_t eventTarget_);
    int32_t SimulateMouseLeftClickRelease(int32_t eventTarget_);
    int32_t SimulateMouseRightClick(const InputEvent& inputEvent);
    int32_t SimulateMouseDoubleClick(const InputEvent& inputEvent);
    int32_t SimulateMouseWheel(const InputEvent& inputEvent);
    int32_t SimulateMouseHwheel(const InputEvent& inputEvent);

    /* rocker event */
    int32_t SimulateRockerKey(const InputEvent& inputEvent);
    int32_t SimulateRockerHat0y(const InputEvent& inputEvent);
    int32_t SimulateRockerAbsrz(const InputEvent& inputEvent);
    int32_t SimulateRockerAbs(const InputEvent& inputEvent);

    /* xbox event */
    int32_t xBoxKey(const InputEvent& inputEvent);
    int32_t xBoxHat0y(const InputEvent& inputEvent);
    int32_t xBoxAbsRxy(const InputEvent& inputEvent);
    int32_t xBoxAbs(const InputEvent& inputEvent);
    int32_t xBoxAbsRz(const InputEvent& inputEvent);
    int32_t SimulateKeyDelayEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchPress(const InputEvent& inputEvent);
    int32_t SimulateTouchMove(const InputEvent& inputEvent);
    int32_t SimulateTouchReless(const InputEvent& inputEvent);
    int32_t ReportTouchEvent(int32_t& eventTarget_, int32_t multiReprot, input_event& event_);

    int32_t SimulateTouchPadKeyEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchPadRingPressEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchPadRingMoveEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchPadRingReleaseEvent(const InputEvent& inputEvent);

    int32_t SimulateTouchFingerPressEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchFingerMoveEvent(const InputEvent& inputEvent);
    int32_t SimulateTouchFingerReleaseEvent(const InputEvent& inputEvent);

    bool RegistSendEvent(WriteDeviceFunMap& msg)
    {
        auto it = mapFuns_.find(msg.id);
        if (it != mapFuns_.end()) {
            return false;
        }
        mapFuns_[msg.id] = msg.fun;
        return true;
    }

    WriteDeviceFun* GetFun(const int32_t id)
    {
        auto it = mapFuns_.find(id);
        if (it == mapFuns_.end()) {
            return nullptr;
        }
        return &it->second;
    }
    int32_t GetDevIndexByName(const std::string& deviceName);
    int32_t SendToHdi(const InputEventArray& inputEventArray);
    int32_t SendToHdi(const int32_t& devType, const RawInputEvent& event);
    bool SendMsg(const NetPacket& ckt);
    void TransitionHdiEvent(const input_event& event, RawInputEvent& sEvent);
public:
    bool injectToHdf_ = false;
private:
    void SetTimeToLibinputEvent();
private:
    CLMAP<int32_t, WriteDeviceFun> mapFuns_;
    input_event event_;
    int32_t eventTarget_ = -1;
private:
    static constexpr int32_t INJECT_SLEEP_TIMES = 10;
};
}
}
#endif
