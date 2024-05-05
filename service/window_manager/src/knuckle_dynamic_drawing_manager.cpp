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

#include "knuckle_dynamic_drawing_manager.h"

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"

#include "define_multimodal.h"
#include "mmi_log.h"
#include "multimodal_input_preferences_manager.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "recording/recording_canvas.h"
#include "ui/rs_canvas_drawing_node.h"
#endif
#include "render/rs_pixel_map_util.h"

#include <fstream>
#include "pixel_map.h"

namespace OHOS {
namespace MMI {
namespace {
const std::string IMAGE_POINTER_PENTAGRAM_PATH = "/system/etc/multimodalinput/mouse_icon/";
const std::string PentagramIconPath = IMAGE_POINTER_PENTAGRAM_PATH + "knuckle_sprite_360.png";
// const std::string PentagramIconPath = IMAGE_POINTER_PENTAGRAM_PATH + "Default.svg";
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KnuckleDynamicDrawingManager" };
constexpr int32_t DENSITY_BASELINE = 160;
constexpr int32_t INDEPENDENT_INNER_PIXELS = 20;
constexpr int32_t INDEPENDENT_OUTER_PIXELS = 21;
constexpr int32_t INDEPENDENT_WIDTH_PIXELS = 2;
constexpr int32_t CALCULATE_MIDDLE = 2;
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t MAX_POINTER_COLOR = 0x00ffff;
} // namespace

float KnuckleDynamicDrawingManager::PAINT_STROKE_WIDTH = 10.0f;
float KnuckleDynamicDrawingManager::PAINT_PATH_RADIUS = 10.0f;
float KnuckleDynamicDrawingManager::DOUBLE = 2.0f;
int KnuckleDynamicDrawingManager::POINT_TOTAL_SIZE = 5;
int KnuckleDynamicDrawingManager::POINT_SYSTEM_SIZE = 500;
int KnuckleDynamicDrawingManager::MAX_DIVERGENCE_NUM = 10;
int KnuckleDynamicDrawingManager::MAX_UPDATE_TIME_MILLIS = 500;
constexpr int32_t DEFAULT_POINTER_SIZE = 1;

KnuckleDynamicDrawingManager::KnuckleDynamicDrawingManager()
{
    InitPointerPathPaint();
}

KnuckleDynamicDrawingManager::~KnuckleDynamicDrawingManager() {}

int32_t KnuckleDynamicDrawingManager::GetPointerSize()
{
    CALL_DEBUG_ENTER;
    std::string name = "pointerSize";
    int32_t pointerSize = PreferencesMgr->GetIntValue(name, DEFAULT_POINTER_SIZE);
    return pointerSize;
}

std::shared_ptr<OHOS::Media::PixelMap> KnuckleDynamicDrawingManager::DecodeImageToPixelMap(const std::string &imagePath)
{
    CALL_DEBUG_ENTER;
    OHOS::Media::SourceOptions opts;
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    CHKPP(imageSource);
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = 80,
        .height = 80
    };
 
    decodeOpts.SVGOpts.fillColor = {.isValidColor = false, .color = MAX_POINTER_COLOR};
    decodeOpts.SVGOpts.strokeColor = {.isValidColor = false, .color = MAX_POINTER_COLOR};

    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        MMI_HILOGE("The pixelMap is nullptr");
    }
    mpixelMap_ = pixelMap;
    return pixelMap;
}

