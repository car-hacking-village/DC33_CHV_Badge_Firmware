cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")

project(dc33_fw_rp2040)

pico_sdk_init()

add_subdirectory(../common common)

add_executable(dc33_fw_rp2040 main.c)
target_link_libraries(dc33_fw_rp2040 pico_stdlib hardware_spi dc33_fw_common)
pico_enable_stdio_usb(dc33_fw_rp2040 1)
pico_enable_stdio_uart(dc33_fw_rp2040 0)
pico_add_extra_outputs(dc33_fw_rp2040)

install(TARGETS dc33_fw_rp2040 DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_rp2040>/dc33_fw_rp2040.uf2 DESTINATION bin)
