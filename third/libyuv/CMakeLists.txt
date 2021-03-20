# CMakeLists for libyuv
# Originally created for "roxlu build system" to compile libyuv on windows
# Run with -DTEST=ON to build unit tests

# Copyright (c) 2014, Evs. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 		CMakeLists.txt
# Author: 		Bernard HARMEL
# Revision:		1.0
# 
# Remarks: 
# None
# 
# History:		05 June 2014: Initial version

###############################################################################
cmake_minimum_required(VERSION 3.0.2)
include(Helpers NO_POLICY_SCOPE)

project(libyuv VERSION 2.1.0)

#Compile_Visual_Studio_With_MD()

include("dependencies.cmake")

AddTargetPlatformDefinitions()

GetTargetOS             (TARGET_OS)
GetStandardLinkLibraries(STANDARD_LINK_LIBRARIES)
GetInstallCompilerSubdir(COMPILER_SUBDIR)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /EHsc")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD /EHsc")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /EHsc")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	# Update if necessary
	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wconversion -Wextra -Wno-long-long -pedantic")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -Wall -Wconversion -Wextra -Wno-long-long -pedantic -DNDEBUG -g")	#Add -g to debug release code
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -Wall -Wconversion -Wextra -Wno-long-long -pedantic -DDEBUG")	 #add -fsanitize=leak to check for leak
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC -Wall -Wconversion -Wextra -Wno-long-long -pedantic -DNDEBUG")			#Add -g to debug release code
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC -Wall -Wconversion -Wextra -Wno-long-long -pedantic -DDEBUG -g")	 #add -fsanitize=leak to check for leak	
endif()
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 
set(CMAKE_VERBOSE_MAKEFILE TRUE) 
set(CMAKE_COLOR_MAKEFILE  TRUE)

set(API_FILES
   source/compare.cc
   source/compare_common.cc
   source/compare_gcc.cc
   source/compare_mmi.cc
   source/compare_msa.cc
   source/compare_neon.cc
   source/compare_neon64.cc
   source/compare_win.cc
   source/convert.cc
   source/convert_argb.cc
   source/convert_from.cc
   source/convert_from_argb.cc
   source/convert_jpeg.cc
   source/convert_to_argb.cc
   source/convert_to_i420.cc
   source/cpu_id.cc
   source/mjpeg_decoder.cc
   source/mjpeg_validate.cc
   source/planar_functions.cc
   source/rotate.cc
   source/rotate_any.cc
   source/rotate_argb.cc
   source/rotate_common.cc
   source/rotate_gcc.cc
   source/rotate_mmi.cc
   source/rotate_msa.cc
   source/rotate_neon.cc
   source/rotate_neon64.cc
   source/rotate_win.cc
   source/row_any.cc
   source/row_common.cc
   source/row_gcc.cc
   source/row_mmi.cc
   source/row_msa.cc
   source/row_neon.cc
   source/row_neon64.cc
   source/row_win.cc
   source/scale.cc
   source/scale_any.cc
   source/scale_argb.cc
   source/scale_common.cc
   source/scale_gcc.cc
   source/scale_mmi.cc
   source/scale_msa.cc
   source/scale_neon.cc
   source/scale_neon64.cc
   source/scale_uv.cc
   source/scale_win.cc
   source/test.sh
   source/video_common.cc

   include/libyuv.h
   include/libyuv/basic_types.h
   include/libyuv/compare.h
   include/libyuv/compare_row.h
   include/libyuv/convert.h
   include/libyuv/convert_argb.h
   include/libyuv/convert_from.h
   include/libyuv/convert_from_argb.h
   include/libyuv/cpu_id.h
   include/libyuv/macros_msa.h
   include/libyuv/mjpeg_decoder.h
   include/libyuv/planar_functions.h
   include/libyuv/rotate.h
   include/libyuv/rotate_argb.h
   include/libyuv/rotate_row.h
   include/libyuv/row.h
   include/libyuv/scale.h
   include/libyuv/scale_argb.h
   include/libyuv/scale_row.h
   include/libyuv/scale_uv.h
   include/libyuv/version.h
   include/libyuv/video_common.h
)

set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
)

source_group("Api"						FILES ${API_FILES})

# ========================
# == BUILD DIRECTIVES   ==
# ========================

add_library(${PROJECT_NAME} STATIC  ${THIS_PROJECT_SOURCE_FILES})

if(${TARGET_OS} STREQUAL "WINDOWS")
	target_compile_definitions(  
		${PROJECT_NAME} PUBLIC 
		$<BUILD_INTERFACE:_WINSOCK_DEPRECATED_NO_WARNINGS>  
		$<BUILD_INTERFACE:_CRT_SECURE_NO_WARNINGS>  
		$<BUILD_INTERFACE:_CRT_NON_CONFORMING_SWPRINTFS>
		$<BUILD_INTERFACE:UNICODE>
	)	
else()	
	target_compile_definitions(  
		${PROJECT_NAME} PUBLIC 
	)	
endif()

target_compile_definitions(  
	${PROJECT_NAME} PUBLIC   
)	


#Define the project include directories
target_include_directories(
	${PROJECT_NAME} PUBLIC 
	#Header path used at build time
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	#Header path used when installed
	$<INSTALL_INTERFACE:include>
)



if (MSVC)  
    set(EXTRAS_OS_LIB
	)
else()
	set(EXTRAS_OS_LIB 
	)
endif()  


#target_link_libraries of a lib will be added when linking app to this lib
if (MSVC)
target_link_libraries(${PROJECT_NAME} PUBLIC
#  ${STANDARD_LINK_LIBRARIES} 
  ${EXTRAS_OS_LIB}
)
else()
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${EXTRAS_OS_LIB}
)
endif()

AddPdbGeneration(${PROJECT_NAME})

#enable_testing()
#add_subdirectory(unittest)

# ========================
# == INSTALL DIRECTIVES ==
# ========================
if ("${INSTALL_REV}" STREQUAL "")
#	message("No INSTALL_REV -> default to 69135")
#	set (INSTALL_REV .69135)
endif()
#will be installed in your local binaries repositories
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${PLATFORM_NAME}/${PROJECT_NAME}/${PROJECT_VERSION}${INSTALL_REV}") 
#Install CMake generated configuration files
#For each visual studio version, files are generated in separated sub-directory
install(EXPORT ${PROJECT_NAME} DESTINATION cmake/${COMPILER_SUBDIR})
#install(EXPORT ${CMAKE_PROJECT_NAME} DESTINATION cmake/${COMPILER_SUBDIR})
 
#Install custom CMake configuration file
install(FILES ${PROJECT_NAME}Config.cmake dependencies.cmake DESTINATION cmake) 
 
#Install produced binaries
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION lib/${COMPILER_SUBDIR}) 
#install(TARGETS ${PROJECT_NAME} EXPORT ${CMAKE_PROJECT_NAME} DESTINATION lib/${COMPILER_SUBDIR}) 
 
#Install pdb
InstallPdb(${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_SUBDIR})
  
#Install header file
install (DIRECTORY "./include/."													DESTINATION include					FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Lib)


