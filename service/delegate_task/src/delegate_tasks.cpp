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
#include "delegate_tasks.h"

#include <sys/syscall.h>
#include <unistd.h>

#include "error_multimodal.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DelegateTasks" };
} // namespace

void DelegateTasks::Task::ProcessTask()
{
    CALL_LOG_ENTER;
    if (hasWaited_) {
        MMI_HILOGE("Expired tasks will be discarded. id:%{public}d", id_);
        return;
    }
    int32_t ret = fun_();
    std::string taskType = ((promise_ == nullptr) ? "Async" : "Sync");
    MMI_HILOGD("process %{public}s task id:%{public}d,ret:%{public}d", taskType.c_str(), id_, ret);
    if (promise_ != nullptr) {
        promise_->set_value(ret);
    }
}

bool DelegateTasks::Init()
{
    CALL_LOG_ENTER;
    int32_t res = pipe(fds_);
    if (res == -1) {
        MMI_HILOGE("pipe create failed,errno:%{public}d", errno);
        return false;
    }
    return true;
}

void DelegateTasks::ProcessTasks()
{
    CALL_LOG_ENTER;
    std::vector<TaskPtr> tasks;
    PopPendingTaskList(tasks);
    for (const auto &it : tasks) {
        it->ProcessTask();
        RecoveryId(it->GetId());
    }
}

int32_t DelegateTasks::PostSyncTask(DTaskCallback callback)
{
    CALL_LOG_ENTER;
    if (IsCallFromWorkerThread()) {
        return callback();
    }
    Promise promise;
    Future future = promise.get_future();
    auto task = PostTask(callback, &promise);
    if (task == nullptr) {
        MMI_HILOGE("post sync task faild");
        return ETASKS_POST_SYNCTASK_FAIL;
    }

    static constexpr int32_t timeout = 3000;
    std::chrono::milliseconds span(timeout);
    auto res = future.wait_for(span);
    task->SetWaited();
    if (res == std::future_status::timeout) {
        MMI_HILOGE("Task timeout");
        return ETASKS_WAIT_TIMEOUT;
    } else if (res == std::future_status::deferred) {
        MMI_HILOGE("Task deferred");
        return ETASKS_WAIT_DEFERRED;
    }
    return future.get();
}

int32_t DelegateTasks::PostAsyncTask(DTaskCallback callback)
{
    if (IsCallFromWorkerThread()) {
        return callback();
    }
    auto task = PostTask(callback);
    if (task == nullptr) {
        MMI_HILOGE("post async task faild");
        return ETASKS_POST_ASYNCTASK_FAIL;
    }
    return RET_OK;
}

void DelegateTasks::PopPendingTaskList(std::vector<TaskPtr> &tasks)
{
    std::lock_guard<std::mutex> guard(mux_);
    int32_t count = 0;
    static constexpr int32_t onceProcessTaskLimit = 10;
    while (!tasks_.empty() && ((count++) < onceProcessTaskLimit)) {
        auto task = tasks_.front();
        CHKPB(task);
        tasks.push_back(task->GetSharedPtr());
        tasks_.pop();
    }
}

DelegateTasks::TaskPtr DelegateTasks::PostTask(DTaskCallback callback, Promise *promise)
{
    if (IsCallFromWorkerThread()) {
        MMI_HILOGE("This interface cannot be called from a worker thread.");
        return nullptr;
    }
    std::lock_guard<std::mutex> guard(mux_);
    static constexpr int32_t maxTasksLimit = 1000;
    auto tsize = tasks_.size();
    if (tsize > maxTasksLimit) {
        MMI_HILOGE("The task queue is full. size:%{public}zu/%{public}d", tsize, maxTasksLimit);
        return nullptr;
    }
    int32_t id = GenerateId();
    TaskData data = {GetThisThreadId(), id};
    auto res = write(fds_[1], &data, sizeof(data));
    if (res == -1) {
        RecoveryId(id);
        MMI_HILOGE("pipe write faild,errno:%{public}d", errno);
        return nullptr;
    }
    TaskPtr task = std::make_shared<Task>(id, callback, promise);
    if (task == nullptr) {
        RecoveryId(id);
        MMI_HILOGE("make task faild");
        return nullptr;
    }
    tasks_.push(task);
    std::string taskType = ((promise == nullptr) ? "Async" : "Sync");
    MMI_HILOGD("post %{public}s task id:%{public}d,tid:%{public}" PRIu64 "", taskType.c_str(), id, data.tid);
    return task->GetSharedPtr();
}
} // namespace MMI
} // namespace OHOS