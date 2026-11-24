# Burnout - XBE Recomp

An unofficial native PC port of the original Xbox version of Burnout (European release), built with the [Xbox XBE Recomp](https://github.com/maxscipio/XboxXBERecomp) SDK.

> [!NOTE]
> This project does not include any game assets. You must provide the files from your own legally acquired copy of the game.

## Installation

- Put your game files in the `assets` folder: the disc image (`.iso`), or `default.xbe` with the game's folders beside it.
- Run `Burnout.exe`.
- Settings are in `settings.toml` beside the exe, and in the F1 settings window.

## Features

- Direct3D 12 renderer with resolution scale, FXAA, SMAA, MSAA, sharpening, a CRT filter and a frame rate cap.
- Aspect ratio: 4:3, 16:9 or fill the screen (21:9). The 3D view is widened and the HUD and menus stay in proportion in a centred 4:3 part.
- Xbox, PlayStation and Switch controllers, with hot plugging and rumble.
- F10 shows the frame rate. The game runs at up to 60 frames a second.

## Known limitations

1. In 16:9 and 21:9 the HUD stays in a centred 4:3 part. Its items still need to be moved to the sides of the screen.
2. The game timer sometimes stops all of a sudden.
3. Once the game finishes, the menu keeps getting clicked.

## System requirements

- Windows 10 64-bit
- A GPU with Direct3D 12 support

## Building

1. Clone this repository.
2. Put the game files in `assets` (`default.xbe` must be there, with the game's folders beside it).
3. Install Visual Studio 2022 with **Desktop development with C++**, **C++ Clang tools for Windows** and **C++ CMake tools for Windows** (`build.bat` offers to add the last two).
4. Run `build.bat`. The first build downloads the SDK ([maxscipio/XboxXBERecomp](https://github.com/maxscipio/XboxXBERecomp)) and the libraries it needs, recompiles the game and takes a while (30 to 90 minutes). The exe is written to `out\build\release`.

To build against a local SDK checkout, put it in `sdk` here, or in `..\XboxXBERecomp`.

`tools\iter.ps1` rebuilds, copies the exe to `out\run` and runs it.

See `docs\PORTING_NOTES.md` for how the game was brought up and `docs\TODO.md` for open items.

## Credits

- [Xbox XBE Recomp](https://github.com/TGP482/XboxXBE-Recomp), the recompiler and runtime this is built on.
- [XenonRecomp](https://github.com/hedge-dev/XenonRecomp) for the overridable function layout and instruction hooks.
- [Zydis](https://github.com/zyantific/zydis) for decoding x86 instructions, [toml++](https://github.com/marzer/tomlplusplus) for the configs, [Dear ImGui](https://github.com/ocornut/imgui) for the settings overlay and [SDL](https://github.com/libsdl-org/SDL) for the window, input and controllers.
