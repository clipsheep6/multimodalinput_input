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

#include <unistd.h>

namespace OHOS {
namespace MMI {
EntrustTasks::Task::Task(ETaskCallback fun) : fun_(fun)
{

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

void EntrustTasks::Task::ProcessTask()
{
    if (isTimeout_) {
        return;
    }
    fun_();
    promise_.set_value();
}

EntrustTasks::EntrustTasks()
{

}
EntrustTasks::~EntrustTasks()
{

}

void EntrustTasks::ProcessTasks()
{
    std::lock_guard<std::mutex> guard(mux_);
    if (tasks_.empty()) {
        return;
    }
    for (int32_t i = 0; i < 10; i++) {
        auto task = tasks_.front();
        if (task) {
            task->ProcessTask();
            tasks_.pop();
        }
    }
}

bool EntrustTasks::PostSyncTask(ETaskCallback callback, int32_t timeout)
{
    auto task = PostTask(callback);
    if (task == nullptr) {
        return false;
    }
    std::chrono::milliseconds span(timeout);
    auto res = task->GetFuture().wait_for(span);
    if (res == std::future_status::timeout) {
        task->Timeout();
        return false;
    }
    return true;
}

bool EntrustTasks::PostSyncTaskEx(ETaskCallback callback, int32_t timeout)
{
    auto task = PostTask(callback);
    if (task == nullptr) {
        return false;
    }
    std::chrono::milliseconds span(timeout);
    auto res = task->GetFuture().wait_for(span);
    if (res == std::future_status::timeout) {
        task->Timeout();
        return false;
    }
    return true;
}

bool EntrustTasks::PostAsyncTask(ETaskCallback callback)
{

    return true;
}

EntrustTasks::TaskPtr EntrustTasks::PostTask(ETaskCallback callback)
{
    std::lock_guard<std::mutex> guard(mux_);
    auto tsize = tasks_.size();
    if (tsize > ET_MAX_TASK_LIMIT) {
        return nullptr;
    }
    auto res = write(fds_[1], reinterpret_cast<void*>(tsize), sizeof(tsize));
    if (res == -1) {
        printf("write error:%d\n", errno);
        return nullptr;
    }
    auto task = std::make_shared<Task>(callback);
    tasks_.push(task);
    return task->GetPtr();
}

EntrustTasks::TaskPtr EntrustTasks::PostTaskEx(ETaskCallback callback)
{
    std::lock_guard<std::mutex> guard(mux_);
    auto tsize = vecTasks_.size();
    if (tsize > ET_MAX_TASK_LIMIT) {
        return nullptr;
    }
    auto res = write(fds_[1], reinterpret_cast<void *>(tsize), sizeof(tsize));
    if (res == -1) {
        printf("write error:%d\n", errno);
        return nullptr;
    }
    auto task = std::make_shared<Task>(callback);
    vecTasks_.push_back(task);
    return task->GetPtr();
}

void EntrustTasks::DelTask(int32_t id)
{
    std::lock_guard<std::mutex> guard(mux_);
}

} // namespace MMI
} // namespace OHOS