/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pointer_drawing_manager.h"

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"

#include "define_multimodal.h"
#include "input_device_manager.h"
#include "input_windows_manager.h"
#include "ipc_skeleton.h"
#include "mmi_log.h"
#include "preferences.h"
#include "preferences_impl.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "preferences_xml_utils.h"
#include "util.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "recording/recording_canvas.h"
#endif

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "PointerDrawingManager" };
const std::string IMAGE_POINTER_DEFAULT_PATH = "/system/etc/multimodalinput/mouse_icon/";
const std::string DefaultIconPath = IMAGE_POINTER_DEFAULT_PATH + "Default.svg";
constexpr int32_t BASELINE_DENSITY = 160;
constexpr int32_t CALCULATE_MIDDLE = 2;
constexpr int32_t DEVICE_INDEPENDENT_PIXELS = 40;
constexpr int32_t POINTER_WINDOW_INIT_SIZE = 64;
constexpr int32_t DEFAULT_POINTER_SIZE = 1;
constexpr int32_t MIN_POINTER_SIZE = 1;
constexpr int32_t MAX_POINTER_SIZE = 7;
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t ANIMATION_DURATION = 500;
constexpr int32_t DEFAULT_POINTER_STYLE = 0;
constexpr int32_t CURSOR_CIRCLE_STYLE = 41;
constexpr float ROTATION_ANGLE = 360.f;
constexpr float LOADING_CENTER_RATIO = 0.5f;
constexpr float RUNNING_X_RATIO = 0.3f;
constexpr float RUNNING_Y_RATIO = 0.675f;
constexpr float INCREASE_RATIO = 1.22;
constexpr int32_t MIN_POINTER_COLOR = 0x000000;
constexpr int32_t MAX_POINTER_COLOR = 0xffffff;
const std::string MOUSE_FILE_NAME = "/data/service/el1/public/multimodalinput/mouse_settings.xml";
} // namespace
} // namespace MMI
} // namespace OHOS

namespace OHOS {
namespace MMI {
PointerDrawingManager::PointerDrawingManager()
{
    InitStyle();
}

PointerStyle PointerDrawingManager::GetLastMouseStyle()
{
    CALL_DEBUG_ENTER;
    return lastMouseStyle_;
}
void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
    const PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("Display:%{public}d,physicalX:%{public}d,physicalY:%{public}d,pointerStyle:%{public}d",
        displayId, physicalX, physicalY, pointerStyle.id);
    FixCursorPosition(physicalX, physicalY);
    lastPhysicalX_ = physicalX;
    lastPhysicalY_ = physicalY;
    AdjustMouseFocus(ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay), physicalX, physicalY);

    if (surfaceNode_ != nullptr) {
        surfaceNode_->SetBounds(physicalX + displayInfo_.x,
            physicalY + displayInfo_.y,
            surfaceNode_->GetStagingProperties().GetBounds().z_,
            surfaceNode_->GetStagingProperties().GetBounds().w_);
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window move success");
        if (lastMouseStyle_ == pointerStyle && !mouseIconUpdate_) {
            MMI_HILOGD("The lastpointerStyle is equal with pointerStyle,id %{public}d size:%{public}d",
                pointerStyle.id, pointerStyle.size);
            return;
        }
        lastMouseStyle_ = pointerStyle;
        int32_t ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
        if (ret != RET_OK) {
            mouseIconUpdate_ = false;
            MMI_HILOGE("Init layer failed");
            return;
        }
        UpdatePointerVisible();
        mouseIconUpdate_ = false;
        MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
            displayId, physicalX, physicalY);
        return;
    }
    CreatePointerWindow(displayId, physicalX, physicalY);
    CHKPV(surfaceNode_);
    UpdateMouseStyle();
    int32_t ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return;
    }
    UpdatePointerVisible();
    MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
        displayId, physicalX, physicalY);
}

void PointerDrawingManager::UpdateMouseStyle()
{
    CALL_DEBUG_ENTER;
    PointerStyle curPointerStyle;
    int result = GetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
    if (result != RET_OK) {
        MMI_HILOGE("Get current pointer style failed");
        return;
    }
    if (curPointerStyle.id == CURSOR_CIRCLE_STYLE) {
        lastMouseStyle_.id = curPointerStyle.id;
        int ret = SetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Set pointer style failed");
        }
        return;
    }
}

