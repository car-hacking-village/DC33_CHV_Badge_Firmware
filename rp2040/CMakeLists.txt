cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")

project(dc33_fw_rp2040)

pico_sdk_init()

if (NOT DEFINED ENV{FREERTOS_KERNEL_PATH})
    message(FATAL_ERROR "You must set FREERTOS_KERNEL_PATH")
endif()

include("$ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake")

if (DEFINED ENV{FW_COMMON_SRC})
  add_subdirectory("$ENV{FW_COMMON_SRC}" common)
else()
  add_subdirectory(../common common)
endif()

add_executable(dc33_fw_rp2040 main.c usb_descriptors.c)
target_include_directories(dc33_fw_rp2040 PRIVATE .)
target_link_libraries(
  dc33_fw_rp2040
  dc33_fw_common
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Static
  hardware_spi
  pico_stdlib
  pico_unique_id
  tinyusb_board
  tinyusb_device
)
pico_enable_stdio_usb(dc33_fw_rp2040 0)
pico_enable_stdio_uart(dc33_fw_rp2040 0)
pico_add_extra_outputs(dc33_fw_rp2040)

install(TARGETS dc33_fw_rp2040 DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_rp2040>/dc33_fw_rp2040.uf2 DESTINATION bin)
