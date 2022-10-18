#
# Copyright (c) 2014, Onbings. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 		CMakeLists.txt
# Author: 		Bernard HARMEL
# Revision:		5.0
# 
# Remarks: 
# None
# 
# History:		05 June 2014: Initial version 
#               06 March 2021 remove dependency on third party and cmake helpers
#               22 April 2022 updated with NMA cmake and vcpkg ideas 

cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/fixup_vs_crt.cmake)

project(bofstd VERSION 5.1.16.3)

# Some naming conventions either requires lower or upper case.
# And some don't like underscore.
# So already prepare all those variables
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(REPLACE "-" "_" PROJECT_NAME_UNDERSCORE       ${PROJECT_NAME})
string(REPLACE "-" "_" PROJECT_NAME_LOWER_UNDERSCORE ${PROJECT_NAME_LOWER})
string(REPLACE "-" "_" PROJECT_NAME_UPPER_UNDERSCORE ${PROJECT_NAME_UPPER})

# Easily tweak build number if needed
option(BOFSTD_VERSION_TWEAK "The build number of bofstd" "")

if(BOFSTD_VERSION_TWEAK)
  set(PROJECT_VERSION_TWEAK ${BOFSTD_VERSION_TWEAK})
  set(PROJECT_VERSION       ${PROJECT_VERSION}.${PROJECT_VERSION_TWEAK})
endif()

# Define options to enable/disable non-necessary artifacts
option(BOFSTD_BUILD_TESTS    "Build units tests of bofstd"      ON)
option(BOFSTD_BUILD_TOOLS    "Build companion tools of bofstd"  ON)
option(BOFSTD_BUILD_EXAMPLES "Build examples of bofstd"         ON)

# Some options allow to tune the 'API & versioning' behaviour.
#  - Do we want to create binaries with explicit version in them (e.g. libbofstd.so.1.0.1) ?
#  - Do we want to force exporting all the symbols (API and non-API) [Not recommended] ?
option(BOFSTD_ENABLE_VERSIONING "Enable library versioning naming scheme for bofstd"          ON)
option(BOFSTD_FORCE_EXPORT_ALL  "Force exporting all symbols of bofstd (for shared library)" OFF)

# option(BOFSTD_GENERATE_HELP  "Generate documentation" OFF)
# if(BOFSTD_GENERATE_HELP)
#   find_package(Doxygen)
#   if (DOXYGEN_FOUND AND NOT TARGET documentation)
# #	message("CMAKE_CURRENT_BINARY_DIR" ${CMAKE_CURRENT_BINARY_DIR})
# 	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/help/bofstd.Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile @ONLY)
# 	add_custom_target(documentation ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)
#   endif()
# endif()

option(BOFSTD_GENERATE_HELP  			"Generate documentation" OFF)
# set(IT_PM_ADDRESS "" CACHE STRING "Execute Intergration Test on PM with ip address set to... (-it_pm_address)")
if(BOFSTD_GENERATE_HELP)
  find_program(THRIFT_EXECUTABLE thrift)
  message("=====================> Thrift compiler: " ${THRIFT_EXECUTABLE})
  if(${THRIFT_EXECUTABLE} STREQUAL "thrift-NOTFOUND")
    message(FATAL_ERROR "Could not find thrift executable. Is thrift in your dependencies ?")
  else()
    if (NOT TARGET DocThrift)
      message("Create DocThrift target") 
	  add_custom_target(DocThrift 
        COMMAND echo Create directory ${CMAKE_CURRENT_BINARY_DIR}/help/thrift
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/help/thrift
	    COMMAND echo "Thrift phase        " ${THRIFT_EXECUTABLE} -out ${CMAKE_CURRENT_BINARY_DIR}/help/thrift -v -r --gen html ${CMAKE_CURRENT_SOURCE_DIR}/../../bofstd/idl/main.thrift
        COMMAND echo "   Thrift result in " ${CMAKE_CURRENT_BINARY_DIR}/help/thrift
        COMMAND ${THRIFT_EXECUTABLE} -out ${CMAKE_CURRENT_BINARY_DIR}/help/thrift -v -r --gen html ${CMAKE_CURRENT_SOURCE_DIR}/../../bofstd/idl/main.thrift WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating bofstd thrift documentation" VERBATIM
      )
    endif()
  endif()
  find_package(Doxygen)
  message("=====================> Doxygen compiler: " ${DOXYGEN_EXECUTABLE})
  if(${DOXYGEN_EXECUTABLE} STREQUAL "doxygen-NOTFOUND")
    message(FATAL_ERROR "Could not find doxygen executable. Is doxygen installed ?")
  else()
    if (DOXYGEN_FOUND AND NOT TARGET DocApi)
	  message("Generate doxygen config file " ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile " from " ${CMAKE_CURRENT_SOURCE_DIR}/help/bofstd.Doxyfile.in)
	  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/help/bofstd.Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile @ONLY)
      message("Create DocApi target")
	  add_custom_target(DocApi 
        COMMAND echo "Doxygen phase        " ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile 
        COMMAND echo "   Doxygen result in " ${CMAKE_CURRENT_BINARY_DIR}/help
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/help/bofstd.Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating bofstd doxygen documentation" VERBATIM
      )
    else()
      message(FATAL_ERROR "DOXYGEN_FOUND ? " ${DOXYGEN_FOUND})
    endif()
  endif()
