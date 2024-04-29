/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "touch_drawing_manager.h"
#include "i_multimodal_input_connect.h"
#include "setting_datashare.h"
#include "text/font_mgr.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchDrawingManager" };
constexpr int32_t DENSITY_BASELINE = 160;
constexpr int32_t INDEPENDENT_INNER_PIXELS = 20;
constexpr int32_t INDEPENDENT_OUTER_PIXELS = 21;
constexpr int32_t INDEPENDENT_WIDTH_PIXELS = 2;
constexpr int32_t CALCULATE_MIDDLE = 2;
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t RECT_COUNT = 6;
constexpr int32_t RECT_TOP = 118;
constexpr int32_t RECT_HEIGHT = 50;
constexpr int32_t TEXT_SPACEING = 0;
constexpr int32_t PEN_WIDTH = 1;
constexpr float TEXT_SIZE = 40.0f;
constexpr float TEXT_SCALE = 1.0f;
constexpr float TEXT_SKEW = 0.0f;
const std::string showCursorSwitchName = "settings.input.show_touch_hint";
const std::string pointerPositionSwitchName = "settings.input.show_touch_hint";
} // namespace

TouchDrawingManager::TouchDrawingManager()
{
    bubbleBrush_.SetColor(Rosen::Drawing::Color::COLOR_WHITE);
    bubbleBrush_.SetAntiAlias(true);
    float innerCircleTransparency = 0.6f;
    bubbleBrush_.SetAlphaF(innerCircleTransparency);

    bubblePen_.SetColor(Rosen::Drawing::Color::COLOR_BLACK);
    bubblePen_.SetAntiAlias(true);
    float outerCircleTransparency = 0.1f;
    bubblePen_.SetAlphaF(outerCircleTransparency);

    textBrush_.SetColor(Rosen::Drawing::Color::COLOR_BLACK);
    pathPen_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(255, 0, 96, 255));
    pointPen_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0));
    crosshairsPen_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(255, 0, 0, 192));
}

TouchDrawingManager::~TouchDrawingManager() {}

void TouchDrawingManager::TouchDrawHandler(const std::shared_ptr<PointerEvent>& pointerEvent)
{
    MMI_HILOGI("ZXH::TouchDrawHandler start");
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    pointerEvent_ = pointerEvent;
    CreateObserver();
    if (bubbleCanvasNode_ == nullptr) {
        bubbleCanvasNode_ = Rosen::RSCanvasNode::Create();
        InitCanvasNode(bubbleCanvasNode_);
    }
    if (trackerCanvasNode_ == nullptr) {
        trackerCanvasNode_ = Rosen::RSCanvasDrawingNode::Create();
        InitCanvasNode(trackerCanvasNode_);
    }
    if (crosshairCanvasNode_ == nullptr) {
        crosshairCanvasNode_ = Rosen::RSCanvasNode::Create();
        InitCanvasNode(crosshairCanvasNode_);
    }  
    if (labelsCanvasNode_ == nullptr) {
        labelsCanvasNode_ = Rosen::RSCanvasNode::Create();
        InitCanvasNode(labelsCanvasNode_);
    } 
    CreateTouchWindow();
    if (bubbleMode_.isShow) {
        DrawBubbleHandler();
    }
    if (pointerMode_.isShow) {
        UpdatePointerPositionInfo();
        ClearTracker();
        DrawPointerPositionHandler();
    }
    MMI_HILOGI("ZXH::TouchDrawHandler end");
}

void TouchDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    displayInfo_ = displayInfo;
    bubble_.innerCircleRadius = displayInfo.dpi * INDEPENDENT_INNER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleRadius = displayInfo.dpi * INDEPENDENT_OUTER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleWidth = static_cast<float>(displayInfo.dpi * INDEPENDENT_WIDTH_PIXELS) / DENSITY_BASELINE;
}

