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
#include "log.h"
#include <cstdio> 
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <string>
#include <cerrno>
#include <algorithm>
#include "securec.h"

#ifdef OHOS_BUILD_MMI_DEBUG

namespace OHOS {
namespace MMI {
const static std::string LOG_CONFIG_PATH = "path";
const static std::string LOG_CONFIG_LEVEL = "level";
const static std::string LOG_CONFIG_DISPLAY = "display";
const static std::string LOG_CONFIG_LIMIT_SIZE = "limitsize";
const static std::string LOG_CONFIG_FILELINE = "fileline";

const static std::string LOG_LEVEL_TRACE = "trace";
const static std::string LOG_LEVEL_DEBUG = "debug";
const static std::string LOG_LEVEL_INFO = "info";
const static std::string LOG_LEVEL_WARN = "warn";
const static std::string LOG_LEVEL_ERROR = "error";
const static std::string LOG_LEVEL_ALARM = "alarm";
const static std::string LOG_LEVEL_FATAL = "fatal";

const static std::string LOG_CONFIG_CLOSE = "false";

const static std::string LOG_COLORS[] = {
    "\e[1;37m", // WHITE
    "\e[1;37m", // WHITE
    "\e[0;34m", // BLUE
    "\e[1;33m", // YELLOW
    "\e[0;31m", // RED
    "\e[1;32m", // L_GREEN
    "\e[1;35m"  // L_PURPLE
};

const static std::string LOG_LEVELSTR[] = {
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "ALARM",
    "FATAL"
};

constexpr static int32_t SECONDE_UNIT = 1000;
constexpr static int32_t USLEEP_DELAY = 50000;
constexpr static int32_t AD_1900 = 1900;
constexpr static int32_t PERCENT = 100;
constexpr static int32_t TIME_OUT = 3000;
constexpr static int32_t RATE_PARA = 2;
constexpr static int32_t ONE_MILLION = 1024 * 1024;

static std::string GetFileName(const std::string& strPath)
{
    size_t nPos = strPath.find_last_of('/');
    if (strPath.npos == nPos)
        nPos = strPath.find_last_of('\\');
    if (strPath.npos == nPos)
        return strPath;
    return strPath.substr(nPos + 1, strPath.npos);
}

static std::string ExtIgnore(const std::string& str)
{
    auto text = str;
    size_t n = str.find_last_not_of(" \r\n\t");
    if (n != std::string::npos) {
        text.erase(n + 1, text.size() - n);
    }
    n = str.find_first_not_of(" \r\n\t");
    if (n != std::string::npos) {
        text.erase(0, n);
    }
    return text;
}

static std::pair<std::string, std::string> SplitString(const std::string& str, const std::string& separator)
{
    auto inStr = str;
    auto last = std::remove(inStr.begin(), inStr.end(), ' ');
    inStr.erase(last, inStr.end());
    std::string::size_type pos = inStr.find(separator.c_str());
    if (pos == std::string::npos) {
        return std::make_pair(inStr, "");
    }
    return std::make_pair(inStr.substr(0, pos), inStr.substr(pos + separator.length(), inStr.length()));
}

static std::string Strftime(const std::string& format = "%F %T", time_t curTime = 0)
{
    if (curTime == 0) {
        curTime = time(nullptr);
    }
    auto t = localtime(&curTime);
    char longTime[LOG_MAX_TIME_LEN] = "";
    size_t ret = strftime(longTime, sizeof(longTime), format.c_str(), t);
    if (ret == 0) {
        return longTime;
    }
    return longTime;
}

#define LOGLOG(fmt, ...) \
    std::string __curtm = Strftime(); \
    printf("%s ", __curtm.c_str()); \
    printf(fmt, ##__VA_ARGS__); \
    printf("\n");

bool LogManager::SemCreate(int32_t count)
{
    if (isCreate_) {
        LOGLOG("LogManager::SemCreate ok");
        return true;
    }
    if (count < 0) {
        count = 0;
    }

    if (sem_init(&semId_, 0, count) != 0) {
        LOGLOG("LogManager::SemCreate sem_init error");
        return false;
    }

    isCreate_ = true;
    return true;
}

bool LogManager::SemWait(int32_t timeout)
{
    if (timeout <= 0) {
        return (sem_wait(&semId_) == 0);
    } else {
        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
            LOGLOG("LogManager::SemWait clock_gettime");
        }
        ts.tv_sec += timeout;
        int s = 0;
        while ((s = sem_timedwait(&semId_, &ts)) == -1 && errno == EINTR) {
            continue;
        }

        if (s == -1) {
            if (errno == ETIMEDOUT) {
                LOGLOG("LogManager::SemWait sem_timedwait() timedout");
                return false;
            } else {
                LOGLOG("LogManager::SemWait sem_timedwait error");
                return false;
            }
        } else {
            LOGLOG("LogManager::SemWait sem_timedwait()succeeded");
            return true;
        }

        return false;
    }
    return true;
}

bool LogManager::SemPost(void)
{
    return (sem_post(&semId_) == 0);
}

bool LogManager::Wait(void)
{
    t_.join();
    return true;
}

void LogManager::WriteFile(LogDataPtr pLog)
{
    if (!pLog) {
        LOGLOG("WriteFile pLog = nullptr");
        return;
    }
    // 时间
    char longTime[LOG_MAX_TIME_LEN] = {};
    struct tm* p = localtime(&pLog->curTime);
    int32_t ret = -1;
    ret = snprintf_s(longTime, sizeof(longTime), LOG_MAX_TIME_LEN, "%02d-%02d-%02d %02d:%02d:%02d.%03d",
               p->tm_year + AD_1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, pLog->precise);
    if (ret < 0) {
        return;
    }
    f_.Write(longTime, strlen(longTime));
    f_.Write(" ", LOG_CHAR_LEN);
    // 进程
    f_.Write("[", LOG_CHAR_LEN);
    f_.Write(proId_.c_str(), proId_.length());
    f_.Write("]", LOG_CHAR_LEN);
    f_.Write(" ", LOG_CHAR_LEN);
    // 级别
    f_.Write("[", LOG_CHAR_LEN);
    f_.Write(LOG_LEVELSTR[pLog->level].c_str(), LOG_LEVELSTR[pLog->level].size());
    f_.Write("]", LOG_CHAR_LEN);
    f_.Write(" ", LOG_CHAR_LEN);
    // 内容
    f_.Write(pLog->data.c_str(), pLog->data.length());
    f_.Write(" ", LOG_CHAR_LEN);
    // 文件
    f_.Write("(", LOG_CHAR_LEN);
    f_.Write(pLog->curFile.c_str(), pLog->curFile.length());
    f_.Write(")", LOG_CHAR_LEN);
    // 行数
    if (logFileline_) {
        char longNum[LOG_MAX_NUM_LEN] = {0};
        snprintf_s(longNum, sizeof(longNum), LOG_MAX_NUM_LEN, "%d", pLog->curLine);
        f_.Write(":", LOG_CHAR_LEN);
        f_.Write(longNum, strlen(longNum));
    }
    // 行尾换行符
    f_.Write("\n", LOG_CHAR_LEN);

    // 控制台输出
    if (logDisplay_) {
        auto tmpStr = GetFileName(pLog->curFile);
        if (logFileline_) {
            printf("%s%s [%s] [%s] %s %s:%d\e[0m\n", LOG_COLORS[pLog->level].c_str(), longTime, proId_.c_str(),
                   LOG_LEVELSTR[pLog->level].c_str(), pLog->data.c_str(), tmpStr.c_str(), pLog->curLine);
        } else {
            printf("%s%s [%s] [%s] %s\e[0m\n", LOG_COLORS[pLog->level].c_str(), longTime, proId_.c_str(),
                   LOG_LEVELSTR[pLog->level].c_str(), pLog->data.c_str());
        }
    }
}

LogManager::LogManager()
{
    runing_ = false;
    proId_ = "0";
    proName_ = "process";
    isCreate_ = false;
    logLevel_ = LL_TRACE;
    limitSize_ = LOG_MAX_FILE_SIZE;
    logDisplay_ = true;
    logFileline_ = false;

    pthread_mutexattr_t attr = {};
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&mutex_, &attr);
    pthread_mutexattr_destroy(&attr);

