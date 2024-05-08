#ifndef VIRTUAL_LID_H
#define VIRTUAL_LID_H

#include "virtual_device.h"

namespace OHOS {
namespace MMI {
class VirtualLid : public VirtualDevice {
public:
    VirtualLid();
    DISALLOW_COPY_AND_MOVE(VirtualLid);
    ~VirtualLid() = default;
};
} // namespace MMI
} // namespace OHOS
#endif // VIRTUAL_LID_H