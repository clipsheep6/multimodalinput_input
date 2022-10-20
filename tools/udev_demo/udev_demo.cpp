#include <stdio.h>
#include "libudev.h"

void test_device(struct udev *udev, const char *syspath)
{
    struct udev_device *device;
    printf("looking at device: %s\n", syspath);
    device = udev_device_new_from_syspath(udev, syspath);
    if (device == NULL) {
        printf("no device found\n");
        return;
    }
    printf("*** device: %p ***\n", device);
    const char *str = udev_device_get_devnode(device);
    if (str != NULL) {
        printf("devname:   '%s'\n", str);
    }
    dev_t devnum = udev_device_get_devnum(device);
    if (major(devnum) > 0) {
        printf("devnum:    %u:%u\n", major(devnum), minor(devnum));
    }
    udev_device_unref(device);
}

int main(int argc, char *argv[])
{
    struct udev *udev = udev_new();
    printf("udev context: %p\n", udev);
    if (udev == NULL) {
        printf("no context\n");
        return 1;
    }
    const char *path = "/sys/devices/virtual/mem/null";
    test_device(udev, path);
    udev_unref(udev);
    return 0;
}