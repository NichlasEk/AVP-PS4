#!/usr/bin/env python3
"""Run inside the extracted relinking kit; never modifies the tested release."""
import argparse
import json
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser()
p.add_argument('--openal', type=Path, help='Replacement static libopenal.a')
a = p.parse_args()
root = Path(__file__).resolve().parent
sdk = Path(os.environ.get('OO_PS4_TOOLCHAIN', '/opt/openorbis/OpenOrbis/PS4Toolchain'))
if not (sdk / 'lib/crt1.o').is_file():
    raise SystemExit('Set OO_PS4_TOOLCHAIN to an installed OpenOrbis SDK')
args = json.loads((root / 'link-arguments.json').read_text())
args = [arg.replace('@SDK@', str(sdk)) for arg in args]
if a.openal:
    replacement = a.openal.resolve(strict=True)
    args = [str(replacement) if arg == 'libraries/libopenal.a' else arg for arg in args]
subprocess.run(args, cwd=root, check=True)
print(root / 'avp-relinked.elf')
