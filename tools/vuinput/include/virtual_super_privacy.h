#ifndef VIRTUAL_SUPER_PRIVACY_H
#define VIRTUAL_SUPER_PRIVACY_H

#include "virtual_device.h"

namespace OHOS {
namespace MMI {
class VirtualSuperPrivacy : public VirtualDevice {
public:
    VirtualSuperPrivacy();
    DISALLOW_COPY_AND_MOVE(VirtualSuperPrivacy);
    ~VirtualSuperPrivacy() = default;
};
} // namespace MMI
} // namespace OHOS
#endif // VIRTUAL_SUPER_PRIVACY_H