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

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"

#include "define_multimodal.h"
#include "mmi_log.h"
#include "i_multimodal_input_connect.h"
#include "input_device_manager.h"
#include "input_windows_manager.h"
#include "ipc_skeleton.h"
#include "mmi_log.h"
#include "multimodal_input_preferences_manager.h"
#include "preferences.h"
#include "preferences_impl.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "preferences_xml_utils.h"
#include "render/rs_pixel_map_util.h"
#include "setting_datashare.h"
#include "util.h"
#include "timer_manager.h"
#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "text/font_mgr.h"



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
const std::string showCursorSwitchName = "settings.input.show_touch_hint";
const std::string showPointerPositionSwitchName = "settings.input.show_touch_hint";
} // namespace

TouchDrawingManager::TouchDrawingManager()
{
    brush_.SetColor(Rosen::Drawing::Color::COLOR_WHITE);
    brush_.SetAntiAlias(true);
    float innerCircleTransparency = 0.6f;
    brush_.SetAlphaF(innerCircleTransparency);

    pen_.SetColor(Rosen::Drawing::Color::COLOR_BLACK);
    pen_.SetAntiAlias(true);
    float outerCircleTransparency = 0.1f;
    pen_.SetAlphaF(outerCircleTransparency);
}

TouchDrawingManager::~TouchDrawingManager() {}

