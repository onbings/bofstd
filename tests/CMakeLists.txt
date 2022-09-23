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
  ./data/colorbar_in_raw_32b_194x259.raw
  ./data/colorbar_jpg_with_bad_ext.png
  ./data/colorbar_jpg_with_no_ext
  ./data/colorbar_png_with_bad_ext.jpg
  ./data/colorbar_png_with_no_ext
  ./data/colorbar_uncompress_32b.tga
  ./data/colorbar_rle_32b.tga
)

set(API_FILES
	include/gtestrunner.h
	src/main.cpp
	src/ut_api.cpp
	src/ut_callbackcollection.cpp 
	src/ut_2d.cpp
)

set(BOFIO_FILES
	src/ut_bofio.cpp
)

set(SERDER_FILES
	include/ut_parser.h
	src/ut_cmdlineparser.cpp
	src/ut_xmlparserwriter.cpp
	data/xmlparser.xml
	src/ut_jsonparserwriter.cpp	
	data/jsonparser.json	
)

set(LOG_FILES
	src/ut_stringformatter.cpp
	src/ut_logger.cpp
)
set(FS_FILES
	src/ut_path.cpp
	src/ut_uri.cpp
	src/ut_fs.cpp
)

set(CRYPTO_FILES
	src/ut_crypto.cpp
)

set(CONTAINER_FILES
	src/ut_narytreekv.cpp
	src/ut_circularbuffer.cpp
	src/ut_rawcircularbuffer.cpp
	src/ut_stringcircularbuffer.cpp
	src/ut_pot.cpp
	src/ut_queue.cpp
)

set(COM_FILES
	src/ut_pipe.cpp
	src/ut_uart.cpp
	src/ut_socketos.cpp
	src/ut_sockettcp.cpp
	src/ut_socketudp.cpp
)

set(SYSTEM_FILES
        src/ut_scriptlauncher.cpp
        src/ut_system.cpp
        src/ut_string.cpp
        src/ut_boftimecode.cpp
)
set(BITENUMGUID_FILES
        src/ut_bit.cpp
        src/ut_enum.cpp
        src/ut_guid.cpp
)
set(THREADING_FILES
	src/ut_threading.cpp
	src/ut_asyncmulticastdelegate.cpp
)

set(DB_FILES
	include/ut_ramdb.h
	src/ut_ramdb.cpp
)


set(SOURCES_FILES
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

# For Visual Studio
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

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${SOURCES_FILES})

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
add_executable(bofstd-tests
  ${SOURCES_FILES}
)

# Includes
target_include_directories(bofstd-tests
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/src>
)

# Link
target_link_libraries(bofstd-tests
  PRIVATE
    ONBINGS::bofstd-objects
    GTest::GTest
)

# Object library is not a shared library
# void import directive only for linking
target_compile_definitions(bofstd-tests
  PRIVATE
    -DBOFSTD_STATIC_DEFINE=1
)



# Add test and allow them to run in //
gtest_discover_tests(bofstd-tests
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


message("Copy test files from " ${CMAKE_CURRENT_SOURCE_DIR} "/data to " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} "/data")
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
foreach(Filename ${DATA_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} "/data")
  file(COPY ${Filename} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
endforeach()