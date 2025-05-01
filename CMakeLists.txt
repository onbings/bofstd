#
# Copyright (c) 2044,2040 Onbings. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Author: 		Bernard HARMEL onbings@gmail.com b.harmel@gmail.com
# Revision:		5.0
# 
# History:		05 June 2004: BHA: Initial version 

#git clone https://github.com/onbings/bofstd.git

#compile with emscripten: 
#cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/pro/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DBUILD_SHARED_LIBS=OFF -DRHEL:STRING="el9" -DCMAKE_GENERATOR=Ninja -DCMAKE_BUILD_TYPE=Debug -SC:/pro/github/bofstd -BC:/bld/bofstd-web -DCMAKE_CROSSCOMPILING_EMULATOR=C:/pro/emsdk/node/16.20.0_64bit/bin/node.exe -G Ninja  
#cmake -DCMAKE_INSTALL_PREFIX=/home/bha/bld/bofstd-web/binaries -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=/home/bha/pro/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_TOOLCHAIN_FILE=/home/bha/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DBUILD_SHARED_LIBS=OFF -DRHEL:STRING="el9" -DCMAKE_BUILD_TYPE=Debug -S/home/bha/pro/evs-xts -B/home/bha/bld/bofstd-web -DCMAKE_CROSSCOMPILING_EMULATOR=/home/bha/pro/emsdk/node/16.20.0_64bit/bin/node -G Ninja
#run
#cd C:\bld\bofstd-web\binaries\bin
#node AppBofStd.js
#RUN: launch http server
#emrun --browser chrome --port 8080 --serve_after_close --serve_after_exit C:\bld\bofstd-web\binaries\bin
#select .html file 
#compil windows:
#cmake -DCMAKE_INSTALL_PREFIX=C:/bld/github-win/binaries -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_OVERLAY_PORTS=C:/pro/vcpkg/ports  -SC:/pro/github/bofstd -BC:/bld/bofstd-win
#compil linux:
#cmake -DCMAKE_TOOLCHAIN_FILE=/home/bha/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_OVERLAY_PORTS=/home/bha/pro/vcpkg/ports -DCMAKE_INSTALL_PREFIX=/tmp/bofstd -DRHEL:STRING="el9" -S/home/bha/pro/github/bofstd -B/home/bha/bld/bofstd-lin


#cmake --build . --config Release -j 16
#cmake --install . --config Debug

#vcpkg list --x-install-root=C:/bld/bofstd-win/vcpkg_installed/
cmake_minimum_required(VERSION 3.25)

# Paranoid check : in-source generation is forbidden
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "You cannot generate your binary solution into your source directory !")
endif()
 
include(cmake/fixup_vs_crt.cmake)
include(cmake/vcpkg_install.cmake)
include(cmake/fetch_content.cmake)

# Programs used for integration testing should not be included
# in code coverage computation as they are test programs themselves.
set(EXTRA_COVERAGE_EXCLUSION "\'${CMAKE_CURRENT_SOURCE_DIR}/integration/*\'")

project(bofstd VERSION 5.8.2.1)

if (EMSCRIPTEN)
message("Force pthread detection for BofStd compilation under EMSCRIPTEN")
set (THREADS_HAVE_PTHREAD_ARG ON)
endif()

# Some naming conventions either requires lower or upper case.
# And some don't like underscore.
# So already prepare all those variables
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(REPLACE "-" "_" PROJECT_NAME_UNDERSCORE       ${PROJECT_NAME})
string(REPLACE "-" "_" PROJECT_NAME_LOWER_UNDERSCORE ${PROJECT_NAME_LOWER})
string(REPLACE "-" "_" PROJECT_NAME_UPPER_UNDERSCORE ${PROJECT_NAME_UPPER})
file(REMOVE ${CMAKE_BINARY_DIR}/DepManifest.txt)

