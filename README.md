# Diesel

Diesel is a Quake III Arena map viewer. It started as a small OpenGL experiment and grew into a feature-complete viewer: volumetric fog, mirrors and portals, Quake III shaders, and the usual pk3 / bsp content path. It is not a game. The work went into look, speed, and supported map features.

Copyright 2002, 2003–2026 Mathias Heyer. Licensed under the GNU General Public License v2 (see [`diesel/LICENSE`](diesel/LICENSE)).

## Running

Build the executable (below), then place it and `default.jpg` in a Quake III base directory.

Diesel looks for:

- pk3 archives under `baseq3/`
- `q3config.cfg` and `autoexec.cfg` (key bindings)
- the shader `gfx/2d/bigchars` under `scripts/`
- `default.jpg`, used when a texture is missing

Add more search paths at runtime with `addPaks <directory>`. Each added directory is scanned for pk3 files.
That alow you to add Quake3 mods.

## Building

CMake is the supported build. From the repository root:

```bat
cmake -S . -B _build -G "Visual Studio 18 2026" -A x64
cmake --build _build --config Debug --parallel
```

Debug produces `DieselD.exe`; other configs produce `Diesel.exe`. First configure fetches zlib, libjpeg-turbo, and OpenAL Soft into `3rdParty/`. 

Details, generators, and the third-party prefix layout are in [`diesel/BUILDING.md`](diesel/BUILDING.md).

## Layout

| Path | Role |
|------|------|
| [`diesel/`](diesel/) | Viewer sources. Historical notes in [`diesel/Readme.txt`](diesel/Readme.txt) |
| [`dxshell/`](dxshell/) | Win32 / OpenGL application framework (window, main loop, VFS) |
| [`3rdParty/`](3rdParty/) | Fetches and builds zlib, libjpeg-turbo, and OpenAL Soft |
| [`licenses/`](licenses/) | Third-party license texts |

The sources accumulated over a long time and do not follow one style. Older DirectDraw / Direct3D / DirectSound pieces in DXShell are not on the CMake build path.

## Credits

- id Software, for the Quake and Quake II source releases
- the Aftershock project
- Tim Ferguson, RoQ decoder
- Paul Nettle, memory manager
- Jean-loup Gailly, Mark Adler, and Gilles Vollant (zlib / minizip)
- the Independent JPEG Group and the libjpeg-turbo project
- Chris Robinson and contributors (OpenAL Soft)

Notices and license texts: [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) and [`licenses/`](licenses/).

Special Thanks to Victor Luchits, Martin Kraus, Dawid Kozlowski, and friends.

## Contact

sonode@gmx.de
