#include "api_time.h"

namespace OHOS {
timespec Statistics::GetCurTime()
{
    //获取当前时间
    struct timespec time_current = {0,0};
    clock_gettime(CLOCK_REALTIME,&time_current);
    return time_current;
}

double Statistics::GetCallApiTime(struct timespec time_start, struct timespec time_end) {
    double time_total;
    time_total = ((time_end.tv_sec - time_start.tv_sec) * 1000000000 + (time_end.tv_nsec - time_start.tv_nsec)) / 1000;
    double time_total_us = time_total / 1000;
    // float f_time_total = time_total_us;
    return time_total_us;
}
} // namespace