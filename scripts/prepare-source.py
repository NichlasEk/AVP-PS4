#!/usr/bin/env python3
"""Fetch pinned public source/compatibility text and apply the local overlay."""
import gzip
import hashlib
from pathlib import Path
import subprocess
import tarfile
import urllib.request

root = Path(__file__).resolve().parents[1]
cache = root / 'build/upstream'
cache.mkdir(parents=True, exist_ok=True)
def fetch(url, name, sha):
    dest = cache / name
    if not dest.exists():
        with urllib.request.urlopen(url, timeout=60) as response:
            data = response.read()
        if hashlib.sha256(data).hexdigest() != sha:
            raise SystemExit(f'Checksum mismatch: {url}')
        dest.write_bytes(data)
    if hashlib.sha256(dest.read_bytes()).hexdigest() != sha:
        raise SystemExit(f'Checksum mismatch: {dest}')
    return dest

archive = fetch('https://icculus.org/avp/files/avp-20170505-a1.tar.gz',
    'avp-20170505-a1.tar.gz', '2acb278c580e056dbcc2a96608b0793b334204f75a4e44cf7b307f99f7852747')
engine = root / 'engine'
if not engine.exists():
    with tarfile.open(archive) as tar:
        tar.extractall(cache, filter='data')
    (cache / 'avp-20170505').rename(engine)
for patch in sorted((root / 'patches').glob('*.patch')):
    reverse = subprocess.run(['patch', '-p1', '--reverse', '--dry-run', '-i', str(patch)],
                             cwd=engine, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if reverse.returncode:
        subprocess.run(['patch', '-p1', '--forward', '--batch', '-i', str(patch)], cwd=engine, check=True)
language = fetch('https://icculus.org/avp/files/etc/english.txt', 'english.txt',
    '3d2f5c201195496f0f0ad4676ad9b6b1dfbd439329b09430e2fc95e93fe7f6ae')
dest = root / 'build/compat-language/english.txt'
dest.parent.mkdir(parents=True, exist_ok=True)
dest.write_bytes(gzip.decompress(language.read_bytes()))
print('Pinned engine and compatibility language ready.')
