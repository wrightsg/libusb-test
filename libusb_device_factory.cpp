#include "libusb_device_factory.h"

#include <cassert>

#include <stdexcept>

namespace libusb {

device_factory::device_factory ()
{
    const auto status = libusb_init(&_context);
    if (status != LIBUSB_SUCCESS) {
        throw std::runtime_error(libusb_error_name(status));
    }
}

device_factory::~device_factory () noexcept
{
    assert(_context);

    libusb_exit(_context);
}

std::vector<device> device_factory::get_devices ()
{
    libusb_device** device_list;
    const auto num_devices = libusb_get_device_list(_context, &device_list);

    std::vector<device> devices;
    for (int n = 0; n < num_devices; n++) {
        try {
            devices.push_back(device(device_list[n]));
        } catch (const std::exception& e) {
            // Do not care for now
        }

    }

    libusb_free_device_list(device_list, 1);

    return devices;
}

std::vector<device> get_devices (std::uint16_t vendor_id, std::uint16_t product_id)
{
    return {};
}

}