    GetProcessInfo();
}

LogManager::~LogManager()
{
    Stop();
    pthread_mutex_destroy(&mutex_);

    if (isCreate_) {
        isCreate_ = false;
        sem_destroy(&semId_);
    }
}

bool LogManager::SetLogName(const std::string& name)
{
    if (name.empty()) {
        LOGLOG("LogManager::SetLogName error, name is empty");
        return false;
    }
    proFileName_ = name;
    return true;
}

bool LogManager::SetLogPath(const std::string& path)
{
    if (path.empty()) {
        LOGLOG("LogManager::SetLogPath error, path is empty");
        return false;
    }

    filePath_ = path;
    char ch = filePath_.at(filePath_.length() - 1);
    if ((ch != '\\') && (ch != '/')) {
        filePath_.append("/");
    }
    return true;
}

// 获取进程ID和进程名
void LogManager::GetProcessInfo(void)
{
    char buf[LOG_MAX_LINE_LEN] = {0};
    if (sprintf_s(buf, LOG_MAX_LINE_LEN, "%06d", getpid()) == -1) {
        LOGLOG("GetProcessInfo sprintf_s pid error");
        return;
    }
    proId_ = buf;
    if (sprintf_s(buf, LOG_MAX_LINE_LEN, "/proc/%d/cmdline", static_cast<int32_t>(getpid())) == -1) {
        LOGLOG("GetProcessInfo sprintf_s /proc/.../cmdline error");
        return;
    }
    FileHandle i;
    i.Open(buf, "rb");
    if (!i.IsOpen()) {
        LOGLOG("LogManager::GetProcessInfo load log file error. filename=%s", buf);
        return;
    }
    proName_ = i.ReadLine();
    i.Close();
    proName_ = GetFileName(proName_);
}

bool LogManager::OpenFileHandle()
{
    if (fileName_.empty()) {
        return false;
    }
    // 打开日志句柄
    f_.Open(fileName_.c_str(), "wb+");
    if (!f_.IsOpen()) {
        LOGLOG("LogManager::CreateFile load log file error. filename=%s", fileName_.c_str());
        return false;
    }
    return true;
}

bool LogManager::CreateFile()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    fileCreateTime_ = tv.tv_sec;
    struct tm* p = nullptr;
    p = localtime(&fileCreateTime_);
    char logFileTime[LOG_MAX_TIME_LEN] = {0};
    int32_t ret = -1;
    ret = snprintf_s(logFileTime, sizeof(logFileTime), LOG_MAX_TIME_LEN, "_%02d%02d%02d%02d%02d%02d",
                     p->tm_year + AD_1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    if (ret < 0) {
        LOGLOG("LogManager::CreateFile get logFileTime error.");
        return false;
    }

    std::string fileTime = logFileTime;
    fileName_ = filePath_ + proFileName_ + fileTime + ".log";
    return OpenFileHandle();
}

void LogManager::ParseLogLevel(const std::string& str)
{
    auto level = logLevel_;
    if (str == LOG_LEVEL_TRACE) {
        logLevel_ = LL_TRACE;
    } else if (str == LOG_LEVEL_DEBUG) {
        logLevel_ = LL_DEBUG;
    } else if (str == LOG_LEVEL_INFO) {
        logLevel_ = LL_INFO;
    } else if (str == LOG_LEVEL_WARN) {
        logLevel_ = LL_WARN;
    } else if (str == LOG_LEVEL_ERROR) {
        logLevel_ = LL_ERROR;
    } else if (str == LOG_LEVEL_ALARM) {
        logLevel_ = LL_ALARM;
    } else if (str == LOG_LEVEL_FATAL) {
        logLevel_ = LL_FATAL;
    } else { // do nothing 
    }
    if (level != logLevel_) {
        LOGLOG("The log level will be modified. The old is '%s' and the new is '%s'",
               LOG_LEVELSTR[level].c_str(), LOG_LEVELSTR[logLevel_].c_str());
    }
}

void LogManager::ParseLogDisplay(const std::string& str)
{
    auto display = logDisplay_;
    if (str == LOG_CONFIG_CLOSE) {
        logDisplay_ = false;
    } else {
        logDisplay_ = true;
    }
    if (display != logDisplay_) {
        LOGLOG("The log DisplayStatus will be modified. The old is %d and the new is %d",
               display, logDisplay_);
    }
}

void LogManager::ParseLogLimitSize(const std::string& str)
{
    auto size = limitSize_;
    auto limit = atoi(str.c_str());
    limit = limit <= 0 ? 10 : limit;
    limit = limit > 1024 ? 1024 : limit;
    limitSize_ = limit * ONE_MILLION;
    if (LOG_MAX_FILE_SIZE != size && size != limitSize_) {
        LOGLOG("The log LimitSize will be modified. The old is %d and the new is %d",
               size, limitSize_);
    }
}

void LogManager::ParseLogFileLine(const std::string& str)
{
    auto fileLine = logFileline_;
    if (str == LOG_CONFIG_CLOSE) {
        logFileline_ = false;
    } else {
        logFileline_ = true;
    }
    if (fileLine != logFileline_) {
        LOGLOG("The log ShowFileLineStatus will be modified. The old is %d and the new is %d",
               fileLine, logFileline_);
    }
}

void LogManager::ParseConfig(const FileHandle& f)
{
    std::string line = f.ReadLine();
    while (!line.empty()) {
        std::string kvLine = ExtIgnore(line);
        std::pair<std::string, std::string> kv = SplitString(kvLine, "=");
        std::string keyConfig = kv.first;
        std::string keyValue = kv.second;
        if (keyValue.empty()) {
            line = f.ReadLine();
            continue;
        }

        if (keyConfig == LOG_CONFIG_PATH) {
            if (!filePath_.empty() && filePath_ != keyValue) {
                LOGLOG("The log path will be modified. old is '%s' and new is '%s'",
                       filePath_.c_str(), keyValue.c_str());
            }
            filePath_ = keyValue;
            SetLogPath(filePath_.c_str());
            if (access(filePath_.c_str(), 0) != 0) {
                char cmdStr[LOG_MAX_CMD_LEN] = {0};
                if (sprintf_s(cmdStr, LOG_MAX_CMD_LEN, "mkdir -p %s", filePath_.c_str()) == -1) {
                    LOGLOG("filePath mkdir %s sprintf_s error", filePath_.c_str());
                    return;
                }
                system(cmdStr);
            }
        } else if (keyConfig == LOG_CONFIG_LEVEL) {
            ParseLogLevel(keyValue);
        } else if (keyConfig == LOG_CONFIG_DISPLAY) {
            ParseLogDisplay(keyValue);
        } else if (keyConfig == LOG_CONFIG_LIMIT_SIZE) {
            ParseLogLimitSize(keyValue);
        } else if (keyConfig == LOG_CONFIG_FILELINE) {
            ParseLogFileLine(keyValue);
        } else { // do nothing 
        }

        line = f.ReadLine();
    }
}

void LogManager::LoadConfig()
{
    FileHandle f;
    f.Open(configPath_.c_str(), "r");
    if (!f.IsOpen()) {
        LOGLOG("LogManager::LoadConfig open file error. filename=%s", configPath_.c_str());
        return;
    }
    ParseConfig(f);
    f.Close();
}

// 初始化
bool LogManager::Init(const std::string& configPath)
{
    LOGLOG("LogManager::Init starting! %d", 11221);
    if (!filePath_.empty()) {
        LOGLOG("LogManager::init error: file=%s, the new config file=%s", filePath_.c_str(), configPath_.c_str());
        return false;
    }
    if (!configPath.data()) {
        LOGLOG("LogManager::init error: configPath == null");
        return false;
    }
    if (access(configPath.c_str(), 0) != 0) {
        LOGLOG("LogManager::init error: configPath is not exsit, configPath = %s", configPath.c_str());
        return false;
    }
    configPath_.append(configPath);
    LoadConfig();

    // 设置日志路径，如果目录不存在，创建目录
    if (filePath_.empty()) {
        LOGLOG("LogManager::init use default logpath = ./logs \n");
        filePath_ = "./logs";
    }
    SetLogPath(filePath_.c_str());
    char cmdStr[LOG_MAX_CMD_LEN] = {0};
    if (access(filePath_.c_str(), 0) != 0) {
        if (sprintf_s(cmdStr, LOG_MAX_CMD_LEN, "mkdir -p %s", filePath_.c_str()) == -1) {
            LOGLOG("LogManager::Init sprintf_s error cmd: mkdir -p %s", filePath_.c_str());
            return false;
        }
        system(cmdStr);
    }
    // 设置日志文件名
    SetLogName(proName_.c_str());
    return CreateFile();
}

bool LogManager::Start(void)
{
    LOGLOG("LogManager::start log thread starting!");
    if (runing_) {
        LOGLOG("log thread started!");
        return false;
    }
    if (configPath_.empty()) {
        LOGLOG("LogManager::Start init error, start error");
        return false;
    }
    SemCreate(0);
    t_ = std::thread(std::bind(&LogManager::OnThread, this));
    return SemWait(TIME_OUT);
}

bool LogManager::Stop(void)
{
    if (runing_) {
        LOGLOG("LogManager::stop log thread stoping");
        runing_ = false;
        Wait();
        while (!logs_.empty()) {
            logs_.pop();
        }
        return true;
    }
    return false;
}

bool LogManager::PushLog(LogDataPtr pLog)
{
    if (!runing_ || (pLog == nullptr)) {
        if (pLog != nullptr) {
            LOGLOG("LogManager is not runing. pLog data:%s", pLog->data.c_str());
        } else {
            LOGLOG("LogManager:: pLog == NULL");
        }
        
        return false;
    }

    pthread_mutex_lock(&mutex_);
    logs_.push(pLog);
    pthread_mutex_unlock(&mutex_);
    return true;
}

const std::vector<char> LogManager::RemovePrivacyIdentifierInFmt(const std::string& format)
{
    const size_t formatLen = format.size();

    std::vector<char> format2;
    format2.reserve(formatLen + 1);
    
    size_t readPos = 0;
    size_t writePos = 0;
    while (readPos < formatLen) {
        if (formatLen - readPos >= 9) {
            const char c1 = format.at(readPos);
            const char c2 = format.at(readPos + 1);
            const char c3 = format.at(readPos + 2);
            const char c4 = format.at(readPos + 3);
            const char c5 = format.at(readPos + 4);
            const char c6 = format.at(readPos + 5);
            const char c7 = format.at(readPos + 6);
            const char c8 = format.at(readPos + 7);
            const char c9 = format.at(readPos + 8);
            if (c1 == '%' && c2 == '{' && c3 == 'p' && c4 == 'u' && c5 == 'b' && c6 == 'l' && c7 == 'i' && c8 == 'c' && c9 == '}') {
                readPos += 9;
                format2.push_back('%');
                ++writePos;
                continue;
            }
        }

        if (formatLen - readPos >= 10) {
            const char c1 = format.at(readPos);
            const char c2 = format.at(readPos + 1);
            const char c3 = format.at(readPos + 2);
            const char c4 = format.at(readPos + 3);
            const char c5 = format.at(readPos + 4);
            const char c6 = format.at(readPos + 5);
            const char c7 = format.at(readPos + 6);
            const char c8 = format.at(readPos + 7);
            const char c9 = format.at(readPos + 8);
            const char c10 = format.at(readPos + 9);
            if (c1 == '%' && c2 == '{' && c3 == 'p' && c4 == 'r' && c5 == 'i' && c6 == 'v' && c7 == 'a' && c8 == 't' && c9 == 'e' && c10 == '}') {
                readPos += 10;
                format2.push_back('%');
                ++writePos;
                continue;
            }
        }

        format2.push_back(format[readPos++]);
        ++writePos;
    }

    while (readPos < formatLen) {
        format2.push_back(format[readPos++]);
        ++writePos;
    };

    format2.push_back('\0');

    return format2;
}

bool LogManager::PushString(const int32_t level, const std::string& file, const int32_t line, const std::string& buf)
{
    if (buf.empty() || file.empty()) {
        LOGLOG("LogManager::PushString buf == NULL or file == NULL");
        return false;
    }

    // 日志限流
    int32_t count = static_cast<int32_t>(logs_.size());
    if (count > LOG_QUEUE_LIMIT_SIZE) {
        int32_t rate = (count - LOG_QUEUE_LIMIT_SIZE) * PERCENT / LOG_QUEUE_LIMIT_SIZE;
        if (rate > PERCENT) {
            rate = PERCENT;
        }
        if (rate > PERCENT / RATE_PARA) {
            LOGLOG("LogManager::PushString log count limit count : %d", count);
            return true;
        }
    }

    LogDataPtr pLog(new LogData());
    if (pLog == nullptr) {
        LOGLOG("LogManager::PushString new LogData error");
        return false;
    }

    struct timeval tv;
    gettimeofday(&tv, nullptr);

    pLog->curTime = tv.tv_sec;
    pLog->precise = uint32_t(tv.tv_usec / SECONDE_UNIT);
    pLog->level = level;
    pLog->curFile = file;
    pLog->curLine = line;
    pLog->data = buf;

    PushLog(pLog);
    return true;
}

bool LogManager::PushFormat(const int32_t level, const std::string& file, const int32_t line, std::string fm, ...)
{
    if (logLevel_ > level) {
        return true;
    }

    if (file.empty()) {
        LOGLOG("LogManager::PushFormat file is empty");
        return false;
    }

    const std::vector<char> newFormat = RemovePrivacyIdentifierInFmt(fm);

    char buf[LOG_MAX_BUF_LEN] = {};
    va_list args;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"
    va_start(args, &newFormat[0]);
#pragma GCC diagnostic pop
    if (vsnprintf_s(buf, LOG_MAX_BUF_LEN, LOG_MAX_BUF_LEN, &newFormat[0], args) == -1) {
        LOGLOG("LogManager::PushFormat vsnprintf_s error");
        va_end(args);
        return false;
    }
    va_end(args);

    if (!PushString(level, file, line, std::string(buf))) {
        LOGLOG("LogManager::PushFormat PushString error");
        return false;
    }
    return true;
}

LogManager::LogDataPtr LogManager::PopLog()
{
    if (logs_.empty()) {
        return nullptr;
    }

    pthread_mutex_lock(&mutex_);
    auto log = logs_.front();
    logs_.pop();
    pthread_mutex_unlock(&mutex_);
    return log;
}

void LogManager::UpdateFile(void)
{
    LoadConfig();
    if (f_.IsOpen()) {
        if (f_.FileSize() > limitSize_) {
            f_.Close();
            CreateFile();
        }
    }
}

void LogManager::OnThread(void)
{
    runing_ = true;
    LOGLOG("LogManager::run log thread started!");

    SemPost();
    int32_t count = 0;
    LogDataPtr pLog = nullptr;
    while (true) {
        count = 0;
        while ((pLog = PopLog()) != nullptr) {
            if (!f_.IsOpen()) {
                if (!OpenFileHandle()) {
                    LOGLOG("LogManager::run file is not Open and open %s failed", fileName_.c_str());
                    PushLog(pLog);
                    break;
                }
            }
            if (f_.IsOpen()) {
                WriteFile(pLog);
                count += 1;
            }
        }
        if (count > 0 && f_.IsOpen()) {
            f_.Flush();
        }
        UpdateFile();

        // 延时，单位微秒
        usleep(USLEEP_DELAY);
        // quit
        if (!runing_) {
            break;
        }
    }
}
}
}

#endif // OHOS_BUILD_MMI_DEBUG
