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

#ifndef ERROR_MULTIMODAL_H
#define ERROR_MULTIMODAL_H

#include <errors.h>

namespace OHOS {
namespace MMI {
inline constexpr int32_t ERROR_UNSUPPORT = -2;
inline constexpr int32_t INPUT_CHECK_MONITOR_FAIL = -201;

enum {
    MODULE_COMMON = 100,  // 公共错误码范围:100-199
    MODULE_MSG    = 200,  // 消息处理错误码范围:200-599
    MODULE_EVENT  = 600   // 业务错误码范围:600-10000
};

enum {
    // 空指针
    INPUT_COMMON_NULLPTR = MODULE_COMMON,

    // 参数错误
    INPUT_COMMON_PARAM_ERROR,
    // 非预期值
    INPUT_COMMON_UNEXPECTED_VALUE,
    // 无效FD
    INPUT_COMMON_INVALID_FD,

    // 消息发送失败
    INPUT_MSG_SEND_FAIL = MODULE_MSG,
    // 消息包读失败
    INPUT_MSG_PACKET_READ_FAIL,
    // 消息包写失败
    INPUT_MSG_PACKET_WRITE_FAIL,

    // 事件注册失败
    INPUT_REG_EVENT_FAIL = MODULE_EVENT,

    // APL鉴权失败
    INPUT_CHECK_PERMISSION_FAIL,
    // 读取配置文件失败
    INPUT_READ_FILE_FAIL,

    // SA_Service初始化错误
    INPUT_INIT_SASERVICE_FAIL,
    // libinput初始化失败
    INPUT_INIT_LIBINPUT_FAIL,
    // 代理任务启动失败
    INPUT_INIT_ETASKS_FAIL,
    // 初始化画鼠标失败
    INPUT_INIT_POINTER_DRAW_FAIL,
    // 多模服务未启动
    INPUT_SERVICE_NOT_RUNNING,
    // Epoll创建失败
    INPUT_INIT_EPOLL_CREATE_FAIL,

    // 委托任务wait超时
    INPUT_ETASKS_WAIT_TIMEOUT,
    // 委托任务wait延期
    INPUT_ETASKS_WAIT_DEFERRED,
    // 生成同步任务失败
    INPUT_ETASKS_POST_SYNCTASK_FAIL,
    // 生成异步任务失败
    INPUT_ETASKS_POST_ASYNCTASK_FAIL,

    // 键盘事件封装失败
    INPUT_KEY_EVENT_PKG_FAIL,

    // 业务错误码最大值
    INPUT_ERROR_CODE_END
};

} // namespace MMI
} // namespace OHOS
#endif // ERROR_MULTIMODAL_H