void TouchDrawingManager::GetOriginalTouchScreenCoordinates(Direction direction, int32_t width, int32_t height,
    int32_t &physicalX, int32_t &physicalY)
{
    switch (direction) {
        case DIRECTION0: {
            MMI_HILOGD("direction is DIRECTION0");
            break;
        }
        case DIRECTION90: {
            int32_t temp = physicalY;
            physicalY = width - physicalX;
            physicalX = temp;
            MMI_HILOGD("direction is DIRECTION90, Original touch screen physicalX:%{public}d, physicalY:%{public}d",
                physicalX, physicalY);
            break;
        }
        case DIRECTION180: {
            physicalX = width - physicalX;
            physicalY = height - physicalY;
            MMI_HILOGD("direction is DIRECTION180, Original touch screen physicalX:%{public}d, physicalY:%{public}d",
                physicalX, physicalY);
            break;
        }
        case DIRECTION270: {
            int32_t temp = physicalX;
            physicalX = height - physicalY;
            physicalY = temp;
            MMI_HILOGD("direction is DIRECTION270, Original touch screen physicalX:%{public}d, physicalY:%{public}d",
                physicalX, physicalY);
            break;
        }
        default: {
            MMI_HILOGW("direction is invalid, direction:%{public}d", direction);
            break;
        }
    }
}

void TouchDrawingManager::UpdateLabels(bool isShow)
{
    if (isShow) {
        MMI_HILOGI("ZXH::UpdateLabels start %{public}d", isShow);
        DrawLabels();
    } else {
        ClearPointerPosition();
    }
}

void TouchDrawingManager::CreateObserver()
{
    if (!hasBubbleObserver_) {
        bubbleMode_.SwitchName = showCursorSwitchName;
        CreateBubbleObserver(bubbleMode_);
        hasBubbleObserver_ = true;
    }

    if (!hasPointerObserver_) {
        pointerMode_.SwitchName = pointerPositionSwitchName;
        CreatePointerObserver(pointerMode_);
        hasPointerObserver_ = true;
    }
}

void TouchDrawingManager::InitCanvasNode(std::shared_ptr<Rosen::RSCanvasNode>& canvasNode)
{
    CHKPV(canvasNode);
    canvasNode->SetBounds(0, 0, displayInfo_.width, displayInfo_.height);
    canvasNode->SetFrame(0, 0, displayInfo_.width, displayInfo_.height);
#ifndef USE_ROSEN_DRAWING
    canvasNode->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    canvasNode->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif
    canvasNode->SetCornerRadius(1);
    canvasNode->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode->SetRotation(0);
    MMI_HILOGI("ZXH::RSCanvasNodeCreate RSCanvasNode");
}

void TouchDrawingManager::CreateTouchWindow()
{
    MMI_HILOGI("ZXH::CreateTouchWindow start");
    CALL_DEBUG_ENTER;
    if (surfaceNode_ != nullptr) {
        MMI_HILOGI("surfaceNode is already.");
        return;
    }
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "touch window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    CHKPV(surfaceNode_);
    surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode_->SetBounds(0, 0, displayInfo_.width, displayInfo_.height);
    surfaceNode_->SetFrame(0, 0, displayInfo_.width, displayInfo_.height);
#ifndef USE_ROSEN_DRAWING
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    surfaceNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif
    surfaceNode_->SetRotation(0);
    if (bubbleCanvasNode_ != nullptr) {
        MMI_HILOGI("add child bubble canvas node");
        surfaceNode_->AddChild(bubbleCanvasNode_, DEFAULT_VALUE);
    }
    if (trackerCanvasNode_ != nullptr) {
        MMI_HILOGI("add child pointer position canvas node");
        surfaceNode_->AddChild(trackerCanvasNode_, DEFAULT_VALUE);
    }
    if (crosshairCanvasNode_ != nullptr) {
        MMI_HILOGI("add child crosshair canvas node");
        surfaceNode_->AddChild(crosshairCanvasNode_, DEFAULT_VALUE);
    }
    if (labelsCanvasNode_ != nullptr) {
        MMI_HILOGI("add child labels canvas node");
        surfaceNode_->AddChild(labelsCanvasNode_, DEFAULT_VALUE);
    }
    surfaceNode_->AttachToDisplay(static_cast<uint64_t>(pointerEvent_->GetTargetDisplayId()));
    Rosen::RSTransaction::FlushImplicitTransaction();
    MMI_HILOGI("ZXH::CreateTouchWindow end");
}

