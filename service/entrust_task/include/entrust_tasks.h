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
#ifndef ENTRUST_TASKS_H
#define ENTRUST_TASKS_H
#include <cinttypes>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>

#include "id_factory.h"

namespace OHOS {
namespace MMI {
using ETaskCallback = std::function<int32_t()>;
class EntrustTasks : public IdFactory<int32_t> {
public:
    struct TaskData {
        uint64_t tid;
        int32_t taskId;
    };
    class Task : public std::enable_shared_from_this<Task> {
    public:
        using Promise = std::promise<int32_t>;
        using Future = std::future<int32_t>;
        using TaskPtr = std::shared_ptr<EntrustTasks::Task>;
        Task(int32_t id, ETaskCallback fun, Promise *promise = nullptr)
            : id_(id), fun_(fun), promise_(promise) {}
        ~Task() = default;
        void ProcessTask();

        int32_t GetId() const
        {
            return id_;
        }
        TaskPtr GetSharedPtr()
        {
            return shared_from_this();
        }
        void SetWaited()
        {
            hasWaited_ = true;
        }
        bool HasNotified() const
        {
            return hasNotified_;
        }

    private:
        std::atomic_bool hasNotified_ = false;
        std::atomic_bool hasWaited_ = false;
        int32_t id_ = 0;
        ETaskCallback fun_;
        Promise* promise_ = nullptr;
    };
    using TaskPtr = Task::TaskPtr;
    using Promise = Task::Promise;
    using Future = Task::Future;
    
public:
    EntrustTasks() = default;
    virtual ~EntrustTasks() = default;

    bool Init();
    void ProcessTasks();
    int32_t PostSyncTask(ETaskCallback callback);
    bool PostAsyncTask(ETaskCallback callback);

    int32_t GetReadFd() const
    {
        return fds_[0];
    }

private:
    void PopPendingTaskList(std::vector<TaskPtr> &tasks);
    TaskPtr PostTask(ETaskCallback callback, Promise *promise = nullptr);

private:
    int32_t fds_[2] = {};
    std::mutex mux_;
    std::queue<TaskPtr> tasks_;
};
} // namespace MMI
} // namespace OHOS
#endif // ENTRUST_TASKS_H