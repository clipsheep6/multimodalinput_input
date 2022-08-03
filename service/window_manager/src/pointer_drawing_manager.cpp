/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pointer_drawing_manager.h"

#include <display_type.h>

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"

#include "define_multimodal.h"
#include "input_device_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "PointerDrawingManager" };
const std::string IMAGE_POINTER_JPEG_PATH = "/system/etc/multimodalinput/mouse_icon/Default.png";
constexpr int32_t MAX_POINTER_STYLE = 38;
} // namespace
} // namespace MMI
} // namespace OHOS

namespace OHOS {
namespace MMI {
PointerDrawingManager::PointerDrawingManager()
{
    mouseIcons_[MOUSE_ICON::DEFAULT] = "/system/etc/multimodalinput/mouse_icon/Default.png";
    mouseIcons_[MOUSE_ICON::EAST] = "/system/etc/multimodalinput/mouse_icon/East.png";
    mouseIcons_[MOUSE_ICON::WEST] = "/system/etc/multimodalinput/mouse_icon/West.png";
    mouseIcons_[MOUSE_ICON::SOUTH] = "/system/etc/multimodalinput/mouse_icon/South.png";
    mouseIcons_[MOUSE_ICON::NORTH] = "/system/etc/multimodalinput/mouse_icon/North.png";
    mouseIcons_[MOUSE_ICON::WEST_EAST] = "/system/etc/multimodalinput/mouse_icon/westEast.png";
    mouseIcons_[MOUSE_ICON::NORTH_SOUTH] = "/system/etc/multimodalinput/mouse_icon/northSouth.png";
    mouseIcons_[MOUSE_ICON::NORTH_EAST] = "/system/etc/multimodalinput/mouse_icon/northEast.png";
    mouseIcons_[MOUSE_ICON::NORTH_WEST] = "/system/etc/multimodalinput/mouse_icon/northWest.png";
    mouseIcons_[MOUSE_ICON::SOUTH_EAST] = "/system/etc/multimodalinput/mouse_icon/southEast.png";
    mouseIcons_[MOUSE_ICON::SOUTH_WEST] = "/system/etc/multimodalinput/mouse_icon/southWest.png";
    mouseIcons_[MOUSE_ICON::NORTH_EAST_SOUTH_WEST] = "/system/etc/multimodalinput/mouse_icon/northEastSouthWest.png";
    mouseIcons_[MOUSE_ICON::NORTH_WEST_SOUTH_EAST] = "/system/etc/multimodalinput/mouse_icon/northWestSouthEast.png";
    mouseIcons_[MOUSE_ICON::CROSS] = "/system/etc/multimodalinput/mouse_icon/Cross.png";
    mouseIcons_[MOUSE_ICON::CURSOR_COPY] = "/system/etc/multimodalinput/mouse_icon/cursorCopy.png";
    mouseIcons_[MOUSE_ICON::CURSOR_FORBID] = "/system/etc/multimodalinput/mouse_icon/cursorForbid.png";
    mouseIcons_[MOUSE_ICON::COLOR_SUCKER] = "/system/etc/multimodalinput/mouse_icon/colorsucker.png";
    mouseIcons_[MOUSE_ICON::HAND_GRABBING] = "/system/etc/multimodalinput/mouse_icon/handGrabbing.png";
    mouseIcons_[MOUSE_ICON::HAND_OPEN] = "/system/etc/multimodalinput/mouse_icon/handOpen.png";
    mouseIcons_[MOUSE_ICON::HAND_POINTING] = "/system/etc/multimodalinput/mouse_icon/handPointing.png";
    mouseIcons_[MOUSE_ICON::HELP] = "/system/etc/multimodalinput/mouse_icon/Help.png";
    mouseIcons_[MOUSE_ICON::CURSOR_MOVE] = "/system/etc/multimodalinput/mouse_icon/Move.png";
    mouseIcons_[MOUSE_ICON::RESIZE_LEFT_RIGHT] = "/system/etc/multimodalinput/mouse_icon/resizeLeftRight.png";
    mouseIcons_[MOUSE_ICON::RESIZE_UP_DOWN] = "/system/etc/multimodalinput/mouse_icon/resizeUpDown.png";
    mouseIcons_[MOUSE_ICON::SCREENSHOT_CHOOSE] = "/system/etc/multimodalinput/mouse_icon/screenshotChoose.png";
    mouseIcons_[MOUSE_ICON::SCREENSHOT_CURSOR] = "/system/etc/multimodalinput/mouse_icon/screenshotCursor.png";
    mouseIcons_[MOUSE_ICON::TEXT_CURSOR] = "/system/etc/multimodalinput/mouse_icon/textCursor.png";
    mouseIcons_[MOUSE_ICON::ZOOM_IN] = "/system/etc/multimodalinput/mouse_icon/zoomIn.png";
    mouseIcons_[MOUSE_ICON::ZOOM_OUT] = "/system/etc/multimodalinput/mouse_icon/zoomOut.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_EAST] = "/system/etc/multimodalinput/mouse_icon/middleBinEast.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_WEST] = "/system/etc/multimodalinput/mouse_icon/middleBinWest.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_SOUTH] = "/system/etc/multimodalinput/mouse_icon/middleBinSouth.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_NORTH] = "/system/etc/multimodalinput/mouse_icon/middleBinNorth.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_NORTH_SOUTH] = "/system/etc/multimodalinput/mouse_icon/middleBinNorthSouth.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_NORTH_EAST] = "/system/etc/multimodalinput/mouse_icon/middleBinNorthEast.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_NORTH_WEST] = "/system/etc/multimodalinput/mouse_icon/middleBinNorthWest.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_SOUTH_EAST] = "/system/etc/multimodalinput/mouse_icon/middleBinSouthEast.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_SOUTH_WEST] = "/system/etc/multimodalinput/mouse_icon/middleBinSouthWest.png";
    mouseIcons_[MOUSE_ICON::MIDDLE_BTN_NORTH_SOUTH_WEST_EAST] = "/system/etc/multimodalinput/mouse_icon/middleBinNorthSouthWestEast.png";
}

