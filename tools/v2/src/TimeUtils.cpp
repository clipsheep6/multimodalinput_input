#include <chrono>

#include "TimeUtils.h"

namespace Input {

    int64_t TimeUtils::GetTimeStampMs() {
        auto time = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    }

    int64_t TimeUtils::GetMonotonicTimeMs() {
        auto time = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    }
}
