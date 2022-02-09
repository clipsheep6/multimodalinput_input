/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_INPUT_WINDOWS_MANAGER_H
#define OHOS_INPUT_WINDOWS_MANAGER_H

#include <vector>
#include "singleton.h"
#include "uds_server.h"
#include "display_info.h"
#include "input_event.h"
#include "pointer_event.h"
#include "libinput.h"

#ifdef OHOS_WESTEN_MODEL
extern "C" {
#include <screen_info.h>
#include <libinput-seat-export.h>
}
#else
SurfaceInfo {
    int surfaceId;
    int dstX;
    int dstY;
    int dstW;
    int dstH;
    int srcX;
    int srcY;
    int srcW;
    int srcH;
    double opacity;
    int visibility; // 0 or 1
    int onLayerId;
};

LayerInfo {
    int layerId;
    int dstX;
    int dstY;
    int dstW;
    int dstH;
    int srcX;
    int srcY;
    int srcW;
    int srcH;
    double opacity;
    int visibility; // 0 or 1
    int onScreenId;
    int nSurfaces;
    SurfaceInfo** surfaces;
};

ScreenInfo {
    int screenId;
    char* connectorName;
    int width;
    int height;
    int nLayers;
    LayerInfo** layers;
};

SeatInfo {
    char* seatName;
    int deviceFlags;
    int focusWindowId;
};

multimodal_libinput_event {
    libinput_event *event;
    void *userdata;
};

SeatInfo** GetSeatsInfo(void);
ScreenInfo** GetScreensInfo(void);
void FreeSurfaceInfo(SurfaceInfo* pSurface);
void FreeLayerInfo(LayerInfo* pLayer);
void FreeScreenInfo(ScreenInfo* pScreen);
void FreeScreensInfo(ScreenInfo** screens);
void FreeSeatsInfo(SeatInfo** seats);
using SeatInfoChangeListener = void (*)();
using ScreenInfoChangeListener = void (*)();
void SetSeatListener(const SeatInfoChangeListener listener);
void SetScreenListener(const ScreenInfoChangeListener listener);

multimodal_libinput_event;
typedef void (*LibInputEventListener)(multimodal_libinput_event *event);
namespace OHOS {
namespace MMI {
void SetLibInputEventListener(const LibInputEventListener listener);
}
}
#endif

MMISurfaceInfo : public SurfaceInfo {
    int32_t screenId;
};

namespace OHOS {
namespace MMI {

MouseLocation {
    int32_t globleX;
    int32_t globleY;
    int32_t localX;
    int32_t localY;
};

class InputWindowsManager : public DelayedSingleton<InputWindowsManager> {
public:
    InputWindowsManager();
    virtual ~InputWindowsManager();

    bool Init(UDSServer& udsServer);
    void UpdateSeatsInfo();
    void UpdateScreensInfo();

    const ScreenInfo* GetScreenInfo(int32_t screenId);
    const LayerInfo* GetLayerInfo(int32_t layerId);
    const MMISurfaceInfo* GetSurfaceInfo(int32_t sufaceId);
    bool CheckFocusSurface(double x, double y, const MMISurfaceInfo& info) const;
    const MMISurfaceInfo* GetTouchSurfaceInfo(double x, double y);
    void TransfromToSurfaceCoordinate(double& x, double& y, const MMISurfaceInfo& info, bool debug = false);

    bool GetTouchSurfaceId(const double x, const double y, std::vector<int32_t>& ids);

    const std::vector<ScreenInfo>& GetScreenInfo() const;

    const std::map<int32_t, LayerInfo>& GetLayerInfo() const;

    const std::map<int32_t, MMISurfaceInfo>& GetSurfaceInfo() const;

    void InsertSurfaceInfo(const MMISurfaceInfo& tmpSurfaceInfo);

    void PrintAllNormalSurface();

    void SetFocusSurfaceId(int32_t id);
    void SetTouchFocusSurfaceId(int32_t id);

    int32_t GetFocusSurfaceId() const;
    int32_t GetTouchFocusSurfaceId() const;

    size_t GetSurfaceIdList(std::vector<int32_t>& ids);
    std::string GetSurfaceIdListString();
    void Clear();
    void Dump(int32_t fd);

