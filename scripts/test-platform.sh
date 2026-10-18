#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build artifacts
for test_name in data-path directories; do
  cc -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined \
    -Iplatform/ps4 "tests/$test_name-test.c" -o "build/$test_name-test"
  "build/$test_name-test" | tee "artifacts/$test_name-v04-test.log"
done
bash scripts/test-input.sh
