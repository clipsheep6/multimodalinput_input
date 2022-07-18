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

#include "white_list_util.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "anonymous_string.h"
#include "distributed_hardware_log.h"

#include "dinput_errcode.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const char* const SPLIT_LINE = "|";
    const char* const SPLIT_COMMA = ",";
    const int32_t COMB_KEY_VEC_MIN_LEN = 2;
    const int32_t LAST_KEY_ACTION_LEN = 1;
    const int32_t LAST_KEY_LEN = 1;
}

WhiteListUtil::WhiteListUtil()
{
    DHLOGI("Ctor WhiteListUtil, addr: %p", this);
    Init();
}

WhiteListUtil::~WhiteListUtil()
{
    DHLOGI("Dtor WhiteListUtil, addr: %p", this);
}

WhiteListUtil &WhiteListUtil::GetInstance(void)
{
    static WhiteListUtil instance;
    return instance;
}

int32_t WhiteListUtil::Init()
{
    const char* const whiteListFilePath = "/etc/dinput_business_event_whitelist.cfg";
    std::ifstream inFile(whiteListFilePath, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        // file open error
        DHLOGE("%s error, file open fail path=%s", __func__, whiteListFilePath);
        return ERR_DH_INPUT_WHILTELIST_INIT_FAIL;
    }

    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;

    std::string line;
    while (getline(inFile, line)) {
        DHLOGI("%s called success, line=%s", __func__, line.c_str());

        vecKeyCode.clear();
        vecCombinationKey.clear();

        std::size_t pos1 = line.find(SPLIT_COMMA);
        while (std::string::npos != pos1) {
            std::string column = line.substr(0, pos1);
            line = line.substr(pos1 + 1, line.size());
            pos1 = line.find(SPLIT_COMMA);
            vecKeyCode.clear();
            ReadLineDataStepOne(column, vecKeyCode, vecCombinationKey);
        }

        if (!line.empty()) {
            int32_t keyCode = std::stoi(line);
            if (keyCode != 0) {
                vecKeyCode.push_back(keyCode);
            }

            if (!vecKeyCode.empty()) {
                vecCombinationKey.push_back(vecKeyCode);
                vecKeyCode.clear();
            }
        }

        if (!vecCombinationKey.empty()) {
            vecWhiteList.push_back(vecCombinationKey);
            vecCombinationKey.clear();
        }
    }

    inFile.close();

    std::string localNetworkId = GetLocalDeviceInfo().networkId;
    if (!localNetworkId.empty()) {
        SyncWhiteList(localNetworkId, vecWhiteList);
    } else {
        DHLOGE("query local network id from softbus failed");
    }

    DHLOGI("Local WhiteListUtil init success");
    return DH_SUCCESS;
}

int32_t WhiteListUtil::UnInit(void)
{
    DHLOGI("%s called", __func__);
    ClearWhiteList();
    return DH_SUCCESS;
}

void WhiteListUtil::ReadLineDataStepOne(std::string &column, TYPE_KEY_CODE_VEC &vecKeyCode,
                                        TYPE_COMBINATION_KEY_VEC &vecCombinationKey) const
{
    std::size_t pos2 = column.find(SPLIT_LINE);
    while (pos2 != std::string::npos) {
        std::string single = column.substr(0, pos2);
        column = column.substr(pos2 + 1, column.size());
        pos2 = column.find(SPLIT_LINE);

        if (!single.empty()) {
            int32_t keyCode = std::stoi(single);
            if (keyCode != 0) {
                vecKeyCode.push_back(keyCode);
            }
        }
    }

    if (!column.empty()) {
        int32_t keyCode = std::stoi(column);
        if (keyCode != 0) {
            vecKeyCode.push_back(keyCode);
        }
    }

    if (!vecKeyCode.empty()) {
        vecCombinationKey.push_back(vecKeyCode);
        vecKeyCode.clear();
    }
}

