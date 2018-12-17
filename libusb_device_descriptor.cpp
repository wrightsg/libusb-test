#include "libusb_device_descriptor.h"

namespace libusb {

device_descriptor::device_descriptor (const libusb_device_descriptor& descriptor) noexcept
    : _descriptor(descriptor)
{
}

std::uint16_t device_descriptor::vendor_id () const noexcept
{
    return _descriptor.idVendor;
}

std::uint16_t device_descriptor::product_id () const noexcept
{
    return _descriptor.idProduct;
}

}
