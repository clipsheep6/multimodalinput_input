/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "input_window_transfer.h"

#include "input_connect_manager.h"
#include "input_manager_impl.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputWindowTransfer"};
} // namespace

void InputWindowTransfer::UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (displayGroupInfo.windowsInfo.empty() || displayGroupInfo.displaysInfo.empty()) {
        MMI_HILOGE("The windows info or display info is empty!");
        return;
    }
    for (const auto &item : displayGroupInfo.windowsInfo) {
        if ((item.defaultHotAreas.size() > WindowInfo::MAX_HOTAREA_COUNT) ||
            (item.pointerHotAreas.size() > WindowInfo::MAX_HOTAREA_COUNT) ||
            item.defaultHotAreas.empty() || item.pointerHotAreas.empty()) {
            MMI_HILOGE("Hot areas check failed! defaultHotAreas:size:%{public}zu,"
                       "pointerHotAreas:size:%{public}zu",
                       item.defaultHotAreas.size(), item.pointerHotAreas.size());
            return;
        }
    }
    displayGroupInfo_ = displayGroupInfo;
    SendDisplayInfo();
    PrintDisplayInfo();
}

void InputWindowTransfer::SendDisplayInfo()
{
    MMIClientPtr client = InputMgrImpl.GetMMIClient();
    CHKPV(client);
    NetPacket pkt(MmiMessageId::DISPLAY_INFO);
    if (PackDisplayData(pkt) == RET_ERR) {
        MMI_HILOGE("Pack display info failed");
        return;
    }
    if (!client->SendMessage(pkt)) {
        MMI_HILOGE("Send message failed, errCode:%{public}d", MSG_SEND_FAIL);
    }
}

int32_t InputWindowTransfer::PackDisplayData(NetPacket &pkt)
{
    pkt << displayGroupInfo_.width << displayGroupInfo_.height << displayGroupInfo_.focusWindowId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write logical data failed");
        return RET_ERR;
    }
    if (PackWindowInfo(pkt) == RET_ERR) {
        MMI_HILOGE("Packet write windows info failed");
        return RET_ERR;
    }
    return PackDisplayInfo(pkt);
}

int32_t InputWindowTransfer::PackWindowInfo(NetPacket &pkt)
{
    uint32_t num = static_cast<uint32_t>(displayGroupInfo_.windowsInfo.size());
    pkt << num;
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        pkt << item.id << item.pid << item.uid << item.area
            << item.defaultHotAreas << item.pointerHotAreas
            << item.agentWindowId << item.flags;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write windows data failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputWindowTransfer::PackDisplayInfo(NetPacket &pkt)
{
    uint32_t num = static_cast<uint32_t>(displayGroupInfo_.displaysInfo.size());
    pkt << num;
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        pkt << item.id << item.x << item.y << item.width
            << item.height << item.name << item.uniq << item.direction;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write display data failed");
        return RET_ERR;
    }
    return RET_OK;
}

void InputWindowTransfer::PrintDisplayInfo()
{
    MMI_HILOGI("logicalInfo,width:%{public}d,height:%{public}d,focusWindowId:%{public}d",
        displayGroupInfo_.width, displayGroupInfo_.height, displayGroupInfo_.focusWindowId);
    MMI_HILOGI("windowsInfos,num:%{public}zu", displayGroupInfo_.windowsInfo.size());
    for (const auto &item : displayGroupInfo_.windowsInfo) {
        MMI_HILOGI("windowsInfos,id:%{public}d,pid:%{public}d,uid:%{public}d,"
            "area.x:%{public}d,area.y:%{public}d,area.width:%{public}d,area.height:%{public}d,"
            "defaultHotAreas.size:%{public}zu,pointerHotAreas.size:%{public}zu,"
            "agentWindowId:%{public}d,flags:%{public}d",
            item.id, item.pid, item.uid, item.area.x, item.area.y, item.area.width,
            item.area.height, item.defaultHotAreas.size(), item.pointerHotAreas.size(),
            item.agentWindowId, item.flags);
        for (const auto &win : item.defaultHotAreas) {
            MMI_HILOGI("defaultHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                win.x, win.y, win.width, win.height);
        }
        for (const auto &pointer : item.pointerHotAreas) {
            MMI_HILOGI("pointerHotAreas:x:%{public}d,y:%{public}d,width:%{public}d,height:%{public}d",
                pointer.x, pointer.y, pointer.width, pointer.height);
        }
    }

    MMI_HILOGI("displayInfos,num:%{public}zu", displayGroupInfo_.displaysInfo.size());
    for (const auto &item : displayGroupInfo_.displaysInfo) {
        MMI_HILOGI("displayInfos,id:%{public}d,x:%{public}d,y:%{public}d,"
            "width:%{public}d,height:%{public}d,name:%{public}s,"
            "uniq:%{public}s,direction:%{public}d",
            item.id, item.x, item.y, item.width, item.height, item.name.c_str(),
            item.uniq.c_str(), item.direction);
    }
}

void InputWindowTransfer::OnConnected()
{
    CALL_DEBUG_ENTER;
    if (displayGroupInfo_.windowsInfo.empty() || displayGroupInfo_.displaysInfo.empty()) {
        MMI_HILOGE("The windows info or display info is empty");
        return;
    }
    SendDisplayInfo();
    PrintDisplayInfo();
}
} // namespace MMI
} // namespace OHOS
