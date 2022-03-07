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

#ifndef INJECTION_EVENT_DISPATCH_H
#define INJECTION_EVENT_DISPATCH_H

#include "manage_inject_device.h"
#include "injection_tools_help_func.h"

namespace OHOS {
namespace MMI {
using InjectFunction = std::function<int32_t()>;

struct InjectFunctionMap {
    std::string id;
    InjectFunction fun;
};

constexpr int32_t ARGV_VALID = 2;

class InjectionEventDispatch {
public:
    InjectionEventDispatch() = default;
    ~InjectionEventDispatch() = default;
    void Init();
    void InitManageFunction();
    void Run();
    int32_t OnSendEvent();
    int32_t OnJson();
    int32_t OnHelp();
    int32_t ExecuteFunction(std::string funId);
    int32_t GetDevTypeIndex(int32_t devIndex);
    int32_t GetDevIndexType(int32_t devType);
    int32_t GetDeviceIndex(const std::string& deviceNameText);
    std::string GetFunId();
    bool VirifyArgvs(const int32_t& argc, const std::vector<std::string>& argv);
    bool RegistInjectEvent(InjectFunctionMap& msg)
    {
        auto it = injectFuns_.find(msg.id);
        if (it != injectFuns_.end()) {
            return false;
        }
        injectFuns_[msg.id] = msg.fun;
        return true;
    }

    InjectFunction* GetFun(std::string id)
    {
        auto it = injectFuns_.find(id);
        if (it == injectFuns_.end()) {
            return nullptr;
        }
        return &it->second;
    }
private:
    std::string funId_ = "";
    int32_t argvNum_ = 0;
    ManageInjectDevice manageInjectDevice_;
    std::vector<std::string> injectArgvs_;
    std::map<std::string, InjectFunction> injectFuns_;
    std::map<std::string, int32_t> sendEventType_;
    std::vector<DeviceInformation> allDevices_ = {};
    std::string GetFileExtendName(const std::string& strName);
    int64_t GetFileSize(const std::string strFileName);
private:
    static constexpr uint32_t SEND_EVENT_ARGV_COUNTS = 5;
    static constexpr uint32_t SEND_EVENT_DEV_NODE_INDEX = 1;
    static constexpr uint32_t SEND_EVENT_TYPE_INDEX = 2;
    static constexpr uint32_t SEND_EVENT_CODE_INDEX = 3;
    static constexpr uint32_t SEND_EVENT_VALUE_INDEX = 4;
    static constexpr int32_t ARGVS_TARGET_INDEX = 1;
    static constexpr int32_t ARGVS_CODE_INDEX = 2;
    static constexpr int32_t SEND_EVENT_TO_DEVICE = 0;
    static constexpr int32_t JSON_FILE_PATH_INDEX = 1;
    static constexpr int64_t JSON_FILE_SIZE = 2097152;
};
} // namespace MMI
} // namespace OHOS
#endif // INJECTION_EVENT_DISPATCH_H