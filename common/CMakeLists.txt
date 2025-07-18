cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(dc33_fw_common)

add_library(dc33_fw_common dc33_fw_spi.pb-c.c spi.c protobuf-c/protobuf-c.c)
target_compile_options(
  dc33_fw_common
  PRIVATE
  -Os
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -fdata-sections
  -ffunction-sections
  -fmessage-length=0
  -fno-exceptions
  -mthumb
)
target_include_directories(dc33_fw_common PUBLIC .)
