# Pin every fetched dependency here. Changing this file retriggers a 3rdParty
# rebuild the next time a consumer configures.
#
# THIRDPARTY_<Name>_MIN: minimum version accepted from the system when
# PREFER_SYSTEM_PACKAGES is ON. Too-old or version-less failures fall through
# to the pinned fetch below. Leave unset to accept any found version.
#
# THIRDPARTY_LAYOUT_VERSION: bump when the install layout changes (forces
# consumers to rebuild the prefix even if pins are unchanged).
set(THIRDPARTY_LAYOUT_VERSION 6)

set(THIRDPARTY_ZLIB_GIT https://github.com/madler/zlib.git)
set(THIRDPARTY_ZLIB_TAG v1.3.1)
set(THIRDPARTY_ZLIB_MIN 1.2.11)

set(THIRDPARTY_PNG_GIT https://github.com/pnggroup/libpng.git)
set(THIRDPARTY_PNG_TAG v1.6.47)
set(THIRDPARTY_PNG_MIN 1.6.0)

set(THIRDPARTY_JPEG_GIT https://github.com/libjpeg-turbo/libjpeg-turbo.git)
set(THIRDPARTY_JPEG_TAG 3.1.1)
# FindJPEG often reports no version; leave MIN unset or force-build JPEG.
# set(THIRDPARTY_JPEG_MIN 2.1)

set(THIRDPARTY_OGG_GIT https://github.com/xiph/ogg.git)
set(THIRDPARTY_OGG_TAG v1.3.5)
set(THIRDPARTY_Ogg_MIN 1.3.0)

set(THIRDPARTY_VORBIS_GIT https://github.com/xiph/vorbis.git)
set(THIRDPARTY_VORBIS_TAG v1.3.7)
set(THIRDPARTY_Vorbis_MIN 1.3.0)

set(THIRDPARTY_OPENAL_GIT https://github.com/kcat/openal-soft.git)
set(THIRDPARTY_OPENAL_TAG 1.24.2)
set(THIRDPARTY_OpenAL_MIN 1.19)

set(THIRDPARTY_BULLET_GIT https://github.com/bulletphysics/bullet3.git)
set(THIRDPARTY_BULLET_TAG 3.25)
set(THIRDPARTY_Bullet_MIN 3.24)

set(THIRDPARTY_BOOST_URL
    https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
)
set(THIRDPARTY_Boost_MIN 1.87)
set(THIRDPARTY_BOOST_LIBS
    lexical_cast
    regex
    optional
    integer
    pool
    dynamic_bitset
    format
    thread
    lockfree
    parameter
    bind
    function
    container
    smart_ptr)

set(THIRDPARTY_OPENVR_GIT https://github.com/ValveSoftware/openvr.git)
set(THIRDPARTY_OPENVR_TAG v2.5.1)
set(THIRDPARTY_OpenVR_MIN 2.5)

set(THIRDPARTY_NVTX_GIT https://github.com/NVIDIA/NVTX.git)
set(THIRDPARTY_NVTX_TAG v3.1.1)
set(THIRDPARTY_nvtx3_MIN 3.0)

set(THIRDPARTY_NVAPI_GIT https://github.com/NVIDIA/nvapi.git)
set(THIRDPARTY_NVAPI_TAG main)
