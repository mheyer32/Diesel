cmake_minimum_required(VERSION 3.28)

# Independent of the game projects. Consumers find_package() against
# CMAKE_INSTALL_PREFIX (default: <this>/_install/<config>).
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_CURRENT_SOURCE_DIR}/_install"
      CACHE PATH "Shared 3rdParty install prefix" FORCE)
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "")
endif()

project(ThirdParty)

include(ExternalProject)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_SCAN_FOR_MODULES OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/versions.cmake")

file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/versions.cmake"
     THIRDPARTY_MANIFEST_HASH)

if(WIN32)
  option(PREFER_SYSTEM_PACKAGES
         "Use system packages when find_package meets the min version" OFF)
else()
  option(PREFER_SYSTEM_PACKAGES
         "Use system packages when find_package meets the min version" ON)
endif()

# Semicolon-separated ExternalProject / find names that must be fetched even
# when a suitable system package exists (e.g. Boost;OpenVR).
set(THIRDPARTY_FORCE_BUILD
    ""
    CACHE STRING
          "Packages that always fetch (semicolon-separated; name or FIND name)")

# Empty = every package below. Consumers pass a subset via
# thirdparty_ensure(ZLIB PNG ...) / -DTHIRDPARTY_REQUIRED=ZLIB;PNG.
set(THIRDPARTY_REQUIRED
    ""
    CACHE STRING
          "Packages to fetch (semicolon- or |-separated; empty = all)")
# UseThirdParty passes '|' so execute_process does not split on ';'.
if(THIRDPARTY_REQUIRED MATCHES "\\|")
  string(REPLACE "|" ";" THIRDPARTY_REQUIRED "${THIRDPARTY_REQUIRED}")
endif()

option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON)

set(THIRDPARTY_SOURCE_CACHE
    "${CMAKE_CURRENT_SOURCE_DIR}/_src"
    CACHE PATH "Where fetched dependency sources are stored")

set(OVR_SDK_ROOT
    "E:/NewProjects/Libraries/ovr_sdk_win_32.0.0"
    CACHE PATH "Oculus SDK root (contains LibOVR)")

if(MSVC)
  if(USE_MSVC_RUNTIME_LIBRARY_DLL)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

set(THIRDPARTY_COMPONENTS "")
set(THIRDPARTY_FROM_SYSTEM "")

# Common args forwarded into every cmake-based ExternalProject.
# CMAKE_POLICY_VERSION_MINIMUM: CMake 4+ rejects projects whose
# cmake_minimum_required is < 3.5 (Bullet 3.25 and friends).
#
# Windows lib/<CONFIG>/ layout:
#   1. Dependencies install to plain lib/ (normal CMAKE_INSTALL_LIBDIR).
#   2. After each ExternalProject install, RelocateConfigLibs.cmake runs with
#      -DCONFIG=$<CONFIG> (build-time genex on the *superbuild* command line)
#      and moves archives into lib/<CONFIG>/, then patches *Targets*.cmake.
# Putting $<CONFIG> / ${CMAKE_INSTALL_CONFIG_NAME} into CMAKE_INSTALL_LIBDIR
# bakes unresolved strings into export files and breaks find_package.
if(WIN32)
  set(_tp_library_path
      "${CMAKE_INSTALL_PREFIX}/lib/RelWithDebInfo|${CMAKE_INSTALL_PREFIX}/lib/Debug|${CMAKE_INSTALL_PREFIX}/lib/Release|${CMAKE_INSTALL_PREFIX}/lib"
  )
else()
  set(_tp_library_path "${CMAKE_INSTALL_PREFIX}/lib")
endif()

set(_tp_post_install
    ${CMAKE_COMMAND}
    -DPREFIX=${CMAKE_INSTALL_PREFIX}
    -DCONFIG=$<CONFIG>
    -DMANIFEST=<BINARY_DIR>/install_manifest.txt
    -P
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RelocateConfigLibs.cmake)

