#pragma once

namespace Input {

    namespace Utils {
        template <typename T> bool UpdateValue(T& dst, const T& src) {
            if (dst == src) {
                return false;
            }

            dst = src;
            return true;
        }

        template <typename T> void MakeInRange(T& value, const T& minValue, const T& maxValue) {
            if (value < minValue) {
                value = minValue;
            } else if (value > maxValue) {
                value = maxValue;
            }
        }

        inline int64_t Combine(int32_t valueA, int32_t valueB) {
            return (((int64_t)valueA) << 32) | (((int64_t)valueB) << 0);
        }
    };

}
