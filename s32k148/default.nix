{
  pkgs ? import <nixpkgs> { },
  stdenv ? pkgs.stdenv,
  cmake ? pkgs.cmake,
  gcc-arm-embedded ? pkgs.gcc-arm-embedded,
  ninja ? pkgs.ninja,
}:

stdenv.mkDerivation {
  pname = "dc33-fw-s32k148";
  version = "0.1.0";
  src = ./.;

  nativeBuildInputs = [
    cmake
    gcc-arm-embedded
    ninja
  ];

  preConfigure = ''
    export CC=${gcc-arm-embedded}/bin/arm-none-eabi-gcc
    export CXX=${gcc-arm-embedded}/bin/arm-none-eabi-g++
  '';
}
