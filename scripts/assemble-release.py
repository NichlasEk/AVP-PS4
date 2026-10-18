#!/usr/bin/env python3
"""Assemble and read back the explicitly selected public release artifacts."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tarfile

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('kit', type=Path)
a = p.parse_args()
kit = a.kit.resolve()
manifest = json.loads((kit / 'MANIFEST.json').read_text())
package = ROOT / 'dist/AvP-PS4-v0.4.0.pkg'
out = ROOT / 'dist/release-v0.4.0'
out.mkdir(exist_ok=False)

def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

subprocess.run(['python3', str(ROOT / 'scripts/verify-package.py'), str(package)], check=True)
payload = json.loads(package.with_suffix('.manifest.json').read_text())
assert payload['contains_game_data'] is False
assert payload['files']['eboot.bin'] == manifest['tested_eboot_sha256']
for name, expected in manifest['files'].items():
    path = Path(name)
    assert not path.is_absolute() and '..' not in path.parts, name
    assert path.suffix.lower() not in {'.iso', '.pkg', '.rif', '.ffl', '.exe', '.prx', '.sprx'}, name
    assert digest(kit / path) == expected, name
archive = out / 'AvP-PS4-v0.4.0-source-relink.tar.gz'
with tarfile.open(archive, 'w:gz') as tar:
    for name in sorted(manifest['files']) + ['MANIFEST.json']:
        info = tar.gettarinfo(str(kit / name), arcname='AvP-PS4-source-relink/' + name)
        info.uid = info.gid = 0
        info.uname = info.gname = ''
        with (kit / name).open('rb') as stream:
            tar.addfile(info, stream)
with tarfile.open(archive) as tar:
    seen = set()
    for member in tar:
        name = member.name.removeprefix('AvP-PS4-source-relink/')
        assert member.isfile() and name not in seen
        seen.add(name)
        with tar.extractfile(member) as stream:
            actual = hashlib.file_digest(stream, 'sha256').hexdigest()
        expected = digest(kit / 'MANIFEST.json') if name == 'MANIFEST.json' else manifest['files'][name]
        assert actual == expected, name
    assert seen == set(manifest['files']) | {'MANIFEST.json'}
shutil.copyfile(package, out / package.name)
for source, name in [('docs/INSTALL.md','INSTALL.md'), ('docs/THIRD-PARTY.md','THIRD-PARTY.md'),
                     ('docs/RELEASE-STATUS.md','VERIFICATION.md'), ('LICENSE','LICENSE')]:
    shutil.copyfile(ROOT / source, out / name)
(out / 'PACKAGE-MANIFEST.json').write_text(json.dumps({
    'package': package.name, 'sha256': digest(package), 'contains_game_data': False,
    'same_eboot_as_user_tested_candidate': True, 'repackaged_container_console_tested': False,
    'files': payload['files'],
}, indent=2) + '\n')
(out / 'SHA256SUMS').write_text(''.join(f'{digest(path)}  {path.name}\n'
    for path in sorted(out.iterdir()) if path.is_file() and path.name != 'SHA256SUMS'))
print(f'Verified {len(seen)} companion files; release artifacts: {out}')
