/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "pointer_renderer.h"

#include <regex>
#include "image_source.h"
#include "window_info.h"
#include "util.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_CURSOR
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "PointerRenderer"

constexpr uint32_t RENDER_STRIDE{4};
constexpr int32_t DEVCIE_INDEPENDENT_PIXELS{40};
constexpr float INCREASE_RATIO{1.22f};
constexpr int32_t MIN_POINTER_COLOR{0x000000};
constexpr int32_t MAX_POINTER_COLOR{0xFFFFFF};
constexpr int32_t OTHER_POINTER_COLOR{0x171717};
constexpr float CALCULATE_IMAGE_MIDDLE{2.0f};
constexpr uint32_t FOCUS_POINT{256};
constexpr float CALCULATE_MOUSE_ICON_BIAS{ 5.0f / 33.0f };
constexpr float ROTATION_ANGLE90 {90.0f};
constexpr float MAX_DPI {1.93f};   //1.93 is the max dpi for hardware cursor pointer to limit the image size within 256
constexpr uint32_t MAX_POINTER_SIZE {7};
const std::string IMAGE_POINTER_DEFAULT_PATH = "/system/etc/multimodalinput/mouse_icon/";

namespace OHOS::MMI {

int32_t RenderConfig::GetImageSize() const
{
    float increaseRatio = AdjustIncreaseRatio(dpi);
    return pow(increaseRatio, size - 1) * dpi * DEVCIE_INDEPENDENT_PIXELS;
}

float RenderConfig::AdjustIncreaseRatio(float originDpi) const
{
    if (originDpi == 0.0f) {
        MMI_HILOGE("The originDpi cannot be 0, return default increase ratio");
        return INCREASE_RATIO;
    }
    if (originDpi <= MAX_DPI) {
        return INCREASE_RATIO;
    } else {
        return INCREASE_RATIO * pow(MAX_DPI / originDpi, float(1.0f / float(MAX_POINTER_SIZE - 1)));
    }
}

std::string RenderConfig::ToString() const
{
    std::ostringstream oss;
    oss << "{style=" << style_ << ", align=" << align_ << ", color=" << color
        << ", size=" << size << ", direction=" << direction
        <<", dpi=" << dpi
        << ", isHard=" << isHard << ", ImageSize=" << GetImageSize() << "}";
    return oss.str();
}

int32_t RenderConfig::GetOffsetX() const
{
    int32_t width = this->GetImageSize();
    switch (this->align_) {
        case ANGLE_E:
            return FOCUS_POINT;
        case ANGLE_S:       // fall-though
        case ANGLE_N:       // fall-though
        case ANGLE_CENTER:  // fall-though
            return static_cast<int32_t>(FOCUS_POINT - width / CALCULATE_IMAGE_MIDDLE);
        case ANGLE_W:       // fall-though
        case ANGLE_SE:      // fall-though
        case ANGLE_NE:      // fall-though
            return FOCUS_POINT - width;
        case ANGLE_SW:
            return FOCUS_POINT;
        case ANGLE_NW:
            return FOCUS_POINT - this->userIconHotSpotX;
        case ANGLE_NW_RIGHT:
            return FOCUS_POINT - width * CALCULATE_MOUSE_ICON_BIAS;
        default:
            MMI_HILOGW("No need calculate physicalX offset");
            return FOCUS_POINT;
    }
}

int32_t RenderConfig::GetOffsetY() const
{
    int32_t height = this->GetImageSize();
    switch (this->align_) {
        case ANGLE_E:
            return FOCUS_POINT - height / CALCULATE_IMAGE_MIDDLE;
        case ANGLE_S:
            return FOCUS_POINT;
        case ANGLE_W:
            return FOCUS_POINT - height;
        case ANGLE_N:
            return FOCUS_POINT - height;
        case ANGLE_SE:
            return FOCUS_POINT - height;
        case ANGLE_NE:
            return FOCUS_POINT;
        case ANGLE_SW:
            return FOCUS_POINT - height;
        case ANGLE_NW:
            return FOCUS_POINT - this->userIconHotSpotY;
        case ANGLE_CENTER:
            return FOCUS_POINT - height / CALCULATE_IMAGE_MIDDLE;
        case ANGLE_NW_RIGHT:
            return FOCUS_POINT;
        default:
            MMI_HILOGW("No need calculate physicalY offset");
            return FOCUS_POINT;
    }
}

image_ptr_t PointerRenderer::UserIconScale(uint32_t width, uint32_t height, const RenderConfig &cfg)
{
    image_ptr_t image = nullptr;
    image = ExtractDrawingImage(cfg.userIconPixelMap);
    return image;
}

int32_t PointerRenderer::Render(uint8_t *addr, uint32_t width, uint32_t height, const RenderConfig &cfg)
{
    CHKPR(addr, RET_ERR);
    MMI_HILOGD("Render cfg:%{private}s, width:%{public}u, height:%{public}u", cfg.ToString().c_str(), width, height);

    uint32_t addrSize = width * height * RENDER_STRIDE;
    if (cfg.style_ == MOUSE_ICON::TRANSPARENT_ICON) {
        (void)memset_s(addr, addrSize, 0, addrSize);
        return RET_OK;
    }

    // construct bitmap
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format {
        OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUE,
    };
    if (!bitmap.Build(width, height, format)) {
        MMI_HILOGE("Bitmap build failed");
    }

    // construct canvas and bind to bitmap
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    if (cfg.direction) {
        int32_t directionFlag = -1;
        int32_t degree = static_cast<int32_t>(directionFlag * static_cast<int32_t>(cfg.direction) * ROTATION_ANGLE90);
        canvas.Rotate(degree, FOCUS_POINT, FOCUS_POINT);
    }
    image_ptr_t image = nullptr;
    if (cfg.userIconPixelMap == nullptr) {
        image = LoadPointerImage(cfg);
    } else {
        image = UserIconScale(width, height, cfg);
    }
    CHKPR(image, RET_ERR);
    // Draw image on canvas
    canvas.DrawImage(*image, cfg.GetOffsetX(), cfg.GetOffsetY(), Rosen::Drawing::SamplingOptions());

    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), bitmap.ComputeByteSize());
    if (ret != EOK) {
        MMI_HILOGE("Memcpy_s failed");
        return RET_ERR;
    }
    return RET_OK;
}

