# v0.4.0 release verification

This early playable release uses the exact EBOOT from the physically tested 0.4
candidate. The tester reports successful startup and apparently trouble-free
gameplay. This is not exhaustive campaign or compatibility testing.

- Title ID `AVPS00001`, app version `00.04`.
- EBOOT SHA-256: `d6fa48518924b6807ffcb1b845e78ade95281e4a1690f989239404120d41b1b6`.
- Release PKG SHA-256: `f0c0d83321a493e732a5cf7a598a8d52548a4efc198abfe1dfe7ab2204b92379`.
- Packaging changes: title no longer says Test, installation guide and expanded
  license notices. This newly packaged container has not separately been
  installed on the physical console; the embedded executable is unchanged.
- Package extraction verifies every listed payload hash and rejects unexpected
  files. No retail game assets, original executables or language text are bundled.
- Platform tests exercise HDD/USB selection, save-directory errors and 131072
  stick samples with species mappings, held attacks, disconnect and menu guards.
- Relinking the captured application objects/libraries reproduces the original
  ELF byte for byte. Rebuilding the provided OpenAL source and linking that
  replacement succeeds; it does not replace the released EBOOT.

## Licensing work

Nichlas Ek confirmed ownership and authorized MIT licensing for original ps4gl
code and the audio/TLS glue. Grants are committed upstream in ScummVM-PS4
`2ea6dc7` and Doom3-Ps4 `446a443`; third-party code is not relicensed.

The audio/TLS files first appear as additions in Doom3-Ps4 commit `92d338b`.
Checks against the online id-Software/DOOM-3 and dhewm/dhewm3 trees found no
matching file paths; distinctive functions were absent from local dhewm3 source.
These checks support the owner's confirmation, not a universal originality proof.

The source/relink companion contains the locally modified OpenAL source, build
instructions, captured application objects and independent static libraries,
plus dependency source/notices and OpenOrbis module sources. Runtime modules
match the SDK's corresponding built module files byte for byte.
See [THIRD-PARTY.md](THIRD-PARTY.md) for provenance limits and license inventory.

## Data and test boundaries

HDD `/data/avp/gamedata` takes priority, followed by USB0 through USB7.
Saves/profiles remain under `/data/avp/.avp` on HDD. Physical USB fallback,
full campaigns, all weapons/vision modes, save/load recovery, long sessions and
suspend/resume have not been exhaustively tested. FMV/CD music are absent;
multiplayer is unvalidated and decal depth artifacts remain possible.
