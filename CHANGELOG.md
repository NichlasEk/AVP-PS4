# Changelog

## 0.4 — 2026-10-18

- Physical PS4 startup and gameplay reported working by the tester.
- Replace desktop directory initialization with native PS4 calls.
- Load external game data from HDD, with USB fallback; keep saves on HDD.
- Preserve existing saves and log startup checkpoints.
- Include analog DualShock 4 movement, progressive look and species-specific actions.
- Include the generated project icon; keep retail game data out of Git.

Earlier test builds established SDL2/Vulkan rendering and OpenAL audio, repaired
RIF bounds handling and empty weapon-slot selection, and localized the console
startup crash. Versions 0.1 and 0.2 were private, data-containing test packages
and must not be redistributed.
