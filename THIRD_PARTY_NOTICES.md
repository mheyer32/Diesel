# Third-party notices

Diesel links the following third-party libraries (versions match
[`3rdParty/versions.cmake`](3rdParty/versions.cmake) pins used by CMake).
Full license texts are under [`licenses/`](licenses/).

## zlib 1.3.1

- **Project:** https://zlib.net / https://github.com/madler/zlib
- **Authors:** Jean-loup Gailly and Mark Adler
- **License:** zlib License — see [`licenses/zlib-LICENSE.txt`](licenses/zlib-LICENSE.txt)
- **Use:** ZIP / pk3 inflation via zlib and **minizip** (`contrib/minizip`, same zlib license terms; copyright Gilles Vollant et al.)

## libjpeg-turbo 3.1.1

- **Project:** https://libjpeg-turbo.org / https://github.com/libjpeg-turbo/libjpeg-turbo
- **License:** IJG License + Modified (3-clause) BSD — see [`licenses/libjpeg-turbo-LICENSE.md`](licenses/libjpeg-turbo-LICENSE.md)
- **Use:** JPEG texture decode (libjpeg API; TurboJPEG API not used)

**Required IJG notice:** This software is based in part on the work of the Independent JPEG Group.

## OpenAL Soft 1.24.2

- **Project:** https://openal-soft.org / https://github.com/kcat/openal-soft
- **License:** GNU Library General Public License, version 2 (LGPLv2) — see [`licenses/OpenAL-Soft-COPYING.txt`](licenses/OpenAL-Soft-COPYING.txt)
- **Bundled component:** pffft — see [`licenses/OpenAL-Soft-LICENSE-pffft.txt`](licenses/OpenAL-Soft-LICENSE-pffft.txt)
- **Use:** Audio device, buffers, and sources (statically linked)

Corresponding OpenAL Soft source is obtained and built by Diesel’s `3rdParty` fetch (git tag above). Diesel itself is distributed under the GNU General Public License v2 (see source headers).

## Other credits (in-tree / protocol)

| Component | Credit / terms |
|-----------|----------------|
| OpenGL / OpenGL Utility | System graphics API (no redistributed library source) |
| DirectX / DirectInput / Win32 | Microsoft platform SDKs |
| Tim Ferguson RoQ decoder | Freely usable with attribution — see `diesel/roqplayer/roq_read.c` |
| Paul Nettle Memory Manager | Historical credit in `diesel/Readme.txt` (optional debug tooling) |
| id Software Quake / Quake II sources | Inspiration / learning credit in `diesel/Readme.txt` |
