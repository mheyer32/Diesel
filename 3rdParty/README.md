# 3rdParty

Standalone CMake project that fetches / builds / installs third-party libraries
into a **shared prefix** (`3rdParty/_install` by default). Game projects
(GL3, DXShell, Doom3, …) only `find_package` against that prefix.

## Layout

| Path | Role |
|---|---|
| `versions.cmake` | Pinned git tags / URLs + min versions. |
| `_src/` | Downloaded sources (gitignored). |
| `_build/` | 3rdParty build tree (gitignored). |
| `_install/` | Shared install prefix (gitignored). |
| `_install/include/` | Headers (shared across configs). |
| `_install/lib/<CONFIG>/` | **Windows:** `Debug` / `RelWithDebInfo` archives. |
| `_install/lib/cmake/` | Package config files (shared). |
| `cmake/UseThirdParty.cmake` | Consumer helper: auto-build prefix if missing/stale. |

On Linux, libraries install to plain `_install/lib/` (single config).

### Windows `lib/<CONFIG>/` (how it works)

Dependencies install to normal `lib/` (CMake exports stay correct). Immediately
after each ExternalProject install, `RelocateConfigLibs.cmake` runs with
`-DCONFIG=$<CONFIG>` — that genex is evaluated on the **superbuild** install
command at build time — moves `*.lib` into `lib/<CONFIG>/`, and patches
`*Targets*.cmake` `IMPORTED_LOCATION` paths.

Do **not** put `$<CONFIG>` or `${CMAKE_INSTALL_CONFIG_NAME}` into
`CMAKE_INSTALL_LIBDIR`: those strings get baked into export files and break
`find_package` consumers.

## Policy

- **Windows** (`PREFER_SYSTEM_PACKAGES=OFF`): fetch/build **Debug** and
  **RelWithDebInfo** (release + debug symbols) into `lib/<CONFIG>/`.
- **Linux** (`PREFER_SYSTEM_PACKAGES=ON`): use a system package only if
  `find_package(<name> <min> ...)` succeeds. Minimums live in `versions.cmake`
  as `THIRDPARTY_<Name>_MIN` (e.g. `THIRDPARTY_Boost_MIN 1.87`). Too old or
  missing → fetch the pinned version.
- **Force fetch**: `-DTHIRDPARTY_FORCE_BUILD=Boost;OpenVR` skips the system
  check for those names even when a new enough package is installed.
- Windows-only packages: `nvapi`, `LibOVR` (from `OVR_SDK_ROOT`).

## Manual build

```bat
cmake -S 3rdParty -B 3rdParty/_build -DCMAKE_INSTALL_PREFIX=3rdParty/_install
cmake --build 3rdParty/_build --config RelWithDebInfo --parallel
cmake --install 3rdParty/_build --config RelWithDebInfo
cmake --build 3rdParty/_build --config Debug --parallel
cmake --install 3rdParty/_build --config Debug
```

Force a specific package off the system:

```bat
cmake -S 3rdParty -B 3rdParty/_build -DTHIRDPARTY_FORCE_BUILD=Boost
```

Point several projects at the same prefix by setting `THIRDPARTY_INSTALL_PREFIX`
(or `CMAKE_PREFIX_PATH`) to that directory.

## Consumer usage

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/cmake")
include(UseThirdParty)
thirdparty_map_debug_to_release()  # maps Release -> RelWithDebInfo, etc.
thirdparty_ensure(ZLIB PNG Boost)  # empty = every package in 3rdParty
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
# ...
```

Only named packages (plus implied deps: PNG → ZLIB, Vorbis → Ogg) are
fetched. The prefix is shared: a later consumer that needs more packages
adds them; existing ones stay. `thirdparty_ensure()` with no arguments
still builds everything.

`thirdparty_ensure()` hashes `versions.cmake`. Changing pins (or
`THIRDPARTY_LAYOUT_VERSION`) rebuilds the prefix the next time any consumer
configures.

Disable auto-build with `-DTHIRDPARTY_AUTO_BUILD=OFF` if you want an explicit
failure when the prefix is missing.

## Updating

1. Edit `versions.cmake`.
2. Reconfigure any consumer (or rebuild 3rdParty manually).
3. Reconfigure / rebuild the game projects.
