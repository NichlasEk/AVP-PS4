#!/usr/bin/env python3
"""Extract the private PKG and verify every allowlisted payload against staging."""
import hashlib
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser()
p.add_argument('package',nargs='?',default=str(root/'dist/AvP-PS4-0.4-NoData.pkg'))
package=Path(p.parse_args().package).resolve()
manifest=json.loads(package.with_suffix('.manifest.json').read_text())
with package.open('rb') as stream:
    if hashlib.file_digest(stream,'sha256').hexdigest()!=manifest['sha256']:
        raise SystemExit('Package hash does not match manifest')
sdk=Path(os.environ.get('OO_PS4_TOOLCHAIN','/opt/openorbis/OpenOrbis/PS4Toolchain'))
dest=Path(tempfile.mkdtemp(prefix='pkg-readback-',dir=root/'build'))
subprocess.run([str(sdk/'bin/linux/PkgTool.Core'),'pkg_extract',str(package),str(dest)],check=True)
dest=dest/'uroot'
# Metadata is stored outside PFS. This tool version accepts table indices.
listing=subprocess.check_output([str(sdk/'bin/linux/PkgTool.Core'),'pkg_listentries',str(package)],text=True)
indices={line.split()[-1]:line.split()[3] for line in listing.splitlines() if line.startswith('0x')}
for symbol,name in (('PARAM_SFO','param.sfo'),('ICON0_PNG','icon0.png')):
    entry=indices[symbol]
    subprocess.run([str(sdk/'bin/linux/PkgTool.Core'),'pkg_extractentry',str(package),str(entry),str(dest/'sce_sys'/name)],check=True)
for name,digest in manifest['files'].items():
    with (dest/name).open('rb') as stream:
        actual=hashlib.file_digest(stream,'sha256').hexdigest()
    if actual != digest: raise SystemExit(f'Readback mismatch: {name}')
result={'payload_files_verified':len(manifest['files']),'extracted':str(dest),
        'package_sha256':manifest['sha256'],'physical_ps4_tested':False}
(root/'artifacts/package-readback.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
