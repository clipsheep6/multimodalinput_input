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

#include "error_multimodal.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EntrustTasks" };
} // namespace

void EntrustTasks::Task::ProcessTask()
{
    CALL_LOG_ENTER2;
    LOGFMTD("process task Begin id:%d", id_);
    MMI_HILOGD("process task Begin id:%{public}d", id_);
    int32_t ret = fun_();
    LOGFMTD("process task End id:%d ret:%d", id_, ret);
    MMI_HILOGD("process task End id:%{public}d ret:%{public}d", id_, ret);
    if (promise_) {
        promise_->set_value(ret);
        LOGFMTD("process task set_value id:%d ret:%d", id_, ret);
        MMI_HILOGD("process task set_value id:%{public}d ret:%{public}d", id_, ret);
    }
}

bool EntrustTasks::Init()
{
    CALL_LOG_ENTER2;
    int32_t res = pipe(fds_);
    if (res == -1) {
        LOGFMTE("pipe create error:%d", errno);
        MMI_HILOGE("pipe create error:%{public}d", errno);
        return false;
    }
    return true;
}

void EntrustTasks::ProcessTasks(uint64_t stid, int32_t pid)
{
    CALL_LOG_ENTER2;
    auto tid = GetThisThreadId();
    LOGFMTD("tid:%" PRId64 " stid:%" PRId64 " pid:%d", tid, stid, pid);
    MMI_HILOGD("tid:%{public}" PRId64 " stid:%{public}" PRId64 " pid:%{public}d", tid, stid, pid);
    std::lock_guard<std::mutex> guard(mux_);
    int32_t count = 0;
    while (!tasks_.empty() && ((count++) < ET_ONCE_PROCESS_TASK_LIMIT)) {
        auto task = tasks_.front();
        CHKPB(task);
        task->ProcessTask();
        RecoveryId(task->GetId());
        tasks_.pop();
    }
}

int32_t EntrustTasks::PostSyncTask(int32_t pid, ETaskCallback callback, int32_t timeout)
{
    CALL_LOG_ENTER2;
    Promise promise;
    Future future = promise.get_future();
    auto ret = PostTask(pid, callback, &promise);
    if (ret != RET_OK) {
        LOGFMTE("Post aync task failed");
        MMI_HILOGE("Post aync task failed");
        return ret;
    }
    std::chrono::milliseconds span(timeout);
    auto res = future.wait_for(span);
    if (res == std::future_status::timeout) {
        LOGFMTW("Task timeout pid:%d", pid);
        MMI_HILOGW("Task timeout pid:%{public}d", pid);
        return ETASKS_WAIT_TIMEOUT;
    } else if (res == std::future_status::deferred) {
        LOGFMTW("Task deferred pid:%d", pid);
        MMI_HILOGW("Task deferred pid:%{public}d", pid);
        return ETASKS_WAIT_DEFERRED;
    }
    ret = future.get();
    return ret;
}

bool EntrustTasks::PostAsyncTask(int32_t pid, ETaskCallback callback)
{
    CALL_LOG_ENTER2;
    auto ret = PostTask(pid, callback);
    if (ret != RET_OK) {
        LOGFMTE("Post aync task failed");
        MMI_HILOGE("Post aync task failed");
        return false;
    }
    LOGFMTD("Post async task pid:%d", pid);
    MMI_HILOGD("Post async task pid:%{public}d", pid);
    return true;
}

int32_t EntrustTasks::PostTask(int32_t pid, ETaskCallback callback, Promise *promise)
{
    CALL_LOG_ENTER2;
    std::lock_guard<std::mutex> guard(mux_);
    auto tsize = tasks_.size();
    if (tsize > ET_MAX_TASK_LIMIT) {
        LOGFMTE("Queue is full, not allowed. size:%zu/%d", tsize, ET_MAX_TASK_LIMIT);
        MMI_HILOGE("Queue is full, not allowed. size:%{public}zu/%{public}d", tsize, ET_MAX_TASK_LIMIT);
        return ETASKS_QUEUE_FULL;
    }
    int32_t id = GenerateId();
    TaskData data = {GetThisThreadId(), pid, id};
    auto res = write(fds_[1], &data, sizeof(data));
    if (res == -1) {
        RecoveryId(id);
        LOGFMTE("write error:%d", errno);
        MMI_HILOGE("write error:%{public}d", errno);
        return ETASKS_PIPE_WAITE_FAIL;
    }
    tasks_.push(std::make_shared<Task>(id, callback, promise));
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS