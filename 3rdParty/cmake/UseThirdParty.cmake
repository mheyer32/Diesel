# Include from any project that should consume the shared 3rdParty install
# prefix. If the prefix is missing (or stale vs versions.cmake), it is
# configured/built/installed automatically before find_package() runs.
#
#   list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../3rdParty/cmake")
#   include(UseThirdParty)
#   thirdparty_ensure(ZLIB PNG Boost)   # empty = every 3rdParty package
#   find_package(ZLIB REQUIRED)
#   ...
#
# Install/build dirs are per-architecture (x86 vs x64) so a 32-bit kit does
# not pick up a 64-bit package config (CMake rejects that as incompatible).

set(THIRDPARTY_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/.."
    CACHE PATH "Path to the 3rdParty project")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_tp_arch x64)
else()
  set(_tp_arch x86)
endif()

# Per-architecture trees so a 32-bit kit does not load a 64-bit
# ThirdPartyConfig (CMake reports that as an incompatible version).
set(_tp_default_install "${THIRDPARTY_ROOT}/_install-${_tp_arch}")
set(_tp_default_build "${THIRDPARTY_ROOT}/_build-${_tp_arch}")

if(DEFINED CACHE{THIRDPARTY_INSTALL_PREFIX})
  get_filename_component(_tp_pref_name "${THIRDPARTY_INSTALL_PREFIX}" NAME)
  if(_tp_pref_name STREQUAL "_install" OR _tp_pref_name MATCHES "^_install-")
    if(NOT _tp_pref_name STREQUAL "_install-${_tp_arch}")
      set(THIRDPARTY_INSTALL_PREFIX
          "${_tp_default_install}"
          CACHE PATH "Shared 3rdParty install prefix used by all consumers"
                FORCE)
    endif()
  endif()
else()
  set(THIRDPARTY_INSTALL_PREFIX
      "${_tp_default_install}"
      CACHE PATH "Shared 3rdParty install prefix used by all consumers")
endif()

if(DEFINED CACHE{THIRDPARTY_BUILD_DIR})
  get_filename_component(_tp_build_name "${THIRDPARTY_BUILD_DIR}" NAME)
  if(_tp_build_name STREQUAL "_build" OR _tp_build_name MATCHES "^_build-")
    if(NOT _tp_build_name STREQUAL "_build-${_tp_arch}")
      set(THIRDPARTY_BUILD_DIR
          "${_tp_default_build}"
          CACHE PATH "Build directory for the 3rdParty ExternalProject tree"
                FORCE)
    endif()
  endif()
else()
  set(THIRDPARTY_BUILD_DIR
      "${_tp_default_build}"
      CACHE PATH "Build directory for the 3rdParty ExternalProject tree")
endif()

option(THIRDPARTY_AUTO_BUILD
       "Configure/build/install 3rdParty when the prefix is missing or stale"
       ON)

function(_thirdparty_build_one_config cfg)
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build "${THIRDPARTY_BUILD_DIR}" --config ${cfg}
            --parallel COMMAND_ECHO STDOUT RESULT_VARIABLE _rc)
  if(NOT _rc EQUAL 0)
    message(FATAL_ERROR "ThirdParty: build (${cfg}) failed (${_rc})")
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} --install "${THIRDPARTY_BUILD_DIR}" --config
            ${cfg} RESULT_VARIABLE _rc)
  if(NOT _rc EQUAL 0)
    message(FATAL_ERROR "ThirdParty: install (${cfg}) failed (${_rc})")
  endif()
endfunction()

function(_thirdparty_in_list needle haystack out)
  string(TOLOWER "${needle}" _n)
  set(_found FALSE)
  foreach(_x IN LISTS haystack)
    string(TOLOWER "${_x}" _xl)
    if(_n STREQUAL _xl)
      set(_found TRUE)
    endif()
  endforeach()
  set(${out} ${_found} PARENT_SCOPE)
endfunction()

