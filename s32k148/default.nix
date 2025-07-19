{
  pkgs ? import <nixpkgs> { },
  stdenv ? pkgs.stdenv,
  cmake ? pkgs.cmake,
  gcc-arm-embedded ? pkgs.gcc-arm-embedded,
  ninja ? pkgs.ninja,
  protobuf ? pkgs.protobuf,
  protobufc ? pkgs.protobufc,
}:

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
  '';
}
