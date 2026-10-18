#!/usr/bin/env bash
set -euo pipefail
root=$(cd -- "$(dirname -- "$0")/.." && pwd)
export OO_PS4_TOOLCHAIN=${OO_PS4_TOOLCHAIN:-/opt/openorbis/OpenOrbis/PS4Toolchain}
fself=${CREATE_FSELF:-/home/nichlas/ut99-orbis/build/create-fself-current}
python3 "$root/scripts/prepare-source.py"
cmake -S "$root" -B "$root/build/ps4" -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$root/cmake/openorbis.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build "$root/build/ps4" -j"${JOBS:-12}"
mkdir -p "$root/build/runtime/sce_module"
"$fself" -in="$root/build/ps4/avp" -out="$root/build/runtime/avp.oelf" \
  --eboot "$root/build/runtime/eboot.bin" --paid 0x3800000000000011
cp "$OO_PS4_TOOLCHAIN/samples/hello_world/sce_module/"{libc.prx,libSceFios2.prx} "$root/build/runtime/sce_module/"
cp "$root/assets/scummvm-controller-only.txt" "$root/build/runtime/scummvm-controller-only.txt"
if [[ ! -e "$root/build/runtime/data" ]]; then ln -s ../data "$root/build/runtime/data"; fi
sha256sum "$root/build/runtime/eboot.bin"
