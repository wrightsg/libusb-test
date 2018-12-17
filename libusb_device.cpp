#include "libusb_device.h"

#include <stdexcept>

namespace libusb {

device::device (libusb_device* device)
{
    // TODO Is it necesary for the device to be opened? Some functions
    // TODO can also be performed without the device handle.

    // Note: libusb_open() automatically increment the reference count
    // of the libusb_device passed to libusb_open().
    const auto status = libusb_open(device, &_handle);
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
}

device::device (device&& other)
    : _handle(other._handle)
{
    other._handle = nullptr;
}

device::~device () noexcept
{
    // Note: libusb_close() will automatically decrement the reference
    // count of the libusb_device which was passed with libusb_open()
    if (_handle) {
        libusb_close(_handle);
    }
}

device::speed device::get_speed ()
{
    const auto speed = libusb_get_device_speed(get_device());
    switch (speed)
    {
    case LIBUSB_SPEED_LOW: return speed::LOW;
    case LIBUSB_SPEED_FULL: return speed::FULL;
    case LIBUSB_SPEED_HIGH: return speed::HIGH;
    case LIBUSB_SPEED_SUPER: return speed::SUPER;
    case LIBUSB_SPEED_UNKNOWN: return speed::UNKNOWN;
    // TODO Better way to handle this?
    default: throw std::logic_error("unexpected value");
    }
}

device_descriptor device::get_descriptor ()
{
    libusb_device_descriptor descriptor;
    const auto status = libusb_get_device_descriptor(get_device(), &descriptor);
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
    return device_descriptor(descriptor);
}

libusb_device* device::get_device ()
{
    return libusb_get_device(_handle);
}


std::ostream& operator<< (std::ostream& os, device::speed speed)
{
    switch (speed)
    {
    case device::speed::LOW: os << "LOW"; break;
    case device::speed::FULL: os << "FULL"; break;
    case device::speed::HIGH: os << "HIGH"; break;
    case device::speed::SUPER: os << "SUPER"; break;
    case device::speed::UNKNOWN: os << "UNKNOWN"; break;
    }
    return os;
}

}
