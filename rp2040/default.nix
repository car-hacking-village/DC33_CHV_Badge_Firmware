{
  pkgs ? import <nixpkgs> { },
  stdenv ? pkgs.stdenv,
  cmake ? pkgs.cmake,
  gcc-arm-embedded ? pkgs.gcc-arm-embedded,
  ninja ? pkgs.ninja,
  pico-sdk ? pkgs.pico-sdk,
  picotool ? pkgs.picotool,
  protobuf ? pkgs.protobuf,
  protobufc ? pkgs.protobufc,
  python3 ? pkgs.python3,
}:

let
  sdk = pico-sdk.override { withSubmodules = true; };
in

stdenv.mkDerivation {
  pname = "dc33-fw-rp2040";
  version = "0.1.0";
  src = ./.;

  nativeBuildInputs = [
    cmake
    gcc-arm-embedded
    ninja
    picotool
    protobuf
    protobufc
    python3
  ];

  buildInputs = [ sdk ];

  preConfigure = ''
    export PICO_SDK_PATH=${sdk}/lib/pico-sdk
    export PICO_PLATFORM=rp2040
    export PICO_BOARD=pico
    export CC=${gcc-arm-embedded}/bin/arm-none-eabi-gcc
    export CXX=${gcc-arm-embedded}/bin/arm-none-eabi-g++
    export FW_COMMON_SRC=${../common}
  '';
}
