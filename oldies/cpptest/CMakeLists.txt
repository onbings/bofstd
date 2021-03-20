# Copyright (c) 2014, Evs. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 			CMakeLists.txt
# Author: 		Bernard HARMEL
# Revision:		1.0
# 
# Remarks: 
# None
# 
# History:		05 June 2014: Initial version

###############################################################################
cmake_minimum_required(VERSION 3.0.2)

include(Helpers)

Project(cpptest VERSION 1.0.0)

include("dependencies.cmake")

# ====================================
# == Get general target information ==
#=====================================

AddTargetPlatformDefinitions()

GetTargetPlatform       (TARGET_PLATFORM)
GetTargetOS             (TARGET_OS)
GetTargetCPU            (TARGET_CPU)
GetHostCompiler         (HOST_COMPILER)
GetLikelyEndianness     (TARGET_ENDIANNESS)
GetStandardLinkLibraries(STANDARD_LINK_LIBRARIES)
GetInstallCompilerSubdir(COMPILER_SUBDIR)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/projectConfig.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake @ONLY) #for make install of win platform
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/projectConfig.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/${PLATFORM_NAME}/${PROJECT_NAME}Config.cmake @ONLY)  #for .bat copy repo
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/project.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/${PLATFORM_NAME}/${COMPILER_SUBDIR}/${PROJECT_NAME}.cmake @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/project-debug.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/${PLATFORM_NAME}/${COMPILER_SUBDIR}/${PROJECT_NAME}-debug.cmake @ONLY)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/project-release.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/cmake_repo/${PLATFORM_NAME}/${COMPILER_SUBDIR}/${PROJECT_NAME}-release.cmake @ONLY)
if (MSVC)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)

# =========================================
# == DATA AND SETTINGS FROM THIS PROJECT ==
#==========================================

#Api def file
set(API_FILES
	source/collectoroutput.cpp
	source/compileroutput.cpp
	source/htmloutput.cpp
	source/missing.cpp
	source/source.cpp
	source/suite.cpp
	source/textoutput.cpp
	source/time.cpp
	source/utils.cpp
	
	include/cpptest/cpptest-assert.h
	include/cpptest/cpptest-collectoroutput.h
	include/cpptest/cpptest-compileroutput.h
	include/cpptest/cpptest-htmloutput.h
	include/cpptest/cpptest-output.h
	include/cpptest/cpptest-source.h
	include/cpptest/cpptest-suite.h
	include/cpptest/cpptest-textoutput.h
	include/cpptest/cpptest-time.h
	include/cpptest/cpptest.h
	include/cpptest/missing.h
	include/cpptest/utils.h	
	include/cpptest/winconfig.h	
	include/cpptest/config.h	
)

#files member of this project
set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
)

#logical groups
source_group("Api"          FILES ${API_FILES})

# ==================================
# == BUILD AND INSTALL DIRECTIVES ==
# ==================================

#executable
add_library(
  ${PROJECT_NAME} STATIC
  ${THIS_PROJECT_SOURCE_FILES}  
)

#Define the project include directories
target_include_directories(
  ${PROJECT_NAME} 
  
  PUBLIC
  #Header path used at build time
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cpptest>  
#  $<BUILD_INTERFACE:${X264_INCLUDE_DIRS}>  
  #Header path used when installed
  $<INSTALL_INTERFACE:include>
  
  PRIVATE
#  $<TARGET_PROPERTY:coreinc,INTERFACE_INCLUDE_DIRECTORIES>
#  $<TARGET_PROPERTY:execstream,INTERFACE_INCLUDE_DIRECTORIES>
)

#link
GetStandardLinkLibraries(STANDARD_LINK_LIBRARIES)
if (MSVC)  
    set(EXTRAS_OS_LIB
	)
else()
	set(EXTRAS_OS_LIB 
	)
endif()  

#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
#       LINK_FLAGS_DEBUG
#       "/NODEFAULTLIB:libcmt.lib"
#)

target_link_libraries(
  ${PROJECT_NAME} 
  ${STANDARD_LINK_LIBRARIES}
  ${EXTRAS_OS_LIB}
	
#  ${X264_LIBRARIES}

)

AddPdbGeneration(${PROJECT_NAME})
#Not for a lib !!! ExtractGccDebugSymbols(${PROJECT_NAME})

# ============= 
# == SUBDIRS ==
# =============
#Add unittest
#enable_testing()
#add_subdirectory(unittest)

# ========================
# == INSTALL DIRECTIVES ==
# ========================

#will be installed in your local binaries repositories
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${PLATFORM_NAME}/${PROJECT_NAME}/${PROJECT_VERSION}${INSTALL_REV}") 

#Install CMake generated configuration files
#For each visual studio version, files are generated in separated sub-directory
#For Visual Studio get the specific version configuration subdirectory
install(EXPORT ${PROJECT_NAME} DESTINATION cmake/${COMPILER_SUBDIR})
 
#Install custom CMake configuration file
install(FILES ${PROJECT_NAME}Config.cmake dependencies.cmake DESTINATION cmake) 
 
#Install produced binaries
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION lib/${COMPILER_SUBDIR}) 
 
#Install pdb
InstallPdb(${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_SUBDIR})
  
#Install header file
install (DIRECTORY "./include/."  DESTINATION include PATTERN ".svn" EXCLUDE)

set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Lib)