void TouchDrawingManager::DrawBubbleHandler()
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent_);
    auto pointerAction = pointerEvent_->GetPointerAction();
    if (IsValidAction(pointerAction)) {
        DrawBubble();
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void TouchDrawingManager::DrawBubble()
{
    CHKPV(pointerEvent_);
    CHKPV(bubbleCanvasNode_);
    auto canvas = static_cast<RosenCanvas *>(bubbleCanvasNode_->BeginRecording(displayInfo_.width, displayInfo_.height));
    CHKPV(canvas);
    auto pointerIdList = pointerEvent_->GetPointerIds();
    for (auto pointerId : pointerIdList) {
        if ((pointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_UP ||
            pointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP) &&
            pointerEvent_->GetPointerId() == pointerId) {
            continue;
        }
        PointerEvent::PointerItem pointerItem;
        if (!pointerEvent_->GetPointerItem(pointerId, pointerItem)) {
            MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
            return;
        }
        int32_t physicalX = pointerItem.GetDisplayX();
        int32_t physicalY = pointerItem.GetDisplayY();
        if (displayInfo_.displayDirection == DIRECTION0) {
            GetOriginalTouchScreenCoordinates(displayInfo_.direction, displayInfo_.width,
                displayInfo_.height, physicalX, physicalY);
        }
        Rosen::Drawing::Point centerPt(physicalX, physicalY);
        bubblePen_.SetWidth(bubble_.outerCircleWidth);
        canvas->AttachPen(bubblePen_);
        canvas->DrawCircle(centerPt, bubble_.outerCircleRadius);
        canvas->DetachPen();

        canvas->AttachBrush(bubbleBrush_);
        canvas->DrawCircle(centerPt, bubble_.innerCircleRadius);
        canvas->DetachBrush();
    }
    bubbleCanvasNode_->FinishRecording();
}

void TouchDrawingManager::DrawPointerPositionHandler()
{
    MMI_HILOGI("ZXH::DrawPointerPositionHandler start");
    CHKPV(pointerEvent_);
    crosshairCanvas_ = static_cast<RosenCanvas *>(crosshairCanvasNode_->BeginRecording(displayInfo_.width, displayInfo_.height));
    auto pointerIdList = pointerEvent_->GetPointerIds();
    for (auto pointerId : pointerIdList) {
        if ((pointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_UP ||
            pointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_PULL_UP) &&
            pointerEvent_->GetPointerId() == pointerId) {
                MMI_HILOGD("action up and pointerId: %{public}d", pointerId);
                continue;
        }
        PointerEvent::PointerItem pointerItem;
        if (!pointerEvent_->GetPointerItem(pointerId, pointerItem)) {
            MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
            return;
        }
        int32_t displayX = pointerItem.GetDisplayX();
        int32_t displayY = pointerItem.GetDisplayY();
        DrawTracker(displayX, displayY, pointerId);
        DrawCrosshairs(displayX, displayY);
        UpdateLastPointerItem(pointerId, pointerItem);
    }
    CHKPV(trackerCanvasNode_);
    DrawLabels();
    crosshairCanvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
    MMI_HILOGI("ZXH::DrawPointerPositionHandler end");
}

void TouchDrawingManager::DrawTracker(int32_t x, int32_t y, int32_t pointerId)
{
    CALL_DEBUG_ENTER;
    CHKPV(trackerCanvasNode_);
    auto canvas = static_cast<RosenCanvas *>(trackerCanvasNode_->BeginRecording(displayInfo_.width, displayInfo_.height));
    if (lastPointerItem_.empty()) {
        MMI_HILOGD("last pointer item is empty.");
        return;
    }
    Rosen::Drawing::Point centerPt(x, y);
    int32_t lastPhysicalX = 0;
    int32_t lastPhysicalY = 0;
    bool bFind = false;
    for (auto &item : lastPointerItem_) {
        if (item.GetPointerId() == pointerId) {
            lastPhysicalX = item.GetDisplayX();
            lastPhysicalY = item.GetDisplayY();
            bFind = true;
            break;
        }
    }
    if (bFind) {
        Rosen::Drawing::Point lastCenterPt(lastPhysicalX, lastPhysicalY);
        pathPen_.SetWidth(PEN_WIDTH);
        canvas->AttachPen(pathPen_);
        MMI_HILOGD("lastCenterPt lastPhysicalX: %{public}d, lastPhysicalY: %{public}d. centerPt x: %{public}d, y: %{public}d",
         lastPhysicalX, lastPhysicalY, x, y);
        canvas->DrawLine(lastCenterPt, centerPt);
        canvas->DetachPen();
        pointPen_.SetWidth(PEN_WIDTH * 3);
        canvas->AttachPen(pointPen_);
        canvas->DrawPoint(lastCenterPt);
        canvas->DetachPen();
    }
    trackerCanvasNode_->FinishRecording();
}

void TouchDrawingManager::DrawCrosshairs(int32_t x, int32_t y)
{
    CHKPV(crosshairCanvasNode_);
    CHKPV(crosshairCanvas_);
    crosshairsPen_.SetWidth(PEN_WIDTH);
    crosshairCanvas_->AttachPen(crosshairsPen_);
    Rosen::Drawing::Point left(0, y);
    Rosen::Drawing::Point right(displayInfo_.width, y);
    crosshairCanvas_->DrawLine(left, right);
    Rosen::Drawing::Point top(x, 0);
    Rosen::Drawing::Point bottom(x, displayInfo_.height);
    crosshairCanvas_->DrawLine(top, bottom);
    crosshairCanvas_->DetachPen();
}

void TouchDrawingManager::DrawLabels()
{
    MMI_HILOGI("ZXH::DrawLabels start");
    CHKPV(labelsCanvasNode_);
    labelsCanvas_ = static_cast<RosenCanvas *>(labelsCanvasNode_->BeginRecording(displayInfo_.width, displayInfo_.height));
    CHKPV(labelsCanvas_);
    int32_t itemW = displayInfo_.width / RECT_COUNT - TEXT_SPACEING;
    std::string viewP = "P: " + std::to_string(currentPointerCount_) + " / " + std::to_string(maxPointerCount_);
    std::string viewX = "X: " + FormatNumber(currentPhysicalX_, 1);
    std::string viewY = "Y: " + FormatNumber(currentPhysicalY_, 1);
    auto dx = currentPhysicalX_ - currentPointerItem_.GetDisplayX();
    auto dy = currentPhysicalY_ - currentPointerItem_.GetDisplayY();
    std::string viewDx = "dX: " + FormatNumber(dx, 1);
    std::string viewDy = "dY: " + FormatNumber(dy, 1);
    std::string viewXv = "Xv: " + FormatNumber(xVelocity_, 3);
    std::string viewYv = "Yv: " + FormatNumber(yVelocity_, 3);
    std::string viewPrs = "Prs: " + FormatNumber(pressure_, 2);
    Rosen::Drawing::Rect rect;
    rect.left_ = TEXT_SPACEING;
    rect.right_ = itemW + rect.left_;
    rect.top_ = RECT_TOP;
    rect.bottom_ = RECT_TOP + RECT_HEIGHT;
    DrawRectItem(labelsCanvas_, viewP, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255));
    if (isDownAction_ || !lastPointerItem_.empty()) {
        rect.left_ += itemW + TEXT_SPACEING;
        rect.right_ += itemW + TEXT_SPACEING;
        DrawRectItem(labelsCanvas_, viewX, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255));
        rect.left_ += itemW + TEXT_SPACEING;
        rect.right_ += itemW + TEXT_SPACEING;
        DrawRectItem(labelsCanvas_, viewY, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255));
    } else {
        Rosen::Drawing::Color color = dx == 0 ?  Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255) :
            Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 0, 0);
        rect.left_ += itemW + TEXT_SPACEING;
        rect.right_ += itemW + TEXT_SPACEING;
        DrawRectItem(labelsCanvas_, viewDx, rect, color);
        color = dy == 0 ?  Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255) :
            Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 0, 0);
        rect.left_ += itemW + TEXT_SPACEING;
        rect.right_ += itemW + TEXT_SPACEING;
        DrawRectItem(labelsCanvas_, viewDy, rect, color);
    }
	
	   rect.left_ += itemW + TEXT_SPACEING;
    rect.right_ += itemW + TEXT_SPACEING;
    DrawRectItem(labelsCanvas_, viewXv, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255));
    rect.left_ += itemW + TEXT_SPACEING;
    rect.right_ += itemW + TEXT_SPACEING;
    DrawRectItem(labelsCanvas_, viewYv, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 255, 255));
    std::shared_ptr<Rosen::Drawing::TextBlob> textPrs = Rosen::Drawing::TextBlob::MakeFromString(viewPrs.c_str(), 
        Rosen::Drawing::Font(nullptr, TEXT_SIZE, TEXT_SCALE, TEXT_SKEW), Rosen::Drawing::TextEncoding::UTF8);
    rect.left_ += itemW + TEXT_SPACEING;
    rect.right_ += itemW + TEXT_SPACEING;
    DrawRectItem(labelsCanvas_, viewPrs, rect, Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 0, 0));
    labelsCanvasNode_->FinishRecording();
    MMI_HILOGI("ZXH::1111111111111END");
}

