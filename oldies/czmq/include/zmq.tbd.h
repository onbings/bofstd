//BHA false zmq.h include file just to be able to compile czmq support file

#ifndef __ZMQ_H_INCLUDED__
#define __ZMQ_H_INCLUDED__

#if defined(_WIN32)
#include <winsock2.h>
#endif

#define ZMQ_DEFINED_STDINT 1
#include <stdint.h>

/*  Version macros for compile-time API version detection                     */
#define ZMQ_VERSION_MAJOR 4
#define ZMQ_VERSION_MINOR 2
#define ZMQ_VERSION_PATCH 0

#define ZMQ_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZMQ_VERSION \
    ZMQ_MAKE_VERSION(ZMQ_VERSION_MAJOR, ZMQ_VERSION_MINOR, ZMQ_VERSION_PATCH)

#define ZMQ_POLLIN 1
#define ZMQ_POLLOUT 2
#define ZMQ_POLLERR 4
#define ZMQ_POLLPRI 8

/*  Socket types.                                                             */
#define ZMQ_PAIR 0
#define ZMQ_PUB 1
#define ZMQ_SUB 2
#define ZMQ_REQ 3
#define ZMQ_REP 4
#define ZMQ_DEALER 5
#define ZMQ_ROUTER 6
#define ZMQ_PULL 7
#define ZMQ_PUSH 8
#define ZMQ_XPUB 9
#define ZMQ_XSUB 10
#define ZMQ_STREAM 11

/*  Send/recv options.                                                        */
#define ZMQ_DONTWAIT 1
#define ZMQ_SNDMORE 2

/*  Security mechanisms                                                       */
#define ZMQ_NULL 0
#define ZMQ_PLAIN 1
#define ZMQ_CURVE 2
#define ZMQ_GSSAPI 3

#define ZMQ_SUBSCRIBE 6
#define ZMQ_TYPE 16

typedef struct zmq_pollitem_t
{
    void *socket;
#if defined _WIN32
    SOCKET fd;
#else
    int fd;
#endif
    short events;
    short revents;
} zmq_pollitem_t;

typedef struct zmq_msg_t {
#if defined (__GNUC__) || defined ( __INTEL_COMPILER) || \
        (defined (__SUNPRO_C) && __SUNPRO_C >= 0x590) || \
        (defined (__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
    unsigned char _ [64] __attribute__ ((aligned (sizeof (void *))));
#elif defined (_MSC_VER) && (defined (_M_X64) || defined (_M_ARM64))
    __declspec (align (8)) unsigned char _ [64];
#elif defined (_MSC_VER) && (defined (_M_IX86) || defined (_M_ARM_ARMV7VE))
    __declspec (align (4)) unsigned char _ [64];
#else
    unsigned char _ [64];
#endif
} zmq_msg_t;
void* zmq_msg_data(zmq_msg_t* msg);
const char* zmq_msg_gets(zmq_msg_t* msg, const char* property);
#endif
