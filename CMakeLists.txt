cmake_minimum_required(VERSION 3.28)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cmake")

set(CMAKE_INSTALL_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/_bin"
    CACHE PATH "Where to place the executable")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# needs to be placed _before_ checking either CMAKE_CXX_COMPILER_ID or MSVC
project(Diesel)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message(STATUS "CLANG Compiler")
  add_compile_options(-Wno-deprecated-declarations -Wno-unused-variable)
elseif(MSVC)
  message(STATUS "Microsoft Compiler")
  add_compile_options(
    /wd4161 # pragma push/pop imbalance in some third party code
    /permissive
  )
else()
  message(STATUS "Unknown Compiler")
endif()

if(MSVC)
  add_compile_options(/MP /Oy- /Oi /fp:fast)
endif()

include(UseThirdParty)
thirdparty_map_debug_to_release()
thirdparty_ensure(ZLIB JPEG)

# ##################################### Dependencies

set(ZLIB_USE_STATIC_LIBS ON)
find_package(ZLIB REQUIRED)
find_package(JPEG QUIET)
if(NOT JPEG_FOUND AND NOT TARGET JPEG::JPEG AND NOT TARGET
                                                 libjpeg-turbo::jpeg-static)
  find_package(libjpeg-turbo CONFIG REQUIRED)
endif()
if(NOT JPEG_FOUND
   AND NOT TARGET JPEG::JPEG
   AND NOT TARGET libjpeg-turbo::jpeg-static
   AND NOT TARGET libjpeg-turbo::jpeg)
  find_package(JPEG REQUIRED)
endif()
find_package(OpenGL REQUIRED)

if(TARGET ZLIB::ZLIB)
  set(_zlib ZLIB::ZLIB)
else()
  set(_zlib ZLIB::zlib)
endif()

if(TARGET JPEG::JPEG)
  set(_jpeg JPEG::JPEG)
elseif(TARGET libjpeg-turbo::jpeg-static)
  set(_jpeg libjpeg-turbo::jpeg-static)
elseif(TARGET jpeg-static)
  set(_jpeg jpeg-static)
else()
  message(FATAL_ERROR "No JPEG imported target found")
endif()

set(FMOD_ROOT
    "E:/NewProjects/Libraries/fmodapi375win/api"
    CACHE PATH "FMOD 3.75 SDK root (contains inc/ and lib/)")
if(NOT EXISTS "${FMOD_ROOT}/inc/fmoddyn.h")
  message(
    FATAL_ERROR
      "FMOD headers not found (looked for ${FMOD_ROOT}/inc/fmoddyn.h). Set FMOD_ROOT."
  )
endif()

function(FilesToFolders files)
  foreach(SOURCE_FILE IN LISTS ${files})
    get_filename_component(SOURCE_PATH "${SOURCE_FILE}" PATH)
    file(RELATIVE_PATH RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
         "${SOURCE_PATH}")
    string(REGEX REPLACE "^[^/]+/[^/]+/(.*)$" "\\1" TRIMMED_PATH
                         "${RELATIVE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${TRIMMED_PATH}")
    source_group("${GROUP_PATH}" FILES "${SOURCE_FILE}")
  endforeach()
endfunction()

add_definitions(-DNOMINMAX)

# ##################################### DXShell

file(
  GLOB_RECURSE
  DXShell_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include/*.hh)

filestofolders(DXShell_SOURCES)

list(FILTER DXShell_SOURCES EXCLUDE REGEX "/d3d/")
list(FILTER DXShell_SOURCES EXCLUDE REGEX "Messaging\\.cpp$")
list(FILTER DXShell_SOURCES EXCLUDE REGEX "Param\\.cpp$")

add_library(DXShell STATIC ${DXShell_SOURCES})
target_include_directories(
  DXShell PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include")
target_compile_definitions(DXShell PUBLIC _CRT_SECURE_NO_WARNINGS
                                          _CRT_SECURE_NO_DEPRECATE)
target_link_libraries(DXShell PUBLIC OpenGL::GL ${_zlib} ${_jpeg})
if(WIN32)
  target_link_libraries(DXShell PUBLIC dxguid winmm)
endif()

set(_minizip_dir "${THIRDPARTY_INSTALL_PREFIX}/include/contrib/minizip")
target_sources(DXShell PRIVATE "${_minizip_dir}/unzip.c" "${_minizip_dir}/ioapi.c")
set_source_files_properties("${_minizip_dir}/unzip.c" "${_minizip_dir}/ioapi.c"
                            PROPERTIES LANGUAGE C)

# ##################################### Diesel

file(
  GLOB_RECURSE
  Diesel_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/diesel/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/diesel/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/diesel/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/diesel/*.hh)

list(FILTER Diesel_SOURCES EXCLUDE REGEX "sroqplay\\.c$")
list(FILTER Diesel_SOURCES EXCLUDE REGEX "roqtoppm\\.c$")
list(FILTER Diesel_SOURCES EXCLUDE REGEX "TVector\\.cpp$")
list(FILTER Diesel_SOURCES EXCLUDE REGEX "PTSLoader\\.cpp$")
list(FILTER Diesel_SOURCES EXCLUDE REGEX "EngineTypes\\.cpp$")
list(FILTER Diesel_SOURCES EXCLUDE REGEX "DoomLexer\\.cpp$")
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  list(FILTER Diesel_SOURCES EXCLUDE REGEX "Memcpy\\.cpp$")
endif()

set(DXShell_RESOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/dxshell/include/win/DXShellLib.rc")
set(Diesel_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/diesel/diesel.rc")

filestofolders(Diesel_SOURCES)

add_executable(Diesel WIN32 ${Diesel_SOURCES} ${DXShell_RESOURCE}
                            ${Diesel_RESOURCE})

target_include_directories(
  Diesel PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/diesel" "${FMOD_ROOT}/inc")

target_link_libraries(Diesel PRIVATE DXShell OpenGL::GL ${_zlib} ${_jpeg})
if(TARGET OpenGL::GLU)
  target_link_libraries(Diesel PRIVATE OpenGL::GLU)
endif()

if(MSVC)
  set_target_properties(
    Diesel
    PROPERTIES OUTPUT_NAME Diesel
               OUTPUT_NAME_DEBUG DieselD
               LINK_FLAGS "/ignore:4099")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                            Diesel)
