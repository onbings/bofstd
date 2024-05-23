include(GoogleTest)

# =============
# == SOURCES ==
# =============
  
set(DATA_FILES
  ./data/xmlparser.xml
  ./data/xmlvectorparser.xml
  ./data/jsonparser.json
  ./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422
  ./data/colorbar.jpg
  ./data/colorbar.png
  ./data/football_seal.tga
  ./data/colorbar_in_raw_32b_194x259.raw
  ./data/colorbar_jpg_with_bad_ext.png
  ./data/colorbar_jpg_with_no_ext
  ./data/colorbar_png_with_bad_ext.jpg
  ./data/colorbar_png_with_no_ext
  ./data/colorbar_uncompress_32b.tga
  ./data/colorbar_rle_32b.tga
  ./data/sample-mp4-file.mp4
  ./data/shell_script_1_lin.bof
  ./data/shell_script_1_win.bof
  ./data/shell_script_2.bof
)

set(API_FILES
	include/gtestrunner.h
	src/main.cpp
	src/ut_api.cpp
)

set(BIT_FILES
    src/ut_bit.cpp
)

set(CALLBACK_FILES
	src/ut_async.cpp 
	src/ut_callbackcollection.cpp 
)

set(CLI_FILES
	src/ut_cmdlineparser.cpp
)

set(COM_FILES
	src/ut_socketos.cpp
	src/ut_sockettcp.cpp
	src/ut_socketudp.cpp
)
if (EMSCRIPTEN)
else()
	list(APPEND COM_FILES
		src/ut_pipe.cpp
		src/ut_uart.cpp
	)
endif()

set(CONIO_SHELL_FILES
	src/ut_shell.cpp
	src/ut_conio.cpp
)

set(CONTAINER_FILES
	src/ut_narytreekv.cpp
#	src/ut_spscqueue.cpp
#	src/ut_mpmcqueue.cpp
	src/ut_circularbuffer.cpp
	src/ut_rawcircularbuffer.cpp
	src/ut_stringcircularbuffer.cpp
	src/ut_pot.cpp
	src/ut_queue.cpp
)

set(CRYPTO_FILES
	src/ut_crypto.cpp
)

set(DAEMON_FILES
)

set(DB_FILES
	include/ut_ramdb.h
	src/ut_ramdb.cpp
)

#set(DELEGATE_FILES
#	src/ut_asyncmulticastdelegate.cpp
#)

set(ENUM_FLAG_FILES
	src/ut_enum.cpp
)

set(FS_FILES
	src/ut_path.cpp
	src/ut_uri.cpp
	src/ut_fs.cpp
)

set(IXML_FILES
)

set(LOG_FILES
	src/ut_stringformatter.cpp
	src/ut_logger.cpp
)

set(OBSERVER_FILES
)

set(PROTOCOL_FILES
)

set(SERDER_FILES
	include/ut_parser.h
	src/ut_xmlparserwriter.cpp
	data/xmlparser.xml
	src/ut_jsonparserwriter.cpp	
	data/jsonparser.json	
	src/ut_binserializer.cpp	
)

set(SOCKIO_FILES
	src/ut_sockio.cpp
#	src/ut_sockio_client_server.cpp
)

set(SYSTEM_FILES
        src/ut_graph.cpp
        src/ut_scopeguard.cpp
        src/ut_string.cpp
        src/ut_timecode.cpp
        src/ut_guid.cpp
        src/ut_datetime.cpp
)

set(THREADING_FILES
	src/ut_threading.cpp
    src/ut_process.cpp
    src/ut_system.cpp
)


set(SOURCES_FILES
	${API_FILES}
	${BIT_FILES}
	${CALLBACK_FILES}
	${CLI_FILES}
	${COM_FILES}
	${CONIO_SHELL_FILES}
	${CONTAINER_FILES}
	${CRYPTO_FILES}
	${DAEMON_FILES}
	${DB_FILES}
	#${DELEGATE_FILES}
	${ENUM_FLAG_FILES}
	${FS_FILES}
	${IXML_FILES}
	${LOG_FILES}
	${OBSERVER_FILES}
	${PROTOCOL_FILES}
	${SERDER_FILES}
	${SOCKIO_FILES}
	${SYSTEM_FILES}
	${THREADING_FILES}
)