void TouchDrawingManager::DrawRectItem(RosenCanvas* canvas, const std::string &text,
    const Rosen::Drawing::Rect &rect, const Rosen::Drawing::Color &color)
{
    MMI_HILOGI("ZXH::DrawRectItem start");
    std::shared_ptr<Rosen::Drawing::TextBlob> textBlob = Rosen::Drawing::TextBlob::MakeFromString(text.c_str(), 
        Rosen::Drawing::Font(nullptr, TEXT_SIZE, TEXT_SCALE, TEXT_SKEW), Rosen::Drawing::TextEncoding::UTF8);
    rectBrush_.SetColor(color);
    canvas->AttachBrush(rectBrush_);
    canvas->DrawRect(rect);
    canvas->DetachBrush();
    canvas->AttachBrush(textBrush_);
    canvas->DrawTextBlob(textBlob.get(), rect.left_ + TEXT_SPACEING, RECT_TOP + 36);
    canvas->DetachBrush();
    MMI_HILOGI("ZXH::DrawRectItem end");
}

void TouchDrawingManager::UpdatePointerPositionInfo()
{
    CHKPV(pointerEvent_);
    int32_t pointerAction = pointerEvent_->GetPointerAction();
    int32_t pointerId = pointerEvent_->GetPointerId();
    MMI_HILOGD("pointerAction is: %{public}d, pointerId is: %{public}d", pointerAction, pointerId);
    if (pointerAction == PointerEvent::POINTER_ACTION_DOWN || pointerAction == PointerEvent::POINTER_ACTION_PULL_DOWN) {
        if (lastPointerItem_.empty()) {
            isFirstDownAction_ = true;
            isDownAction_ = true;
            maxPointerCount_ = 0;
            currentPointerCount_ = 0;
            currentPointerId_ = pointerId;
            xVelocity_ = 0.0;
            yVelocity_ = 0.0;      
        }
        currentPointerCount_ += 1;
        if (currentPointerCount_ > maxPointerCount_) {
            maxPointerCount_ = currentPointerCount_;
        }
    } else if (pointerAction == PointerEvent::POINTER_ACTION_UP || pointerAction == PointerEvent::POINTER_ACTION_PULL_UP) {
        for (auto it = lastPointerItem_.begin(); it != lastPointerItem_.end(); ++it) {
            if (it->GetPointerId() == pointerId) {
                lastPointerItem_.erase(it);
                MMI_HILOGD("erase item id: %{public}d", pointerId);
                break;
            }
        }

        if (lastPointerItem_.empty()) {
            MMI_HILOGD("lastPointerItem_ is empty");
            isDownAction_ = false;
            currentPointerCount_ = 0;
            isFirstDownAction_ = false;
            
        } else {
            --currentPointerCount_;
            if (currentPointerId_ == pointerId) {
                currentPointerId_ = lastPointerItem_.front().GetPointerId();
                isUpAction_ = true;
                MMI_HILOGD("current action is up");
            }
        }
        isUpAction_ = true;
    }
    if (isFirstDownAction_) {
        isFirstDownAction_ = false;
        PointerEvent::PointerItem pointerItem;
        if (!pointerEvent_->GetPointerItem(pointerId, pointerItem)) {
            MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
            return;
        }
        currentPointerItem_ = pointerItem;
    }
    UpdateVelocity();
    UpdateDisplayCoord();
}

