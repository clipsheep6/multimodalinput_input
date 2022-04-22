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
#include <future>
#include <functional>
#include <iterator>
#include <queue>
#include <memory>
#include <mutex>
#include <vector>

#include "id_factory.h"

namespace OHOS {
namespace MMI {
using ETaskCallback = std::function<void()>;
using Promise = std::promise<void>;
using Future = std::future<void>;
#define ET_DEFINE_TIMEOUT 3000
#define ET_MAX_TASK_LIMIT 1000
class EntrustTasks : public IdFactroy<int32_t> {
public:
    class Task : public std::enable_shared_from_this<Task> {
    public:
        using TaskPtr = std::shared_ptr<EntrustTasks::Task>;
        using TaskVec = std::vector<TaskPtr>;
        using TaskVecIter = TaskVec::iterator;
        Task(ETaskCallback fun);
        ~Task() = default;
        void ProcessTask();

        Future& GetFuture()
        {
            return future_;
        }
        TaskPtr GetPtr()
        {
            return shared_from_this();
        }
        void Timeout()
        {
            isTimeout_ = true;
        }
    private:
        std::atomic_bool isTimeout_ = false;
        ETaskCallback fun_;
        Promise promise_;
        Future future_ = promise_.get_future();
    };
    using TaskPtr = Task::TaskPtr;
    using TaskVec = Task::TaskVec;
    using TaskVecIter = TaskVec::iterator;
    
public:
    EntrustTasks();
    virtual ~EntrustTasks();

    bool Init();
    void ProcessTasks();
    bool PostSyncTask(ETaskCallback callback, int32_t timeout = ET_DEFINE_TIMEOUT);
    bool PostSyncTaskEx(ETaskCallback callback, int32_t timeout = ET_DEFINE_TIMEOUT);
    bool PostAsyncTask(ETaskCallback callback);

    int32_t GetReadFd() const
    {
        return fds_[0];
    }

private:
    TaskPtr PostTask(ETaskCallback callback);
    TaskPtr PostTaskEx(ETaskCallback callback);
    void DelTask(int32_t id);

private:
    int32_t fds_[2] = {};
    std::mutex mux_;
    std::queue<TaskPtr> tasks_;
    TaskVec vecTasks_;
};
} // namespace MMI
} // namespace OHOS
#endif // ENTRUST_TASKS_H