PointerDrawingManager::~PointerDrawingManager() {}

void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY, const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("display:%{public}d,physicalX:%{public}d,physicalY:%{public}d,mouseStyle:%{public}d", displayId, physicalX, physicalY, mouseStyle);
    FixCursorPosition(physicalX, physicalY);
    lastPhysicalX_ = physicalX;
    lastPhysicalY_ = physicalY;
    if (pointerWindow_ != nullptr) {
        if (IsPointerVisible()) {
            pointerWindow_->MoveTo(physicalX, physicalY);
        }

        if (preMouseStyle_ == mouseStyle) {
            MMI_HILOGD("The preMouseStyle is equal with mouseStyle");
            MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
                displayId, physicalX, physicalY);
            return;
        }
        preMouseStyle_ = mouseStyle;
        int32_t ret = InitLayer(mouseStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Init layer failed");
            return;
        }

        MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
            displayId, physicalX, physicalY);
        return;
    }
    
    CreatePointerWindow(displayId, physicalX, physicalY);
    int32_t ret = InitLayer(mouseStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return;
    }

    MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
        displayId, physicalX, physicalY);
    return;
}

int32_t PointerDrawingManager::InitLayer(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerWindow_, RET_ERR);
    sptr<OHOS::Surface> layer = GetLayer();
    if (layer == nullptr) {
        MMI_HILOGE("Init layer is failed, get layer is nullptr");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
        return RET_ERR;
    }

    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        MMI_HILOGE("Init layer is failed, buffer or virAddr is nullptr");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
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
    MMI_HILOGD("Init layer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
    UpdatePointerVisible();
    return RET_OK;
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
    if (direction_ == Direction0 || direction_ == Direction180) {
        if (physicalX > (displayWidth_ - IMAGE_WIDTH / cursorUnit)) {
            physicalX = displayWidth_ - IMAGE_WIDTH / cursorUnit;
        }
        if (physicalY > (displayHeight_ - IMAGE_HEIGHT / cursorUnit)) {
            physicalY = displayHeight_ - IMAGE_HEIGHT / cursorUnit;
        }
    } else {
        if (physicalX > (displayHeight_ - IMAGE_HEIGHT / cursorUnit)) {
            physicalX = displayHeight_ - IMAGE_HEIGHT / cursorUnit;
        }
        if (physicalY > (displayWidth_ - IMAGE_WIDTH / cursorUnit)) {
            physicalY = displayWidth_ - IMAGE_WIDTH / cursorUnit;
        }
    }
}

void PointerDrawingManager::CreatePointerWindow(int32_t displayId, int32_t physicalX, int32_t physicalY)
{
    sptr<OHOS::Rosen::WindowOption> option = new (std::nothrow) OHOS::Rosen::WindowOption();
    CHKPV(option);
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_POINTER);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetDisplayId(displayId);
    OHOS::Rosen::Rect rect = {
        .posX_ = physicalX,
        .posY_ = physicalY,
        .width_ = IMAGE_WIDTH,
        .height_ = IMAGE_HEIGHT,
    };
    option->SetWindowRect(rect);
    option->SetFocusable(false);
    option->SetTouchable(false);
    std::string windowName = "pointer window";
    pointerWindow_ = OHOS::Rosen::Window::Create(windowName, option, nullptr);
}

sptr<OHOS::Surface> PointerDrawingManager::GetLayer()
{
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode = pointerWindow_->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        MMI_HILOGE("Draw pointer is failed, get node is nullptr");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
        return nullptr;
    }
    return surfaceNode->GetSurface();
}