# Pass the MSVC runtime genex via -C so ExternalProject does not expand
# $<CONFIG> in CMAKE_ARGS (that reconfigures every child per superbuild
# config and breaks packages that cache ASM compilers, e.g. libjpeg-turbo).
set(_tp_msvc_init)
if(MSVC)
  set(_tp_msvc_init "${CMAKE_CURRENT_BINARY_DIR}/tp_msvc_runtime.cmake")
  file(WRITE "${_tp_msvc_init}"
       "set(CMAKE_MSVC_RUNTIME_LIBRARY \"${CMAKE_MSVC_RUNTIME_LIBRARY}\" CACHE STRING \"\" FORCE)\n")
endif()

set(_tp_cmake_base
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_INSTALL_BINDIR=bin
    -DCMAKE_LIBRARY_PATH=${_tp_library_path}
    -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    -DCMAKE_INSTALL_MESSAGE=LAZY
    -DZLIB_USE_STATIC_LIBS=ON)
if(_tp_msvc_init)
  list(APPEND _tp_cmake_base -C ${_tp_msvc_init})
endif()
# VS picks the toolchain; forwarding cl.exe marks the cache UNINITIALIZED and
# can force a compiler re-detect on every child configure.
if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  list(APPEND _tp_cmake_base -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
       -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})
endif()

