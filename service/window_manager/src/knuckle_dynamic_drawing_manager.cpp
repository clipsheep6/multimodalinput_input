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
// #include "image_converter.h"
#endif

#include "render/rs_pixel_map_util.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS {
namespace MMI {
namespace {
const std::string IMAGE_POINTER_PENTAGRAM_PATH = "/system/etc/multimodalinput/mouse_icon/";
const std::string PentagramIconPath = IMAGE_POINTER_PENTAGRAM_PATH + "Default.svg";
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KnuckleDynamicDrawingManager" };
constexpr int32_t DENSITY_BASELINE = 160;
constexpr int32_t INDEPENDENT_INNER_PIXELS = 20;
constexpr int32_t INDEPENDENT_OUTER_PIXELS = 21;
constexpr int32_t INDEPENDENT_WIDTH_PIXELS = 2;
constexpr int32_t CALCULATE_MIDDLE = 2;
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t MIN_POINTER_COLOR = 0x000000;
} // namespace

float KnuckleDynamicDrawingManager::PAINT_STROKE_WIDTH = 10.0f;
float KnuckleDynamicDrawingManager::PAINT_PATH_RADIUS = 10.0f;
float KnuckleDynamicDrawingManager::DOUBLE = 2.0f;
int KnuckleDynamicDrawingManager::POINT_TOTAL_SIZE = 5;
int KnuckleDynamicDrawingManager::POINT_SYSTEM_SIZE = 200;
int KnuckleDynamicDrawingManager::MAX_DIVERGENCE_NUM = 10;
int KnuckleDynamicDrawingManager::MAX_UPDATE_TIME_MILLIS = 500;
constexpr int32_t DEFAULT_POINTER_SIZE = 1;

KnuckleDynamicDrawingManager::KnuckleDynamicDrawingManager()
{
    InitPointerPathPaint();
    
    brush_.SetColor(Rosen::Drawing::Color::COLOR_WHITE);
    brush_.SetAntiAlias(true);
    float innerCircleTransparency = 0.6f;
    brush_.SetAlphaF(innerCircleTransparency);
}

KnuckleDynamicDrawingManager::~KnuckleDynamicDrawingManager() {}

int32_t KnuckleDynamicDrawingManager::GetPointerSize()
{
    CALL_DEBUG_ENTER;
    std::string name = "pointerSize";
    int32_t pointerSize = PreferencesMgr->GetIntValue(name, DEFAULT_POINTER_SIZE);
    MMI_HILOGD("Get pointer size successfully, pointerSize:%{public}d", pointerSize);
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
    MMI_HILOGD("Get supported format ret:%{public}u", ret);
    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = 5,
        .height = 5
    };
 
    decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};
    decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};

    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        MMI_HILOGE("The pixelMap is nullptr");
    }
    return pixelMap;
}

