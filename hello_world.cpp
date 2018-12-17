#include <iomanip>
#include <iostream>

#include "libusb_device_factory.h"

int main (int argc, char** argv)
{
    libusb::device_factory device_factory;

    auto devices = device_factory.get_devices();
    std::cout << "found " << devices.size() << " devices that could be opened" << "\n";

    for (auto& device : devices) {
        std::cout << "device with " << device.get_speed() << " speed" << "\n";
        std::cout << "device has vendor ID 0x" << std::hex << std::setw(4) << std::setfill('0') << device.get_descriptor().vendor_id()
                  << " and product ID 0x" << std::hex << std::setw(4) << std::setfill('0') << device.get_descriptor().product_id() << "\n";
        std::cout << "----------------------------------------------------------------" << "\n";
    }

    return EXIT_SUCCESS;
}
