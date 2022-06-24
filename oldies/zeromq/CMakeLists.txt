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

Project(zeromq VERSION 4.2.0)


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
set (BLD_TARGET_PLATFORM  "DESKTOP_WIN64")
endif()

if(${BLD_TARGET_PLATFORM} STREQUAL "ANDROID")
	message("ANDROID")
  set(ZMQ_USE_KQUEUE            0)
  set(ZMQ_USE_EPOLL             1)
  set(ZMQ_USE_DEVPOLL           0)
  set(ZMQ_USE_POLL              0)
  set(ZMQ_USE_SELECT            0)
  set(ZMQ_FORCE_MUTEXES         0)
  set(HAVE_FORK                 1)
  set(HAVE_CLOCK_GETTIME        1)
  set(HAVE_GETHRTIME            0)
  set(ZMQ_HAVE_UIO              1)
  set(ZMQ_HAVE_EVENTFD          1)
  set(ZMQ_HAVE_IFADDRS          1)
  set(ZMQ_HAVE_SO_PEERCRED      1)
  set(ZMQ_HAVE_LOCAL_PEERCRED   0)
  set(ZMQ_HAVE_SOCK_CLOEXEC     0)
  set(ZMQ_HAVE_SO_KEEPALIVE     1)
  set(ZMQ_HAVE_TCP_KEEPCNT      1)
  set(ZMQ_HAVE_TCP_KEEPIDLE     1)
  set(ZMQ_HAVE_TCP_KEEPINTVL    1)
  set(ZMQ_HAVE_TCP_KEEPALIVE    0)
  set(ZMQ_MAKE_VALGRIND_HAPPY   0)
  set(ZMQ_HAVE_WINDOWS          0)
	
	set (HAVE_EPOLL ON)
  set (POLLER "epoll")
	
elseif (${BLD_TARGET_PLATFORM} STREQUAL "LINUX")
  message("LINUX")
  set(ZMQ_USE_KQUEUE            0)
  set(ZMQ_USE_EPOLL             1)
  set(ZMQ_USE_DEVPOLL           0)
  set(ZMQ_USE_POLL              0)
  set(ZMQ_USE_SELECT            0)
  set(ZMQ_FORCE_MUTEXES         0)
  set(HAVE_FORK                 1)
  set(HAVE_CLOCK_GETTIME        1)
  set(HAVE_GETHRTIME            0)
  set(ZMQ_HAVE_UIO              1)
  set(ZMQ_HAVE_EVENTFD          1)
  set(ZMQ_HAVE_IFADDRS          1)
  set(ZMQ_HAVE_SO_PEERCRED      1)
  set(ZMQ_HAVE_LOCAL_PEERCRED   0)
  set(ZMQ_HAVE_SOCK_CLOEXEC     1)
  set(ZMQ_HAVE_SO_KEEPALIVE     1)
  set(ZMQ_HAVE_TCP_KEEPCNT      1)
  set(ZMQ_HAVE_TCP_KEEPIDLE     1)
  set(ZMQ_HAVE_TCP_KEEPINTVL    1)
  set(ZMQ_HAVE_TCP_KEEPALIVE    0)
  set(ZMQ_MAKE_VALGRIND_HAPPY   0)
  set(ZMQ_HAVE_WINDOWS          0)

	set (HAVE_EPOLL ON)
  set (POLLER "epoll")
	
elseif (${BLD_TARGET_PLATFORM} STREQUAL "DESKTOP_WIN64")
  message("WINDOWS")
  set(ZMQ_USE_KQUEUE            0)
  set(ZMQ_USE_EPOLL             0)
  set(ZMQ_USE_DEVPOLL           0)
  set(ZMQ_USE_POLL              1)
  set(ZMQ_USE_SELECT            0)
  set(ZMQ_FORCE_MUTEXES         0)
  set(HAVE_FORK                 0)
  set(HAVE_CLOCK_GETTIME        1)
  set(HAVE_GETHRTIME            0)
  set(ZMQ_HAVE_UIO              0)
  set(ZMQ_HAVE_EVENTFD          0)
  set(ZMQ_HAVE_IFADDRS          0)
  set(ZMQ_HAVE_SO_PEERCRED      0)
  set(ZMQ_HAVE_LOCAL_PEERCRED   0)
  set(ZMQ_HAVE_SOCK_CLOEXEC     0)
  set(ZMQ_HAVE_SO_KEEPALIVE     1)
  set(ZMQ_HAVE_TCP_KEEPCNT      1)
  set(ZMQ_HAVE_TCP_KEEPIDLE     1)
  set(ZMQ_HAVE_TCP_KEEPINTVL    1)
  set(ZMQ_HAVE_TCP_KEEPALIVE    0)
  set(ZMQ_MAKE_VALGRIND_HAPPY   0)
  set(ZMQ_HAVE_WINDOWS          1)
  set (HAVE_POLL ON)
  set (POLLER "poll")