Rosen::Drawing::AlphaType KnuckleDynamicDrawingManager::AlphaTypeToAlphaType(Media::AlphaType alphaType)
{
    CALL_DEBUG_ENTER;
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Rosen::Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Rosen::Drawing::AlphaType::ALPHATYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Rosen::Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Rosen::Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

Rosen::Drawing::ColorType KnuckleDynamicDrawingManager::PixelFormatToColorType(Media::PixelFormat pixelFormat)
{
     switch (pixelFormat) {
        case Media::PixelFormat::RGB_565:
            return Rosen::Drawing::ColorType::COLORTYPE_RGB_565;
        case Media::PixelFormat::RGBA_8888:
            return Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888;
        case Media::PixelFormat::BGRA_8888:
            return Rosen::Drawing::ColorType::COLORTYPE_BGRA_8888;
        case Media::PixelFormat::ALPHA_8:
            return Rosen::Drawing::ColorType::COLORTYPE_ALPHA_8;
        case Media::PixelFormat::RGBA_F16:
            return Rosen::Drawing::ColorType::COLORTYPE_RGBA_F16;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return Rosen::Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

std::shared_ptr<Rosen::Drawing::Bitmap> KnuckleDynamicDrawingManager::PixelMapToBitmap(
    std::shared_ptr<Media::PixelMap>& pixelMap)
{
    CALL_DEBUG_ENTER;
    auto data = pixelMap->GetPixels();
    Rosen::Drawing::Bitmap bitmap;
    Rosen::Drawing::ColorType colorType = PixelFormatToColorType(pixelMap->GetPixelFormat());
    Rosen::Drawing::AlphaType alphaType = AlphaTypeToAlphaType(pixelMap->GetAlphaType());
    Rosen::Drawing::ImageInfo imageInfo(pixelMap->GetWidth(), pixelMap->GetHeight(), colorType, alphaType);
    bitmap.Build(imageInfo);
    bitmap.SetPixels(const_cast<uint8_t*>(data));
    return std::make_shared<Rosen::Drawing::Bitmap>(bitmap);
}

void KnuckleDynamicDrawingManager::InitPointerPathPaint()
{
    CALL_DEBUG_ENTER;
    for (int i = 0; i < POINT_TOTAL_SIZE; i++) {
        Rosen::Drawing::Point point = Rosen::Drawing::Point();
        mTraceControlpoints_.push_back(point);
    }
    mpixelMap_ = DecodeImageToPixelMap(PentagramIconPath);
    CHKPV(mpixelMap_);
    auto bitmap = PixelMapToBitmap(mpixelMap_);
    CHKPV(bitmap);
    if (mGlowTraceSystem_ == nullptr) {
        mGlowTraceSystem_ = std::make_shared<KnuckleGlowTraceSystem>(POINT_SYSTEM_SIZE, *bitmap, MAX_DIVERGENCE_NUM);
    }
    mPointerPathPaint_.setStyle(SkPaint::Style::kStroke_Style);
    mPointerPathPaint_.setStrokeJoin(SkPaint::Join::kRound_Join);
    mPointerPathPaint_.setStrokeCap(SkPaint::Cap::kRound_Cap);
    mPointerPathPaint_.setStrokeWidth(PAINT_STROKE_WIDTH);
    mPointerPathPaint_.setAntiAlias(true);
}

void KnuckleDynamicDrawingManager::UpdateTrackColors()
{
    CALL_DEBUG_ENTER;
    mPointerPathPaint_.setColor(0xFFCCCCCC);
}

void KnuckleDynamicDrawingManager::KnuckleDynamicDrawHandler(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    if (!IsSingleKnuckle(pointerEvent)) {
        return;
    }
    auto displayId = pointerEvent->GetTargetDisplayId();
    CreateTouchWindow(displayId);
    if (CheckPointerAction(pointerEvent)) {
        StartTouchDraw(pointerEvent);
    }
}

bool KnuckleDynamicDrawingManager::IsSingleKnuckle(const std::shared_ptr<PointerEvent> touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(touchEvent);
    auto id = touchEvent->GetPointerId();
    PointerEvent::PointerItem item;
    touchEvent->GetPointerItem(id, item);
    MMI_HILOGI("item.GetToolType() = %{public}d", item.GetToolType());
    if (item.GetToolType() != PointerEvent::TOOL_TYPE_KNUCKLE ||
        touchEvent->GetPointerIds().size() != 1) {
        if (canvasNode_ != nullptr) {
            MMI_HILOGI("canvasNode_ != nullptr");
            isStop = true;
            mTraceControlpoints_.clear();
            mPointerPath_.Reset();
            canvasNode_->ResetSurface();
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
        return false;
    }
    return true;
}

bool KnuckleDynamicDrawingManager::CheckPointerAction(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    size_t size = pointerEvent->GetPointerIds().size();
    if (size > 1) {
        mPointerPath_.Reset();
        mGlowTraceSystem_->Clear();
        return false;
    }
    switch (pointerEvent->GetPointerAction()) {
        case PointerEvent::POINTER_ACTION_UP:
        case PointerEvent::POINTER_ACTION_PULL_UP:
            ProcessUpAndCancelEvent(pointerEvent);
            return false;
            break;
        case PointerEvent::POINTER_ACTION_DOWN:
        case PointerEvent::POINTER_ACTION_PULL_DOWN:
            ProcessDownEvent(pointerEvent);
            break;
        case PointerEvent::POINTER_ACTION_MOVE:
        case PointerEvent::POINTER_ACTION_PULL_MOVE:
            if (!isStop) {
                ProcessMoveEvent(pointerEvent);
                return true;
            }
            return false;
            break;
        default:
            return false;
            break;
    }
    return true;
}

void KnuckleDynamicDrawingManager::StartTouchDraw(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    int32_t ret = DrawGraphic(pointerEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("Draw graphic failed");
        return;
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    MMI_HILOGI("Draw graphic success");
}

void KnuckleDynamicDrawingManager::ProcessUpAndCancelEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    if (mPointerPath_.IsValid()) {
        auto id = pointerEvent->GetPointerId();
        PointerEvent::PointerItem pointerItem;
        pointerEvent->GetPointerItem(id, pointerItem);
        mGlowTraceSystem_->ResetDivergentPoints(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }

    mPointerPath_.Reset();
    mGlowTraceSystem_->Clear();
    canvasNode_->ResetSurface();
    Rosen::RSTransaction::FlushImplicitTransaction();
    mIsDrawing_ = false;
}

void KnuckleDynamicDrawingManager::ProcessDownEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    UpdateTrackColors();
    mLastUpdateTimeMillis_ = pointerEvent->GetActionTime();
    mPointCounter = 0;
    auto id = pointerEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(id, pointerItem);
    mTraceControlpoints_[mPointCounter].Set(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    mGlowTraceSystem_->ResetDivergentPoints(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    mIsDrawing_ = false;
    isStop = false;
}

void KnuckleDynamicDrawingManager::ProcessMoveEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    mPointCounter++;
    if (mPointCounter >= POINT_TOTAL_SIZE) {
        MMI_HILOGE("mTraceControlpoints_ index out of size");
        return;
    }
    auto id = pointerEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(id, pointerItem);
    mTraceControlpoints_[mPointCounter].Set(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());

    int pointIndex4 = 4;

    if (mPointCounter == pointIndex4) {
        int pointIndex0 = 0;
        int pointIndex1 = 1;
        int pointIndex2 = 2;
        int pointIndex3 = 3;

        mTraceControlpoints_[pointIndex3].Set(
            (mTraceControlpoints_[pointIndex2].GetX() + mTraceControlpoints_[pointIndex4].GetX()) / DOUBLE,
            (mTraceControlpoints_[pointIndex2].GetY() + mTraceControlpoints_[pointIndex4].GetY()) / DOUBLE);

        // Add a cubic Bezier from pt[0] to pt[3] with control pointspt[1] and pt[2]
        mPointerPath_.MoveTo (mTraceControlpoints_[pointIndex0].GetX(), mTraceControlpoints_[pointIndex0].GetY());
        mPointerPath_.CubicTo(mTraceControlpoints_[pointIndex1].GetX(),
            mTraceControlpoints_[pointIndex1].GetY(),
            mTraceControlpoints_[pointIndex2].GetX(),
            mTraceControlpoints_[pointIndex2].GetY(),
            mTraceControlpoints_[pointIndex3].GetX(),
            mTraceControlpoints_[pointIndex3].GetY());
        mTraceControlpoints_[pointIndex0].Set(mTraceControlpoints_[pointIndex3].GetX(),
            mTraceControlpoints_[pointIndex3].GetY());
        mTraceControlpoints_[pointIndex1].Set (mTraceControlpoints_[pointIndex4].GetX(),
            mTraceControlpoints_[pointIndex4].GetY());
        mPointCounter = 1;
        // Add glowing particles onto the last path segment that was drawn
        long now = pointerEvent->GetActionTime();
        double len = mPointerPath_.GetLength(false);
        mGlowTraceSystem_->AddGlowPoints(mPointerPath_, (now - mLastUpdateTimeMillis_) / 1000);
        mPointerPath_.Reset();
        mLastUpdateTimeMillis_ = now;
    }
    mGlowTraceSystem_->ResetDivergentPoints(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    mIsDrawing_ = false;
}

void KnuckleDynamicDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    displayInfo_ = displayInfo;
    bubble_.innerCircleRadius = displayInfo.dpi * INDEPENDENT_INNER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleRadius = displayInfo.dpi * INDEPENDENT_OUTER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleWidth = displayInfo.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE;
}

int32_t KnuckleDynamicDrawingManager::DrawGraphic(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, RET_ERR);
#ifndef USE_ROSEN_DRAWING
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#else
    auto canvas = static_cast<Rosen::Drawing::RecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#endif

    CHKPR(canvas, RET_ERR);
    mGlowTraceSystem_->Update();
    if (mPointerPath_.IsValid()) {
        return RET_ERR;
    }
    mGlowTraceSystem_->Draw(canvas);
    canvasNode_->FinishRecording();
    if (!mIsDrawing_) {
        canvasNode_->ResetSurface();
        mIsDrawing_ = true;
    }
    return RET_OK;
}

void KnuckleDynamicDrawingManager::CreateTouchWindow(const int32_t displayId)
{
    CALL_DEBUG_ENTER;
    if (surfaceNode_ != nullptr) {
        MMI_HILOGD("surfaceNode is already.");
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

    screenId_ = static_cast<uint64_t>(displayId);
    std::cout << "ScreenId: " << screenId_ << std::endl;
    surfaceNode_->SetRotation(0);

    CreateCanvasNode();
    surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    surfaceNode_->AttachToDisplay(screenId_);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void KnuckleDynamicDrawingManager::CreateCanvasNode()
{
    CALL_DEBUG_ENTER;
    canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    CHKPV(canvasNode_);
    canvasNode_->SetBounds(0, 0, displayInfo_.width, displayInfo_.height);
    canvasNode_->SetFrame(0, 0, displayInfo_.width, displayInfo_.height);
#ifndef USE_ROSEN_DRAWING
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    canvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);
}
} // namespace MMI
} // namespace OHOS
