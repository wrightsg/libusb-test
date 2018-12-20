#include <array>
#include <iostream>

#include <libusb-1.0/libusb.h>

static constexpr std::uint16_t VENDOR_ID = 0x04B4;
static constexpr std::uint16_t PRODUCT_ID = 0x00F1;
static constexpr unsigned int TRANSFER_TIMEOUT_MS = 1000;

static bool has_failed (const std::string& function_name, int status)
{
    if (status < LIBUSB_SUCCESS) {
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

    std::array<unsigned char, 128> data;
    const auto num_bytes_transferred = libusb_control_transfer(
        device
        , LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE
        , 0x76 // bRequest
        , 0x0000 // wValue
        , 0x0000 // vIndex
        , &data.front()
        , 0x0004 // wLength
        , TRANSFER_TIMEOUT_MS
    );
    if (has_failed("libusb_control_transfer", num_bytes_transferred)) {
        return EXIT_FAILURE;
    }
    std::cout << "control transfer successful (transferred " << num_bytes_transferred << " bytes)" << "\n";

    for (int n = 0; n < num_bytes_transferred; n++) {
        std::cout << "data[" << n << "] = 0x" << std::hex << static_cast<unsigned>(data[n]) << "\n";
    }

    return EXIT_SUCCESS;
}
