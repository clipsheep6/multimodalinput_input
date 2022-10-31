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

#include "stack_dumper_helper.h"

#include "dfx_dump_catcher.h"
#include "dfx_dump_res.h"

std::tuple<bool, std::string> OHOS::MMI::StackDumperHelper::Dump()
{
    OHOS::HiviewDFX::DfxDumpCatcher dumplog;
    std::string msg = "";
    bool ret = dumplog.DumpCatch(getpid(), gettid(), msg);
    return { ret, msg };
}