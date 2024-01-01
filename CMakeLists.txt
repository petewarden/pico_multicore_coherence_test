cmake_minimum_required(VERSION 3.12)

set(PROJECT pico_multicore_coherence_test)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
pico_sdk_init()

project(${PROJECT} C CXX ASM)

add_executable(${PROJECT} ${PROJECT}.c)
target_link_libraries(${PROJECT} 
    pico_stdlib
    pico_multicore
)
pico_add_extra_outputs(${PROJECT})
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)
