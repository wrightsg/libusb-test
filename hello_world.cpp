#include <iostream>

#include <libusb-1.0/libusb.h>

int main (int argc, char** argv)
{
    libusb_context* context;
    if (libusb_init(&context)) {
        std::cerr << "failed to initialize libusb" << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "initialized libusb" << "\n";

    libusb_device** device_list;
    const auto num_devices = libusb_get_device_list(context, &device_list);
    std::cout << "found " << num_devices << " devices" << "\n";

    libusb_free_device_list(device_list, 1);

    return EXIT_SUCCESS;
}
