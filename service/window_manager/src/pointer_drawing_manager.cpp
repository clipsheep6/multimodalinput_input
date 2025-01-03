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

#include "pointer_drawing_manager.h"

#include <parameters.h>
#include <regex>

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "table_dump.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "magic_pointer_drawing_manager.h"
#include "magic_pointer_velocity_tracker.h"
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "i_multimodal_input_connect.h"
#include "input_device_manager.h"
#include "i_input_windows_manager.h"
#include "ipc_skeleton.h"
#include "mmi_log.h"
#include "i_preference_manager.h"
#include "parameters.h"
#include "pipeline/rs_recording_canvas.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_helper.h"
#include "render/rs_pixel_map_util.h"
#include "scene_board_judgement.h"
#include "setting_datashare.h"
#include "util.h"
#include "dfx_hisysevent.h"
#include "timer_manager.h"
#include "surface.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_CURSOR
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "PointerDrawingManager"
#define FOCUS_COORDINATES(FOCUS_COORDINATES_, CHANGE) float FOCUS_COORDINATES_##CHANGE
#define CALCULATE_CANVAS_SIZE(CALCULATE_CANVAS_SIZE_, CHANGE) float CALCULATE_CANVAS_SIZE_##CHANGE

namespace OHOS {
namespace MMI {
namespace {
const std::string FOLD_SCREEN_FLAG = system::GetParameter("const.window.foldscreen.type", "");
const std::string IMAGE_POINTER_DEFAULT_PATH = "/system/etc/multimodalinput/mouse_icon/";
const std::string DefaultIconPath = IMAGE_POINTER_DEFAULT_PATH + "Default.svg";
const std::string CursorIconPath = IMAGE_POINTER_DEFAULT_PATH + "Cursor_Circle.png";
const std::string LoadingIconPath = IMAGE_POINTER_DEFAULT_PATH + "Loading.svg";
const std::string LoadingRightIconPath = IMAGE_POINTER_DEFAULT_PATH + "Loading_Right.svg";
const std::string POINTER_COLOR { "pointerColor" };
const std::string POINTER_SIZE { "pointerSize" };
const std::string MAGIC_POINTER_COLOR { "magicPointerColor" };
const std::string MAGIC_POINTER_SIZE { "magicPointerSize"};
const std::string POINTER_CURSOR_RENDER_RECEIVER_NAME { "PointerCursorReceiver" };
const std::vector<std::string> DEVICE_TYPES = {};
constexpr int32_t BASELINE_DENSITY { 160 };
constexpr int32_t CALCULATE_MIDDLE { 2 };
[[ maybe_unused ]] constexpr int32_t MAGIC_INDEPENDENT_PIXELS { 30 };
constexpr int32_t DEVICE_INDEPENDENT_PIXELS { 40 };
constexpr int32_t POINTER_WINDOW_INIT_SIZE { 64 };
constexpr int32_t DEFAULT_POINTER_SIZE { 1 };
constexpr int32_t MIN_POINTER_SIZE { 1 };
constexpr int32_t MAX_POINTER_SIZE { 7 };
constexpr int32_t DEFAULT_VALUE { -1 };
constexpr int32_t ANIMATION_DURATION { 500 };
constexpr int32_t DEFAULT_POINTER_STYLE { 0 };
constexpr int32_t CURSOR_CIRCLE_STYLE { 41 };
constexpr int32_t MOUSE_ICON_BAIS { 5 };
constexpr int32_t VISIBLE_LIST_MAX_SIZE { 100 };
[[ maybe_unused ]] constexpr int32_t WAIT_TIME_FOR_MAGIC_CURSOR { 6000 };
constexpr float ROTATION_ANGLE { 360.f };
constexpr float LOADING_CENTER_RATIO { 0.5f };
constexpr float RUNNING_X_RATIO { 0.3f };
constexpr float RUNNING_Y_RATIO { 0.675f };
constexpr float INCREASE_RATIO { 1.22f };
constexpr float ROTATION_ANGLE90 { 90.f };
constexpr int32_t MIN_POINTER_COLOR { 0x000000 };
constexpr int32_t MAX_POINTER_COLOR { 0x00ffffff };
constexpr int32_t MIN_CURSOR_SIZE { 64 };
constexpr uint32_t RGB_CHANNEL_BITS_LENGTH { 24 };
constexpr float MAX_ALPHA_VALUE { 255.f };
constexpr int32_t MOUSE_STYLE_OPT { 0 };
constexpr int32_t MAGIC_STYLE_OPT { 1 };
constexpr size_t RETRY_TIMES { 3 };
const std::string MOUSE_FILE_NAME { "mouse_settings.xml" };
bool g_isRsRemoteDied { false };
bool g_isHdiRemoteDied { false };
bool g_isReStartVsync { false };
constexpr uint64_t FOLD_SCREEN_ID_FULL { 0 };
constexpr uint64_t FOLD_SCREEN_ID_MAIN { 5 };
constexpr float IMAGE_PIXEL { 0.0f };
constexpr float CALCULATE_IMAGE_MIDDLE { 2.0f };
constexpr int32_t QUEUE_SIZE { 5 };
constexpr int32_t RETRY_COUNT { 2 };
constexpr int32_t DYNAMIC_ROTATION_ANGLE { 12 };
constexpr float CALCULATE_MOUSE_ICON_BAIS { 5.0f };
constexpr int32_t SYNC_FENCE_WAIT_TIME { 3000 };
float g_hardwareCanvasSize = { 512.0f };
float g_focalPoint = { 256.0f };
constexpr int32_t REPEAT_COOLING_TIME { 10000 };
constexpr int32_t REPEAT_ONCE { 1 };
constexpr int32_t ANGLE_90 { 90 };
constexpr int32_t ANGLE_360 { 360 };
constexpr int32_t MAX_CUSTOM_CURSOR_SIZE { 256 };
constexpr float MAX_CUSTOM_CURSOR_DIMENSION { 256.0f };
const int32_t ERROR_WINDOW_ID_PERMISSION_DENIED = 26500001;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
std::map<Rosen::ScreenId, sptr<Rosen::ScreenInfo>> g_screenSourceMode;
std::atomic<bool> g_hasMirrorScreen { false };
std::atomic<bool> g_hasExtendScreen { false };
std::atomic<bool> g_hasVirtualScreen { false };
std::mutex screenMtx;
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
} // namespace
} // namespace MMI
} // namespace OHOS

