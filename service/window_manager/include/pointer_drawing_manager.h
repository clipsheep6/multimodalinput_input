/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef POINTER_DRAWING_MANAGER_H
#define POINTER_DRAWING_MANAGER_H

#include <iostream>
#include <list>

#include "common/rs_thread_handler.h"
#include "draw/canvas.h"
#include "event_handler.h"
#include "nocopyable.h"
#include "pixel_map.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "window.h"

#include "device_observer.h"
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
#include "hardware_cursor_pointer_manager.h"
#include "dm_common.h"
#include "screen_manager_lite.h"
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
#include "i_pointer_drawing_manager.h"
#include "mouse_event_normalize.h"
#include "setting_observer.h"
#include "struct_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t DEFAULT_FRAME_RATE { 30 };
constexpr int32_t INVALID_DISPLAY_ID { -1 };
constexpr float HARDWARE_CANVAS_SIZE { 512.0f };
} // namespace

struct isMagicCursor {
    std::string name;
    bool isShow { false };
};

struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<Media::PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<Media::PixelMap> pixelMap_ { nullptr };
};
class DelegateInterface;
class PointerDrawingManager final : public IPointerDrawingManager,
                                    public IDeviceObserver,
                                    public std::enable_shared_from_this<PointerDrawingManager> {
public:
    PointerDrawingManager();
    DISALLOW_COPY_AND_MOVE(PointerDrawingManager);
    ~PointerDrawingManager();
    void DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
        const PointerStyle pointerStyle, Direction direction) override;
    void UpdateDisplayInfo(const DisplayInfo& displayInfo) override;
    void OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo) override;
    void OnWindowInfo(const WinInfo &info) override;
    void UpdatePointerDevice(bool hasPointerDevice, bool isPointerVisible, bool isHotPlug) override;
    bool Init() override;
    int32_t SetPointerColor(int32_t color) override;
    int32_t GetPointerColor() override;
    void DeletePointerVisible(int32_t pid) override;
    int32_t SetPointerVisible(int32_t pid, bool visible, int32_t priority, bool isHap) override;
    bool GetPointerVisible(int32_t pid) override;
    int32_t SetPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle,
        bool isUiExtension = false) override;
    int32_t ClearWindowPointerStyle(int32_t pid, int32_t windowId) override;
    int32_t GetPointerStyle(int32_t pid, int32_t windowId, PointerStyle &pointerStyle,
        bool isUiExtension = false) override;
    int32_t SetPointerSize(int32_t size) override;
    int32_t GetPointerSize() override;
    int32_t GetCursorSurfaceId(uint64_t &surfaceId) override;
    void DrawPointerStyle(const PointerStyle& pointerStyle) override;
    bool IsPointerVisible() override;
    void SetPointerLocation(int32_t x, int32_t y) override;
    void AdjustMouseFocus(Direction direction, ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    void SetMouseDisplayState(bool state) override;
    bool GetMouseDisplayState() const override;
    int32_t SetCustomCursor(void* pixelMap, int32_t pid, int32_t windowId, int32_t focusX, int32_t focusY) override;
    int32_t SetCustomCursor(int32_t pid, int32_t windowId, CustomCursor cursor, CursorOptions options) override;
    int32_t SetMouseIcon(int32_t pid, int32_t windowId, void* pixelMap) override;
    int32_t SetMouseHotSpot(int32_t pid, int32_t windowId, int32_t hotSpotX, int32_t hotSpotY) override;
    PointerStyle GetLastMouseStyle() override;
    const std::map<MOUSE_ICON, IconStyle>& GetMouseIconPath() override;
    IconStyle GetIconStyle(const MOUSE_ICON mouseStyle) override;
    bool HasMagicCursor();
    int32_t DrawCursor(const MOUSE_ICON mouseStyle);
    int32_t SwitchPointerStyle() override;
    void DrawMovePointer(int32_t displayId, int32_t physicalX, int32_t physicalY) override;
    void Dump(int32_t fd, const std::vector<std::string> &args) override;
    void AttachToDisplay();
    int32_t EnableHardwareCursorStats(int32_t pid, bool enable) override;
    int32_t GetHardwareCursorStats(int32_t pid, uint32_t &frameCount, uint32_t &vsyncCount) override;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t GetPointerSnapshot(void *pixelMapPtr) override;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    void InitPointerCallback() override;
    void InitPointerObserver() override;
    void OnSessionLost(int32_t pid) override;
    int32_t SkipPointerLayer(bool isSkip) override;
    void SetDelegateProxy(std::shared_ptr<DelegateInterface> proxy) override
    {
        delegateProxy_ = proxy;
    }
    void DestroyPointerWindow() override;
    void DrawScreenCenterPointer(const PointerStyle& pointerStyle) override;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    void InitScreenInfo() override;
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

private:
    IconStyle GetIconType(MOUSE_ICON mouseIcon);
    void GetPreferenceKey(std::string &name);
    void DrawLoadingPointerStyle(const MOUSE_ICON mouseStyle);
    void DrawRunningPointerAnimate(const MOUSE_ICON mouseStyle);
    void CreatePointerWindow(int32_t displayId, int32_t physicalX, int32_t physicalY, Direction direction);
    sptr<OHOS::Surface> GetLayer();
    sptr<OHOS::SurfaceBuffer> GetSurfaceBuffer(sptr<OHOS::Surface> layer);
    sptr<OHOS::SurfaceBuffer> RetryGetSurfaceBuffer(sptr<OHOS::Surface> layer);
    void DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const MOUSE_ICON mouseStyle = MOUSE_ICON::DEFAULT);
    void DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas, const MOUSE_ICON mouseStyle);
    void DrawManager();
    void FixCursorPosition(int32_t &physicalX, int32_t &physicalY);
    std::shared_ptr<OHOS::Media::PixelMap> LoadCursorSvgWithColor(MOUSE_ICON type, int32_t color);
    std::shared_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(MOUSE_ICON type);
    void UpdatePointerVisible();
    int32_t UpdateDefaultPointerStyle(int32_t pid, int32_t windowId, PointerStyle style, bool isUiExtension = false);
    void CheckMouseIconPath();
    void InitStyle();
    int32_t InitLayer(const MOUSE_ICON mouseStyle);
    int32_t SetPointerStylePreference(PointerStyle pointerStyle);
    void UpdateMouseStyle();
    int32_t UpdateCursorProperty(void* pixelMap, const int32_t &focusX, const int32_t &focusY);
    int32_t UpdateCursorProperty(CustomCursor cursor);
    void RotateDegree(Direction direction);
    int32_t DrawMovePointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
        PointerStyle pointerStyle, Direction direction);
    void AdjustMouseFocusByDirection0(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    void AdjustMouseFocusByDirection90(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    void AdjustMouseFocusByDirection180(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    void AdjustMouseFocusByDirection270(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    void CreateMagicCursorChangeObserver();
    int32_t CreatePointerSwitchObserver(isMagicCursor& item);
    void UpdateStyleOptions();
    int32_t GetIndependentPixels();
    bool CheckPointerStyleParam(int32_t windowId, PointerStyle pointerStyle);
    std::map<MOUSE_ICON, IconStyle>& GetMouseIcons();
    void UpdateIconPath(const MOUSE_ICON mouseStyle, std::string iconPath);
    std::shared_ptr<Rosen::Drawing::ColorSpace> ConvertToColorSpace(Media::ColorSpace colorSpace);
    Rosen::Drawing::ColorType PixelFormatToColorType(Media::PixelFormat pixelFormat);
    Rosen::Drawing::AlphaType AlphaTypeToAlphaType(Media::AlphaType alphaType);
    std::shared_ptr<Rosen::Drawing::Image> ExtractDrawingImage(std::shared_ptr<Media::PixelMap> pixelMap);
    void DrawImage(OHOS::Rosen::Drawing::Canvas &canvas, MOUSE_ICON mouseStyle);
    int32_t UpdateLoadingAndLoadingRightPixelMap();
    void InitLoadingAndLoadingRightPixelMap();
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    void SetPixelMap(std::shared_ptr<OHOS::Media::PixelMap> pixelMap);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    void ForceClearPointerVisiableStatus() override;
    int32_t UpdateSurfaceNodeBounds(int32_t physicalX, int32_t physicalY);
    void CreateCanvasNode();
    void SetSurfaceNodeVisible(bool visible);
    float CalculateHardwareXOffset(ICON_TYPE iconType);
    float CalculateHardwareYOffset(ICON_TYPE iconType);
    bool SetTraditionsHardWareCursorLocation(int32_t displayId, int32_t physicalX, int32_t physicalY,
        ICON_TYPE iconType);
    void SetHardwareCursorPosition(int32_t displayId, int32_t physicalX, int32_t physicalY,
        PointerStyle pointerStyle);
    void CreateDynamicCanvas();
    int32_t ParsingDynamicImage(MOUSE_ICON mouseStyle);
    void DrawDynamicImage(OHOS::Rosen::Drawing::Canvas &canvas, MOUSE_ICON mouseStyle);
    std::shared_ptr<OHOS::Media::PixelMap> GetUserIconCopy();
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    bool SetDynamicHardWareCursorLocation(int32_t physicalX, int32_t physicalY, MOUSE_ICON mouseStyle);
    void RenderThreadLoop();
    int32_t RequestNextVSync();
    void OnVsync(uint64_t timestamp);
    void PostTask(Rosen::RSTaskMessage::RSTask task);
    void DoHardwareCursorDraw();
    int32_t FlushBuffer();
    int32_t GetSurfaceInformation();
    void UpdateBindDisplayId(int32_t displayId);
    void PostTaskRSLocation(int32_t physicalX, int32_t physicalY, std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode);
    void DrawTraditionsCursor(MOUSE_ICON mouseStyle);
    int32_t InitVsync(MOUSE_ICON mouseStyle);
    void DumpScreenInfo(std::ostringstream& oss);
    bool IsSupported() override;
    int32_t PrepareBuffer(uint32_t width, uint32_t height);
    int32_t DrawHardwareCursor(const MOUSE_ICON mouseStyle);
    int32_t DrawCursor(const MOUSE_ICON mouseStyle, uint32_t width, uint32_t height);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

private:
    struct PidInfo {
        int32_t pid { 0 };
        bool visible { false };
    };
    struct loadingAndLoadingPixelMapInfo {
        std::shared_ptr<OHOS::Media::PixelMap> pixelMap { nullptr };
        int32_t imageWidth { 0 };
        int32_t imageHeight { 0 };
        int32_t pointerColor { 0 };
    };
    bool hasDisplay_ { false };
    DisplayInfo displayInfo_ {};
    bool hasPointerDevice_ { false };
    int32_t lastPhysicalX_ { -1 };
    int32_t lastPhysicalY_ { -1 };
    PointerStyle lastMouseStyle_ {};
    PointerStyle currentMouseStyle_ {};
    PointerStyle lastDrawPointerStyle_ {};
    int32_t pid_ { 0 };
    int32_t windowId_ { 0 };
    int32_t imageWidth_ { 0 };
    int32_t imageHeight_ { 0 };
    int32_t canvasWidth_ = 64;
    int32_t canvasHeight_ = 64;
    int32_t cursorWidth_ { 0 };
    int32_t cursorHeight_ { 0 };
    std::map<MOUSE_ICON, IconStyle> mouseIcons_;
    std::list<PidInfo> pidInfos_;
    std::list<PidInfo> hapPidInfos_;
    bool mouseDisplayState_ { false };
    bool mouseIconUpdate_ { false };
    std::shared_ptr<OHOS::Media::PixelMap> userIcon_ { nullptr };
    uint64_t screenId_ { 0 };
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode_;
    std::shared_ptr<Rosen::RSCanvasNode> canvasNode_;
    int32_t userIconHotSpotX_ { 0 };
    int32_t userIconHotSpotY_ { 0 };
    int32_t tempPointerColor_ { -1 };
    Direction lastDirection_ { DIRECTION0 };
    Direction currentDirection_ { DIRECTION0 };
    isMagicCursor hasMagicCursor_;
    bool hasInitObserver_ { false };
    std::atomic<bool> hasHardwareCursorAnimate_ { false };
    std::atomic<bool> hasLoadingPointerStyle_ { false };
    int32_t frameCount_ { DEFAULT_FRAME_RATE };
    int32_t currentFrame_ { 0 };
    sptr<OHOS::Surface> layer_ { nullptr };
    sptr<OHOS::SurfaceBuffer> buffer_ { nullptr };
    std::shared_ptr<uint8_t *> addr_ { nullptr };
    int32_t displayId_ { INVALID_DISPLAY_ID };
    std::shared_ptr<OHOS::Rosen::Drawing::Bitmap> dynamicBitmap_ { nullptr };
    std::shared_ptr<OHOS::Rosen::Drawing::Canvas> dynamicCanvas_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> runningRightImage_ { nullptr };
    std::shared_ptr<Rosen::Drawing::Image> image_ { nullptr };
    std::shared_ptr<AppExecFwk::EventRunner> runner_ { nullptr };
    std::shared_ptr<AppExecFwk::EventHandler> handler_ { nullptr };
    std::shared_ptr<Rosen::VSyncReceiver> receiver_ { nullptr };
    std::atomic<bool> isRenderRuning_{ false };
    std::unique_ptr<std::thread> renderThread_ { nullptr };
    bool isInit_ { false };
    std::mutex mtx_;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    std::shared_ptr<HardwareCursorPointerManager> hardwareCursorPointerManager_ { nullptr };
    std::vector<sptr<OHOS::SurfaceBuffer>> cursorBuffers_;
    uint32_t bufferId_ { 0 };
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    float hardwareCanvasSize_ { HARDWARE_CANVAS_SIZE };
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap_ { nullptr };
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    std::shared_ptr<DelegateInterface> delegateProxy_ { nullptr };
    int32_t lastDisplayId_ { DEFAULT_DISPLAY_ID };
    std::map<MOUSE_ICON, loadingAndLoadingPixelMapInfo> mousePixelMap_;
    int32_t initLoadingAndLoadingRightPixelTimerId_ { -1 };
    int releaseFence_ { -1 };
};

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
class ScreenModeChangeListener : public Rosen::ScreenManagerLite::IScreenModeChangeListener {
public:
    ScreenModeChangeListener() = default;
    virtual ~ScreenModeChangeListener() = default;
    void NotifyScreenModeChange(const std::vector<sptr<Rosen::ScreenInfo>> &screenInfos) override;
};

class ScreenListener : public Rosen::ScreenManagerLite::IScreenListener {
public:
    void OnConnect(Rosen::ScreenId screenId) override{};
    void OnDisconnect(Rosen::ScreenId screenId) override;
    void OnChange(Rosen::ScreenId screenId) override{};
};
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
} // namespace MMI
} // namespace OHOS
#endif // POINTER_DRAWING_MANAGER_H
