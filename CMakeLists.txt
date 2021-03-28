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
#if 0
set(BOFIO_FILES
	include/bofstd/bofsocketserver.h
	source/bofsocketserver.cpp
	include/bofstd/bofsocketio.h
	source/bofsocketio.cpp
	include/bofstd/bofsocketsessionmanager.h
	source/bofsocketsessionmanager.cpp
	include/bofstd/ibofsocketio.h
	include/bofstd/ibofsocketsessionfactory.h
)

set(PATTERN_FILES
	include/bofstd/bofflag.h
	include/bofstd/bofobserver.h
	source/bofobserver.cpp
)

set(SERDER_FILES
	include/bofstd/bofbinserializer.h
	include/bofstd/bofparameter.h
	source/bofparameter.cpp	
	include/bofstd/bofxmlparser.h
	source/bofxmlparser.cpp	
	include/bofstd/bofxmlwriter.h
	source/bofxmlwriter.cpp		
	include/bofstd/bofjsonparser.h
	source/bofjsonparser.cpp	
	include/bofstd/bofjsonwriter.h
	source/bofjsonwriter.cpp	
)

SET( JSONCPP_VERSION 1.8.0 )
SET( JSONCPP_USE_SECURE_MEMORY 0) # CACHE STRING "-D...=1 to use memory-wiping allocator for STL" )
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/third/jsoncpp-1.8.0/src/lib_json/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/third/jsoncpp-1.8.0/include/json/version.h @ONLY)
set(JSONCPP_FILES
	third/jsoncpp-1.8.0/src/lib_json/json_tool.h
	third/jsoncpp-1.8.0/src/lib_json/json_reader.cpp
	third/jsoncpp-1.8.0/src/lib_json/json_valueiterator.inl
	third/jsoncpp-1.8.0/src/lib_json/json_value.cpp
	third/jsoncpp-1.8.0/src/lib_json/json_writer.cpp
	third/jsoncpp-1.8.0/src/lib_json/version.h
)