namespace OHOS {
namespace MMI {

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
void PointerDrawingManager::InitScreenInfo()
{
    Rosen::DMError retMode =
        Rosen::ScreenManagerLite::GetInstance().RegisterScreenModeChangeListener(new ScreenModeChangeListener());
    if (retMode != Rosen::DMError::DM_OK) {
        MMI_HILOGE("RegisterScreenModeChangeListener fail");
        return;
    }
    Rosen::DMError ret = Rosen::ScreenManagerLite::GetInstance().RegisterScreenListener(new ScreenListener());
    if (ret != Rosen::DMError::DM_OK) {
        MMI_HILOGE("RegisterScreenListener fail");
        return;
    }
}

void UpdateScreenModeChange()
{
    g_hasMirrorScreen.store(false);
    g_hasExtendScreen.store(false);
    g_hasVirtualScreen.store(false);
    for (auto iter = g_screenSourceMode.begin(); iter != g_screenSourceMode.end(); ++iter) {
        if (iter->second->GetType() == Rosen::ScreenType::REAL &&
            iter->second->GetSourceMode() == Rosen::ScreenSourceMode::SCREEN_MIRROR) {
            g_hasMirrorScreen.store(true);
        } else if (iter->second->GetSourceMode() == Rosen::ScreenSourceMode::SCREEN_EXTEND) {
            g_hasExtendScreen.store(true);
        } else if (iter->second->GetType() == Rosen::ScreenType::VIRTUAL) {
            g_hasVirtualScreen.store(true);
        } else {
            MMI_HILOGE("no screenType match");
        }
    }
}

void ScreenModeChangeListener::NotifyScreenModeChange(const std::vector<sptr<Rosen::ScreenInfo>> &screenInfos)
{
    std::lock_guard<std::mutex> guard(screenMtx);
    if (screenInfos.empty()) {
        return;
    }
    g_screenSourceMode.clear();
    for (auto screenInfo : screenInfos) {
        MMI_HILOGI("insert current screenId:%{public}ld", static_cast<uint64_t>(screenInfo->GetScreenId()));
        g_screenSourceMode.emplace(screenInfo->GetScreenId(), screenInfo);
    }
    UpdateScreenModeChange();
}

void ScreenListener::OnDisconnect(Rosen::ScreenId screenId)
{
    std::lock_guard<std::mutex> guard(screenMtx);
    auto it = g_screenSourceMode.find(screenId);
    if (it != g_screenSourceMode.end()) {
        g_screenSourceMode.erase(screenId);
        MMI_HILOGD("Clear current screenId:%{public}ld", static_cast<uint64_t>(screenId));
    }
    UpdateScreenModeChange();
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

static bool IsSingleDisplayFoldDevice()
{
    return (!FOLD_SCREEN_FLAG.empty() && FOLD_SCREEN_FLAG[0] == '1');
}

void RsRemoteDiedCallback()
{
    CALL_INFO_TRACE;
    g_isRsRemoteDied = true;
    g_isHdiRemoteDied = true;
    g_isReStartVsync = true;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MAGIC_CURSOR->RsRemoteDiedCallbackForMagicCursor();
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    IPointerDrawingManager::GetInstance()->DestroyPointerWindow();
}

void PointerDrawingManager::InitPointerCallback()
{
    MMI_HILOGI("Init RS Callback start");
    g_isRsRemoteDied = false;
    Rosen::OnRemoteDiedCallback callback = RsRemoteDiedCallback;
    Rosen::RSInterfaces::GetInstance().SetOnRemoteDiedCallback(callback);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor() && surfaceNode_ != nullptr) {
        surfaceNode_ = nullptr;
        MAGIC_CURSOR->RsRemoteInitCallbackForMagicCursor();
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

void PointerDrawingManager::DestroyPointerWindow()
{
    CALL_INFO_TRACE;
    CHKPV(delegateProxy_);
    delegateProxy_->OnPostSyncTask([this] {
        if (surfaceNode_ != nullptr) {
            MMI_HILOGI("Pointer window destroy start");
            g_isRsRemoteDied = false;
            surfaceNode_->DetachToDisplay(screenId_);
            surfaceNode_ = nullptr;
            Rosen::RSTransaction::FlushImplicitTransaction();
            MMI_HILOGI("Pointer window destroy success");
        }
        return RET_OK;
    });
}

static inline bool IsNum(const std::string &str)
{
    std::istringstream sin(str);
    double num;
    return (sin >> num) && sin.eof();
}

static float GetCanvasSize()
{
    auto ret = system::GetParameter("rosen.multimodalinput.pc.setcanvassize", "512.0");
    if (IsNum(ret)) {
        return g_hardwareCanvasSize;
    }
    return std::stoi(ret.c_str());
}

static float GetFocusCoordinates()
{
    auto ret = system::GetParameter("rosen.multimodalinput.pc.setfocuscoordinates", "256.0");
    if (IsNum(ret)) {
        return g_focalPoint;
    }
    return std::stoi(ret.c_str());
}

PointerDrawingManager::PointerDrawingManager()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MMI_HILOGI("magiccurosr InitStyle");
    hasMagicCursor_.name = "isMagicCursor";
    MAGIC_CURSOR->InitStyle();
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    InitStyle();
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    hardwareCursorPointerManager_ = std::make_shared<HardwareCursorPointerManager>();
    g_hardwareCanvasSize = GetCanvasSize();
    g_focalPoint = GetFocusCoordinates();
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

PointerDrawingManager::~PointerDrawingManager()
{
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    if ((renderThread_ != nullptr) && renderThread_->joinable()) {
        renderThread_->join();
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

PointerStyle PointerDrawingManager::GetLastMouseStyle()
{
    CALL_DEBUG_ENTER;
    return lastMouseStyle_;
}

float PointerDrawingManager::CalculateHardwareXOffset(ICON_TYPE iconType)
{
    int32_t width = imageWidth_;
    int32_t userIconHotSpotX = 0;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        width = cursorWidth_;
        userIconHotSpotX = userIconHotSpotX_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_E:
            return g_focalPoint;
        case ANGLE_S:
            return (g_focalPoint - (width / CALCULATE_IMAGE_MIDDLE));
        case ANGLE_W:
            return (g_focalPoint - width);
        case ANGLE_N:
            return (g_focalPoint - (width / CALCULATE_IMAGE_MIDDLE));
        case ANGLE_SE:
            return (g_focalPoint - width);
        case ANGLE_NE:
            return (g_focalPoint - width);
        case ANGLE_SW:
            return g_focalPoint;
        case ANGLE_NW:
            return g_focalPoint - userIconHotSpotX;
        case ANGLE_CENTER:
            return (g_focalPoint - (width / CALCULATE_IMAGE_MIDDLE));
        case ANGLE_NW_RIGHT:
            return g_focalPoint - CALCULATE_MOUSE_ICON_BAIS;
        default:
            MMI_HILOGW("No need calculate physicalX offset, iconType:%{public}d", iconType);
            return g_focalPoint;
    }
}

float PointerDrawingManager::CalculateHardwareYOffset(ICON_TYPE iconType)
{
    int32_t height = imageHeight_;
    int32_t userIconHotSpotY = 0;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        height = cursorHeight_;
        userIconHotSpotY = userIconHotSpotY_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_E:
            return (g_focalPoint - (height / CALCULATE_IMAGE_MIDDLE));
        case ANGLE_S:
            return g_focalPoint;
        case ANGLE_W:
            return (g_focalPoint - height);
        case ANGLE_N:
            return (g_focalPoint - height);
        case ANGLE_SE:
            return (g_focalPoint - height);
        case ANGLE_NE:
            return g_focalPoint;
        case ANGLE_SW:
            return (g_focalPoint - height);
        case ANGLE_NW:
            return g_focalPoint - userIconHotSpotY;
        case ANGLE_CENTER:
            return (g_focalPoint - (height / CALCULATE_IMAGE_MIDDLE));
        case ANGLE_NW_RIGHT:
            return g_focalPoint;
        default:
            MMI_HILOGW("No need calculate physicalY offset, iconType:%{public}d", iconType);
            return g_focalPoint;
    }
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
bool PointerDrawingManager::SetDynamicHardWareCursorLocation
    (int32_t physicalX, int32_t physicalY, MOUSE_ICON mouseStyle)
{
    CHKPF(hardwareCursorPointerManager_);
    CHKPF(surfaceNode_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    ICON_TYPE iconType = ICON_TYPE::ANGLE_NW;
    if (mouseStyle == MOUSE_ICON::LOADING) {
        iconType = ICON_TYPE::ANGLE_CENTER;
    } else {
        iconType = ICON_TYPE::ANGLE_NW;
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
    surfaceNode_->SetBounds((physicalX - CalculateHardwareXOffset(iconType)), (physicalY -
        CalculateHardwareYOffset(iconType)), g_hardwareCanvasSize, g_hardwareCanvasSize);
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    return true;
}

void PointerDrawingManager::PostTaskRSLocation(int32_t physicalX, int32_t physicalY,
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode)
{
    if (!(g_hasMirrorScreen.load() || g_hasVirtualScreen.load())) {
        return;
    }
    hardwareCanvasSize_ = g_hardwareCanvasSize;
    PostTask([this, physicalX, physicalY, surfaceNode]() -> void {
        CHKPV(surfaceNode);
        int64_t nodeId = surfaceNode->GetId();
        Rosen::RSInterfaces::GetInstance().SetHwcNodeBounds(nodeId,
            physicalX, physicalY, hardwareCanvasSize_, hardwareCanvasSize_);
        Rosen::RSTransaction::FlushImplicitTransaction();
    });
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

bool PointerDrawingManager::SetTraditionsHardWareCursorLocation(int32_t displayId, int32_t physicalX,
    int32_t physicalY, ICON_TYPE iconType)
{
    bool magicCursorSetBounds = false;
    if (UpdateSurfaceNodeBounds(physicalX, physicalY) == RET_OK) {
        magicCursorSetBounds = true;
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPF(hardwareCursorPointerManager_);
    CHKPF(surfaceNode_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported() && (hasLoadingPointerStyle_ || hasHardwareCursorAnimate_)) {
        if (cursorBuffers_[bufferId_] && cursorBuffers_[bufferId_]->GetVirAddr() &&
            hardwareCursorPointerManager_->SetPosition(
                (physicalX - CalculateHardwareXOffset(iconType)),
                (physicalY - CalculateHardwareYOffset(iconType)),
                cursorBuffers_[bufferId_]->GetBufferHandle()) != RET_OK) {
            MMI_HILOGE("Set hardware cursor position fail");
            return false;
        }
        return true;
    }
    if (!magicCursorSetBounds) {
        if (hardwareCursorPointerManager_->IsSupported()) {
            // Change the coordinates issued by RS to asynchronous,
            // without blocking the issuance of HardwareCursor coordinates.
            PostTaskRSLocation((physicalX - CalculateHardwareXOffset(iconType)), (physicalY -
                CalculateHardwareYOffset(iconType)), surfaceNode_);
        } else {
            surfaceNode_->SetBounds(physicalX, physicalY, surfaceNode_->GetStagingProperties().GetBounds().z_,
                surfaceNode_->GetStagingProperties().GetBounds().w_);
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
    }
#else
    if (!magicCursorSetBounds) {
        surfaceNode_->SetBounds(physicalX, physicalY,
            surfaceNode_->GetStagingProperties().GetBounds().z_, surfaceNode_->GetStagingProperties().GetBounds().w_);
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPF(hardwareCursorPointerManager_);
    hardwareCursorPointerManager_->SetTargetDevice(displayId);
    if (hardwareCursorPointerManager_->IsSupported()) {
        if (cursorBuffers_[bufferId_] && cursorBuffers_[bufferId_]->GetVirAddr() &&
            hardwareCursorPointerManager_->SetPosition(
                (physicalX - CalculateHardwareXOffset(iconType)),
                (physicalY - CalculateHardwareYOffset(iconType)),
                cursorBuffers_[bufferId_]->GetBufferHandle()) != RET_OK) {
            MMI_HILOGE("Set hardware cursor position error");
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    return true;
}

void PointerDrawingManager::ForceClearPointerVisiableStatus()
{
    MMI_HILOGI("force clear all pointer visiable status");
    pidInfos_.clear();
    UpdatePointerVisible();
}

void PointerDrawingManager::SetSurfaceNodeVisible(bool visible)
{
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    CHKPV(surfaceNode_);
    if (!hardwareCursorPointerManager_->IsSupported()) {
        surfaceNode_->SetVisible(visible);
    }
#else
    surfaceNode_->SetVisible(visible);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

int32_t PointerDrawingManager::DrawMovePointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
    PointerStyle pointerStyle, Direction direction)
{
    CHKPR(surfaceNode_, RET_ERR);
    MMI_HILOGD("Pointer window move success, pointerStyle id:%{public}d", pointerStyle.id);
    displayId_ = displayId;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    bool cursorEnlarged = MAGIC_POINTER_VELOCITY_TRACKER->GetCursorEnlargedStatus();
    if (cursorEnlarged) {
        MAGIC_POINTER_VELOCITY_TRACKER->SetLastPointerStyle(pointerStyle);
        MAGIC_POINTER_VELOCITY_TRACKER->SetDirection(direction);
        if (pointerStyle.id != MOUSE_ICON::DEFAULT && pointerStyle.id != MOUSE_ICON::CROSS) {
            pointerStyle.id = MOUSE_ICON::DEFAULT;
        }
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    UpdateBindDisplayId(displayId);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (lastMouseStyle_ == pointerStyle && !mouseIconUpdate_ && lastDirection_ == direction) {
        if (!SetTraditionsHardWareCursorLocation(displayId, physicalX, physicalY,
            ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay))) {
            return RET_ERR;
        }
        MMI_HILOGD("The lastpointerStyle is equal with pointerStyle, id:%{public}d, size:%{public}d",
            pointerStyle.id, pointerStyle.size);
        return RET_OK;
    }
    if (lastDirection_ != direction) {
        RotateDegree(direction);
        lastDirection_ = direction;
    }
    lastMouseStyle_ = pointerStyle;
    SetSurfaceNodeVisible(false);
    if (InitLayer(MOUSE_ICON(lastMouseStyle_.id)) != RET_OK) {
        mouseIconUpdate_ = false;
        MMI_HILOGE("Init layer failed");
        return RET_ERR;
    }
    SetSurfaceNodeVisible(true);
    if (!SetTraditionsHardWareCursorLocation(displayId, physicalX, physicalY,
        ICON_TYPE(mouseIcons_[MOUSE_ICON(lastMouseStyle_.id)].alignmentWay))) {
        MMI_HILOGE("Set traditions hardware cursor location error");
        return RET_ERR;
    }
    UpdatePointerVisible();
    mouseIconUpdate_ = false;
    MMI_HILOGD("Leave, display:%{public}d, physicalX:%{public}d, physicalY:%{public}d",
        displayId, physicalX, physicalY);
    return RET_OK;
}

int32_t PointerDrawingManager::UpdateSurfaceNodeBounds(int32_t physicalX, int32_t physicalY)
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        if (currentMouseStyle_.id == DEVELOPER_DEFINED_ICON) {
            surfaceNode_->SetBounds(physicalX, physicalY,
                canvasWidth_, canvasHeight_);
        } else {
            CHKPR(surfaceNode_, RET_ERR);
            surfaceNode_->SetBounds(physicalX, physicalY,
                imageWidth_, imageHeight_);
        }
        return RET_OK;
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    return RET_ERR;
}

void PointerDrawingManager::DrawMovePointer(int32_t displayId, int32_t physicalX, int32_t physicalY)
{
    CALL_DEBUG_ENTER;
    if (surfaceNode_ != nullptr) {
        if (!SetTraditionsHardWareCursorLocation(displayId, physicalX, physicalY,
            ICON_TYPE(mouseIcons_[MOUSE_ICON(lastMouseStyle_.id)].alignmentWay))) {
            return;
        }
        MMI_HILOGD("Move pointer, physicalX:%d, physicalY:%d", physicalX, physicalY);
    }
}

void PointerDrawingManager::SetHardwareCursorPosition(int32_t displayId, int32_t physicalX, int32_t physicalY,
    PointerStyle pointerStyle)
{
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    hardwareCursorPointerManager_->SetTargetDevice(displayId);
    if (hardwareCursorPointerManager_->IsSupported() && lastMouseStyle_.id != MOUSE_ICON::LOADING &&
            lastMouseStyle_.id != MOUSE_ICON::RUNNING) {
        float XOffset = CalculateHardwareXOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay));
        float YOffset = CalculateHardwareYOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay));
        if (cursorBuffers_[bufferId_] && cursorBuffers_[bufferId_]->GetVirAddr() &&
            hardwareCursorPointerManager_->SetPosition(
                (physicalX - XOffset), (physicalY - YOffset),
                cursorBuffers_[bufferId_]->GetBufferHandle()) != RET_OK) {
            MMI_HILOGE("Set hardware cursor position error");
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
    const PointerStyle pointerStyle, Direction direction)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("Display:%{public}d, physicalX:%{public}d, physicalY:%{public}d, pointerStyle:%{public}d",
        displayId, physicalX, physicalY, pointerStyle.id);
    FixCursorPosition(physicalX, physicalY);
    lastPhysicalX_ = physicalX;
    lastPhysicalY_ = physicalY;
    currentMouseStyle_ = pointerStyle;
    currentDirection_ = direction;
    if (pointerStyle.id == MOUSE_ICON::DEFAULT && mouseIcons_[MOUSE_ICON(pointerStyle.id)].iconPath == CursorIconPath) {
        AdjustMouseFocus(direction, ICON_TYPE(mouseIcons_[MOUSE_ICON(MOUSE_ICON::CURSOR_CIRCLE)].alignmentWay),
            physicalX, physicalY);
    } else {
        AdjustMouseFocus(direction, ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay),
            physicalX, physicalY);
    }
    // Log printing only occurs when the mouse style changes
    if (currentMouseStyle_.id != lastMouseStyle_.id) {
        MMI_HILOGD("MagicCursor AdjustMouseFocus:%{public}d",
            ICON_TYPE(mouseIcons_[MOUSE_ICON(pointerStyle.id)].alignmentWay));
    }
    if (DrawMovePointer(displayId, physicalX, physicalY, pointerStyle, direction) == RET_OK) {
        return;
    }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor() && currentMouseStyle_.id != DEVELOPER_DEFINED_ICON) {
        MMI_HILOGD("magicCursor DrawPointer enter CreatePointerWindow");
        MAGIC_CURSOR->CreatePointerWindow(displayId, physicalX, physicalY, direction, surfaceNode_);
    } else {
        CreatePointerWindow(displayId, physicalX, physicalY, direction);
    }
#else
    CreatePointerWindow(displayId, physicalX, physicalY, direction);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    CHKPV(surfaceNode_);
    UpdateMouseStyle();
    if (InitLayer(MOUSE_ICON(lastMouseStyle_.id)) != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return;
    }
    UpdatePointerVisible();
    SetHardwareCursorPosition(displayId, physicalX, physicalY, lastMouseStyle_);
    MMI_HILOGI("Leave, display:%{public}d, physicalX:%d, physicalY:%d", displayId, physicalX, physicalY);
}

void PointerDrawingManager::UpdateMouseStyle()
{
    CALL_DEBUG_ENTER;
    PointerStyle curPointerStyle;
    GetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
    if (curPointerStyle.id == CURSOR_CIRCLE_STYLE) {
        lastMouseStyle_.id = curPointerStyle.id;
        int ret = SetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Set pointer style failed");
        }
        return;
    }
}

int32_t PointerDrawingManager::SwitchPointerStyle()
{
    CALL_DEBUG_ENTER;
    int32_t size = GetPointerSize();
    if (size < MIN_POINTER_SIZE) {
        size = MIN_POINTER_SIZE;
    } else if (size > MAX_POINTER_SIZE) {
        size = MAX_POINTER_SIZE;
    }
    imageWidth_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    imageHeight_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    canvasWidth_ = (imageWidth_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    canvasHeight_ = (imageHeight_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MAGIC_CURSOR->SetPointerSize(imageWidth_, imageHeight_);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    Direction direction = DIRECTION0;
    int32_t physicalX = lastPhysicalX_;
    int32_t physicalY = lastPhysicalY_;
    AdjustMouseFocus(
        direction, ICON_TYPE(GetIconStyle(MOUSE_ICON(lastMouseStyle_.id)).alignmentWay), physicalX, physicalY);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        MAGIC_CURSOR->EnableCursorInversion();
        MAGIC_CURSOR->CreatePointerWindow(displayInfo_.id, physicalX, physicalY, direction, surfaceNode_);
    } else {
        MAGIC_CURSOR->DisableCursorInversion();
        CreatePointerWindow(displayInfo_.id, physicalX, physicalY, direction);
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
    if (ret != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return ret;
    }
    UpdatePointerVisible();
    SetHardwareCursorPosition(displayInfo_.id, physicalX, physicalY, lastMouseStyle_);
    return RET_OK;
}

void PointerDrawingManager::CreateMagicCursorChangeObserver()
{
    // Listening enabling cursor deformation and color inversion
    SettingObserver::UpdateFunc func = [](const std::string& key) {
        bool statusValue = false;
        auto ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).GetBoolValue(key, statusValue);
        if (ret != RET_OK) {
            MMI_HILOGE("Get value from setting date fail");
            return;
        }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        MAGIC_CURSOR->UpdateMagicCursorChangeState(statusValue);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    };
    std::string dynamicallyKey = "smartChange";
    sptr<SettingObserver> magicCursorChangeObserver = SettingDataShare::GetInstance(
        MULTIMODAL_INPUT_SERVICE_ID).CreateObserver(dynamicallyKey, func);
    ErrCode ret =
        SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).RegisterObserver(magicCursorChangeObserver);
    if (ret != ERR_OK) {
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        DfxHisysevent::ReportMagicCursorFault(dynamicallyKey, "Register setting observer failed");
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
        MMI_HILOGE("Register magic cursor change observer failed, ret:%{public}d", ret);
        magicCursorChangeObserver = nullptr;
    }
}

void PointerDrawingManager::UpdateStyleOptions()
{
    CALL_DEBUG_ENTER;
    PointerStyle curPointerStyle;
    WIN_MGR->GetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
    curPointerStyle.options = HasMagicCursor() ? MAGIC_STYLE_OPT : MOUSE_STYLE_OPT;
    int ret = WIN_MGR->SetPointerStyle(pid_, GLOBAL_WINDOW_ID, curPointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer style failed");
    }
}

void PointerDrawingManager::InitPointerObserver()
{
    CALL_INFO_TRACE;
    if (hasInitObserver_) {
        MMI_HILOGI("Settingdata observer has init");
        return;
    }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t ret = CreatePointerSwitchObserver(hasMagicCursor_);
    if (ret == RET_OK) {
        hasInitObserver_ = true;
        MMI_HILOGD("Create pointer switch observer success");
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

int32_t PointerDrawingManager::CreatePointerSwitchObserver(isMagicCursor& item)
{
    CALL_DEBUG_ENTER;
    SettingObserver::UpdateFunc updateFunc = [this, &item](const std::string& key) {
        bool statusValue = false;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        statusValue = true;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
        auto ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).GetBoolValue(key, statusValue);
        if (ret != RET_OK) {
            MMI_HILOGE("Get value from setting date fail");
            return;
        }
        bool tmp = item.isShow;
        item.isShow = statusValue;
        this->UpdateStyleOptions();
        if (item.isShow != tmp) {
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
            MAGIC_CURSOR->InitRenderThread([]() { IPointerDrawingManager::GetInstance()->SwitchPointerStyle(); });
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
            CHKPV(surfaceNode_);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
            MMI_HILOGD("Switch pointer style");
            int64_t nodeId = static_cast<int64_t>(this->surfaceNode_->GetId());
            if (nodeId != MAGIC_CURSOR->GetSurfaceNodeId(nodeId)) {
                surfaceNode_->DetachToDisplay(screenId_);
                Rosen::RSTransaction::FlushImplicitTransaction();
            }
            MAGIC_CURSOR->DetachDisplayNode();
            this->SwitchPointerStyle();
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
        }
    };
    sptr<SettingObserver> statusObserver =
        SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).CreateObserver(item.name, updateFunc);
    ErrCode ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).RegisterObserver(statusObserver);
    if (ret != ERR_OK) {
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        DfxHisysevent::ReportMagicCursorFault(item.name, "Register setting observer failed");
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
        MMI_HILOGE("Register setting observer failed, ret:%{public}d", ret);
        statusObserver = nullptr;
        return RET_ERR;
    }
    CreateMagicCursorChangeObserver();
    return RET_OK;
}

bool PointerDrawingManager::HasMagicCursor()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (!MAGIC_CURSOR->isExistDefaultStyle) {
        MMI_HILOGE("MagicCursor default icon file is not exist");
        return false;
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    return hasMagicCursor_.isShow;
}

void PointerDrawingManager::CreateDynamicCanvas()
{
    CALL_DEBUG_ENTER;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUE };
    if (dynamicBitmap_ == nullptr) {
        dynamicBitmap_ = std::make_shared<OHOS::Rosen::Drawing::Bitmap>();
    }
    g_hardwareCanvasSize = GetCanvasSize();
    dynamicBitmap_->Build(g_hardwareCanvasSize, g_hardwareCanvasSize, format);
    if (dynamicCanvas_ == nullptr) {
        dynamicCanvas_ = std::make_shared<OHOS::Rosen::Drawing::Canvas>();
    }
    dynamicCanvas_->Bind(*dynamicBitmap_);
}

int32_t PointerDrawingManager::ParsingDynamicImage(MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<OHOS::Media::PixelMap> pixelmap = nullptr;
    if (mouseStyle == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGD("Set mouseicon by userIcon_");
        auto userIconCopy = GetUserIconCopy();
        image_ = ExtractDrawingImage(userIconCopy);
    } else {
        if (mouseStyle == MOUSE_ICON::RUNNING) {
            pixelmap = DecodeImageToPixelMap(MOUSE_ICON::RUNNING_LEFT);
        } else {
            pixelmap = DecodeImageToPixelMap(mouseStyle);
        }
        CHKPR(pixelmap, RET_ERR);
        if (mouseStyle == MOUSE_ICON::RUNNING_RIGHT) {
            runningRightImage_ = ExtractDrawingImage(pixelmap);
            CHKPR(runningRightImage_, RET_ERR);
            return RET_OK;
        }
        image_ = ExtractDrawingImage(pixelmap);
        CHKPR(image_, RET_ERR);
    }
    return RET_OK;
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
void PointerDrawingManager::DrawTraditionsCursor(MOUSE_ICON mouseStyle)
{
    PostTask([this, mouseStyle]() -> void {
        this->DrawCursor(mouseStyle);
    });
}

int32_t PointerDrawingManager::InitVsync(MOUSE_ICON mouseStyle)
{
    if (ParsingDynamicImage(mouseStyle) != RET_OK) {
        MMI_HILOGE("Parsing mouseStyle fail");
        return RET_ERR;
    }
    if (ParsingDynamicImage(MOUSE_ICON::RUNNING_RIGHT) != RET_OK) {
        MMI_HILOGE("Parsing mouse icon fail");
        return RET_ERR;
    }
    if (mouseStyle == MOUSE_ICON::LOADING) {
        hasLoadingPointerStyle_ = true;
    } else if (mouseStyle == MOUSE_ICON::RUNNING) {
        hasHardwareCursorAnimate_ = true;
    }
    CreateDynamicCanvas();
    if (g_isReStartVsync) {
        isRenderRuning_.store(true);
        auto rsClient = std::static_pointer_cast<Rosen::RSRenderServiceClient>(
            Rosen::RSIRenderClient::CreateRenderServiceClient());
        CHKPR(rsClient, RET_ERR);
        receiver_ = rsClient->CreateVSyncReceiver(POINTER_CURSOR_RENDER_RECEIVER_NAME, handler_);
        if (receiver_ == nullptr || receiver_->Init() != VSYNC_ERROR_OK) {
            MMI_HILOGE("Receiver init failed");
            return RET_ERR;
        }
        g_isReStartVsync = false;
    }
    return RequestNextVSync();
}

sptr<OHOS::SurfaceBuffer> PointerDrawingManager::RetryGetSurfaceBuffer(sptr<OHOS::Surface> layer)
{
    sptr<OHOS::SurfaceBuffer> buffer;
    if (hardwareCursorPointerManager_->IsSupported()) {
        for (size_t i = 0; i < RETRY_TIMES; i++) {
            buffer = GetSurfaceBuffer(layer);
            if (buffer != nullptr && buffer->GetVirAddr() != nullptr) {
                return buffer;
            }
        }
    }
    return buffer;
}

#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

int32_t PointerDrawingManager::InitLayer(const MOUSE_ICON mouseStyle)
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor() && mouseStyle != MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGD("magiccursor enter MAGIC_CURSOR->Initlayer");
        return MAGIC_CURSOR->InitLayer(mouseStyle);
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPR(hardwareCursorPointerManager_, RET_ERR);
    hasLoadingPointerStyle_ = false;
    hasHardwareCursorAnimate_ = false;
    if (hardwareCursorPointerManager_->IsSupported()) {
        if ((mouseStyle == MOUSE_ICON::LOADING) || (mouseStyle == MOUSE_ICON::RUNNING)) {
            return InitVsync(mouseStyle);
        } else {
            GetCanvasSize();
            GetFocusCoordinates();
            hardwareCanvasSize_ = g_hardwareCanvasSize;
            // Change the drawing to asynchronous, and when obtaining the surfaceBuffer fails,
            // repeatedly obtain the surfaceBuffer.
            DrawHardwareCursor(mouseStyle);
            DrawTraditionsCursor(mouseStyle);
            return RET_OK;
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    return DrawCursor(mouseStyle);
}

int32_t PointerDrawingManager::DrawCursor(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPR(surfaceNode_, RET_ERR);
    DrawLoadingPointerStyle(mouseStyle);
    DrawRunningPointerAnimate(mouseStyle);
    sptr<OHOS::Surface> layer = GetLayer();
    if (layer == nullptr) {
        MMI_HILOGE("Init layer is failed, Layer is nullptr");
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGE("Pointer window destroy success");
        return RET_ERR;
    }
    if (!isInit_) {
        layer->SetQueueSize(QUEUE_SIZE);
        isInit_ = true;
    }
    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
        buffer = RetryGetSurfaceBuffer(layer);
        if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
            return RET_ERR;
        }
#else
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGE("Pointer window destroy success");
        return RET_ERR;
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    CHKPR(addr, RET_ERR);
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), mouseStyle);
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, DEFAULT_VALUE, flushConfig);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        MMI_HILOGE("Init layer failed, FlushBuffer return ret:%{public}s", SurfaceErrorStr(ret).c_str());
        layer->CancelBuffer(buffer);
        return RET_ERR;
    }
    MMI_HILOGD("Init layer success");
    return RET_OK;
}

void PointerDrawingManager::DrawLoadingPointerStyle(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPV(surfaceNode_);
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported()) {
        return;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    Rosen::RSAnimationTimingProtocol protocol;
    if (mouseStyle != MOUSE_ICON::LOADING &&
        (mouseStyle != MOUSE_ICON::DEFAULT ||
            mouseIcons_[mouseStyle].iconPath != (IMAGE_POINTER_DEFAULT_PATH + "Loading.svg"))) {
        protocol.SetDuration(0);
        Rosen::RSNode::Animate(
            protocol,
            Rosen::RSAnimationTimingCurve::LINEAR,
            [this]() {
                if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
                    RotateDegree(DIRECTION0);
                    return;
                }
                RotateDegree(currentDirection_);
            });
        MMI_HILOGE("current pointer is not loading");
        Rosen::RSTransaction::FlushImplicitTransaction();
        return;
    }
    float ratio = imageWidth_ * 1.0 / canvasWidth_;
    surfaceNode_->SetPivot({LOADING_CENTER_RATIO * ratio, LOADING_CENTER_RATIO * ratio});
    protocol.SetDuration(ANIMATION_DURATION);
    protocol.SetRepeatCount(DEFAULT_VALUE);

