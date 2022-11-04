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

#include "touch_2_key_handler.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "Touch2KeyHandler"};
std::shared_ptr<PointerEvent> pointevent_ = nullptr;
std::shared_ptr<KeyEvent> keyEvent_ = nullptr;
Touch2KeyHandler::PluginDispatchCmd cmd;
Touch2KeyHandler::PluginDispatchEventType EventType;
bool Phalangeal_Joint_ { false } ;
} // namespace

void Touch2KeyHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    pluginContext_->next->HandlePointerEvent();
    // keyEvent_ = keyEvent;
    // cmd = PluginDispatchCmd::REDIRECT;
    // EventType = PluginDispatchEventType::KEY_EVENT;
    // MMI_HILOGE("1111111111111111111111111111111DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
}

void Touch2KeyHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    pointevent_ = pointerEvent;
    cmd = PluginDispatchCmd::REDIRECT;
    EventType = PluginDispatchEventType::POINT_EVENT;
}

void Touch2KeyHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    pointevent_ = pointerEvent;
    cmd = PluginDispatchCmd::REDIRECT;
    EventType = PluginDispatchEventType::TOUCH_EVENT;
    if (Phalangeal_Joint_) {
        std::vector<int32_t> pointerIds { pointevent_->GetPointerIds() };
        for (const auto& pointerId : pointerIds) {
            PointerEvent::PointerItem item;
            if (!pointevent_->GetPointerItem(pointerId, item)) {
                MMI_HILOGE("Invalid pointer: %{public}d.", pointerId);
                return;
            }
            std::string type = pointevent_->DumpPointerAction();
            if (item.GetDisplayY() < 300 &&  item.GetDisplayY() > 200) {
                if (type == "down") {
                    keyEvent_ = KeyEvent::Create();
                    KeyEvent::KeyItem item;
                    keyEvent_->SetKeyCode(2017);
                    keyEvent_->SetActionTime(pointerEvent->GetActionTime());
                    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
                    item.SetKeyCode(2017);
                    item.SetDownTime(pointerEvent->GetActionStartTime());
                    item.SetPressed(true);
                    keyEvent_->AddKeyItem(item);
                    EventType = PluginDispatchEventType::KEY_EVENT;
                } else if (type == "up") {
                    keyEvent_ = KeyEvent::Create();
                    KeyEvent::KeyItem item;
                    keyEvent_->SetKeyCode(2017);
                    keyEvent_->SetActionTime(pointerEvent->GetActionTime());
                    keyEvent_->SetKeyAction(KeyEvent::KEY_ACTION_UP);
                    item.SetKeyCode(2017);
                    item.SetDownTime(pointerEvent->GetActionStartTime());
                    item.SetPressed(true);
                    keyEvent_->AddKeyItem(item);
                    EventType = PluginDispatchEventType::KEY_EVENT;
                } else {
                    MMI_HILOGE("", pointerEvent->GetAction());
                }
            }
        }
        MMI_HILOGE("EventType:%{public}s,ActionTime:%{public}" PRId64 ",Action:%{public}d,"
                "ActionStartTime:%{public}" PRId64 ",Flag:%{public}d,PointerAction:%{public}s,"
                "SourceType:%{public}s,ButtonId:%{public}d,VerticalAxisValue:%{public}.2f,"
                "HorizontalAxisValue:%{public}.2f,PinchAxisValue:%{public}.2f,PointerId:%{public}d,"
                "PointerCount:%{public}zu,EventNumber:%{public}d",
                InputEvent::EventTypeToString(pointevent_->GetEventType()), pointevent_->GetActionTime(),
                pointevent_->GetAction(), pointevent_->GetActionStartTime(), pointevent_->GetFlag(),
                pointevent_->DumpPointerAction(), pointevent_->DumpSourceType(), pointevent_->GetButtonId(),
                pointevent_->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
                pointevent_->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
                pointevent_->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH),
                pointevent_->GetPointerId(), pointerIds.size(), pointevent_->GetId());

        for (const auto& pointerId : pointerIds) {
            PointerEvent::PointerItem item;
            if (!pointevent_->GetPointerItem(pointerId, item)) {
                MMI_HILOGE("Invalid pointer: %{public}d.", pointerId);
                return;
            }
            MMI_HILOGE("pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,DisplayX:%{public}d,"
                "DisplayY:%{public}d,WindowX:%{public}d,WindowY:%{public}d,Width:%{public}d,Height:%{public}d,"
                "TiltX:%{public}.2f,TiltY:%{public}.2f,ToolDisplayX:%{public}d,ToolDisplayY:%{public}d,"
                "ToolWindowX:%{public}d,ToolWindowY:%{public}d,ToolWidth:%{public}d,ToolHeight:%{public}d,"
                "Pressure:%{public}.2f,ToolType:%{public}d,LongAxis:%{public}d,ShortAxis:%{public}d",
                pointerId, item.GetDownTime(), item.IsPressed(), item.GetDisplayX(),
                item.GetDisplayY(), item.GetWindowX(), item.GetWindowY(), item.GetWidth(), item.GetHeight(),
                item.GetTiltX(), item.GetTiltY(), item.GetToolDisplayX(), item.GetToolDisplayY(), item.GetToolWindowX(),
                item.GetToolWindowY(), item.GetToolWidth(), item.GetToolHeight(), item.GetPressure(), item.GetToolType(),
                item.GetLongAxis(), item.GetShortAxis());
        }
    }
}

Touch2KeyHandler::PluginDispatchCmd Touch2KeyHandler::GetDispatchCmd()
{
    return cmd;
}

Touch2KeyHandler::PluginDispatchEventType Touch2KeyHandler::GetDispatchEventType()
{
    return EventType;
}

const std::shared_ptr<KeyEvent> Touch2KeyHandler::GetKeyEvent()
{
    return keyEvent_;
}

const std::shared_ptr<PointerEvent> Touch2KeyHandler::GetPointEvent()
{
    return pointevent_;
}

void Touch2KeyHandler::SetPluginfunctionStatus(std::map<PluginfunctionId, bool> FunctionStatus) {
    Phalangeal_Joint_ = FunctionStatus[PluginfunctionId::PHALANGEAL_JOINT];
};

IInputEventConvertHandler::PluginInfo Touch2KeyHandler::GetPluginInfo()
{
    PluginInfo pluginInfo;
    pluginInfo.name = "name";
    pluginInfo.version = "1.0";
    pluginInfo.priority = 0;
    pluginInfo.handler.reset(create());
    return pluginInfo;
}
} // namespace MMI
} // namespace OHOS