image_ptr_t PointerRenderer::LoadPointerImage(const RenderConfig &cfg)
{
    auto pixelmap = LoadCursorSvgWithColor(cfg);
    return ExtractDrawingImage(pixelmap);
}

static void ChangeSvgCursorColor(std::string& str, int32_t color)
{
    std::string targetColor = IntToHexRGB(color);
    StringReplace(str, "#000000", targetColor);
    if (color == MAX_POINTER_COLOR) {
        // stroke=\"#FFFFFF" fill="#000000" stroke-linejoin="round" transform="xxx"
        std::regex re("(<path.*)(stroke=\"#[a-fA-F0-9]{6}\")(.*path>)");
        str = std::regex_replace(str, re, "$1stroke=\"#000000\"$3");
    }
}

void SetCursorColorBaseOnStyle(const RenderConfig &cfg, OHOS::Media::DecodeOptions &decodeOpts)
{
    const bool isHandColor =
        (cfg.style_ == HAND_GRABBING) || (cfg.style_ == HAND_OPEN) || (cfg.style_ == HAND_POINTING);
    if (!isHandColor) {
        return;
    }

    if ((cfg.color == MAX_POINTER_COLOR) || (cfg.color == MIN_POINTER_COLOR) || (cfg.color == OTHER_POINTER_COLOR)) {
        decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = MAX_POINTER_COLOR};
        decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};
        return;
    }

    decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = cfg.color};
    if (cfg.color == MAX_POINTER_COLOR) {
        decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};
    } else {
        decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MAX_POINTER_COLOR};
    }
}