    // create property animation
    Rosen::RSNode::Animate(
        protocol,
        Rosen::RSAnimationTimingCurve::LINEAR,
        [this]() { surfaceNode_->SetRotation(ROTATION_ANGLE); });

    Rosen::RSTransaction::FlushImplicitTransaction();
}

std::shared_ptr<Rosen::Drawing::ColorSpace> PointerDrawingManager::ConvertToColorSpace(
    Media::ColorSpace colorSpace)
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

Rosen::Drawing::ColorType PointerDrawingManager::PixelFormatToColorType(Media::PixelFormat pixelFormat)
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

Rosen::Drawing::AlphaType PointerDrawingManager::AlphaTypeToAlphaType(Media::AlphaType alphaType)
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

static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx != nullptr) {
        delete ctx;
    }
}

std::shared_ptr<Rosen::Drawing::Image> PointerDrawingManager::ExtractDrawingImage(
    std::shared_ptr<Media::PixelMap> pixelMap)
{
    CHKPP(pixelMap);
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    Rosen::Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        PixelFormatToColorType(imageInfo.pixelFormat),
        AlphaTypeToAlphaType(imageInfo.alphaType),
        ConvertToColorSpace(imageInfo.colorSpace) };
    Rosen::Drawing::Pixmap imagePixmap(drawingImageInfo,
        reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowBytes());
    PixelMapReleaseContext* releaseContext = new (std::nothrow) PixelMapReleaseContext(pixelMap);
    CHKPP(releaseContext);
    auto image = Rosen::Drawing::Image::MakeFromRaster(imagePixmap, PixelMapReleaseProc, releaseContext);
    if (image == nullptr) {
        MMI_HILOGE("ExtractDrawingImage image fail");
        delete releaseContext;
    }
    return image;
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
void PointerDrawingManager::PostTask(Rosen::RSTaskMessage::RSTask task)
{
    CHKPV(hardwareCursorPointerManager_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (handler_ != nullptr) {
        handler_->PostTask(task);
    }
}

void PointerDrawingManager::DoHardwareCursorDraw()
{
    CHKPV(hardwareCursorPointerManager_);
    CHKPV(dynamicCanvas_);
    CHKPV(dynamicBitmap_);
    dynamicCanvas_->Save();
    dynamicCanvas_->Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    FOCUS_COORDINATES(FOCUS_COORDINATES_, CHANGE) = GetFocusCoordinates();
    CALCULATE_CANVAS_SIZE(CALCULATE_CANVAS_SIZE_, CHANGE) = GetCanvasSize();
    if (hardwareCursorPointerManager_->IsSupported() && hasLoadingPointerStyle_) {
        dynamicCanvas_->Rotate(DYNAMIC_ROTATION_ANGLE * currentFrame_, FOCUS_COORDINATES_CHANGE,
            FOCUS_COORDINATES_CHANGE);
        currentFrame_++;
        if (currentFrame_ == frameCount_) {
            currentFrame_ = 0;
        }
    }
    DrawDynamicImage(*dynamicCanvas_, MOUSE_ICON(lastMouseStyle_.id));
    if (hardwareCursorPointerManager_->IsSupported() && hasHardwareCursorAnimate_) {
        dynamicCanvas_->Rotate(DYNAMIC_ROTATION_ANGLE * currentFrame_, (FOCUS_COORDINATES_CHANGE +
            (imageWidth_ * RUNNING_X_RATIO)), (FOCUS_COORDINATES_CHANGE + (imageHeight_ * RUNNING_Y_RATIO)));
        DrawDynamicImage(*dynamicCanvas_, MOUSE_ICON::RUNNING_RIGHT);
        currentFrame_++;
        if (currentFrame_ == frameCount_) {
            currentFrame_ = 0;
        }
    }
    dynamicCanvas_->Restore();
    static constexpr uint32_t stride = 4;
    uint32_t addrSize =
        static_cast<uint32_t>(this->buffer_->GetWidth()) * static_cast<uint32_t>(this->buffer_->GetHeight()) * stride;
    CHKPV(addr_);
    errno_t ret = memcpy_s(*addr_, addrSize, dynamicBitmap_->GetPixels(), addrSize);
    if (ret != EOK) {
        MMI_HILOGE("Memcpy data is error, ret:%{public}d", ret);
    }
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    auto canvasSize = static_cast<int32_t>(CALCULATE_CANVAS_SIZE_CHANGE);
    PrepareBuffer(canvasSize, canvasSize);
    auto addr = static_cast<uint8_t *>(cursorBuffers_[bufferId_]->GetVirAddr());
    CHKPV(addr);
    ret = memcpy_s(addr, addrSize, dynamicBitmap_->GetPixels(), addrSize);
    if (ret != EOK) {
        MMI_HILOGE("Memcpy data is error, ret:%{public}d", ret);
        return;
    }
#endif //OHOS_BUILD_ENABLE_HARDWARE_CURSOR

int32_t PointerDrawingManager::FlushBuffer()
{
    CHKPR(buffer_, RET_ERR);
    CHKPR(layer_, RET_ERR);
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer_->GetWidth(),
            .h = buffer_->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer_->FlushBuffer(buffer_, DEFAULT_VALUE, flushConfig);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        MMI_HILOGE("Init layer failed, FlushBuffer return ret:%{public}s", SurfaceErrorStr(ret).c_str());
        layer_->CancelBuffer(buffer_);
    }
    return ret;
}

int32_t PointerDrawingManager::GetSurfaceInformation()
{
    CHKPR(surfaceNode_, RET_ERR);
    if (currentMouseStyle_.id != MOUSE_ICON::RUNNING && currentMouseStyle_.id != MOUSE_ICON::LOADING) {
        MMI_HILOGE("Current mouse style is not equal to last mouse style");
        return RET_ERR;
    }
    layer_ = GetLayer();
    CHKPR(layer_, RET_ERR);
    buffer_ = GetSurfaceBuffer(layer_);
    if (buffer_ == nullptr || buffer_->GetVirAddr() == nullptr) {
        MMI_HILOGE("Init layer is failed, buffer or virAddr is nullptr");
        return RET_ERR;
    }
    addr_ = std::make_shared<uint8_t *>(static_cast<uint8_t *>(buffer_->GetVirAddr()));
    CHKPR(addr_, RET_ERR);
    return RET_OK;
}

void PointerDrawingManager::OnVsync(uint64_t timestamp)
{
    if (currentMouseStyle_.id != MOUSE_ICON::RUNNING && currentMouseStyle_.id != MOUSE_ICON::LOADING) {
        MMI_HILOGE("Current mouse style is not equal to last mouse style");
        return;
    }
    PostTask([this]() -> void {
        if (currentMouseStyle_.id != MOUSE_ICON::RUNNING && currentMouseStyle_.id != MOUSE_ICON::LOADING) {
            MMI_HILOGE("Current post task mouse style is not equal to last mouse style");
            return;
        }
        if (GetSurfaceInformation() != RET_OK) {
            MMI_HILOGE("OnVsync Get surface information fail");
            return;
        }
        DoHardwareCursorDraw();
        FlushBuffer();
        UpdatePointerVisible();
        mouseIconUpdate_ = false;
        if (!SetDynamicHardWareCursorLocation(lastPhysicalX_, lastPhysicalY_, MOUSE_ICON(lastMouseStyle_.id))) {
            MMI_HILOGE("OnVsync set dynamic hardware cursor location error");
            return;
        }
    });
    RequestNextVSync();
}

int32_t PointerDrawingManager::RequestNextVSync()
{
    if (handler_ != nullptr) {
        Rosen::VSyncReceiver::FrameCallback fcb = {
            .userData_ = this,
            .callback_ = [this] (uint64_t timestamp, void*) {
                return this->OnVsync(timestamp);
            },
        };
        if (receiver_ != nullptr) {
            receiver_->RequestNextVSync(fcb);
            return RET_OK;
        }
    }
    return RET_ERR;
}

void PointerDrawingManager::RenderThreadLoop()
{
    isRenderRuning_.store(true);
    runner_ = AppExecFwk::EventRunner::Create(false);
    CHKPV(runner_);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    CHKPV(handler_);
    auto rsClient = std::static_pointer_cast<Rosen::RSRenderServiceClient>(
        Rosen::RSIRenderClient::CreateRenderServiceClient());
    CHKPV(rsClient);
    receiver_ = rsClient->CreateVSyncReceiver(POINTER_CURSOR_RENDER_RECEIVER_NAME, handler_);
    if (receiver_ == nullptr || receiver_->Init() != VSYNC_ERROR_OK) {
        MMI_HILOGE("Receiver init failed");
        return;
    }
    if (runner_ != nullptr) {
        MMI_HILOGI("Runner is run");
        runner_->Run();
    }
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

void PointerDrawingManager::DrawRunningPointerAnimate(const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported()) {
        return;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(surfaceNode_);
    CHKPV(canvasNode_);
    if (mouseStyle != MOUSE_ICON::RUNNING && (mouseStyle != MOUSE_ICON::DEFAULT ||
            mouseIcons_[mouseStyle].iconPath != (IMAGE_POINTER_DEFAULT_PATH + "Loading_Left.svg"))) {
        if (canvasNode_ != nullptr) {
            Rosen::RSAnimationTimingProtocol protocol;
            protocol.SetDuration(0);
            Rosen::RSNode::Animate(
                protocol,
                Rosen::RSAnimationTimingCurve::LINEAR,
                [this]() { canvasNode_->SetRotation(0); });
            Rosen::RSTransaction::FlushImplicitTransaction();
            canvasNode_->SetVisible(false);
        }
        MMI_HILOGE("current pointer is not running");
        return;
    }
    canvasNode_->SetVisible(true);
    float ratio = imageWidth_ * 1.0 / canvasWidth_;
    canvasNode_->SetPivot({RUNNING_X_RATIO * ratio, RUNNING_Y_RATIO * ratio});
    std::shared_ptr<OHOS::Media::PixelMap> pixelmap =
        DecodeImageToPixelMap(MOUSE_ICON::RUNNING_RIGHT);
    CHKPV(pixelmap);
    MMI_HILOGD("Set mouseicon to OHOS system");

#ifndef USE_ROSEN_DRAWING
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));
    canvas->DrawPixelMap(pixelmap, 0, 0, SkSamplingOptions(), nullptr);
#else
    Rosen::Drawing::Brush brush;
    Rosen::Drawing::Rect src = Rosen::Drawing::Rect(0, 0, pixelmap->GetWidth(), pixelmap->GetHeight());
    Rosen::Drawing::Rect dst = Rosen::Drawing::Rect(src);
    auto canvas =
        static_cast<Rosen::ExtendRecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));
    canvas->AttachBrush(brush);
    canvas->DrawPixelMapRect(pixelmap, src, dst, Rosen::Drawing::SamplingOptions());
    canvas->DetachBrush();
