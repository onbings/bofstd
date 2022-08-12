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
	include/bofstd/bofdaemonservice.h
	src/bofdaemonservice.cpp
	include/bofstd/bofcallbackcollection.h
)

set(BOFIO_FILES
	include/bofstd/bofsocketserver.h
	src/bofsocketserver.cpp
	include/bofstd/bofsocketio.h
	src/bofsocketio.cpp
	include/bofstd/bofsocketsessionmanager.h
	src/bofsocketsessionmanager.cpp
	include/bofstd/ibofsocketio.h
	include/bofstd/ibofsocketsessionfactory.h
)

set(PATTERN_FILES
	include/bofstd/bofflag.h
	include/bofstd/bofobserver.h
	src/bofobserver.cpp
)

set(SERDER_FILES
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

set(ASYNCMULTICASTDELEGATE_FILES
		third/AsyncMulticastDelegate/src/Allocator.cpp
		third/AsyncMulticastDelegate/src/DelegateAsync.cpp
		third/AsyncMulticastDelegate/src/DelegateRemoteInvoker.cpp
		third/AsyncMulticastDelegate/src/MulticastDelegateBase.cpp
		third/AsyncMulticastDelegate/src/LockGuard.cpp
		third/AsyncMulticastDelegate/src/xallocator.cpp
		third/AsyncMulticastDelegate/src/bofmsgthread.cpp
		third/AsyncMulticastDelegate/src/Semaphore.cpp

		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Allocator.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Delegate.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateAsync.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateAsyncWait.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/DelegateInvoker.h
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
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/LockGuard.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegate.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegateBase.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/MulticastDelegateSafe.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/Semaphore.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/SinglecastDelegate.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/xallocator.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/ThreadMsg.h
		third/AsyncMulticastDelegate/include/asyncmulticastdelegate/bofmsgthread.h
		)

set(TBDYUV_FILES
		third/libyuv/src/compare.cc
		third/libyuv/src/compare_common.cc
		third/libyuv/src/compare_gcc.cc
		third/libyuv/src/compare_mmi.cc
		third/libyuv/src/compare_msa.cc
		third/libyuv/src/compare_neon.cc
		third/libyuv/src/compare_neon64.cc
		third/libyuv/src/compare_win.cc
		third/libyuv/src/convert.cc
		third/libyuv/src/convert_argb.cc
		third/libyuv/src/convert_from.cc
		third/libyuv/src/convert_from_argb.cc
		third/libyuv/src/convert_jpeg.cc
		third/libyuv/src/convert_to_argb.cc
		third/libyuv/src/convert_to_i420.cc
		third/libyuv/src/cpu_id.cc
		third/libyuv/src/mjpeg_decoder.cc
		third/libyuv/src/mjpeg_validate.cc
		third/libyuv/src/planar_functions.cc
		third/libyuv/src/rotate.cc
		third/libyuv/src/rotate_any.cc
		third/libyuv/src/rotate_argb.cc
		third/libyuv/src/rotate_common.cc
		third/libyuv/src/rotate_gcc.cc
		third/libyuv/src/rotate_mmi.cc
		third/libyuv/src/rotate_msa.cc
		third/libyuv/src/rotate_neon.cc
		third/libyuv/src/rotate_neon64.cc
		third/libyuv/src/rotate_win.cc
		third/libyuv/src/row_any.cc
		third/libyuv/src/row_common.cc
		third/libyuv/src/row_gcc.cc
		third/libyuv/src/row_mmi.cc
		third/libyuv/src/row_msa.cc
		third/libyuv/src/row_neon.cc
		third/libyuv/src/row_neon64.cc
		third/libyuv/src/row_win.cc
		third/libyuv/src/scale.cc
		third/libyuv/src/scale_any.cc
		third/libyuv/src/scale_argb.cc
		third/libyuv/src/scale_common.cc
		third/libyuv/src/scale_gcc.cc
		third/libyuv/src/scale_mmi.cc
		third/libyuv/src/scale_msa.cc
		third/libyuv/src/scale_neon.cc
		third/libyuv/src/scale_neon64.cc
		third/libyuv/src/scale_uv.cc
		third/libyuv/src/scale_win.cc
		third/libyuv/src/test.sh
		third/libyuv/src/video_common.cc

		third/libyuv/include/libyuv.h
		third/libyuv/include/libyuv/basic_types.h
		third/libyuv/include/libyuv/compare.h
		third/libyuv/include/libyuv/compare_row.h
		third/libyuv/include/libyuv/convert.h
		third/libyuv/include/libyuv/convert_argb.h
		third/libyuv/include/libyuv/convert_from.h
		third/libyuv/include/libyuv/convert_from_argb.h
		third/libyuv/include/libyuv/cpu_id.h
		third/libyuv/include/libyuv/macros_msa.h
		third/libyuv/include/libyuv/mjpeg_decoder.h
		third/libyuv/include/libyuv/planar_functions.h
		third/libyuv/include/libyuv/rotate.h
		third/libyuv/include/libyuv/rotate_argb.h
		third/libyuv/include/libyuv/rotate_row.h
		third/libyuv/include/libyuv/row.h
		third/libyuv/include/libyuv/scale.h
		third/libyuv/include/libyuv/scale_argb.h
		third/libyuv/include/libyuv/scale_row.h
		third/libyuv/include/libyuv/scale_uv.h
		third/libyuv/include/libyuv/version.h
		third/libyuv/include/libyuv/video_common.h
		)
set(COMMANDLINE_FILES
	include/bofstd/bofcommandlineparser.h
	src/bofcommandlineparser.cpp
	include/bofstd/bofgetopt.h
	src/bofgetopt.cpp	
)

#add_subdirectory(third/zeromq)
set (TBDZMQ_FILES
    third/zeromq/src/address.cpp
    third/zeromq/src/client.cpp
    third/zeromq/src/clock.cpp
    third/zeromq/src/ctx.cpp
    third/zeromq/src/curve_client.cpp
    third/zeromq/src/curve_server.cpp
    third/zeromq/src/dealer.cpp
    third/zeromq/src/devpoll.cpp
    third/zeromq/src/dgram.cpp
    third/zeromq/src/dist.cpp
    third/zeromq/src/epoll.cpp
    third/zeromq/src/err.cpp
    third/zeromq/src/fq.cpp
    third/zeromq/src/io_object.cpp
    third/zeromq/src/io_thread.cpp
    third/zeromq/src/ip.cpp
    third/zeromq/src/ipc_address.cpp
    third/zeromq/src/ipc_connecter.cpp
    third/zeromq/src/ipc_listener.cpp
    third/zeromq/src/kqueue.cpp
    third/zeromq/src/lb.cpp
    third/zeromq/src/mailbox.cpp
    third/zeromq/src/mailbox_safe.cpp
    third/zeromq/src/mechanism.cpp
    third/zeromq/src/metadata.cpp
    third/zeromq/src/msg.cpp
    third/zeromq/src/mtrie.cpp
    third/zeromq/src/object.cpp
    third/zeromq/src/options.cpp
    third/zeromq/src/own.cpp
    third/zeromq/src/null_mechanism.cpp
    third/zeromq/src/pair.cpp
    third/zeromq/src/pgm_receiver.cpp
    third/zeromq/src/pgm_sender.cpp
    third/zeromq/src/pgm_socket.cpp
    third/zeromq/src/pipe.cpp
    third/zeromq/src/plain_client.cpp
    third/zeromq/src/plain_server.cpp
    third/zeromq/src/poll.cpp
    third/zeromq/src/poller_base.cpp
    third/zeromq/src/pollset.cpp
    third/zeromq/src/precompiled.cpp
    third/zeromq/src/proxy.cpp
    third/zeromq/src/pub.cpp
    third/zeromq/src/pull.cpp
    third/zeromq/src/push.cpp
    third/zeromq/src/random.cpp
    third/zeromq/src/raw_encoder.cpp
    third/zeromq/src/raw_decoder.cpp
    third/zeromq/src/reaper.cpp
    third/zeromq/src/rep.cpp
    third/zeromq/src/req.cpp
    third/zeromq/src/router.cpp
    third/zeromq/src/select.cpp
    third/zeromq/src/server.cpp
    third/zeromq/src/session_base.cpp
    third/zeromq/src/signaler.cpp
    third/zeromq/src/socket_base.cpp
    third/zeromq/src/socks.cpp
    third/zeromq/src/socks_connecter.cpp
    third/zeromq/src/stream.cpp
    third/zeromq/src/stream_engine.cpp
    third/zeromq/src/sub.cpp
    third/zeromq/src/tcp.cpp
    third/zeromq/src/tcp_address.cpp
    third/zeromq/src/tcp_connecter.cpp
    third/zeromq/src/tcp_listener.cpp
    third/zeromq/src/thread.cpp
    third/zeromq/src/trie.cpp
    third/zeromq/src/v1_decoder.cpp
    third/zeromq/src/v1_encoder.cpp
    third/zeromq/src/v2_decoder.cpp
    third/zeromq/src/v2_encoder.cpp
    third/zeromq/src/xpub.cpp
    third/zeromq/src/xsub.cpp
    third/zeromq/src/zmq.cpp
    third/zeromq/src/zmq_utils.cpp
    third/zeromq/src/decoder_allocators.cpp
    third/zeromq/src/socket_poller.cpp
    third/zeromq/src/timers.cpp
    third/zeromq/src/config.hpp
    third/zeromq/src/radio.cpp
    third/zeromq/src/dish.cpp
    third/zeromq/src/udp_engine.cpp
    third/zeromq/src/udp_address.cpp
    third/zeromq/src/scatter.cpp
    third/zeromq/src/gather.cpp
)

set(TBDCZMQ_FILES
    third/czmq/include/czmq_library.h
    third/czmq/include/czmq_prelude.h
    third/czmq/include/czmq.h
    third/czmq/include/zactor.h
    third/czmq/include/zarmour.h
    third/czmq/include/zcert.h
    third/czmq/include/zcertstore.h
    third/czmq/include/zchunk.h
    third/czmq/include/zclock.h
    third/czmq/include/zconfig.h
    third/czmq/include/zdigest.h
    third/czmq/include/zdir.h
    third/czmq/include/zdir_patch.h
    third/czmq/include/zfile.h
    third/czmq/include/zframe.h
    third/czmq/include/zhash.h
    third/czmq/include/zhashx.h
    third/czmq/include/ziflist.h
    third/czmq/include/zlist.h
    third/czmq/include/zlistx.h
    third/czmq/include/zloop.h
    third/czmq/include/zmsg.h
    third/czmq/include/zpoller.h
    third/czmq/include/zsock.h
    third/czmq/include/zstr.h
    third/czmq/include/zuuid.h
    third/czmq/include/zauth.h
    third/czmq/include/zbeacon.h
    third/czmq/include/zgossip.h
    third/czmq/include/zmonitor.h
    third/czmq/include/zproxy.h
    third/czmq/include/zrex.h
    third/czmq/include/zsys.h
    third/czmq/src/zgossip_msg.h
    third/czmq/src/zsock_option.inc
    third/czmq/src/zgossip_engine.inc
    third/czmq/src/zhash_primes.inc
    third/czmq/src/foreign/sha1/sha1.inc_c
    third/czmq/src/foreign/sha1/sha1.h
    third/czmq/src/foreign/slre/slre.inc_c
    third/czmq/src/foreign/slre/slre.h
    third/czmq/src/foreign/slre/readme.txt

    third/czmq/src/zactor.c
    third/czmq/src/zarmour.c
    third/czmq/src/zcert.c
    third/czmq/src/zcertstore.c
    third/czmq/src/zchunk.c
    third/czmq/src/zclock.c
    third/czmq/src/zconfig.c
    third/czmq/src/zdigest.c
    third/czmq/src/zdir.c
    third/czmq/src/zdir_patch.c
    third/czmq/src/zfile.c
    third/czmq/src/zframe.c
    third/czmq/src/zhash.c
    third/czmq/src/zhashx.c
    third/czmq/src/ziflist.c
    third/czmq/src/zlist.c
    third/czmq/src/zlistx.c
    third/czmq/src/zloop.c
    third/czmq/src/zmsg.c
    third/czmq/src/zpoller.c
    third/czmq/src/zsock.c
    third/czmq/src/zstr.c
    third/czmq/src/zuuid.c
    third/czmq/src/zauth.c
    third/czmq/src/zbeacon.c
    third/czmq/src/zgossip.c
    third/czmq/src/zmonitor.c
    third/czmq/src/zproxy.c
    third/czmq/src/zrex.c
    third/czmq/src/zsys.c
    third/czmq/src/zgossip_msg.c
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
set(TBDFMT_FILES
	third/fmt-5.3.0/src/format.cc
	third/fmt-5.3.0/src/posix.cc
	third/fmt-5.3.0/include/fmt/chrono.h
	third/fmt-5.3.0/include/fmt/color.h
	third/fmt-5.3.0/include/fmt/core.h
	third/fmt-5.3.0/include/fmt/format.h
	third/fmt-5.3.0/include/fmt/format-inl.h
	third/fmt-5.3.0/include/fmt/locale.h
	third/fmt-5.3.0/include/fmt/ostream.h
	third/fmt-5.3.0/include/fmt/posix.h
	third/fmt-5.3.0/include/fmt/printf.h
	third/fmt-5.3.0/include/fmt/ranges.h
	third/fmt-5.3.0/include/fmt/time.h
)

set(TBDSPDLOG_FILES
	third/spdlog-1.3.1/include/spdlog/spdlog.h
)

set(LOG_FILES
	include/bofstd/iboflogchannel.h
	include/bofstd/boflogchannel_spdlog.h
	include/bofstd/boflogsink_spdlog.h
	src/iboflogchannel.cpp	
	src/boflogchannel_spdlog.cpp	
	src/boflogsink_spdlog.cpp
	include/bofstd/boflogger.h
	include/bofstd/bofstringformatter.h
	src/bofstringformatter.cpp
	src/boflogger.cpp	
)

set(FS_FILES
	include/bofstd/bofpath.h
	src/bofpath.cpp
	include/bofstd/boffs.h
	src/boffs.cpp
)

set(CRYPTO_FILES
	include/bofstd/bofcrypto.h
	src/bofcrypto.cpp
)

set(CONTAINER_FILES
	include/bofstd/bofcircularbuffer.h
	include/bofstd/bofstringcircularbuffer.h
	src/bofstringcircularbuffer.cpp
	include/bofstd/bofrawcircularbuffer.h
	src/bofrawcircularbuffer.cpp
	include/bofstd/bofpot.h
	include/bofstd/bofstack.h
	include/bofstd/boflist.h
	include/bofstd/bofqueue.h
	src/bofstack.cpp
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

set(PROTO_FILES
	include/bofstd/bofrpc.h
	src/bofrpc.cpp
)

set(SYSTEM_FILES
	include/bofstd/bofsystem.h
	include/bofstd/bofstring.h
	include/bofstd/bofscriptlauncher.h
	include/bofstd/bofperformance.h
	include/bofstd/bofstatistics.h
	include/bofstd/boftimecode.h
	include/bofstd/bofasync.h
	include/bofstd/bofvideostandard.h
	include/bofstd/bofaudiostandard.h
	include/bofstd/bofrational.h

	src/bofsystem.cpp
	src/bofstring.cpp
	src/bofperformance.cpp  
	#src/bofstatistics.cpp
	src/bofscriptlauncher.cpp
	src/boftimecode.cpp
	src/bofasync.cpp
	src/bofvideostandard.cpp
	src/bofaudiostandard.cpp
	src/bofrational.cpp
)

set(BITENUMGUID_FILES
	include/bofstd/bofbit.h
	src/bofbit.cpp
	include/bofstd/bofenum.h
	include/bofstd/bofguid.h
	src/bofguid.cpp
)

set(THREADING_FILES
	include/bofstd/bofthread.h
	src/bofthread.cpp
)

set(CONIO_FILES
	include/bofstd/bofconio.h
	src/bofconio.cpp	
	third/cpp-linenoise-master/linenoise.hpp
)

set(DB_FILES
	include/bofstd/bofavlnode.h
	include/bofstd/bofavltree.h
	include/bofstd/boframdb.h
)

set(GRAPH_FILES
	include/bofstd/bof2d.h
	src/bof2d.cpp
)

set(SOURCES_FILES
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
	${ASYNCMULTICASTDELEGATE_FILES}
	${YUV_FILES}
	${ZMQ_FILES}
	${CZMQ_FILES}
	${IXML_FILES}
)

# For Visual Studio

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/bofstd PREFIX "public" FILES ${PUBLIC_INCLUDES_FILES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src            PREFIX "src"    FILES ${SOURCES_FILES})

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
	# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/fmt-5.3.0/include>  
	# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/spdlog-1.3.1/include>  
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/cpp-linenoise-master>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/AsyncMulticastDelegate/include>
	# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/libyuv/include>
	# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/zeromq/include>
	# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/czmq/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/inc>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/ixml/src/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>		#for bofstd_export.h install (below) and bofversioninfo.h (above)
	
	#Header path used when installed
	$<INSTALL_INTERFACE:include>
#   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link
target_link_libraries(bofstd-objects
  PUBLIC
    #ONBINGS::onbings-another-lib

  PRIVATE
#    Qt5::Widgets
    jsoncpp::jsoncpp
    spdlog::spdlog
	libzmq-static
	czmq-static
	yuv
)

	
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
endif()	

# Defines
#use external libfmt lib instead of the bundled one
#disable exception in spd log and replace thme by assert(false) to avoid try around every sprintf
target_compile_definitions(bofstd-objects
  PUBLIC
	# $<BUILD_INTERFACE:SPDLOG_FMT_EXTERNAL>		#can also be set in tweakme.h
	# $<BUILD_INTERFACE:SPDLOG_USE_STD_FORMAT>
	# $<BUILD_INTERFACE:SPDLOG_ENABLE_MESSAGE_COUNTER>	#can also be set in tweakme.h
	# $<BUILD_INTERFACE:FMT_EXCEPTIONS>  
	# $<BUILD_INTERFACE:ZMQ_DEFINED_STDINT>  
	# $<BUILD_INTERFACE:ZMQ_STATIC>
	# $<BUILD_INTERFACE:CZMQ_STATIC>
	$<BUILD_INTERFACE:IXML_STATIC_LIB>
	$<BUILD_INTERFACE:_CRT_SECURE_NO_WARNINGS>
	$<BUILD_INTERFACE:_CRT_NONSTDC_NO_DEPRECATE>
	$<BUILD_INTERFACE:_WINSOCK_DEPRECATED_NO_WARNINGS>

	# $<INSTALL_INTERFACE:SPDLOG_ENABLE_MESSAGE_COUNTER>	#can also be set in tweakme.h

  PRIVATE
    # Make directives export
    -Dbofstd_EXPORTS=1	#in lower case to define the SYMBOL BOFSTD_EXPORT (upper case)in bofstd_export.h
)

# Features (We need at least C++11 support)
target_compile_features(bofstd-objects
  PUBLIC
    cxx_std_11
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

