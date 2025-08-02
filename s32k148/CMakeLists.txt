cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(dc33_fw_s32k148 C CXX ASM)

if(DEFINED ENV{FW_COMMON_SRC})
  add_subdirectory("$ENV{FW_COMMON_SRC}" common)
else()
  add_subdirectory(../common common)
endif()

if(DEFINED ENV{BOOTLOADER_PATH})
  set(BOOTLOADER_PATH "$ENV{BOOTLOADER_PATH}")
else()
  set(BOOTLOADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/bootloader")
endif()

if(DEFINED ENV{COOKBOOK_PATH})
  set(COOKBOOK_PATH "$ENV{COOKBOOK_PATH}")
else()
  set(COOKBOOK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/s32k148_cookbook")
endif()

add_executable(
  dc33_fw_s32k148
  main.c
  LPSPI.c
  LPUART.c
  "${BOOTLOADER_PATH}/AN12218_S32K148_Example_App/Example_app/Project_Settings/Startup_Code/startup.c"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Example_App/Example_app/Project_Settings/Startup_Code/startup_S32K148.S"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Example_App/Example_app/Project_Settings/Startup_Code/system_S32K148.c"
  "${COOKBOOK_PATH}/S32K148_Project_FlexCan/src/FlexCAN.c"
  "${COOKBOOK_PATH}/S32K148_Project_FlexCan/src/clocks_and_modes.c"
  hex.c
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
  -specs=nosys.specs
  $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
  $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++23>
)
target_compile_options(dc33_fw_common PRIVATE -mcpu=cortex-m4)
target_include_directories(
  dc33_fw_s32k148
  PRIVATE
  "${BOOTLOADER_PATH}/AN12218_S32K148_Example_App/Example_app/include"
  "${COOKBOOK_PATH}/S32K148_Project_FlexCan/src"
)
target_link_libraries(dc33_fw_s32k148 dc33_fw_common)
target_link_options(
  dc33_fw_s32k148
  PRIVATE
  "-Wl,-Map=$<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map"
  --entry=Reset_Handler
  -T "${CMAKE_CURRENT_SOURCE_DIR}/main.ld"
  -Wl,-gc-sections
  -mcpu=cortex-m4
  -mthumb
  -n
  -specs=nosys.specs
)
set_target_properties(dc33_fw_s32k148 PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/main.ld")

add_custom_command(OUTPUT dc33_fw_s32k148.srec COMMAND arm-none-eabi-objcopy -O srec $<TARGET_FILE:dc33_fw_s32k148> dc33_fw_s32k148.srec DEPENDS dc33_fw_s32k148)
add_custom_target(dc33_fw_s32k148_srec ALL DEPENDS dc33_fw_s32k148.srec)

install(TARGETS dc33_fw_s32k148 DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.map DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148>/dc33_fw_s32k148.srec DESTINATION bin)

add_executable(
  dc33_fw_s32k148_bootloader
  bootloader.c
  LPUART.c
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/Project_Settings/Startup_Code/startup.c"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/Project_Settings/Startup_Code/startup_S32K148.S"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/Project_Settings/Startup_Code/system_S32K148.c"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/src/drivers/fsl_flash_driver_c90tfs.c"
  "${COOKBOOK_PATH}/S32K148_Project_LPUART/src/clocks_and_modes.c"
)
target_compile_options(
  dc33_fw_s32k148_bootloader
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
  -specs=nosys.specs
  $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
  $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++23>
)
target_include_directories(
  dc33_fw_s32k148_bootloader
  PRIVATE
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/include"
  "${BOOTLOADER_PATH}/AN12218_S32K148_Bootloader_Software/S32K148_bootloader/src/drivers/inc"
  "${COOKBOOK_PATH}/S32K148_Project_LPUART/src"
)
target_link_options(
  dc33_fw_s32k148_bootloader
  PRIVATE
  "-Wl,-Map=$<TARGET_FILE_DIR:dc33_fw_s32k148_bootloader>/dc33_fw_s32k148_bootloader.map"
  --entry=Reset_Handler
  -T "${CMAKE_CURRENT_SOURCE_DIR}/bootloader.ld"
  -Wl,-gc-sections
  -mcpu=cortex-m4
  -mthumb
  -n
  -specs=nosys.specs
)
set_target_properties(dc33_fw_s32k148_bootloader PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bootloader.ld")

add_custom_command(OUTPUT dc33_fw_s32k148_bootloader.srec COMMAND arm-none-eabi-objcopy -O srec $<TARGET_FILE:dc33_fw_s32k148_bootloader> dc33_fw_s32k148_bootloader.srec DEPENDS dc33_fw_s32k148_bootloader)
add_custom_target(dc33_fw_s32k148_bootloader_srec ALL DEPENDS dc33_fw_s32k148_bootloader.srec)

install(TARGETS dc33_fw_s32k148_bootloader DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148_bootloader>/dc33_fw_s32k148_bootloader.map DESTINATION bin)
install(FILES $<TARGET_FILE_DIR:dc33_fw_s32k148_bootloader>/dc33_fw_s32k148_bootloader.srec DESTINATION bin)