    /*********************************新框架接口添加*************************** */
    int32_t GetPidUpdateTarget(std::shared_ptr<InputEvent> inputEvent);
    int32_t UpdateTarget(std::shared_ptr<InputEvent> inputEvent);
    void UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
        const std::vector<LogicalDisplayInfo> &logicalDisplays);
    bool TouchPadPointToDisplayPoint_2(libinput_event_touch* touch,
    int32_t& logicalX, int32_t& logicalY, int32_t& logicalDisplayId);
    const std::vector<LogicalDisplayInfo>& GetLogicalDisplayInfo() const;
    const std::map<int32_t, WindowInfo>& GetWindowInfo() const;
    bool FindWindow(std::shared_ptr<PointerEvent> pointerEvent);
    MouseLocation GetMouseInfo();
    void UpdateAndAdjustMouseLoction(double& x, double& y);
    void AdjustGlobalCoordinate(int32_t& globalX, int32_t& globalY, int32_t width, int32_t height);
    bool IsCheckDisplayIdIfExist(int32_t& displayId);
    LogicalDisplayInfo* GetLogicalDisplayById(int32_t displayId);
    int32_t UpdateTargetPointer(std::shared_ptr<PointerEvent> pointerEvent);
    bool TouchPadPointToDisplayPoint(libinput_event_touch* touch,
    int32_t& logicalX, int32_t& logicalY, int32_t& logicalDisplayId);
    bool TransformTouchPointToDisplayPoint(libinput_event_touch* touch,
    int32_t targetDisplayId, int32_t& displayX, int32_t& displayY);

    void AdjustCoordinate(double &coordinateX, double &coordinateY);
    void FixCursorPosition(int32_t &globalX, int32_t &globalY, int cursorW, int cursorH);

private:
    void SetFocusId(int32_t id);
    void PrintDebugInfo();
    void SaveScreenInfoToMap(const ScreenInfo **screen_info);
    bool FindSurfaceByCoordinate(double x, double y, const SurfaceInfo& pstrSurface);
    int32_t UpdateMouseTargetOld(std::shared_ptr<PointerEvent> pointerEvent);
    int32_t UpdateTouchScreenTargetOld(std::shared_ptr<PointerEvent> pointerEvent);
    int32_t UpdateTouchPadTargetOld(std::shared_ptr<PointerEvent> pointerEvent);

    /*********************************新框架接口添加*************************** */
    bool IsTouchWindow(int32_t x, int32_t y, const WindowInfo &info) const;
    void PrintDisplayDebugInfo();
    void SetLocalInfo(int32_t x, int32_t y);
    int32_t UpdateMouseTarget(std::shared_ptr<PointerEvent> pointerEvent);
    int32_t UpdateTouchScreenTarget(std::shared_ptr<PointerEvent> pointerEvent);
    int32_t UpdateTouchPadTarget(std::shared_ptr<PointerEvent> pointerEvent);
    PhysicalDisplayInfo* GetPhysicalDisplayById(int32_t id);
    PhysicalDisplayInfo* FindMatchedPhysicalDisplayInfo(const std::string seatId, const std::string seatName);
private:
    std::mutex mu_;
    SeatInfo** seatsInfo_ = nullptr;
    ScreenInfo **screensInfo_ = nullptr;
    int32_t focusInfoID_ = 0;
    int32_t touchFocusId_ = 0;
    std::vector<int32_t> surfacesList_; // surfaces ids list
    std::vector<ScreenInfo> screenInfoVec_ = {};
    std::map<int32_t, LayerInfo> layers_ = {};
    std::map<int32_t, MMISurfaceInfo> surfaces_ = {};
    UDSServer* udsServer_ = nullptr;

    /* *********************************新框架接口添加*************************** */
    std::vector<PhysicalDisplayInfo> physicalDisplays_ = {};
    std::vector<LogicalDisplayInfo> logicalDisplays_ = {};
    std::map<int32_t, WindowInfo> windowInfos_ = {};
    MouseLocation mouseLoction_ = {};
    const int32_t INVALID_LOCATION = 0;
};
}
}

#define WinMgr OHOS::MMI::InputWindowsManager::GetInstance()
#endif