endif()


#option(BUILD_SHARED_LIBS "If ON, all libraries will be built shared unless the library was explicitly added as a static library"          ON)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
#Problem with vcpkg/cmake set(CMAKE_DEBUG_POSTFIX _d)

# ===============================
# == NON TRANSITIVE PROPERTIES ==
# ===============================

# Non transitive properties are properties that are not
# usage requirements. It means properties that only need
# to be set for building this target only. As such they
# don't need to propagated outside.
# For convenience, use global initializer for those

# Disable compiler extension to maximize portability
set(CMAKE_CXX_EXTENSIONS OFF)

# For shared libraries we need to compile with "-fPIC"
# But for static libraries user can choose
if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE  ${BUILD_SHARED_LIBS})
endif()

# Make sure MSVC runtime is consistent accross
# all languages (i.e. CXX, CUDA, etc, ...)
# cfr : https://gitlab.kitware.com/cmake/cmake/-/issues/19428
set_msvc_runtime_library()

# API visibility control
if(BOFSTD_FORCE_EXPORT_ALL)

  if(MSVC)

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

# This creates the 'test' target
# even if you don't have tests
enable_testing()

# ==================
# == CONFIG FILES ==
# ==================

# Add configuration files to a dummy library to
# make sure files are properly found
add_library(bofstd-config-files INTERFACE
  Config.cmake.in
  Dependencies.cmake.in
  Dependencies.private.cmake
  vcpkg-configuration.json
  vcpkg.json
  lib/CMakeLists.txt
)

# ===========================
# == EXTERNAL DEPENDENCIES ==
# ===========================

# We need to perform some "find_package" to find out our direct dependencies.
#
# Moreover, other projects that will depend on us will also have to perform
# those find_package in order to transitively link with our own dependencies.
#
# By isolating them in a file, we fulfill a double objective
#  1°) We can simply include it to find the dependencies we need
#  2°) We can later install that file and include it from our configuration
#      file. As such, our dependees will automatically transitively find
#      our dependencies
#
# Remark 1
# --------
#  As always, we might have 2 types of dependencies.
#    1°) The ones needed to link with our libraries (i.e. usage requirements)
#    2°) The ones only needed to build              (i.e. build requirements)
#
#   As a consequence, we should probably have 2 dependencies files
#     - A public  one that will be installed     (Dependencies.cmake)
#     - A private one that will NOT be installed (Dependencies.private.cmake)
#
# Remark 2
# --------
#  ALL dependencies used by the library (public AND private) should be referenced in the public
#  dependency file (Dependencies.cmake). Because,
#  1°) When compiled as a static library, even private libraries needs to be supplied to the linker.
#  2°) To run unit tests, we might still need to have found private libraries

# Give the file a nice naming convention
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bofstd-dependencies.cmake
  COPYONLY
)

include(${CMAKE_CURRENT_BINARY_DIR}/bofstd-dependencies.cmake)
include(Dependencies.private.cmake)

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

# ===========
# == BUILD ==
# ===========

# Mandatory
add_subdirectory(lib)

# Optional
if(BOFSTD_BUILD_EXAMPLES)
  add_subdirectory(example)
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

