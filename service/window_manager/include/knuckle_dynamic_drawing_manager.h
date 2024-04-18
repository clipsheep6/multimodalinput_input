/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef KNUCKLE_DYNAMIC_DRAWING_MANAGER_H
#define KNUCKLE_DYNAMIC_DRAWING_MANAGER_H

#include <ui/rs_canvas_node.h>
#include <ui/rs_surface_node.h>
#include <transaction/rs_transaction.h>
#include <vector>

#include "knuckle_glow_trace_system.h"
#include "include/core/SkPath.h"
#include "include/core/SkPaint.h"
#include "draw/canvas.h"
#include "utils/point.h"
#include "nocopyable.h"
#include "pointer_event.h"
#include "window_info.h"
#include "singleton.h"


namespace OHOS {
namespace MMI {
struct KnuBubble {
    int32_t innerCircleRadius { 0 };
    int32_t outerCircleRadius { 0 };
    float outerCircleWidth { 0 };
};

class KnuckleDynamicDrawingManager {

public:
    KnuckleDynamicDrawingManager();
    ~KnuckleDynamicDrawingManager();
    void KnuckleDynamicDrawHandler(const std::shared_ptr<PointerEvent> pointerEvent);
    void UpdateDisplayInfo(const DisplayInfo& displayInfo);
    void GetOriginalTouchScreenCoordinates(Direction direction, int32_t width, int32_t height,
                                           int32_t &physicalX, int32_t &physicalY);

private:
    void StartTouchDraw(const std::shared_ptr<PointerEvent> pointerEvent);
    void CreateTouchWindow(const int32_t displayId);
    void CreateCanvasNode();
    int32_t DrawGraphic(const std::shared_ptr<PointerEvent> pointerEvent);

    bool CheckPointerAction(const std::shared_ptr<PointerEvent> pointerEvent);
    void ProcessUpAndCancelEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    void ProcessDownEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    void ProcessMoveEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    void InitPointerPathPaint();
    void UpdateTrackColors();
    std::shared_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(const std::string &imagePath);
    int32_t GetPointerSize();
    std::shared_ptr<Rosen::Drawing::Bitmap> PixelMapToBitmap(
    std::shared_ptr<Media::PixelMap>& pixelMap);
    Rosen::Drawing::AlphaType AlphaTypeToAlphaType(Media::AlphaType alphaType);
    Rosen::Drawing::ColorType PixelFormatToColorType(Media::PixelFormat pixelFormat);

    void TestDrawPixelmap(Rosen::Drawing::CoreCanvas* canvas, uint32_t width, uint32_t height);

private:
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_;
    DisplayInfo displayInfo_ {};
    uint64_t screenId_ { 0 };
    KnuBubble bubble_;
    Rosen::Drawing::Brush brush_;
    Rosen::Drawing::Pen pen_;
    std::vector<Rosen::Drawing::Point> mTraceControlpoints_;

    int mPointCounter = 0;
    bool mIsDrawing_ = false;
    std::shared_ptr<KnuckleGlowTraceSystem> mGlowTraceSystem_;
    std::shared_ptr<OHOS::Media::PixelMap> mpixelMap_;
    // SkPath mPointerPath_;
    Rosen::Drawing::Path mPointerPath_;
    SkPaint mPointerPathPaint_;
    int64_t mLastUpdateTimeMillis_;

    static float PAINT_STROKE_WIDTH;
    static float PAINT_PATH_RADIUS;
    static float DOUBLE;
    static int POINT_TOTAL_SIZE;
    static int POINT_SYSTEM_SIZE;
    static int MAX_DIVERGENCE_NUM;
    static int MAX_UPDATE_TIME_MILLIS;
};
} // namespace MMI
} // namespace OHOS
#endif // KNUCKLE_DYNAMIC_DRAWING_MANAGER_H
