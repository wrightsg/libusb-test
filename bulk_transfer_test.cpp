#include <iostream>
#include <vector>

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

    static constexpr std::uint16_t transfer_size = 0x1337;
    const auto num_bytes_transferred = libusb_control_transfer(
        device
        , LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE
        , 0x75 // bRequest
        , transfer_size // wValue
        , 0x0000 // vIndex
        , nullptr
        , 0x0000 // wLength
        , TRANSFER_TIMEOUT_MS
    );
    if (has_failed("libusb_control_transfer", num_bytes_transferred)) {
        return EXIT_FAILURE;
    }
    std::cout << "control transfer successful (transferred " << num_bytes_transferred << " bytes)" << "\n";

    // TODO Why is this necessary? If the interface is not claimed, the following bulk transfer will fail.
    const auto status_claim = libusb_claim_interface(device, 0);
    if (has_failed("libusb_claim_interface", status_claim)) {
        return EXIT_FAILURE;
    }
    std::cout << "claimed interface" << "\n";

    std::vector<unsigned char> data(transfer_size);
    int actual_length;
    const auto status_transfer = libusb_bulk_transfer(
        device
        , 0x82
        , &data.front()
        , data.size()
        , &actual_length
        , TRANSFER_TIMEOUT_MS
    );
    if (has_failed("libusb_bulk_transfer", status_transfer)) {
        return EXIT_FAILURE;
    }
    data.resize(actual_length);
    std::cout << "bulk transfer successful (transferred " << actual_length << " bytes)" << "\n";

    if (data.size() != transfer_size) {
        std::cerr << "got " << data.size() << " bytes but expected " << transfer_size << "\n";
    }

    return EXIT_SUCCESS;
}
