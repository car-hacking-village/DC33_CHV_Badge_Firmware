cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(dc33_fw_common)

add_library(dc33_fw_common dc33_fw_spi.pb-c.c protobuf-c/protobuf-c.c)
target_include_directories(dc33_fw_common PUBLIC .)
