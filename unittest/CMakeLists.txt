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

cmake_minimum_required(VERSION 3.0.2)
#Launch from a Visual Studio x64 Command Prompt Console or a unix console
#cmake -DCMAKE_INSTALL_PREFIX=D:/repo //For cross compile -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchain_windows.cmake ..
#-DBLD_TARGET_PLATFORM=DESKTOP_DEBIAN -DCMAKE_FIND_ROOT_PATH=/Data/user/bha/repo
include("../../cmake/bld_toolbox.cmake")

project(ut_bofstd VERSION 2.2.6)

bld_cxx_init(DESKTOP_DEBIAN)

set(API_FILES
	include/gtestrunner.h
	source/main.cpp
	source/ut_callbackcollection.cpp 
	source/ut_2d.cpp
)

set(BOFIO_FILES
	source/ut_bofio.cpp
)

set(SERDER_FILES
	include/ut_parser.h
		source/ut_cmdlineparser.cpp
	source/ut_xmlparserwriter.cpp
	xmlparser.xml
	source/ut_jsonparserwriter.cpp	
	jsonparser.json	
)

set(LOG_FILES
	source/ut_stringformatter.cpp
	source/ut_logger.cpp
)
set(FS_FILES
	source/ut_fs.cpp
)

set(CRYPTO_FILES
	source/ut_crypto.cpp
)

set(CONTAINER_FILES
	source/ut_circularbuffer.cpp
	source/ut_rawcircularbuffer.cpp
	source/ut_stringcircularbuffer.cpp
	source/ut_pot.cpp
	source/ut_queue.cpp
)

set(COM_FILES
	source/ut_pipe.cpp
	source/ut_uart.cpp
	source/ut_socketos.cpp
	source/ut_sockettcp.cpp
	source/ut_socketudp.cpp
)

set(SYSTEM_FILES
        source/ut_scriptlauncher.cpp
        source/ut_system.cpp
        source/ut_string.cpp
        source/ut_boftimecode.cpp
)
set(BITENUMGUID_FILES
        source/ut_bit.cpp
        source/ut_enum.cpp
        source/ut_guid.cpp
)
set(THREADING_FILES
	source/ut_threading.cpp
	source/ut_asyncmulticastdelegate.cpp
)

set(DB_FILES
	include/ut_ramdb.h
	source/ut_ramdb.cpp
)


set(THIS_PROJECT_SOURCE_FILES
	${API_FILES}
	${BOFIO_FILES}
	${CRYPTO_FILES}
	${CONTAINER_FILES}
	${COM_FILES}
	${SYSTEM_FILES}
	${BITENUMGUID_FILES}
	${THREADING_FILES}
	${DB_FILES}
	${SERDER_FILES}
	${LOG_FILES}
	${FS_FILES}
)

source_group("Api"				FILES ${API_FILES})
source_group("BofIo"			FILES ${BOFIO_FILES})
source_group("Crypto"			FILES ${CRYPTO_FILES})
source_group("Container"		FILES ${CONTAINER_FILES})
source_group("Com Channel"		FILES ${COM_FILES})
source_group("System" 			FILES ${SYSTEM_FILES})
source_group("Bit_Enum_Guid"	FILES ${BITENUMGUID_FILES})
source_group("Threading"		FILES ${THREADING_FILES})
source_group("Database"			FILES ${DB_FILES})
source_group("Ser_Deser"			FILES ${SERDER_FILES})
source_group("Log"				FILES ${LOG_FILES})
source_group("FileSystem"       FILES ${FS_FILES})
source_group("StateMachine"       FILES ${STATEMACHINE_FILES})

add_executable(${PROJECT_NAME}  ${THIS_PROJECT_SOURCE_FILES})


bld_std_cxx_compile_link_setting()

target_compile_definitions(  
	${PROJECT_NAME} PUBLIC 
	$<BUILD_INTERFACE:$<TARGET_PROPERTY:bofstd,INTERFACE_COMPILE_DEFINITIONS>>  
)	
	
#Define the project include directories
target_include_directories(
	${PROJECT_NAME} PUBLIC
	#Header path used at build time
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	
	#Header path used when installed
	$<INSTALL_INTERFACE:include>
)

target_link_libraries(
  ${PROJECT_NAME} 
  GTest::gtest
#  gtest
  bofstd
  ${BLD_STANDARD_LINK_LIBRARIES}
)

#message("copy file to " ${CMAKE_CURRENT_BINARY_DIR})
file(COPY data/xmlparser.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY data/xmlvectorparser.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY data/jsonparser.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY data/colorbar_1920x1080p59_8bit_captured_by_sdk.422 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER Lib/Ut)

bld_show_info()