pixelmap_ptr_t PointerRenderer::LoadCursorSvgWithColor(const RenderConfig &cfg)
{
    std::string svgContent;
    if (!ReadFile(cfg.path_, svgContent)) {
        MMI_HILOGE("read file failed");
        return nullptr;
    }

    const bool isPartColor = (cfg.style_ == CURSOR_COPY) || (cfg.style_ == CURSOR_FORBID) || (cfg.style_ == HELP);
    if (isPartColor) {
        ChangeSvgCursorColor(svgContent, cfg.color);
    }
    OHOS::Media::SourceOptions opts;
    std::unique_ptr<std::istream> isp(std::make_unique<std::istringstream>(svgContent));
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(std::move(isp), opts, ret);
    if (!imageSource || ret != ERR_OK) {
        MMI_HILOGE("Get ImageSource failed, ret=%{public}u", ret);
    }
    CHKPP(imageSource);

    int32_t imgSize = cfg.GetImageSize();
    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {.width = imgSize, .height = imgSize};
    if (!isPartColor) {
        decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = cfg.color};
        if (cfg.color == MAX_POINTER_COLOR) {
            decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};
        } else {
            decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MAX_POINTER_COLOR};
        }
        SetCursorColorBaseOnStyle(cfg, decodeOpts);
    }

    pixelmap_ptr_t pmap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (ret != ERR_OK) {
        MMI_HILOGE("CreatePixelMap failed, ret=%{public}u", ret);
    }
    return pmap;
}

class PixelMapContext {
public:
    explicit PixelMapContext(pixelmap_ptr_t pixelMap) : pixelMap_(pixelMap) {}
    ~PixelMapContext()
    {
        pixelMap_ = nullptr;
    }

private:
    pixelmap_ptr_t pixelMap_{nullptr};
};

static void PixelMapReleaseProc(const void * /* pixels */, void *context)
{
    PixelMapContext *ctx = static_cast<PixelMapContext *>(context);
    if (ctx != nullptr) {
        delete ctx;
    }
}


static Rosen::Drawing::ColorType PixelFormatToColorType(Media::PixelFormat pixelFormat)
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
        case Media::PixelFormat::UNKNOWN:       // fall-though
        case Media::PixelFormat::ARGB_8888:     // fall-though
        case Media::PixelFormat::RGB_888:       // fall-though
        case Media::PixelFormat::NV21:          // fall-though
        case Media::PixelFormat::NV12:          // fall-though
        case Media::PixelFormat::CMYK:          // fall-though
        default:
            return Rosen::Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}

static Rosen::Drawing::AlphaType AlphaTypeToAlphaType(Media::AlphaType alphaType)
{
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

static std::shared_ptr<Rosen::Drawing::ColorSpace> ConvertToColorSpace(Media::ColorSpace colorSpace)
{
    switch (colorSpace) {
        case Media::ColorSpace::DISPLAY_P3:
            return Rosen::Drawing::ColorSpace::CreateRGB(
                Rosen::Drawing::CMSTransferFuncType::SRGB, Rosen::Drawing::CMSMatrixType::DCIP3);
        case Media::ColorSpace::LINEAR_SRGB:
            return Rosen::Drawing::ColorSpace::CreateSRGBLinear();
        case Media::ColorSpace::SRGB:
            return Rosen::Drawing::ColorSpace::CreateSRGB();
        default:
            return Rosen::Drawing::ColorSpace::CreateSRGB();
    }
}

image_ptr_t PointerRenderer::ExtractDrawingImage(pixelmap_ptr_t pixelMap)
{
    CHKPP(pixelMap);
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Rosen::Drawing::ImageInfo drawingImageInfo {
        imageInfo.size.width,
        imageInfo.size.height,
        PixelFormatToColorType(imageInfo.pixelFormat),
        AlphaTypeToAlphaType(imageInfo.alphaType),
        ConvertToColorSpace(imageInfo.colorSpace),
    };
    Rosen::Drawing::Pixmap imagePixmap(drawingImageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()),
        pixelMap->GetRowStride());
    PixelMapContext *releaseContext = new (std::nothrow) PixelMapContext(pixelMap);
    CHKPP(releaseContext);
    auto image = Rosen::Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (image == nullptr) {
        MMI_HILOGE("ExtractDrawingImage image fail");
        delete releaseContext;
        releaseContext = nullptr;
    }
    return image;
}

