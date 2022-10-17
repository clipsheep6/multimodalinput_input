#include <algorithm>

#include "TouchPadSingleMoveTransformer.h"
#include "PointerEvent.h"
#include "RelEvent.h"
#include "Log.h"

namespace Input {

    std::shared_ptr<TouchPadSingleMoveTransformer> TouchPadSingleMoveTransformer::Create(const IInputContext* context, 
            const std::shared_ptr<INewEventListener>& listener) {
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return nullptr;
        }

        if (!listener) {
            LOG_E("Leave, null listener");
            return nullptr;
        }

        return std::shared_ptr<TouchPadSingleMoveTransformer>(new TouchPadSingleMoveTransformer(context, listener));
    }

    TouchPadSingleMoveTransformer::TouchPadSingleMoveTransformer(const IInputContext* context, 
            const std::shared_ptr<INewEventListener>& listener)
        : context_(context), listener_(listener), name_("TouchPadSingleMoveTransformer")
    {
    }

    bool TouchPadSingleMoveTransformer::HandleEvent(const std::shared_ptr<const KeyEvent>& event)
    {
        return false;
    }

    bool TouchPadSingleMoveTransformer::HandleEvent(const std::shared_ptr<const PointerEvent>& event)
    {
        if (!event) {
            return false;
        }

        switch (state_) {
            case Waiting:
                ProcessOnWaiting(event);
                break;
            case Moving:
                ProcessOnMoving(event);
                break;
            case Dead:
                ProcessOnDead(event);
                break;
            default:
                break;
        }

        return false;
    }

    const std::string& TouchPadSingleMoveTransformer::GetName() const
    {
        return name_;
    }

    void TouchPadSingleMoveTransformer::ProcessOnWaiting(const std::shared_ptr<const PointerEvent>& event)
    {
       auto pointerAction = event->GetPointerAction(); 
       auto pointerId = event->GetPointerId();
       auto pointerIdList = event->GetPointerIdList();
       auto it = std::find(pointerIdList.begin(), pointerIdList.end(), pointerId);
       if (it == pointerIdList.end()) {
           state_ = Dead;
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
               state_ = Dead;
               LOG_E("Leave, Not Exist CurrentPointerItem:$s, Enter Dead State", event);
               return;
           }
           pointerId_ = pointerItem->GetId();
           preX_ = pointerItem->GetGlobalX();
           preY_ = pointerItem->GetGlobalY();
           state_ = Moving;
           LOG_D("Leave, Enter Moving State by Pointer Down");
           return;
       }

       if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
           pointerIdList.erase(it);
           if (pointerIdList.size() != 1) {
               LOG_D("Leave");
               return;
           }

           auto pointerItem = event->GetPointerItem(*pointerIdList.begin());
           if (!pointerItem) {
               state_ = Dead;
               LOG_E("Leave, null pointerItem, Enter Dead State");
               return;
           }

           pointerId_ = pointerId;
           preX_ = pointerItem->GetGlobalX();
           preY_ = pointerItem->GetGlobalY();
           state_ = Moving;
           LOG_D("Leave, Enter Moving State by Pointer Up");
           return;
       }
    }

    void TouchPadSingleMoveTransformer::ProcessOnMoving(const std::shared_ptr<const PointerEvent>& event)
    {
        auto pointerAction = event->GetPointerAction(); 
        auto pointerId = event->GetPointerId();
        auto pointerIdList = event->GetPointerIdList();
        auto it = std::find(pointerIdList.begin(), pointerIdList.end(), pointerId);
        if (it == pointerIdList.end()) {
            state_ = Dead;
            LOG_E("Leave, PointerId Not In PointerIdList, PointerEvent:$s, Enter Dead State", event);
            return;
        } 

        if (pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
            state_ = Waiting;
            LOG_D("Leave, Down Action When Moving, Enter to Dead State");
            return;
        }

        if (pointerAction == PointerEvent::POINTER_ACTION_MOVE) {
           auto pointerItem = event->GetPointerItem();
           if (!pointerItem) {
               state_ = Dead;
               LOG_E("Leave, null pointerItem on moving, enter to Dead State");
               return;
           }

           auto pointerId = event->GetPointerId();
           if (pointerId != pointerId_) {
               state_ = Dead;
               LOG_E("Leave, pointerId Mismatch When moving, enter to Dead State");
               return;
           }

           if (!relEvent_) {
               relEvent_ = std::make_shared<RelEvent>(event->GetDeviceId());
               relEvent_->SetAction(RelEvent::ACTION_MOVE);
           }

           auto curX = pointerItem->GetGlobalX();
           auto curY = pointerItem->GetGlobalY();
           auto deltaX = curX - preX_;
           if (deltaX > minDeltaX_ && deltaX < minDeltaX_) {
               return;
           }

           auto deltaY = curY - preY_;
           if (deltaY > minDeltaY_ && deltaY < minDeltaY_) {
               return;
           }

           preX_ = curX;
           preY_ = curY;
           relEvent_->SetRelX(deltaX);
           relEvent_->SetRelY(deltaY);
           relEvent_->SetActionTime(event->GetActionTime());
           if (listener_) {
               listener_->OnEvent(relEvent_);
           }

           LOG_D("Moving, relEvent:$s", relEvent_);
           return;
       }

       if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
           state_ = Waiting;
           LOG_D("Leave, Enter Waiting by Pointer Up");
           return;
       }

       state_ = Dead;
       LOG_E("Leave, Invalid PointerAction:$s When moving, enter to Dead State", pointerAction);
    }

    void TouchPadSingleMoveTransformer::ProcessOnDead(const std::shared_ptr<const PointerEvent>& event)
    {
        auto pointerAction = event->GetPointerAction(); 
        if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
            auto count = GetTouchedPointerCount(event);
            if (count < 0) {
                LOG_E("Leave, Invalid PointerEvent, touched Pointer Cout < 0");
                return;
            }

            if (count > 0) {
                return;
            }
            
            LOG_D("Enter Waiting State");
            state_ = Waiting;
            return;
        }
    }

    int32_t TouchPadSingleMoveTransformer::GetTouchedPointerCount(const std::shared_ptr<const PointerEvent>& event)
    {
        if (!event) {
            return -1;
        }

        auto idList = event->GetPointerIdList();
        auto pointerAction = event->GetPointerAction();
        if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
            auto pointerId = event->GetPointerId();
            auto it = std::find(idList.begin(), idList.end(), pointerId);
            if (it != idList.end()) {
                idList.erase(it);
            }
        }

        return (int32_t)(idList.size());
    }

}
