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
#include "register_eventhandle_manager.h"
#include <iostream>
#include "proto.h"
#include "util.h"
#include "util_ex.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "RegisterEventHandleManager" };
    }
}

OHOS::MMI::RegisterEventHandleManager::RegisterEventHandleManager()
{
}

OHOS::MMI::RegisterEventHandleManager::~RegisterEventHandleManager()
{
}

int32_t OHOS::MMI::RegisterEventHandleManager::RegisterEvent(int32_t eventHangle, int32_t fd)
{
    LOCKGUARD(mu_);
    CHKR(eventHangle >= 0, PARAM_INPUT_INVALID, UNKNOWN_EVENT);
    switch (eventHangle) {
        case ID_MSG_COMMONEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_COMMONEVENTHANDLE_BEGIN, ID_MSG_COMMONEVENTHANDLE_END, fd);
            break;
        case ID_MSG_KEYEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_KEYEVENTHANDLE_BEGIN, ID_MSG_KEYEVENTHANDLE_END, fd);
            break;
        case ID_MSG_MEDIAEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_MEDIAEVENTHANDLE_BEGIN, ID_MSG_MEDIAEVENTHANDLE_END, fd);
            break;
        case ID_MSG_SYSTEMEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_SYSTEMEVENTHANDLE_BEGIN, ID_MSG_SYSTEMEVENTHANDLE_END, fd);
            break;
        case ID_MSG_TELEPHONEEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_TELEPHONEEVENTHANDLE_BEGIN, ID_MSG_TELEPHONEEVENTHANDLE_END, fd);
            break;
        case ID_MSG_TOUCHEVENTHANDLE_BEGIN:
            RegisterEventHandleByIdMsage(ID_MSG_TOUCHEVENTHANDLE_BEGIN, ID_MSG_TOUCHEVENTHANDLE_END, fd);
            break;
        default:
            MMI_LOGT("It's no this event handle! \n");
            return UNKNOWN_EVENT;
    }
    MMI_LOGT("event:%{public}d fd:%{public}d \n", eventHangle, fd);
    return RET_OK;
}

int32_t OHOS::MMI::RegisterEventHandleManager::UnregisterEventHandleManager(int32_t eventHangle, int32_t fd)
{
    LOCKGUARD(mu_);
    CHKR(eventHangle >= 0, PARAM_INPUT_INVALID, UNKNOWN_EVENT);
    switch (eventHangle) {
        case ID_MSG_COMMONEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_COMMONEVENTHANDLE_BEGIN, ID_MSG_COMMONEVENTHANDLE_END, fd);
            break;
        case ID_MSG_KEYEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_KEYEVENTHANDLE_BEGIN, ID_MSG_KEYEVENTHANDLE_END, fd);
            break;
        case ID_MSG_MEDIAEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_MEDIAEVENTHANDLE_BEGIN, ID_MSG_MEDIAEVENTHANDLE_END, fd);
            break;
        case ID_MSG_SYSTEMEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_SYSTEMEVENTHANDLE_BEGIN, ID_MSG_SYSTEMEVENTHANDLE_END, fd);
            break;
        case ID_MSG_TELEPHONEEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_TELEPHONEEVENTHANDLE_BEGIN, ID_MSG_TELEPHONEEVENTHANDLE_END, fd);
            break;
        case ID_MSG_TOUCHEVENTHANDLE_BEGIN:
            UnregisterEventHandleByIdMsage(ID_MSG_TOUCHEVENTHANDLE_BEGIN, ID_MSG_TOUCHEVENTHANDLE_END, fd);
            break;
        default:
            MMI_LOGT("It's no this event handle! \n");
            return UNKNOWN_EVENT;
    }

    return RET_OK;
}

void OHOS::MMI::RegisterEventHandleManager::UnregisterEventHandleBySocketFd(int32_t fd)
{
    LOCKGUARD(mu_);
    CHK(fd >= 0, PARAM_INPUT_INVALID);
    auto iter = mapRegisterManager_.begin();
    while (iter != mapRegisterManager_.end()) {
        if (iter->second == fd) {
            iter = mapRegisterManager_.erase(iter);
        } else {
            iter++;
        }
    }
}

int32_t OHOS::MMI::RegisterEventHandleManager::FindSocketFdsByEventHandle(const int32_t eventHangle,
                                                                          std::vector<int32_t>& fds)
{
    LOCKGUARD(mu_);
    CHKR(eventHangle >= 0, PARAM_INPUT_INVALID, UNKNOWN_EVENT);
    auto it = mapRegisterManager_.find(eventHangle);
    if (it != mapRegisterManager_.end()) {
        for (size_t k = 0; k < mapRegisterManager_.count(eventHangle); k++, it++) {
            fds.push_back(it->second);
        }
        return RET_OK;
    } else {
        return RET_ERR;
    }
}

void OHOS::MMI::RegisterEventHandleManager::PrintfMap()
{
    LOCKGUARD(mu_);
    for (auto i : mapRegisterManager_) {
        std::cout << "event handle is " << i.first << ", fd is " << i.second << std::endl;
    }
}

void OHOS::MMI::RegisterEventHandleManager::Dump(int32_t fd)
{
    LOCKGUARD(mu_);
    std::string strTmp;
    mprintf(fd, "RegsEvent: count=%d", mapRegisterManager_.size());
    for (auto it = mapRegisterManager_.begin(); it != mapRegisterManager_.end(); 
         it = mapRegisterManager_.upper_bound(it->first))
    {
        strTmp.clear();
        auto evs = mapRegisterManager_.equal_range(it->first);
        strTmp = "type=";
        strTmp += std::to_string(it->first) + " fds:[";
        for (auto itr = evs.first; itr != evs.second; ++itr)
        {
            strTmp += std::to_string(itr->second) + ",";
        }
        strTmp.resize(strTmp.size()-1);
        strTmp += "]";
        mprintf(fd, "\t%s", strTmp.c_str());
    }
}

void OHOS::MMI::RegisterEventHandleManager::Clear()
{
    if (mu_.try_lock()) {
        mu_.unlock();
    }
    mapRegisterManager_.clear();
}

void OHOS::MMI::RegisterEventHandleManager::RegisterEventHandleByIdMsage(const int32_t idMsgBegin,
                                                                         const int32_t idMsgEnd,
                                                                         const int32_t fd)
{
    int32_t idMsg = idMsgBegin + 1;
    for (int32_t it = idMsg; it < idMsgEnd; it++) {
        mapRegisterManager_.insert(std::pair<int32_t, int32_t>(it, fd));
    }
}

void OHOS::MMI::RegisterEventHandleManager::UnregisterEventHandleByIdMsage(const int32_t idMsgBegin,
                                                                           const int32_t idMsgEnd,
                                                                           const int32_t fd)
{
    int32_t idMsg = idMsgBegin + 1;
    auto it = mapRegisterManager_.find(idMsg);
    while (it != mapRegisterManager_.end()) {
        if ((it->first < idMsgEnd) && (it->second == fd)) {
            it = mapRegisterManager_.erase(it);
        } else {
            it++;
        }
    }
}