option(BOFSTD_BUILD_EXAMPLES    "Build example of BofStd project"                                      ON)
option(BOFSTD_BUILD_TESTS       "Build units tests of BofStd project"                                  ON)
option(BOFSTD_BUILD_TOOLS       "Build tools of BofStd project"                                        OFF)
option(BOFSTD_ENABLE_VERSIONING "Enable library versioning naming scheme for BofStd libraries"         ON)
option(BOFSTD_FORCE_EXPORT_ALL  "Force exporting all symbols of BofStd libraries (for shared library)" OFF)
option(INSTALL_GTEST  "Disable installation of gtest" OFF)
#EMSCRIPTEN
option(EMSCRIPTEN_GEN_HTML "EMSCRIPTEN generateq .wasm and .js file. If you set this option to ON we also generate an html container" OFF)
set(BOFSTD_EMSCRIPTEN_MEMORY 512MB CACHE STRING "Specify emscripten default memory size")
message("emscripten default memory size: ${BOFSTD_EMSCRIPTEN_MEMORY}")
set(BOFSTD_EMSCRIPTEN_STACK 8MB CACHE STRING "Specify emscripten default stack size")
message("emscripten default stack size: ${BOFSTD_EMSCRIPTEN_STACK}")
set(BOFSTD_EMSCRIPTEN_THREAD_POOL 4 CACHE STRING "Specify emscripten default thread pool size")
message("emscripten default thread pool size: ${BOFSTD_EMSCRIPTEN_THREAD_POOL}")
set(BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR ${CMAKE_BINARY_DIR}/binaries/bin/data CACHE STRING "Specify emscripten fs source dir")
message("emscripten fs source file directory: ${BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR}")
set(BOFSTD_EMSCRIPTEN_FS_DST_ROOT_DIR data CACHE STRING "Specify emscripten fs destination dir")
message("emscripten fs destination file directory: ${BOFSTD_EMSCRIPTEN_FS_DST_ROOT_DIR}")
#set(BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/tests/data CACHE STRING "Specify emscripten preload file dir")
#em++: error: 'C:\pro\emsdk\upstream\emscripten\tools\file_packager.bat binaries\bin\app_bofstd.data --from-emcc --preload C:/bld/bofstd-web/binaries/bin/data' failed (returned 1)
# ===============================
# == NON TRANSITIVE PROPERTIES ==
# ===============================

# Non transitive properties are properties that are not
# usage requirements. It means properties that only need
# to be set for building this target only. As such they
# don't need to propagated outside.
# For convenience, use global initializer for those

# Disable compiler extension to maximize portability
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
#SET(INSTALL_GTEST OFF)
# set(CMAKE_DEBUG_POSTFIX _d)
if (EMSCRIPTEN)
#FS layout (Memory)
#Root: /tmp
#Root: /home
#Root: /dev
#Root: /proc
#Root: /C:
#home: '/home/web_user/' Sz 4096 Ft 2
#dev: '/dev/shm/tmp/' Sz 4096 Ft 2
#dev: '/dev/shm/' Sz 4096 Ft 2
#proc: '/proc/self/' Sz 4096 Ft 2
#--embed-file ${BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR}
#file: '/C:/bld/bofstd-web/binaries/bin/data/colorbar.jpg' Sz 9830 Ft 1
#file: '/C:/bld/bofstd-web/binaries/bin/data/colorbar.png' Sz 1483 Ft 1
#...
#file: '/C:/bld/bofstd-web/binaries/bin/data/' Sz 4096 Ft 2
#file: '/C:/bld/bofstd-web/binaries/bin/' Sz 4096 Ft 2
#file: '/C:/bld/bofstd-web/binaries/' Sz 4096 Ft 2
#file: '/C:/bld/bofstd-web/' Sz 4096 Ft 2
#file: '/C:/bld/' Sz 4096 Ft 2
#
#OR --embed-file ${BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR}@${BOFSTD_EMSCRIPTEN_FS_DST_ROOT_DIR}
#data: '/data/colorbar.jpg' Sz 9830 Ft 1
#data: '/data/colorbar.png' Sz 1483 Ft 1
#data: '/data/colorbar_1920x1080p59_8bit_captured_by_sdk.422' Sz 2073600 Ft 1
# with TEST(Fs_Test, FileLayout)
#  pwd is /TstRoot/
#  file: '/TstRoot/SubDir_0/Level1/Level2/File_000000.2.ren' Sz 2000 Ft 1
#  file: '/TstRoot/SubDir_0/Level1/Level2/File_000000.2' Sz 2000 Ft 1
#  file: '/TstRoot/SubDir_0/Level1/Level2/' Sz 4096 Ft 2
#  file: '/TstRoot/SubDir_0/Level1/' Sz 4096 Ft 2
#  file: '/TstRoot/SubDir_0/' Sz 4096 Ft 2
#
#FS Layout (persisten): stored at C:\Users\bha\AppData\Local\Google\Chrome\User Data\Default\IndexedDB
#
#sometime it cannot find pthread. If i force it in C:\Program Files\CMake\share\cmake-3.28\Modules\FindThreads.cmake it works...
#macro(_threads_check_libc)
#      set(CMAKE_THREAD_LIBS_INIT "")
#      set(Threads_FOUND TRUE)
  if (EMSCRIPTEN_GEN_HTML)
    message("===EMSCRIPTEN=== Add .html to .wasm and .js files")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")   #to generate .html in addition to .js and .wasm
  else()
    message("===EMSCRIPTEN=== Generate .wasm and .js files")
  endif()

  #her we can kee -fwasm-exceptions but not on dear imgui app
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -pthread -fwasm-exceptions")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fwasm-exceptions")

  #The @ symbol is needed because sometimes it is useful to package files that are not nested below the compile-time directory, and for which #there is therefore no default mapping to a location in the virtual file system.
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fwasm-exceptions -s TOTAL_STACK=${BOFSTD_EMSCRIPTEN_STACK} -s TOTAL_MEMORY=${BOFSTD_EMSCRIPTEN_MEMORY} -s PTHREAD_POOL_SIZE=${BOFSTD_EMSCRIPTEN_THREAD_POOL} --embed-file ${BOFSTD_EMSCRIPTEN_FS_SRC_ROOT_DIR}@${BOFSTD_EMSCRIPTEN_FS_DST_ROOT_DIR}")    
  #-sPROXY_TO_PTHREAD  not good for  imgui car je pense qu'on est dans un autre thread entre imgui et le main...
  #-s ASYNCIFY (very long and sometime forever)
  message("===EMSCRIPTEN=== CMAKE_EXE_LINKER_FLAGS is ${CMAKE_EXE_LINKER_FLAGS}")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  endif ()
  message("===EMSCRIPTEN=== CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
endif()
#  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /utf-8")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# For shared libraries we need to compile with "-fPIC"
# But for static libraries user can choose
if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE  ${BUILD_SHARED_LIBS})
endif()