void TouchDrawingManager::UpdateLastPointerItem(int32_t pointerId, PointerEvent::PointerItem &pointerItem)
{
    if (!pointerItem.IsPressed()) {
        return;
    }
    for (auto &item : lastPointerItem_) {
        if (item.GetPointerId() == pointerId) {
            item = pointerItem;
            return;
        }
    }
    lastPointerItem_.emplace_back(pointerItem);
}

void TouchDrawingManager::UpdateVelocity()
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent_);
    auto pointerId = pointerEvent_->GetPointerId();
    int64_t actionTime = pointerEvent_->GetActionTime();
    if (pointerId == currentPointerId_) {
        MMI_HILOGE("pointerId is %{public}d", pointerId);
        if (!lastPointerItem_.empty()) {
            PointerEvent::PointerItem pointerItem;
            if (!pointerEvent_->GetPointerItem(pointerId, pointerItem)) {
                MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
                return;
            }
            int32_t physicalX = pointerItem.GetDisplayX();
            int32_t physicalY = pointerItem.GetDisplayY();
            float diffTime = (actionTime - lastActionTime_) / 1000;
            MMI_HILOGE("diffTime is %{public}lf", diffTime);
            if (diffTime == 0) {
                xVelocity_ = 0.0;
                yVelocity_ = 0.0;
                MMI_HILOGE("diffTime is 0");
                return;
            }
            auto diffX = physicalX - lastPointerItem_.front().GetDisplayX();
            auto diffY = physicalY - lastPointerItem_.front().GetDisplayY();
            xVelocity_ = diffX / diffTime;
            yVelocity_ = diffY / diffTime;
        }
        lastActionTime_ = actionTime;
    }
}

