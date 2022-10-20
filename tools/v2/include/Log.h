#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <ios>

namespace Input {

class Log {
    public:
        enum {LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR, LEVEL_FALT, LEVEL_NONE};

        static int SetLogLevel(int newLogLevel);
        static int GetLogLevel();
        static const char* LogLevelToStr(int logLevel);

        template <typename... ARGS> static void Print(int logLevel, const char* prettyFunc, int32_t line, const char* fmt, const ARGS&... args) {
            if (logLevel < GetLogLevel()) {
                return;
            }

            if (fmt == nullptr || fmt[0] == 0) {
                return;
            }

            std::stringstream outStream;
            AppendPrefix(outStream, logLevel, prettyFunc, line);
            AppendFmt(outStream, fmt, args ...);
            outStream << '\n';
            Print(outStream.str());
        }

        static void Print(const std::string& msg);
        
    private:
        enum class Holder {None, Str, Hex};

    private:
        static void AppendFmt(std::stringstream& outStream, const char*& fmt);
        static Holder AppendNextPart(std::stringstream& outStream, const char*& fmt);

        template <typename T, typename... ARGS> 
        static void AppendFmt(std::stringstream& outStream, const char*& fmt, const T& arg, const ARGS&... args) {
            AppendFmtItem(outStream, fmt, arg);
            AppendFmt(outStream, fmt, args ...);
        }

        template <typename T> 
        static void AppendFmtItem(std::stringstream& outStream, const char*& fmt, const T& arg) {
            while (fmt[0] != 0) {
                auto holder = AppendNextPart(outStream, fmt);
                switch (holder) {
                    case Holder::None:
                        break;
                    case Holder::Str:
                        outStream << arg;
                        return;
                    case Holder::Hex:
                        outStream << std::hex << arg << std::dec;
                        return;
                    default:
                        return;
                }
            }
        }

        static void ReadNextPart(const char*& fmt, const char* & partEndPos, Holder& holder);

        static void AppendPrefix(std::stringstream& outStream, int logLevel, const char* prettyFunc, int line);
};

}

#define LOG_D(FMT, ...) do { \
    if (::Input::Log::LEVEL_DEBUG < ::Input::Log::GetLogLevel()) { \
        break;\
    }\
    ::Input::Log::Print(::Input::Log::LEVEL_DEBUG, __PRETTY_FUNCTION__, __LINE__, FMT, ##__VA_ARGS__);\
} while(0)

#define LOG_I(FMT, ...) do { \
    if (::Input::Log::LEVEL_INFO < ::Input::Log::GetLogLevel()) { \
        break;\
    }\
    ::Input::Log::Print(::Input::Log::LEVEL_INFO, __PRETTY_FUNCTION__, __LINE__, FMT, ##__VA_ARGS__);\
} while(0)

#define LOG_W(FMT, ...) do { \
    if (::Input::Log::LEVEL_WARN < ::Input::Log::GetLogLevel()) { \
        break;\
    }\
    ::Input::Log::Print(::Input::Log::LEVEL_WARN, __PRETTY_FUNCTION__, __LINE__, FMT, ##__VA_ARGS__);\
} while(0)

#define LOG_E(FMT, ...) do { \
    if (::Input::Log::LEVEL_ERROR < ::Input::Log::GetLogLevel()) { \
        break;\
    }\
    ::Input::Log::Print(::Input::Log::LEVEL_ERROR, __PRETTY_FUNCTION__, __LINE__, FMT, ##__VA_ARGS__);\
} while(0)

#define LOG_F(FMT, ...) do { \
    if (::Input::Log::LEVEL_FALT < ::Input::Log::GetLogLevel()) { \
        break;\
    }\
    ::Input::Log::Print(::Input::Log::LEVEL_FALT, __PRETTY_FUNCTION__, __LINE__, FMT, ##__VA_ARGS__);\
} while(0)


#define LOG_NERVER()  LOG_E("SHOULD NERVER COME HERE!")