# For Visual Studio
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/bofstd PREFIX "public" FILES ${PUBLIC_INCLUDES_FILES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src            PREFIX "src"    FILES ${SOURCES_FILES})

source_group("Api"		 FILES ${API_FILES})
source_group("Bit"		 FILES ${BIT_FILES})
source_group("Callback"	 FILES ${CALLBACK_FILES})
source_group("Cli"		 FILES ${CLI_FILES})
source_group("Com"		 FILES ${COM_FILES})
source_group("ConIo"	 FILES ${CONIO_SHELL_FILES})
source_group("Container" FILES ${CONTAINER_FILES})
source_group("Crypto"	 FILES ${CRYPTO_FILES})
source_group("Daemon"	 FILES ${DAEMON_FILES})
source_group("Db"		 FILES ${DB_FILES})
#source_group("Delegate"	 FILES ${DELEGATE_FILES})
source_group("EnumFlag"	 FILES ${ENUM_FLAG_FILES})
source_group("Fs"		 FILES ${FS_FILES})
source_group("Xml"		 FILES ${IXML_FILES})
source_group("Log"		 FILES ${LOG_FILES})
source_group("Observer"	 FILES ${OBSERVER_FILES})
source_group("Protocol"	 FILES ${PROTOCOL_FILES})
source_group("SerDeser"	 FILES ${SERDER_FILES})
source_group("SockIo"	 FILES ${SOCKIO_FILES})
source_group("System"	 FILES ${SYSTEM_FILES})
source_group("Threading" FILES ${THREADING_FILES})

# ===========
# == BUILD ==
# ===========

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/bigobj)
else()
    # lots of warnings and all warnings as errors
#    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Create target
add_executable(ut_bofstd
  ${SOURCES_FILES}
)
# Includes
target_include_directories(ut_bofstd
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/src>
)

### VLD ###########################################
set(CMAKE_VLD_TARGET ut_bofstd)
configure_file(
  ${CMAKE_SOURCE_DIR}/vld.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/vld.cmake
  @ONLY
)
include(${CMAKE_CURRENT_BINARY_DIR}/vld.cmake)
### VLD ########################################

target_link_libraries(ut_bofstd
  PRIVATE
	ONBINGS::bofstd
	#vcpkg GTest::GTest
	GTest::gtest_main
)
          
# Object library is not a shared library
# void import directive only for linking
target_compile_definitions(ut_bofstd
  PRIVATE
    -DBOFSTD_STATIC_DEFINE=1
)

# Add test and allow them to run in //
gtest_discover_tests(ut_bofstd
  XML_OUTPUT_DIR     ${CMAKE_BINARY_DIR}/GTestReports/
  TEST_PREFIX        bofstd_
   DISCOVERY_TIMEOUT 120
   DISCOVERY_MODE    PRE_TEST
   WORKING_DIRECTORY $<TARGET_FILE_DIR:bofstd>
)

# If you plan on running your unit tests with Valgrind
# it might happen that you need to add 'suppressions'
# to silence known issues. valgrind accepts suppressions
# by taking a file (--log-file=...). The variable
# MEMORYCHECK_SUPPRESSIONS_FILE is not a CMake variable
# but can be used as a convenience to accumulate suppressions
# from all the unit tests. Consequently, if you configure
# valgrind to use that file and define this variable, you will
# have the expected result.
if(MEMORYCHECK_SUPPRESSIONS_FILE)
  file(READ   valgrind_suppression.txt           CONTENT)
  file(APPEND ${MEMORYCHECK_SUPPRESSIONS_FILE} ${CONTENT})
endif()

message("ut_bofstd: Create data dir " ${COMPILED_BINARIES_DIR} "/bin/data")
file(MAKE_DIRECTORY ${COMPILED_BINARIES_DIR}/bin/data)  #Ut under linux from vscode
foreach(Filename ${DATA_FILES})
  message("ut_bofstd: Copy " ${Filename} " to " ${COMPILED_BINARIES_DIR} "/bin/data")
  file(COPY ${Filename} DESTINATION ${COMPILED_BINARIES_DIR}/bin/data)
endforeach() 

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION  ${COMPILED_BINARIES_DIR}/bin FILES_MATCHING PATTERN "*")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION  ${CMAKE_INSTALL_BINDIR}      COMPONENT bofstd-runtime FILES_MATCHING PATTERN "*")
