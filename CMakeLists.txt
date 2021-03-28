# Copyright (c) 2014, Onbings. All rights reserved.
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
#               06 March 2021 remove dependency on third party and cmake helpers

cmake_minimum_required(VERSION 3.0.2)
#Launch from a Visual Studio x64 Command Prompt Console or a unix console
#cmake -DCMAKE_INSTALL_PREFIX=D:/repo //For cross compile -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchain_windows.cmake .. 
include("./cmake/bld_toolbox.cmake")

project(bofstd VERSION 3.1.0)

bld_cxx_init()

set(API_FILES
	include/bofstd/boferr.h
	include/bofstd/bofstd.h
	source/bofstd.cpp
	include/bofstd/bofdaemonservice.h
	source/bofdaemonservice.cpp
	include/bofstd/bofcallbackcollection.h
)


set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
	${BOFIO_FILES}
	${CRYPTO_FILES}
	${CONTAINER_FILES}
	${COM_FILES}
	${PROTO_FILES}
	${SYSTEM_FILES}
	${BITENUMGUID_FILES}
	${THREADING_FILES}
	${DB_FILES}
	${CONIO_FILES}
	${GRAPH_FILES}
	${PATTERN_FILES}
	${PARAMETER_FILES}
	${SERDER_FILES}
	${LOG_FILES}
	${FMT_FILES}
	${SPDLOG_FILES}
	${FS_FILES}
	${COMMANDLINE_FILES}
	${JSONCPP_FILES}
	${ASYNCMULTICASTDELEGATE_FILES}
	${YUV_FILES}
	${ZMQ_FILES}
	${CZMQ_FILES}
	${IXML_FILES}
)

source_group("Api"						FILES ${API_FILES})
source_group("BofIo"					FILES ${BOFIO_FILES})
source_group("Crypto"					FILES ${CRYPTO_FILES})
source_group("Container"				FILES ${CONTAINER_FILES})
source_group("ComChannel"				FILES ${COM_FILES})
source_group("Protocol"					FILES ${PROTO_FILES})
source_group("System"					FILES ${SYSTEM_FILES})
source_group("Bit_Enum_Guid"			FILES ${BITENUMGUID_FILES})
source_group("Threading"				FILES ${THREADING_FILES})
source_group("Database"					FILES ${DB_FILES})
source_group("Conio"					FILES ${CONIO_FILES})
source_group("Graphic"					FILES ${GRAPH_FILES})
source_group("Pattern"					FILES ${PATTERN_FILES})
source_group("Parameter"				FILES ${PARAMETER_FILES})
source_group("Ser_Deser"				FILES ${SERDER_FILES})
source_group("Ser_Deser\\JsonCpp"		FILES ${JSONCPP_FILES})
source_group("AsyncMulticastDelegate"	FILES ${ASYNCMULTICASTDELEGATE_FILES})
source_group("Yuv"						FILES 				${YUV_FILES})
source_group("Pattern"					FILES ${PATTERN_FILES})
source_group("Log"						FILES ${LOG_FILES})
source_group("Log\\Fmt"					FILES ${FMT_FILES})
source_group("Log\\SpdLog"				FILES ${SPDLOG_FILES})
source_group("FileSystem"				FILES ${FS_FILES})
source_group("CommandLine"				FILES ${COMMANDLINE_FILES})
source_group("Zmq"				FILES ${ZMQ_FILES})
source_group("CZmq"				FILES ${CZMQ_FILES})
source_group("IXml"				FILES ${IXML_FILES})

add_library(${PROJECT_NAME} STATIC  ${THIS_PROJECT_SOURCE_FILES})

bld_std_cxx_compile_link_setting()

#use external libfmt lib instead of the bundled one
#disable exception in spd log and replace thme by assert(false) to avoid try around every sprintf
target_compile_definitions(  
	${PROJECT_NAME} PUBLIC   
	$<BUILD_INTERFACE:SPDLOG_FMT_EXTERNAL>		#can also be set in tweakme.h
	$<BUILD_INTERFACE:SPDLOG_ENABLE_MESSAGE_COUNTER>	#can also be set in tweakme.h
	$<BUILD_INTERFACE:FMT_EXCEPTIONS>  
	$<BUILD_INTERFACE:ZMQ_DEFINED_STDINT>  
	$<BUILD_INTERFACE:ZMQ_STATIC>
	$<BUILD_INTERFACE:CZMQ_STATIC>
	$<BUILD_INTERFACE:IXML_STATIC_LIB>
	$<BUILD_INTERFACE:${BLD_TARGET_PLATFORM}>
	$<INSTALL_INTERFACE:SPDLOG_ENABLE_MESSAGE_COUNTER>	#can also be set in tweakme.h
)	

#Define the project include directories
target_include_directories(
	${PROJECT_NAME} PUBLIC 
	#Header path used at build time
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/fmt-5.3.0/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/spdlog-1.3.1/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/jsoncpp-1.8.0/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/cpp-linenoise-master>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/AsyncMulticastDelegate/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/libyuv/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/zeromq/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/czmq/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/inc>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/src/inc>

	#Header path used when installed
	$<INSTALL_INTERFACE:include>
	$<INSTALL_INTERFACE:include/fmt>  
	$<INSTALL_INTERFACE:include/spdlog>  
	$<INSTALL_INTERFACE:include/jsoncpp>  
	$<INSTALL_INTERFACE:include/async>
	$<INSTALL_INTERFACE:include/libyuv>
)

#target_link_libraries of a lib will be added when linking app to this lib
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${BLD_STANDARD_LINK_LIBRARIES} 
)

enable_testing()

#add_subdirectory(unittest)

bld_std_cxx_install_setting()

install (DIRECTORY "./include/."													DESTINATION include					FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/fmt-5.3.0/include/.			DESTINATION include/fmt				FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/spdlog-1.3.1/include/.			DESTINATION include/spdlog			FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/jsoncpp-1.8.0/include/json/.	DESTINATION include/jsoncpp/json	FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/AsyncMulticastDelegate/include/asyncmulticastdelegate/.	DESTINATION include/async/asyncmulticastdelegate			FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/libyuv/include/.	DESTINATION include/libyuv			FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Lib)

bld_show_info()


