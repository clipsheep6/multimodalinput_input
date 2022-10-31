#include <algorithm>

#include <linux/input-event-codes.h>

#include "TouchPadSingleTapTransformer.h"
#include "PointerEvent.h"
#include "KernelKeyEvent.h"
#include "Log.h"

namespace Input {

    std::shared_ptr<TouchPadSingleTapTransformer> TouchPadSingleTapTransformer::Create(const IInputContext* context, 
            const std::shared_ptr<INewEventListener>& listener) {
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return nullptr;
        }

        if (!listener) {
            LOG_E("Leave, null listener");
            return nullptr;
        }

        return std::shared_ptr<TouchPadSingleTapTransformer>(new TouchPadSingleTapTransformer(context, listener));
    }

    TouchPadSingleTapTransformer::TouchPadSingleTapTransformer(const IInputContext* context, 
            const std::shared_ptr<INewEventListener>& listener)
        : context_(context), listener_(listener), name_("TouchPadSingleTapTransformer")
    {
    }

    bool TouchPadSingleTapTransformer::HandleEvent(const std::shared_ptr<const KeyEvent>& event)
    {
        return false;
    }

    bool TouchPadSingleTapTransformer::HandleEvent(const std::shared_ptr<const PointerEvent>& event)
    {
        if (!event) {
            return false;
        }

        switch (state_) {
            case Idle:
                ProcessOnIdle(event);
                break;
            case FirstDown:
                ProcessOnFirstDown(event);
                break;
            default:
                break;
        }

        return false;
    }

    const std::string& TouchPadSingleTapTransformer::GetName() const
    {
        return name_;
    }

    void TouchPadSingleTapTransformer::ProcessOnIdle(const std::shared_ptr<const PointerEvent>& event)
    {
       auto pointerAction = event->GetPointerAction(); 
       auto pointerId = event->GetPointerId();
       auto pointerIdList = event->GetPointerIdList();
       auto it = std::find(pointerIdList.begin(), pointerIdList.end(), pointerId);
       if (it == pointerIdList.end()) {
           state_ = Idle;
           LOG_E("Leave, PointerId Not In PointerIdList:$s, Enter Dead State", event);
           return;
       } 

       if (pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
           if (pointerIdList.size() != 1) {
               LOG_D("Leave, Not First Down");
               return;
           }

           auto pointerItem = event->GetPointerItem();
           if (!pointerItem) {
               state_ = Idle;
               LOG_E("Leave, Not Exist CurrentPointerItem:$s, Enter Dead State", event);
               return;
           }
           pointerId_ = pointerItem->GetId();
           pointerDownTime_ = event->GetActionTime();
           downX_ = pointerItem->GetGlobalX();
           downY_ = pointerItem->GetGlobalY();
           state_ = FirstDown;
           LOG_D("Leave, Enter FirstDown State by Pointer Down");
           return;
       }
    }

    void TouchPadSingleTapTransformer::ProcessOnFirstDown(const std::shared_ptr<const PointerEvent>& event)
    {
        auto pointerAction = event->GetPointerAction(); 
        auto pointerId = event->GetPointerId();
        auto pointerIdList = event->GetPointerIdList();
        auto it = std::find(pointerIdList.begin(), pointerIdList.end(), pointerId);
        if (it == pointerIdList.end()) {
            state_ = Idle;
            LOG_E("Leave, PointerId Not In PointerIdList, PointerEvent:$s, Enter Idle State", event);
            return;
        } 

        if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
            pointerIdList.erase(it);
            if (!pointerIdList.empty()) {
                LOG_E("Leave, POINTER_ACTION_UP, pointerIdList not empty, Enter Idle State");
                state_ = Idle;
                return;
            }

            int64_t curActionTime = event->GetActionTime();
            if (curActionTime - pointerDownTime_ <= maxIntervalMs_) {
                LOG_I("SingleTap to Mouse Left Button Click");
                std::shared_ptr<KernelKeyEvent> buttonEvent = std::make_shared<KernelKeyEvent>(event->GetDeviceId());
                buttonEvent->SetAction(KernelKeyEvent::ACTION_DOWN);
                buttonEvent->SetKeyCode(BTN_LEFT);
                buttonEvent->SetActionTime(curActionTime);

                if (listener_) {
                    listener_->OnEvent(buttonEvent);
                    buttonEvent->SetAction(KernelKeyEvent::ACTION_UP);
                    listener_->OnEvent(buttonEvent);
                }
            }

            state_ = Idle;
            LOG_D("Leave, Enter Idle by Pointer Up");
            return;
        }

        if (pointerAction == PointerEvent::POINTER_ACTION_MOVE) {
            auto pointerItem = event->GetPointerItem();
            if (!pointerItem) {
                state_ = Idle;
                LOG_D("Leave, null pointerItem, Enter Idle");
                return;
            }

            auto curPointerId = event->GetPointerId();
            auto curX = pointerItem->GetGlobalX();
            auto curY = pointerItem->GetGlobalY();
            if (curPointerId != pointerId_) {
                state_ = Idle;
                LOG_D("Leave, pointerId Mismatch, Enter Idle");
                return;
            }

            auto deltaX = curX - downX_;
            if (deltaX < -maxDeltaX_ || deltaX > maxDeltaX_) {
                state_ = Idle;
                LOG_D("Leave, Move too far deltaX:$s(limit:$s), Enter Idle", deltaX, maxDeltaX_);
                return;
            }

            auto deltaY = curY - downY_;
            if (deltaY < -maxDeltaY_ || deltaY > maxDeltaY_) {
                state_ = Idle;
                LOG_D("Leave, Move too far deltaY:$s(limit:$s), Enter Idle", deltaY, maxDeltaY_);
                return;
            }

            return;
        }

        state_ = Idle;
        LOG_E("Leave, PointerAction:$s Enter Idle State", PointerEvent::ActionToString(pointerAction));
        return;
    }

}