function(thirdparty_ensure)
  cmake_parse_arguments(_te "" "" "COMPONENTS" ${ARGN})
  set(_requested ${_te_COMPONENTS} ${_te_UNPARSED_ARGUMENTS})

  set(_versions "${THIRDPARTY_ROOT}/versions.cmake")
  if(NOT EXISTS "${_versions}")
    message(FATAL_ERROR "UseThirdParty: ${_versions} not found")
  endif()
  file(SHA256 "${_versions}" _wanted_hash)

  set(_stamp
      "${THIRDPARTY_INSTALL_PREFIX}/lib/cmake/ThirdParty/ThirdPartyConfig.cmake")
  set(_need_build FALSE)
  set(_installed)

  if(NOT EXISTS "${_stamp}")
    set(_need_build TRUE)
    message(
      STATUS
        "ThirdParty: install prefix empty (${THIRDPARTY_INSTALL_PREFIX})")
  else()
    include("${_stamp}" OPTIONAL RESULT_VARIABLE _tp_cfg)
    if(NOT _tp_cfg)
      set(_need_build TRUE)
    elseif(NOT DEFINED ThirdParty_MANIFEST_HASH)
      set(_need_build TRUE)
    elseif(NOT ThirdParty_MANIFEST_HASH STREQUAL _wanted_hash)
      message(
        STATUS
          "ThirdParty: versions.cmake changed (prefix hash ${ThirdParty_MANIFEST_HASH}, wanted ${_wanted_hash})"
      )
      set(_need_build TRUE)
    else()
      set(_installed ${ThirdParty_COMPONENTS})
    endif()
  endif()

  # Prefix is shared: fetch the union of this consumer's list and whatever
  # is already installed so another project is not stripped.
  set(_enable ${_requested} ${_installed})
  list(REMOVE_DUPLICATES _enable)

  if(_requested AND NOT _need_build)
    foreach(_c IN LISTS _requested)
      _thirdparty_in_list(${_c} "${_installed}" _have)
      if(NOT _have)
        message(STATUS "ThirdParty: ${_c} not in prefix; building")
        set(_need_build TRUE)
      endif()
    endforeach()
  endif()

  # Windows layout expects both Debug and RelWithDebInfo archives.
  if(WIN32 AND NOT _need_build)
    if(NOT EXISTS "${THIRDPARTY_INSTALL_PREFIX}/lib/Debug"
       OR NOT EXISTS "${THIRDPARTY_INSTALL_PREFIX}/lib/RelWithDebInfo")
      message(
        STATUS
          "ThirdParty: missing lib/Debug or lib/RelWithDebInfo; rebuilding")
      set(_need_build TRUE)
    endif()
  endif()

  if(_need_build)
    if(NOT THIRDPARTY_AUTO_BUILD)
      message(
        FATAL_ERROR
          "ThirdParty prefix at ${THIRDPARTY_INSTALL_PREFIX} is missing or stale.\n"
          "Build it once (Windows installs Debug + RelWithDebInfo):\n"
          "  cmake -S \"${THIRDPARTY_ROOT}\" -B \"${THIRDPARTY_BUILD_DIR}\" "
          "-DCMAKE_INSTALL_PREFIX=\"${THIRDPARTY_INSTALL_PREFIX}\"\n"
          "  cmake --build \"${THIRDPARTY_BUILD_DIR}\" --config RelWithDebInfo --parallel\n"
          "  cmake --install \"${THIRDPARTY_BUILD_DIR}\" --config RelWithDebInfo\n"
          "  cmake --build \"${THIRDPARTY_BUILD_DIR}\" --config Debug --parallel\n"
          "  cmake --install \"${THIRDPARTY_BUILD_DIR}\" --config Debug")
    endif()

    message(STATUS "ThirdParty: building into ${THIRDPARTY_INSTALL_PREFIX}")

    set(_prefer OFF)
    if(UNIX AND NOT WIN32)
      set(_prefer ON)
    endif()
    if(DEFINED CACHE{PREFER_SYSTEM_PACKAGES})
      set(_prefer ${PREFER_SYSTEM_PACKAGES})
    endif()

    set(_force_arg)
    if(DEFINED CACHE{THIRDPARTY_FORCE_BUILD} AND THIRDPARTY_FORCE_BUILD)
      set(_force_arg "-DTHIRDPARTY_FORCE_BUILD=${THIRDPARTY_FORCE_BUILD}")
    endif()

    set(_req_arg)
    if(_enable)
      list(JOIN _enable "|" _req_joined)
      set(_req_arg "-DTHIRDPARTY_REQUIRED=${_req_joined}")
    endif()

    set(_gen_args)
    if(CMAKE_GENERATOR)
      list(APPEND _gen_args -G "${CMAKE_GENERATOR}")
    endif()
    if(CMAKE_GENERATOR_PLATFORM)
      list(APPEND _gen_args -A "${CMAKE_GENERATOR_PLATFORM}")
    elseif(WIN32 AND CMAKE_GENERATOR MATCHES "Visual Studio")
      if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND _gen_args -A x64)
      else()
        list(APPEND _gen_args -A Win32)
      endif()
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
      list(APPEND _gen_args -T "${CMAKE_GENERATOR_TOOLSET}")
    endif()
    # NMake/Ninja/etc.: architecture comes from the compiler host/target dir.
    if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
      if(CMAKE_C_COMPILER)
        list(APPEND _gen_args "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
      endif()
      if(CMAKE_CXX_COMPILER)
        list(APPEND _gen_args "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
      endif()
    endif()

    set(_multi_config FALSE)
    if(CMAKE_GENERATOR MATCHES "Visual Studio|Xcode|Multi-Config")
      set(_multi_config TRUE)
    endif()

    if(WIN32 AND NOT _multi_config)
      # Single-config generators need a separate configure per build type so
      # both Debug and RelWithDebInfo land under lib/<CONFIG>/.
      foreach(_cfg IN ITEMS RelWithDebInfo Debug)
        message(STATUS "ThirdParty: configure ${_cfg}")
        execute_process(
          COMMAND
            ${CMAKE_COMMAND} -S "${THIRDPARTY_ROOT}" -B
            "${THIRDPARTY_BUILD_DIR}" ${_gen_args}
            -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_PREFIX}
            -DPREFER_SYSTEM_PACKAGES=${_prefer}
            -DCMAKE_BUILD_TYPE=${_cfg} ${_force_arg} ${_req_arg}
          RESULT_VARIABLE _rc)
        if(NOT _rc EQUAL 0)
          message(FATAL_ERROR "ThirdParty: configure (${_cfg}) failed (${_rc})")
        endif()
        message(STATUS "ThirdParty: ${_cfg}")
        _thirdparty_build_one_config(${_cfg})
      endforeach()
    else()
      set(_configure_type RelWithDebInfo)
      execute_process(
        COMMAND
          ${CMAKE_COMMAND} -S "${THIRDPARTY_ROOT}" -B
          "${THIRDPARTY_BUILD_DIR}" ${_gen_args}
          -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_PREFIX}
          -DPREFER_SYSTEM_PACKAGES=${_prefer}
          -DCMAKE_BUILD_TYPE=${_configure_type} ${_force_arg} ${_req_arg}
        RESULT_VARIABLE _rc)
      if(NOT _rc EQUAL 0)
        message(FATAL_ERROR "ThirdParty: configure failed (${_rc})")
      endif()

      if(WIN32)
        foreach(_cfg IN ITEMS RelWithDebInfo Debug)
          message(STATUS "ThirdParty: ${_cfg}")
          _thirdparty_build_one_config(${_cfg})
        endforeach()
      else()
        _thirdparty_build_one_config(RelWithDebInfo)
      endif()
    endif()
  else()
    message(STATUS "ThirdParty: using ${THIRDPARTY_INSTALL_PREFIX}")
  endif()

  list(PREPEND CMAKE_PREFIX_PATH "${THIRDPARTY_INSTALL_PREFIX}")
  set(CMAKE_PREFIX_PATH
      "${CMAKE_PREFIX_PATH}"
      PARENT_SCOPE)
  set(CMAKE_PREFIX_PATH
      "${CMAKE_PREFIX_PATH}"
      CACHE STRING "" FORCE)

  set(CMAKE_FIND_PACKAGE_PREFER_CONFIG
      ON
      PARENT_SCOPE)
  set(CMAKE_FIND_PACKAGE_PREFER_CONFIG
      ON
      CACHE BOOL "" FORCE)

  file(
    GLOB _tp_inc_children
    LIST_DIRECTORIES true
    "${THIRDPARTY_INSTALL_PREFIX}/include/*")
  set(_tp_inc_paths "${THIRDPARTY_INSTALL_PREFIX}/include")
  foreach(_d IN LISTS _tp_inc_children)
    if(IS_DIRECTORY "${_d}")
      list(APPEND _tp_inc_paths "${_d}")
    endif()
  endforeach()
  list(PREPEND CMAKE_INCLUDE_PATH ${_tp_inc_paths})
  set(CMAKE_INCLUDE_PATH
      "${CMAKE_INCLUDE_PATH}"
      PARENT_SCOPE)
  set(CMAKE_INCLUDE_PATH
      "${CMAKE_INCLUDE_PATH}"
      CACHE STRING "" FORCE)

  # Module-mode find_library (FindZLIB etc.) searches these.
  set(_tp_lib_paths "${THIRDPARTY_INSTALL_PREFIX}/lib")
  foreach(_cfg IN ITEMS RelWithDebInfo Release Debug MinSizeRel)
    if(EXISTS "${THIRDPARTY_INSTALL_PREFIX}/lib/${_cfg}")
      list(APPEND _tp_lib_paths "${THIRDPARTY_INSTALL_PREFIX}/lib/${_cfg}")
    endif()
  endforeach()
  list(PREPEND CMAKE_LIBRARY_PATH ${_tp_lib_paths})
  set(CMAKE_LIBRARY_PATH
      "${CMAKE_LIBRARY_PATH}"
      PARENT_SCOPE)
  set(CMAKE_LIBRARY_PATH
      "${CMAKE_LIBRARY_PATH}"
      CACHE STRING "" FORCE)

  find_package(ThirdParty CONFIG REQUIRED)
endfunction()

# Map configs that we do not ship onto the closest installed variant.
macro(thirdparty_map_debug_to_release)
  set(CMAKE_MAP_IMPORTED_CONFIG_RELEASE "RelWithDebInfo;Release;Debug;")
  set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO
      "RelWithDebInfo;Release;Debug;")
  set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "RelWithDebInfo;Release;Debug;")
  set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Debug;RelWithDebInfo;Release;")
endmacro()
