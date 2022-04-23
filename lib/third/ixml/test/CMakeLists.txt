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

Project(ut_ixml VERSION 1.0.0)

include("dependencies.cmake")

set(API_FILES
	test_document.c	
)
file(GLOB_RECURSE RES_FILES "testdata/*.*")

#files member of this project
set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
	${RES_FILES}
)

#logical groups
source_group("Api" 	FILES ${API_FILES})
source_group("Res" 	FILES ${RES_FILES})

# ======================
# == BUILD DIRECTIVES ==
# ======================

#Define the project target type
add_executable(
  ${PROJECT_NAME}
  ${THIS_PROJECT_SOURCE_FILES}  
)

#Define the project include directories
target_include_directories(
  ${PROJECT_NAME} 
  
  PUBLIC
  #Header path used at build time
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../inc>  
  #Header path used when installed
  $<INSTALL_INTERFACE:inc>
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
    ixml

)

set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER App)

if(0)
  enable_testing()
  add_subdirectory(unittest)
endif()