int32_t PointerRenderer::DrawImage(OHOS::Rosen::Drawing::Canvas &canvas, const RenderConfig &cfg)
{
    if (cfg.style_ == MOUSE_ICON::LOADING) {
        auto loadingImg = FindImg(cfg);
        if (loadingImg == nullptr) {
            loadingImg = LoadPointerImage(cfg);
            CHKPR(loadingImg, RET_ERR);
            PushImg(cfg, loadingImg);
        }
        canvas.Rotate(cfg.rotationAngle, cfg.rotationFocusX, cfg.rotationFocusY);
        canvas.DrawImage(*loadingImg, cfg.GetOffsetX(), cfg.GetOffsetY(), Rosen::Drawing::SamplingOptions());
    } else {
        RenderConfig runingLCfg = cfg;
        runingLCfg.style_ = MOUSE_ICON::RUNNING_LEFT;
        runingLCfg.align_ = ANGLE_NW;
        runingLCfg.path_ = IMAGE_POINTER_DEFAULT_PATH + "Loading_Left.svg";
        auto runningImgLeft = FindImg(runingLCfg);
        if (runningImgLeft == nullptr) {
            runningImgLeft = LoadPointerImage(runingLCfg);
            CHKPR(runningImgLeft, RET_ERR);
            PushImg(runingLCfg, runningImgLeft);
        }
        CHKPR(runningImgLeft, RET_ERR);
        canvas.DrawImage(*runningImgLeft, runingLCfg.GetOffsetX(), runingLCfg.GetOffsetY(),
            Rosen::Drawing::SamplingOptions());

        RenderConfig runingRCfg = cfg;
        runingRCfg.style_ = MOUSE_ICON::RUNNING_RIGHT;
        runingRCfg.align_ = ANGLE_NW;
        runingRCfg.path_ = IMAGE_POINTER_DEFAULT_PATH + "Loading_Right.svg";
        auto runningImgRight = FindImg(runingRCfg);
        if (runningImgRight == nullptr) {
            runningImgRight = LoadPointerImage(runingRCfg);
            CHKPR(runningImgRight, RET_ERR);
            PushImg(runingRCfg, runningImgRight);
        }
        canvas.Rotate(runingRCfg.rotationAngle, runingRCfg.rotationFocusX, runingRCfg.rotationFocusY);
        CHKPR(runningImgRight, RET_ERR);
        canvas.DrawImage(*runningImgRight, runingRCfg.GetOffsetX(), runingRCfg.GetOffsetY(),
            Rosen::Drawing::SamplingOptions());
    }
    return RET_OK;
}

int32_t PointerRenderer::DynamicRender(uint8_t *addr, uint32_t width, uint32_t height, const RenderConfig &cfg)
{
    CHKPR(addr, RET_ERR);
    uint32_t addrSize = width * height * RENDER_STRIDE;
    if (cfg.style_ == MOUSE_ICON::TRANSPARENT_ICON) {
        memset_s(addr, addrSize, 0, addrSize);
        return RET_OK;
    }

    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUE };
    if (!bitmap.Build(width, height, format)) {
        MMI_HILOGE("Bitmap build failed");
    }
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);

    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);

    OHOS::Rosen::Drawing::Brush brush;
    brush.SetColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    canvas.DrawBackground(brush);

    if (cfg.direction) {
        int32_t directionFlag = -1;
        int32_t degree = static_cast<int32_t>(directionFlag * static_cast<int32_t>(cfg.direction) * ROTATION_ANGLE90);
        canvas.Rotate(degree, FOCUS_POINT, FOCUS_POINT);
    }

    if (DrawImage(canvas, cfg) != RET_OK) {
        return RET_ERR;
    }
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), bitmap.ComputeByteSize());
    if (ret != EOK) {
        return RET_ERR;
    }
    return RET_OK;
}
} // namespace OHOS::MMI