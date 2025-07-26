{ ... }@args:

with import <nixpkgs> args;

let
  build = writeShellScriptBin "build" ''
    set -e
    set -x

    cmake -S . -B build -G Ninja
    ninja -C build -v
  '';

  format = writeShellScriptBin "format" ''
    set -e
    set -x

    find . -type f \( -name '*.cpp' -or -name '*.h' \) -print0 | xargs -0 -t ${clang-tools}/bin/clang-format -i
  '';

  vendor = callPackage ../vendor/default.nix { };
in

mkShell {
  packages = [
    build
    clang-tools
    gdb
    format
  ];

  inputsFrom = [ (callPackage ./default.nix { }) ];

  shellHook = ''
    export CC=${gcc-arm-embedded}/bin/arm-none-eabi-gcc
    export CXX=${gcc-arm-embedded}/bin/arm-none-eabi-g++
    export PROTOBUF_C_PATH=${vendor.protobuf-c}
  '';
}
