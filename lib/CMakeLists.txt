
include(GNUInstallDirs)
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# =============
# == SOURCES ==
# =============

set(PUBLIC_INCLUDES_FILES
)

set(API_FILES
	bofversioninfo.h   #generated in binary build dir with configure_file
	include/bofstd/boferr.h
	include/bofstd/bofstd.h
	src/bofstd.cpp
)

set(BIT_FILES
	include/bofstd/bofbit.h
	src/bofbit.cpp
)

set(CALLBACK_FILES
	include/bofstd/bofcallbackcollection.h
	include/bofstd/bofasync.h
	src/bofasync.cpp
)

set(CLI_FILES
	include/bofstd/bofcommandlineparser.h
	src/bofcommandlineparser.cpp
	include/bofstd/bofgetopt.h
	src/bofgetopt.cpp	
)

set(COM_FILES
	include/bofstd/bofcomchannel.h
	src/bofcomchannel.cpp
	include/bofstd/bofuart.h
	src/bofuart.cpp
	include/bofstd/bofsocketos.h
	src/bofsocketos.cpp
	include/bofstd/bofsocket.h
	src/bofsocket.cpp
	include/bofstd/bofpipe.h
	src/bofpipe.cpp
)

set(CONIO_SHELL_FILES
	include/bofstd/bofconio.h
	src/bofconio.cpp	
	include/bofstd/bofshell.h
	src/bofshell.cpp	 
	third/cpp-linenoise-master/linenoise.hpp
)

set(CONTAINER_FILES
	include/bofstd/bofcircularbuffer.h
	include/bofstd/bofstringcircularbuffer.h
	src/bofstringcircularbuffer.cpp
	include/bofstd/bofrawcircularbuffer.h
	src/bofrawcircularbuffer.cpp
	include/bofstd/bofnarytreekv.h
	include/bofstd/bofpot.h
	include/bofstd/bofstack.h
	src/bofstack.cpp
	include/bofstd/boflist.h
	include/bofstd/bofqueue.h
	include/bofstd/bofstream.h
)

set(CRYPTO_FILES
	include/bofstd/bofcrypto.h
	src/bofcrypto.cpp
)

set(DAEMON_FILES
	include/bofstd/bofdaemonservice.h
	src/bofdaemonservice.cpp
)

set(DB_FILES
	include/bofstd/bofavlnode.h
	include/bofstd/bofavltree.h
	include/bofstd/boframdb.h
)

set(DELEGATE_FILES
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/bofmsgthread.h
	third/AsyncMulticastDelegate/src/bofmsgthread.cpp

	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Allocator.h
	third/AsyncMulticastDelegate/src/Allocator.cpp
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateAsync.h
	third/AsyncMulticastDelegate/src/DelegateAsync.cpp
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateInvoker.h
	third/AsyncMulticastDelegate/src/DelegateRemoteInvoker.cpp
	third/AsyncMulticastDelegate/src/MulticastDelegateBase.cpp
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/LockGuard.h
	third/AsyncMulticastDelegate/src/LockGuard.cpp
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/xallocator.h
	third/AsyncMulticastDelegate/src/xallocator.cpp
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Semaphore.h
	third/AsyncMulticastDelegate/src/Semaphore.cpp

	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Delegate.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateAsyncWait.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateLib.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateMsg.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateOpt.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateRemoteInvoker.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateRemoteRecv.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateRemoteSend.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateSp.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateSpAsync.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateThread.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateTransport.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegate.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegateBase.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegateSafe.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/SinglecastDelegate.h
	third/AsyncMulticastDelegate/include/asyncmulticastdelegate/ThreadMsg.h
)

set(ENUM_FLAG_FILES
	include/bofstd/bofflag.h
	include/bofstd/bofenum.h
)

set(FS_FILES
	include/bofstd/bofpath.h
	src/bofpath.cpp
	include/bofstd/bofuri.h
	src/bofuri.cpp
	include/bofstd/boffs.h
	src/boffs.cpp
)

set(IXML_FILES
	third/ixml/inc/ixml.h
	third/ixml/inc/ixmldebug.h
	third/ixml/src/attr.c
	third/ixml/src/document.c
	third/ixml/src/element.c
	third/ixml/src/ixml.c
	third/ixml/src/ixmldebug.c
	third/ixml/src/ixmlmembuf.c
	third/ixml/src/ixmlparser.c
	third/ixml/src/namedNodeMap.c
	third/ixml/src/node.c
	third/ixml/src/nodeList.c
	third/ixml/src/inc/ixmlmembuf.h
	third/ixml/src/inc/ixmlparser.h	
)

set(LOG_FILES
	include/bofstd/iboflogchannel.h
	src/iboflogchannel.cpp	
	include/bofstd/boflogchannel_spdlog.h
	src/boflogchannel_spdlog.cpp	
	include/bofstd/boflogsink_spdlog.h
	src/boflogsink_spdlog.cpp
	include/bofstd/boflogger.h
	src/boflogger.cpp	
)

