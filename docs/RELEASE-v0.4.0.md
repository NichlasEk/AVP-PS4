# AvP PS4 v0.4.0 — first playable release

An unofficial native OpenOrbis port of Rebellion's Aliens versus Predator (1999),
maintained by Nichlas Ek. Physical PS4 startup and gameplay reported working well.

## Downloads

- **AvP-PS4-v0.4.0.pkg** — installable app with the generated icon; no retail game data.
- **INSTALL.md** — preparing your own data and HDD/USB setup.
- **AvP-PS4-v0.4.0-source-relink.tar.gz** — source, third-party notices, OpenAL build
  material and application relinking kit. Keep this companion when redistributing.
- **SHA256SUMS**, **PACKAGE-MANIFEST.json**, **VERIFICATION.md** — integrity and test evidence.

## Features

- Native 1280×720 Vulkan presentation and OpenAL output.
- Analog DualShock 4 movement/look with species-specific controls.
- HDD-first data lookup at `/data/avp/gamedata`, then USB0–USB7 at `avp/gamedata`.
- Saves/profiles always on internal HDD at `/data/avp/.avp`.
- Native directory initialization fixes the earlier console startup failure.

Bring your own legally obtained PC game data. Use the importer described in
INSTALL.md; do not simply place an ISO on the USB drive.

## Early-release limitations

No FMV or CD music. Multiplayer is unvalidated. Decal depth artifacts are
possible. Full campaigns, long sessions, every vision/weapon mode, save recovery
and physical USB fallback have not been exhaustively tested.

This PKG has expanded notices and a final title, but embeds the exact executable
from the working physical-console candidate. The newly packaged container has
not separately been installed on the console. App version is `00.04`, title ID
`AVPS00001`; back up saves before replacing earlier test builds.

Original game/source copyright Rebellion, 1999–2000. **Noncommercial use only.**
Third-party components retain their own licenses; see THIRD-PARTY.md. This is not
an official Rebellion or Sony product. Do not redistribute retail assets or
earlier private, data-containing test packages.