#endif // USE_ROSEN_DRAWING

    canvasNode_->FinishRecording();

    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    protocol.SetRepeatCount(DEFAULT_VALUE);

    // create property animation
    Rosen::RSNode::Animate(
        protocol,
        Rosen::RSAnimationTimingCurve::LINEAR,
        [this]() { canvasNode_->SetRotation(ROTATION_ANGLE); });

    Rosen::RSTransaction::FlushImplicitTransaction();
}

void PointerDrawingManager::AdjustMouseFocus(Direction direction, ICON_TYPE iconType,
    int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    switch (direction) {
        case DIRECTION0: {
            AdjustMouseFocusByDirection0(iconType, physicalX, physicalY);
            break;
        }
        case DIRECTION90: {
            AdjustMouseFocusByDirection90(iconType, physicalX, physicalY);
            break;
        }
        case DIRECTION180: {
            AdjustMouseFocusByDirection180(iconType, physicalX, physicalY);
            break;
        }
        case DIRECTION270: {
            AdjustMouseFocusByDirection270(iconType, physicalX, physicalY);
            break;
        }
        default: {
            MMI_HILOGW("direction is invalid,direction:%{public}d", direction);
            break;
        }
    }
}

void PointerDrawingManager::AdjustMouseFocusByDirection0(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    int32_t height = imageHeight_;
    int32_t width = imageWidth_;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        height = cursorHeight_;
        width = cursorWidth_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_SW: {
            physicalY -= height;
            break;
        }
        case ANGLE_CENTER: {
            physicalX -= width / CALCULATE_MIDDLE;
            physicalY -= height / CALCULATE_MIDDLE;
            break;
        }
        case ANGLE_NW_RIGHT: {
            physicalX -= MOUSE_ICON_BAIS;
            [[fallthrough]];
        }
        case ANGLE_NW: {
            auto userIconCopy = GetUserIconCopy();
            if (userIconCopy != nullptr && currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
                physicalX -= userIconHotSpotX_;
                physicalY -= userIconHotSpotY_;
            }
            break;
        }
        default: {
            MMI_HILOGW("No need adjust mouse focus,iconType:%{public}d", iconType);
            break;
        }
    }
}

void PointerDrawingManager::AdjustMouseFocusByDirection90(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    int32_t height = imageHeight_;
    int32_t width = imageWidth_;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        height = cursorHeight_;
        width = cursorWidth_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_SW: {
            physicalY += height;
            break;
        }
        case ANGLE_CENTER: {
            physicalX -= width / CALCULATE_MIDDLE;
            physicalY += height / CALCULATE_MIDDLE;
            break;
        }
        case ANGLE_NW_RIGHT: {
            physicalX -= MOUSE_ICON_BAIS;
            [[fallthrough]];
        }
        case ANGLE_NW: {
            auto userIconCopy = GetUserIconCopy();
            if (userIconCopy != nullptr && currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
                physicalX -= userIconHotSpotX_;
                physicalY += userIconHotSpotY_;
            }
            break;
        }
        default: {
            MMI_HILOGW("No need adjust mouse focus,iconType:%{public}d", iconType);
            break;
        }
    }
}

void PointerDrawingManager::AdjustMouseFocusByDirection180(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    int32_t height = imageHeight_;
    int32_t width = imageWidth_;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        height = cursorHeight_;
        width = cursorWidth_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_SW: {
            physicalY += height;
            break;
        }
        case ANGLE_CENTER: {
            physicalX += width / CALCULATE_MIDDLE;
            physicalY += height / CALCULATE_MIDDLE;
            break;
        }
        case ANGLE_NW_RIGHT: {
            physicalX += MOUSE_ICON_BAIS;
            [[fallthrough]];
        }
        case ANGLE_NW: {
            auto userIconCopy = GetUserIconCopy();
            if (userIconCopy != nullptr && currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
                physicalX += userIconHotSpotX_;
                physicalY += userIconHotSpotY_;
            }
            break;
        }
        default: {
            MMI_HILOGW("No need adjust mouse focus,iconType:%{public}d", iconType);
            break;
        }
    }
}

void PointerDrawingManager::AdjustMouseFocusByDirection270(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    int32_t height = imageHeight_;
    int32_t width = imageWidth_;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported() &&
        currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        height = cursorHeight_;
        width = cursorWidth_;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    switch (iconType) {
        case ANGLE_SW: {
            physicalY -= height;
            break;
        }
        case ANGLE_CENTER: {
            physicalX += width / CALCULATE_MIDDLE;
            physicalY -= height / CALCULATE_MIDDLE;
            break;
        }
        case ANGLE_NW_RIGHT: {
            physicalX += MOUSE_ICON_BAIS;
            [[fallthrough]];
        }
        case ANGLE_NW: {
            auto userIconCopy = GetUserIconCopy();
            if (userIconCopy != nullptr && currentMouseStyle_.id == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
                physicalX += userIconHotSpotX_;
                physicalY -= userIconHotSpotY_;
            }
            break;
        }
        default: {
            MMI_HILOGW("No need adjust mouse focus,iconType:%{public}d", iconType);
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
        MMI_HILOGI("state:%{public}s", state ? "true" : "false");
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
    Direction direction = static_cast<Direction>((
        ((displayInfo_.direction - displayInfo_.displayDirection) * ANGLE_90 + ANGLE_360) % ANGLE_360) / ANGLE_90);
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (hardwareCursorPointerManager_->IsSupported()) {
        direction = displayInfo_.direction;
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        direction = displayInfo_.direction;
    }
    if (direction == DIRECTION0 || direction == DIRECTION180) {
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

void PointerDrawingManager::AttachToDisplay()
{
    CALL_DEBUG_ENTER;
    CHKPV(surfaceNode_);
    if (IsSingleDisplayFoldDevice() && (WIN_MGR->GetDisplayMode() == DisplayMode::MAIN)
        && (screenId_ == FOLD_SCREEN_ID_FULL)) {
        screenId_ = FOLD_SCREEN_ID_MAIN;
    }
    MMI_HILOGI("screenId_:%{public}" PRIu64"", screenId_);
    surfaceNode_->AttachToDisplay(screenId_);
}

void PointerDrawingManager::CreateCanvasNode()
{
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    CHKPV(surfaceNode_);
    if (!hardwareCursorPointerManager_->IsSupported()) {
        canvasNode_ = Rosen::RSCanvasNode::Create();
        CHKPV(canvasNode_);
        canvasNode_->SetBounds(0, 0, canvasWidth_, canvasHeight_);
        canvasNode_->SetFrame(0, 0, canvasWidth_, canvasHeight_);
#ifndef USE_ROSEN_DRAWING
        canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
        canvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif // USE_ROSEN_DRAWING
        canvasNode_->SetCornerRadius(1);
        canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
        canvasNode_->SetRotation(0);
        surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    }
#else
    canvasNode_ = Rosen::RSCanvasNode::Create();
    CHKPV(canvasNode_);
    canvasNode_->SetBounds(0, 0, canvasWidth_, canvasHeight_);
    canvasNode_->SetFrame(0, 0, canvasWidth_, canvasHeight_);
#ifndef USE_ROSEN_DRAWING
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    canvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif // USE_ROSEN_DRAWING
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);
    surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

void PointerDrawingManager::CreatePointerWindow(int32_t displayId, int32_t physicalX, int32_t physicalY,
    Direction direction)
{
    CALL_DEBUG_ENTER;
    CALL_INFO_TRACE;
    BytraceAdapter::StartRsSurfaceNode(displayId);
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "pointer window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    CHKPV(surfaceNode_);
    surfaceNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    hardwareCursorPointerManager_->SetTargetDevice(displayId);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
        surfaceNode_->SetFrameGravity(Rosen::Gravity::TOP_LEFT);
        g_isHdiRemoteDied = false;
        CALCULATE_CANVAS_SIZE(CALCULATE_CANVAS_SIZE_, CHANGE) = GetCanvasSize();
        surfaceNode_->SetBounds(
            (physicalX - CalculateHardwareXOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(
                lastMouseStyle_.id)].alignmentWay))),
            (physicalY - CalculateHardwareYOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(
                lastMouseStyle_.id)].alignmentWay))),
            CALCULATE_CANVAS_SIZE_CHANGE,
            CALCULATE_CANVAS_SIZE_CHANGE);
    } else {
        surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
        surfaceNode_->SetBounds(physicalX, physicalY, canvasWidth_, canvasHeight_);
    }
