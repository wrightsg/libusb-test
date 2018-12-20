cmake_minimum_required(VERSION 3.5.1)
project(libusb-test VERSION 0.0.1 LANGUAGES C CXX)

set(CONAN_CMAKE_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/cmake-conan/conan.cmake)
set(CONAN_BUILDINFO_FILE ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

if(EXISTS ${CONAN_CMAKE_FILE})
    include(cmake/cmake-conan/conan.cmake)

    conan_cmake_run(
        REQUIRES libusb/1.0.21@bincrafters/stable
        BASIC_SETUP
        CMAKE_TARGETS
        BUILD missing
    )
else()
    message(FATAL "provide conan.cmake (run \"git submodule update --init\")")
endif()

add_library(libusb_cpp
    libusb_device.cpp
    libusb_device_descriptor.cpp
    libusb_device_factory.cpp
)

target_link_libraries(libusb_cpp PUBLIC CONAN_PKG::libusb)

foreach(example hello_world)
    add_executable(${example} ${example}.cpp)

    target_link_libraries(${example} PRIVATE libusb_cpp)
endforeach()

add_executable(fxload examples/libusb/fxload/ezusb.c examples/libusb/fxload/fxload.c)
target_link_libraries(fxload PRIVATE CONAN_PKG::libusb)

add_executable(bulk_transfer_test bulk_transfer_test.cpp)
target_link_libraries(bulk_transfer_test PRIVATE CONAN_PKG::libusb)
