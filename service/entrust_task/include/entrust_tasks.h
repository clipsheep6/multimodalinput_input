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
#include <atomic>
#include <cinttypes>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>

#include "id_factory.h"

namespace OHOS {
namespace MMI {
using ETaskCallback = std::function<void()>;
#define ET_DEFINE_TIMEOUT 3000
#define ET_MAX_TASK_LIMIT 1000
#define ET_ONCE_PROCESS_TASK_LIMIT 10
class EntrustTasks : public IdFactroy<int32_t> {
    using Promise = std::promise<void>;
    using Future = std::future<void>;
public:
    struct TaskData {
        uint64_t tid;
        int32_t taskId;
    };
    class Task : public std::enable_shared_from_this<Task> {
    public:
        using TaskPtr = std::shared_ptr<EntrustTasks::Task>;
        Task(int32_t id, ETaskCallback fun, bool asyncTask = false)
            : id_(id), hasWaited_(asyncTask), fun_(fun) {}
        ~Task() = default;

        bool WaitFor(int32_t ms);
        void ProcessTask();

        int32_t GetId() const
        {
            return id_;
        }
        TaskPtr GetPtr()
        {
            return shared_from_this();
        }
        bool HasReady() const
        {
            return (hasNotified_ && hasWaited_);
        }

    private:
        int32_t id_ = 0;
        std::atomic_bool hasNotified_ = false;
        std::atomic_bool hasWaited_ = false;
        ETaskCallback fun_;
        Promise promise_;
        Future future_ = promise_.get_future();
    };
    using TaskPtr = Task::TaskPtr;
    
public:
    EntrustTasks() = default;
    virtual ~EntrustTasks() = default;

    bool Init();
    void ProcessTasks(uint64_t stid = 0);
    bool PostSyncTask(ETaskCallback callback, int32_t timeout = ET_DEFINE_TIMEOUT);
    bool PostAsyncTask(ETaskCallback callback);

    int32_t GetReadFd() const
    {
        return fds_[0];
    }

private:
    TaskPtr PostTask(ETaskCallback callback, bool asyncTask = false);

private:
    int32_t fds_[2] = {};
    std::mutex mux_;
    std::list<TaskPtr> tasks_;
};
} // namespace MMI
} // namespace OHOS
#endif // ENTRUST_TASKS_H