#else
    surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetBounds(physicalX, physicalY, canvasWidth_, canvasHeight_);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
#ifndef USE_ROSEN_DRAWING
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
#else
    surfaceNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
#endif

    screenId_ = static_cast<uint64_t>(displayId);
    std::cout << "ScreenId: " << screenId_ << std::endl;
    AttachToDisplay();
    lastDisplayId_ = displayId;
    RotateDegree(direction);
    lastDirection_ = direction;
    CreateCanvasNode();
    Rosen::RSTransaction::FlushImplicitTransaction();
    BytraceAdapter::StopRsSurfaceNode();
}

sptr<OHOS::Surface> PointerDrawingManager::GetLayer()
{
    CALL_DEBUG_ENTER;
    CHKPP(surfaceNode_);
    return surfaceNode_->GetSurface();
}

sptr<OHOS::SurfaceBuffer> PointerDrawingManager::GetSurfaceBuffer(sptr<OHOS::Surface> layer)
{
    CALL_DEBUG_ENTER;
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = -1;
    int32_t width = 0;
    int32_t height = 0;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPP(hardwareCursorPointerManager_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
        CALCULATE_CANVAS_SIZE(CALCULATE_CANVAS_SIZE_, CHANGE) = GetCanvasSize();
        auto canvasSize = static_cast<int32_t>(CALCULATE_CANVAS_SIZE_CHANGE);
        width = canvasSize;
        height = canvasSize;
    } else {
        width = canvasWidth_;
        height = canvasHeight_;
    }
#else
    width = canvasWidth_;
    height = canvasHeight_;
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    OHOS::BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 150,
    };

    OHOS::SurfaceError ret = layer->RequestBuffer(buffer, releaseFence, config);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        MMI_HILOGE("Request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    sptr<OHOS::SyncFence> tempFence = new OHOS::SyncFence(releaseFence);
    if (tempFence != nullptr && (tempFence->Wait(SYNC_FENCE_WAIT_TIME) < 0)) {
        MMI_HILOGE("Failed to create surface, this buffer is not available");
    }
    return buffer;
}

void PointerDrawingManager::DrawDynamicImage(OHOS::Rosen::Drawing::Canvas &canvas, MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    CHKPV(image_);
    OHOS::Rosen::Drawing::Brush brush;
    brush.SetColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    canvas.DrawBackground(brush);
    ICON_TYPE iconType = ICON_TYPE::ANGLE_NW;
    if (mouseStyle == MOUSE_ICON::LOADING) {
        iconType = ICON_TYPE::ANGLE_CENTER;
    } else {
        iconType = ICON_TYPE::ANGLE_NW;
    }
    float physicalXOffset = CalculateHardwareXOffset(iconType);
    float physicalYOffset = CalculateHardwareYOffset(iconType);
    if (mouseStyle == MOUSE_ICON::RUNNING_RIGHT) {
        CHKPV(runningRightImage_);
        canvas.DrawImage(*runningRightImage_, physicalXOffset, physicalYOffset, Rosen::Drawing::SamplingOptions());
    } else {
        canvas.DrawImage(*image_, physicalXOffset, physicalYOffset, Rosen::Drawing::SamplingOptions());
    }
}

void PointerDrawingManager::DrawImage(OHOS::Rosen::Drawing::Canvas &canvas, MOUSE_ICON mouseStyle)
{
    MMI_HILOGI("Draw mouse icon of style(%{public}d)", static_cast<int32_t>(mouseStyle));
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    std::shared_ptr<Rosen::Drawing::Image> image = nullptr;
    std::shared_ptr<OHOS::Media::PixelMap> pixelmap = nullptr;
    if (mouseStyle == MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGD("Set mouseicon by userIcon_");
        auto userIconCopy = GetUserIconCopy();
        image = ExtractDrawingImage(userIconCopy);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        SetPixelMap(userIconCopy);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    } else {
        if (mouseStyle == MOUSE_ICON::RUNNING) {
            pixelmap = DecodeImageToPixelMap(MOUSE_ICON::RUNNING_LEFT);
        } else {
            pixelmap = DecodeImageToPixelMap(mouseStyle);
        }
        CHKPV(pixelmap);
        image = ExtractDrawingImage(pixelmap);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        if ((mouseStyle == MOUSE_ICON::DEFAULT) || (mouseStyle == MOUSE_ICON::CURSOR_CIRCLE)) {
            SetPixelMap(pixelmap);
        }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    }
    CHKPV(image);
    OHOS::Rosen::Drawing::Brush brush;
    brush.SetColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    canvas.DrawBackground(brush);
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (hardwareCursorPointerManager_->IsSupported()) {
        float physicalXOffset = CalculateHardwareXOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(
            mouseStyle)].alignmentWay));
        float physicalYOffset = CalculateHardwareYOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(
            mouseStyle)].alignmentWay));
        canvas.DrawImage(*image, physicalXOffset, physicalYOffset, Rosen::Drawing::SamplingOptions());
    } else {
        canvas.DrawImage(*image, IMAGE_PIXEL, IMAGE_PIXEL, Rosen::Drawing::SamplingOptions());
    }
#else
    canvas.DrawImage(*image, IMAGE_PIXEL, IMAGE_PIXEL, Rosen::Drawing::SamplingOptions());
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    MMI_HILOGD("Canvas draw image, success");
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
void PointerDrawingManager::SetPixelMap(std::shared_ptr<OHOS::Media::PixelMap> pixelMap)
{
    MMI_HILOGI("Set pointer snapshot");
    pixelMap_ = pixelMap;
}

int32_t PointerDrawingManager::GetPointerSnapshot(void *pixelMapPtr)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<Media::PixelMap> *newPixelMapPtr = static_cast<std::shared_ptr<Media::PixelMap> *>(pixelMapPtr);
    MMI_HILOGI("Get pointer snapshot");
    *newPixelMapPtr = pixelMap_;
    if (HasMagicCursor()) {
        MMI_HILOGE("magic pixelmap");
        *newPixelMapPtr = MAGIC_CURSOR->GetPixelMap();
    }
    CHKPR(*newPixelMapPtr, ERROR_NULL_POINTER);
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR

void PointerDrawingManager::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    CHKPV(addr);
    currentFrame_ = 0;
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    DrawImage(canvas, mouseStyle);
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
        MMI_HILOGD("Set mouseicon by userIcon_");
        auto userIconCopy = GetUserIconCopy();
        CHKPV(userIconCopy);
        OHOS::Rosen::RSPixelMapUtil::DrawPixelMap(canvas, *userIconCopy, 0, 0);
    } else {
        std::shared_ptr<OHOS::Media::PixelMap> pixelmap;
        if (mouseStyle == MOUSE_ICON::RUNNING) {
            pixelmap = DecodeImageToPixelMap(MOUSE_ICON::RUNNING_LEFT);
        } else {
            pixelmap = DecodeImageToPixelMap(mouseStyle);
        }
        CHKPV(pixelmap);
        MMI_HILOGD("Set mouseicon to OHOS system");
        OHOS::Rosen::RSPixelMapUtil::DrawPixelMap(canvas, *pixelmap, 0, 0);
    }
}

int32_t PointerDrawingManager::SetCustomCursor(void* pixelMap, int32_t pid, int32_t windowId, int32_t focusX,
    int32_t focusY)
{
    CALL_DEBUG_ENTER;
    CHKPR(pixelMap, RET_ERR);
    if (pid == -1) {
        MMI_HILOGE("The pid is invalid");
        return RET_ERR;
    }
    if (windowId < 0) {
        MMI_HILOGE("The windowId is invalid, windowId:%{public}d", windowId);
        return RET_ERR;
    }
    if (WIN_MGR->CheckWindowIdPermissionByPid(windowId, pid) != RET_OK) {
        MMI_HILOGE("The windowId not in right pid");
        return RET_ERR;
    }
    int32_t ret = UpdateCursorProperty(pixelMap, focusX, focusY);
    if (ret != RET_OK) {
        MMI_HILOGE("UpdateCursorProperty is failed");
        return ret;
    }
    mouseIconUpdate_ = true;
    PointerStyle style;
    style.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    lastMouseStyle_ = style;

    ret = SetPointerStyle(pid, windowId, style);
    if (ret == RET_ERR) {
        MMI_HILOGE("SetPointerStyle is failed");
    }
    MMI_HILOGD("style.id:%{public}d, userIconHotSpotX_:%{public}d, userIconHotSpotY_:%{public}d",
        style.id, userIconHotSpotX_, userIconHotSpotY_);
    return ret;
}

int32_t PointerDrawingManager::UpdateCursorProperty(void* pixelMap, const int32_t &focusX, const int32_t &focusY)
{
    CHKPR(pixelMap, RET_ERR);
    Media::PixelMap* newPixelMap = static_cast<Media::PixelMap*>(pixelMap);
    CHKPR(newPixelMap, RET_ERR);
    Media::ImageInfo imageInfo;
    newPixelMap->GetImageInfo(imageInfo);
    int32_t cursorSize = GetPointerSize();
    cursorWidth_ =
        pow(INCREASE_RATIO, cursorSize - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    cursorHeight_ =
        pow(INCREASE_RATIO, cursorSize - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    cursorWidth_ = cursorWidth_ < MIN_CURSOR_SIZE ? MIN_CURSOR_SIZE : cursorWidth_;
    cursorHeight_ = cursorHeight_ < MIN_CURSOR_SIZE ? MIN_CURSOR_SIZE : cursorHeight_;
    float xAxis = (float)cursorWidth_ / (float)imageInfo.size.width;
    float yAxis = (float)cursorHeight_ / (float)imageInfo.size.height;
    newPixelMap->scale(xAxis, yAxis, Media::AntiAliasingOption::LOW);
    {
        std::lock_guard<std::mutex> guard(mtx_);
        userIcon_.reset(newPixelMap);
    }
    userIconHotSpotX_ = static_cast<int32_t>((float)focusX * xAxis);
    userIconHotSpotY_ = static_cast<int32_t>((float)focusY * yAxis);
    MMI_HILOGI("cursorWidth:%{public}d, cursorHeight:%{public}d, imageWidth:%{public}d, imageHeight:%{public}d,"
        "focusX:%{public}d, focuxY:%{public}d, xAxis:%{public}f, yAxis:%{public}f, userIconHotSpotX_:%{public}d,"
        "userIconHotSpotY_:%{public}d", cursorWidth_, cursorHeight_, imageInfo.size.width, imageInfo.size.height,
        focusX, focusY, xAxis, yAxis, userIconHotSpotX_, userIconHotSpotY_);
    return RET_OK;
}

int32_t PointerDrawingManager::SetMouseIcon(int32_t pid, int32_t windowId, void* pixelMap)
    __attribute__((no_sanitize("cfi")))
{
    CALL_DEBUG_ENTER;
    if (pid == -1) {
        MMI_HILOGE("pid is invalid return -1");
        return RET_ERR;
    }
    CHKPR(pixelMap, RET_ERR);
    if (windowId < 0) {
        MMI_HILOGE("Get invalid windowId, %{public}d", windowId);
        return RET_ERR;
    }
    if (WIN_MGR->CheckWindowIdPermissionByPid(windowId, pid) != RET_OK) {
        MMI_HILOGE("windowId not in right pid");
        return RET_ERR;
    }
    OHOS::Media::PixelMap* pixelMapPtr = static_cast<OHOS::Media::PixelMap*>(pixelMap);
    {
        std::lock_guard<std::mutex> guard(mtx_);
        userIcon_.reset(pixelMapPtr);
    }
    
    mouseIconUpdate_ = true;
    PointerStyle style;
    style.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    int32_t ret = SetPointerStyle(pid, windowId, style);
    if (ret == RET_ERR) {
        MMI_HILOGE("SetPointerStyle return RET_ERR here");
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
    if (WIN_MGR->CheckWindowIdPermissionByPid(windowId, pid) != RET_OK) {
        MMI_HILOGE("windowId not in right pid");
        return RET_ERR;
    }
    auto userIconCopy = GetUserIconCopy();
    if (hotSpotX < 0 || hotSpotY < 0 || userIconCopy == nullptr) {
        MMI_HILOGE("invalid value");
        return RET_ERR;
    }
    PointerStyle pointerStyle;
    WIN_MGR->GetPointerStyle(pid, windowId, pointerStyle);
    if (pointerStyle.id != MOUSE_ICON::DEVELOPER_DEFINED_ICON) {
        MMI_HILOGE("Get pointer style failed, pid %{public}d, pointerStyle %{public}d", pid, pointerStyle.id);
        return RET_ERR;
    }
    userIconHotSpotX_ = hotSpotX;
    userIconHotSpotY_ = hotSpotY;
    return RET_OK;
}

void ChangeSvgCursorColor(std::string& str, int32_t color)
{
    std::string targetColor = IntToHexRGB(color);
    StringReplace(str, "#000000", targetColor);
    if (color == MAX_POINTER_COLOR) {
        // stroke=\"#FFFFFF" fill="#000000" stroke-linejoin="round" transform="xxx"
        std::regex re("(<path.*)(stroke=\"#[a-fA-F0-9]{6}\")(.*path>)");
        str = std::regex_replace(str, re, "$1stroke=\"#000000\"$3");
    }
}

std::shared_ptr<OHOS::Media::PixelMap> PointerDrawingManager::LoadCursorSvgWithColor(MOUSE_ICON type, int32_t color)
{
    CALL_DEBUG_ENTER;
    std::string svgContent;
    std::string imagePath = mouseIcons_[type].iconPath;
    if (!ReadFile(imagePath, svgContent)) {
        MMI_HILOGE("read file failed");
        return nullptr;
    }
    const bool isPartColor = (type == CURSOR_COPY) || (type == CURSOR_FORBID) || (type == HELP);
    if (isPartColor) {
        ChangeSvgCursorColor(svgContent, color);
    }

    OHOS::Media::SourceOptions opts;
    uint32_t ret = 0;
    std::unique_ptr<std::istream>  isp(std::make_unique<std::istringstream>(svgContent));
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(std::move(isp), opts, ret);
    if (!imageSource || ret != ERR_OK) {
        MMI_HILOGE("Get image source failed, ret:%{public}d", ret);
    }
    CHKPP(imageSource);
    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = imageWidth_,
        .height = imageHeight_
    };
    if (!isPartColor) {
        decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = color};
        if (color == MAX_POINTER_COLOR) {
            decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MIN_POINTER_COLOR};
        } else {
            decodeOpts.SVGOpts.strokeColor = {.isValidColor = true, .color = MAX_POINTER_COLOR};
        }
    }

    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    CHKPL(pixelMap);
    return pixelMap;
}
std::shared_ptr<OHOS::Media::PixelMap> PointerDrawingManager::DecodeImageToPixelMap(MOUSE_ICON type)
{
    CALL_DEBUG_ENTER;
    for (auto item = mousePixelMap_.begin(); item != mousePixelMap_.end(); ++item) {
        if (item->first != type) {
            continue;
        }
        if (item->second.imageWidth != imageWidth_ || item->second.imageHeight != imageHeight_
            || item->second.pointerColor != GetPointerColor()) {
            if (!UpdateLoadingAndLoadingRightPixelMap()) {
                MMI_HILOGI("Update Loading And Loading Right Pointer success");
            }
            return mousePixelMap_[type].pixelMap;
        } else {
            return item->second.pixelMap;
        }
    }
    
    return LoadCursorSvgWithColor(type, GetPointerColor());
}