set(OBSERVER_FILES
	include/bofstd/bofobserver.h
	src/bofobserver.cpp
)

set(PROTOCOL_FILES
	include/bofstd/bofrpc.h
	src/bofrpc.cpp
)

set(SERDER_FILES
	src/bofbinserializer.cpp
	include/bofstd/bofbinserializer.h
	include/bofstd/bofparameter.h
	src/bofparameter.cpp	
	include/bofstd/bofxmlparser.h
	src/bofxmlparser.cpp	
	include/bofstd/bofxmlwriter.h
	src/bofxmlwriter.cpp		
	include/bofstd/bofjsonparser.h
	src/bofjsonparser.cpp	
	include/bofstd/bofjsonwriter.h
	src/bofjsonwriter.cpp	
)

set(SOCKIO_FILES
	include/bofstd/bofsocketthread.h
	src/bofsocketthread.cpp
	include/bofstd/bofsocketserver.h
	src/bofsocketserver.cpp
	include/bofstd/bofsocketio.h
	src/bofsocketio.cpp
	include/bofstd/bofsocketsessionmanager.h
	src/bofsocketsessionmanager.cpp
	include/bofstd/ibofsocketio.h
	include/bofstd/ibofsocketsessionfactory.h
)

set(SYSTEM_FILES
	include/bofstd/bofsystem.h
	src/bofsystem.cpp
	include/bofstd/bofstring.h
	src/bofstring.cpp
	include/bofstd/bofscriptlauncher.h
	src/bofscriptlauncher.cpp
	include/bofstd/bofperformance.h
	src/bofperformance.cpp  
	include/bofstd/bofstatistics.h
	include/bofstd/boftimecode.h
	src/boftimecode.cpp
	include/bofstd/bofvideostandard.h
	src/bofvideostandard.cpp
	include/bofstd/bofaudiostandard.h
	src/bofaudiostandard.cpp
	include/bofstd/bofdatetime.h
	src/bofdatetime.cpp
	include/bofstd/bofrational.h
	src/bofrational.cpp
	include/bofstd/bofguid.h
	src/bofguid.cpp
)

set(THREADING_FILES
	include/bofstd/bofthread.h
	src/bofthread.cpp
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
	${DELEGATE_FILES}
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
source_group("Delegate"	 FILES ${DELEGATE_FILES})
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

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/include/bofstd/bofversioninfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/bofversioninfo.h @ONLY)

# ===========
# == BUILD ==
# ===========

if (MSVC)
    # warning level 4 and all warnings as errors
#    add_compile_options(/bigobj)
else()
    # lots of warnings and all warnings as errors
#    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# When creating shared library, we will want to control API visibility. This means, hide symbols that are
# not part of the API. This is great for the end user but not for us as we can no longer test our non API code.
#
# The solution is thus to create an intermediate step which is an object library.
# Object library compiles the files but does not link them.
# Later on, this object library will be linked to our target library
add_library(bofstd-objects OBJECT
  ${PUBLIC_INCLUDES_FILES}
  ${SOURCES_FILES}
)

# Alias with the namespaced version (consistency)
add_library(ONBINGS::bofstd-objects ALIAS bofstd-objects)

# Includes
target_include_directories(bofstd-objects
  PUBLIC
	#Header path used at build time
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/cpp-linenoise-master>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/AsyncMulticastDelegate/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/inc>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/src/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>		#for bofstd_export.h install (below) and bofversioninfo.h (above)
	#$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}
	#$<BUILD_INTERFACE:${FFMPEG_INCLUDE_DIRS}
	#Header path used when installed
	$<INSTALL_INTERFACE:include>
)

# Link
target_link_libraries(bofstd-objects
  PUBLIC
    #ONBINGS::onbings-another-lib

  PRIVATE
	date::date 
	date::date-tz
	jsoncpp::jsoncpp 
    spdlog::spdlog
#    Qt5::Widgets
#	mediainfo
#	yuv
#	asio::asio
#	czmq
#	libzmq
#	libzmq-static
#	czmq-static
#	ZLIB::ZLIB
#	zen
#	OpenSSL::SSL 
#	OpenSSL::Crypto
#	${Boost_LIBRARIES}
#	${FFMPEG_LIBRARIES}
)
#message("=Boost_INCLUDE_DIRS==============>"${Boost_INCLUDE_DIRS})
#message("=Boost_LIBRARIES==============>"${Boost_LIBRARIES})
#message("=FFMPEG_INCLUDE_DIRS==============>"${FFMPEG_INCLUDE_DIRS})
#message("=FFMPEG_LIBRARIES==============>"${FFMPEG_LIBRARIES})	
if(WIN32)
target_link_libraries(bofstd-objects
  PUBLIC
	Ws2_32
	Iphlpapi
	Winmm
	Rpcrt4

  PRIVATE
#    Qt5::Widgets
)
else()
  if( ANDROID )
  else()
    if(UNIX)
       target_link_libraries(bofstd-objects
         PRIVATE
	       rt
       )
    endif()
  endif()
endif()	

