/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MOUSE_TRANSFORM_PROCESSOR_H
#define MOUSE_TRANSFORM_PROCESSOR_H

#include <map>
#include <memory>

#include "libinput.h"
#include "singleton.h"
#include "define_multimodal.h"

#include "pointer_event.h"
#include "window_info.h"

namespace OHOS {

extern "C" {
    struct Offset {
        double dx;
        double dy;
    };
    int32_t HandleMotionAccelerate(const Offset* offset, bool mode, double* abs_x, double* abs_y, int32_t speed);
}

namespace MMI {
struct AccelerateCurve {
    std::vector<int32_t> speeds;
    std::vector<double> slopes;
    std::vector<double> diffNums;
};
class MouseTransformProcessor final : public std::enable_shared_from_this<MouseTransformProcessor> {
public:
    enum class RightClickType {
        TP_RIGHT_BUTTON = 1,
        TP_LEFT_BUTTON = 2,
        TP_TWO_FINGER_TAP = 3,
    };

    enum class PointerDataSource {
        MOUSE = 1,
        TOUCHPAD = 2,
    };

public:
    DISALLOW_COPY_AND_MOVE(MouseTransformProcessor);
    explicit MouseTransformProcessor(int32_t deviceId);
    ~MouseTransformProcessor() = default;
    std::shared_ptr<PointerEvent> GetPointerEvent() const;
    int32_t Normalize(struct libinput_event *event);
    int32_t NormalizeRotateEvent(struct libinput_event *event, int32_t type, double angle);
    void Dump(int32_t fd, const std::vector<std::string> &args);
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    bool NormalizeMoveMouse(int32_t offsetX, int32_t offsetY);
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
private:
    int32_t HandleMotionInner(struct libinput_event_pointer* data, struct libinput_event *event);
    int32_t HandleButtonInner(struct libinput_event_pointer* data, struct libinput_event *event);
    int32_t HandleAxisInner(struct libinput_event_pointer* data);
    int32_t HandleAxisBeginEndInner(struct libinput_event *event);
    void HandleAxisPostInner(PointerEvent::PointerItem &pointerItem);
    void HandlePostInner(struct libinput_event_pointer* data, PointerEvent::PointerItem &pointerItem);
    void HandleTouchPadAxisState(libinput_pointer_axis_source source, int32_t& direction, bool& tpScrollSwitch);
    void HandleTouchpadRightButton(struct libinput_event_pointer* data, const int32_t evenType, uint32_t &button);
    void HandleTouchpadLeftButton(struct libinput_event_pointer* data, const int32_t evenType, uint32_t &button);
    void HandleTouchpadTwoFingerButton(struct libinput_event_pointer* data, const int32_t evenType, uint32_t &button);
    void TransTouchpadRightButton(struct libinput_event_pointer* data, const int32_t type, uint32_t &button);
    void CalculateOffset(Direction direction, Offset &offset);
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    void HandleMotionMoveMouse(int32_t offsetX, int32_t offsetY);
    void HandlePostMoveMouse(PointerEvent::PointerItem &pointerItem);
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
    int32_t HandleButtonValueInner(struct libinput_event_pointer* data, uint32_t button, int32_t type);
    void DumpInner();
    void SetDxDyForDInput(PointerEvent::PointerItem& pointerItem, struct libinput_event_pointer* data);
    int32_t GetTouchpadSpeed(void);
    static int32_t PutConfigDataToDatabase(std::string &key, bool value);
    static int32_t GetConfigDataFromDatabase(std::string &key, bool &value);
    static int32_t PutConfigDataToDatabase(std::string &key, int32_t value);
    static int32_t GetConfigDataFromDatabase(std::string &key, int32_t &value);

public:
    static void InitAbsolution();
    static void OnDisplayLost(int32_t displayId);
    static int32_t GetDisplayId();
    static int32_t SetMousePrimaryButton(int32_t primaryButton);
    static int32_t GetMousePrimaryButton();
    static int32_t SetMouseScrollRows(int rows);
    static int32_t GetMouseScrollRows();
    static int32_t SetPointerSpeed(int32_t speed);
    static int32_t GetPointerSpeed();
    static int32_t SetPointerLocation(int32_t x, int32_t y);
    static int32_t SetTouchpadScrollSwitch(bool switchFlag);
    static int32_t GetTouchpadScrollSwitch(bool &switchFlag);
    static int32_t SetTouchpadScrollDirection(bool state);
    static int32_t GetTouchpadScrollDirection(bool &state);
    static int32_t SetTouchpadTapSwitch(bool switchFlag);
    static int32_t GetTouchpadTapSwitch(bool &switchFlag);
    static int32_t SetTouchpadRightClickType(int32_t type);
    static int32_t GetTouchpadRightClickType(int32_t &type);
    static int32_t SetTouchpadPointerSpeed(int32_t speed);
    static int32_t GetTouchpadPointerSpeed(int32_t &speed);

private:
    static double absolutionX_;
    static double absolutionY_;
    static int32_t currentDisplayId_;
    static int32_t globalPointerSpeed_;

    std::shared_ptr<PointerEvent> pointerEvent_ { nullptr };
    int32_t timerId_ { -1 };
    int32_t buttonId_ { -1 };
    bool isPressed_ { false };
    int32_t deviceId_ { -1 };
    bool isAxisBegin_ { false };
};
} // namespace MMI
} // namespace OHOS
#endif // MOUSE_TRANSFORM_PROCESSOR_H