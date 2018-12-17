#ifndef _LIBUSB_DEVICE_FACTORY_H_
#define _LIBUSB_DEVICE_FACTORY_H_

#include <cstdint>

#include <vector>

#include <libusb-1.0/libusb.h>

#include "libusb_device.h"

namespace libusb {

class device_factory
{
public:
    device_factory ();
    ~device_factory () noexcept;

    std::vector<device> get_devices ();
    std::vector<device> get_devices (std::uint16_t vendor_id, std::uint16_t product_id);

private:
    libusb_context* _context = nullptr;
};

}

#endif // _LIBUSB_DEVICE_FACTORY_H_
