# OpenAL relinking kit

This companion contains no retail game assets. It is not a standalone game or
complete SDK. Retain it and its licenses alongside redistributed binaries.

Install OpenOrbis v0.5.3 (LLVM 18 distribution), clang/LLVM, CMake, Ninja and
Python 3. Set `OO_PS4_TOOLCHAIN` to the installed SDK. The SDK provides the
system runtime and firmware-service link stubs; do not substitute Sony SDK
files or distribute firmware dumps.

From the extracted kit:

```sh
python3 relink.py
# Edit openal-soft-1.23.1, then rebuild and relink:
bash rebuild-openal.sh
# Or use your own compatible OpenAL static library:
python3 relink.py --openal /absolute/path/to/libopenal.a
```

The output is `avp-relinked.elf`, separate from the original tested executable.
Use OpenOrbis create-fself to generate an installable eboot:

```sh
create-fself -in=avp-relinked.elf -out=avp-relinked.oelf --eboot eboot.bin --paid 0x3800000000000011
```

Package that eboot using the AvP repository's packaging workflow. Modified builds
need their own console testing; they are not covered by the original test report.

The library tree is the locally modified OpenAL Soft 1.23.1 source, not pristine
upstream. `openal-ps4.patch` records PS4 context/TLS and semaphore adaptations
present in the 2026-10-18 candidate. It is already applied; do not apply twice.
Source comments mark the modified files; these comments were added when preparing
the companion and do not change executable code. OpenAL and modifications to it
retain its LGPL terms. No additional restriction
on modifying OpenAL or debugging such modifications is imposed by this kit.
AvP's original source remains copyright Rebellion, 1999–2000, under the included
noncommercial license. Other components retain their respective licenses.