int32_t WhiteListUtil::SyncWhiteList(const std::string &deviceId, const TYPE_WHITE_LIST_VEC &vecWhiteList)
{
    DHLOGI("deviceId=%s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    mapDeviceWhiteList_[deviceId] = vecWhiteList;
    for (auto combKeys : vecWhiteList) {
        GetCombKeysHash(combKeys, combKeysHashMap_[deviceId]);
    }

    return DH_SUCCESS;
}

void WhiteListUtil::GetCombKeysHash(TYPE_COMBINATION_KEY_VEC combKeys, std::unordered_set<std::string> &targetSet)
{
    if (combKeys.size() < COMB_KEY_VEC_MIN_LEN) {
        DHLOGE("white list item length invalid");
        return;
    }

    TYPE_KEY_CODE_VEC lastKeyAction = combKeys.back();
    if (lastKeyAction.size() != LAST_KEY_ACTION_LEN) {
        DHLOGE("last key action invalid");
        return;
    }
    combKeys.pop_back();
    TYPE_KEY_CODE_VEC lastKey = combKeys.back();
    if (lastKey.size() != LAST_KEY_LEN) {
        DHLOGE("last key invalid");
        return;
    }
    combKeys.pop_back();

    std::unordered_set<std::string> hashSets;
    WhiteListItemHash hash;
    GetAllComb(combKeys, hash, combKeys.size(), hashSets);

    for (auto &hash : hashSets) {
        targetSet.insert(hash + std::to_string(lastKey[0]) + std::to_string(lastKeyAction[0]));
    }
}

void WhiteListUtil::GetAllComb(TYPE_COMBINATION_KEY_VEC vecs, WhiteListItemHash hash,
    int32_t targetLen, std::unordered_set<std::string> &hashSets)
{
    for (size_t i = 0; i < vecs.size(); i++) {
        TYPE_KEY_CODE_VEC nowVec = vecs[i];
        for (int32_t code : nowVec) {
            WhiteListItemHash newHash = { hash.hash + std::to_string(code), hash.len + 1 };
            TYPE_COMBINATION_KEY_VEC leftVecs = vecs;
            leftVecs.erase(leftVecs.begin() + i);
            GetAllComb(leftVecs, newHash, targetLen, hashSets);
        }
    }

    if (hash.len == targetLen) {
        hashSets.insert(hash.hash);
    }
}

int32_t WhiteListUtil::ClearWhiteList(const std::string &deviceId)
{
    DHLOGI("deviceId=%s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    mapDeviceWhiteList_.erase(deviceId);
    return DH_SUCCESS;
}

int32_t WhiteListUtil::ClearWhiteList(void)
{
    std::lock_guard<std::mutex> lock(mutex_);
    TYPE_DEVICE_WHITE_LIST_MAP().swap(mapDeviceWhiteList_);
    return DH_SUCCESS;
}

int32_t WhiteListUtil::GetWhiteList(const std::string &deviceId, TYPE_WHITE_LIST_VEC &vecWhiteList)
{
    DHLOGI("start, deviceId=%s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    TYPE_DEVICE_WHITE_LIST_MAP::const_iterator iter = mapDeviceWhiteList_.find(deviceId);
    if (iter != mapDeviceWhiteList_.end()) {
        vecWhiteList = iter->second;
        DHLOGI("GetWhiteList success, deviceId=%s", GetAnonyString(deviceId).c_str());
        return DH_SUCCESS;
    }

    DHLOGI("GetWhiteList fail, deviceId=%s", GetAnonyString(deviceId).c_str());
    return ERR_DH_INPUT_WHILTELIST_GET_WHILTELIST_FAIL;
}

bool WhiteListUtil::CheckSubVecData(const TYPE_COMBINATION_KEY_VEC::iterator &iter2,
                                    const TYPE_KEY_CODE_VEC::iterator &iter3) const
{
    bool bIsMatching = false;
    for (TYPE_KEY_CODE_VEC::iterator iter4 = iter2->begin(); iter4 != iter2->end(); ++iter4) {
        if (*iter4 == *iter3) {
            bIsMatching = true;
            break;
        }
    }
    return bIsMatching;
}

std::string WhiteListUtil::GetBusinessEventHash(const BusinessEvent &event)
{
    std::string hash = "";
    for (auto &p : event.pressedKeys) {
        hash += std::to_string(p);
    }
    hash += std::to_string(event.keyCode);
    hash += std::to_string(event.keyAction);

    return hash;
}

bool WhiteListUtil::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    DHLOGI("start, deviceId=%s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    if (combKeysHashMap_.empty()) {
        DHLOGE("%s called, white list is empty!", __func__);
        return false;
    }

    auto iter = combKeysHashMap_.find(deviceId);
    if (iter == combKeysHashMap_.end()) {
        DHLOGE("%s called, not find by deviceId!", __func__);
        return false;
    }

    std::string hash = GetBusinessEventHash(event);
    DHLOGI("Searched business event hash: %s", hash.c_str());

    return combKeysHashMap_[deviceId].find(hash) != combKeysHashMap_[deviceId].end();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
