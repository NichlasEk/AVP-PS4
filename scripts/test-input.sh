#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
sdk=${OO_PS4_TOOLCHAIN:-/opt/openorbis/OpenOrbis/PS4Toolchain}
mkdir -p build artifacts
cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined \
  -Iplatform/ps4 -idirafter "$sdk/include" tests/input-test.c -lm -o build/input-test
build/input-test | tee artifacts/input-v02-test.log