Rosen::Drawing::ColorType KnuckleDynamicDrawingManager::PixelFormatToColorType(Media::PixelFormat pixelFormat)
{
    CALL_DEBUG_ENTER;
    switch (pixelFormat) {
        case Media::PixelFormat::BGRA_8888:
            return Rosen::Drawing::ColorType::COLORTYPE_BGRA_8888;
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::ALPHA_8:
        case Media::PixelFormat::RGBA_8888:
        case Media::PixelFormat::RGB_565:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::RGBA_F16:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        case Media::PixelFormat::UNKNOWN:
        default:
            return Rosen::Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

Rosen::Drawing::AlphaType KnuckleDynamicDrawingManager::AlphaTypeToAlphaType(Media::AlphaType alphaType)
{
    CALL_DEBUG_ENTER;
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Rosen::Drawing::AlphaType::ALPHATYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Rosen::Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
        default:
            return Rosen::Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}

std::shared_ptr<Rosen::Drawing::Bitmap> KnuckleDynamicDrawingManager::PixelMapToBitmap(
    std::shared_ptr<Media::PixelMap>& pixelMap)
{
    CALL_DEBUG_ENTER;
    auto data = pixelMap->GetPixels();
    Rosen::Drawing::Bitmap bitmap;
    Rosen::Drawing::BitmapFormat format { Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888, Rosen::Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(pixelMap->GetWidth(), pixelMap->GetHeight(), format);
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
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = DecodeImageToPixelMap(PentagramIconPath);
    mpixelMap_ = pixelMap;
    auto bitmap = PixelMapToBitmap(pixelMap);

    if (mGlowTraceSystem_ == nullptr) {
        MMI_HILOGD("mGlowTraceSystem_ nullptr");
        mGlowTraceSystem_ = std::make_shared<KnuckleGlowTraceSystem>(POINT_SYSTEM_SIZE, *bitmap, MAX_DIVERGENCE_NUM);
    }
    mPointerPathPaint_.setStyle(SkPaint::Style::kStroke_Style);
    mPointerPathPaint_.setStrokeJoin(SkPaint::Join::kRound_Join);
    mPointerPathPaint_.setStrokeCap(SkPaint::Cap::kRound_Cap);
    mPointerPathPaint_.setStrokeWidth (PAINT_STROKE_WIDTH);

    mPointerPathPaint_.setStrokeWidth(PAINT_STROKE_WIDTH);
    mPointerPathPaint_.setAntiAlias(true);
}

void KnuckleDynamicDrawingManager::UpdateTrackColors()
{
    CALL_DEBUG_ENTER;
    mPointerPathPaint_.setColor(0xFFCCCCCC);
    mGlowTraceSystem_->SetTrackColor(0xFFCCCCCC);
}

void KnuckleDynamicDrawingManager::KnuckleDynamicDrawHandler(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    auto displayId = pointerEvent->GetTargetDisplayId();
    CreateTouchWindow(displayId);
    if (CheckPointerAction(pointerEvent)) {
        StartTouchDraw(pointerEvent);
    }
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
        break;
        case PointerEvent::POINTER_ACTION_DOWN:
        case PointerEvent::POINTER_ACTION_PULL_DOWN:
        ProcessDownEvent(pointerEvent);
        break;
        case PointerEvent::POINTER_ACTION_MOVE:
        case PointerEvent::POINTER_ACTION_PULL_MOVE:
        ProcessMoveEvent(pointerEvent);
        break;
        default:
        break;
    }

    return true;
}

void KnuckleDynamicDrawingManager::ProcessUpAndCancelEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    if(mPointerPath_.IsValid()) {
        auto id = pointerEvent->GetPointerId();
        PointerEvent::PointerItem pointerItem;
        pointerEvent->GetPointerItem(id, pointerItem);
        mGlowTraceSystem_->ResetDivergentPoints(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }

    mPointerPath_.Reset();
    mGlowTraceSystem_->Clear();
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
    MMI_HILOGE(" GetDisplayX : %{public}d GetDisplayY: %{public}d ", pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
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
        mTraceControlpoints_[pointIndex0].Set(mTraceControlpoints_[pointIndex3].GetX(), mTraceControlpoints_[pointIndex3].GetY());
        mTraceControlpoints_[pointIndex1].Set (mTraceControlpoints_[pointIndex4].GetX(), mTraceControlpoints_[pointIndex4].GetY());
        mPointCounter = 1;
        // Add glowing particles onto the last path segment that was drawn
        long now = pointerEvent->GetActionTime();
        double len = mPointerPath_.GetLength(false);
        MMI_HILOGE(" 11111111111 : %{public}f now: %{public}ld  mLastUpdateTimeMillis_:%{public}lld ", len, now, mLastUpdateTimeMillis_);
        mGlowTraceSystem_->AddGlowPoints(mPointerPath_, (now - mLastUpdateTimeMillis_)/1000);
        mPointerPath_.Reset();
        mLastUpdateTimeMillis_ = now;
    }

    mGlowTraceSystem_->ResetDivergentPoints(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
}

void KnuckleDynamicDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    displayInfo_ = displayInfo;
    bubble_.innerCircleRadius = displayInfo.dpi * INDEPENDENT_INNER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleRadius = displayInfo.dpi * INDEPENDENT_OUTER_PIXELS / DENSITY_BASELINE / CALCULATE_MIDDLE;
    bubble_.outerCircleWidth = displayInfo.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE;
}

void KnuckleDynamicDrawingManager::StartTouchDraw(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    #ifndef USE_ROSEN_DRAWING
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#else
    auto canvas = static_cast<Rosen::Drawing::RecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#endif

    CHKPV(canvas);
    mGlowTraceSystem_->Update();

    if(mPointerPath_.IsValid()){
        return;
    }
    MMI_HILOGE("GetActionTime:%{public}lld", pointerEvent->GetActionTime() - mLastUpdateTimeMillis_);
    if (pointerEvent->GetActionTime() - mLastUpdateTimeMillis_ > MAX_UPDATE_TIME_MILLIS) {
        mGlowTraceSystem_->Draw(canvas);
    }

    if (!mIsDrawing_) {
        MMI_HILOGE("Fingersense drawing line");
        mIsDrawing_ = true;
    }
    canvasNode_->FinishRecording();
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
    canvasNode_ = Rosen::RSCanvasNode::Create();
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