void TouchDrawingManager::UpdateDisplayCoord()
{
    CHKPV(pointerEvent_);
    PointerEvent::PointerItem pointerItem;
    if (!pointerEvent_->GetPointerItem(currentPointerId_, pointerItem)) {
        MMI_HILOGE("Can't find pointer item, pointer:%{public}d", currentPointerId_);
        return;
    }
    if (pointerItem.IsPressed()) {
        currentPhysicalX_ = pointerItem.GetDisplayX();
        currentPhysicalY_ = pointerItem.GetDisplayY();
        pressure_ = pointerItem.GetPressure();
    }
}

void TouchDrawingManager::ClearPointerPosition()
{
    MMI_HILOGI("ZXH::BeginRecording");
    auto canvas = static_cast<RosenCanvas *>(labelsCanvasNode_->BeginRecording(displayInfo_.width, displayInfo_.height));
    CHKPV(canvas);
//    canvas->Clear(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    Rosen::Drawing::Rect rect;
    rect.left_ = 0;
    rect.right_ = displayInfo_.width;
    rect.top_ = RECT_TOP;
    rect.bottom_ = RECT_TOP + RECT_HEIGHT;
    rectBrush_.SetColor(Rosen::Drawing::Color::COLOR_WHITE);
    canvas->AttachBrush(rectBrush_);
    canvas->DrawRect(rect);
    canvas->DetachBrush();
    labelsCanvasNode_->FinishRecording();
    MMI_HILOGI("ZXH::FinishRecording");
}

