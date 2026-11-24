# Burnout recomp: open items

Working now: video (colours, car, road, buildings, reflections), sound (music and effects), controller, Esc to quit.

## Bugs and unknowns

- **The application sometimes runs twice, and the first run often has no sound.** To debug. Starting points:
  - A second run can come from the runtime's relaunch: `HalReturnToFirmware` (`sdk\runtime\kernel\xboxkrnl.cpp`, ~1180) calls `Relaunch` (`sdk\runtime\kernel\boot.cpp`, ~171), which starts `Burnout.exe --relaunched`. The old window stays up until the new one has shown its first frames (`sdk\runtime\app\handover.cpp`).
  - Check whether the game calls `HalReturnToFirmware` or `XLaunchNewImage` on its own, for example a reboot after a first-run step, and why. `logs\burnout.log` keeps one log per session, and a relaunch continues its predecessor's.
  - The first run having no sound could be a DirectSound or XAudio2 init race, or audio started in an instance that is then handed over. Compare the two runs' logs, and check that a relaunched run initialises audio again.
  - There is no setup screen any more, so nothing differs between the two runs on that side.
- **NaN texture factor** in `sub_0007D39E`. Cause not found.
- **Rare crash:** `C0000005` on a guest thread, seen once about 145 s into an earlier run. Not reproduced or diagnosed.
- **Unmapped 4361 render-state slots.** `d3d8.cpp` only maps the slots Burnout uses so far (deferred block, material sources, ambient, points, complex block). Other slots are unverified.
- **`CreateSoundBuffer` churn.** About 1,300 calls in a few seconds. It may be normal per-effect buffer creation, or a leak. Unchecked.
- **DirectSound fallback coverage.** Only the `BUFFER` methods fall back to the original recompiled body when the object was made by library code. The stream and `CreateSoundBuffer` HLEs have not needed it so far, but could.
- **Other layout keys.** `sdk\xdk\4361\layout.toml` has only the keys Burnout has needed. Any key left out traps on first use, so new screens or modes may trip one.

## Features and ports

- **Multiplayer:** split-screen and system link. Untested; needs the 4361 layout keys and the network path below.
- **Network support:** the `xnet` HLE in `sdk\runtime\xdk\xnet.cpp` (system link, `join_ip` setting) for Burnout's network modes.
- **Graphics post-processing:** FXAA, SMAA, MSAA, sharpening and a CRT filter are in the SDK's F1 window. Still open: ambient occlusion and soft shadows (needs Burnout's depth projection and shadow-map format measured from an F3 capture) and bloom.
- **Ultrawide:** 4:3, 16:9 and 21:9 work (see `PORTING_NOTES.md`). Still to check: a real 21:9 or wider monitor, the pause menu and other tracks. The game's other uses of the 16:9 and 4:3 constants (`0xA2F44`, `0xA331C`, `0xDB140`, `0xE2F4C`) are untouched.
- **Frame interpolation (to study):** the game steps its logic once per swap with a fixed dt, so a higher frame rate needs either a higher step rate (tried, choppy, removed) or frames drawn between two logic steps. Interpolating would keep the game at 60 steps a second and show 120 or more: keep the previous and current object transforms and camera, blend them by the fraction of the step elapsed. Open questions: where the game keeps its transforms (world matrices per draw, or objects), what to do with particles, HUD and cuts (no blending across a camera cut), and the cost of drawing each frame twice on the host side (the draw path is the bottleneck, so this needs the draw cost cut first or a cache of the previous frame's draws).
- **Frame rate:** the F1 frame rate cap drives the game (`src\common\graphics.cpp`): unlimited and caps above 60 mean 60, and 30 shows every second step (raising the step rate above 60 was choppy and is removed). When the machine cannot keep the steps up the dt follows the rate it manages, so the game keeps its speed. To check: gameplay speed at each cap, and whether the PAL flag at `0x8A7834` is set at run time.
- **Port to Linux:** the SDK has a Vulkan backend (`XBE_BACKEND=vulkan`, no `g_wide_2d` handling yet); the rest needs replacements for the Win32 window, `windows.h` kernel and audio code, and the settings and input paths.
- **Port to ARM:** the recompiled x86 code, and any x86-specific parts of the runtime, need an ARM64 build (Windows on ARM first, then Linux).

## Untested

- Menus, replays, other tracks, split-screen, save and load, and long play sessions. Only a track view has been checked.
- Performance on other GPUs. Only an Intel Iris Xe has been tried.

## Memory

- The game uses about 350 MB. About 192 MB of that is the GPU upload ring (`kUploadSize` 64 MB Ã— 3 frames in `sdk\runtime\gpu\d3d12.cpp`).
- Trying 16 MB with a 4 MB reserve cut it to about 195 MB private, but it stuttered in play, so it is reverted to 64 MB / 8 MB.
- Next: run with `XBE_PERF=1`, read the per-frame upload MB and overflow count in `logs\game.log` during a race, then pick a size that avoids stalls (try 32 MB). Other options: trim the texture cache (2048 textures, 10 s idle), lower the render scale.

## Housekeeping

- The recompiler's input is `assets\default.xbe` (extracted from the disc image), which stays in the repo's `assets\`. The game runs from `out\run`, whose `assets\` holds only the disc image (a hardlink): a loose `default.xbe` beside the ISO makes the runtime skip mounting the disc. `tools\iter.ps1` rebuilds, copies the exe and DLL to `out\run` and runs it.
