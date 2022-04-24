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
#include "entrust_tasks.h"

#include <sys/syscall.h>
#include <unistd.h>

std::string GetThisThreadIdOfString2()
{
    thread_local std::string threadLocalId;
    if (threadLocalId.empty()) {
        long tid = syscall(SYS_gettid);
        constexpr size_t bufSize = 10;
        char buf[bufSize] = {};
        const int32_t ret = sprintf(buf, "%06d", tid);
        if (ret < 0) {
            printf("ERR: in %s, #%d, call sprintf_s fail, ret = %d.\n", __func__, __LINE__, ret);
            return threadLocalId;
        }
        buf[bufSize - 1] = '\0';
        threadLocalId = buf;
    }

    return threadLocalId;
}

uint64_t GetThisThreadId2()
{
    std::string stid = GetThisThreadIdOfString2();
    auto tid = std::stoull(stid);
    return tid;
}

namespace OHOS {
namespace MMI {
bool EntrustTasks::Task::WaitFor(int32_t ms)
{
    std::chrono::milliseconds span(ms);
    auto res = future_.wait_for(span);
    if (res == std::future_status::timeout) {
        printf("Task timeout id:%d\n", id_);
    }
    hasWaited_ = true;
    return (res == std::future_status::ready);
}

void EntrustTasks::Task::ProcessTask()
{
    if (hasNotified_) {
        return;
    }
    fun_();
    promise_.set_value();
    hasNotified_ = true;
}

bool EntrustTasks::Init()
{
    int32_t res = pipe(fds_);
    if (res == -1) {
        printf("pipe create error:%d\n", errno);
        return false;
    }
    return true;
}

void EntrustTasks::ProcessTasks(int32_t stid)
{
    auto tid = GetThisThreadId2();
    printf("ProcessTasks this tid:%d stid:%d\n", tid, stid);
    std::lock_guard<std::mutex> guard(mux_);
    if (tasks_.empty()) {
        return;
    }

    int32_t count = 0;
    for (auto it = tasks_.begin(); it != tasks_.end(); count++) {
        if (count > ET_ONCE_PROCESS_TASK_LIMIT) {
            break;
        }
        auto task = *it;
        if (task->HasReady()) {
            RecoveryId(task->GetId());
            it = tasks_.erase(it);
        } else {
            task->ProcessTask();
            it++;
        }
    }
}

bool EntrustTasks::PostSyncTask(ETaskCallback callback, int32_t timeout)
{
    auto task = PostTask(callback);
    if (task == nullptr) {
        printf("Post task failed\n");
        return false;
    }
    return task->WaitFor(timeout);
}


bool EntrustTasks::PostAsyncTask(ETaskCallback callback)
{
    auto task = PostTask(callback, true);
    if (task == nullptr) {
        printf("Post task failed\n");
        return false;
    }
    return true;
}

EntrustTasks::TaskPtr EntrustTasks::PostTask(ETaskCallback callback, bool asyncTask)
{
    std::lock_guard<std::mutex> guard(mux_);
    auto tsize = tasks_.size();
    if (tsize > ET_MAX_TASK_LIMIT) {
        return nullptr;
    }
    int32_t id = GenerateId();
    TaskData data = {GetThisThreadId2(), id};
    auto res = write(fds_[1], &data, sizeof(data));
    if (res == -1) {
        RecoveryId(id);
        printf("write error:%d\n", errno);
        return nullptr;
    }
    auto task = std::make_shared<Task>(id, callback, asyncTask);
    tasks_.push_back(task);
    return task->GetPtr();
}
} // namespace MMI
} // namespace OHOS