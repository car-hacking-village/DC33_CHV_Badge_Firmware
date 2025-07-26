cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(dc33_fw_common)

if(DEFINED ENV{PROTOBUF_C_PATH})
  set(PROTOBUF_C_PATH "$ENV{PROTOBUF_C_PATH}")
else()
  set(PROTOBUF_C_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/protobuf-c")
endif()

add_custom_command(
  OUTPUT dc33_fw_spi.pb-c.c dc33_fw_spi.pb-c.h
  COMMAND protoc "-I${CMAKE_CURRENT_SOURCE_DIR}" "--c_out=${CMAKE_CURRENT_BINARY_DIR}" dc33_fw_spi.proto
  DEPENDS dc33_fw_spi.proto
  VERBATIM
)
add_custom_target(dc33_fw_common_protobuf DEPENDS dc33_fw_spi.pb-c.c dc33_fw_spi.pb-c.h)

add_library(dc33_fw_common dc33_fw_spi.pb-c.c led.c spi.c "${PROTOBUF_C_PATH}/protobuf-c/protobuf-c.c")
add_dependencies(dc33_fw_common dc33_fw_common_protobuf)
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
target_include_directories(dc33_fw_common PUBLIC . "${CMAKE_CURRENT_BINARY_DIR}" "${PROTOBUF_C_PATH}")
