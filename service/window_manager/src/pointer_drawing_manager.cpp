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
const std::string IMAGE_POINTER_JPEG_PATH = "/system/etc/multimodalinput/mouse_icon/angle.png";
} // namespace
} // namespace MMI
} // namespace OHOS

namespace OHOS {
namespace MMI {
void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t globalX, int32_t globalY)
{
    CALL_LOG_ENTER;
    MMI_HILOGD("display:%{public}d,globalX:%{public}d,globalY:%{public}d", displayId, globalX, globalY);
    FixCursorPosition(globalX, globalY);
    lastGlobalX_ = globalX;
    lastGlobalY_ = globalY;
    if (pointerWindow_ != nullptr) {
        pointerWindow_->MoveTo(globalX, globalY);
        if (IsPointerVisible()) {
            pointerWindow_->Show();
        }
        MMI_HILOGD("leave, display:%{public}d,globalX:%{public}d,globalY:%{public}d", displayId, globalX, globalY);
        return;
    }
    
    CreatePointerWindow(displayId, globalX, globalY);
    CHKPV(pointerWindow_);
    sptr<OHOS::Surface> layer = GetLayer();
    if (layer == nullptr) {
        MMI_HILOGE("draw pointer is faild, get layer is nullptr");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
        return;
    }

    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        MMI_HILOGE("draw pointer is faild, buffer or virAddr is nullptr");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
        return;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    MMI_HILOGD("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
    if (IsPointerVisible()) {
        pointerWindow_->Show();
    }
    MMI_HILOGD("display:%{public}d,globalX:%{public}d,globalY:%{public}d", displayId, globalX, globalY);
}

void PointerDrawingManager::FixCursorPosition(int32_t &globalX, int32_t &globalY)
{
    if (globalX < 0) {
        globalX = 0;
    }

    if (globalY < 0) {
        globalY = 0;
    }

    const int32_t cursorUnit = 16;
    if (globalX > (displayWidth_ - IMAGE_WIDTH / cursorUnit)) {
        globalX = displayWidth_ - IMAGE_WIDTH / cursorUnit;
    }
    if (globalY > (displayHeight_ - IMAGE_HEIGHT / cursorUnit)) {
        globalY = displayHeight_ - IMAGE_HEIGHT / cursorUnit;
    }
}

void PointerDrawingManager::CreatePointerWindow(int32_t displayId, int32_t globalX, int32_t globalY)
{
    sptr<OHOS::Rosen::WindowOption> option = new (std::nothrow) OHOS::Rosen::WindowOption();
    CHKPV(option);
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_POINTER);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetDisplayId(displayId);
    OHOS::Rosen::Rect rect = {
        .posX_ = globalX,
        .posY_ = globalY,
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
        MMI_HILOGE("draw pointer is faild, get node is nullptr");
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
        MMI_HILOGE("request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    return buffer;
}

void PointerDrawingManager::DoDraw(uint8_t *addr, uint32_t width, uint32_t height)
{
    CALL_LOG_ENTER;
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUYE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    DrawPixelmap(canvas);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        MMI_HILOGE("Memcpy data is error, ret:%{public}d", ret);
        return;
    }
}

void PointerDrawingManager::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas)
{
    CALL_LOG_ENTER;
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = DecodeImageToPixelMap(IMAGE_POINTER_JPEG_PATH);
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
    MMI_HILOGD("get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        MMI_HILOGE("pixelMap is nullptr");
    }
    return pixelMap;
}

void PointerDrawingManager::OnDisplayInfo(int32_t displayId, int32_t width, int32_t height) 
{
    CALL_LOG_ENTER;
    hasDisplay_ = true;
    displayId_ = displayId;
    displayWidth_ = width;
    displayHeight_ = height;
    DrawManager();
}

void PointerDrawingManager::UpdatePointerDevice(bool hasPointerDevice)
{
    CALL_LOG_ENTER;
    hasPointerDevice_ = hasPointerDevice;
    DrawManager();
}

void PointerDrawingManager::DrawManager()
{
    if (hasDisplay_ && hasPointerDevice_ && pointerWindow_ == nullptr) {
        MMI_HILOGD("draw pointer begin");
        if (lastGlobalX_ == -1 || lastGlobalY_ == -1) {
            DrawPointer(displayId_, displayWidth_/2, displayHeight_/2);
            return;
        }
        DrawPointer(displayId_, lastGlobalX_, lastGlobalY_);
        return;
    }

    if (!hasPointerDevice_ && pointerWindow_ != nullptr) {
        MMI_HILOGD("destroy draw pointer");
        pointerWindow_->Destroy();
        pointerWindow_ = nullptr;
    }
}

bool PointerDrawingManager::Init()
{
    CALL_LOG_ENTER;
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
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            return;
        }
    }
}

void PointerDrawingManager::UpdataPidInfo(int32_t pid, bool visible)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    PidInfo info = { .pid = pid, .visible = visible };
    pidInfos_.push_back(info);
}

void PointerDrawingManager::UpdataPointerVisible()
{
    CALL_LOG_ENTER;
    CHKPV(pointerWindow_);
    if (IsPointerVisible()) {
        pointerWindow_->Show();
    } else {
        pointerWindow_->Hide();
    }
}

bool PointerDrawingManager::IsPointerVisible()
{
    CALL_LOG_ENTER;
    if (pidInfos_.empty()) {
        MMI_HILOGD("visible property is true");
        return true;
    }
    auto info = pidInfos_.back();
    MMI_HILOGD("visible property:%{public}zu.%{public}d-%{public}d", pidInfos_.size(), info.pid, info.visible);
    return info.visible;
}

void PointerDrawingManager::DeletePointerVisible(int32_t pid)
{
    CALL_LOG_ENTER;
    DeletePidInfo(pid);
    UpdataPointerVisible();
}

int32_t PointerDrawingManager::SetPointerVisible(int32_t pid, bool visible)
{
    CALL_LOG_ENTER;
    UpdataPidInfo(pid, visible);
    UpdataPointerVisible();
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
