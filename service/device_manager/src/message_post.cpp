/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifdef OHOS_WESTEN_MODEL
#include "message_post.h"
#include "uds_socket.h"

namespace OHOS {
namespace MMI {
void MessagePost::SetFd(int fd)
{
    fd_ = fd;
}

void MessagePost::RunOnWestonThread(std::function<void(weston_compositor *)> taskItem)
{
    MMI_LOGT("enter");
    {
        std::lock_guard<std::mutex> guard(lk_);
        asyncTasks_.push_back(taskItem);
    }
    NotifyWestonThread();
}

void MessagePost::NotifyWestonThread()
{
    MMI_LOGT("enter");
    if (fd_ == -1) {
        return;
    }
    int32_t value = 0;
    write(fd_, &value, sizeof(value));
}

void MessagePost::RunTasks()
{
    MMI_LOGT("enter");
    while (true) {
        std::function<void(weston_compositor *)> taskItem;
        {
            std::lock_guard<std::mutex> guard(lk_);
            if (asyncTasks_.empty()) {
                return;
            }
            taskItem = asyncTasks_.front();
            asyncTasks_.pop_front();
        }
        taskItem(ec_);
    }
}

int MessagePost::RunTaskOnWestonThread(int fd, uint32_t mask, void *data)
{
    MMI_LOGT("enter");
    int32_t value = 0;
    read(fd, &value, sizeof(value));
    MMIMsgPost.RunTasks();
    return 0;
}

void MessagePost::SetWestonCompositor(weston_compositor *ec)
{
    MMI_LOGT("enter");
    ec_ = ec;
}
} // namespace MMI
} // namespace OHOS

#endif // OHOS_WESTEN_MODEL
