#include <sstream>
#include <iostream>
#include <ctime>
#include <map>
#include <functional>
#include <cstring>
#include <cctype>
#include <chrono>
#include <iomanip>

#include <sys/types.h>
#include <unistd.h>

#include "Log.h"

namespace Input {

static int s_logLevel = Log::LEVEL_DEBUG;
//static int s_logLevel = Log::LEVEL_FALT;

int Log::SetLogLevel(int newLogLevel) {
    int preLogLevel = s_logLevel;
    s_logLevel = newLogLevel;
    if (s_logLevel < Log::LEVEL_DEBUG) {
        s_logLevel = Log::LEVEL_DEBUG;
    } else if (s_logLevel > Log::LEVEL_NONE) {
        s_logLevel = Log::LEVEL_NONE;
    }
    return preLogLevel;
}

int Log::GetLogLevel() {
    return s_logLevel;
}

const char* Log::LogLevelToStr(int logLevel) {
    int fixedLogLevel = logLevel < LEVEL_DEBUG ? LEVEL_DEBUG : (logLevel > LEVEL_NONE ? LEVEL_NONE : logLevel);
    switch (fixedLogLevel) {
        case LEVEL_DEBUG:
            return "[D]";
        case LEVEL_INFO:
            return "[I]";
        case LEVEL_WARN:
            return "[W]";
        case LEVEL_ERROR:
            return "[E]";
        case LEVEL_FALT:
            return "[F]";
        default:
            return "[X]";
    }
}

void Log::Print(const std::string& msg) {
    std::cout << msg << std::flush;
}

void Log::ReadNextPart(const char*& fmt, const char* & partEndPos, Log::Holder& holder) {
    partEndPos = nullptr;
    holder = Log::Holder::None;

    while (*fmt != '\0') {
        while (*fmt != '\0') {
            char ch = *fmt++;
            if (ch == '$') {
                break;
            }
        }

        if (*fmt == '\0') {
            partEndPos = fmt;
            break;
        }

        if (*fmt == '$') {
            partEndPos = fmt;
            ++fmt;
            break;
        }

        partEndPos = fmt - 1;
        if (*fmt == 'x') {
            ++fmt;
            holder = Log::Holder::Hex;
            break;
        }

        ++fmt;
        holder = Log::Holder::Str;
        break;
    }
}

static void FindPurePrettyFunc(const char* prettyFunc, const char* & startPos, const char*& endPos) {
    const char* unknownFunc = "(null)";
    if (prettyFunc == nullptr) {
        startPos = unknownFunc;
        endPos = startPos + strlen(unknownFunc);
        return;
    }

    startPos = prettyFunc;
    for (const char* cur = prettyFunc; *cur != 0; ++cur) {
        if (std::isspace(*cur)) {
            startPos = nullptr;
            continue;
        }

        if (startPos == nullptr) {
            startPos = cur;
            continue;
        }

        if (*cur == '(') {
            endPos = cur;
            break;
        }
    }

    if (startPos == nullptr || endPos == nullptr) {
        startPos = nullptr;
        endPos = nullptr;
    }
}

void Log::AppendPrefix(std::stringstream& outStream, int logLevel, const char* prettyFunc, int line) {
    auto nowTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    outStream << std::boolalpha << std::showbase << std::uppercase
        << LogLevelToStr(logLevel)
        << ' ' << std::put_time(std::localtime(&nowTime), "%F %T")
        << ' ' << getpid()
        << ' ' << gettid()
        << ' ';

    const char* startPos = nullptr;
    const char* endPos = nullptr;
    FindPurePrettyFunc(prettyFunc, startPos, endPos);
    outStream.write(startPos, endPos - startPos);

    outStream << " #" << line << ' ';
}

void Log::AppendFmt(std::stringstream& outStream, const char*& fmt) {
    if (fmt[0] == '\0') {
        return;
    }
    while (fmt[0] != '\0') {
        const char* startPos = fmt;
        const char* endPos = nullptr;
        Holder holder = Holder::None;
        ReadNextPart(fmt, endPos, holder);
        if (endPos != nullptr) {
            outStream.write(startPos, endPos - startPos);
        }

        switch (holder) {
            case Holder::None:
                break;
            case Holder::Str:
            case Holder::Hex:
                outStream << "[MISS]";
                break;
            default:
                return;
        }
    }
}

Log::Holder Log::AppendNextPart(std::stringstream& outStream, const char*& fmt) {
    const char* startPos = fmt;
    const char* endPos = nullptr;
    Holder holder = Holder::None;
    ReadNextPart(fmt, endPos, holder);
    if (endPos != nullptr) {
        outStream.write(startPos, endPos - startPos);
    }

    return holder;
}


}

