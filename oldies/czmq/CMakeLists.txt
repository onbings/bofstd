# Copyright (c) 2014, Evs. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 		  CMakeLists.txt
# Author: 		Bernard HARMEL
# Revision:		1.0
# 
# Remarks: 
# None
# 
# History:		23 Dec 2016: Initial version

###############################################################################
cmake_minimum_required(VERSION 3.0.2)

if(NOT "$ENV{EVS_CMAKE_HELPERS}" STREQUAL "")
  list(APPEND CMAKE_MODULE_PATH "$ENV{EVS_CMAKE_HELPERS}")
endif()

include(Helpers  NO_POLICY_SCOPE)

Project(czmq VERSION 4.0.1)
include(dependencies.cmake)

#enable_language(C)
enable_language(ASM)
enable_testing()

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

# =========================================
# == DATA AND SETTINGS FROM THIS PROJECT ==
#==========================================
  
# File configuration
include(CheckIncludeFiles)
include(CheckLibraryExists)

set(HAVE_LIBZMQ 1)
set(HAVE_MEMSET 1)

if (${TARGET_OS} STREQUAL "WINDOWS")

set(HAVE_ARPA_INET_H      0)
set(HAVE_DLFCN_H          0)
set(HAVE_ERRNO_H          1)
set(HAVE_SIGNAL_H         1)
set(HAVE_IFADDRS_H        0)
set(HAVE_INTTYPES_H       1)
set(HAVE_LIMITS_H         1)
set(HAVE_LINUX_WIRELESS_H 0)
set(HAVE_MEMORY_H         1)
set(HAVE_NETINET_IN_H     0)
set(HAVE_NETINET_TCP_H    0)
set(HAVE_NET_IF_H         0)
set(HAVE_NET_IF_MEDIA_H   0)
set(HAVE_STDDEF_H         1)
set(HAVE_STDINT_H         1)
set(HAVE_STDLIB_H         1)
set(HAVE_STRING_H         1)
set(HAVE_STRINGS_H        0)
set(HAVE_SYS_SOCKET_H     0)
set(HAVE_SYS_STAT_H       1)
set(HAVE_SYS_TIME_H       0)
set(HAVE_SYS_TYPES_H      1)
set(HAVE_UNISTD_H         0)
set(HAVE_WINDOWS_H        1)
set(HAVE_LIBPTHREAD       0)
set(HAVE_LIBUUID          1)

else()

set(HAVE_ARPA_INET_H      1)
set(HAVE_DLFCN_H          1)
set(HAVE_ERRNO_H          1)
set(HAVE_SIGNAL_H         1)
set(HAVE_IFADDRS_H        1)
set(HAVE_INTTYPES_H       1)
set(HAVE_LIMITS_H         1)
set(HAVE_LINUX_WIRELESS_H 0)
set(HAVE_MEMORY_H         1)
set(HAVE_NETINET_IN_H     1)
set(HAVE_NETINET_TCP_H    0)
set(HAVE_NET_IF_H         1)
set(HAVE_NET_IF_MEDIA_H   0)
set(HAVE_STDDEF_H         1)
set(HAVE_STDINT_H         1)
set(HAVE_STDLIB_H         1)
set(HAVE_STRING_H         1)
set(HAVE_STRINGS_H        1)
set(HAVE_SYS_SOCKET_H     1)
set(HAVE_SYS_STAT_H       1)
set(HAVE_SYS_TIME_H       1)
set(HAVE_SYS_TYPES_H      1)
set(HAVE_UNISTD_H         1)
set(HAVE_WINDOWS_H        0)
set(HAVE_LIBPTHREAD       1)
set(HAVE_LIBUUID          0)

endif()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# Select flags
SET(CMAKE_C_FLAGS_RELEASE "-O3")

OPTION (ENABLE_DRAFTS "Build and install draft classes and methods" ON)
IF (ENABLE_DRAFTS)
    ADD_DEFINITIONS (-DCZMQ_BUILD_DRAFT_API)
ENDIF (ENABLE_DRAFTS)

########################################################################
# platform.h
########################################################################
include(CheckIncludeFile)
CHECK_INCLUDE_FILE("linux/wireless.h" HAVE_LINUX_WIRELESS_H)
CHECK_INCLUDE_FILE("net/if_media.h" HAVE_NET_IF_MEDIA_H)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("getifaddrs" HAVE_GETIFADDRS)
CHECK_FUNCTION_EXISTS("freeifaddrs" HAVE_FREEIFADDRS)