void TouchDrawingManager::TouchDrawHandler(const std::shared_ptr<PointerEvent>& pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    pointerEvent_ = pointerEvent;
    CreateObserver();
    CreateCanvasNode(bubbleMode_.isShow, bubbleCanvasNode_);
    CreateCanvasNode(pointerMode_.isShow, pointerCanvasNode_);   
    CreateTouchWindow();
    if (bubbleMode_.isShow) {
        DrawBubbleHandler();
    }
    if (pointerMode_.isShow) {
        ClearTracker();
        UpdatePointerPositionInfo();
        DrawPointerPositionHandler();
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
#ifndef USE_ROSEN_DRAWING
    bubbleCanvas_ = static_cast<Rosen::RSRecordingCanvas *>(bubbleCanvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#else
    bubbleCanvas_ = static_cast<Rosen::Drawing::RecordingCanvas *>(bubbleCanvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#endif
    CHKPV(bubbleCanvas_);

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
        pen_.SetWidth(bubble_.outerCircleWidth);
        bubbleCanvas_->AttachPen(pen_);
        bubbleCanvas_->DrawCircle(centerPt, bubble_.outerCircleRadius);
        bubbleCanvas_->DetachPen();

        bubbleCanvas_->AttachBrush(brush_);
        bubbleCanvas_->DrawCircle(centerPt, bubble_.innerCircleRadius);
        bubbleCanvas_->DetachBrush();
    }
    bubbleCanvasNode_->FinishRecording();
}

void TouchDrawingManager::DrawPointerPositionHandler()
{
    CHKPV(pointerEvent_);
    CHKPV(pointerCanvasNode_);
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
        int32_t longAxis = pointerItem.GetLongAxis();
        int32_t shortAxis = pointerItem.GetShortAxis();
        DrawOval(displayX, displayY, longAxis,shortAxis);
        UpdateLastPointerItem(pointerId, pointerItem);
    }
    DrawLabels();
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void TouchDrawingManager::DrawTracker(int32_t x, int32_t y, int32_t pointerId)
{
    CALL_DEBUG_ENTER;
    if (lastPointerItem_.empty()) {
        MMI_HILOGD("last pointer item is empty.");
        return;
    }
    Rosen::Drawing::Point centerPt(x, y);
    for (auto &item : lastPointerItem_) {
        if (item.GetPointerId() == pointerId) {
            x = item.GetDisplayX();
            y = item.GetDisplayY();
            break;
        }
    }
	Rosen::Drawing::Point lastCenterPt(x, y);
    pathPen_.SetWidth(displayInfo_.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE);
    trackerCanvas_->AttachPen(pathPen_);
    trackerCanvas_->DrawLine(lastCenterPt, centerPt);
    trackerCanvas_->DetachPen();
    pointPen_.SetWidth(displayInfo_.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE);
    trackerCanvas_->AttachPen(pointPen_);
    trackerCanvas_->DrawPoint(lastCenterPt);
    trackerCanvas_->DetachPen();
}

void TouchDrawingManager::DrawCrosshairs(int32_t x, int32_t y)
{
    crosshairsPen_.SetWidth(displayInfo_.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE);
    trackerCanvas_->AttachPen(crosshairsPen_);
    Rosen::Drawing::Point left(0, y);
    Rosen::Drawing::Point right(displayInfo_.width, y);
    trackerCanvas_->DrawLine(left, right);
    Rosen::Drawing::Point top(x, 0);
    Rosen::Drawing::Point bottom(x, displayInfo_.height);
    trackerCanvas_->DrawLine(top, bottom);
    trackerCanvas_->DetachPen();
}

void TouchDrawingManager::DrawOval(int32_t x, int32_t y,
                                    int32_t longAxis, int32_t shortAxis)
{
    rect_.left_ = x - shortAxis / 2;
    rect_.right_ = y + shortAxis / 2;
    rect_.top_ = y - longAxis / 2;
    rect_.bottom_ = y + longAxis / 2;
    trackerCanvas_->AttachPen(crosshairsPen_);
    trackerCanvas_->DrawOval(rect_);
    trackerCanvas_->DetachPen();
}

void TouchDrawingManager::DrawLabels()
{
    int w = displayInfo_.width - 20;
    int itemW = w / 6;
    int headerPaddingTop = 150;
    int headerBottom = headerPaddingTop + 24 + 10;
    std::string viewP = "P " + std::to_string(currentPointerCount_) + " / " + std::to_string(maxPointerCount_);
    std::string viewX = "X " + std::to_string(currentPhysicalX_);
    std::string viewY = "Y " + std::to_string(currentPhysicalY_);
    std::string viewDx = "dX " + std::to_string((currentPhysicalX_ - currentPointerItem_.GetDisplayX()));
    std::string viewDy = "dY " + std::to_string((currentPhysicalY_ - currentPointerItem_.GetDisplayY()));
    std::string viewXv = "Xv " + std::to_string(xVelocity_);
    std::string viewYv = "Yv " + std::to_string(yVelocity_);
    std::string viewPrs = "Prs " + std::to_string(pressure_);
//    auto fontMgr = Rosen::Drawing::FontMgr::CreateDefaultFontMgr();
//    std::shared_ptr<Rosen::Drawing::Typeface> typeFace = 
//        Rosen::Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Regular.ttf");
    std::shared_ptr<Rosen::Drawing::TextBlob> textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewP.c_str(), 
        Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
    trackerCanvas_->Save();
    trackerCanvas_->Translate(10, 0);
    brush_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(128, 255, 255, 255));
    trackerCanvas_->AttachBrush(brush_);
    auto bounds = textBlob->Bounds();
    rect_.left_ = bounds->GetLeft();
    rect_.right_ = bounds->GetRight();
    rect_.top_ = bounds->GetTop();
    rect_.bottom_ = bounds->GetBottom();
    trackerCanvas_->DrawRect(rect_);
    trackerCanvas_->DetachBrush();
	textPen_.SetWidth(displayInfo_.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE);
    trackerCanvas_->AttachPen(textPen_);
    textBrush_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(128, 0, 0, 0));
    trackerCanvas_->AttachBrush(textBrush_);
    trackerCanvas_->DrawTextBlob(textBlob.get(), 1, headerPaddingTop + 5);
    trackerCanvas_->DetachBrush();
//    trackerCanvas_->DetachPen();
    if (isDownAction_ || !lastPointerItem_.empty()) {
        MMI_HILOGD("viewX is :%{public}s", viewX.c_str());
        MMI_HILOGD("viewY is :%{public}s", viewY.c_str());
        DrawRect(itemW, headerPaddingTop, (itemW * 2) -1 , headerBottom);
        textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewX.c_str(), 
            Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
        trackerCanvas_->AttachPen(textPen_);
        trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + itemW, headerPaddingTop + 5);
        trackerCanvas_->DetachPen();
        DrawRect(itemW * 2, headerPaddingTop, (itemW * 3) -1 , headerBottom);
        textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewY.c_str(), 
            Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
        trackerCanvas_->AttachPen(textPen_);
        trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + (itemW * 2), headerPaddingTop + 5);
        trackerCanvas_->DetachPen();
    } else {
        MMI_HILOGE("viewDx is :%{public}s", viewDx.c_str());
        MMI_HILOGE("viewDy is :%{public}s", viewDy.c_str());
        brush_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 0, 0));
        DrawRect(itemW, headerPaddingTop, (itemW * 2) -1 , headerBottom);
        textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewDx.c_str(), 
            Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
        trackerCanvas_->AttachPen(textPen_);
        trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + itemW, headerPaddingTop + 5);
        trackerCanvas_->DetachPen();
        DrawRect(itemW * 2, headerPaddingTop, (itemW * 3) -1 , headerBottom);
        textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewDy.c_str(), 
            Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
        trackerCanvas_->AttachPen(textPen_);
        trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + (itemW * 2), headerPaddingTop + 5);
        trackerCanvas_->DetachPen();
    }	
	brush_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(128, 255, 255, 255));
    DrawRect(itemW * 3, headerPaddingTop, (itemW * 4) -1 , headerBottom);
    textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewXv.c_str(), 
        Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
    trackerCanvas_->AttachPen(textPen_);
    trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + (itemW * 3), headerPaddingTop + 5);
    trackerCanvas_->DetachPen();
    DrawRect(itemW * 4, headerPaddingTop, (itemW * 5) -1 , headerBottom);
    textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewYv.c_str(), 
        Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
    trackerCanvas_->AttachPen(textPen_);
    trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + (itemW * 4), headerPaddingTop + 5);
    trackerCanvas_->DetachPen();
    brush_.SetColor(Rosen::Drawing::Color::ColorQuadSetARGB(192, 255, 0, 0));
    DrawRect(itemW * 5, headerPaddingTop, (itemW * 6) -1 , headerBottom);
    textBlob = Rosen::Drawing::TextBlob::MakeFromString(viewPrs.c_str(), 
        Rosen::Drawing::Font(nullptr, 24.0f, 1.0f, 0.0f), Rosen::Drawing::TextEncoding::UTF8);
    trackerCanvas_->AttachPen(textPen_);
    trackerCanvas_->DrawTextBlob(textBlob.get(), 1 + (itemW * 5), headerPaddingTop + 5);
    trackerCanvas_->DetachPen();
    trackerCanvas_->Restore();
}