int32_t PointerDrawingManager::InitLayer(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    if (surfaceNode_ == nullptr) {
        MMI_HILOGD("surfaceNode_ is nullptr");
        return RET_ERR;
    }
    DrawLoadingPointerStyle(mouseStyle);
    DrawRunningPointerAnimate(mouseStyle);
    sptr<OHOS::Surface> layer = GetLayer();
    if (layer == nullptr) {
        MMI_HILOGE("Init layer is failed, Layer is nullptr");
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
        return RET_ERR;
    }

    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        MMI_HILOGE("Init layer is failed, buffer or virAddr is nullptr");
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
        return RET_ERR;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), mouseStyle);
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        MMI_HILOGE("Init layer failed, FlushBuffer return ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return RET_ERR;
    }
    MMI_HILOGD("Init layer success");
    return RET_OK;
}

void PointerDrawingManager::DrawLoadingPointerStyle(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPV(surfaceNode_);
    Rosen::RSAnimationTimingProtocol protocol;
    if (mouseStyle != MOUSE_ICON::LOADING &&
        (mouseStyle != MOUSE_ICON::DEFAULT ||
            mouseIcons_[mouseStyle].iconPath != (IMAGE_POINTER_DEFAULT_PATH + "Loading.svg"))) {
        protocol.SetDuration(0);
        Rosen::RSNode::Animate(
            protocol,
            Rosen::RSAnimationTimingCurve::LINEAR,
            [&]() { surfaceNode_->SetRotation(0); },
            []() { MMI_HILOGE("animate callback"); });
        MMI_HILOGD("current pointer is not loading");
        Rosen::RSTransaction::FlushImplicitTransaction();
        return;
    }
    float ratio = imageWidth_ * 1.0 / IMAGE_WIDTH;
    surfaceNode_->SetPivot({LOADING_CENTER_RATIO * ratio, LOADING_CENTER_RATIO * ratio});
    protocol.SetDuration(ANIMATION_DURATION);
    protocol.SetRepeatCount(DEFAULT_VALUE);

    // create property animation
    Rosen::RSNode::Animate(
        protocol,
        Rosen::RSAnimationTimingCurve::LINEAR,
        [&]() { surfaceNode_->SetRotation(ROTATION_ANGLE); },
        []() { MMI_HILOGE("animate callback"); });

    Rosen::RSTransaction::FlushImplicitTransaction();
}

void PointerDrawingManager::DrawRunningPointerAnimate(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPV(surfaceNode_);
    if (mouseStyle != MOUSE_ICON::RUNNING &&
        (mouseStyle != MOUSE_ICON::DEFAULT ||
            mouseIcons_[mouseStyle].iconPath != (IMAGE_POINTER_DEFAULT_PATH + "Loading_Left.svg"))) {
        if (canvasNode_ != nullptr) {
            canvasNode_->SetVisible(false);
        }
        MMI_HILOGD("current pointer is not running");
        return;
    }
    canvasNode_->SetVisible(true);
    float ratio = imageWidth_ * 1.0 / IMAGE_WIDTH;
    canvasNode_->SetPivot({RUNNING_X_RATIO * ratio, RUNNING_Y_RATIO * ratio});
    std::shared_ptr<OHOS::Media::PixelMap> pixelmap =
        DecodeImageToPixelMap(mouseIcons_[MOUSE_ICON::RUNNING_RIGHT].iconPath);
    CHKPV(pixelmap);
    MMI_HILOGD("set mouseicon to OHOS system");
    
#ifndef USE_ROSEN_DRAWING
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));
    canvas->DrawPixelMap(pixelmap, 0, 0, SkSamplingOptions(), nullptr);
#else
    auto canvas =
        static_cast<Rosen::Drawing::RecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));
    canvas->DrawPixelMap(pixelmap, {}, {});
#endif
    
    canvasNode_->FinishRecording();

    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    protocol.SetRepeatCount(DEFAULT_VALUE);

    // create property animation
    Rosen::RSNode::Animate(
        protocol,
        Rosen::RSAnimationTimingCurve::LINEAR,
        [&]() { canvasNode_->SetRotation(ROTATION_ANGLE); },
        []() { MMI_HILOGE("animate callback"); });

    Rosen::RSTransaction::FlushImplicitTransaction();
}