void PointerDrawingManager::GetPreferenceKey(std::string &name)
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        if (name == POINTER_COLOR) {
            name = MAGIC_POINTER_COLOR;
        } else if (name == POINTER_SIZE) {
            name = MAGIC_POINTER_SIZE;
        }
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

int32_t PointerDrawingManager::UpdateLoadingAndLoadingRightPixelMap()
{
    for (auto iter = mousePixelMap_.begin(); iter != mousePixelMap_.end();) {
        int32_t pointerColor = GetPointerColor();
        std::shared_ptr<OHOS::Media::PixelMap> pixelMap = LoadCursorSvgWithColor(iter->first, pointerColor);
        CHKPR(pixelMap, RET_ERR);
        iter->second.pixelMap = pixelMap;
        iter->second.imageWidth = imageWidth_;
        iter->second.imageHeight = imageHeight_;
        iter->second.pointerColor = pointerColor;
        int32_t width = pixelMap->GetWidth();
        int32_t height = pixelMap->GetHeight();
        MMI_HILOGI("Pixelmap width:%{public}d, height:%{public}d, %{public}d update success",
            width, height, iter->first);
        ++iter;
    }
    return RET_OK;
}

int32_t PointerDrawingManager::SetPointerColor(int32_t color)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("PointerColor:%{public}x", color);
    // ARGB从表面看比RGB多了个A，也是一种色彩模式，是在RGB的基础上添加了Alpha（透明度）通道。
    // 透明度也是以0到255表示的，所以也是总共有256级，透明是0，不透明是255。
    // 这个color每8位代表一个通道值，分别是alpha和rgb，总共32位。
    color = static_cast<int32_t>(static_cast<uint32_t>(color) & static_cast<uint32_t>(MAX_POINTER_COLOR));
    std::string name = POINTER_COLOR;
    GetPreferenceKey(name);
    int32_t ret = PREFERENCES_MGR->SetIntValue(name, MOUSE_FILE_NAME, color);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer color failed, color:%{public}d", color);
        return ret;
    }
    MMI_HILOGD("Set pointer color successfully, color:%{public}d", color);
    if (!WIN_MGR->GetExtraData().drawCursor) {
        if (surfaceNode_ != nullptr) {
            float alphaRatio = (static_cast<uint32_t>(color) >> RGB_CHANNEL_BITS_LENGTH) / MAX_ALPHA_VALUE;
            if (alphaRatio > 1) {
                MMI_HILOGW("Invalid alphaRatio:%{public}f", alphaRatio);
            } else {
                surfaceNode_->SetAlpha(1 - alphaRatio);
            }
        }
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        if (HasMagicCursor()) {
            ret = MAGIC_CURSOR->SetPointerColor(color);
        } else {
            ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
        }
#else
        ret = InitLayer(MOUSE_ICON(lastMouseStyle_.id));
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
        if (ret != RET_OK) {
            MMI_HILOGE("Init layer failed");
            return RET_ERR;
        }
    }
    UpdatePointerVisible();
    SetHardwareCursorPosition(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, lastMouseStyle_);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerColor()
{
    CALL_DEBUG_ENTER;
    std::string name = POINTER_COLOR;
    GetPreferenceKey(name);
    int32_t pointerColor = PREFERENCES_MGR->GetIntValue(name, DEFAULT_VALUE);
    tempPointerColor_ = pointerColor;
    if (pointerColor == DEFAULT_VALUE) {
        pointerColor = MIN_POINTER_COLOR;
    }
    MMI_HILOGD("Get pointer color successfully, pointerColor:%{public}d", pointerColor);
    return pointerColor;
}

void PointerDrawingManager::UpdateDisplayInfo(const DisplayInfo &displayInfo)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    hardwareCursorPointerManager_->SetTargetDevice(displayInfo.id);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    hasDisplay_ = true;
    displayInfo_ = displayInfo;
    int32_t size = GetPointerSize();
    imageWidth_ = pow(INCREASE_RATIO, size - 1) * displayInfo.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    imageHeight_ = pow(INCREASE_RATIO, size - 1) * displayInfo.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    canvasWidth_ = (imageWidth_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    canvasHeight_ = (imageHeight_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MAGIC_CURSOR->SetDisplayInfo(displayInfo);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

int32_t PointerDrawingManager::GetIndependentPixels()
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        return MAGIC_INDEPENDENT_PIXELS;
    } else {
        return DEVICE_INDEPENDENT_PIXELS;
    }
#else
    return DEVICE_INDEPENDENT_PIXELS;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

int32_t PointerDrawingManager::SetPointerSize(int32_t size)
{
    CALL_DEBUG_ENTER;
    if (size < MIN_POINTER_SIZE) {
        size = MIN_POINTER_SIZE;
    } else if (size > MAX_POINTER_SIZE) {
        size = MAX_POINTER_SIZE;
    }
    std::string name = POINTER_SIZE;
    GetPreferenceKey(name);
    int32_t ret = PREFERENCES_MGR->SetIntValue(name, MOUSE_FILE_NAME, size);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer size failed, code:%{public}d", ret);
        return ret;
    }

    CHKPR(surfaceNode_, RET_OK);
    imageWidth_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    imageHeight_ = pow(INCREASE_RATIO, size - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    canvasWidth_ = (imageWidth_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    canvasHeight_ = (imageHeight_ / POINTER_WINDOW_INIT_SIZE + 1) * POINTER_WINDOW_INIT_SIZE;
    int32_t physicalX = lastPhysicalX_;
    int32_t physicalY = lastPhysicalY_;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MAGIC_CURSOR->SetPointerSize(imageWidth_, imageHeight_);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    Direction direction = static_cast<Direction>((
        ((displayInfo_.direction - displayInfo_.displayDirection) * ANGLE_90 + ANGLE_360) % ANGLE_360) / ANGLE_90);
    auto& iconPath = GetMouseIconPath();
    AdjustMouseFocus(direction, ICON_TYPE(iconPath.at(MOUSE_ICON(lastMouseStyle_.id)).alignmentWay),
        physicalX, physicalY);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        MAGIC_CURSOR->CreatePointerWindow(displayInfo_.id, physicalX, physicalY, direction, surfaceNode_);
    } else {
        CreatePointerWindow(displayInfo_.id, physicalX, physicalY, direction);
    }
#else
    CreatePointerWindow(displayInfo_.id, physicalX, physicalY, direction);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    if (InitLayer(MOUSE_ICON(lastMouseStyle_.id)) != RET_OK) {
        MMI_HILOGE("Init layer failed");
        return RET_ERR;
    }
    UpdatePointerVisible();
    SetHardwareCursorPosition(displayInfo_.id, physicalX, physicalY, lastMouseStyle_);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerSize()
{
    CALL_DEBUG_ENTER;
    std::string name = POINTER_SIZE;
    GetPreferenceKey(name);
    int32_t pointerSize = PREFERENCES_MGR->GetIntValue(name, DEFAULT_POINTER_SIZE);
    MMI_HILOGD("Get pointer size successfully, pointerSize:%{public}d", pointerSize);
    return pointerSize;
}

int32_t PointerDrawingManager::GetCursorSurfaceId(uint64_t &surfaceId)
{
    surfaceId = (surfaceNode_ != nullptr ? surfaceNode_->GetId() : Rosen::INVALID_NODEID);
    MMI_HILOGI("CursorSurfaceId:%{public}" PRIu64, surfaceId);
    return RET_OK;
}

void PointerDrawingManager::OnDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
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
#ifndef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
        surfaceNode_ = nullptr;
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
    }
    MMI_HILOGD("displayId_:%{public}d, displayWidth_:%{public}d, displayHeight_:%{public}d",
        displayInfo_.id, displayInfo_.width, displayInfo_.height);
}

void PointerDrawingManager::OnWindowInfo(const WinInfo &info)
{
    CALL_DEBUG_ENTER;
    if (pid_ != info.windowPid) {
        windowId_ = info.windowId;
        pid_ = info.windowPid;
        UpdatePointerVisible();
    }
}

void PointerDrawingManager::UpdatePointerDevice(bool hasPointerDevice, bool isPointerVisible,
    bool isHotPlug)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("hasPointerDevice:%{public}s, isPointerVisible:%{public}s",
        hasPointerDevice ? "true" : "false", isPointerVisible? "true" : "false");
    hasPointerDevice_ = hasPointerDevice;
    if (hasPointerDevice_) {
        bool pointerVisible = isPointerVisible;
        if (!isHotPlug) {
            pointerVisible = (pointerVisible && IsPointerVisible());
        }
        SetPointerVisible(getpid(), pointerVisible, 0, false);
    } else {
        DeletePointerVisible(getpid());
    }
    DrawManager();
    if (!hasPointerDevice_ && surfaceNode_ != nullptr) {
        MMI_HILOGD("Pointer window destroy start");
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
        MMI_HILOGD("Pointer window destroy success");
    }
}

void PointerDrawingManager::DrawManager()
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor() && lastDrawPointerStyle_.id != currentMouseStyle_.id
        && (lastDrawPointerStyle_.id == DEVELOPER_DEFINED_ICON
        || currentMouseStyle_.id == DEVELOPER_DEFINED_ICON)) {
        if (surfaceNode_ != nullptr) {
            surfaceNode_->DetachToDisplay(screenId_);
            surfaceNode_ = nullptr;
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
    }
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    if (hasDisplay_ && hasPointerDevice_ && surfaceNode_ == nullptr) {
        MMI_HILOGD("Draw pointer begin");
        PointerStyle pointerStyle;
        WIN_MGR->GetPointerStyle(pid_, windowId_, pointerStyle);
        MMI_HILOGD("Get pid %{public}d with pointerStyle %{public}d", pid_, pointerStyle.id);
        Direction direction = static_cast<Direction>((
            ((displayInfo_.direction - displayInfo_.displayDirection) * ANGLE_90 + ANGLE_360) % ANGLE_360) / ANGLE_90);
        lastDrawPointerStyle_ = pointerStyle;
        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayInfo_.id, displayInfo_.width / CALCULATE_MIDDLE, displayInfo_.height / CALCULATE_MIDDLE,
                pointerStyle, direction);
            MMI_HILOGD("Draw manager, mouseStyle:%{public}d, last physical is initial value", pointerStyle.id);
            return;
        }
        DrawPointer(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, pointerStyle, direction);
        MMI_HILOGD("Draw manager, mouseStyle:%{public}d", pointerStyle.id);
        return;
    }
}

void PointerDrawingManager::InitLoadingAndLoadingRightPixelMap()
{
    auto pointerColor = GetPointerColor();
    for (auto iter = mousePixelMap_.begin(); iter != mousePixelMap_.end(); ++iter) {
        std::shared_ptr<OHOS::Media::PixelMap> pixelMap = LoadCursorSvgWithColor(iter->first, pointerColor);
        CHKPV(pixelMap);
        iter->second.pixelMap = pixelMap;
        iter->second.imageWidth = imageWidth_;
        iter->second.imageHeight = imageHeight_;
        iter->second.pointerColor = pointerColor;
        MMI_HILOGI("%{public}s init success", mouseIcons_[iter->first].iconPath.data());
    }
}

bool PointerDrawingManager::Init()
{
    CALL_DEBUG_ENTER;
    INPUT_DEV_MGR->Attach(shared_from_this());
    pidInfos_.clear();
    hapPidInfos_.clear();
    mousePixelMap_[MOUSE_ICON::LOADING];
    mousePixelMap_[MOUSE_ICON::RUNNING];
    InitLoadingAndLoadingRightPixelMap();
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
        MMI_HILOGI("Pointer window show success, mouseDisplayState_:%{public}s",
            mouseDisplayState_ ? "true" : "false");
    } else {
        if (!WIN_MGR->GetExtraData().drawCursor) {
            surfaceNode_->SetVisible(false);
            MMI_HILOGI("Pointer window hide success, mouseDisplayState_:%{public}s",
                mouseDisplayState_ ? "true" : "false");
        }
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

bool PointerDrawingManager::IsPointerVisible()
{
    CALL_DEBUG_ENTER;
    if (!pidInfos_.empty()) {
        auto info = pidInfos_.back();
        if (!info.visible) {
            MMI_HILOGI("High priority visible property:%{public}zu.%{public}d-visible:%{public}s",
                pidInfos_.size(), info.pid, info.visible?"true":"false");
            return info.visible;
        }
    }
    if (!hapPidInfos_.empty()) {
        for (auto& item : hapPidInfos_) {
            if (item.pid == pid_) {
                MMI_HILOGI("Visible pid:%{public}d-visible:%{public}s",
                    item.pid, item.visible ? "true" : "false");
                return item.visible;
            }
        }
        if (!(INPUT_DEV_MGR->HasPointerDevice() || WIN_MGR->IsMouseSimulate()) || pid_ == 0) {
            auto info = hapPidInfos_.back();
            MMI_HILOGI("Only hap visible pid:%{public}d-visible:%{public}s",
                info.pid, info.visible ? "true" : "false");
            return info.visible;
        }
    }
    if (pidInfos_.empty()) {
        MMI_HILOGD("Visible property is true");
        return true;
    }
    auto info = pidInfos_.back();
    MMI_HILOGI("Visible property:%{public}zu.%{public}d-visible:%{public}s",
        pidInfos_.size(), info.pid, info.visible ? "true" : "false");
    return info.visible;
}

void PointerDrawingManager::DeletePointerVisible(int32_t pid)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("g_isRsRemoteDied:%{public}d", g_isRsRemoteDied ? 1 : 0);
    if (g_isRsRemoteDied && surfaceNode_ != nullptr) {
        g_isRsRemoteDied = false;
        surfaceNode_->DetachToDisplay(screenId_);
        surfaceNode_ = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
    if (pidInfos_.empty()) {
        return;
    }
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

bool PointerDrawingManager::GetPointerVisible(int32_t pid)
{
    bool ret = true;
    int32_t count = 0;
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            count++;
            ret = it->visible;
            break;
        }
    }
    if (count == 0 && !hapPidInfos_.empty()) {
        for (auto& item : hapPidInfos_) {
            if (item.pid == pid_) {
                MMI_HILOGI("Visible pid:%{public}d-visible:%{public}s",
                    item.pid, item.visible ? "true" : "false");
                count++;
                ret = item.visible;
                break;
            }
        }
    }
    return ret;
}