if(NOT CMAKE_CONFIGURATION_TYPES)
  list(APPEND _tp_cmake_base -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()

# Working copy of THIRDPARTY_REQUIRED with implied deps (PNG -> ZLIB, etc.).
set(_tp_enable ${THIRDPARTY_REQUIRED})
if(_tp_enable)
  set(_tp_extra "")
  foreach(_e IN LISTS _tp_enable)
    string(TOLOWER "${_e}" _el)
    if(_el STREQUAL "png")
      list(APPEND _tp_extra ZLIB)
    elseif(_el STREQUAL "vorbis")
      list(APPEND _tp_extra Ogg)
    endif()
  endforeach()
  list(APPEND _tp_enable ${_tp_extra})
  list(REMOVE_DUPLICATES _tp_enable)
  message(STATUS "ThirdParty: required ${_tp_enable}")
endif()

function(_thirdparty_wanted out)
  if(NOT _tp_enable)
    set(${out} TRUE PARENT_SCOPE)
    return()
  endif()
  set(_ok FALSE)
  foreach(_n IN LISTS ARGN)
    string(TOLOWER "${_n}" _nl)
    foreach(_e IN LISTS _tp_enable)
      string(TOLOWER "${_e}" _el)
      if(_nl STREQUAL _el)
        set(_ok TRUE)
      endif()
    endforeach()
  endforeach()
  set(${out} ${_ok} PARENT_SCOPE)
endfunction()

function(thirdparty_add name)
  set(options PREBUILT FLAT_INCLUDE)
  set(oneValueArgs FIND GIT_REPOSITORY GIT_TAG URL SOURCE_SUBDIR
                   INSTALL_SCRIPT LIST_SEPARATOR MIN_VERSION INCLUDE_SUBDIR)
  set(multiValueArgs CMAKE_ARGS CMAKE_CACHE_ARGS DEPENDS FIND_COMPONENTS
                     CONFIGURE_COMMAND BUILD_COMMAND INSTALL_COMMAND)
  cmake_parse_arguments(TP "${options}" "${oneValueArgs}" "${multiValueArgs}"
                        ${ARGN})

  set(_find_name ${name})
  if(TP_FIND)
    set(_find_name ${TP_FIND})
  endif()

  _thirdparty_wanted(_want ${name} ${_find_name})
  if(NOT _want)
    message(STATUS "ThirdParty: ${_find_name} (not required)")
    return()
  endif()

  # Headers go under include/<lib>/ unless FLAT_INCLUDE (upstream already nests
  # e.g. include/ogg, include/boost, include/nvtx3).
  string(TOLOWER "${name}" _inc_subdir_default)
  if(TP_INCLUDE_SUBDIR)
    set(_inc_subdir "${TP_INCLUDE_SUBDIR}")
  else()
    set(_inc_subdir "${_inc_subdir_default}")
  endif()
  if(TP_FLAT_INCLUDE)
    set(_inc_dir "include")
  else()
    set(_inc_dir "include/${_inc_subdir}")
  endif()

  # Resolve minimum version: explicit arg > THIRDPARTY_<find>_MIN >
  # THIRDPARTY_<name>_MIN.
  set(_min "")
  if(TP_MIN_VERSION)
    set(_min "${TP_MIN_VERSION}")
  elseif(DEFINED THIRDPARTY_${_find_name}_MIN)
    set(_min "${THIRDPARTY_${_find_name}_MIN}")
  elseif(DEFINED THIRDPARTY_${name}_MIN)
    set(_min "${THIRDPARTY_${name}_MIN}")
  endif()

  set(_force FALSE)
  if(name IN_LIST THIRDPARTY_FORCE_BUILD OR _find_name IN_LIST
                                            THIRDPARTY_FORCE_BUILD)
    set(_force TRUE)
  endif()

  if(PREFER_SYSTEM_PACKAGES AND NOT _force)
    if(_min)
      if(TP_FIND_COMPONENTS)
        find_package(${_find_name} ${_min} QUIET COMPONENTS
                     ${TP_FIND_COMPONENTS})
      else()
        find_package(${_find_name} ${_min} QUIET)
      endif()
    else()
      if(TP_FIND_COMPONENTS)
        find_package(${_find_name} QUIET COMPONENTS ${TP_FIND_COMPONENTS})
      else()
        find_package(${_find_name} QUIET)
      endif()
    endif()
    if(${_find_name}_FOUND)
      if(_min)
        message(STATUS "ThirdParty: ${_find_name} (system >= ${_min})")
      else()
        message(STATUS "ThirdParty: ${_find_name} (system)")
      endif()
      set(THIRDPARTY_FROM_SYSTEM
          ${THIRDPARTY_FROM_SYSTEM} ${_find_name}
          PARENT_SCOPE)
      set(THIRDPARTY_COMPONENTS
          ${THIRDPARTY_COMPONENTS} ${_find_name}
          PARENT_SCOPE)
      return()
    elseif(_min)
      message(
        STATUS
          "ThirdParty: ${_find_name} system package missing or < ${_min}; will fetch"
      )
    endif()
  elseif(_force)
    message(STATUS "ThirdParty: ${_find_name} (force-build)")
  endif()

  if(_min)
    message(
      STATUS
        "ThirdParty: ${_find_name} (build -> ${_inc_dir}, pin meets >= ${_min})")
  else()
    message(STATUS "ThirdParty: ${_find_name} (build -> ${_inc_dir})")
  endif()
  set(THIRDPARTY_COMPONENTS
      ${THIRDPARTY_COMPONENTS} ${_find_name}
      PARENT_SCOPE)

  set(_src "${THIRDPARTY_SOURCE_CACHE}/${name}")
  set(_ep_args
      UPDATE_DISCONNECTED
      TRUE
      SOURCE_DIR
      "${_src}")

  if(TP_LIST_SEPARATOR)
    list(APPEND _ep_args LIST_SEPARATOR "${TP_LIST_SEPARATOR}")
  elseif(WIN32)
    # Default so CMAKE_LIBRARY_PATH's '|' separators survive ExternalProject.
    list(APPEND _ep_args LIST_SEPARATOR "|")
  endif()

  if(TP_GIT_REPOSITORY)
    list(APPEND _ep_args GIT_REPOSITORY "${TP_GIT_REPOSITORY}" GIT_SHALLOW ON)
    if(TP_GIT_TAG)
      list(APPEND _ep_args GIT_TAG "${TP_GIT_TAG}")
    endif()
  elseif(TP_URL)
    list(APPEND _ep_args URL "${TP_URL}")
  endif()

  if(TP_PREBUILT)
    if(NOT TP_INSTALL_SCRIPT)
      message(FATAL_ERROR "${name}: PREBUILT requires INSTALL_SCRIPT")
    endif()
    # Empty CONFIGURE/BUILD_COMMAND strings are dropped by list(APPEND) and
    # ExternalProject then falls back to running cmake on SOURCE_DIR.
    list(
      APPEND
      _ep_args
      CONFIGURE_COMMAND
      ${CMAKE_COMMAND}
      -E
      true
      BUILD_COMMAND
      ${CMAKE_COMMAND}
      -E
      true
      INSTALL_COMMAND
      ${CMAKE_COMMAND}
      -DSRC=<SOURCE_DIR>
      -DDST=${CMAKE_INSTALL_PREFIX}
      -DINCLUDE_SUBDIR=${_inc_subdir}
      -DCONFIG=$<CONFIG>
      -P
      "${TP_INSTALL_SCRIPT}")
  else()
    if(TP_SOURCE_SUBDIR)
      list(APPEND _ep_args SOURCE_SUBDIR "${TP_SOURCE_SUBDIR}")
    endif()
    if(TP_CONFIGURE_COMMAND)
      list(APPEND _ep_args CONFIGURE_COMMAND ${TP_CONFIGURE_COMMAND})
    endif()
    if(TP_BUILD_COMMAND)
      list(APPEND _ep_args BUILD_COMMAND ${TP_BUILD_COMMAND})
    endif()
    if(TP_INSTALL_COMMAND)
      if(WIN32)
        list(APPEND _ep_args INSTALL_COMMAND ${TP_INSTALL_COMMAND} COMMAND
                             ${_tp_post_install})
      else()
        list(APPEND _ep_args INSTALL_COMMAND ${TP_INSTALL_COMMAND})
      endif()
    else()
      if(WIN32)
        list(
          APPEND
          _ep_args
          INSTALL_COMMAND
          ${CMAKE_COMMAND}
          --install
          <BINARY_DIR>
          --config
          $<CONFIG>
          COMMAND
          ${_tp_post_install})
      endif()
    endif()
    if(NOT TP_CONFIGURE_COMMAND)
      list(APPEND _ep_args CMAKE_ARGS ${_tp_cmake_base}
                           -DCMAKE_INSTALL_INCLUDEDIR=${_inc_dir}
                           ${TP_CMAKE_ARGS})
      if(TP_CMAKE_CACHE_ARGS)
        list(APPEND _ep_args CMAKE_CACHE_ARGS ${TP_CMAKE_CACHE_ARGS})
      endif()
    endif()
  endif()

  if(TP_DEPENDS)
    list(APPEND _ep_args DEPENDS ${TP_DEPENDS})
  endif()

  ExternalProject_Add(${name} ${_ep_args})
endfunction()

# ##############################################################################
# Packages
# ##############################################################################

thirdparty_add(
  ZLIB
  GIT_REPOSITORY ${THIRDPARTY_ZLIB_GIT}
  GIT_TAG ${THIRDPARTY_ZLIB_TAG}
  CMAKE_ARGS -DZLIB_BUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=OFF
  INSTALL_COMMAND
    ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
    COMMAND
    ${CMAKE_COMMAND}
    -DSRC=<SOURCE_DIR>
    -DDST=${CMAKE_INSTALL_PREFIX}
    -DCONFIGFILE=${CMAKE_CURRENT_BINARY_DIR}/ZLIBConfig.cmake
    -P
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallZlib.cmake)

set(_png_depends)
if(TARGET ZLIB)
  list(APPEND _png_depends ZLIB)
endif()

thirdparty_add(
  PNG
  GIT_REPOSITORY ${THIRDPARTY_PNG_GIT}
  GIT_TAG ${THIRDPARTY_PNG_TAG}
  DEPENDS ${_png_depends}
  CMAKE_ARGS -DPNG_SHARED=OFF
             -DPNG_STATIC=ON
             -DPNG_TESTS=OFF
             -DPNG_TOOLS=OFF
             -DPNG_BUILD_ZLIB=OFF
             -DSKIP_INSTALL_ALL=OFF)

# WITH_SIMD=OFF: libjpeg-turbo enable_language(ASM_NASM) FATAL_ERRORs when a
# cached NASM path goes stale. REQUIRE_SIMD=OFF does not prevent that.
set(_jpeg_crt)
if(WIN32)
  list(APPEND _jpeg_crt -DWITH_CRT_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL})
