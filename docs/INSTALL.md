# AvP PS4 v0.4.0 — installation

Unofficial native port by Nichlas Ek. Original game and source copyright
Rebellion, 1999–2000. Noncommercial use only. You need your own copy of the PC
game and a PS4 environment capable of running OpenOrbis homebrew.

The PKG includes the engine, generated icon and runtime modules, NOT game data.
Install AvP-PS4-v0.4.0.pkg using your console's usual homebrew package installer.
Title ID: AVPS00001. Application version: 00.04. It replaces the earlier test
application with the same title ID; back up your saves before installation.

Prepare your own disc data on a PC using the source repository:

```sh
python3 scripts/prepare-source.py
python3 scripts/import-media.py /absolute/path/to/your/AvP_disc1.iso
```

The importer needs 7z and a C++ compiler. See the repository README for build
dependencies. Copy the CONTENTS of build/data, not the ISO, to one of:

- Internal HDD: /data/avp/gamedata/
- USB root: avp/gamedata/ (seen on PS4 as /mnt/usb0/avp/gamedata/, etc.)

The data root contains avp_huds/, avp_rifs/, fastfile/ and language.txt. Preserve
the importer's lowercase filenames and compatibility-adjusted language file.
It refuses an existing import destination to avoid overwriting your files.

The game checks HDD first, then USB0 through USB7. Missing, empty or unreadable
required core files allow fallback to the next root. It selects one root, never
mixes HDD and USB files, and does not guarantee recovery from corrupted or
missing non-core files after a root has been selected.

Profiles/saves always use /data/avp/.avp on HDD. Startup log:
/data/avp/bootstrap-v04.log. Data and saves are not bundled inside the app.

Controls: left stick move, right stick look; Cross jump; Circle/R3 crouch or
Alien wall-walk; Square operate; R2/L2 attacks; Options pause/back. R1/L1 select
weapons for Marine/Predator, or wall-walk/jump for Alien. L3 holds precision
movement. Triangle and D-pad have species-specific actions; full table in README.

Physical-console startup and gameplay were reported working by the tester.
Full campaigns, save/load recovery, all weapons/vision modes, long sessions and
physical USB fallback have not been exhaustively tested. FMV and CD music are
not implemented. Multiplayer is not validated. Decal depth artifacts are possible.

If redistributing the release, retain notices and the source/relink companion.
Never include your retail data, ISO, or the older data-containing private PKGs.