void PointerDrawingManager::OnSessionLost(int32_t pid)
{
    for (auto it = hapPidInfos_.begin(); it != hapPidInfos_.end(); ++it) {
        if (it->pid == pid) {
            hapPidInfos_.erase(it);
            break;
        }
    }
}

int32_t PointerDrawingManager::SetPointerVisible(int32_t pid, bool visible, int32_t priority, bool isHap)
{
    MMI_HILOGI("pid:%{public}d,visible:%{public}s,priority:%{public}d,isHap:%{public}s", pid,
        visible ? "true" : "false", priority, isHap ? "true" : "false");
    if (isHap) {
        for (auto it = hapPidInfos_.begin(); it != hapPidInfos_.end(); ++it) {
            if (it->pid == pid) {
                hapPidInfos_.erase(it);
                break;
            }
        }
        PidInfo info = { .pid = pid, .visible = visible };
        hapPidInfos_.push_back(info);
        if (hapPidInfos_.size() > VISIBLE_LIST_MAX_SIZE) {
            hapPidInfos_.pop_front();
        }
        UpdatePointerVisible();
        return RET_OK;
    }
    if (WIN_MGR->GetExtraData().appended && visible && priority == 0) {
        MMI_HILOGE("current is drag state, can not set pointer visible");
        return RET_ERR;
    }
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    PidInfo info = { .pid = pid, .visible = visible };
    pidInfos_.push_back(info);
    if (pidInfos_.size() > VISIBLE_LIST_MAX_SIZE) {
        pidInfos_.pop_front();
    }
    if (!WIN_MGR->HasMouseHideFlag() || INPUT_DEV_MGR->HasPointerDevice() || INPUT_DEV_MGR->HasVirtualPointerDevice()) {
        UpdatePointerVisible();
    }
    return RET_OK;
}

void PointerDrawingManager::SetPointerLocation(int32_t x, int32_t y)
{
    CALL_DEBUG_ENTER;
    FixCursorPosition(x, y);
    lastPhysicalX_ = x;
    lastPhysicalY_ = y;
    MMI_HILOGD("Pointer window move, x:%{public}d, y:%{public}d", lastPhysicalX_, lastPhysicalY_);
    CHKPV(surfaceNode_);
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPV(hardwareCursorPointerManager_);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
        if (!SetTraditionsHardWareCursorLocation(displayId_, x, y,
            ICON_TYPE(mouseIcons_[MOUSE_ICON(lastMouseStyle_.id)].alignmentWay))) {
            MMI_HILOGE("Set hardware cursor position fail");
            return;
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    MMI_HILOGD("Pointer window move success");
}

int32_t PointerDrawingManager::UpdateDefaultPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle,
    bool isUiExtension)
{
    if (windowId != GLOBAL_WINDOW_ID) {
        MMI_HILOGD("No need to change the default icon style");
        return RET_OK;
    }
    PointerStyle style;
    WIN_MGR->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style, isUiExtension);
    if (pointerStyle.id != style.id) {
        auto iconPath = GetMouseIconPath();
        auto it = iconPath.find(MOUSE_ICON(MOUSE_ICON::DEFAULT));
        if (it == iconPath.end()) {
            MMI_HILOGE("Cannot find the default style");
            return RET_ERR;
        }
        std::string newIconPath;
        if (pointerStyle.id == MOUSE_ICON::DEFAULT) {
            newIconPath = DefaultIconPath;
        } else {
            newIconPath = iconPath.at(MOUSE_ICON(pointerStyle.id)).iconPath;
        }
        MMI_HILOGD("Default path has changed from %{public}s to %{public}s",
            it->second.iconPath.c_str(), newIconPath.c_str());
        UpdateIconPath(MOUSE_ICON(MOUSE_ICON::DEFAULT), newIconPath);
    }
    lastMouseStyle_ = style;
    return RET_OK;
}

const std::map<MOUSE_ICON, IconStyle>& PointerDrawingManager::GetMouseIconPath()
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        MMI_HILOGD("Magiccurosr get magic mouse map");
        return MAGIC_CURSOR->magicMouseIcons_;
    } else {
        MMI_HILOGD("Magiccurosr get mouse icon, HasMagicCursor is false");
        return mouseIcons_;
    }
#else
    return mouseIcons_;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

IconStyle PointerDrawingManager::GetIconStyle(const MOUSE_ICON mouseStyle)
{
    std::map<MOUSE_ICON, IconStyle> mouseIcons = GetMouseIcons();
    auto iter = mouseIcons.find(mouseStyle);
    if (iter == mouseIcons.end()) {
        MMI_HILOGE("Cannot find the mouseStyle:%{public}d", static_cast<int32_t>(mouseStyle));
        return IconStyle();
    }
    return iter->second;
}

std::map<MOUSE_ICON, IconStyle>& PointerDrawingManager::GetMouseIcons()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (HasMagicCursor()) {
        MMI_HILOGD("Magiccurosr get magic mouse map");
        return MAGIC_CURSOR->magicMouseIcons_;
    } else {
        MMI_HILOGD("Magiccurosr get mouse icon, HasMagicCursor is false");
        return mouseIcons_;
    }
#else
    return mouseIcons_;
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
}

void PointerDrawingManager::UpdateIconPath(const MOUSE_ICON mouseStyle, std::string iconPath)
{
    auto iter = mouseIcons_.find(mouseStyle);
    if (iter == mouseIcons_.end()) {
        MMI_HILOGE("Cannot find the mouseStyle:%{public}d", static_cast<int32_t>(mouseStyle));
        return;
    }
    iter->second.iconPath = iconPath;
}

int32_t PointerDrawingManager::SetPointerStylePreference(PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    std::string name = "pointerStyle";
    int32_t ret = PREFERENCES_MGR->SetIntValue(name, MOUSE_FILE_NAME, pointerStyle.id);
    if (ret == RET_OK) {
        MMI_HILOGE("Set pointer style successfully, style:%{public}d", pointerStyle.id);
    }
    return RET_OK;
}

bool PointerDrawingManager::CheckPointerStyleParam(int32_t windowId, PointerStyle pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (windowId < -1) {
        return false;
    }
    if ((pointerStyle.id < MOUSE_ICON::DEFAULT && pointerStyle.id != MOUSE_ICON::DEVELOPER_DEFINED_ICON) ||
        pointerStyle.id > MOUSE_ICON::RUNNING_RIGHT) {
        return false;
    }
    return true;
}

int32_t PointerDrawingManager::SetPointerStyle(int32_t pid, int32_t windowId, PointerStyle pointerStyle,
    bool isUiExtension)
{
    CALL_DEBUG_ENTER;
    if (!CheckPointerStyleParam(windowId, pointerStyle)) {
        MMI_HILOGE("PointerStyle param is invalid");
        return RET_ERR;
    }
    if (windowId == GLOBAL_WINDOW_ID) {
        int32_t ret = SetPointerStylePreference(pointerStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Set style preference is failed, ret:%{public}d", ret);
            return RET_ERR;
        }
    }
    auto& iconPath = GetMouseIconPath();
    if (iconPath.find(MOUSE_ICON(pointerStyle.id)) == iconPath.end()) {
        MMI_HILOGE("The param pointerStyle is invalid");
        return RET_ERR;
    }
    if (UpdateDefaultPointerStyle(pid, windowId, pointerStyle) != RET_OK) {
        MMI_HILOGE("Update default pointer iconPath failed");
        return RET_ERR;
    }
    if (WIN_MGR->SetPointerStyle(pid, windowId, pointerStyle, isUiExtension) != RET_OK) {
        MMI_HILOGE("Set pointer style failed");
        return RET_ERR;
    }
    if (!INPUT_DEV_MGR->HasPointerDevice() && !INPUT_DEV_MGR->HasVirtualPointerDevice()) {
        MMI_HILOGD("The pointer device is not exist");
        return RET_OK;
    }
    if (!WIN_MGR->IsNeedRefreshLayer(windowId)) {
        MMI_HILOGD("Not need refresh layer, window type:%{public}d, pointer style:%{public}d",
            windowId, pointerStyle.id);
        return RET_OK;
    }
    if (windowId != GLOBAL_WINDOW_ID && (pointerStyle.id == MOUSE_ICON::DEFAULT &&
        iconPath.at(MOUSE_ICON(pointerStyle.id)).iconPath != DefaultIconPath)) {
        PointerStyle style;
        WIN_MGR->GetPointerStyle(pid, GLOBAL_WINDOW_ID, style);
        pointerStyle = style;
    }
    if (windowId == windowId_ || windowId == GLOBAL_WINDOW_ID) {
        // Draw mouse style only when the current window is the top-level window
        if (!WIN_MGR->SelectPointerChangeArea(windowId, lastPhysicalX_ + displayInfo_.x,
            lastPhysicalY_ + displayInfo_.y)) {
            if (!WIN_MGR->GetExtraData().drawCursor) {
                DrawPointerStyle(pointerStyle);
            }
        } else {
            MMI_HILOGW("skip the pointerstyle");
        }
    } else {
        MMI_HILOGW("set windowid:%{public}d, top windowid:%{public}d, dont draw pointer", windowId, windowId_);
    }
    MMI_HILOGI("Window id:%{public}d set pointer style:%{public}d success", windowId, pointerStyle.id);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerStyle(int32_t pid, int32_t windowId, PointerStyle &pointerStyle,
    bool isUiExtension)
{
    CALL_DEBUG_ENTER;
    if (windowId == GLOBAL_WINDOW_ID) {
        std::string name = POINTER_COLOR;
        pointerStyle.color = PREFERENCES_MGR->GetIntValue(name, DEFAULT_VALUE);
        name = POINTER_SIZE;
        pointerStyle.size = PREFERENCES_MGR->GetIntValue(name, DEFAULT_POINTER_SIZE);
        name = "pointerStyle";
        int32_t style = PREFERENCES_MGR->GetIntValue(name, DEFAULT_POINTER_STYLE);
        MMI_HILOGD("Get pointer style successfully, pointerStyle:%{public}d", style);
        if (style == CURSOR_CIRCLE_STYLE) {
            pointerStyle.id = style;
            return RET_OK;
        }
    }
    WIN_MGR->GetPointerStyle(pid, windowId, pointerStyle, isUiExtension);
    MMI_HILOGD("Window id:%{public}d get pointer style:%{public}d success", windowId, pointerStyle.id);
    return RET_OK;
}

int32_t PointerDrawingManager::ClearWindowPointerStyle(int32_t pid, int32_t windowId)
{
    CALL_DEBUG_ENTER;
    return WIN_MGR->ClearWindowPointerStyle(pid, windowId);
}

void PointerDrawingManager::DrawPointerStyle(const PointerStyle& pointerStyle)
{
    CALL_DEBUG_ENTER;
    bool simulate = WIN_MGR->IsMouseSimulate();
    if (hasDisplay_ && (hasPointerDevice_ || simulate)) {
        if (surfaceNode_ != nullptr) {
            AttachToDisplay();
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
        Direction direction = static_cast<Direction>((
            ((displayInfo_.direction - displayInfo_.displayDirection) * ANGLE_90 + ANGLE_360) % ANGLE_360) / ANGLE_90);
        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayInfo_.id, displayInfo_.width / CALCULATE_MIDDLE, displayInfo_.height / CALCULATE_MIDDLE,
                pointerStyle, direction);
            MMI_HILOGD("Draw pointer style, mouseStyle:%{public}d", pointerStyle.id);
            return;
        }

        DrawPointer(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, pointerStyle, direction);
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

int32_t PointerDrawingManager::EnableHardwareCursorStats(int32_t pid, bool enable)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPR(hardwareCursorPointerManager_, ERROR_NULL_POINTER);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
        if ((hardwareCursorPointerManager_->EnableStats(enable)) != RET_OK) {
            MMI_HILOGE("Enable stats failed");
            return RET_ERR;
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    MMI_HILOGI("EnableHardwareCursorStats, enable:%{private}d", enable);
    return RET_OK;
}

int32_t PointerDrawingManager::GetHardwareCursorStats(int32_t pid, uint32_t &frameCount, uint32_t &vsyncCount)
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    CHKPR(hardwareCursorPointerManager_, ERROR_NULL_POINTER);
    if (g_isHdiRemoteDied) {
        hardwareCursorPointerManager_->SetHdiServiceState(false);
    }
    if (hardwareCursorPointerManager_->IsSupported()) {
        if ((hardwareCursorPointerManager_->GetCursorStats(frameCount, vsyncCount)) != RET_OK) {
            MMI_HILOGE("Query stats failed");
            return RET_ERR;
        }
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    MMI_HILOGI("GetHardwareCursorStats, frameCount:%{private}d, vsyncCount:%{private}d", frameCount, vsyncCount);
    return RET_OK;
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
        {HAND_POINTING, {ANGLE_NW_RIGHT, IMAGE_POINTER_DEFAULT_PATH + "Hand_Pointing.svg"}},
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
        {MIDDLE_BTN_EAST_WEST, {ANGLE_CENTER, IMAGE_POINTER_DEFAULT_PATH + "MIDDLE_BTN_EAST_WEST.svg"}},
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
#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    std::string productType = OHOS::system::GetParameter("const.build.product", "HYM");
    if (std::find(DEVICE_TYPES.begin(), DEVICE_TYPES.end(), productType) != DEVICE_TYPES.end()) {
        renderThread_ = std::make_unique<std::thread>([this] { this->RenderThreadLoop(); });
    }
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR
}

void PointerDrawingManager::RotateDegree(Direction direction)
{
    CHKPV(surfaceNode_);
    surfaceNode_->SetPivot(0, 0);
    float degree = (static_cast<int>(DIRECTION0) - static_cast<int>(direction)) * ROTATION_ANGLE90;
    surfaceNode_->SetRotation(degree);
}

int32_t PointerDrawingManager::SkipPointerLayer(bool isSkip)
{
    CALL_INFO_TRACE;
    if (surfaceNode_ != nullptr) {
        surfaceNode_->SetSkipLayer(isSkip);
    }
    return RET_OK;
}

void PointerDrawingManager::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    std::ostringstream oss;
    oss << std::endl;

    std::vector<std::string> displayTitles = {"ID", "X", "Y", "Width", "Height", "DPI", "Name", "Uniq",
                                              "Direction", "Display Direction", "Display Mode"};
    DisplayInfo &di = displayInfo_;
    std::vector<std::vector<std::string>> displayInfo = {
        {std::to_string(di.id), std::to_string(di.x), std::to_string(di.y), std::to_string(di.width),
         std::to_string(di.height), std::to_string(di.dpi), di.name, di.uniq,
         std::to_string(static_cast<int32_t>(di.direction)), std::to_string(static_cast<int32_t>(di.displayDirection)),
         std::to_string(static_cast<int32_t>(di.displayMode))}};

    DumpFullTable(oss, "Display Info", displayTitles, displayInfo);
    oss << std::endl;

    std::vector<std::string> titles1 = {"hasDisplay", "hasPointerDevice", "lastPhysicalX", "lastPhysicalY",
                                        "pid", "windowId", "imageWidth", "imageHeight", "canvasWidth", "canvasHeight"};
    std::vector<std::vector<std::string>> data1 = {
        {std::to_string(hasDisplay_), std::to_string(hasPointerDevice_), std::to_string(lastPhysicalX_),
         std::to_string(lastPhysicalY_), std::to_string(pid_), std::to_string(windowId_),
         std::to_string(imageWidth_), std::to_string(imageHeight_), std::to_string(canvasWidth_),
         std::to_string(canvasHeight_)}};

    DumpFullTable(oss, "Cursor Info", titles1, data1);
    oss << std::endl;

    std::vector<std::string> titles2 = {"mouseDisplayState", "mouseIconUpdate", "screenId", "userIconHotSpotX",
                                        "userIconHotSpotY", "tempPointerColor", "lastDirection", "currentDirection"};
    std::vector<std::vector<std::string>> data2 = {
        {std::to_string(mouseDisplayState_), std::to_string(mouseIconUpdate_), std::to_string(screenId_),
         std::to_string(userIconHotSpotX_), std::to_string(userIconHotSpotY_), std::to_string(tempPointerColor_),
         std::to_string(lastDirection_), std::to_string(currentDirection_)}};

    DumpFullTable(oss, "Cursor Info", titles2, data2);
    oss << std::endl;

    std::vector<std::string> styleTitles = {"name", "Size", "Color", "ID"};
    std::vector<std::vector<std::string>> styleData = {
        {"lastMouseStyle", std::to_string(lastMouseStyle_.size), std::to_string(lastMouseStyle_.color),
         std::to_string(lastMouseStyle_.id)},
        {"currentMouseStyle", std::to_string(currentMouseStyle_.size), std::to_string(currentMouseStyle_.color),
         std::to_string(currentMouseStyle_.id)}};

    DumpFullTable(oss, "Cursor Style Info", styleTitles, styleData);
    oss << std::endl;

    std::vector<std::string> pidTitles = {"pid", "visible"};
    std::vector<std::vector<std::string>> pidInfos;
    for (const auto &pidInfo : pidInfos_) {
        pidInfos.push_back({std::to_string(pidInfo.pid), pidInfo.visible ? "true" : "false"});
    }
    DumpFullTable(oss, "Visible Info", pidTitles, pidInfos);
    oss << std::endl;

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
    DumpScreenInfo(oss);
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

    std::string dumpInfo = oss.str();
    dprintf(fd, dumpInfo.c_str());
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
void PointerDrawingManager::DumpScreenInfo(std::ostringstream& oss)
{
    if (!g_screenSourceMode.empty()) {
        std::vector<std::string> allScreenIds;
        std::vector<std::string> allScreenSourceMode;
        for (auto iter = g_screenSourceMode.begin(); iter != g_screenSourceMode.end(); ++iter) {
            allScreenIds.push_back(std::to_string(iter->first));
            if (iter->second->GetType() == Rosen::ScreenType::REAL &&
                iter->second->GetSourceMode() == Rosen::ScreenSourceMode::SCREEN_MIRROR) {
                allScreenSourceMode.push_back("SCREEN_MIRROR");
            } else if (iter->second->GetSourceMode() == Rosen::ScreenSourceMode::SCREEN_EXTEND) {
                allScreenSourceMode.push_back("SCREEN_EXTEND");
            } else if (iter->second->GetType() == Rosen::ScreenType::VIRTUAL) {
                allScreenSourceMode.push_back("SCREEN_VIRTUAL");
            } else if (iter->second->GetSourceMode() == Rosen::ScreenSourceMode::SCREEN_MAIN) {
                allScreenSourceMode.push_back("SCREEN_MAIN");
            } else {
                MMI_HILOGE("No screenType match");
            }
        }
        std::vector<std::vector<std::string>> data3;
        data3.push_back(allScreenSourceMode);
        DumpFullTable(oss, "ScreenInfo", allScreenIds, data3);
        oss << std::endl;
    }
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
void PointerDrawingManager::UpdateBindDisplayId(int32_t displayId)
{
    if (lastDisplayId_ != displayId) {
        MMI_HILOGI("Mouse traversal occurs, lastDisplayId_:%{public}d, displayId:%{public}d",
            lastDisplayId_, displayId);
        CHKPV(surfaceNode_);
        surfaceNode_->DetachToDisplay(screenId_);
        Rosen::RSTransaction::FlushImplicitTransaction();
        screenId_ = static_cast<uint64_t>(displayId);
        MMI_HILOGI("screenId_:%{public}" PRIu64, screenId_);
        AttachToDisplay();
        DrawCursor(MOUSE_ICON(lastMouseStyle_.id));
        DrawHardwareCursor(MOUSE_ICON(lastMouseStyle_.id));
        int32_t currnetPhysicalX =
            lastPhysicalX_ -
            CalculateHardwareXOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(lastDrawPointerStyle_.id)].alignmentWay));
        int32_t currnetPhysicalY =
            lastPhysicalY_ -
            CalculateHardwareYOffset(ICON_TYPE(mouseIcons_[MOUSE_ICON(lastDrawPointerStyle_.id)].alignmentWay));
        surfaceNode_->SetBounds(currnetPhysicalX, currnetPhysicalY, hardwareCanvasSize_, hardwareCanvasSize_);
        Rosen::RSTransaction::FlushImplicitTransaction();
        lastDisplayId_ = displayId;
    }
}