include(CheckIncludeFiles)
check_include_files("sys/socket.h;net/if.h" HAVE_NET_IF_H)
if (NOT HAVE_NET_IF_H)
    CHECK_INCLUDE_FILE("net/if.h" HAVE_NET_IF_H)
endif()

file(WRITE "${SOURCE_DIR}/src/platform.h.in" "
#cmakedefine HAVE_LINUX_WIRELESS_H
#cmakedefine HAVE_NET_IF_H
#cmakedefine HAVE_NET_IF_MEDIA_H
#cmakedefine HAVE_GETIFADDRS
#cmakedefine HAVE_FREEIFADDRS
")

configure_file("${SOURCE_DIR}/src/platform.h.in" "${SOURCE_DIR}/src/platform.h")

#The MSVC C compiler is too out of date,
#so the sources have to be compiled as c++
if (MSVC)
    enable_language(CXX)
    file(GLOB sources "${SOURCE_DIR}/src/*.c")
    set_source_files_properties(
        ${sources}
        PROPERTIES LANGUAGE CXX
    )
    set(MORE_LIBRARIES ws2_32 Rpcrt4 Iphlpapi)
endif()

# required libraries for mingw
if (MINGW)
    set(MORE_LIBRARIES -lws2_32 -lrpcrt4 -liphlpapi)
endif()

# required libraries for cygwin
if (CYGWIN)
    set(MORE_LIBRARIES)
endif()

list(APPEND CMAKE_MODULE_PATH "${SOURCE_DIR}")
set(OPTIONAL_LIBRARIES)

########################################################################
# LIBZMQ dependency
########################################################################

if (${TARGET_OS} STREQUAL "WINDOWS")
else()
##	list(APPEND OPTIONAL_LIBRARIES "stdc++")
##		list(APPEND OPTIONAL_LIBRARIES "m")
##		set (OPENPGM_LIBRARY_DIRS optimized "C:/PinguBuild/openpgm/MTPC_LINUX/VsGdb/cmake.Release/lib" debug "C:/PinguBuild/openpgm/MTPC_LINUX/VsGdb/cmake.Debug/lib")
##		set (OPENPGM_LIBRARIES   optimized openpgm.a    debug openpgm_d.a)
##		list(APPEND OPTIONAL_LIBRARIES ${OPENPGM_LIBRARIES})
##		link_directories(${OPENPGM_LIBRARY_DIRS})	
endif()
link_directories(${ZEROMQ_LIBRARIES_DIRS})
link_directories(${OPENPGM_LIBRARY_DIRS})
list(APPEND MORE_LIBRARIES ${ZEROMQ_LIBRARIES})
list(APPEND OPTIONAL_LIBRARIES ${OPENPGM_LIBRARIES})
#list(APPEND OPTIONAL_LIBRARIES ${ZEROMQ_LIBRARIES})
list(APPEND OPTIONAL_LIBRARIES ${STANDARD_LINK_LIBRARIES})
include_directories(${ZEROMQ_INCLUDE_DIRS})

add_definitions(-DHAVE_UUID)
if (${TARGET_OS} STREQUAL "WINDOWS")
else()
#    include_directories(${UUID_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES uuid)
    list(APPEND OPTIONAL_LIBRARIES uuid)
endif()

message("ZEROMQ_FOUND " ${ZEROMQ_FOUND})
message("ZEROMQ_LIBRARIES_DIRS " ${ZEROMQ_LIBRARIES_DIRS})
message("ZEROMQ_LIBRARIES " ${ZEROMQ_LIBRARIES})
message("ZEROMQ_INCLUDE_DIRS " ${ZEROMQ_INCLUDE_DIRS})
message("MORE_LIBRARIES " ${MORE_LIBRARIES})
message("OPTIONAL_LIBRARIES " ${OPTIONAL_LIBRARIES})

########################################################################
# SYSTEMD dependency
########################################################################
find_package(systemd)
IF (SYSTEMD_FOUND)
    include_directories(${SYSTEMD_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${SYSTEMD_LIBRARIES})
    set(pkg_config_libs_private "${pkg_config_libs_private} -lsystemd")
    add_definitions(-DHAVE_LIBSYSTEMD)
    list(APPEND OPTIONAL_LIBRARIES ${SYSTEMD_LIBRARIES})
ENDIF (SYSTEMD_FOUND)

########################################################################
# includes
########################################################################
set (czmq_headers
    include/czmq_library.h
    include/czmq_prelude.h
    include/czmq.h
    include/zactor.h
    include/zarmour.h
    include/zcert.h
    include/zcertstore.h
    include/zchunk.h
    include/zclock.h
    include/zconfig.h
    include/zdigest.h
    include/zdir.h
    include/zdir_patch.h
    include/zfile.h
    include/zframe.h
    include/zhash.h
    include/zhashx.h
    include/ziflist.h
    include/zlist.h
    include/zlistx.h
    include/zloop.h
    include/zmsg.h
    include/zpoller.h
    include/zsock.h
    include/zstr.h
    include/zuuid.h
    include/zauth.h
    include/zbeacon.h
    include/zgossip.h
    include/zmonitor.h
    include/zproxy.h
    include/zrex.h
    include/zsys.h
    src/zgossip_msg.h
    src/zsock_option.inc
    src/zgossip_engine.inc
    src/zhash_primes.inc
    src/foreign/sha1/sha1.inc_c
    src/foreign/sha1/sha1.h
    src/foreign/slre/slre.inc_c
    src/foreign/slre/slre.h
    src/foreign/slre/readme.txt
)

IF (ENABLE_DRAFTS)
    list(APPEND czmq_headers
        include/zproc.h
        include/ztimerset.h
        include/ztrie.h
    )
ENDIF (ENABLE_DRAFTS)

source_group ("Header Files" FILES ${czmq_headers})
install(FILES ${czmq_headers} DESTINATION include)

########################################################################
# library
########################################################################
include_directories("${SOURCE_DIR}/src" "${SOURCE_DIR}/include")
set (czmq_sources
    src/zactor.c
    src/zarmour.c
    src/zcert.c
    src/zcertstore.c
    src/zchunk.c
    src/zclock.c
    src/zconfig.c
    src/zdigest.c
    src/zdir.c
    src/zdir_patch.c
    src/zfile.c
    src/zframe.c
    src/zhash.c
    src/zhashx.c
    src/ziflist.c
    src/zlist.c
    src/zlistx.c
    src/zloop.c
    src/zmsg.c
    src/zpoller.c
    src/zsock.c
    src/zstr.c
    src/zuuid.c
    src/zauth.c
    src/zbeacon.c
    src/zgossip.c
    src/zmonitor.c
    src/zproxy.c
    src/zrex.c
    src/zsys.c
    src/zgossip_msg.c
)

IF (ENABLE_DRAFTS)
    list (APPEND czmq_sources
        src/zproc.c
        src/ztimerset.c
        src/ztrie.c
    )
ENDIF (ENABLE_DRAFTS)

add_library(czmq STATIC ${czmq_sources} ${czmq_headers})
add_definitions(-DZMQ_STATIC)
add_definitions(-DCZMQ_STATIC)
set_target_properties(czmq
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${SOURCE_DIR}/src"
)
target_link_libraries(czmq
    ${ZEROMQ_LIBRARIES} ${MORE_LIBRARIES}
)

install(TARGETS czmq
    LIBRARY DESTINATION "lib${LIB_SUFFIX}" # .so file
    ARCHIVE DESTINATION "lib${LIB_SUFFIX}" # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# executables
########################################################################
add_executable(
    zmakecert
    "${SOURCE_DIR}/src/zmakecert.c"
)
target_link_libraries(
    zmakecert
    czmq
    ${ZEROMQ_LIBRARIES}
    ${OPTIONAL_LIBRARIES}
)
set_target_properties(
    zmakecert
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${SOURCE_DIR}/src"
)
add_executable(
    czmq_selftest
    "${SOURCE_DIR}/src/czmq_selftest.c"
)
target_link_libraries(
    czmq_selftest
    czmq
    ${ZEROMQ_LIBRARIES}
    ${OPTIONAL_LIBRARIES}
)
set_target_properties(
    czmq_selftest
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${SOURCE_DIR}/src"
)

########################################################################
# tests
########################################################################
set(CLASSTEST_TIMEOUT 60 CACHE STRING "Timeout of the selftest of a class")
set(TOTAL_TIMEOUT 600 CACHE STRING "Timout of the total testsuite")

if(UNIX)
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-reachable=yes
                                     --error-exitcode=1
                                     --suppressions=src/.valgrind.supp")
endif()

set(TEST_CLASSES
    zactor
    zarmour
    zcert
    zcertstore
    zchunk
    zclock
    zconfig
    zdigest
    zdir
    zdir_patch
    zfile
    zframe
    zhash
    zhashx
    ziflist
    zlist
    zlistx
    zloop
    zmsg
    zpoller
    zsock
    zstr
    zuuid
    zauth
    zbeacon
    zgossip
    zmonitor
    zproxy
    zrex
    zsys
)

IF (ENABLE_DRAFTS)
    list (APPEND TEST_CLASSES
    zproc
    ztimerset
    ztrie
    )
ENDIF (ENABLE_DRAFTS)

foreach(TEST_CLASS ${TEST_CLASSES})
    add_test(
        NAME ${TEST_CLASS}
        COMMAND czmq_selftest --continue --verbose --test ${TEST_CLASS}
    )
    set_tests_properties(
        ${TEST_CLASS}
        PROPERTIES TIMEOUT ${CLASSTEST_TIMEOUT}
    )
endforeach(TEST_CLASS)

include(CTest)

########################################################################
# cleanup
########################################################################
add_custom_target (distclean @echo Cleaning for source distribution)

set(cmake_generated ${CMAKE_BINARY_DIR}/CMakeCache.txt
                    ${CMAKE_BINARY_DIR}/cmake_install.cmake
                    ${CMAKE_BINARY_DIR}/Makefile
                    ${CMAKE_BINARY_DIR}/CMakeFiles
                    ${CMAKE_BINARY_DIR}/CTestTestfile.cmake
                    ${CMAKE_BINARY_DIR}/DartConfiguration.tcl
                    ${CMAKE_BINARY_DIR}/Testing
                    ${CMAKE_BINARY_DIR}/compile_commands.json
                    ${CMAKE_BINARY_DIR}/src/platform.h
                    ${CMAKE_BINARY_DIR}/src/libczmq.pc
                    ${CMAKE_BINARY_DIR}/src/libczmq.so
                    ${CMAKE_BINARY_DIR}/src/czmq_selftest
                    ${CMAKE_BINARY_DIR}/src/zmakecert
                    ${CMAKE_BINARY_DIR}/src/czmq_selftest
)

add_custom_command(
    DEPENDS clean
    COMMENT "distribution clean"
    COMMAND rm
    ARGS    -rf CMakeTmp ${cmake_generated}
    TARGET  distclean
)
#-----------------------------------------------------------------------------
# installer
#will be installed in your local binaries repositories
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${PLATFORM_NAME}/${PROJECT_NAME}/${PROJECT_VERSION}${INSTALL_REV}") 
 
#Install CMake generated configuration files
#For each visual studio version, files are generated in separated sub-directory

install(EXPORT ${PROJECT_NAME} DESTINATION cmake${COMPILER_SUBDIR})
 
#Install custom CMake configuration file
install(FILES ${PROJECT_NAME}Config.cmake DESTINATION cmake)
 
#Install produced binaries
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION lib${COMPILER_SUBDIR})
 
 #Install pdb
InstallPdb(${PROJECT_NAME} lib${COMPILER_SUBDIR})

#Install header file
install (DIRECTORY "./include/."  DESTINATION include PATTERN ".svn" EXCLUDE)

########################################################################
# summary
########################################################################
message ("")
message (STATUS "******************* Configuration Summary *******************")
message (STATUS "General:")
message (STATUS "  Version           :   ${VERSION}")
message (STATUS "  System            :   ${CMAKE_SYSTEM_NAME}")
message (STATUS "  C compiler        :   ${CMAKE_C_COMPILER}")
message (STATUS "  Debug C flags     :   ${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
message (STATUS "  Release C flags   :   ${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
message (STATUS "  Build type        :   ${CMAKE_BUILD_TYPE}")
IF (ENABLE_DRAFTS)
message (STATUS "  Draft API         :   Yes")
ELSE (ENABLE_DRAFTS)
message (STATUS "  Draft API         :   No")
ENDIF (ENABLE_DRAFTS)
message (STATUS "")
message (STATUS "Dependencies:")
include(FeatureSummary)
feature_summary (WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
message (STATUS "")
message (STATUS "Install:")
message (STATUS "  Install prefix    :" "${CMAKE_INSTALL_PREFIX}")
message (STATUS "")
message (STATUS "************************* Options ***************************")
message (STATUS "Options:")
message (STATUS "  Use the Draft API (default = yes):")
message (STATUS "  -DENABLE-DRAFTS=[yes|no]")
message (STATUS "")
message (STATUS "*************************************************************")
message (STATUS "Configuration complete! Now procced with:")
message (STATUS "  'make'                compile the project")
message (STATUS "  'make test'           run the project's selftest")
message (STATUS "  'make install'        install the project to ${CMAKE_INSTALL_PREFIX}")
message (STATUS "")
message (STATUS "Further options are:")
message (STATUS "  'ctest -T memcheck'   run the project's selftest with")
message (STATUS "                        valgrind to check for memory leaks")
message (STATUS "")


