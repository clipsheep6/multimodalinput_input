#pragma once
#include <cstdint>
#include <cstddef>

namespace Input {

constexpr int INVALID_FD = -1;

struct NonCopyable {
    NonCopyable() = default;
    virtual ~NonCopyable() = default;;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
};

#define CASE_STR(item) case item: do {return #item;} while(0)

inline constexpr size_t LongsOfBits(int32_t bitsCount)  {
    return (bitsCount / (sizeof(long) * 8)) + !!(bitsCount % (sizeof(long) * 8));
}


#define LENTH_OF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))

}


