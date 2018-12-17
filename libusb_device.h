#ifndef _LIBUSB_DEVICE_H_
#define _LIBUSB_DEVICE_H_

#include <ostream>

#include <libusb-1.0/libusb.h>

#include "libusb_device_descriptor.h"

namespace libusb {

class device final
{
public:
    explicit device (libusb_device* device);
    device (device&& other);
    ~device () noexcept;

    device (const device& other) = delete;
    device& operator= (const device& other) = delete;
    device& operator= (device&& other) = delete;

    enum class speed
    {
        LOW, FULL, HIGH, SUPER, UNKNOWN
    };
    speed get_speed ();

    device_descriptor get_descriptor ();

private:
    libusb_device_handle* _handle = nullptr;

    libusb_device* get_device ();
};

std::ostream& operator<< (std::ostream& os, device::speed speed);

}

#endif // _LIBUSB_DEVICE_H_