# Defines
#use external libfmt lib instead of the bundled one
#disable exception in spd log and replace thme by assert(false) to avoid try around every sprintf
target_compile_definitions(bofstd-objects
  PUBLIC
	#-DBOFSTD_BUILD_MEDIAINFO_AS_LIBRARY=1	#MediaInfo dll is a shame -> use only the static lib version

  PRIVATE
    -Dbofstd_EXPORTS=1	#in lower case to define the SYMBOL BOFSTD_EXPORT (upper case)in bofstd_export.h
	-DCRT_SECURE_NO_WARNINGS
	-DCRT_NONSTDC_NO_DEPRECATE
	-DWINSOCK_DEPRECATED_NO_WARNINGS
	-DIXML_STATIC_LIB
)

# Features (We need at least C++11 support)
target_compile_features(bofstd-objects
  PUBLIC
    cxx_std_14
)

# ====================
# == CREATE LIBRARY ==
# ====================

# Now create the "real" library
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Dummy.cpp)

# Make sure we have at least one file
add_library(bofstd
  ${CMAKE_CURRENT_BINARY_DIR}/Dummy.cpp
)

# Link with object library
target_link_libraries(bofstd
  PUBLIC
    ONBINGS::bofstd-objects
#PRIVATE 
#	fmt::fmt   # Or use the header-only version   target_link_libraries(main PRIVATE fmt::fmt-header-only)
)


# Create alias
add_library(ONBINGS::bofstd ALIAS bofstd)

# ================
# == VERSIONING ==
# ================

# Set version
if(BOFSTD_ENABLE_VERSIONING)
  # Under Linux this function will create the
  # library file with the full version name
  # and the proper symlinks.
  # e.g.
  #  libbofstd.so.1.0.1
  #  libbofstd.so.1 --> libbofstd.so.1.0.1
  set_target_properties(bofstd PROPERTIES
    VERSION   ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
  )
endif()

# ====================
# == API management ==
# ====================

include(GenerateExportHeader)

# This CMake function generates a header file (i.e. bofstd_export.h)
# that contains all the proper 'defines' to import/export symbols
# (e.g. BOFSTD_EXPORT) for shared libraries. When building as static
# those macros are simply blank. This file will be by default generated
# into ${CMAKE_CURRENT_BINARY_DIR} (see target_include_directories above)
generate_export_header(bofstd)

# ==================
# == INSTALLATION ==
# ==================

if(NOT APPLE)

  # Fix RPATH upon installation by replacing
  # the absolute RPATH generated by the compiler
  # by a keyword that basically says 'look for my
  # dependencies in the directory I'm currently
  # installed'.
  set_target_properties(bofstd PROPERTIES
    INSTALL_RPATH $ORIGIN
  )

endif()

# Install the target runtime
### message("====ANDROID_PLATFORM==LIB=2=================>" ${ANDROID_PLATFORM})
### if(NOT "${ANDROID_PLATFORM}")
### message("====ANDROID_PLATFORM==LIB=2=================>" ${ANDROID_PLATFORM})
### install(
###   TARGETS  bofstd bofstd-objects
###   EXPORT   bofstd
###   RUNTIME  DESTINATION        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}     # For binaries and dll
###            COMPONENT          bofstd-runtime
###   LIBRARY  DESTINATION        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}     # For shared libraries
###            COMPONENT          bofstd-runtime
###            NAMELINK_COMPONENT bofstd-devel
###   ARCHIVE  DESTINATION        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}     # For static libraries
###            COMPONENT          bofstd-devel
### )
### else()		   
install(
  TARGETS  bofstd bofstd-objects
  EXPORT   bofstd
  RUNTIME  DESTINATION        ${CMAKE_INSTALL_BINDIR}     # For binaries and dll
           COMPONENT          bofstd-runtime
  LIBRARY  DESTINATION        ${CMAKE_INSTALL_LIBDIR}     # For shared libraries
           COMPONENT          bofstd-runtime
           NAMELINK_COMPONENT bofstd-devel
  ARCHIVE  DESTINATION        ${CMAKE_INSTALL_LIBDIR}     # For static libraries
           COMPONENT          bofstd-devel
)
### endif()		   

# Install the public headers (don't forget the generated export directives)
install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT   bofstd-devel
)
install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/bofstd_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT   bofstd-devel
)
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/fmt-5.3.0/include/fmt/.		
# 		 DESTINATION include/fmt-5.3.0
#          COMPONENT   bofstd-devel
# )
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/spdlog-1.3.1/include/.			
#          DESTINATION include/spdlog-1.3.1		
#          COMPONENT   bofstd-devel
# )
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/vld/.	
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bofstd/vld
         COMPONENT   bofstd-devel
)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/AsyncMulticastDelegate/include/asyncmulticastdelegate/.	
         DESTINATION include/asyncmulticastdelegate
         COMPONENT   bofstd-devel
)
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/libyuv/include/.	
#          DESTINATION include/libyuv
#          COMPONENT   bofstd-devel
# )

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/html/. DESTINATION doc COMPONENT doc OPTIONAL)

#set_target_properties(bofstd PROPERTIES FOLDER Lib)

