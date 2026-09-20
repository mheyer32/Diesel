# cmake -P InstallZlib.cmake -DSRC=<zlib source> -DDST=<prefix>
#                            [-DCONFIGFILE=<ZLIBConfig.cmake>]
#
# zlib 1.3.1 always installs a DLL (zlib.lib import + zlib.dll) in addition to
# zlibstatic. Strip the shared artifacts so consumers link statically.
# Also copies contrib/minizip (not part of zlib's install).
if(NOT SRC OR NOT DST)
  message(FATAL_ERROR "InstallZlib: SRC and DST are required")
endif()

set(_src "${SRC}/contrib/minizip")
if(NOT EXISTS "${_src}/unzip.h")
  message(FATAL_ERROR "InstallZlib: ${_src}/unzip.h not found")
endif()

set(_mz "${DST}/include/contrib/minizip")
file(MAKE_DIRECTORY "${_mz}")
foreach(_f unzip.h unzip.c ioapi.h ioapi.c crypt.h)
  file(COPY "${_src}/${_f}" DESTINATION "${_mz}")
endforeach()

# Shared names only — do not match zlibstatic / zlibstaticd / libz.a.
foreach(_dir "${DST}/lib" "${DST}/lib/Debug" "${DST}/lib/RelWithDebInfo"
             "${DST}/lib/Release" "${DST}/lib/MinSizeRel" "${DST}/bin")
  foreach(_n zlib zlibd zlib1)
    foreach(_ext lib dll pdb exp so dylib)
      set(_p "${_dir}/${_n}.${_ext}")
      if(EXISTS "${_p}")
        file(REMOVE "${_p}")
      endif()
    endforeach()
  endforeach()
endforeach()

if(CONFIGFILE AND EXISTS "${CONFIGFILE}")
  set(_cfgdir "${DST}/lib/cmake/ZLIB")
  file(MAKE_DIRECTORY "${_cfgdir}")
  file(COPY "${CONFIGFILE}" DESTINATION "${_cfgdir}")
endif()
