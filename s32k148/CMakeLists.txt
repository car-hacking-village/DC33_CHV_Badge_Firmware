cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(dc33_fw_s32k148 C CXX ASM)

add_subdirectory(../common common)

add_executable(
  dc33_fw_s32k148
  main.c
  LPSPI.c
  LPUART.c
  clocks_and_modes.c
  startup.c
  startup_S32K148.S
  system_S32K148.c
)
target_compile_options(
  dc33_fw_s32k148
  PRIVATE
  -DCPU_S32K148
  -DSTART_FROM_FLASH
  -Os
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -fdata-sections
  -ffunction-sections
  -fmessage-length=0
  -fno-exceptions
  -mcpu=cortex-m4
  -mthumb
  $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
  $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++23>
)
target_link_libraries(dc33_fw_s32k148 dc33_fw_common)
target_link_options(
  dc33_fw_s32k148
  PRIVATE
  "-Wl,-Map=$<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map"
  --entry=Reset_Handler
  -T "${CMAKE_CURRENT_SOURCE_DIR}/S32K148_256_flash.ld"
  -Wl,-gc-sections
  -mcpu=cortex-m4
  -mthumb
)
set_target_properties(dc33_fw_s32k148 PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/S32K148_256_flash.ld")

add_custom_command(OUTPUT dc33_fw_s32k148.srec COMMAND arm-none-eabi-objcopy -O srec $<TARGET_FILE:dc33_fw_s32k148> dc33_fw_s32k148.srec DEPENDS dc33_fw_s32k148)
add_custom_target(dc33_fw_s32k148_srec ALL DEPENDS dc33_fw_s32k148.srec)

install(TARGETS dc33_fw_s32k148 DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.srec DESTINATION bin)
