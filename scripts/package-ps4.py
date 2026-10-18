#!/usr/bin/env python3
"""Build a private test PKG; game data is included only with --include-data."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET

p=argparse.ArgumentParser()
p.add_argument('--include-data',action='store_true')
p.add_argument('--release-kit',type=Path,help='Reviewed relinking kit; makes data-free v0.4.0 release')
a=p.parse_args()
if a.release_kit and a.include_data:
    p.error('A public release must never include game data')
root=Path(__file__).resolve().parents[1]
sdk=Path(os.environ.get('OO_PS4_TOOLCHAIN','/opt/openorbis/OpenOrbis/PS4Toolchain'))
stack=root.parent/'ScummVM-PS4/.tools/opengnm-stack'
stage=Path(tempfile.mkdtemp(prefix='avp-pkg-',dir=root/'build'))
dist=root/'dist'
dist.mkdir(exist_ok=True)
content='IV0000-AVPS00001_00-ALIENSVSPRED0001'
tool=sdk/'bin/linux/PkgTool.Core'
files=[]
def run(*args):
    subprocess.run(list(map(str,args)),cwd=stage,check=True)
def copy(source,name):
    target=stage/name
    target.parent.mkdir(parents=True,exist_ok=True)
    shutil.copyfile(source,target)
    files.append(name)
copy(root/'build/runtime/eboot.bin','eboot.bin')
copy(root/'assets/scummvm-controller-only.txt','scummvm-controller-only.txt')
copy(root/'engine/LICENSE','licenses/AvP-Rebellion.txt')
copy(sdk/'LICENSE','licenses/OpenOrbis.txt')
for name in ('vulkan-ps4','opengnm','opengnm-psbc'):
    copy(stack/name/'LICENSE','licenses/'+name+'.txt')
copy(root.parent/'Doom3-Ps4/build/openal-soft-1.23.1/COPYING','licenses/OpenAL.txt')
if a.release_kit:
    candidate=json.loads((a.release_kit/'MANIFEST.json').read_text())
    if hashlib.sha256((root/'build/runtime/eboot.bin').read_bytes()).hexdigest()!=candidate['tested_eboot_sha256']:
        raise SystemExit('Release executable differs from the tested candidate')
    for notice in sorted((a.release_kit/'licenses').glob('*')):
        if notice.is_file():
            name='licenses/'+notice.name
            if name in files: files.remove(name)
            copy(notice,name)
    copy(root/'docs/THIRD-PARTY.md','licenses/THIRD-PARTY.md')
    copy(root/'docs/INSTALL.md','INSTALL.txt')
for name in ('sce_sys/about/right.sprx','sce_module/libc.prx','sce_module/libSceFios2.prx'):
    copy(sdk/'samples/hello_world'/name,name)
run('magick',root/'assets/icon-generated-v02.png','-resize','512x512!',
    '-alpha','off','-strip','-depth','8','PNG24:'+str(stage/'sce_sys/icon0.png'))
files.append('sce_sys/icon0.png')
if a.include_data:
    data=root/'build/data'
    for folder in ('avp_huds','avp_rifs','fastfile'):
        sources=sorted((data/folder).rglob('*'))
        if not sources: raise SystemExit(f'Missing game data: {folder}')
        for src in sources:
            if src.is_file(): copy(src,'data/'+src.relative_to(data).as_posix())
    copy(data/'language.txt','data/language.txt')
    if (data/'cd tracks.txt').exists(): copy(data/'cd tracks.txt','data/cd tracks.txt')
sfo=stage/'sce_sys/param.sfo'
run(tool,'sfo_new',sfo)
for key,kind,size,value in [
    ('APP_TYPE','Integer',4,'1'),('APP_VER','Utf8',8,'00.04'),('ATTRIBUTE','Integer',4,'0'),
    ('CATEGORY','Utf8',4,'gd'),('CONTENT_ID','Utf8',48,content),('DOWNLOAD_DATA_SIZE','Integer',4,'0'),
    ('SYSTEM_VER','Integer',4,'0'),('TITLE','Utf8',128,'Aliens versus Predator' if a.release_kit else 'Aliens versus Predator - Test'),
    ('TITLE_ID','Utf8',12,'AVPS00001'),('VERSION','Utf8',8,'00.04')]:
    run(tool,'sfo_setentry',sfo,key,'--type',kind,'--maxsize',size,'--value',value)
files.append('sce_sys/param.sfo')
# Write XML directly: data filenames include spaces.
project=ET.Element('psproject',fmt='gp4',version='1000')
volume=ET.SubElement(project,'volume')
ET.SubElement(volume,'volume_type').text='pkg_ps4_app'
ET.SubElement(volume,'volume_id').text='PS4VOLUME'
ET.SubElement(volume,'volume_ts').text='2026-10-18 00:00:00'
ET.SubElement(volume,'package',content_id=content,passcode='0'*32,storage_type='digital50',app_type='full')
nodes=ET.SubElement(project,'files',img_no='0')
dirs=ET.SubElement(project,'rootdir')
for name in files:
    ET.SubElement(nodes,'file',targ_path=name,orig_path=str(stage/name))
    parent=dirs
    for part in Path(name).parts[:-1]:
        child=next((x for x in parent if x.get('targ_name')==part),None)
        if child is None: child=ET.SubElement(parent,'dir',targ_name=part)
        parent=child
tree=ET.ElementTree(project)
ET.indent(tree)
tree.write(stage/'project.gp4',encoding='utf-8',xml_declaration=True)
run(tool,'pkg_build','project.gp4',stage)
package=dist/('AvP-PS4-v0.4.0.pkg' if a.release_kit else ('AvP-PS4-0.4-Private-Test.pkg' if a.include_data else 'AvP-PS4-0.4-NoData.pkg'))
shutil.copyfile(stage/(content+'.pkg'),package)
run(tool,'pkg_validate',package)
# PkgTool appends PUBTOOLINFO/PUBTOOLVER while building; record the final SFO.
listing=subprocess.check_output([str(tool),'pkg_listentries',str(package)],text=True)
indices={line.split()[-1]:line.split()[3] for line in listing.splitlines() if line.startswith('0x')}
run(tool,'pkg_extractentry',package,indices['PARAM_SFO'],sfo)
def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream,'sha256').hexdigest()
manifest={'package':str(package),'sha256':sha(package),'contains_game_data':a.include_data,
          'physical_ps4_tested':False,'stage':str(stage),'files':{name:sha(stage/name) for name in files}}
package.with_suffix('.manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
package.with_suffix('.pkg.sha256').write_text(manifest['sha256']+'  '+package.name+'\n')
print(package,manifest['sha256'])
