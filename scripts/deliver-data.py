#!/usr/bin/env python3
"""Publish a new HDD data tree only after every uploaded file is read back."""
import argparse
import ftplib
import hashlib
import json
from pathlib import Path
import uuid

p=argparse.ArgumentParser()
p.add_argument('--host',required=True)
p.add_argument('--port',type=int,default=2121)
a=p.parse_args()
root=Path(__file__).resolve().parents[1]
data=root/'build/data'
sources=[]
for folder in ('avp_huds','avp_rifs','fastfile'):
    files=sorted(x for x in (data/folder).rglob('*') if x.is_file())
    if not files: raise SystemExit('Missing data folder: '+folder)
    sources.extend(files)
sources.append(data/'language.txt')
if (data/'cd tracks.txt').exists(): sources.append(data/'cd tracks.txt')
manifest={}
for path in sources:
    with path.open('rb') as stream: digest=hashlib.file_digest(stream,'sha256').hexdigest()
    manifest[path.relative_to(data).as_posix()]={'bytes':path.stat().st_size,'sha256':digest}
ftp=ftplib.FTP()
ftp.connect(a.host,a.port,timeout=30)
ftp.login()
ftp.cwd('/data/avp')
if 'gamedata' in [x.rstrip('/').rsplit('/',1)[-1] for x in ftp.nlst()]:
    raise SystemExit('HDD gamedata already exists; refusing to replace it')
stage='.gamedata-upload-'+uuid.uuid4().hex[:12]
ftp.mkd(stage)
ftp.cwd(stage)
for folder in ('avp_huds','avp_rifs','fastfile'): ftp.mkd(folder)
made=set(('avp_huds','avp_rifs','fastfile'))
for index,(name,info) in enumerate(manifest.items(),1):
    parent=Path(name).parent
    for directory in reversed((parent,*parent.parents)):
        rel=directory.as_posix()
        if rel!='.' and rel not in made:
            ftp.mkd(rel); made.add(rel)
    with (data/name).open('rb') as stream:
        ftp.storbinary('STOR '+name,stream,blocksize=256*1024)
    digest=hashlib.sha256()
    count=[0]
    def consume(block):
        digest.update(block); count[0]+=len(block)
    ftp.retrbinary('RETR '+name,consume,blocksize=256*1024)
    if count[0]!=info['bytes'] or digest.hexdigest()!=info['sha256']:
        raise SystemExit('Readback mismatch; retained staging tree: '+stage+'/'+name)
    if index%20==0 or index==len(manifest):
        print(f'Uploaded and readback-verified {index}/{len(manifest)} files',flush=True)
ftp.cwd('/data/avp')
if 'gamedata' in [x.rstrip('/').rsplit('/',1)[-1] for x in ftp.nlst()]:
    raise SystemExit('Target appeared; staging tree retained: '+stage)
ftp.rename(stage,'gamedata')
ftp.quit()
result={'host':a.host,'port':a.port,'remote_root':'/data/avp/gamedata',
        'files':manifest,'verified_files':len(manifest),
        'bytes':sum(v['bytes'] for v in manifest.values()),'all_files_ftp_readback_verified':True}
(root/'artifacts/hdd-data-delivery-v03.json').write_text(json.dumps(result,indent=2)+'\n')
print(f'Published {len(manifest)} verified files to /data/avp/gamedata',flush=True)
