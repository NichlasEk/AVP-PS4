#!/usr/bin/env python3
"""Extract a local AvP disc into a new lowercase data tree (never overwrite)."""
import argparse
from pathlib import Path
import subprocess
import shutil

p = argparse.ArgumentParser()
p.add_argument('iso', type=Path)
p.add_argument('--output', type=Path, default=Path('build/data'))
a = p.parse_args()
language = Path(__file__).resolve().parents[1] / 'build/compat-language/english.txt'
if not language.is_file():
    p.error('Run scripts/prepare-source.py first for the compatible language file.')
if a.output.exists():
    p.error(f'output already exists: {a.output}')
a.output.mkdir(parents=True)
subprocess.run(['7z', 'x', '-y', f'-o{a.output}', str(a.iso)], check=True,
               stdout=subprocess.DEVNULL)
for path in sorted(a.output.rglob('*'), key=lambda x: len(x.parts), reverse=True):
    target = path.with_name(path.name.lower())
    if target != path:
        if target.exists():
            raise RuntimeError(f'case collision: {path}')
        path.rename(target)
# Retail disc separates high-quality sound archives from the texture directory.
sound = a.output / 'hsound/fastfile'
if sound.is_dir():
    for path in sound.iterdir():
        if path.is_file():
            target = a.output / 'fastfile' / path.name
            if target.exists() and target.read_bytes() != path.read_bytes():
                raise RuntimeError(f'conflicting sound file: {target}')
            shutil.copyfile(path, target)
shutil.copyfile(language, a.output / 'language.txt')
print(f'Imported {len(list(a.output.rglob("*")))} entries into {a.output}')
