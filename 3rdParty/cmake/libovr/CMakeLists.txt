cmake_minimum_required(VERSION 3.28)
project(LibOVRInstall)

if(NOT OVR_SDK_LIBOVR)
  message(FATAL_ERROR "OVR_SDK_LIBOVR is required")
endif()

if(NOT CMAKE_INSTALL_INCLUDEDIR)
  set(CMAKE_INSTALL_INCLUDEDIR include/libovr)
endif()
if(NOT CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif()

add_subdirectory("${OVR_SDK_LIBOVR}" libovr_build)

install(TARGETS LibOVR ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} LIBRARY
                                                          DESTINATION
                                                          ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "${OVR_SDK_LIBOVR}/Include/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
