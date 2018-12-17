cmake_minimum_required(VERSION 3.5.1)
project(libusb-test VERSION 0.0.1 LANGUAGES CXX)

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

foreach(example hello_world)
    add_executable(${example} ${example}.cpp)

    target_link_libraries(${example} PRIVATE CONAN_PKG::libusb)
endforeach()
