cmake_minimum_required(VERSION 3.28)
project(openvr_wrap)

if(NOT OPENVR_SOURCE_DIR)
  message(FATAL_ERROR "OPENVR_SOURCE_DIR is required")
endif()

if(NOT CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR include/openvr)
endif()
if(NOT CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif()

set(BUILD_SHARED
    OFF
    CACHE BOOL "" FORCE)

add_subdirectory("${OPENVR_SOURCE_DIR}" openvr_build)

if(TARGET openvr_api64)
  set(_openvr_target openvr_api64)
elseif(TARGET openvr_api)
  set(_openvr_target openvr_api)
else()
  message(FATAL_ERROR "OpenVR did not define openvr_api / openvr_api64")
endif()

install(TARGETS ${_openvr_target} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                                  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "${OPENVR_SOURCE_DIR}/headers/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