void PointerDrawingManager::AdjustMouseFocus(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    switch (iconType) {
        case ANGLE_SW: {
            physicalY -= imageHeight_;
            break;
        }
        case ANGLE_CENTER: {
            physicalX -= imageWidth_ / CALCULATE_MIDDLE;
            physicalY -= imageHeight_ / CALCULATE_MIDDLE;
            break;
        }
        case ANGLE_NW:
            if (userIcon_ != nullptr && lastMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
                physicalX -= userIconHotSpotX_;
                physicalY -= userIconHotSpotY_;
            }
            break;
        default: {
            MMI_HILOGD("No need adjust mouse focus");
            break;
        }
    }
}

void PointerDrawingManager::SetMouseDisplayState(bool state)
{
    CALL_DEBUG_ENTER;
    if (mouseDisplayState_ != state) {
        mouseDisplayState_ = state;
        if (mouseDisplayState_) {
            InitLayer(MOUSE_ICON(lastMouseStyle_.id));
        }
        UpdatePointerVisible();
    }
}

bool PointerDrawingManager::GetMouseDisplayState() const
{
    return mouseDisplayState_;
}

void PointerDrawingManager::FixCursorPosition(int32_t &physicalX, int32_t &physicalY)
{
    if (physicalX < 0) {
        physicalX = 0;
    }

    if (physicalY < 0) {
        physicalY = 0;
    }
    const int32_t cursorUnit = 16;
    if (displayInfo_.direction == DIRECTION0 || displayInfo_.direction == DIRECTION180) {
        if (physicalX > (displayInfo_.width - imageWidth_ / cursorUnit)) {
            physicalX = displayInfo_.width - imageWidth_ / cursorUnit;
        }
        if (physicalY > (displayInfo_.height - imageHeight_ / cursorUnit)) {
            physicalY = displayInfo_.height - imageHeight_ / cursorUnit;
        }
    } else {
        if (physicalX > (displayInfo_.height - imageHeight_ / cursorUnit)) {
            physicalX = displayInfo_.height - imageHeight_ / cursorUnit;
        }
        if (physicalY > (displayInfo_.width - imageWidth_ / cursorUnit)) {
            physicalY = displayInfo_.width - imageWidth_ / cursorUnit;
        }
    }
}

void PointerDrawingManager::CreatePointerWindow(int32_t displayId, int32_t physicalX, int32_t physicalY)
{
    CALL_DEBUG_ENTER;
    CALL_INFO_TRACE;
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "pointer window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    CHKPV(surfaceNode_);
    surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode_->SetBounds(physicalX, physicalY, IMAGE_WIDTH, IMAGE_HEIGHT);
#ifndef USE_ROSEN_DRAWING
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    surfaceNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif
    
    screenId_ = static_cast<uint64_t>(displayId);
    std::cout << "ScreenId: " << screenId_ << std::endl;
    surfaceNode_->AttachToDisplay(screenId_);
    surfaceNode_->SetRotation(0);

    canvasNode_ = Rosen::RSCanvasNode::Create();
    canvasNode_->SetBounds(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    canvasNode_->SetFrame(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
#ifndef USE_ROSEN_DRAWING
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    surfaceNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);
    surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

sptr<OHOS::Surface> PointerDrawingManager::GetLayer()
{
    CALL_DEBUG_ENTER;
    if (surfaceNode_ == nullptr) {
        MMI_HILOGE("Draw pointer is failed, get node is nullptr");
        return nullptr;
    }
    return surfaceNode_->GetSurface();
}

sptr<OHOS::SurfaceBuffer> PointerDrawingManager::GetSurfaceBuffer(sptr<OHOS::Surface> layer) const
{
    CALL_DEBUG_ENTER;
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    OHOS::BufferRequestConfig config = {
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };

    OHOS::SurfaceError ret = layer->RequestBuffer(buffer, releaseFence, config);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        MMI_HILOGE("Request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    return buffer;
}

void PointerDrawingManager::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    DrawPixelmap(canvas, mouseStyle);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        MMI_HILOGE("Memcpy data is error, ret:%{public}d", ret);
        return;
    }
}

void PointerDrawingManager::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas, const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    if (mouseStyle == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGD("set mouseicon by userIcon_");
        canvas.DrawBitmap(*userIcon_, 0, 0);
    } else {
        std::shared_ptr<OHOS::Media::PixelMap> pixelmap;
        if (mouseStyle == MOUSE_ICON::RUNNING) {
            pixelmap = DecodeImageToPixelMap(mouseIcons_[MOUSE_ICON::RUNNING_LEFT].iconPath);
        } else {
            pixelmap = DecodeImageToPixelMap(mouseIcons_[mouseStyle].iconPath);
        }
        CHKPV(pixelmap);
        MMI_HILOGD("set mouseicon to OHOS system");
        canvas.DrawBitmap(*pixelmap, 0, 0);
    }
}