sptr<OHOS::SurfaceBuffer> PointerDrawingManager::GetSurfaceBuffer(sptr<OHOS::Surface> layer) const
{
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    OHOS::BufferRequestConfig config = {
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
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
    DrawPixelmap(canvas, mouseIcons_[mouseStyle]);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        MMI_HILOGE("Memcpy data is error, ret:%{public}d", ret);
        return;
    }
}

void PointerDrawingManager::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas, const std::string& iconPath)
{
    CALL_DEBUG_ENTER;
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = DecodeImageToPixelMap(iconPath);
    CHKPV(pixelmap);
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    canvas.DrawBitmap(*pixelmap, 0, 0);
}

std::unique_ptr<OHOS::Media::PixelMap> PointerDrawingManager::DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    CHKPP(imageSource);
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    MMI_HILOGD("Get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        MMI_HILOGE("The pixelMap is nullptr");
    }
    return pixelMap;
}

void PointerDrawingManager::OnDisplayInfo(int32_t displayId, int32_t pid, int32_t focusWindowId,
                                          int32_t width, int32_t height, Direction direction) 
{
    CALL_DEBUG_ENTER;
    hasDisplay_ = true;
    displayId_ = displayId;
    displayWidth_ = width;
    displayHeight_ = height;
    direction_ = direction;
    focusWindowId_ = focusWindowId;
    pid_ = pid;
    DrawManager();
}

void PointerDrawingManager::UpdatePointerDevice(bool hasPointerDevice)
{
    CALL_DEBUG_ENTER;
    hasPointerDevice_ = hasPointerDevice;
    DrawManager();
}

void PointerDrawingManager::DrawManager()
{
    MMI_HILOGD("Draw manager, focusWindowId:%{public}d", focusWindowId_);
    if (hasDisplay_ && hasPointerDevice_) {
        MMI_HILOGD("Draw pointer begin");
        int32_t mouseStyle = 0;
        int32_t ret = WinMgr->GetPointerStyle(pid_, focusWindowId_, mouseStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Get pointer style failed, mouse style return default style");
        }

        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayId_, displayWidth_/2, displayHeight_/2, MOUSE_ICON(mouseStyle));
            MMI_HILOGD("Draw manager, mouseStyle:%{public}d", mouseStyle);
            return;
        }
       
        DrawPointer(displayId_, lastPhysicalX_, lastPhysicalY_, MOUSE_ICON(mouseStyle));
        MMI_HILOGD("Draw manager, mouseStyle:%{public}d", mouseStyle);
        return;
    }

    if (!hasPointerDevice_ && pointerWindow_ != nullptr) {
        MMI_HILOGD("Destroy draw pointer");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
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

void PointerDrawingManager::DeletePidInfo(int32_t pid)
{
    CALL_DEBUG_ENTER;
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            return;
        }
    }
}

void PointerDrawingManager::UpdatePidInfo(int32_t pid, bool visible)
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
}

void PointerDrawingManager::UpdatePointerVisible()
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerWindow_);
    if (IsPointerVisible()) {
        pointerWindow_->Show();
    } else {
        pointerWindow_->Hide();
    }
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
    DeletePidInfo(pid);
    UpdatePointerVisible();
}

int32_t PointerDrawingManager::SetPointerVisible(int32_t pid, bool visible)
{
    CALL_DEBUG_ENTER;
    UpdatePidInfo(pid, visible);
    UpdatePointerVisible();
    return RET_OK;
}

int32_t PointerDrawingManager::SetPointerStyle(int32_t pid, int32_t windowId, int32_t pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (pointerStyle > MAX_POINTER_STYLE) {
        MMI_HILOGE("The param pointerStyle is invalid");
        return RET_ERR;
    }

    if (WinMgr->SetPointerStyle(pid, windowId, pointerStyle)) {
        MMI_HILOGE("Set pointer style failed");
        return RET_ERR;
    }

    if (!WinMgr->isNeedRefreshLayer(windowId)) {
        MMI_HILOGD("Not need refresh layer, window type:%{public}d, pointer style:%{public}d", windowId, pointerStyle);
        return RET_OK;
    }

    int32_t ret = InitLayer(MOUSE_ICON(pointerStyle));
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return RET_ERR;
    }
    MMI_HILOGD("Window type:%{public}d set pointer style:%{public}d success", windowId, pointerStyle);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerStyle(int32_t pid, int32_t windowId, int32_t &pointerStyle)
{
    CALL_DEBUG_ENTER;
    int32_t ret = WinMgr->GetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Get pointer style failed, pointer style return default style");
        return RET_ERR;
    }

    MMI_HILOGD("Window type:%{public}d get pointer style:%{public}d success", windowId, pointerStyle); 
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