bool PointerDrawingManager::IsSupported()
{
    return hardwareCursorPointerManager_->IsSupported();
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

void PointerDrawingManager::DrawScreenCenterPointer(const PointerStyle& pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (hasDisplay_ && hasPointerDevice_) {
        if (surfaceNode_ != nullptr) {
            AttachToDisplay();
            Rosen::RSTransaction::FlushImplicitTransaction();
        }
        Direction direction = static_cast<Direction>((
            ((displayInfo_.direction - displayInfo_.displayDirection) * ANGLE_90 + ANGLE_360) % ANGLE_360) / ANGLE_90);
        DrawPointer(displayInfo_.id, displayInfo_.width / CALCULATE_MIDDLE, displayInfo_.height / CALCULATE_MIDDLE,
            pointerStyle, direction);
    }
}

std::shared_ptr<OHOS::Media::PixelMap> PointerDrawingManager::GetUserIconCopy()
{
    std::lock_guard<std::mutex> guard(mtx_);
    return userIcon_;
}

#ifdef OHOS_BUILD_ENABLE_HARDWARE_CURSOR
int32_t PointerDrawingManager::PrepareBuffer(uint32_t width, uint32_t height)
{
    if (cursorBuffers_.size() == QUEUE_SIZE) {
        bufferId_++;
        if (bufferId_ == cursorBuffers_.size()) {
            bufferId_ = 0;
        }
        return RET_OK;
    }

    OHOS::BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER,
        .timeout = 150,
    };

    for (size_t i = 0; i < RETRY_COUNT * QUEUE_SIZE && cursorBuffers_.size() < QUEUE_SIZE; i++) {
        sptr<OHOS::SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
        if (buffer == nullptr) {
            MMI_HILOGE("SurfaceBuffer Create failed");
            continue;
        }

        OHOS::GSError ret = buffer->Alloc(requestConfig);
        if (ret != OHOS::GSERROR_OK) {
            MMI_HILOGE("SurfaceBuffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
            continue;
        }

        cursorBuffers_.push_back(buffer);
    }

    return RET_OK;
}

int32_t PointerDrawingManager::DrawHardwareCursor(const MOUSE_ICON mouseStyle)
{
    CALCULATE_CANVAS_SIZE(CALCULATE_CANVAS_SIZE_, CHANGE) = GetCanvasSize();
    auto canvasSize = static_cast<int32_t>(CALCULATE_CANVAS_SIZE_CHANGE);
    int32_t width = canvasSize;
    int32_t height = canvasSize;

    DrawCursor(mouseStyle, width, height);

    MMI_HILOGD("DrawHardwareCursor success");
    return RET_OK;
}

int32_t PointerDrawingManager::DrawCursor(const MOUSE_ICON mouseStyle, uint32_t width, uint32_t height)
{
    if (PrepareBuffer(width, height) != RET_OK ||
        cursorBuffers_[bufferId_] == nullptr ||
        cursorBuffers_[bufferId_]->GetVirAddr() == nullptr ||
        hardwareCursorPointerManager_ == nullptr) {
        MMI_HILOGE("buffer is null");
        return RET_ERR;
    }

    auto addr = static_cast<uint8_t *>(cursorBuffers_[bufferId_]->GetVirAddr());
    DoDraw(addr, cursorBuffers_[bufferId_]->GetWidth(), cursorBuffers_[bufferId_]->GetHeight(), mouseStyle);

    MMI_HILOGD("DrawCursor success");
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_HARDWARE_CURSOR

int32_t PointerDrawingManager::SetCustomCursor(int32_t pid, int32_t windowId, CustomCursor cursor,
    CursorOptions options)
{
    CALL_DEBUG_ENTER;
    if (windowId < 0 || WIN_MGR->CheckWindowIdPermissionByPid(windowId, pid) != RET_OK) {
        MMI_HILOGE("The windowId not in right pid");
        return ERROR_WINDOW_ID_PERMISSION_DENIED;
    }
    if (options.followSystem) {
        return SetCustomCursor(cursor.pixelMap, pid, windowId, cursor.focusX, cursor.focusY);
    }
    int32_t ret = UpdateCursorProperty(cursor);
    if (ret != RET_OK) {
        MMI_HILOGE("UpdateCursorProperty is failed");
        return ret;
    }
    mouseIconUpdate_ = true;
    PointerStyle style;
    style.id = MOUSE_ICON::DEVELOPER_DEFINED_ICON;
    lastMouseStyle_ = style;

    ret = SetPointerStyle(pid, windowId, style);
    if (ret == RET_ERR) {
        MMI_HILOGE("SetPointerStyle is failed");
    }
    MMI_HILOGD("style.id:%{public}d, userIconHotSpotX_:%{public}d, userIconHotSpotY_:%{public}d",
        style.id, userIconHotSpotX_, userIconHotSpotY_);
    return ret;
}

int32_t PointerDrawingManager::UpdateCursorProperty(CustomCursor cursor)
{
    CHKPR(cursor.pixelMap, RET_ERR);
    Media::PixelMap* newPixelMap = static_cast<Media::PixelMap*>(cursor.pixelMap);
    CHKPR(newPixelMap, RET_ERR);
    Media::ImageInfo imageInfo;
    newPixelMap->GetImageInfo(imageInfo);
    if (imageInfo.size.width < cursor.focusX || imageInfo.size.width < cursor.focusY) {
        MMI_HILOGE("focus is invalid");
        return RET_ERR;
    }
    float scale = 1.0f;
    if (imageInfo.size.width > MAX_CUSTOM_CURSOR_SIZE || imageInfo.size.height > MAX_CUSTOM_CURSOR_SIZE) {
        scale = MAX_CUSTOM_CURSOR_DIMENSION / std::max(imageInfo.size.width, imageInfo.size.height);
        newPixelMap->scale(scale, scale, Media::AntiAliasingOption::LOW);
    }
 
    int32_t cursorSize = GetPointerSize();
    cursorWidth_ = pow(INCREASE_RATIO, cursorSize - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
    cursorHeight_ = pow(INCREASE_RATIO, cursorSize - 1) * displayInfo_.dpi * GetIndependentPixels() / BASELINE_DENSITY;
 
    cursorWidth_ = cursorWidth_ < MIN_CURSOR_SIZE ? MIN_CURSOR_SIZE : cursorWidth_;
    cursorHeight_ = cursorHeight_ < MIN_CURSOR_SIZE ? MIN_CURSOR_SIZE : cursorHeight_;
 
    float xAxis = static_cast<float>(cursorWidth_) / static_cast<float>(imageInfo.size.width);
    float yAxis = static_cast<float>(cursorHeight_) / static_cast<float>(imageInfo.size.height);
 
    {
        std::lock_guard<std::mutex> guard(mtx_);
        userIcon_.reset(newPixelMap);
    }
 
    userIconHotSpotX_ = static_cast<int32_t>((float)cursor.focusX * xAxis * scale);
    userIconHotSpotY_ = static_cast<int32_t>((float)cursor.focusY * yAxis * scale);
 
    MMI_HILOGI("cursorWidth:%{public}d, cursorHeight:%{public}d, imageWidth:%{public}d, imageHeight:%{public}d,"
        "focusX:%{public}d, focusY:%{public}d, xAxis:%{public}f, yAxis:%{public}f, userIconHotSpotX_:%{public}d,"
        "userIconHotSpotY_:%{public}d", cursorWidth_, cursorHeight_, imageInfo.size.width, imageInfo.size.height,
        cursor.focusX, cursor.focusY, xAxis, yAxis, userIconHotSpotX_, userIconHotSpotY_);
 
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
