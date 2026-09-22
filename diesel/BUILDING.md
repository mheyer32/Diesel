# Building Diesel

CMake is the supported build path. Legacy Visual Studio / PropertySheets projects under `diesel/` and `dxshell/` still exist but are not required.

## Layout

| Path | Role |
|------|------|
| `diesel/` | Engine sources and this historical `Readme.txt` |
| `dxshell/include/` | DXShell framework (headers + sources) |
| `3rdparty/` | Fetches/builds ZLIB, JPEG, and OpenAL into a shared prefix |
| `propertysheets/` | Legacy MSBuild property sheets |

## Configure and build

From the repository root (`Diesel/`):

```bat
cmake -S . -B _build -G "Visual Studio 18 2026" -A x64
cmake --build _build --config Debug --parallel
```

Targets:

- `DXShell` — static library
- `Diesel` — Win32 executable (`DieselD.exe` in Debug, `Diesel.exe` otherwise)

Qt Creator / NMake kits work the same via their CMake kit; use a matching architecture (x86 or x64).

## Third-party libraries

`CMakeLists.txt` calls `thirdparty_ensure(ZLIB JPEG OpenAL)`. On first configure, missing packages are fetched and installed under:

- `3rdparty/_install-x64` — 64-bit kits
- `3rdparty/_install-x86` — 32-bit kits

Matching build trees live in `3rdparty/_build-<arch>/`. Sources cache in `3rdparty/_src/`.

## OpenAL

Sound uses OpenAL Soft (static, via 3rdParty). No runtime `fmod.dll` or `OpenAL32.dll` is required for the CMake build. WAV music/SFX are loaded through DXShell's VFS (`CFileManager` / pk3).

## Third-party attributions

See [`THIRD_PARTY_NOTICES.md`](../THIRD_PARTY_NOTICES.md) and the [`licenses/`](../licenses/) directory for zlib, libjpeg-turbo, OpenAL Soft, and related notices (including the required Independent JPEG Group statement).

## Running

Same as the original readme: put the executable with `default.jpg` in a Quake III–based directory so `baseq3/*.pk3`, `q3config.cfg`, and `autoexec.cfg` can be found.
