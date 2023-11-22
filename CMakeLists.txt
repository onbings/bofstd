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

#cmake -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -DVCPKG_HOST_TRIPLET=x64-windows-static-md -DBUILD_SHARED_LIBS=OFF -DVCPKG_OVERLAY_PORTS=C:/pro/github/onbings-vcpkg-registry/ports -DCMAKE_INSTALL_PREFIX=C:\tmp\bofstd -B C:/bld/bofstd -S C:/pro/github/bofstd
#cmake --build .  --config Release -- -j 16
#cmake --install . --config Debug
cmake_minimum_required(VERSION 3.25)

# Paranoid check : in-source generation is forbidden
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "You cannot generate your binary solution into your source directory !")
endif()
 
include(vcpkg/fixup_vs_crt.cmake)
include(vcpkg/vcpkg_install.cmake)

# Programs used for integration testing should not be included
# in code coverage computation as they are test programs themselves.
set(EXTRA_COVERAGE_EXCLUSION "\'${CMAKE_CURRENT_SOURCE_DIR}/integration/*\'")

project(bofstd VERSION 5.3.4.5)
# Some naming conventions either requires lower or upper case.
# And some don't like underscore.
# So already prepare all those variables
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(REPLACE "-" "_" PROJECT_NAME_UNDERSCORE       ${PROJECT_NAME})
string(REPLACE "-" "_" PROJECT_NAME_LOWER_UNDERSCORE ${PROJECT_NAME_LOWER})
string(REPLACE "-" "_" PROJECT_NAME_UPPER_UNDERSCORE ${PROJECT_NAME_UPPER})

option(BOFSTD_BUILD_TESTS       "Build units tests of BofStd project"                                  ON)
option(BOFSTD_ENABLE_VERSIONING "Enable library versioning naming scheme for BofStd libraries"         ON)
option(BOFSTD_FORCE_EXPORT_ALL  "Force exporting all symbols of BofStd libraries (for shared library)" OFF)

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
# set(CMAKE_DEBUG_POSTFIX _d)

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

# ===============
# == PACKAGING ==
# ===============

include(packaging.cmake)

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

set(INTERNAL_NAMESPACE ONBINGS)

# Create and install the cmake configuration file based on our template
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}
)

unset(INTERNAL_NAMESPACE)

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



