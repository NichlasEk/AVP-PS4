#!/usr/bin/env python3
"""Capture the tested link inputs and OpenAL source in a portable, local kit.

This assembles evidence, not legal clearance. Never publish until the release
checklist and the licenses of every captured input have been reviewed.
"""
import hashlib
import json
import os
import re
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SDK = Path(os.environ.get('OO_PS4_TOOLCHAIN', '/opt/openorbis/OpenOrbis/PS4Toolchain'))
DOOM = ROOT.parent / 'Doom3-Ps4'
SCUMM = ROOT.parent / 'ScummVM-PS4'
BUILD = ROOT / 'build/ps4'
EXPECTED = 'd6fa48518924b6807ffcb1b845e78ade95281e4a1690f989239404120d41b1b6'

def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def main():
    if digest(ROOT / 'build/runtime/eboot.bin') != EXPECTED:
        raise SystemExit('Not the physically tested executable; review before continuing')
    output = Path(tempfile.mkdtemp(prefix='avp-relink-', dir=ROOT / 'build'))
    def copy(source, name):
        target = output / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, target)
        return name
    command = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'commands'], text=True).splitlines()[-1]
    args = shlex.split(command)
    # CMake wraps this command in ': && compiler ... && :'. Never execute a shell.
    if args[:2] != [':', '&&'] or args[-2:] != ['&&', ':']:
        raise SystemExit('Unexpected link command wrapper')
    args = args[2:-2]
    args[0] = 'clang++'
    rewritten = []
    objects = 0
    for arg in args:
        path = Path(arg)
        if arg.endswith('.o') and not path.is_absolute():
            objects += 1
            arg = copy(BUILD / path, f'objects/{objects:04d}-{path.name}')
        elif arg.endswith('.a') and path.is_absolute() and not path.is_relative_to(SDK):
            arg = copy(path, 'libraries/' + path.name)
        elif arg == 'avp':
            arg = 'avp-relinked.elf'
        arg = arg.replace(str(SDK), '@SDK@')
        rewritten.append(arg)
    (output / 'link-arguments.json').write_text(json.dumps(rewritten, indent=2) + '\n')
    shutil.copytree(DOOM / 'build/openal-soft-1.23.1', output / 'openal-soft-1.23.1',
                    ignore=shutil.ignore_patterns('.git', '*.orig', '*.rej', '__pycache__'))
    copy(DOOM / 'patches/003-openal-ps4.patch', 'openal-ps4.patch')
    for name in set(re.findall(r'^\+\+\+ b/(.+)$', (output / 'openal-ps4.patch').read_text(), re.M)):
        changed = output / 'openal-soft-1.23.1' / name
        changed.write_text('/* PS4 adaptations by Nichlas Ek, present as of 2026-10-18.\n'
                           ' * See ../openal-ps4.patch; original LGPL terms retained. */\n' + changed.read_text())
    copy(DOOM / 'cmake/openorbis.cmake', 'openal-toolchain.cmake')
    copy(ROOT / 'scripts/relink.py', 'relink.py')
    copy(ROOT / 'scripts/rebuild-openal.sh', 'rebuild-openal.sh')
    copy(ROOT / 'docs/RELINKING.md', 'README.md')
    copy(ROOT / 'LICENSE', 'licenses/AvP-Rebellion.txt')
    copy(ROOT / 'licenses/ps4gl-MIT.txt', 'licenses/ps4gl-MIT.txt')
    copy(DOOM / 'build/openal-soft-1.23.1/COPYING', 'licenses/OpenAL.txt')
    copy(DOOM / 'platform/ps4/LICENSE-audio-tls.MIT', 'licenses/audio-tls-MIT.txt')
    # Source snapshots preserve upstream per-file notices as well as top-level
    # licenses. Only known source roots are included; never game/data trees.
    source_suffixes = {'.c', '.h', '.cpp', '.hpp', '.cc', '.inc', '.S', '.s',
                       '.cmake', '.mak', '.mk', '.py', '.sh', '.txt', '.md',
                       '.in', '.y', '.l', '.json', '.xml', '.png'}
    sources = {
        'ps4gl': SCUMM / 'compat/ps4gl',
        'sdl': SCUMM / '.tools/sdl-input-src',
        'glslang': SCUMM / '.tools/glslang',
        'opengnm': SCUMM / '.tools/opengnm-stack/opengnm',
        'vulkan-ps4': SCUMM / '.tools/opengnm-stack/vulkan-ps4',
        'opengnm-psbc': SCUMM / '.tools/opengnm-stack/opengnm-psbc',
        'Vulkan-Headers': SCUMM / '.tools/opengnm-stack/Vulkan-Headers',
        'SPIRV-Headers': SCUMM / '.tools/opengnm-stack/SPIRV-Headers',
        'openorbis-modules': SDK / 'src/modules',
    }
    for label, directory in sources.items():
        for path in sorted(directory.rglob('*')):
            relative = path.relative_to(directory)
            if not path.is_file() or path.is_symlink() or any(part in
                {'.git', '__pycache__', 'build', 'out', 'node_modules'} for part in relative.parts):
                continue
            if path.suffix in source_suffixes or path.name.startswith(('LICENSE', 'COPYING', 'COPYRIGHT', 'Makefile')):
                copy(path, f'dependency-sources/{label}/{relative}')
    for name in ('audio.cpp', 'audio.h', 'tls_destructors.cpp', 'LICENSE-audio-tls.MIT'):
        copy(DOOM / 'platform/ps4' / name, 'dependency-sources/audio-tls/' + name)
    for name in ('build-ps4-input.sh', 'build-ps4-sdlgl.sh', 'build-ps4-glslang.sh'):
        copy(SCUMM / 'scripts' / name, 'dependency-build-reference/' + name)
    notices = {
        'SDL.txt': SCUMM / '.tools/sdl-input-src/COPYING.txt',
        'glslang.txt': SCUMM / '.tools/glslang/LICENSE.txt',
        'OpenOrbis.txt': SDK / 'LICENSE',
        'musl.txt': SCUMM / '.tools/openorbis-musl-2023-reference/COPYRIGHT',
        'OpenAL-BSD.txt': DOOM / 'build/openal-soft-1.23.1/BSD-3Clause',
    }
    for name in ('opengnm', 'vulkan-ps4', 'opengnm-psbc'):
        notices[name + '.txt'] = sources[name] / 'LICENSE'
    for name in ('libcxx', 'libcxxabi', 'libunwind', 'compiler-rt'):
        notices['LLVM-' + name + '.txt'] = SCUMM / '.tools/openorbis-llvm-v12-reference' / name / 'LICENSE.TXT'
    for name, path in notices.items():
        copy(path, 'licenses/' + name)
    copy(ROOT / 'docs/THIRD-PARTY.md', 'THIRD-PARTY.md')
    copy(ROOT / 'build/upstream/avp-20170505-a1.tar.gz', 'avp-upstream-source.tar.gz')
    for name in subprocess.check_output(['git','-C',str(ROOT),'ls-files'], text=True).splitlines():
        copy(ROOT / name, 'port-source/' + name)
    # Verify the actual modules, rather than inferring provenance from filenames.
    for src, built in [('sce_module/libc.prx', 'libc/libc.prx'),
                       ('sce_module/libSceFios2.prx', 'libSceFios2/libSceFios2.prx'),
                       ('sce_sys/about/right.sprx', 'right/right.sprx')]:
        if digest(SDK / 'samples/hello_world' / src) != digest(SDK / 'src/modules' / built):
            raise SystemExit('SDK module/source-build identity changed: ' + src)
    files = {p.relative_to(output).as_posix(): digest(p)
             for p in sorted(output.rglob('*')) if p.is_file()}
    (output / 'MANIFEST.json').write_text(json.dumps({
        'status': 'source and relinking companion; see THIRD-PARTY.md',
        'tested_eboot_sha256': EXPECTED, 'application_objects': objects,
        'original_elf_sha256': digest(BUILD / 'avp'), 'files': files,
    }, indent=2) + '\n')
    print(output)

if __name__ == '__main__':
    main()
