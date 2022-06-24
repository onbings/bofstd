//Dumy file added to make compilation

#ifndef __ZMQ_PLATFORM_HPP_INCLUDED__
#define __ZMQ_PLATFORM_HPP_INCLUDED__


#if defined(ANDROID)
  //#define ZMQ_USE_KQUEUE            0
  #define ZMQ_USE_EPOLL             1
  //#define ZMQ_USE_DEVPOLL           0
  //#define ZMQ_USE_POLL              0
  //#define ZMQ_USE_SELECT            0
  //#define ZMQ_FORCE_MUTEXES         0
  #define HAVE_FORK                 1
  #define HAVE_CLOCK_GETTIME        1
  //#define HAVE_GETHRTIME            0
  #define ZMQ_HAVE_UIO              1
  #define ZMQ_HAVE_EVENTFD          1
  #define ZMQ_HAVE_IFADDRS          1
  #define ZMQ_HAVE_SO_PEERCRED      1
  //#define ZMQ_HAVE_LOCAL_PEERCRED   0
  //#define ZMQ_HAVE_SOCK_CLOEXEC     0
  #define ZMQ_HAVE_SO_KEEPALIVE     1
  #define ZMQ_HAVE_TCP_KEEPCNT      1
  #define ZMQ_HAVE_TCP_KEEPIDLE     1
  #define ZMQ_HAVE_TCP_KEEPINTVL    1
  //#define ZMQ_HAVE_TCP_KEEPALIVE    0
  //#define ZMQ_MAKE_VALGRIND_HAPPY   0
  //#define ZMQ_HAVE_WINDOWS          0
	
  //set (HAVE_EPOLL ON)
  //set (POLLER "epoll")
#elif defined(__linux__) // || defined(DESKTOP_LINUX32) || defined(DESKTOP_LINUX64) || defined(DESKTOP_CENT))
  //#define ZMQ_USE_KQUEUE            0
  #define ZMQ_USE_EPOLL             1
  //#define ZMQ_USE_DEVPOLL           0
  //#define ZMQ_USE_POLL              0
  //#define ZMQ_USE_SELECT            0
  //#define ZMQ_FORCE_MUTEXES         0
  #define HAVE_FORK                 1
  #define HAVE_CLOCK_GETTIME        1
  //#define HAVE_GETHRTIME            0
  #define ZMQ_HAVE_UIO              1
  #define ZMQ_HAVE_EVENTFD          1
  #define ZMQ_HAVE_IFADDRS          1
  #define ZMQ_HAVE_SO_PEERCRED      1
  //#define ZMQ_HAVE_LOCAL_PEERCRED   0
  #define ZMQ_HAVE_SOCK_CLOEXEC     1
  #define ZMQ_HAVE_SO_KEEPALIVE     1
  #define ZMQ_HAVE_TCP_KEEPCNT      1
  #define ZMQ_HAVE_TCP_KEEPIDLE     1
  #define ZMQ_HAVE_TCP_KEEPINTVL    1
  //#define ZMQ_HAVE_TCP_KEEPALIVE    0
  //#define ZMQ_MAKE_VALGRIND_HAPPY   0
  //#define ZMQ_HAVE_WINDOWS          0

  //set (HAVE_EPOLL ON)
  //set (POLLER "epoll")
#elif (defined(_WIN32) || defined(_WIN64))
  //#define ZMQ_USE_KQUEUE            0
  //#define ZMQ_USE_EPOLL             0
  //#define ZMQ_USE_DEVPOLL           0
  #define ZMQ_USE_POLL              1
  //#define ZMQ_USE_SELECT            0
  //#define ZMQ_FORCE_MUTEXES         0
  //#define HAVE_FORK                 0
  #define HAVE_CLOCK_GETTIME        1
  //#define HAVE_GETHRTIME            0
  //#define ZMQ_HAVE_UIO              0
  //#define ZMQ_HAVE_EVENTFD          0
  //#define ZMQ_HAVE_IFADDRS          0
  //#define ZMQ_HAVE_SO_PEERCRED      0
  //#define ZMQ_HAVE_LOCAL_PEERCRED   0
  //#define ZMQ_HAVE_SOCK_CLOEXEC     0
  #define ZMQ_HAVE_SO_KEEPALIVE     1
  #define ZMQ_HAVE_TCP_KEEPCNT      1
  #define ZMQ_HAVE_TCP_KEEPIDLE     1
  #define ZMQ_HAVE_TCP_KEEPINTVL    1
  //#define ZMQ_HAVE_TCP_KEEPALIVE    0
  //#define ZMQ_MAKE_VALGRIND_HAPPY   0
  #define ZMQ_HAVE_WINDOWS          1
  //set (HAVE_POLL ON)
  //set (POLLER "poll")
#endif

#endif