endif()
thirdparty_add(
  JPEG
  FIND JPEG
  GIT_REPOSITORY ${THIRDPARTY_JPEG_GIT}
  GIT_TAG ${THIRDPARTY_JPEG_TAG}
  CMAKE_ARGS -DENABLE_SHARED=OFF
             -DENABLE_STATIC=ON
             -DWITH_TURBOJPEG=OFF
             -DWITH_SIMD=OFF
             -DREQUIRE_SIMD=OFF
             ${_jpeg_crt})

thirdparty_add(
  Ogg
  FIND Ogg
  FLAT_INCLUDE
  GIT_REPOSITORY ${THIRDPARTY_OGG_GIT}
  GIT_TAG ${THIRDPARTY_OGG_TAG}
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF
             -DINSTALL_DOCS=OFF)

set(_vorbis_depends)
if(TARGET Ogg)
  list(APPEND _vorbis_depends Ogg)
endif()

thirdparty_add(
  Vorbis
  FIND Vorbis
  FLAT_INCLUDE
  GIT_REPOSITORY ${THIRDPARTY_VORBIS_GIT}
  GIT_TAG ${THIRDPARTY_VORBIS_TAG}
  DEPENDS ${_vorbis_depends}
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF)

thirdparty_add(
  OpenAL
  FIND OpenAL
  FLAT_INCLUDE
  GIT_REPOSITORY ${THIRDPARTY_OPENAL_GIT}
  GIT_TAG ${THIRDPARTY_OPENAL_TAG}
  CMAKE_ARGS -DLIBTYPE=STATIC
             -DALSOFT_UTILS=OFF
             -DALSOFT_EXAMPLES=OFF
             -DALSOFT_TESTS=OFF
             -DALSOFT_INSTALL_EXAMPLES=OFF
             -DALSOFT_INSTALL_UTILS=OFF
             -DALSOFT_NO_CONFIG_UTIL=ON
             -DALSOFT_ENABLE_MODULES=OFF)