# API visibility control
if(BOFSTD_FORCE_EXPORT_ALL)

  if(MSVC AND ${BUILD_SHARED_LIBS})
    message(WARNING "\nYou requested to force exporting ALL symbols "
                    "in a Windows DLL through CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS\n"
                    "This is a hack that has limitations. Use it as a temporary workaround")
  endif()

  # By "default",
  #  - GCC and Clang export everything (i.e. nothing to do)
  #  - MSVC hides everything and requires explicit listing
  #
  # This command tells CMake to generate such listing
  # for all the symbols found in the binary
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS   ON)

else()

  # We only want the API symbols to be visible
  # Tell CMake to pass the proper flags to hide
  # everything that is not explicitly marked
  set(CMAKE_CXX_VISIBILITY_PRESET         hidden)
  set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN ON)

endif()

enable_testing()

# Make sure MSVC runtime is consistent accross
# all languages (i.e. CXX, CUDA, etc, ...)
# cfr : https://gitlab.kitware.com/cmake/cmake/-/issues/19428
set_msvc_runtime_library()

# Fix rpath from installed vpckg dependencies
patch_vcpkg_installed_rpath()

# ==============================
# == PRESET INSTALL VARIABLES ==
# ==============================

# This include default installation directories
# based on detection performed by CMake. They
# are just default that can be overriden by the user.
include(GNUInstallDirs)
include(CMakePackageConfigHelpers) 
# It creates for example the following variables
#  ${CMAKE_INSTALL_BINDIR}      --> bin
#  ${CMAKE_INSTALL_LIBDIR}      --> lib, lib64, etc, ...
#  ${CMAKE_INSTALL_INCLUDEDIR}  --> include
#
# All those paths are relative to ${CMAKE_INSTALL_PREFIX} that is not
# known at configuration time and can be overriden by the user when installing.
#
# For platforms that support RPATH and ORIGIN, we can then compute the
# relative path from ${CMAKE_INSTALL_BINDIR} to ${CMAKE_INSTALL_PREFIX}
# example :
#   bin           --> some/weird/path
#   relative path --> ../../../
#
# and override RPATH with $ORIGIN/RELATIVE_PATH/${CMAKE_INSTALL_LIBDIR} at installation
#
file(RELATIVE_PATH BIN_TO_BASE_INSTALL_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_BINDIR} ${CMAKE_CURRENT_LIST_DIR})
set (RELATIVE_PATH_BIN_TO_LIB ${BIN_TO_BASE_INSTALL_PATH}${CMAKE_INSTALL_LIBDIR})
#see also update_rpath function in update_rpath.cmake

# =============================
# == SETUP BUILD DIRECTORIES ==
# =============================

# Pre-stage binaries like if they were already installed
#  - CMAKE_LIBRARY_OUTPUT_DIRECTORY is for shared libraries
#  - CMAKE_ARCHIVE_OUTPUT_DIRECTORY is for static libraries
#  - CMAKE_RUNTIME_OUTPUT_DIRECTORY is for applications
set(COMPILED_BINARIES_DIR ${CMAKE_BINARY_DIR}/binaries)
set(BASE_RESOURCES_DIR    ${CMAKE_BINARY_DIR}/resources)
 
if(WIN32)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMPILED_BINARIES_DIR}/bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMPILED_BINARIES_DIR}/bin)
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMPILED_BINARIES_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMPILED_BINARIES_DIR}/bin)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMPILED_BINARIES_DIR}/lib)


# =======================
# == POPULATE PROJECTS ==
# =======================

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