{
    MMI_HILOGI("ClearTracker start");
    CHKPV(trackerCanvasNode_);
    if (lastPointerItem_.empty() && isDownAction_) {
       auto canvasNode = static_cast<Rosen::RSCanvasDrawingNode*>(trackerCanvasNode_.get());
       canvasNode->ResetSurface();
    }
}

bool TouchDrawingManager::IsValidAction(const int32_t action)
{
    if (action == PointerEvent::POINTER_ACTION_DOWN || action == PointerEvent::POINTER_ACTION_PULL_DOWN ||
        action == PointerEvent::POINTER_ACTION_MOVE || action == PointerEvent::POINTER_ACTION_PULL_MOVE ||
        action == PointerEvent::POINTER_ACTION_UP || action == PointerEvent::POINTER_ACTION_PULL_UP) {
        return true;
    }
    return false;
}

template <class T>
inline void TouchDrawingManager::CreateBubbleObserver(T &item)
{
    CALL_DEBUG_ENTER;
    SettingObserver::UpdateFunc updateFunc = [&item](const std::string& key) {
        auto ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .GetBoolValue(key, item.isShow);
        if (ret != RET_OK) {
            MMI_HILOGE("Get value from setting date fail");
            return;
        }
        MMI_HILOGI("key: %{public}s, statusValue: %{public}d", key.c_str(), item.isShow);
    };
    sptr<SettingObserver> statusObserver = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
        .CreateObserver(item.SwitchName, updateFunc);
    ErrCode ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).RegisterObserver(statusObserver);
    if (ret != ERR_OK) {
        MMI_HILOGE("register setting observer failed, ret=%{public}d", ret);
        statusObserver = nullptr;
    }
}

template <class T>
inline void TouchDrawingManager::CreatePointerObserver(T &item)
{
    CALL_DEBUG_ENTER;
    SettingObserver::UpdateFunc updateFunc = [&item](const std::string& key) {
        auto ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .GetBoolValue(key, item.isShow);
        if (ret != RET_OK) {
            MMI_HILOGE("Get value from setting date fail");
            return;
        }
        TouchDrawingMgr->UpdateLabels(item.isShow);
        MMI_HILOGI("key: %{public}s, statusValue: %{public}d", key.c_str(), item.isShow);
    };
    sptr<SettingObserver> statusObserver = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
        .CreateObserver(item.SwitchName, updateFunc);
    ErrCode ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).RegisterObserver(statusObserver);
    if (ret != ERR_OK) {
        MMI_HILOGE("register setting observer failed, ret=%{public}d", ret);
        statusObserver = nullptr;
    }
}

template <class T>
std::string TouchDrawingManager::FormatNumber(T& number, int32_t precision)
{
    std::string temp(".000");
    auto str = std::to_string(number);
    if (str.find(".") == std::string::npos) {
        str += temp;
    }
    return str.substr(0, str.find(".") + precision + 1);
}
} // namespace MMI
} // namespace OHOS