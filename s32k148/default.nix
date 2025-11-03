{
  pkgs ? import <nixpkgs> { },
  callPackage ? pkgs.callPackage,
  stdenv ? pkgs.stdenv,
  cmake ? pkgs.cmake,
  gcc-arm-embedded ? pkgs.gcc-arm-embedded,
  ninja ? pkgs.ninja,
  protobuf ? pkgs.protobuf,
  protobufc ? pkgs.protobufc,
}:

let
  vendor = callPackage ../vendor/default.nix { };
in

stdenv.mkDerivation {
  pname = "dc33-fw-s32k148";
  version = "0.1.0";
  src = ./.;

  nativeBuildInputs = [
    cmake
    gcc-arm-embedded
    ninja
    protobuf
    protobufc
  ];

  preConfigure = ''
    export CC=${gcc-arm-embedded}/bin/arm-none-eabi-gcc
    export CXX=${gcc-arm-embedded}/bin/arm-none-eabi-g++
    export FW_COMMON_SRC=${../common}
    export BOOTLOADER_PATH=${vendor.bootloader}
    export COOKBOOK_PATH=${vendor.s32k148_cookbook}
    export FREERTOS_KERNEL_PATH=${vendor.FreeRTOS-Kernel}
    export PROTOBUF_C_PATH=${vendor.protobuf-c}
  '';
}