set(VCPKG_DEBUG_LIB_DIR   "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/lib")
set(VCPKG_RELEASE_LIB_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib")

# Fix RPATH depending on build type
if("x${CMAKE_BUILD_TYPE_LOWER}x" STREQUAL "xdebugx")
  set(CMAKE_BUILD_RPATH "${VCPKG_DEBUG_LIB_DIR};${VCPKG_RELEASE_LIB_DIR}")
else()
  set(CMAKE_BUILD_RPATH "${VCPKG_RELEASE_LIB_DIR};${VCPKG_DEBUG_LIB_DIR}")
endif()

# To avoid problem with EMSCRIPTEN compilation we only get third party libs which dont 
# depend on pthread lib. These one are get using the fetch_vcpkg_library function as
# they must be built with a strict control of options which is not possible with vcpkg 
# build in manifest mode 
message("==============================================start fetch=========================")
fetch_external_library_add(googletest "${LIST_OF_LIB}" https://github.com/google/googletest release-1.12.1)
#fetch_external_library_add(spdlog "${LIST_OF_LIB}" https://github.com/gabime/spdlog v1.12.0)
make_external_library_available("${LIST_OF_LIB}")
#target_compile_definitions(spdlog PUBLIC SPDLOG_HEADER_ONLY)
message("==============================================END fetch=========================")
include(Dependencies.cmake)

find_package(Doxygen)
message("=====================> Doxygen compiler: " ${DOXYGEN_EXECUTABLE})
if(${DOXYGEN_EXECUTABLE} STREQUAL "doxygen-NOTFOUND")
  message("WARNING: Could not find doxygen executable. Is doxygen installed ?")
endif()

# Mandatory
add_subdirectory(lib)

# Optional
if(BOFSTD_BUILD_EXAMPLES)
  add_subdirectory(apps)
endif()

if(BOFSTD_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BOFSTD_BUILD_TOOLS)
  add_subdirectory(tools)
endif()
add_subdirectory(lib/third/spdlog)

# ==========================
# == PACKAGE INSTALLATION ==
# ==========================

# Export our targets
install(
  EXPORT      bofstd
  FILE        bofstd-targets.cmake
  NAMESPACE   ONBINGS::
  DESTINATION ${CMAKE_INSTALL_DATADIR}
  COMPONENT   bofstd-devel
)

# ===============
# == PACKAGING ==
# ===============

#if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#  # make package only if BofStd is the main project (not a submodule)
#  include(packaging.cmake)
#endif()
if (PROJECT_IS_TOP_LEVEL)
   set(RHEL "el9")
   include(cmake/cpack_main.cmake)
endif()

set(INTERNAL_NAMESPACE ONBINGS)
# Create and install the cmake configuration file based on our template
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}
)
unset(INTERNAL_NAMESPACE)

# Install dependencies
set(INCLUDE_REGEXES "^${CMAKE_BINARY_DIR}/vcpkg_installed/")
set(EXCLUDE_REGEXES 
  ".*[Aa][Pp][Ii]-[Mm][Ss].*"                                                         # *api-ms*
  ".*[Ee][Xx][Tt]-[Mm][Ss]-[Ww][Ii][Nn].*"                                            # *ext-ms-win*
  ".*[/\\\\][Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss][Yy][Ss][Tt][Ee][Mm]32[/\\\\].*"   # *\Windows\System32\*
  "^/lib"                                                                             # /lib (unix system)
  "^/usr/lib"                                                                         # /usr/lib (unix system)
) 

install(RUNTIME_DEPENDENCY_SET bofstd-imported-runtime-set
  PRE_INCLUDE_REGEXES    ${INCLUDE_REGEXES}
  PRE_EXCLUDE_REGEXES    ${EXCLUDE_REGEXES}
  POST_INCLUDE_REGEXES   ${INCLUDE_REGEXES}
  POST_EXCLUDE_REGEXES   ${EXCLUDE_REGEXES}
  COMPONENT              bofstd-runtime
)

# Create configuration file so that find_package() from
# our dependees can find us
include(CMakePackageConfigHelpers)

# Give the file a nice naming convention
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-dependencies.cmake
  COPYONLY
)

# Generate the version file and indicates compatibility strategy
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-version.cmake
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

# Install them
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-version.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-dependencies.cmake
  DESTINATION ${CMAKE_INSTALL_DATADIR}
  COMPONENT   bofstd-devel
)

if (PROJECT_IS_TOP_LEVEL)
   set(INSTALL_GTEST OFF)
# Add a custom command to run the vcpkg list command during configuration
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E env vcpkg list --x-install-root=./vcpkg_installed
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}  # Change to your desired working directory
		OUTPUT_VARIABLE DepVcpkg
	)
	file(APPEND ${CMAKE_BINARY_DIR}/DepManifest.txt "Vcpkg Dependencies for ${PROJECT_NAME}:\n")
	file(APPEND ${CMAKE_BINARY_DIR}/DepManifest.txt "${DepVcpkg}")
endif()