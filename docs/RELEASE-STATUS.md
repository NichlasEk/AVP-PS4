# Public release status

The source repository and generated icon are available. **No public binary
release is approved yet.** The existing local 0.4 NoData package is a tested
candidate, not a completed redistribution audit.

## Working candidate

The tester reports successful startup and apparently trouble-free gameplay on a
physical PS4. This is not exhaustive campaign or compatibility testing.

- App version: `00.04`; title ID: `AVPS00001`.
- EBOOT SHA-256: `d6fa48518924b6807ffcb1b845e78ade95281e4a1690f989239404120d41b1b6`.
- Local NoData PKG SHA-256: `6ea5ed7bc71aa666dd40a014ba7fbff56629d4887f43edffc533c4f56702c750`.
- The release audit does not change the tested executable.
- Data lookup: `/data/avp/gamedata`, then `/mnt/usb0/avp/gamedata` through
  `/mnt/usb7/avp/gamedata`. Save/profile root: `/data/avp/.avp` on HDD.

## Binary distribution gates

1. **Graphics glue licensing:** `CMakeLists.txt` compiles the sibling
   `ScummVM-PS4/compat/ps4gl` sources into AvP. That project's README declares
   its project code GPL-3.0-or-later; no separate permission was found in the
   inspected graphics files. Rebellion's AvP license prohibits commercial use.
   Do not assume these terms permit distribution of the combined executable.
   Establish provenance and obtain a compatible license/exception from all
   relevant rights holders, or replace the affected code. Repository ownership
   alone does not establish ownership of every contribution.
2. **OpenAL Soft 1.23.1:** preserve notices and provide corresponding library
   source, local changes and build instructions, plus sufficient application
   objects/source and dependencies for relinking with a modified library.
   See [the upstream license, section 6](https://raw.githubusercontent.com/kcat/openal-soft/1.23.1/COPYING).
   Merely including COPYING in the PKG is not sufficient.
3. **Other dependencies:** finish source/provenance and license inventory for
   SDL, OpenGNM, Vulkan, shader compiler/bridge, shared audio/TLS glue and SDK
   runtime modules. Tool licensing is not automatically the license of every
   runtime component. Confirm the actual bundled modules are redistributable.
4. **Final artifact:** include required notices and companions, extract and
   audit the complete payload, verify absence of retail data and confirm the
   tested EBOOT hash is unchanged. If executable code must change, repeat the
   physical-console test before calling it the tested release.

No ISO, imported language text, retail assets, original executables, private
logs or old private PKGs belong in the public repository or release assets.

This document records engineering release gates, not a legal clearance.
