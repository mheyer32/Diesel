# cmake -P InstallNvapi.cmake -DSRC=... -DDST=... [-DINCLUDE_SUBDIR=nvapi] [-DCONFIG=Debug]
if(NOT SRC OR NOT DST)
  message(FATAL_ERROR "InstallNvapi: SRC and DST are required")
endif()

if(NOT INCLUDE_SUBDIR)
  set(INCLUDE_SUBDIR nvapi)
endif()

file(GLOB _headers "${SRC}/*.h")
set(_inc "${DST}/include/${INCLUDE_SUBDIR}")
file(MAKE_DIRECTORY "${_inc}")
file(COPY ${_headers} DESTINATION "${_inc}")

if(NOT EXISTS "${SRC}/amd64/nvapi64.lib")
  message(FATAL_ERROR "InstallNvapi: ${SRC}/amd64/nvapi64.lib not found")
endif()

# Prebuilt vendor lib: install into every requested config dir (and plain lib/).
set(_configs ${CONFIG})
if(NOT _configs)
  set(_configs Debug RelWithDebInfo Release)
endif()
foreach(_c IN LISTS _configs)
  set(_libdir "${DST}/lib/${_c}")
  file(MAKE_DIRECTORY "${_libdir}")
  file(COPY "${SRC}/amd64/nvapi64.lib" DESTINATION "${_libdir}")
endforeach()
