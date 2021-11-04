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

#include <signal.h>
#include "libmmi_util.h"
#include "st_data_process.h"
#include "st_helper.h"
#include "st_client_helper.h"

namespace {
using namespace OHOS;
using namespace OHOS::MMI;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIClientDemo" };

static std::atomic_bool g_clientExit(false);
StClientHelper g_stClientHelper;
void RunClient()
{    
    MMI_LOGT("RunClient enter.");
    g_stClientHelper.StartStClient();

    while (!g_clientExit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    MMI_LOGT("RunClient leave.");
}

void ClientStopHandler(int signalNo)
{
    MMI_LOGI("client will exit! %{public}d\n", signalNo);
    g_clientExit = true;
}
}

int main(int argc, char* argv[])
{
#ifdef OHOS_BUILD_MMI_DEBUG
    VerifyLogManagerRun();
#endif
    const sighandler_t ret = signal(SIGTERM, ClientStopHandler);
    if (ret == SIG_ERR) {
        MMI_LOGT("signal ret SIG_ERR.");
        return RET_ERR;
    }

    RunClient();

    while (!g_clientExit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    MMI_LOGT("hosmmi-st-client exit.");

    return 0;
}