int32_t PointerDrawingManager::SetMouseIcon(int32_t pid, int32_t windowId, void* pixelMap)
{
    CALL_DEBUG_ENTER;
    if (pid == -1) {
        MMI_HILOGE("pid is invalid return -1");
        return RET_ERR;
    }
    if (pixelMap == nullptr) {
        MMI_HILOGE("pixelMap is null!");
        return RET_ERR;
    }
    if (windowId < 0) {
        MMI_HILOGE("get invalid windowId, %{public}d", windowId);
        return RET_ERR;
    }
    OHOS::Media::PixelMap* pixelMapPtr = static_cast<OHOS::Media::PixelMap*>(pixelMap);
    userIcon_.reset(pixelMapPtr);
    mouseIconUpdate_ = true;
    userIconHotSpotX_ = 0;
    userIconHotSpotY_ = 0;
    PointerStyle style;
    style.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    int32_t ret = SetPointerStyle(pid, windowId, style);
    if (ret == RET_ERR) {
        MMI_HILOGE("SetPointerStyle return RET_ERR here!");
    }
    return ret;
}

int32_t PointerDrawingManager::SetMouseHotSpot(int32_t pid, int32_t windowId, int32_t hotSpotX, int32_t hotSpotY)
{
    CALL_DEBUG_ENTER;
    if (pid == -1) {
        MMI_HILOGE("pid is invalid return -1");
        return RET_ERR;
    }
    if (windowId < 0) {
        MMI_HILOGE("invalid windowId, %{public}d", windowId);
        return RET_ERR;
    }
    if (hotSpotX < 0 || hotSpotY < 0 || userIcon_ == nullptr) {
        MMI_HILOGE("invalid value");
        return RET_ERR;
    }
    PointerStyle pointerStyle;
    int32_t ret = WinMgr->GetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK || pointerStyle.id != MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGE("Get pointer style failed, pid %{publid}d, pointerStyle %{public}d", pid, pointerStyle.id);
        return RET_ERR;
    }
    userIconHotSpotX_ = hotSpotX;
    userIconHotSpotY_ = hotSpotY;
    return RET_OK;
}

std::shared_ptr<OHOS::Media::PixelMap> PointerDrawingManager::DecodeImageToPixelMap(const std::string &imagePath)
{
    CALL_DEBUG_ENTER;
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    CHKPP(imageSource);
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    MMI_HILOGD("Get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = imageWidth_,
        .height = imageHeight_
    };
    int32_t pointerColor = GetPointerColor();
    if (tempPointerColor_ != DEFAULT_VALUE) {
        decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = pointerColor};
    }

    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        MMI_HILOGE("The pixelMap is nullptr");
    }
    return pixelMap;
}