thirdparty_add(
  Bullet
  FIND Bullet
  FLAT_INCLUDE
  GIT_REPOSITORY ${THIRDPARTY_BULLET_GIT}
  GIT_TAG ${THIRDPARTY_BULLET_TAG}
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF
             -DBUILD_BULLET3=OFF
             -DBUILD_EXTRAS=OFF
             -DBUILD_UNIT_TESTS=OFF
             -DBUILD_BULLET2_DEMOS=OFF
             -DBUILD_CPU_DEMOS=OFF
             -DBUILD_OPENGL3_DEMOS=OFF
             -DBUILD_PYBULLET=OFF
             -DBUILD_CLSOCKET=OFF
             -DBUILD_ENET=OFF
             -DUSE_GLUT=OFF
             -DUSE_GRAPHICAL_BENCHMARK=OFF
             -DINSTALL_LIBS=ON
             -DUSE_MSVC_RUNTIME_LIBRARY_DLL=${USE_MSVC_RUNTIME_LIBRARY_DLL})

# ExternalProject treats ';' as list separators; use '|' inside BOOST_INCLUDE_LIBRARIES.
string(REPLACE ";" "|" _boost_libs_sep "${THIRDPARTY_BOOST_LIBS}")
thirdparty_add(
  Boost
  FIND Boost
  FLAT_INCLUDE
  FIND_COMPONENTS regex thread
  URL ${THIRDPARTY_BOOST_URL}
  LIST_SEPARATOR "|"
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DBOOST_SKIP_INSTALL_RULES=OFF
             -DBOOST_INCLUDE_LIBRARIES=${_boost_libs_sep})

