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

    for (int n = 0; n < num_devices; n++) {
        const auto device = device_list[n];
        const auto device_speed = libusb_get_device_speed(device);
        std::cout << "device " << n << " is a " << [&](){
            switch (device_speed)
            {
            case LIBUSB_SPEED_LOW: return "LOW";
            case LIBUSB_SPEED_FULL: return "FULL";
            case LIBUSB_SPEED_HIGH: return "HIGH";
            case LIBUSB_SPEED_SUPER: return "SUPER";
            case LIBUSB_SPEED_UNKNOWN: return "UNKNOWN";
            default: return "";
            }
        }() << " speed device" << "\n";

        libusb_device_handle* handle;
        const auto status_open = libusb_open(device, &handle);
        if (status_open != LIBUSB_SUCCESS) {
            std::cerr << "failed to open device " << n << " (" << libusb_error_name(status_open) << ")" << "\n";
        } else {
            std::cout << "opened device " << n << "\n";
            libusb_close(handle);
        }
    }

    libusb_free_device_list(device_list, 1);

    return EXIT_SUCCESS;
}
