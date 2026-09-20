# cmake -P RelocateConfigLibs.cmake -DPREFIX=... -DCONFIG=RelWithDebInfo
#                                 [-DMANIFEST=<binary-dir>/install_manifest.txt]
#
# Third-party packages install archives to lib/ (CMAKE_INSTALL_LIBDIR=lib).
# At ExternalProject install time $<CONFIG> is known; move those archives into
# lib/<CONFIG>/ and patch exported *Targets*.cmake so IMPORTED_LOCATION matches.
#
# Only files from this package's install manifest are moved. Globbing lib/*.lib
# races with parallel ExternalProject installs (file(RENAME) of a sibling's
# archive, or a missing source after another relocate already claimed it).
#
# Do not put $<CONFIG> or ${CMAKE_INSTALL_CONFIG_NAME} into CMAKE_INSTALL_LIBDIR:
# those strings get baked into export files and break find_package consumers.
if(NOT PREFIX OR NOT CONFIG)
  message(FATAL_ERROR "RelocateConfigLibs: PREFIX and CONFIG are required")
endif()

file(TO_CMAKE_PATH "${PREFIX}" PREFIX)
file(TO_CMAKE_PATH "${PREFIX}/lib" _lib)
set(_dst "${_lib}/${CONFIG}")
file(MAKE_DIRECTORY "${_dst}")

set(_relocated_names "")
set(_files_to_move "")
set(_cmake_files "")

if(MANIFEST AND EXISTS "${MANIFEST}")
  file(STRINGS "${MANIFEST}" _manifest_files)
  foreach(_f IN LISTS _manifest_files)
    file(TO_CMAKE_PATH "${_f}" _f)
    get_filename_component(_dir "${_f}" DIRECTORY)
    get_filename_component(_name "${_f}" NAME)
    file(TO_CMAKE_PATH "${_dir}" _dir)
    if(_dir STREQUAL "${_lib}" AND _name MATCHES "\\.(lib|a|dll|pdb|so|dylib)$")
      list(APPEND _files_to_move "${_f}")
    elseif(_f MATCHES "\\.cmake$")
      list(APPEND _cmake_files "${_f}")
    endif()
  endforeach()
else()
  file(
    GLOB _entries
    LIST_DIRECTORIES false
    "${_lib}/*")
  foreach(_entry IN LISTS _entries)
    get_filename_component(_name "${_entry}" NAME)
    if(_name MATCHES "\\.(lib|a|dll|pdb|so|dylib)$")
      list(APPEND _files_to_move "${_entry}")
    endif()
  endforeach()
  file(
    GLOB_RECURSE _cmake_files
    "${_lib}/cmake/*.cmake")
endif()

foreach(_entry IN LISTS _files_to_move)
  get_filename_component(_name "${_entry}" NAME)
  set(_dest "${_dst}/${_name}")
  if(EXISTS "${_entry}")
    if(EXISTS "${_dest}")
      file(REMOVE "${_dest}")
    endif()
    file(RENAME "${_entry}" "${_dest}")
    list(APPEND _relocated_names "${_name}")
  elseif(EXISTS "${_dest}")
    # Parallel relocate already moved it, or a previous install of this config.
    list(APPEND _relocated_names "${_name}")
  endif()
endforeach()

if(NOT _relocated_names AND NOT _cmake_files)
  return()
endif()

foreach(_cf IN LISTS _cmake_files)
  if(NOT EXISTS "${_cf}")
    continue()
  endif()
  file(READ "${_cf}" _content)
  set(_orig "${_content}")

  string(REPLACE "lib/\${CMAKE_INSTALL_CONFIG_NAME}/" "lib/${CONFIG}/"
                 _content "${_content}")
  string(REPLACE "lib/\${CMAKE_INSTALL_CONFIG_NAME}" "lib/${CONFIG}" _content
                 "${_content}")
  string(REPLACE "lib/$<CONFIG>/" "lib/${CONFIG}/" _content "${_content}")
  string(REPLACE "lib/$<CONFIG>" "lib/${CONFIG}" _content "${_content}")

  foreach(_name IN LISTS _relocated_names)
    string(FIND "${_content}" "/lib/${CONFIG}/${_name}" _already)
    if(_already EQUAL -1)
      string(REPLACE "/lib/${_name}" "/lib/${CONFIG}/${_name}" _content
                     "${_content}")
    endif()
  endforeach()

  if(NOT _content STREQUAL _orig)
    file(WRITE "${_cf}" "${_content}")
  endif()
endforeach()
