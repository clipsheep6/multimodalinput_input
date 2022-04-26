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

#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EntrustTasks" };
} // namespace

bool EntrustTasks::Task::WaitFor(int32_t ms)
{
    std::chrono::milliseconds span(ms);
    auto res = future_.wait_for(span);
    if (res == std::future_status::timeout) {
        MMI_HILOGW("Task timeout id:%{public}d", id_);
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
        MMI_HILOGE("pipe create error:%{public}d", errno);
        return false;
    }
    return true;
}

void EntrustTasks::ProcessTasks(uint64_t stid)
{
    auto tid = GetThisThreadId();
    MMI_HILOGD("tid:%{public}" PRId64 " stid:%{public}" PRId64 "", tid, stid);
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
        MMI_HILOGE("Post aync task failed");
        return false;
    }
    return task->WaitFor(timeout);
}

bool EntrustTasks::PostAsyncTask(ETaskCallback callback)
{
    auto task = PostTask(callback, true);
    if (task == nullptr) {
        MMI_HILOGE("Post async task failed");
        return false;
    }
    return true;
}

EntrustTasks::TaskPtr EntrustTasks::PostTask(ETaskCallback callback, bool asyncTask)
{
    std::lock_guard<std::mutex> guard(mux_);
    auto tsize = tasks_.size();
    if (tsize > ET_MAX_TASK_LIMIT) {
        MMI_HILOGE("Queue is full, not allowed. size:%{public}zu/%{public}d", tsize, ET_MAX_TASK_LIMIT);
        return nullptr;
    }
    int32_t id = GenerateId();
    TaskData data = {GetThisThreadId(), id};
    auto res = write(fds_[1], &data, sizeof(data));
    if (res == -1) {
        RecoveryId(id);
        MMI_HILOGE("write error:%{public}d", errno);
        return nullptr;
    }
    auto task = std::make_shared<Task>(id, callback, asyncTask);
    tasks_.push_back(task);
    return task->GetPtr();
}
} // namespace MMI
} // namespace OHOS