int32_t PointerDrawingManager::SetPointerColor(int32_t color)
{
    CALL_DEBUG_ENTER;
    if (color < MIN_POINTER_COLOR) {
        color = MIN_POINTER_COLOR;
    } else if (color > MAX_POINTER_COLOR) {
        color = MAX_POINTER_COLOR;
    }
    int32_t errCode = RET_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
    if (pref == nullptr) {
        MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
        return RET_ERR;
    }
    std::string name = "pointerColor";
    int32_t ret = pref->PutInt(name, color);
    if (ret != RET_OK) {
        MMI_HILOGE("Put color is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    ret = pref->FlushSync();
    if (ret != RET_OK) {
        MMI_HILOGE("Flush sync is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGD("Set pointer color successfully, color:%{public}d", color);
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);

    ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return RET_ERR;
    }
    UpdatePointerVisible();
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerColor()
{
    CALL_DEBUG_ENTER;
    int32_t errCode = RET_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
    if (pref == nullptr) {
        MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
        return RET_ERR;
    }
    std::string name = "pointerColor";
    int32_t pointerColor = pref->GetInt(name, DEFAULT_VALUE);
    tempPointerColor_ = pointerColor;
    if (pointerColor == DEFAULT_VALUE) {
        pointerColor = MIN_POINTER_COLOR;
    }
    MMI_HILOGD("Get pointer color successfully, pointerColor:%{public}d", pointerColor);
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);
    return pointerColor;
}

void PointerDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    hasDisplay_ = true;
    displayInfo_ = displayInfo;
    int32_t size = GetPointerSize();
    imageWidth_ = pow(INCREASE_RATIO, size - 1) * displayInfo.dpi * DEVICE_INDEPENDENT_PIXELS / BASELINE_DENSITY;
    imageHeight_ = pow(INCREASE_RATIO, size - 1) * displayInfo.dpi * DEVICE_INDEPENDENT_PIXELS / BASELINE_DENSITY;
    IMAGE_WIDTH = (imageWidth_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    IMAGE_HEIGHT = (imageHeight_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
}

int32_t PointerDrawingManager::SetPointerSize(int32_t size)
{
    CALL_DEBUG_ENTER;
    if (size < MIN_POINTER_SIZE) {
        size = MIN_POINTER_SIZE;
    } else if (size > MAX_POINTER_SIZE) {
        size = MAX_POINTER_SIZE;
    }
    int32_t errCode = RET_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
    if (pref == nullptr) {
        MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
        return RET_ERR;
    }
    std::string name = "pointerSize";
    int32_t ret = pref->PutInt(name, size);
    if (ret != RET_OK) {
        MMI_HILOGE("Put size is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    ret = pref->FlushSync();
    if (ret != RET_OK) {
        MMI_HILOGE("Flush sync is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGD("Set pointer size successfully, size:%{public}d", size);
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);

    if (surfaceNode_ == nullptr) {
        MMI_HILOGI("surfaceNode_ is nullptr");
        return RET_OK;
    }
    imageWidth_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * DEVICE_INDEPENDENT_PIXELS / BASELINE_DENSITY;
    imageHeight_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * DEVICE_INDEPENDENT_PIXELS / BASELINE_DENSITY;
    IMAGE_WIDTH = (imageWidth_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    IMAGE_HEIGHT = (imageHeight_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    int32_t physicalX = lastPhysicalX_;
    int32_t physicalY = lastPhysicalY_;
    AdjustMouseFocus(ICON_TYPE(mouseIcons_[MOUSE_ICON(lastMouseStyle_.id)].alignmentWay), physicalX, physicalY);
    CreatePointerWindow(displayInfo_.id, physicalX, physicalY);
    ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return RET_ERR;
    }
    UpdatePointerVisible();
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerSize()
{
    CALL_DEBUG_ENTER;
    int32_t errCode = RET_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
    if (pref == nullptr) {
        MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
        return RET_ERR;
    }
    std::string name = "pointerSize";
    int32_t pointerSize = pref->GetInt(name, DEFAULT_POINTER_SIZE);
    MMI_HILOGD("Get pointer size successfully, pointerSize:%{public}d", pointerSize);
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);
    return pointerSize;
}

void PointerDrawingManager::OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    for (const auto& item : displayGroupInfo.displaysInfo) {
        if (item.id == displayInfo_.id) {
            UpdateDisplayInfo(item);
            DrawManager();
            return;
        }
    }
    UpdateDisplayInfo(displayGroupInfo.displaysInfo[0]);
    lastPhysicalX_ = displayGroupInfo.displaysInfo[0].width / CALCULATE_MIDDLE;
    lastPhysicalY_ = displayGroupInfo.displaysInfo[0].height / CALCULATE_MIDDLE;
    MouseEventHdr->OnDisplayLost(displayInfo_.id);
    if (surfaceNode_ != nullptr) {
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
    }
    MMI_HILOGD("displayId_:%{public}d, displayWidth_:%{public}d, displayHeight_:%{public}d",
        displayInfo_.id, displayInfo_.width, displayInfo_.height);
}

void PointerDrawingManager::OnWindowInfo(const WinInfo &info)
{
    CALL_DEBUG_ENTER;
    windowId_ = info.windowId;
    pid_ = info.windowPid;
}

void PointerDrawingManager::UpdatePointerDevice(bool hasPointerDevice, bool isPointerVisible)
{
    CALL_DEBUG_ENTER;
    hasPointerDevice_ = hasPointerDevice;
    if (hasPointerDevice_) {
        SetPointerVisible(getpid(), isPointerVisible);
    } else {
        DeletePointerVisible(getpid());
    }
    DrawManager();
}

void PointerDrawingManager::DrawManager()
{
    CALL_DEBUG_ENTER;
    if (hasDisplay_ && hasPointerDevice_ && surfaceNode_ == nullptr) {
        MMI_HILOGD("Draw pointer begin");
        PointerStyle pointerStyle;
        int32_t ret = WinMgr->GetPointerStyle(pid_, windowId_, pointerStyle);
        MMI_HILOGD("get pid %{publid}d with pointerStyle %{public}d", pid_, pointerStyle.id);
        if (ret != RET_OK) {
            MMI_HILOGE("Get pointer style failed, pointerStyleInfo is nullptr");
            return;
        }
        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayInfo_.id, displayInfo_.width / CALCULATE_MIDDLE, displayInfo_.height / CALCULATE_MIDDLE,
                pointerStyle);
            WinMgr->SendPointerEvent(PointerEvent::POINTER_ACTION_MOVE);
            MMI_HILOGD("Draw manager, mouseStyle:%{public}d, last physical is initial value", pointerStyle.id);
            return;
        }
        DrawPointer(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, pointerStyle);
        WinMgr->SendPointerEvent(PointerEvent::POINTER_ACTION_MOVE);
        MMI_HILOGD("Draw manager, mouseStyle:%{public}d", pointerStyle.id);
        return;
    }
    if (!hasPointerDevice_ && surfaceNode_ != nullptr) {
        MMI_HILOGD("Pointer window destroy start");
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
    }
}

bool PointerDrawingManager::Init()
{
    CALL_DEBUG_ENTER;
    InputDevMgr->Attach(shared_from_this());
    pidInfos_.clear();
    return true;
}

std::shared_ptr<IPointerDrawingManager> IPointerDrawingManager::GetInstance()
{
    if (iPointDrawMgr_ == nullptr) {
        iPointDrawMgr_ = std::make_shared<PointerDrawingManager>();
    }
    return iPointDrawMgr_;
}

void PointerDrawingManager::UpdatePointerVisible()
{
    CALL_DEBUG_ENTER;
    CHKPV(surfaceNode_);
    if (IsPointerVisible() && mouseDisplayState_) {
        surfaceNode_->SetVisible(true);
        MMI_HILOGD("Pointer window show success");
    } else {
        surfaceNode_->SetVisible(false);
        MMI_HILOGD("Pointer window hide success");
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

bool PointerDrawingManager::IsPointerVisible()
{
    CALL_DEBUG_ENTER;
    if (pidInfos_.empty()) {
        MMI_HILOGD("Visible property is true");
        return true;
    }
    auto info = pidInfos_.back();
    MMI_HILOGD("Visible property:%{public}zu.%{public}d-%{public}d", pidInfos_.size(), info.pid, info.visible);
    return info.visible;
}

void PointerDrawingManager::DeletePointerVisible(int32_t pid)
{
    CALL_DEBUG_ENTER;
    auto it = pidInfos_.begin();
    for (; it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    if (it != pidInfos_.end()) {
        if (IsPointerVisible()) {
            InitLayer(MOUSE_ICON(lastMouseStyle_.id));
        }
        UpdatePointerVisible();
    }
}

int32_t PointerDrawingManager::SetPointerVisible(int32_t pid, bool visible)
{
    CALL_DEBUG_ENTER;
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    PidInfo info = { .pid = pid, .visible = visible };
    pidInfos_.push_back(info);
    if (visible) {
        InitLayer(MOUSE_ICON(lastMouseStyle_.id));
    }
    UpdatePointerVisible();
    return RET_OK;
}

void PointerDrawingManager::SetPointerLocation(int32_t pid, int32_t x, int32_t y)
{
    CALL_DEBUG_ENTER;
    FixCursorPosition(x, y);
    lastPhysicalX_ = x;
    lastPhysicalY_ = y;
    if (surfaceNode_ != nullptr) {
        surfaceNode_->SetBounds(x,
            y,
            surfaceNode_->GetStagingProperties().GetBounds().z_,
            surfaceNode_->GetStagingProperties().GetBounds().w_);
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window move success");
    }
}

int32_t PointerDrawingManager::UpdateDefaultPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle)
{
    if (windowId != GLOBAL_WINDOW_ID) {
        MMI_HILOGD("No need to change the default icon style");
        return RET_OK;
    }
    PointerStyle style;
    int32_t ret = WinMgr->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style);
    if (ret != RET_OK) {
        MMI_HILOGE("Get global pointer style failed!");
        return RET_ERR;
    }
    if (pointerStyle.id != style.id) {
        auto it = mouseIcons_.find(MOUSE_ICON(MOUSE_ICON::DEFAULT));
        if (it == mouseIcons_.end()) {
            MMI_HILOGE("Cannot find the default style in mouseIcons_");
            return RET_ERR;
        }
        std::string newIconPath;
        if (pointerStyle.id == MOUSE_ICON::DEFAULT) {
            newIconPath = DefaultIconPath;
        } else {
            newIconPath = mouseIcons_[MOUSE_ICON(pointerStyle.id)].iconPath;
        }
        MMI_HILOGD("default path has changed from %{public}s to %{public}s",
            it->second.iconPath.c_str(), newIconPath.c_str());
        it->second.iconPath = newIconPath;
    }
    return RET_OK;
}

std::map<MOUSE_ICON, IconStyle> PointerDrawingManager::GetMouseIconPath()
{
    CALL_DEBUG_ENTER;
    return mouseIcons_;
}

int32_t PointerDrawingManager::SetPointerStylePreference(PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    int32_t errCode = RET_OK;
    std::shared_ptr<NativePreferences::Preferences> pref =
        NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
    if (pref == nullptr) {
        MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
        return RET_ERR;
    }
    std::string name = "pointerStyle";
    int32_t ret = pref->PutInt(name, pointerStyle.id);
    if (ret != RET_OK) {
        MMI_HILOGE("Put style is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    ret = pref->FlushSync();
    if (ret != RET_OK) {
        MMI_HILOGE("Flush sync is failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGD("Set pointer style successfully, style:%{public}d", pointerStyle.id);
    NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);
    return RET_OK;
}

int32_t PointerDrawingManager::SetPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (windowId == GLOBAL_WINDOW_ID) {
        int32_t ret = SetPointerStylePreference(pointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Set style preference is failed, ret:%{public}d", ret);
            return RET_ERR;
        }
    }
    auto it = mouseIcons_.find(MOUSE_ICON(pointerStyle.id));
    if (it == mouseIcons_.end()) {
        MMI_HILOGE("The param pointerStyle is invalid");
        return RET_ERR;
    }
    int32_t ret = UpdateDefaultPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Update default pointer iconPath failed!");
        return ret;
    }

    ret = WinMgr->SetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer style failed");
        return ret;
    }

    if (!InputDevMgr->HasPointerDevice()) {
        MMI_HILOGD("The pointer device is not exist");
        return RET_OK;
    }

    if (!WinMgr->IsNeedRefreshLayer(windowId)) {
        MMI_HILOGD("Not need refresh layer, window type:%{public}d, pointer style:%{public}d",
            windowId, pointerStyle.id);
        return RET_OK;
    }
    if (windowId != GLOBAL_WINDOW_ID && (pointerStyle.id == MOUSE_ICON::DEFAULT &&
        mouseIcons_[MOUSE_ICON(pointerStyle.id)].iconPath != DefaultIconPath)) {
        PointerStyle style;
        int32_t ret = WinMgr->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style);
        if (ret != RET_OK) {
            MMI_HILOGE("Get global pointer style failed!");
            return RET_ERR;
        }
        pointerStyle = style;
    }
    DrawPointerStyle(pointerStyle);
    MMI_HILOGD("Window id:%{public}d set pointer style:%{public}d success", windowId, pointerStyle.id);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerStyle(int32_t pid, int32_t windowId, PointerStyle &pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (windowId == GLOBAL_WINDOW_ID) {
        int32_t errCode = RET_OK;
        std::shared_ptr<NativePreferences::Preferences> pref =
            NativePreferences::PreferencesHelper::GetPreferences(MOUSE_FILE_NAME, errCode);
        if (pref == nullptr) {
            MMI_HILOGE("pref is nullptr,  errCode: %{public}d", errCode);
            return RET_ERR;
        }
        std::string name = "pointerStyle";
        int32_t style = pref->GetInt(name, DEFAULT_POINTER_STYLE);
        MMI_HILOGD("Get pointer style successfully, pointerStyle:%{public}d", style);
        if (style == CURSOR_CIRCLE_STYLE) {
            pointerStyle.id = style;
            NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);
            return RET_OK;
        }
        NativePreferences::PreferencesHelper::RemovePreferencesFromCache(MOUSE_FILE_NAME);
    }
    int32_t ret = WinMgr->GetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Get pointer style failed, pointerStyleInfo is nullptr");
        return ret;
    }
    MMI_HILOGD("Window id:%{public}d get pointer style:%{public}d success", windowId, pointerStyle.id);
    return RET_OK;
}

int32_t PointerDrawingManager::ClearWindowPointerStyle(int32_t pid, int32_t windowId)
{
    CALL_DEBUG_ENTER;
    return WinMgr->ClearWindowPointerStyle(pid, windowId);
}

void PointerDrawingManager::DrawPointerStyle(const PointerStyle& pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (hasDisplay_ && hasPointerDevice_) {
        if (surfaceNode_ != nullptr) {
            surfaceNode_->AttachToDisplay(screenId_);
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayInfo_.id, displayInfo_.width / CALCULATE_MIDDLE, displayInfo_.height / CALCULATE_MIDDLE,
                pointerStyle);
            MMI_HILOGD("Draw pointer style, mouseStyle:%{public}d", pointerStyle.id);
            return;
        }

        DrawPointer(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, pointerStyle);
        MMI_HILOGD("Draw pointer style, mouseStyle:%{public}d", pointerStyle.id);
    }
}

void PointerDrawingManager::CheckMouseIconPath()
{
    for (auto iter = mouseIcons_.begin(); iter != mouseIcons_.end();) {
        if ((ReadCursorStyleFile(iter->second.iconPath)) != RET_OK) {
            iter = mouseIcons_.erase(iter);
            continue;
        }
        ++iter;
    }
}

void PointerDrawingManager::InitStyle()
{
    CALL_DEBUG_ENTER;
    mouseIcons_ = {
        {DEFAULT, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Default.svg"}},
        {EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "East.svg"}},
        {WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "West.svg"}},
        {SOUTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "South.svg"}},
        {NORTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North.svg"}},
        {WEST_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "West_East.svg"}},
        {NORTH_SOUTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North_South.svg"}},
        {NORTH_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North_East.svg"}},
        {NORTH_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North_West.svg"}},
        {SOUTH_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "South_East.svg"}},
        {SOUTH_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "South_West.svg"}},
        {NORTH_EAST_SOUTH_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North_East_South_West.svg"}},
        {NORTH_WEST_SOUTH_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "North_West_South_East.svg"}},
        {CROSS, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Cross.svg"}},
        {CURSOR_COPY, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Copy.svg"}},
        {CURSOR_FORBID, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Forbid.svg"}},
        {COLOR_SUCKER, {ANGLE_SW, IMAGE_POINTER_DEFAULT_PATH + "Colorsucker.svg"}},
        {HAND_GRABBING, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Hand_Grabbing.svg"}},
        {HAND_OPEN, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Hand_Open.svg"}},
        {HAND_POINTING, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Hand_Pointing.svg"}},
        {HELP, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Help.svg"}},
        {CURSOR_MOVE, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Move.svg"}},
        {RESIZE_LEFT_RIGHT, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Resize_Left_Right.svg"}},
        {RESIZE_UP_DOWN, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Resize_Up_Down.svg"}},
        {SCREENSHOT_CHOOSE, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Screenshot_Cross.svg"}},
        {SCREENSHOT_CURSOR, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Screenshot_Cursor.png"}},
        {TEXT_CURSOR, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Text_Cursor.svg"}},
        {ZOOM_IN, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Zoom_In.svg"}},
        {ZOOM_OUT, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Zoom_Out.svg"}},
        {MIDDLE_BTN_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_East.svg"}},
        {MIDDLE_BTN_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_West.svg"}},
        {MIDDLE_BTN_SOUTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_South.svg"}},
        {MIDDLE_BTN_NORTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_North.svg"}},
        {MIDDLE_BTN_NORTH_SOUTH, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_North_South.svg"}},
        {MIDDLE_BTN_NORTH_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_North_East.svg"}},
        {MIDDLE_BTN_NORTH_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_North_West.svg"}},
        {MIDDLE_BTN_SOUTH_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_South_East.svg"}},
        {MIDDLE_BTN_SOUTH_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MID_Btn_South_West.svg"}},
        {MIDDLE_BTN_NORTH_SOUTH_WEST_EAST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH +
            "MID_Btn_North_South_West_East.svg"}},
        {HORIZONTAL_TEXT_CURSOR, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Horizontal_Text_Cursor.svg"}},
        {CURSOR_CROSS, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Cursor_Cross.svg"}},
        {CURSOR_CIRCLE, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Cursor_Circle.png"}},
        {LOADING, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Loading.svg"}},
        {RUNNING, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Loading_Left.svg"}},
        {RUNNING_LEFT, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Loading_Left.svg"}},
        {RUNNING_RIGHT, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "Loading_Right.svg"}},
        {DEVELOPER_DEFINED_ICON, {ANGLE_NW, IMAGE_POINTER_DEFAULT_PATH + "Default.svg"}},
    };
    CheckMouseIconPath();
}
} // namespace MMI
} // namespace OHOS