thirdparty_add(
  nvtx3
  FIND nvtx3
  PREBUILT
  FLAT_INCLUDE
  GIT_REPOSITORY ${THIRDPARTY_NVTX_GIT}
  GIT_TAG ${THIRDPARTY_NVTX_TAG}
  INSTALL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallNvtx.cmake")

# OpenVR's own CMake has no install rules; wrap it.
thirdparty_add(
  OpenVR
  FIND OpenVR
  INCLUDE_SUBDIR openvr
  GIT_REPOSITORY ${THIRDPARTY_OPENVR_GIT}
  GIT_TAG ${THIRDPARTY_OPENVR_TAG}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND}
    -S
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/openvr_wrap
    -B
    <BINARY_DIR>
    -DOPENVR_SOURCE_DIR=<SOURCE_DIR>
    -DCMAKE_INSTALL_INCLUDEDIR=include/openvr
    ${_tp_cmake_base}
  BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
                --parallel
  INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>)

if(WIN32)
  thirdparty_add(
    nvapi
    FIND nvapi
    PREBUILT
    INCLUDE_SUBDIR nvapi
    GIT_REPOSITORY ${THIRDPARTY_NVAPI_GIT}
    GIT_TAG ${THIRDPARTY_NVAPI_TAG}
    INSTALL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallNvapi.cmake")

  _thirdparty_wanted(_want_ovr LibOVR)
  if(_want_ovr)
    if(EXISTS "${OVR_SDK_ROOT}/LibOVR/CMakeLists.txt")
      message(STATUS "ThirdParty: LibOVR (SDK at ${OVR_SDK_ROOT})")
      list(APPEND THIRDPARTY_COMPONENTS LibOVR)
      ExternalProject_Add(
        LibOVR
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libovr"
        UPDATE_DISCONNECTED TRUE
        LIST_SEPARATOR "|"
        CMAKE_ARGS ${_tp_cmake_base} -DCMAKE_INSTALL_INCLUDEDIR=include/libovr
                   -DOVR_SDK_LIBOVR=${OVR_SDK_ROOT}/LibOVR
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config
                        $<CONFIG> COMMAND ${_tp_post_install})
    else()
      message(
        WARNING
          "ThirdParty: LibOVR skipped (set OVR_SDK_ROOT to the Oculus SDK). Looked in ${OVR_SDK_ROOT}"
      )
    endif()
  else()
    message(STATUS "ThirdParty: LibOVR (not required)")
  endif()
endif()

# ##############################################################################
# Package config installed into the prefix
# ##############################################################################

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ThirdPartyConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ThirdPartyConfig.cmake"
  INSTALL_DESTINATION lib/cmake/ThirdParty)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ThirdPartyConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ThirdPartyConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/ThirdPartyConfigVersion.cmake"
        DESTINATION lib/cmake/ThirdParty)

if(TARGET ZLIB)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ZLIBConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ZLIBConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ZLIB)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ZLIBConfig.cmake"
          DESTINATION lib/cmake/ZLIB)
endif()

if(TARGET OpenVR)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OpenVRConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OpenVRConfig.cmake"
    INSTALL_DESTINATION lib/cmake/OpenVR)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OpenVRConfig.cmake"
          DESTINATION lib/cmake/OpenVR)
endif()

if(TARGET nvtx3)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nvtx3Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nvtx3Config.cmake"
    INSTALL_DESTINATION lib/cmake/nvtx3)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nvtx3Config.cmake"
          DESTINATION lib/cmake/nvtx3)
endif()

if(WIN32)
  if(TARGET nvapi)
    configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nvapiConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/nvapiConfig.cmake"
      INSTALL_DESTINATION lib/cmake/nvapi)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nvapiConfig.cmake"
            DESTINATION lib/cmake/nvapi)
  endif()

  if(TARGET LibOVR)
    configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LibOVRConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/LibOVRConfig.cmake"
      INSTALL_DESTINATION lib/cmake/LibOVR)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LibOVRConfig.cmake"
            DESTINATION lib/cmake/LibOVR)
  endif()
endif()
