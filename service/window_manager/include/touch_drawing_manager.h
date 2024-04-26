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

#ifndef TOUCH_DRAWING_MANAGER_H
#define TOUCH_DRAWING_MANAGER_H

#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <transaction/rs_transaction.h>

#include "draw/canvas.h"
#include "nocopyable.h"
#include "pointer_event.h"
#include "window_info.h"
#include "singleton.h"
#include "setting_observer.h"

#include "ui/rs_canvas_drawing_node.h"
#include "utils/rect.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "recording/recording_canvas.h"
#include "ui/rs_canvas_drawing_node.h"
#endif
namespace OHOS {
namespace MMI {
struct Bubble {
    int32_t innerCircleRadius { 0 };
    int32_t outerCircleRadius { 0 };
    float outerCircleWidth { 0 };
};
struct DevMode {
    std::string SwitchName;
    bool isShow { false };
};
class TouchDrawingManager {
    DECLARE_DELAYED_SINGLETON(TouchDrawingManager);
public:
    DISALLOW_COPY_AND_MOVE(TouchDrawingManager);
    void TouchDrawHandler(const std::shared_ptr<PointerEvent>& pointerEvent);
    void UpdateDisplayInfo(const DisplayInfo& displayInfo);
    void GetOriginalTouchScreenCoordinates(Direction direction, int32_t width, int32_t height,
        int32_t &physicalX, int32_t &physicalY);
    void UpdateLabels(bool isShow);

private:
    void CreateObserver();
    void CreateCanvasNode(std::shared_ptr<Rosen::RSCanvasNode>& canvasNode);
    void CreateTouchWindow();
#ifndef USE_ROSEN_DRAWING
    Rosen::RSRecordingCanvas*
#else
    Rosen::Drawing::RecordingCanvas*
#endif
    CreateCanvas(const std::shared_ptr<Rosen::RSCanvasNode>& canvasNode);
    void DrawBubbleHandler();
    void DrawBubble();
    void DrawPointerPositionHandler();
    void DrawTracker(int32_t x, int32_t y, int32_t pointerId);
    void DrawCrosshairs(int32_t x, int32_t y);
    void DrawOval(int32_t x, int32_t y, int32_t longAxis, int32_t shortAxis);
    void DrawLabels();
    void DrawRect(int32_t left, int32_t top, int32_t right, int32_t bottom);
    void ClearPointerPosition();
    void ClearTracker();
    void UpdatePointerPositionInfo();
    void UpdateLastPointerItem(int32_t pointerId, PointerEvent::PointerItem &pointerItem);

    bool IsValidAction(const int32_t action);
    void UpdateVelocity();
    void UpdateDisplayCoord();
    
    template <class T>
    void CreateBubbleObserver(T& item);
    template <class T>
    void CreatePointerObserver(T& item);

private:
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_ { nullptr };
    std::shared_ptr<Rosen::RSCanvasNode> bubbleCanvasNode_ { nullptr };
    std::shared_ptr<Rosen::RSCanvasNode> pointerCanvasNode_ { nullptr };
//    std::shared_ptr<Rosen::RSCanvasDrawingNode>  pointerCanvasNode_ { nullptr };
    DisplayInfo displayInfo_ {};
    uint64_t screenId_ { 0 };
    Bubble bubble_;
    Rosen::Drawing::Brush brush_;
    Rosen::Drawing::Pen pen_;
    std::shared_ptr<PointerEvent> pointerEvent_{ nullptr };
    DevMode bubbleMode_;
    DevMode pointerMode_;
    bool hasBubbleObserver_{ false };
    bool hasPointerObserver_{ false };
    int32_t currentPointerId_ { 0 };
    int32_t maxPointerCount_ { 0 };
    int32_t currentPointerCount_ { 0 };
    int64_t lastActionTime_ { 0 };
    float xVelocity_ { 0.0 };
    float yVelocity_ { 0.0 };
    int32_t currentPhysicalX_ { 0 };
    int32_t currentPhysicalY_ { 0 };

    double pressure_ { 0.0 };
    Rosen::Drawing::Brush textBrush_;
    Rosen::Drawing::Pen textPen_;
    Rosen::Drawing::Pen pathPen_;
    Rosen::Drawing::Pen pointPen_;
    Rosen::Drawing::Pen crosshairsPen_;
    bool isFirstDownAction_ { false };
    bool isUpAction_ { false };
    bool isDownAction_ { false };
    std::list<PointerEvent::PointerItem> lastPointerItem_;
    Rosen::Drawing::Rect rect_;
    PointerEvent::PointerItem currentPointerItem_;
#ifndef USE_ROSEN_DRAWING
    Rosen::RSRecordingCanvas *trackerCanvas_;
//    Rosen::RSRecordingCanvas *bubbleCanvas_;
#else
    Rosen::Drawing::RecordingCanvas *trackerCanvas_;
//    Rosen::Drawing::RecordingCanvas *bubbleCanvas_;
#endif

};
#define TouchDrawingMgr ::OHOS::DelayedSingleton<TouchDrawingManager>::GetInstance()

} // namespace MMI
} // namespace OHOS
#endif // TOUCH_DRAWING_MANAGER_H
