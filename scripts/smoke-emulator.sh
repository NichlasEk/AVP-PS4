#!/usr/bin/env bash
# Requires an existing test profile for automatic navigation (otherwise use manual input).
set -euo pipefail
root=$(cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root"
species=${1:-alien}
runtime=${2:-"$root/build/runtime"}
export SHADPS4=${SHADPS4:-/home/nichlas/shadPS4/build-validation/shadps4}
export AVP_TEST_RUNTIME="$runtime"
export AVP_TEST_SPECIES="$species"
case "$species" in alien|marine|predator) ;; *) exit 2;; esac
mkdir -p artifacts
timeout -k 3s 65s xvfb-run -a -s '-screen 0 1280x720x24' bash -c '
  export SDL_VIDEODRIVER=x11 XDG_DATA_HOME="$PWD/build/emulator-profile"
  "$SHADPS4" --fullscreen false "$AVP_TEST_RUNTIME/eboot.bin" > "artifacts/final-$AVP_TEST_SPECIES.log" 2>&1 &
  app_pid=$!
  trap '\''kill "$app_pid" 2>/dev/null || true'\'' EXIT
  sleep 5
  keys=(n wait:2 n wait:2)
  if [[ $AVP_TEST_SPECIES == marine ]]; then keys+=(Down); fi
  if [[ $AVP_TEST_SPECIES == predator ]]; then keys+=(Down Down); fi
  python3 scripts/x11-keys.py "${keys[@]}" n wait:2 n wait:2 n wait:2 n wait:18
  kill -0 "$app_pid"
  import -window root "artifacts/final-$AVP_TEST_SPECIES.png"
  python3 scripts/x11-keys.py hold:o:1 hold:w:0.5 hold:l:0.5
  sleep 2
  kill -0 "$app_pid"
  import -window root "artifacts/final-$AVP_TEST_SPECIES-input.png"
'