set(ASYNCMULTICASTDELEGATE_FILES
		third/AsyncMulticastDelegate/source/Allocator.cpp
		third/AsyncMulticastDelegate/source/DelegateAsync.cpp
		third/AsyncMulticastDelegate/source/DelegateRemoteInvoker.cpp
		third/AsyncMulticastDelegate/source/MulticastDelegateBase.cpp
		third/AsyncMulticastDelegate/source/LockGuard.cpp
		third/AsyncMulticastDelegate/source/xallocator.cpp
		third/AsyncMulticastDelegate/source/bofmsgthread.cpp
		third/AsyncMulticastDelegate/source/Semaphore.cpp

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

set(YUV_FILES
		third/libyuv/source/compare.cc
		third/libyuv/source/compare_common.cc
		third/libyuv/source/compare_gcc.cc
		third/libyuv/source/compare_mmi.cc
		third/libyuv/source/compare_msa.cc
		third/libyuv/source/compare_neon.cc
		third/libyuv/source/compare_neon64.cc
		third/libyuv/source/compare_win.cc
		third/libyuv/source/convert.cc
		third/libyuv/source/convert_argb.cc
		third/libyuv/source/convert_from.cc
		third/libyuv/source/convert_from_argb.cc
		third/libyuv/source/convert_jpeg.cc
		third/libyuv/source/convert_to_argb.cc
		third/libyuv/source/convert_to_i420.cc
		third/libyuv/source/cpu_id.cc
		third/libyuv/source/mjpeg_decoder.cc
		third/libyuv/source/mjpeg_validate.cc
		third/libyuv/source/planar_functions.cc
		third/libyuv/source/rotate.cc
		third/libyuv/source/rotate_any.cc
		third/libyuv/source/rotate_argb.cc
		third/libyuv/source/rotate_common.cc
		third/libyuv/source/rotate_gcc.cc
		third/libyuv/source/rotate_mmi.cc
		third/libyuv/source/rotate_msa.cc
		third/libyuv/source/rotate_neon.cc
		third/libyuv/source/rotate_neon64.cc
		third/libyuv/source/rotate_win.cc
		third/libyuv/source/row_any.cc
		third/libyuv/source/row_common.cc
		third/libyuv/source/row_gcc.cc
		third/libyuv/source/row_mmi.cc
		third/libyuv/source/row_msa.cc
		third/libyuv/source/row_neon.cc
		third/libyuv/source/row_neon64.cc
		third/libyuv/source/row_win.cc
		third/libyuv/source/scale.cc
		third/libyuv/source/scale_any.cc
		third/libyuv/source/scale_argb.cc
		third/libyuv/source/scale_common.cc
		third/libyuv/source/scale_gcc.cc
		third/libyuv/source/scale_mmi.cc
		third/libyuv/source/scale_msa.cc
		third/libyuv/source/scale_neon.cc
		third/libyuv/source/scale_neon64.cc
		third/libyuv/source/scale_uv.cc
		third/libyuv/source/scale_win.cc
		third/libyuv/source/test.sh
		third/libyuv/source/video_common.cc

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
	source/bofcommandlineparser.cpp
	include/bofstd/bofgetopt.h
	source/bofgetopt.cpp	
)

#add_subdirectory(third/zeromq)
set (ZMQ_FILES
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

set(CZMQ_FILES
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
set(FMT_FILES
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

set(SPDLOG_FILES
	third/spdlog-1.3.1/include/spdlog/spdlog.h
)

set(LOG_FILES
	include/bofstd/iboflogchannel.h
	include/bofstd/boflogchannel_spdlog.h
	include/bofstd/boflogsink_spdlog.h
	source/iboflogchannel.cpp	
	source/boflogchannel_spdlog.cpp	
	source/boflogsink_spdlog.cpp
	include/bofstd/boflogger.h
	include/bofstd/bofstringformatter.h
	source/bofstringformatter.cpp
	source/boflogger.cpp	
)

set(FS_FILES
	include/bofstd/bofpath.h
	source/bofpath.cpp
	include/bofstd/boffs.h
	source/boffs.cpp
)

set(CRYPTO_FILES
	include/bofstd/bofcrypto.h
	source/bofcrypto.cpp
)

set(CONTAINER_FILES
	include/bofstd/bofcircularbuffer.h
	include/bofstd/bofstringcircularbuffer.h
	source/bofstringcircularbuffer.cpp
	include/bofstd/bofrawcircularbuffer.h
	source/bofrawcircularbuffer.cpp
	include/bofstd/bofpot.h
	include/bofstd/bofstack.h
	include/bofstd/boflist.h
	include/bofstd/bofqueue.h
	source/bofstack.cpp
)

set(COM_FILES
	include/bofstd/bofcomchannel.h
	source/bofcomchannel.cpp
	include/bofstd/bofuart.h
	source/bofuart.cpp
	include/bofstd/bofsocketos.h
	source/bofsocketos.cpp
	include/bofstd/bofsocket.h
	source/bofsocket.cpp
	include/bofstd/bofpipe.h
	source/bofpipe.cpp
)

set(PROTO_FILES
	include/bofstd/bofrpc.h
	source/bofrpc.cpp
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
	include/bofstd/bofrational.h

	source/bofsystem.cpp
	source/bofstring.cpp
	source/bofperformance.cpp  
	#source/bofstatistics.cpp
	source/bofscriptlauncher.cpp
	source/boftimecode.cpp
	source/bofasync.cpp
	source/bofvideostandard.cpp
	source/bofrational.cpp
)

set(BITENUMGUID_FILES
	include/bofstd/bofbit.h
	source/bofbit.cpp
	include/bofstd/bofenum.h
	include/bofstd/bofguid.h
	source/bofguid.cpp
)

set(THREADING_FILES
	include/bofstd/bofthread.h
	source/bofthread.cpp
)

set(CONIO_FILES
	include/bofstd/bofconio.h
	source/bofconio.cpp	
	third/cpp-linenoise-master/linenoise.hpp
)

set(DB_FILES
	include/bofstd/bofavlnode.h
	include/bofstd/bofavltree.h
	include/bofstd/boframdb.h
)

set(GRAPH_FILES
	include/bofstd/bof2d.h
	source/bof2d.cpp
)
#endif
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


