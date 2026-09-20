# cmake -P InstallNvtx.cmake -DSRC=<NVTX repo root> -DDST=<prefix>
# Header-only: copy c/include/nvtx3 into the prefix (no CMake install rules upstream).
if(NOT SRC OR NOT DST)
  message(FATAL_ERROR "InstallNvtx: SRC and DST are required")
endif()

set(_hdr "${SRC}/c/include")
if(NOT EXISTS "${_hdr}/nvtx3")
  message(FATAL_ERROR "InstallNvtx: ${_hdr}/nvtx3 not found")
endif()

file(MAKE_DIRECTORY "${DST}/include")
file(COPY "${_hdr}/nvtx3" DESTINATION "${DST}/include")
