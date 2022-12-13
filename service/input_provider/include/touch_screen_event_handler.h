/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef TOUCH_SCREEN_EVENT_HANDLER_H
#define TOUCH_SCREEN_EVENT_HANDLER_H

#include <memory>
#include <string>
#include <map>
#include <mtdev-plumbing.h>
#include <nocopyable.h>

#include "mtdev.h"

#include "i_event_handler_manager.h"
#include "i_event_handler.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
static const int ev_max[EV_MAX + 1] = {
    SYN_MAX, KEY_MAX, REL_MAX, ABS_MAX, MSC_MAX, 
    SW_MAX, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, 
    LED_MAX, SND_MAX, -1, REP_MAX, FF_MAX,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1,
};
static constexpr int32_t ACTION_NONE = 0;
static constexpr int32_t ACTION_DOWN = 1;
static constexpr int32_t ACTION_MOVE = 2;
static constexpr int32_t ACTION_UP = 3;
static constexpr int32_t SOURCE_TYPE_NONE = 0;
static constexpr int32_t SOURCE_TYPE_TOUCHSCREEN = 1;
static constexpr int32_t SOURCE_TYPE_TOUCHPAD = 2;
static constexpr int32_t SOURCE_TYPE_END = 3;

struct Slot {
    std::shared_ptr<PointerEvent::PointerItem> pointer;
    bool isDirty { false };
    int32_t id { 0 };
    int32_t pointerAction { ACTION_NONE };
};

class TouchScreenEventHandler : public IEventHandler {
public:
    virtual ~TouchScreenEventHandler() = default;
    DISALLOW_COPY_AND_MOVE(TouchScreenEventHandler);
    static std::shared_ptr<TouchScreenEventHandler> CreateInstance();
    virtual void HandleEvent(const struct input_event& event) override;
    virtual void SetAxisInfo(std::shared_ptr<IInputDevice::AxisInfo> xInfo, std::shared_ptr<IInputDevice::AxisInfo> yInfo) override;
    virtual void SetEventHandlerManager(std::shared_ptr<IEventHandlerManager> eventhandleManager) override { iEventHandlerMgr_ = eventhandleManager; }

private:
    void HandleAbsEvent(int code, int value);
    void HandleSyncEvent();
    void HandleMtSlot(int value);
    void HandleMtPositionX(int value);
    void HandleMtPositionY(int value);
    void HandleMtTrackingId(int value);
    void HandleMtTouchMajor(int value);
    void HandleMtTouchMinor(int value);
    void HandleMtPressure(int value);
    void HandleMtToolX(int value);
    void HandleMtToolY(int value);
    void HandleMtToolType(int value);
    std::shared_ptr<Slot> GetCurrentPointer(bool createIfNotExist);
    void ProcessEvent(const struct input_event& event);
    void OnInputEvent(std::shared_ptr<Slot> slot);
    const std::shared_ptr<PointerEvent> TransformToPointerEvent(std::shared_ptr<Slot> slot);
    bool OnEventTouchDown(std::shared_ptr<Slot> slot);
    bool OnEventTouchUp(std::shared_ptr<Slot> slot);
    bool OnEventTouchMotion(std::shared_ptr<Slot> slot);
    int32_t EventIsCode(const struct input_event& ev, unsigned int type, unsigned int code);
    int32_t EventIsType(const struct input_event& ev, unsigned int type);
    int32_t EventTypeGetMax(unsigned int type);

protected:
    TouchScreenEventHandler();

private:
    int32_t slotNum_ { 10 };
    int32_t curSlot_ { 0 };
    std::map<int32_t, std::shared_ptr<Slot>> slots_;
    std::shared_ptr<PointerEvent> pointerEvent_ { nullptr };
    mtdev* mtdev_ { nullptr };
    std::shared_ptr<IInputDevice::AxisInfo> xInfo_;
    std::shared_ptr<IInputDevice::AxisInfo> yInfo_;
    std::shared_ptr<IEventHandlerManager> iEventHandlerMgr_;
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_SCREEN_EVENT_HANDLER_H