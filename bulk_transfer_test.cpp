#include <iostream>

#include <libusb-1.0/libusb.h>

static constexpr std::uint16_t VENDOR_ID = 0x04B4;
static constexpr std::uint16_t PRODUCT_ID = 0x00F1;

static bool has_failed (const std::string& function_name, int status)
{
    if (status != LIBUSB_SUCCESS) {
        std::cerr << function_name << " failed (" << libusb_error_name(status) << ")" << "\n";
        return true;
    }
    return false;
}

int main (int argc, char** argv)
{
    libusb_context* context;
    if (has_failed("libusb_init", libusb_init(&context))) {
        return EXIT_FAILURE;
    }

    auto device = libusb_open_device_with_vid_pid(context, VENDOR_ID, PRODUCT_ID);
    if (!device) {
        std::cerr << "failed to open device with VID 0x" << std::hex << VENDOR_ID
                  << " and PID 0x" << std::hex << PRODUCT_ID << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "opened device with VID 0x" << std::hex << VENDOR_ID << " and PID 0x" << PRODUCT_ID << "\n";

    return EXIT_SUCCESS;
}
