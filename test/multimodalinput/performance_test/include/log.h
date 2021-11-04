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
#ifndef OHOS_LOG_H
#define OHOS_LOG_H

#include <string>
#include "hilog/log.h"

namespace OHOS {
    namespace MMI {
        namespace {
            constexpr uint32_t MMI_LOG_DOMAIN = 0xD002800;
        }
    }
}

#ifdef OHOS_BUILD_MMI_DEBUG
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include <atomic>
#include <sys/select.h>
#include <iostream>
#include <map>
#include "refbase.h"

namespace OHOS {
namespace MMI {
// 宏定义日志文件名
#define LOG_MAX_BUF_LEN (8*1024)
#define LOG_MAX_CMD_LEN 520
#define LOG_MAX_TIME_LEN 100
#define LOG_MAX_LINE_LEN 500
#define LOG_MAX_NUM_LEN 16
#define LOG_CHAR_LEN 1
#define LOG_MAX_FILE_SIZE (10*1024*1024)
#define LOG_MAX_SAVE_DAY 7
#define LOG_QUEUE_LIMIT_SIZE 20000

// 日志级别
enum ENUM_LL {
    LL_TRACE = 0,
    LL_DEBUG = 1,
    LL_INFO = 2,
    LL_WARN = 3,
    LL_ERROR = 4,
    LL_ALARM = 5,
    LL_FATAL = 6,
};

class FileHandle {
public:
    FileHandle()
    {
        fp_ = nullptr;
    }

    ~FileHandle()
    {
        Close();
    }

    bool IsOpen(void)
    {
        return fp_ != nullptr;
    }

    size_t Open(const std::string& path, const std::string& option)
    {
        if (fp_ != nullptr) {
            fclose(fp_);
            fp_ = nullptr;
        }

        fp_ = fopen(path.c_str(), option.c_str());
        if (fp_) {
            auto cur = ftell(fp_);
            fseek(fp_, 0, SEEK_END);
            auto len = (size_t)ftell(fp_);
            fseek(fp_, cur, SEEK_SET);
            return len;
        }
        return 0;
    }

    void Close(void)
    {
        if (!fp_) {
            return;
        }
        Flush();
        fclose(fp_);
        fp_ = nullptr;
    }

    size_t FileSize()
    {
        long len = 0;
        if (fp_) {
            len = ftell(fp_);
        }
        return (size_t)len;
    }

    void Write(const std::string& data, size_t len)
    {
        if (fp_ && len > 0) {
            if (fwrite(data.c_str(), 1, len, fp_) != len) {
                Close();
            }
        }
    }

    void Flush(void)
    {
        if (fp_) {
            fflush(fp_);
        }
    }

    std::string ReadLine(void) const
    {
        char buf[LOG_MAX_LINE_LEN] = {0};
        if (fp_ && (fgets(buf, LOG_MAX_LINE_LEN, fp_) != NULL)) {
            return buf;
        }
        return std::string();
    }

protected:
    FILE* fp_ = nullptr;
};

    // ---日志信息
struct LogData : public RefBase {
    time_t curTime;       // 时间
    uint32_t precise;     // 毫秒
    std::string curFile;  // 所在文件名
    int32_t curLine;      // 所在文件中行数
    int32_t level;        // 级别
    std::string data;     // 内容

    LogData()
    {
        curTime = 0;
        precise = 0;
        curLine = 0;
        level = 0;
    }
};

class LogManager {
public:
    LogManager();
    virtual ~LogManager();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    static LogManager& GetInstance()
    {
        static LogManager instanceLog;
        return instanceLog;
    }

    using LogDataPtr = sptr<LogData>;
    bool Init(const std::string& configPath);
    bool Start(void);
    bool Stop(void);

    void OnThread(void);

    bool PushString(const int32_t level, const std::string& file, const int32_t line, const std::string& buf);
    bool PushFormat(const int32_t level, const std::string& file, const int32_t line, std::string fm, ...);

    bool IsRuning() const
    {
        return runing_;
    }

protected:
    bool OpenFileHandle(void);
    bool CreateFile(void);
    void UpdateFile(void);
    void LoadConfig(void);
    void ParseConfig(const FileHandle& f);
    void GetProcessInfo(void);
    bool SetLogName(const std::string& name);
    bool SetLogPath(const std::string& path);
    LogDataPtr PopLog();
    bool PushLog(LogDataPtr pLog);
    const std::vector<char> RemovePrivacyIdentifierInFmt(const std::string &fmt);

    void ParseLogLevel(const std::string& str);
    void ParseLogDisplay(const std::string& str);
    void ParseLogLimitSize(const std::string& str);
    void ParseLogFileLine(const std::string& str);

    bool Wait(void);
    void WriteFile(LogDataPtr pLog);

    bool SemCreate(int32_t count);
    bool SemWait(int32_t timeout);
    bool SemPost(void);

private:
    // 线程状态
    std::atomic<bool> runing_;
    std::thread t_;
    std::mutex mu_;
    sem_t semId_;
    pthread_mutex_t mutex_;

    int32_t logLevel_ = LL_TRACE;
    bool logDisplay_ = true;
    bool logFileline_ = false;
    bool isCreate_ = true;

    // 进程信息--放入主函数
    std::string proId_;
    std::string proName_;
    std::string configPath_;

    // 日志文件信息
    FileHandle f_;            // 文件句柄
    std::string fileName_;    // 当前日志文件名
    std::string proFileName_; // 进程文件名
    std::string filePath_;    // 文件路径
    time_t fileCreateTime_;   // 文件创建时间
    size_t limitSize_;        // 文件限制最大值

    // 日志队列
    std::queue<LogDataPtr> logs_;
};
} // namespace MMI
} // namespace OHOS

#define MMI_LOGT(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Debug(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGD(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Debug(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGI(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Info(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGW(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Warn(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGE(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Error(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGA(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_ALARM, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Error(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#define MMI_LOGF(fmt, ...) {\
    OHOS::MMI::LogManager::GetInstance().PushFormat(OHOS::MMI::LL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__);\
    OHOS::HiviewDFX::HiLog::Fatal(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__);\
}

#else
#define MMI_LOGT(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Debug(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGD(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Debug(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGI(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Info(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGW(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Warn(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGE(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Error(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGA(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Error(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define MMI_LOGF(fmt, ...) \
    OHOS::HiviewDFX::HiLog::Fatal(LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)


#endif // OHOS_BUILD_MMI_DEBUG

#endif // OHOS_LOG_H
