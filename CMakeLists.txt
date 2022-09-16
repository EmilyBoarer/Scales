cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(scales
  scales.c
)

pico_enable_stdio_usb(scales 1)
pico_enable_stdio_uart(scales 1)

pico_add_extra_outputs(scales)

add_subdirectory(extern/pico-scale)
add_subdirectory(extern/pico-ssd1306)
target_link_libraries(scales pico_stdlib hardware_gpio hardware_pio hardware_i2c pico-scale pico-ssd1306)