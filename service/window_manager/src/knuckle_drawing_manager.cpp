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

#include "knuckle_drawing_manager.h"

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "ui/rs_canvas_drawing_node.h"
#endif

#include "define_multimodal.h"
#include "mmi_log.h"


namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KnuckleDrawingManager" };
constexpr int32_t DENSITY_BASELINE = 80;
constexpr int32_t INDEPENDENT_WIDTH_PIXELS = 2;
constexpr int32_t POINT_INDEX0 = 0;
constexpr int32_t POINT_INDEX1 = 1;
constexpr int32_t POINT_INDEX2 = 2;
constexpr int32_t POINT_INDEX3 = 3;
constexpr int32_t POINT_INDEX4 = 4;
constexpr int32_t MAX_POINTER_NUM = 5;
constexpr int32_t MID_POINT = 2;
constexpr int32_t DEFAULT_VALUE = -1;
} // namespace

KnuckleDrawingManager::KnuckleDrawingManager()
{
    pen_.SetColor(Rosen::Drawing::Color::COLOR_CYAN);
    pen_.SetAntiAlias(true);
    float outerCircleTransparency = 0.1f;
    pen_.SetAlphaF(outerCircleTransparency);
    pen_.SetJoinStyle(Rosen::Drawing::Pen::JoinStyle::ROUND_JOIN);
}

KnuckleDrawingManager::~KnuckleDrawingManager() {}

void KnuckleDrawingManager::KnuckleDrawHandler(std::shared_ptr<PointerEvent> touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);

    if (!IsSingleKnuckle(touchEvent)) {
        return;
    }
    int32_t displayId = touchEvent->GetTargetDisplayId();
    CreateTouchWindow(displayId);
    int32_t touchAction = touchEvent->GetPointerAction();
    if (IsValidAction(touchAction)) {
        StartTouchDraw(touchEvent);
    }
}

bool KnuckleDrawingManager::IsSingleKnuckle(std::shared_ptr<PointerEvent> touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(touchEvent);
    auto id = touchEvent->GetPointerId();
    PointerEvent::PointerItem item;
    touchEvent->GetPointerItem(id, item);
    if (item.GetToolType() != PointerEvent::TOOL_TYPE_KNUCKLE ||
        touchEvent->GetPointerIds().size() != 1) {
        if (canvasNode_ != nullptr) {
            path.Reset();
            pointerInfos_.clear();
            canvasNode_->ResetSurface();
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
        MMI_HILOGE("touch tool type is not single knuckle");
        return false;
    }
    return true;
}

bool KnuckleDrawingManager::IsValidAction(const int32_t action)
{
    CALL_DEBUG_ENTER;
    if (action == PointerEvent::POINTER_ACTION_DOWN || action == PointerEvent::POINTER_ACTION_UP ||
        (action == PointerEvent::POINTER_ACTION_MOVE && (!pointerInfos_.empty()))) {
        return true;
    }
    MMI_HILOGE("action is not down or move or up");
    return false;
}

void KnuckleDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    if (displayInfo.dpi == displayInfo_.dpi) {
        MMI_HILOGD("dpi is not need to change");
        return;
    }
    displayInfo_ = displayInfo;
    width_ = displayInfo.dpi * INDEPENDENT_WIDTH_PIXELS / DENSITY_BASELINE;
}

void KnuckleDrawingManager::StartTouchDraw(std::shared_ptr<PointerEvent> touchEvent)
{
    CHKPV(touchEvent);
    int32_t ret = DrawGraphic(touchEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("Draw graphic failed");
        return;
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    MMI_HILOGD("Draw graphic success");
}

void KnuckleDrawingManager::CreateTouchWindow(const int32_t displayId)
{
    CALL_DEBUG_ENTER;
    if (surfaceNode_ != nullptr) {
        MMI_HILOGD("surfaceNode is already exit");
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
#endif // USE_ROSEN_DRAWING

    screenId_ = static_cast<uint64_t>(displayId);
    MMI_HILOGI("ScreenId: %{public}llu", screenId_);
    surfaceNode_->SetRotation(0);

    CreateCanvasNode();
    surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    surfaceNode_->AttachToDisplay(screenId_);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void KnuckleDrawingManager::CreateCanvasNode()
{
    canvasNode_ = Rosen::RSCanvasDrawingNode::Create();
    CHKPV(canvasNode_);
    canvasNode_->SetBounds(0, 0, displayInfo_.width, displayInfo_.height);
    canvasNode_->SetFrame(0, 0, displayInfo_.width, displayInfo_.height);
#ifndef USE_ROSEN_DRAWING
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    canvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif // USE_ROSEN_DRAWING
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);
}

int32_t KnuckleDrawingManager::GetPointerPos(std::shared_ptr<PointerEvent> touchEvent)
{
    CHKPR(touchEvent, RET_ERR);
    if (touchEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        isActionUp_ = true;
        return RET_OK;
    }
    PointerInfo pointerInfo;
    auto pointerId = touchEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    if (!touchEvent->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
        return RET_ERR;
    }
    pointerInfo.x = pointerItem.GetDisplayX();
    pointerInfo.y = pointerItem.GetDisplayY();
    pointerInfos_.push_back(pointerInfo);

    if (pointerInfos_.size() == MAX_POINTER_NUM) {
        pointerInfos_[POINT_INDEX3].x = (pointerInfos_[POINT_INDEX2].x + pointerInfos_[POINT_INDEX4].x) / MID_POINT;
        pointerInfos_[POINT_INDEX3].y = (pointerInfos_[POINT_INDEX2].y + pointerInfos_[POINT_INDEX4].y) / MID_POINT;
    } else {
        MMI_HILOGI("Can't get enough pointers to draw");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t KnuckleDrawingManager::DrawGraphic(std::shared_ptr<PointerEvent> touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    CHKPR(canvasNode_, RET_ERR);
    if (GetPointerPos(touchEvent) != RET_OK) {
        MMI_HILOGE("GetPointerPos failed");
        return RET_ERR;
    }
#ifndef USE_ROSEN_DRAWING
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#else
    auto canvas = static_cast<Rosen::Drawing::RecordingCanvas *>(canvasNode_->
        BeginRecording(displayInfo_.width, displayInfo_.height));
#endif // USE_ROSEN_DRAWING
    CHKPR(canvas, RET_ERR);
    if (!isActionUp_) {
        if (pointerInfos_.size() != MAX_POINTER_NUM) {
            MMI_HILOGE("Size of pointerInfos_:%{public}u", pointerInfos_.size());
            return RET_ERR;
        }
        pen_.SetWidth(width_);
        path.MoveTo(pointerInfos_[POINT_INDEX0].x, pointerInfos_[POINT_INDEX0].y);
        path.CubicTo(pointerInfos_[POINT_INDEX1].x, pointerInfos_[POINT_INDEX1].y,
            pointerInfos_[POINT_INDEX2].x, pointerInfos_[POINT_INDEX2].y,
            pointerInfos_[POINT_INDEX3].x, pointerInfos_[POINT_INDEX3].y);
        canvas->AttachPen(pen_);
        canvas->DrawPath(path);
        canvas->DetachPen();
        pointerInfos_.erase(pointerInfos_.begin(), pointerInfos_.begin() + POINT_INDEX3);
    } else {
        MMI_HILOGD("isActionUp_ == true");
        isActionUp_ = false;
        pointerInfos_.clear();
        canvasNode_->ResetSurface();
    }
    path.Reset();
    canvasNode_->FinishRecording();
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS