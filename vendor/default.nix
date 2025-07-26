{
  pkgs ? import <nixpkgs> { },
  fetchFromGitHub ? pkgs.fetchFromGitHub,
}:

{
  bootloader = fetchFromGitHub {
    owner = "nxp-auto-support";
    repo = "AN12218_sw";
    rev = "fec1343e589a8723a62c9a9854c793480906297b";
    hash = "sha256-AukJraC/abGpIyTUOL/vvESN76ehvzNzBpXcpn96RTc=";
  };

  protobuf-c = fetchFromGitHub {
    owner = "protobuf-c";
    repo = "protobuf-c";
    tag = "v1.5.2";
    hash = "sha256-bpxk2o5rYLFkx532A3PYyhh2MwVH2Dqf3p/bnNpQV7s=";
  };

  s32k148_cookbook = fetchFromGitHub {
    owner = "nxp-auto-support";
    repo = "s32k148_cookbook";
    rev = "794598d8986a8f8f3b15f495cb19cf683ef7f65f";
    hash = "sha256-Ci5LOfFKz/AdkM8Vuh6W3QuGipiyU5IYkewp6zp3iks=";
  };
}