endif()

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=gnu11" COMPILER_SUPPORTS_C11)
if(COMPILER_SUPPORTS_C11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
endif()

# Will be used to add flags to pkg-config useful when apps want to statically link
set (pkg_config_libs_private "")

option (WITH_OPENPGM "Build with support for OpenPGM" OFF)

if (APPLE)
    option (ZMQ_BUILD_FRAMEWORK "Build as OS X framework" ON)
endif ()

# Select curve encryption library, defaults to tweetnacl
# To use libsodium instead, use --with-libsodium (must be installed)
# To disable curve, use --disable-curve

option (WITH_LIBSODIUM "Use libsodium instead of built-in tweetnacl" OFF)
option (ENABLE_CURVE "Enable CURVE security" ON)

if (NOT ENABLE_CURVE)
    message (STATUS "CURVE security is disabled")

elseif (WITH_LIBSODIUM)
    find_package (Sodium)
    if (SODIUM_FOUND)
        message (STATUS "Using libsodium for CURVE security")
        include_directories (${SODIUM_INCLUDE_DIRS})

        # On Solaris, libsodium depends on libssp
        if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
            target_link_libraries (${PROJECT_NAME} ssp)
        endif ()
        set (ZMQ_USE_LIBSODIUM 1)
        set (ZMQ_HAVE_CURVE 1)
        set (pkg_config_libs_private "${pkg_config_libs_private} -lsodium")
    else ()
        message (FATAL_ERROR
            "libsodium is not installed. Install it, then run CMake again")
    endif ()

else ()
    message (STATUS "Using tweetnacl for CURVE security")
    list (APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/src/tweetnacl.c)
    set (ZMQ_USE_TWEETNACL 1)
    set (ZMQ_HAVE_CURVE 1)
endif ()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

if (EXISTS "${SOURCE_DIR}/.git")
    OPTION (ENABLE_DRAFTS "Build and install draft classes and methods" ON)
else ()
    OPTION (ENABLE_DRAFTS "Build and install draft classes and methods" OFF)
endif ()

message("---BHA---> Force ENABLE_DRAFTS")
set (ENABLE_DRAFTS ON)

IF (ENABLE_DRAFTS)
    ADD_DEFINITIONS (-DZMQ_BUILD_DRAFT_API)
    set (pkg_config_defines "-DZMQ_BUILD_DRAFT_API=1")
ELSE (ENABLE_DRAFTS)
    set (pkg_config_defines "")
ENDIF (ENABLE_DRAFTS)

option (WITH_MILITANT "Enable militant assertions" OFF)
if (WITH_MILITANT)
    add_definitions(-DZMQ_ACT_MILITANT)
endif()

set (POLLER "" CACHE STRING "Choose polling system. valid values are
                            kqueue, epoll, devpoll, pollset, poll or select [default=autodetect]")

include (CheckFunctionExists)
include (CheckTypeSize)

message ("POLLER START-->" ${POLLER})

if (POLLER STREQUAL "")
    set (CMAKE_REQUIRED_INCLUDES sys/event.h)
    check_function_exists (kqueue HAVE_KQUEUE)
    set (CMAKE_REQUIRED_INCLUDES)
    if (HAVE_KQUEUE)
        set (POLLER "kqueue")
    endif()
endif ()

if (POLLER STREQUAL "")
    set (CMAKE_REQUIRED_INCLUDES sys/epoll.h)
    check_function_exists (epoll_create HAVE_EPOLL)
    set (CMAKE_REQUIRED_INCLUDES)
    if (HAVE_EPOLL)
        set (POLLER "epoll")
    endif ()
endif ()

if (POLLER STREQUAL "")
    set (CMAKE_REQUIRED_INCLUDES sys/devpoll.h)
    check_type_size ("struct pollfd" DEVPOLL)
    set (CMAKE_REQUIRED_INCLUDES)
    if (HAVE_DEVPOLL)
        set (POLLER "devpoll")
    endif ()
endif ()

if (POLLER STREQUAL "")
    set (CMAKE_REQUIRED_INCLUDES sys/pollset.h)
    check_function_exists (pollset_create HAVE_POLLSET)
    set (CMAKE_REQUIRED_INCLUDES)
    if (HAVE_POLLSET)
        set (POLLER "pollset")
    endif()
endif ()

if (POLLER STREQUAL "")
    set (CMAKE_REQUIRED_INCLUDES poll.h)
    check_function_exists (poll HAVE_POLL)
    set (CMAKE_REQUIRED_INCLUDES)
    if (HAVE_POLL)
        set (POLLER "poll")
    endif ()
endif ()

message ("POLLER END-->" ${POLLER})

if (POLLER STREQUAL "")
     message ("fallback POLLER")
    if (WIN32)
        set (CMAKE_REQUIRED_INCLUDES winsock2.h)
        set (HAVE_SELECT 1)
    else ()
        set (CMAKE_REQUIRED_INCLUDES sys/select.h)
        check_function_exists (select HAVE_SELECT)
        set (CMAKE_REQUIRED_INCLUDES)
    endif ()
    if (HAVE_SELECT)
        set (POLLER "select")
    else ()
        message (FATAL_ERROR
            "Could not autodetect polling method")
    endif ()
endif ()

if (POLLER STREQUAL "kqueue"
 OR POLLER STREQUAL "epoll"
 OR POLLER STREQUAL "devpoll"
 OR POLLER STREQUAL "pollset"
 OR POLLER STREQUAL "poll"
 OR POLLER STREQUAL "select")
    message (STATUS "Detected ${POLLER} polling method")
    string (TOUPPER ${POLLER} UPPER_POLLER)
    set (ZMQ_USE_${UPPER_POLLER} 1)
else ()
    message (FATAL_ERROR "Invalid polling method")
endif ()

set (ZMQ_CMAKE_MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/Modules)
list (APPEND CMAKE_MODULE_PATH ${ZMQ_CMAKE_MODULES_DIR})

include (TestZMQVersion)
include (ZMQSourceRunChecks)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckCCompilerFlag)
include (CheckCXXCompilerFlag)
include (CheckCSourceCompiles)
include (CheckCSourceRuns)
include (CMakeDependentOption)
include (CheckCXXSymbolExists)

check_include_files (ifaddrs.h ZMQ_HAVE_IFADDRS)
check_include_files (windows.h ZMQ_HAVE_WINDOWS)
check_include_files (sys/uio.h ZMQ_HAVE_UIO)
check_include_files (sys/eventfd.h ZMQ_HAVE_EVENTFD)

check_library_exists (ws2_32 fopen "" HAVE_WS2_32) # TODO: Why doesn't something logical like WSAStartup work?
check_library_exists (ws2 fopen "" HAVE_WS2)
check_library_exists (rpcrt4 fopen "" HAVE_RPCRT4) # UuidCreateSequential
check_library_exists (iphlpapi fopen "" HAVE_IPHLAPI) # GetAdaptersAddresses

check_cxx_symbol_exists (SO_PEERCRED sys/socket.h ZMQ_HAVE_SO_PEERCRED)
check_cxx_symbol_exists (LOCAL_PEERCRED sys/socket.h ZMQ_HAVE_LOCAL_PEERCRED)

find_library (RT_LIBRARY rt)

find_package (Threads)

if (WIN32 AND NOT CYGWIN)
  if (NOT HAVE_WS2_32 AND NOT HAVE_WS2)
    message (FATAL_ERROR "Cannot link to ws2_32 or ws2")
  endif ()

  if (NOT HAVE_RPCRT4)
    message (FATAL_ERROR "Cannot link to rpcrt4")
  endif ()

  if (NOT HAVE_IPHLAPI)
    message (FATAL_ERROR "Cannot link to iphlapi")
  endif ()
endif ()

set (CMAKE_REQUIRED_LIBRARIES rt)
check_function_exists (clock_gettime HAVE_CLOCK_GETTIME)
set (CMAKE_REQUIRED_LIBRARIES)

set (CMAKE_REQUIRED_INCLUDES unistd.h)
check_function_exists (fork HAVE_FORK)
set (CMAKE_REQUIRED_INCLUDES)

set (CMAKE_REQUIRED_INCLUDES sys/time.h)
check_function_exists (gethrtime HAVE_GETHRTIME)
set (CMAKE_REQUIRED_INCLUDES)

set (CMAKE_REQUIRED_INCLUDES stdlib.h)
check_function_exists (mkdtemp HAVE_MKDTEMP)
set (CMAKE_REQUIRED_INCLUDES)

add_definitions (-D_REENTRANT -D_THREAD_SAFE)
add_definitions (-DZMQ_CUSTOM_PLATFORM_HPP)

option (ENABLE_EVENTFD "Enable/disable eventfd" ZMQ_HAVE_EVENTFD)

macro (zmq_check_cxx_flag_prepend flag)
  check_cxx_compiler_flag ("${flag}" HAVE_FLAG_${flag})

  if (HAVE_FLAG_${flag})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif ()
endmacro ()


if (MSVC)
  zmq_check_cxx_flag_prepend ("/W3")

  if (MSVC_IDE)
    set (MSVC_TOOLSET "-${CMAKE_VS_PLATFORM_TOOLSET}")
  else ()
    set (MSVC_TOOLSET "")
  endif ()
else ()
  zmq_check_cxx_flag_prepend ("-Wall")
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  zmq_check_cxx_flag_prepend ("-Wextra")
endif ()

#   TODO: why is -Wno-long-long defined differently than in configure.ac?
zmq_check_cxx_flag_prepend ("-Wno-long-long")
zmq_check_cxx_flag_prepend ("-Wno-uninitialized")

option (LIBZMQ_PEDANTIC "" ON)
option (LIBZMQ_WERROR "" OFF)

if (LIBZMQ_PEDANTIC)
  zmq_check_cxx_flag_prepend ("-pedantic")

  if (${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    zmq_check_cxx_flag_prepend ("-strict-ansi")
  endif ()

  if (${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")
    zmq_check_cxx_flag_prepend ("-compat=5")
  endif ()
endif ()

if (LIBZMQ_WERROR)
  zmq_check_cxx_flag_prepend ("-Werror")
  zmq_check_cxx_flag_prepend ("-errwarn=%all")
endif ()


if (CMAKE_SYSTEM_PROCESSOR MATCHES "^sparc")
  zmq_check_cxx_flag_prepend ("-mcpu=v9")
endif ()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")
  zmq_check_cxx_flag_prepend ("-features=zla")
endif ()


if (CMAKE_SYSTEM_NAME MATCHES "SunOS" OR CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  message (STATUS "Checking whether atomic operations can be used")
  check_c_source_compiles (
  "
   #include <atomic.h>

    int main ()
    {
      uint32_t value;
      atomic_cas_32 (&value, 0, 0);
      return 0;
    }
    "
    HAVE_ATOMIC_H)

  if (NOT HAVE_ATOMIC_H)
    set (ZMQ_FORCE_MUTEXES 1)
  endif ()
endif ()


#-----------------------------------------------------------------------------
zmq_check_sock_cloexec ()
zmq_check_so_keepalive ()
zmq_check_tcp_keepcnt ()
zmq_check_tcp_keepidle ()
zmq_check_tcp_keepintvl ()
zmq_check_tcp_keepalive ()
message("---BHA---> Clear ZMQ_HAVE_TIPC")
set (ZMQ_HAVE_TIPC			0)
zmq_check_tcp_tipc ()
message("---BHA---> ZMQ_HAVE_TIPC=" ${ZMQ_HAVE_TIPC})



if (    CMAKE_SYSTEM_NAME MATCHES "Linux"
    OR CMAKE_SYSTEM_NAME MATCHES "GNU/kFreeBSD"
    OR CMAKE_SYSTEM_NAME MATCHES "GNU/Hurd"
    OR CYGWIN)
  add_definitions (-D_GNU_SOURCE)
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  add_definitions (-D__BSD_VISIBLE)
elseif (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  add_definitions (-D_NETBSD_SOURCE)
elseif (CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
  add_definitions (-D_OPENBSD_SOURCE)
elseif (CMAKE_SYSTEM_NAME MATCHES "SunOS")
  add_definitions (-D_PTHREADS)
elseif (CMAKE_SYSTEM_NAME MATCHES "HP-UX")
  add_definitions (-D_POSIX_C_SOURCE=200112L)
  zmq_check_cxx_flag_prepend (-Ae)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions (-D_DARWIN_C_SOURCE)
endif ()

set (CMAKE_PYTHON_VERSION 2.7 2.6 2.5 2.4)
find_package (PythonInterp)
find_package (AsciiDoc)

cmake_dependent_option (WITH_DOC "Build Reference Guide documentation (requires DocBook)" ON
                       "PYTHONINTERP_FOUND;ASCIIDOC_FOUND" OFF)

if (WITH_OPENPGM)
	add_definitions(-DZMQ_HAVE_OPENPGM)
	set(OPTIONAL_LIBRARIES ${OPENPGM_LIBRARIES})
	include_directories(${OPENPGM_INCLUDE_DIRS})
	link_directories(${OPENPGM_LIBRARY_DIRS})
endif ()

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)

#-----------------------------------------------------------------------------
# platform specifics

if (WIN32)
    #   Socket limit is 16K (can be raised arbitrarily)
    add_definitions (-DFD_SETSIZE=16384)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif ()

if (MSVC)
  # Parallel make.
  ##set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  # Optimization flags.
  # http://msdn.microsoft.com/en-us/magazine/cc301698.aspx
  if (NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
    set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
  endif ()
endif ()


#-----------------------------------------------------------------------------
# source files

set (cxx-sources
        address.cpp
        client.cpp
        clock.cpp
        ctx.cpp
        curve_client.cpp
        curve_server.cpp
        dealer.cpp
        devpoll.cpp
        dgram.cpp
        dist.cpp
        epoll.cpp
        err.cpp
        fq.cpp
        io_object.cpp
        io_thread.cpp
        ip.cpp
        ipc_address.cpp
        ipc_connecter.cpp
        ipc_listener.cpp
        kqueue.cpp
        lb.cpp
        mailbox.cpp
        mailbox_safe.cpp
        mechanism.cpp
        metadata.cpp
        msg.cpp
        mtrie.cpp
        object.cpp
        options.cpp
        own.cpp
        null_mechanism.cpp
        pair.cpp
        pgm_receiver.cpp
        pgm_sender.cpp
        pgm_socket.cpp
        pipe.cpp
        plain_client.cpp
        plain_server.cpp
        poll.cpp
        poller_base.cpp
        pollset.cpp
        precompiled.cpp
        proxy.cpp
        pub.cpp
        pull.cpp
        push.cpp
        random.cpp
        raw_encoder.cpp
        raw_decoder.cpp
        reaper.cpp
        rep.cpp
        req.cpp
        router.cpp
        select.cpp
        server.cpp
        session_base.cpp
        signaler.cpp
        socket_base.cpp
        socks.cpp
        socks_connecter.cpp
        stream.cpp
        stream_engine.cpp
        sub.cpp
        tcp.cpp
        tcp_address.cpp
        tcp_connecter.cpp
        tcp_listener.cpp
        thread.cpp
        trie.cpp
        v1_decoder.cpp
        v1_encoder.cpp
        v2_decoder.cpp
        v2_encoder.cpp
        xpub.cpp
        xsub.cpp
        zmq.cpp
        zmq_utils.cpp
        decoder_allocators.cpp
        socket_poller.cpp
        timers.cpp
        config.hpp
        radio.cpp
        dish.cpp
        udp_engine.cpp
        udp_address.cpp
        scatter.cpp
        gather.cpp)

set (rc-sources version.rc)

include_directories (include ${CMAKE_CURRENT_BINARY_DIR})
set (public_headers include/zmq.h
                    include/zmq_utils.h)

set (readme-docs AUTHORS
                COPYING
                COPYING.LESSER
                NEWS)

#-----------------------------------------------------------------------------
# optional modules
if (ZMQ_HAVE_TIPC)
    add_definitions (-DZMQ_HAVE_TIPC)
    list (APPEND cxx-sources tipc_address.cpp tipc_connecter.cpp tipc_listener.cpp)
endif (ZMQ_HAVE_TIPC)

#-----------------------------------------------------------------------------
# source generators

foreach (source ${cxx-sources})
  list (APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/src/${source})
endforeach ()

foreach (source ${rc-sources})
  list (APPEND sources ${CMAKE_CURRENT_BINARY_DIR}/${source})
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/${source}.in ${CMAKE_CURRENT_BINARY_DIR}/${source})
endforeach ()

#   Delete any src/platform.hpp left by configure
file (REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/src/platform.hpp)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/builds/cmake/platform.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/platform.hpp)
list (APPEND sources ${CMAKE_CURRENT_BINARY_DIR}/platform.hpp)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/libzmq.pc.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libzmq.pc @ONLY)
set (zmq-pkgconfig ${CMAKE_CURRENT_BINARY_DIR}/libzmq.pc)

#-----------------------------------------------------------------------------
# output

if (MSVC)
  add_library (${PROJECT_NAME} STATIC ${sources})
  set_target_properties (${PROJECT_NAME} PROPERTIES
#    PUBLIC_HEADER "${public_headers}"
    COMPILE_FLAGS "/DZMQ_STATIC"
    OUTPUT_NAME "${PROJECT_NAME}")
if (HAVE_WS2_32)
  target_link_libraries (${PROJECT_NAME} ws2_32)
elseif (HAVE_WS2)
  target_link_libraries (${PROJECT_NAME} ws2)
endif ()

if (HAVE_RPCRT4)
  target_link_libraries (${PROJECT_NAME} rpcrt4)
endif ()

if (HAVE_IPHLAPI)
  target_link_libraries (${PROJECT_NAME} iphlpapi)
endif ()
		
else ()
    add_library (${PROJECT_NAME} STATIC ${sources}) # ${public_headers} ${html-docs} ${readme-docs} ${zmq-pkgconfig})
    set_target_properties (${PROJECT_NAME} PROPERTIES
#      PUBLIC_HEADER "${public_headers}"
      COMPILE_DEFINITIONS "ZMQ_STATIC"
      OUTPUT_NAME "${PROJECT_NAME}"
      PREFIX "")
endif ()

target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

if (SODIUM_FOUND)
  target_link_libraries (${PROJECT_NAME} ${SODIUM_LIBRARIES})
endif ()


if (RT_LIBRARY)
  target_link_libraries (${PROJECT_NAME} ${RT_LIBRARY})
endif ()

set (perf-tools local_lat
               remote_lat
               local_thr
               remote_thr
               inproc_lat
               inproc_thr)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug") # Why?
option (WITH_PERF_TOOL "Build with perf-tools" ON)
else ()
option (WITH_PERF_TOOL "Build with perf-tools" OFF)
endif ()

if (WITH_PERF_TOOL)
  foreach (perf-tool ${perf-tools})
    add_executable (${perf-tool} perf/${perf-tool}.cpp)
    target_link_libraries (${perf-tool} ${PROJECT_NAME} ${OPTIONAL_LIBRARIES})
		add_definitions(-DZMQ_STATIC)

    if (RT_LIBRARY)
      target_link_libraries (${perf-tool} ${RT_LIBRARY})
    endif ()
  endforeach ()
endif ()

#-----------------------------------------------------------------------------
# tests

set (ZMQ_BUILD_TESTS OFF CACHE BOOL "Build the tests for ZeroMQ")

if (ZMQ_BUILD_TESTS)
  enable_testing () # Enable testing only works in root scope
  ADD_SUBDIRECTORY (tests)
endif ()

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
 

#Install header file
install (DIRECTORY "./include/."  DESTINATION include PATTERN ".svn" EXCLUDE)