void TouchDrawingManager::DrawRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    rect_.left_ = left;
    rect_.right_ = right;
    rect_.top_ = top;
    rect_.bottom_ = bottom;
    trackerCanvas_->AttachBrush(brush_);
    trackerCanvas_->DrawRect(rect_);
    trackerCanvas_->DetachBrush();
}

void TouchDrawingManager::ClearPointerPosition()
{
    ClearTracker();
}

void TouchDrawingManager::ClearTracker()
{
    if (lastPointerItem_.empty() && isDownAction_) {
//        pointerCanvasNode_->ResetSurface();
    }
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
            lastPointerItem_.clear();
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
    if (isUpAction_) {
        xVelocity_ = 0.0;
        yVelocity_ = 0.0;
        isUpAction_ = false;
        MMI_HILOGD("actionPointerUp_ is true");
        return;
    }
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
            MMI_HILOGE("diffX is %{public}d", diffX);
            MMI_HILOGE("diffY is %{public}d", diffY);
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

void TouchDrawingManager::UpdateLabels(bool isShow)
{
    if (isShow)
    {
        CreateCanvasNode(isShow, pointerCanvasNode_);
        CreateTouchWindow();
        CreateCanvas(pointerCanvasNode_);
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
        pointerMode_.SwitchName = showCursorSwitchName;
        CreatePointerObserver(pointerMode_);
        hasPointerObserver_ = true;
    }
}

void TouchDrawingManager::CreateCanvasNode(bool isShow, std::shared_ptr<Rosen::RSCanvasNode>& canvasNode)
{
    if (!isShow) {
        MMI_HILOGD("canvasNode is already.");
        return;
    }
    if (canvasNode != nullptr) {
        MMI_HILOGD("canvasNode is already.");
        return;
    }
    canvasNode = Rosen::RSCanvasNode::Create();
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

}

void TouchDrawingManager::CreateTouchWindow()
{
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
    if (bubbleCanvasNode_ != nullptr && bubbleMode_.isShow) {
        surfaceNode_->AddChild(bubbleCanvasNode_, DEFAULT_VALUE);
    }
    if (pointerCanvasNode_ != nullptr && pointerMode_.isShow) {
        surfaceNode_->AddChild(pointerCanvasNode_, DEFAULT_VALUE);
    }
    surfaceNode_->AttachToDisplay(static_cast<uint64_t>(pointerEvent_->GetTargetDisplayId()));
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void TouchDrawingManager::CreateCanvas(std::shared_ptr<Rosen::RSCanvasNode>& canvasNode)
{
#ifndef USE_ROSEN_DRAWING
    trackerCanvas_ = static_cast<Rosen::RSRecordingCanvas *>(pointerCanvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#else
    trackerCanvas_ = static_cast<Rosen::Drawing::RecordingCanvas *>(pointerCanvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#endif
}

} // namespace MMI
} // namespace OHOS