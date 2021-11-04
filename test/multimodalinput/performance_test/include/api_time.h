#include <time.h>

namespace OHOS {
class Statistics {
public:
    timespec GetCurTime();
    double GetCallApiTime(timespec time_start,timespec time_end);
    struct timespec time_start = { 0, 0 }, time_end = { 0, 0 };
};
} // namespace
const double CALLAPITIME = 10;
const int CALLAPINUM = 1000;
