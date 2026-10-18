# Third-party notices and source companion

Aliens versus Predator (1999–2000) was created by and is copyright Rebellion.
The original source and this port are for noncommercial use under the repository
LICENSE. This does not change the licenses of the independent libraries below.
This is an unofficial port, not an endorsement by Rebellion or Sony.

| Component | Terms / accompanying material |
| --- | --- |
| Original AvP / Icculus source | Rebellion noncommercial license; pinned source and local patches in the source workflow |
| Nichlas Ek's ps4gl | MIT alternative authorized 2026-10-18; ScummVM-PS4 commit `2ea6dc7` |
| Nichlas Ek's audio.cpp, audio.h, tls_destructors.cpp | MIT, authorized 2026-10-18; Doom3-Ps4 commit `446a443` |
| OpenAL Soft 1.23.1 | LGPL v2 or later as specified by its source notices; COPYING and BSD-3Clause included; modified source and application relinking kit accompany the release |
| SDL / SDL-PS4 | zlib-style license; source revision `7774cf3e7d04448c44d569c50d1f9a4532a2efe7`, PS4 joystick changes and original ps4gl video adapter |
| OpenGNM / Vulkan-PS4 | MIT; individual third-party notices retained in source companion |
| PSBC / Mesa shader compiler | MIT and per-file upstream notices; source companion retains notices; private archive namespaces C11 threading symbols |
| glslang | Mixed permissive licenses, see glslang.txt and per-file notices; revision `fa9c3deb49e035a8abcabe366f26aac010f6cbfb`; yydebug symbol renamed |
| LLVM libc++, libc++abi, libunwind, compiler-rt | Apache-2.0 with LLVM exceptions and retained component notices |
| musl | MIT and retained third-party copyright notices |
| OpenOrbis attribution/dummy modules | GPLv3; corresponding module source and Makefiles in companion; these are separate SDK-built modules, not Sony firmware dumps |

OpenAL Soft is copyright its contributors, including Chris Robinson. This
application statically links OpenAL Soft. Its library source, local modifications,
license and application objects/libraries for relinking are supplied together in
the companion. Users may modify the library and relink for their own use; no
additional restriction on reverse engineering to debug those modifications is
imposed by this port. Preserve the companion when redistributing a binary.

The installed SDK is the OpenOrbis v0.5.3 LLVM 18 distribution. Its archive SHA-256
is `562e0bf5088d11041534059ab8963029a355786736d6b28fa62250fe5d02cb2d`.
The LLVM runtime source tag is OpenOrbis v1.2
(`83e2569f0c062d980ae142d8dc583f6b8733be35`). The musl revision
`779f95174b44d39e6a8a788b36289cf4768944a9` is inferred from the historical
workflow/header match, not independently proven by a reproducible build.
The relinking kit requires an installed SDK; system runtime libraries and
firmware services are not supplied by that kit.

Sources: [OpenAL 1.23.1](https://github.com/kcat/openal-soft/tree/1.23.1),
[OpenOrbis v0.5.3](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/tree/v0.5.3),
[OpenOrbis LLVM v1.2](https://github.com/OpenOrbis/llvm-project/tree/v1.2),
[SDL-PS4 v1.0](https://github.com/OpenOrbis/SDL-PS4/tree/v1.0).

The generated project icon is included; no retail game assets or original game
executables are included. Alien/Predator names and depicted characters remain
associated with their respective rights holders; generated artwork does not
grant trademark or franchise rights.
