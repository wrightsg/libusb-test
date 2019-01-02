#include <iostream>
#include <vector>

#include <libusb-1.0/libusb.h>

static constexpr std::uint16_t VENDOR_ID = 0x04B4;
static constexpr std::uint16_t PRODUCT_ID = 0x00F1;
static constexpr unsigned int TRANSFER_TIMEOUT_MS = 1000;

struct command_line_args
{
    std::uint16_t wValue = 1024;
    std::uint16_t wIndex = 0x0001;
};

static command_line_args parse_command_line_args (int argc, char** argv)
{
    command_line_args args;

    for (int n = 1; n < argc; n++) {
        const std::string arg(argv[n]);
        if (arg == "-v") {
            args.wValue = std::strtoul(argv[++n], nullptr, 10);
        } else if (arg == "-i") {
            args.wIndex = std::strtoul(argv[++n], nullptr, 10);
        } else {
            std::cerr << "unreconigzed command line argument " << arg << "\n";
        }
    }

    return args;
}

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
    const auto args = parse_command_line_args(argc, argv);

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

    const auto transfer_size = static_cast<std::uint32_t>(args.wValue) * static_cast<std::uint32_t>(args.wIndex);
    std::cout << "requesting bulk transfer with " << std::dec << transfer_size << " bytes" << "\n";
    const auto num_bytes_transferred = libusb_control_transfer(
        device
        , LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE
        , 0x75 // bRequest
        , args.wValue // wValue
        , args.wIndex // vIndex
        , nullptr
        , 0x0000 // wLength
        , TRANSFER_TIMEOUT_MS
    );
    if (has_failed("libusb_control_transfer", num_bytes_transferred)) {
        return EXIT_FAILURE;
    }
    std::cout << "control transfer successful (transferred " << std::dec << num_bytes_transferred << " bytes)" << "\n";

    // TODO Why is this necessary? If the interface is not claimed, the following bulk transfer will fail.
    const auto status_claim = libusb_claim_interface(device, 0);
    if (has_failed("libusb_claim_interface", status_claim)) {
        return EXIT_FAILURE;
    }
    std::cout << "claimed interface" << "\n";

    std::vector<unsigned char> data(2*transfer_size);
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
    std::cout << "bulk transfer successful (transferred " << std::dec << actual_length << " bytes)" << "\n";

    if (data.size() != transfer_size) {
        std::cerr << "got " << std::dec << data.size() << " bytes but expected " << std::dec << transfer_size << "\n";
    }

    return EXIT_SUCCESS;
}
