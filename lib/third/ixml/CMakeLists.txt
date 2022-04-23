# Copyright (c) 2015, Onbings. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 		CMakeLists.txt
# Author: 		Bernard HARMEL (onbings@gmail.com)
# Revision:		1.0
# 
# Remarks: 
# None
# 
# History:		03 March 2016: Initial version

###############################################################################
cmake_minimum_required(VERSION 3.0.2)

include(Helpers)
include(CFlagsOverride)

Project(ixml VERSION 1.2.1)

GetTargetOS             (TARGET_OS)
GetTargetCPU            (TARGET_CPU)
GetInstallCompilerSubdir(COMPILER_SUBDIR)

include("dependencies.cmake")

set(API_FILES
	inc/ixml.h
	inc/ixmldebug.h
	src/attr.c
	src/document.c
	src/element.c
	src/ixml.c
	src/ixmldebug.c
	src/ixmlmembuf.c
	src/ixmlparser.c
	src/namedNodeMap.c
	src/node.c
	src/nodeList.c
	src/inc/ixmlmembuf.h
	src/inc/ixmlparser.h	
)

set(DOC_FILES
	doc/docxx.sty
	doc/intro.dxx
	doc/ixml.dxx
	doc/license.dxx
	doc/Makefile.am
	doc/Makefile.in
)

#files member of this project
set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
	${DOC_FILES}
)

#logical groups
source_group("Api" 	FILES ${API_FILES})
source_group("Doc" 	FILES ${DOC_FILES})

# ======================
# == BUILD DIRECTIVES ==
# ======================

#Define the project target type
add_library(
  ${PROJECT_NAME} STATIC
  ${THIS_PROJECT_SOURCE_FILES}  
)

if(${TARGET_OS} STREQUAL "WINDOWS")
add_definitions("-DIXML_USE_MSVCPP")
endif()
# define inherited by projet using the lib
target_compile_definitions(${PROJECT_NAME} PUBLIC 
	-DIXML_STATIC_LIB
)

#Define the project include directories
target_include_directories(
  ${PROJECT_NAME} 
  
  PUBLIC
  #Header path used at build time
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>  
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/inc>  
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../upnp/inc>  
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../build/inc>  
  #Header path used when installed
  $<INSTALL_INTERFACE:include>
#  $<INSTALL_INTERFACE:${SPDLOG_INCLUDE_DIRS}>
  
#  PRIVATE
#  $<TARGET_PROPERTY:coreinc,INTERFACE_INCLUDE_DIRECTORIES>
#  $<TARGET_PROPERTY:execstream,INTERFACE_INCLUDE_DIRECTORIES>
)


ExtractGccDebugSymbols(${PROJECT_NAME} )
AddPdbGeneration(${PROJECT_NAME} )
#link
GetStandardLinkLibraries(STANDARD_LINK_LIBRARIES)
if (MSVC)  
    set(EXTRAS_OS_LIB
	)
else()
	set(EXTRAS_OS_LIB 
	)
endif()  

target_link_libraries(
  ${PROJECT_NAME} 
  ${STANDARD_LINK_LIBRARIES}
  ${EXTRAS_OS_LIB}
)

set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Libraries)

add_subdirectory(test)
if(0)
  enable_testing()
  add_subdirectory(unittest)
endif()


# ========================
# == INSTALL DIRECTIVES ==
# ========================


#will be installed in your local binaries repositories
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${PLATFORM_NAME}/${PROJECT_NAME}/${PROJECT_VERSION}${INSTALL_REV}") 
#Install CMake generated configuration files
#For each visual studio version, files are generated in separated sub-directory
install(EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake/${COMPILER_SUBDIR})
 
#Install custom CMake configuration file
install(FILES ${PROJECT_NAME}Config.cmake dependencies.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake) 
 
#Install produced binaries
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_SUBDIR}) 
 
#Install pdb
InstallPdb(${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_SUBDIR})
  
#Install header file
install (DIRECTORY "./inc/."												DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ixml					FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Lib)
