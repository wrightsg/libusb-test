#ifndef _LIBUSB_DEVICE_DESCRIPTOR_H_
#define _LIBUSB_DEVICE_DESCRIPTOR_H_

#include <cstdint>

#include <libusb-1.0/libusb.h>

namespace libusb {

class device_descriptor final
{
public:
    explicit device_descriptor (const libusb_device_descriptor& descriptor) noexcept;

    std::uint16_t vendor_id () const noexcept;
    std::uint16_t product_id () const noexcept;

private:
    libusb_device_descriptor _descriptor;
};

}

#endif // _LIBUSB_DEVICE_DESCRIPTOR_H_
