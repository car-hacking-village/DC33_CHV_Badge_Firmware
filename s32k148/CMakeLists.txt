cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(dc33_fw_s32k148 C CXX ASM)

add_executable(
  dc33_fw_s32k148
  main.cpp
  "$ENV{NXP_SDK}/platform/devices/S32K148/startup/system_S32K148.c"
  "$ENV{NXP_SDK}/platform/devices/startup.c"
  "$ENV{NXP_SDK}/platform/drivers/src/pins/pins_driver.c"
  "$ENV{NXP_SDK}/platform/drivers/src/pins/pins_port_hw_access.c"
  "$ENV{NXP_SDK}/platform/drivers/src/clock/S32K1xx/clock_S32K1xx.c"
  "$ENV{NXP_SDK}/platform/drivers/src/interrupt/interrupt_manager.c"
  "$ENV{NXP_SDK}/platform/devices/S32K148/startup/gcc/startup_S32K148.S"
)
target_include_directories(
  dc33_fw_s32k148
  PRIVATE
  "$ENV{NXP_SDK}/platform"
  "$ENV{NXP_SDK}/platform/drivers/src"
  "$ENV{NXP_SDK}/platform/drivers/src/clock"
  "$ENV{NXP_SDK}/platform/drivers/src/interrupt"
  "$ENV{NXP_SDK}/platform/drivers/src/clock/S32K1xx"
  "$ENV{NXP_SDK}/platform/devices"
  "$ENV{NXP_SDK}/platform/devices/S32K148"
  "$ENV{NXP_SDK}/platform/devices/S32K148/startup"
  "$ENV{NXP_SDK}/platform/devices/S32K148/startup/gcc"
  "$ENV{NXP_SDK}/platform/devices/S32K148/include"
  "$ENV{NXP_SDK}/platform/devices/common"
  "$ENV{NXP_SDK}/platform/drivers/inc"
  "$ENV{NXP_SDK}/platform/drivers/src/adc"
  "$ENV{NXP_SDK}/platform/drivers/src/flash"
  "$ENV{NXP_SDK}/platform/drivers/src/lpuart"
  "$ENV{NXP_SDK}/platform/drivers/src/lpi2c"
  "$ENV{NXP_SDK}/platform/drivers/src/pins"
)
target_compile_options(
  dc33_fw_s32k148
  PRIVATE
  $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
  -mcpu=cortex-m4
  -mthumb
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Os
  -funsigned-char
  -funsigned-bitfields
  -fshort-enums
  -ffunction-sections
  -fdata-sections
  -fno-jump-tables
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++23>
  -save-temps=obj
  -ggdb
  -DCPU_S32K148
  -DSTART_FROM_FLASH
  --sysroot /path/to/newlib
)
target_link_options(
  dc33_fw_s32k148
  PRIVATE
  "-Wl,-Map=$<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map"
  -nostartfiles
  -nodefaultlibs
  -nostdlib
  -T "$ENV{NXP_SDK}/platform/devices/S32K148/linker/gcc/S32K148_256_flash.ld"
  --entry=Reset_Handler
  -Wl,-gc-sections
  -lgcc
  -lm
  -lc
  -lrdimon
  --specs=rdimon.specs
  --sysroot /path/to/newlib
)

add_custom_command(OUTPUT dc33_fw_s32k148.srec COMMAND arm-none-eabi-objcopy -O srec $<TARGET_FILE:dc33_fw_s32k148> dc33_fw_s32k148.srec DEPENDS dc33_fw_s32k148)
add_custom_target(dc33_fw_s32k148_srec ALL DEPENDS dc33_fw_s32k148.srec)

install(TARGETS dc33_fw_s32k148 DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.srec DESTINATION bin)
