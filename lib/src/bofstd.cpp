/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the boflogger class
 *
 * Name:        bosstd.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
/*
Republish a new version
- Change version in C:\pro\github\bofstd\CMakeLists.txt: project(bofstd VERSION 5.3.0.1)
- Change version in C:\pro\github\bofstd\vcpkg.json:     "version": "5.3.0.1",

Push all modified files to git and get git push ref:
get push ref dd2b5f2d947de02e2733f603299ffce4d6eff74b		used also during last phase with the .\update-port.py command at rhe end of the procedure

- Change REF and SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bofstd\portfile.cmake / C:\pro\evs-vcpkg-registry\ports\bofstd\portfile.cmake
  REF dd2b5f2d947de02e2733f603299ffce4d6eff74b
  SHA512 0

- Validate BofSrd and bofstd with the C:\pro\github\vcpkg-packaging-env project
  cd C:\bld\a
  del * /S /Q
  cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic
-DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic
-DVCPKG_OVERLAY_PORTS=C:\pro\evs-vcpkg-registry\ports\ C:\pro\vcpkg-packaging-env

  [DEBUG] Downloading https:\\github.com\onbings\bofstd\archive\dd2b5f2d947de02e2733f603299ffce4d6eff74b.tar.gz
  Error: Failed to download from mirror set:
  File does not have the expected hash:
             url : [ https:\\github.com\onbings\bofstd\archive\dd2b5f2d947de02e2733f603299ffce4d6eff74b.tar.gz ]
       File path : [ C:\pro\vcpkg\downloads\onbings-bofstd-dd2b5f2d947de02e2733f603299ffce4d6eff74b.tar.gz.20964.part ]
   Expected hash : [ 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 ]
     Actual hash : [ 92a91738360c047ee9c52108964c0f66040b776894afdcf720a16d6865d2488e26d8021659be8e22b076f76378e716f33022c4602b8a63c5f70dce631f3a0ae8 ]

  get the Actual hash  and put it back in SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bofstd\portfile.cmake

  cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic
-DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env Should be ok, compile it with  cmake --build .

  cd C:\pro\github\onbings-vcpkg-registry / cd c:\pro\evs-vcpkg-registry
  git pull
  python .\update-port.py bofstd 5.3.0.1 dd2b5f2d947de02e2733f603299ffce4d6eff74b
  git push
  git log
  commit message should be "Update bofstd to 5.3.0.1\dd2b5f2d947de02e2733f603299ffce4d6eff74b"

  get the new push git ref bf6851917ef574e02fb406c2f50d5956b7edd85d

  update baseline in your vcpkg-configuration of your project which want to use this new lib
  C:\pro\github\vcpkg-packaging-env\vcpkg-configuration.json   //bf6851917ef574e02fb406c2f50d5956b7edd85d
  C:\pro\vcpkg-packaging-env\vcpkg-configuration.json          //efddb77285b616de2e547d7c986d4d61646ffa13
  C:\pro\evs-muse\vcpkg-configuration.json                     //efddb77285b616de2e547d7c986d4d61646ffa13 and bf6851917ef574e02fb406c2f50d5956b7edd85d

  "baseline": "bf6851917ef574e02fb406c2f50d5956b7edd85d", / "baseline": "efddb77285b616de2e547d7c986d4d61646ffa13",

cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic C:\pro\github\vcpkg-packaging-env

cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic C:\pro\vcpkg-packaging-env
cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-windows-dynamic C:\pro\evs-muse

cmake -DCMAKE_TOOLCHAIN_FILE=/home/bha/pro/github/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-linux-dynamic -DVCPKG_OVERLAY_PORTS=/home/bha/pro/github/onbings-vcpkg-registry/ports/
/home/bha/pro/github/bofstd
*/
#include <bofstd/bofsocketos.h>
#include <bofstd/boffs.h>
#include <bofstd/bofsocket.h>
#include <bofversioninfo.h>
#include <locale.h>
#include <map>
 
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

#if defined(_WIN32)
#else
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <termios.h>
#include <unistd.h>
#endif

BOFSTD_EXPORT BOF::BOFSTDPARAM GL_BofStdParam_X;

BEGIN_BOF_NAMESPACE()

#if defined(_WIN32)
static BOOL WINAPI S_SignalHandler(DWORD _CtrlType_DW)
{
  BOOL Rts_B = false;
//For all  if (_CtrlType_DW == CTRL_C_EVENT || _CtrlType_DW == CTRL_BREAK_EVENT)
  {
    if (GL_BofStdParam_X.SignalHandler != nullptr)
    {
      Rts_B = GL_BofStdParam_X.SignalHandler(_CtrlType_DW);
    }
  }
  return Rts_B;
}
#else
static void S_SignalHandler(int _Signal_i)
{
  if (GL_BofStdParam_X.SignalHandler != nullptr)
  {
    GL_BofStdParam_X.SignalHandler(_Signal_i);
  }
}
#endif

static bool S_DefaultBofSignalHandler(uint32_t _Signal_U32)
{
  Bof_Shutdown();
  exit(_Signal_U32);
  return true;
  //return (Bof_Shutdown() == BOF_ERR_NO_ERROR);
}
#if defined(_WIN32)
DWORD S_ModeIn_DW = 0, S_ModeOut_DW = 0;
#else
#endif
static char S_pUnknownError_c[128];
static std::map<BOFERR, const char *> S_ErrorCodeCollection{
    {BOF_ERR_NO_ERROR, "BOF_ERR_NO_ERROR: No error"},
    {BOF_ERR_EPERM, "BOF_ERR_EPERM: Operation not permitted"},
    {BOF_ERR_ENOENT, "BOF_ERR_ENOENT: No such file or directory"},
    {BOF_ERR_ESRCH, "BOF_ERR_ESRCH: No such process"},
    {BOF_ERR_EINTR, "BOF_ERR_EINTR: Interrupted system call"},
    {BOF_ERR_EIO, "BOF_ERR_EIO: I/O error"},
    {BOF_ERR_ENXIO, "BOF_ERR_ENXIO: No such device or address"},
    {BOF_ERR_E2BIG, "BOF_ERR_E2BIG: Argument list too long"},
    {BOF_ERR_ENOEXEC, "BOF_ERR_ENOEXEC: Exec format error"},
    {BOF_ERR_EBADF, "Bad file number"},
    {BOF_ERR_EBADF, "BOF_ERR_EBADF: Bad file number"},
    {BOF_ERR_ECHILD, "BOF_ERR_ECHILD: No child processes"}, // 10
    {BOF_ERR_EAGAIN, "BOF_ERR_EAGAIN: Try again"},
    {BOF_ERR_ENOMEM, "BOF_ERR_ENOMEM: Out of memory"},
    {BOF_ERR_EACCES, "BOF_ERR_EACCES: Permission denied"},
    {BOF_ERR_EFAULT, "BOF_ERR_EFAULT: Bad address"},
    {BOF_ERR_ENOTBLK, "BOF_ERR_ENOTBLK: Block device required"},
    {BOF_ERR_EBUSY, "BOF_ERR_EBUSY: Device or resource busy"},
    {BOF_ERR_EEXIST, "BOF_ERR_EEXIST: File exists"},
    {BOF_ERR_EXDEV, "BOF_ERR_EXDEV: Cross-device link"},
    {BOF_ERR_ENODEV, "BOF_ERR_ENODEV: No such device"},
    {BOF_ERR_ENOTDIR, "BOF_ERR_ENOTDIR: Not a directory"}, // 20
    {BOF_ERR_EISDIR, "BOF_ERR_EISDIR: Is a directory"},
    {BOF_ERR_EINVAL, "BOF_ERR_EINVAL: Invalid argument"},
    {BOF_ERR_ENFILE, "BOF_ERR_ENFILE: File table overflow"},
    {BOF_ERR_EMFILE, "BOF_ERR_EMFILE: Too many open files"},
    {BOF_ERR_ENOTTY, "BOF_ERR_ENOTTY: Not a typewriter"},
    {BOF_ERR_ETXTBSY, "BOF_ERR_ETXTBSY: Text file busy"},
    {BOF_ERR_EFBIG, "BOF_ERR_EFBIG: File too large"},
    {BOF_ERR_ENOSPC, "BOF_ERR_ENOSPC: No space left on device"},
    {BOF_ERR_ESPIPE, "BOF_ERR_ESPIPE: Illegal seek"},
    {BOF_ERR_EROFS, "BOF_ERR_EROFS: Read-only file system"}, // 30
    {BOF_ERR_EMLINK, "BOF_ERR_EMLINK: Too many links"},
    {BOF_ERR_EPIPE, "BOF_ERR_EPIPE: Broken pipe"},
    {BOF_ERR_EDOM, "BOF_ERR_EDOM: Math argument out of domain of func"},
    {BOF_ERR_ERANGE, "BOF_ERR_ERANGE: Math result not representable"},   // 34
    {BOF_ERR_EDEADLK, "BOF_ERR_EDEADLK: Resource deadlock would occur"}, // 35
    {BOF_ERR_ENAMETOOLONG, "BOF_ERR_ENAMETOOLONG: File name too long"},
    {BOF_ERR_ENOLCK, "BOF_ERR_ENOLCK: No record locks available"},
    {BOF_ERR_ENOSYS, "BOF_ERR_ENOSYS: Invalid system call number"},
    {BOF_ERR_ENOTEMPTY, "BOF_ERR_ENOTEMPTY: Directory not empty"},
    {BOF_ERR_ELOOP, "BOF_ERR_ELOOP: Too many symbolic links encountered"}, // 40
    {BOF_ERR_ENOMSG, "BOF_ERR_ENOMSG: No message of desired type"},
    {BOF_ERR_EIDRM, "BOF_ERR_EIDRM: Identifier removed"},
    {BOF_ERR_ECHRNG, "BOF_ERR_ECHRNG: Channel number out of range"},
    {BOF_ERR_EL2NSYNC, "BOF_ERR_EL2NSYNC: Level 2 not synchronized"},
    {BOF_ERR_EL3HLT, "BOF_ERR_EL3HLT: Level 3 halted"},
    {BOF_ERR_EL3RST, "BOF_ERR_EL3RST: Level 3 reset"},
    {BOF_ERR_ELNRNG, "BOF_ERR_ELNRNG: Link number out of range"},
    {BOF_ERR_EUNATCH, "BOF_ERR_EUNATCH: Protocol driver not attached"},
    {BOF_ERR_ENOCSI, "BOF_ERR_ENOCSI: No CSI structure available"}, // 50
    {BOF_ERR_EL2HLT, "BOF_ERR_EL2HLT: Level 2 halted"},
    {BOF_ERR_EBADE, "BOF_ERR_EBADE: Invalid exchange"},
    {BOF_ERR_EBADR, "BOF_ERR_EBADR: Invalid request descriptor"},
    {BOF_ERR_EXFULL, "BOF_ERR_EXFULL: Exchange full"},
    {BOF_ERR_ENOANO, "BOF_ERR_ENOANO: No anode"},
    {BOF_ERR_EBADRQC, "BOF_ERR_EBADRQC: Invalid request code"},
    {BOF_ERR_EBADSLT, "BOF_ERR_EBADSLT: Invalid slot"},
    {BOF_ERR_EBFONT, "BOF_ERR_EBFONT: Bad font file format"},
    {BOF_ERR_ENOSTR, "BOF_ERR_ENOSTR: Device not a stream"}, // 60
    {BOF_ERR_ENODATA, "BOF_ERR_ENODATA: No data available"},
    {BOF_ERR_ETIME, "BOF_ERR_ETIME: Timer expired"},
    {BOF_ERR_ENOSR, "BOF_ERR_ENOSR: Out of streams resources"},
    {BOF_ERR_ENONET, "BOF_ERR_ENONET: Machine is not on the network"},
    {BOF_ERR_ENOPKG, "BOF_ERR_ENOPKG: Package not installed"},
    {BOF_ERR_EREMOTE, "BOF_ERR_EREMOTE: Object is remote"},
    {BOF_ERR_ENOLINK, "BOF_ERR_ENOLINK: Link has been severed"},
    {BOF_ERR_EADV, "BOF_ERR_EADV: Advertise error"},
    {BOF_ERR_ESRMNT, "BOF_ERR_ESRMNT: Srmount error"},
    {BOF_ERR_ECOMM, "BOF_ERR_ECOMM: Communication error on send"}, // 70
    {BOF_ERR_EPROTO, "BOF_ERR_EPROTO: Protocol error"},
    {BOF_ERR_EMULTIHOP, "BOF_ERR_EMULTIHOP: Multihop attempted"},
    {BOF_ERR_EDOTDOT, "BOF_ERR_EDOTDOT: RFS specific error"},
    {BOF_ERR_EBADMSG, "BOF_ERR_EBADMSG: Not a data message"},
    {BOF_ERR_EOVERFLOW, "BOF_ERR_EOVERFLOW: Value too large for defined data type"},
    {BOF_ERR_ENOTUNIQ, "BOF_ERR_ENOTUNIQ: Name not unique on network"},
    {BOF_ERR_EBADFD, "BOF_ERR_EBADFD: File descriptor in bad state"},
    {BOF_ERR_EREMCHG, "BOF_ERR_EREMCHG: Remote address changed"},
    {BOF_ERR_ELIBACC, "BOF_ERR_ELIBACC: Can not access a needed shared library"},
    {BOF_ERR_ELIBBAD, "BOF_ERR_ELIBBAD: Accessing a corrupted shared library"}, // 80
    {BOF_ERR_ELIBSCN, "BOF_ERR_ELIBSCN: .lib section in a.out corrupted"},
    {BOF_ERR_ELIBMAX, "BOF_ERR_ELIBMAX: Attempting to link in too many shared libraries"},
    {BOF_ERR_ELIBEXEC, "BOF_ERR_ELIBEXEC: Cannot exec a shared library directly"},
    {BOF_ERR_EILSEQ, "BOF_ERR_EILSEQ: Illegal byte sequence"},
    {BOF_ERR_ERESTART, "BOF_ERR_ERESTART: Interrupted system call should be restarted"},
    {BOF_ERR_ESTRPIPE, "BOF_ERR_ESTRPIPE: Streams pipe error"},
    {BOF_ERR_EUSERS, "BOF_ERR_EUSERS: Too many users"},
    {BOF_ERR_ENOTSOCK, "BOF_ERR_ENOTSOCK: Socket operation on non-socket"},
    {BOF_ERR_EDESTADDRREQ, "BOF_ERR_EDESTADDRREQ: Destination address required"},
    {BOF_ERR_EMSGSIZE, "BOF_ERR_EMSGSIZE: Message too long"}, // 90
    {BOF_ERR_EPROTOTYPE, "BOF_ERR_EPROTOTYPE: Protocol wrong type for socket"},
    {BOF_ERR_ENOPROTOOPT, "BOF_ERR_ENOPROTOOPT: Protocol not available"},
    {BOF_ERR_EPROTONOSUPPORT, "BOF_ERR_EPROTONOSUPPORT: Protocol not supported"},
    {BOF_ERR_ESOCKTNOSUPPORT, "BOF_ERR_ESOCKTNOSUPPORT: Socket type not supported"},
    {BOF_ERR_EOPNOTSUPP, "BOF_ERR_EOPNOTSUPP: Operation not supported on transport endpoint"},
    {BOF_ERR_EPFNOSUPPORT, "BOF_ERR_EPFNOSUPPORT: Protocol family not supported"},
    {BOF_ERR_EAFNOSUPPORT, "BOF_ERR_EAFNOSUPPORT: Address family not supported by protocol"},
    {BOF_ERR_EADDRINUSE, "BOF_ERR_EADDRINUSE: Address already in use"},
    {BOF_ERR_EADDRNOTAVAIL, "BOF_ERR_EADDRNOTAVAIL: Cannot assign requested address"},
    {BOF_ERR_ENETDOWN, "BOF_ERR_ENETDOWN: Network is down"}, // 100
    {BOF_ERR_ENETUNREACH, "BOF_ERR_ENETUNREACH: Network is unreachable"},
    {BOF_ERR_ENETRESET, "BOF_ERR_ENETRESET: Network dropped connection because of reset"},
    {BOF_ERR_ECONNABORTED, "BOF_ERR_ECONNABORTED: Software caused connection abort"},
    {BOF_ERR_ECONNRESET, "BOF_ERR_ECONNRESET: Connection reset by peer"},
    {BOF_ERR_ENOBUFS, "BOF_ERR_ENOBUFS: No buffer space available"},
    {BOF_ERR_EISCONN, "BOF_ERR_EISCONN: Transport endpoint is already connected"},
    {BOF_ERR_ENOTCONN, "BOF_ERR_ENOTCONN: Transport endpoint is not connected"},
    {BOF_ERR_ESHUTDOWN, "BOF_ERR_ESHUTDOWN: Cannot send after transport endpoint shutdown"},
    {BOF_ERR_ETOOMANYREFS, "BOF_ERR_ETOOMANYREFS: Too many references: cannot splice"},
    {BOF_ERR_ETIMEDOUT, "BOF_ERR_ETIMEDOUT: Connection timed out"}, // 110
    {BOF_ERR_ECONNREFUSED, "BOF_ERR_ECONNREFUSED: Connection refused"},
    {BOF_ERR_EHOSTDOWN, "BOF_ERR_EHOSTDOWN: Host is down"},
    {BOF_ERR_EHOSTUNREACH, "BOF_ERR_EHOSTUNREACH: No route to host"},
    {BOF_ERR_EALREADY, "BOF_ERR_EALREADY: Operation already in progress"},
    {BOF_ERR_EINPROGRESS, "BOF_ERR_EINPROGRESS: Operation now in progress"},
    {BOF_ERR_ESTALE, "BOF_ERR_ESTALE: Stale file handle"},
    {BOF_ERR_EUCLEAN, "BOF_ERR_EUCLEAN: Structure needs cleaning"},
    {BOF_ERR_ENOTNAM, "BOF_ERR_ENOTNAM: Not a XENIX named type file"},
    {BOF_ERR_ENAVAIL, ": No XENIX semaphores available"},
    {BOF_ERR_EISNAM, "BOF_ERR_EISNAM: Is a named type file"}, // 120
    {BOF_ERR_EREMOTEIO, "BOF_ERR_EREMOTEIO: Remote I/O error"},
    {BOF_ERR_EDQUOT, "BOF_ERR_EDQUOT: Quota exceeded"},
    {BOF_ERR_ENOMEDIUM, "BOF_ERR_ENOMEDIUM: No medium found"},
    {BOF_ERR_EMEDIUMTYPE, "BOF_ERR_EMEDIUMTYPE: Wrong medium type"},
    {BOF_ERR_ECANCELED, "BOF_ERR_ECANCELED: Operation Canceled"},
    {BOF_ERR_ENOKEY, "BOF_ERR_ENOKEY: Required key not available"},
    {BOF_ERR_EKEYEXPIRED, "BOF_ERR_EKEYEXPIRED: Key has expired"},
    {BOF_ERR_EKEYREVOKED, "BOF_ERR_EKEYREVOKED: Key has been revoked"},
    {BOF_ERR_EKEYREJECTED, "BOF_ERR_EKEYREJECTED: Key was rejected by service"},
    {BOF_ERR_EOWNERDEAD, "BOF_ERR_EOWNERDEAD: Owner died"}, // 130
    {BOF_ERR_ENOTRECOVERABLE, "BOF_ERR_ENOTRECOVERABLE: State not recoverable"},
    {BOF_ERR_ERFKILL, "BOF_ERR_ERFKILL: Operation not possible due to RF-kill"},
    {BOF_ERR_EHWPOISON, "BOF_ERR_EHWPOISON: Memory page has hardware error"},

    // Not errno compatble error code
    {BOF_ERR_SPECIFIC, "BOF_ERR_SPECIFIC"},
    {BOF_ERR_ALREADY_OPENED, "BOF_ERR_ALREADY_OPENED"},
    {BOF_ERR_NOT_OPENED, "BOF_ERR_NOT_OPENED"},
    {BOF_ERR_CREATE, "BOF_ERR_CREATE"},
    {BOF_ERR_READ, "BOF_ERR_READ"},
    {BOF_ERR_WRITE, "BOF_ERR_WRITE"},
    {BOF_ERR_PURGE, "BOF_ERR_PURGE"},
    {BOF_ERR_SEEK, "BOF_ERR_SEEK"},
    {BOF_ERR_BIND, "BOF_ERR_BIND"},
    {BOF_ERR_CLOSE, "BOF_ERR_CLOSE"},
    {BOF_ERR_INTERNAL, "BOF_ERR_INTERNAL"},
    {BOF_ERR_INIT, "BOF_ERR_INIT"},
    {BOF_ERR_NOT_INIT, "BOF_ERR_NOT_INIT"},
    {BOF_ERR_NO_MORE, "BOF_ERR_NO_MORE"},
    {BOF_ERR_TOO_SMALL, "BOF_ERR_TOO_SMALL"},
    {BOF_ERR_TOO_BIG, "BOF_ERR_TOO_BIG"},
    {BOF_ERR_WRONG_SIZE, "BOF_ERR_WRONG_SIZE"},
    {BOF_ERR_FOUND, "BOF_ERR_FOUND"},
    {BOF_ERR_NOT_FOUND, "BOF_ERR_NOT_FOUND"},
    {BOF_ERR_FREE, "BOF_ERR_FREE"},
    {BOF_ERR_FULL, "BOF_ERR_FULL"},
    {BOF_ERR_EMPTY, "BOF_ERR_EMPTY"},
    {BOF_ERR_EOF, "BOF_ERR_EOF"},
    {BOF_ERR_OPERATION_FAILED, "BOF_ERR_OPERATION_FAILED"},
    {BOF_ERR_OVERRUN, "BOF_ERR_OVERRUN"},
    {BOF_ERR_UNDERRUN, "BOF_ERR_UNDERRUN"},
    {BOF_ERR_STARTED, "BOF_ERR_STARTED"},
    {BOF_ERR_STOPPED, "BOF_ERR_STOPPED"},
    {BOF_ERR_CANNOT_START, "BOF_ERR_CANNOT_START"},
    {BOF_ERR_CANNOT_STOP, "BOF_ERR_CANNOT_STOP"},
    {BOF_ERR_INTERFACE, "BOF_ERR_INTERFACE"},
    {BOF_ERR_PROTOCOL, "BOF_ERR_PROTOCOL"},
    {BOF_ERR_ADDRESS, "BOF_ERR_ADDRESS"},
    {BOF_ERR_FORMAT, "BOF_ERR_FORMAT"},
    {BOF_ERR_WRONG_MODE, "BOF_ERR_WRONG_MODE"},
    {BOF_ERR_NOT_AVAILABLE, "BOF_ERR_NOT_AVAILABLE"},
    {BOF_ERR_INVALID_COMMAND, "BOF_ERR_INVALID_COMMAND"},
    {BOF_ERR_INVALID_ANSWER, "BOF_ERR_INVALID_ANSWER"},
    {BOF_ERR_INVALID_CHECKSUM, "BOF_ERR_INVALID_CHECKSUM"},
    {BOF_ERR_INVALID_CHECKSUM, "BOF_ERR_INVALID_CHECKSUM"},
    {BOF_ERR_BAD_TYPE, "BOF_ERR_BAD_TYPE"},
    {BOF_ERR_NOT_SUPPORTED, "BOF_ERR_NOT_SUPPORTED"},
    {BOF_ERR_PARSER, "BOF_ERR_PARSER"},
    {BOF_ERR_INVALID_STATE, "BOF_ERR_INVALID_STATE"},
    {BOF_ERR_YES, "BOF_ERR_YES"},
    {BOF_ERR_NO, "BOF_ERR_NO"},
    {BOF_ERR_CANCEL, "BOF_ERR_CANCEL"},
    {BOF_ERR_INDEX, "BOF_ERR_INDEX"},
    {BOF_ERR_CURSOR, "BOF_ERR_CURSOR"},
    {BOF_ERR_DUPLICATE, "BOF_ERR_DUPLICATE"},
    {BOF_ERR_SET, "BOF_ERR_SET"},
    {BOF_ERR_GET, "BOF_ERR_GET"},
    {BOF_ERR_PENDING, "BOF_ERR_PENDING"},
    {BOF_ERR_RUNNING, "BOF_ERR_RUNNING"},
    {BOF_ERR_NOT_RUNNING, "BOF_ERR_NOT_RUNNING"},
    {BOF_ERR_LOCK, "BOF_ERR_LOCK"},
    {BOF_ERR_UNLOCK, "BOF_ERR_UNLOCK"},
    {BOF_ERR_FINISHED, "BOF_ERR_FINISHED"},
    {BOF_ERR_SERIALIZE, "BOF_ERR_SERIALIZE"},
    {BOF_ERR_DESERIALIZE, "BOF_ERR_DESERIALIZE"},
    {BOF_ERR_INVALID_SRC, "BOF_ERR_INVALID_SRC1"},
    {BOF_ERR_INVALID_DST, "BOF_ERR_INVALID_DST"},
    {BOF_ERR_BAD_STATUS, "BOF_ERR_BAD_STATUS"},
    {BOF_ERR_BAD_ID, "BOF_ERR_BAD_ID"},
    {BOF_ERR_INPUT, "BOF_ERR_INPUT"},
    {BOF_ERR_OUTPUT, "BOF_ERR_OUTPUT"},
    {BOF_ERR_CODEC, "BOF_ERR_CODEC"},
    {BOF_ERR_INVALID_HANDLE, "BOF_ERR_INVALID_HANDLE"},
    {BOF_ERR_DONT_EXIST, "BOF_ERR_DONT_EXIST"},
    {BOF_ERR_OUT_OF_RANGE, "BOF_ERR_OUT_OF_RANGE"},
    {BOF_ERR_EXCEPTION, "BOF_ERR_EXCEPTION"},
    {BOF_ERR_ABORT, "BOF_ERR_ABORT"},
    {BOF_ERR_DMA, "BOF_ERR_DMA"},
    {BOF_ERR_DMA_IN, "BOF_ERR_DMA_IN"},
    {BOF_ERR_DMA_OUT, "BOF_ERR_DMA_OUT"},
    {BOF_ERR_STORAGE, "BOF_ERR_STORAGE"},
    {BOF_ERR_BUFFER, "BOF_ERR_BUFFER"},
    {BOF_ERR_RESET, "BOF_ERR_RESET"},
    {BOF_ERR_MAP, "BOF_ERR_MAP"},
    {BOF_ERR_UNMAP, "BOF_ERR_UNMAP"},
    {BOF_ERR_SCHEDULER, "BOF_ERR_SCHEDULER"},
    {BOF_ERR_PRIORITY, "BOF_ERR_PRIORITY"},
    {BOF_ERR_HARD_INIT, "BOF_ERR_HARD_INIT"},
    {BOF_ERR_SOFT_INIT, "BOF_ERR_SOFT_INIT"},
    {BOF_ERR_LOAD, "BOF_ERR_LOAD"},
    {BOF_ERR_SAVE, "BOF_ERR_SAVE"},
    {BOF_ERR_NO_CLIENT, "BOF_ERR_NO_CLIENT"},
    {BOF_ERR_ALREADY_CLOSED, "BOF_ERR_ALREADY_CLOSED"},
    {BOF_ERR_NOT_STARTED, "BOF_ERR_NOT_STARTED"},
    {BOF_ERR_SKIP, "BOF_ERR_SKIP"},
    {BOF_ERR_MTU, "BOF_ERR_MTU"},
    {BOF_ERR_QOS, "BOF_ERR_QOS"},
    {BOF_ERR_IP, "BOF_ERR_IP"},
    {BOF_ERR_EXIT_THREAD, "BOF_ERR_EXIT_THREAD"},
};

/*
 This does not always work as this static var will be initialized when a function of the translation unit will be called (Bof_KbHit ok but Bof_sleep no
class BofStdInitializer
{
public:
  BofStdInitializer() { Bof_Initialize(); }
  ~BofStdInitializer() { Bof_Shutdown(); }
};

static BofStdInitializer S_BofStdInitializer;		//Just to call Bof_Initialize/Bof_Shutdown when the app linked with this lib is started/stopped
*/
uint32_t GL_BofDbgPrintfStartTime_U32 = 0;
// bool  GL_BofLoggerHasBeenDeleted_B = false;

std::string Bof_GetVersion()
{
  // Just to check
  // const char *pFfmpegVersion_c = av_version_info();
  // const char *pOpenSslVersion_c = OpenSSL_version(OPENSSL_VERSION);
  // const char *pBoostVersion_c = BOOST_LIB_VERSION;

  return std::to_string(BOFSTD_VERSION_MAJOR) + "." + std::to_string(BOFSTD_VERSION_MINOR) + "." + std::to_string(BOFSTD_VERSION_PATCH) + "." + std::to_string(BOFSTD_VERSION_BUILD);
}

// Emscripten only accept this kind of callback
#if defined(__EMSCRIPTEN__)
static void S_BofEmscriptenCallback(void *_pArg)
{
  BOFERR Sts_E;

  // if (!EM_ASM_INT({ return Module.fs_is_ready; }))
  //  if ((GL_BofStdParam_X.pPersistentRootDir_c) && (!EM_ASM_INT({ return Module.fs_is_ready; })))
  if ((GL_BofStdParam_X.pPersistentRootDir_c) &&
      (!EM_ASM_INT({ return Module.fs_is_ready; })))
  {
    printf("wait\n");
  }
  else
  {
    if (GL_BofStdParam_X.EmscriptenCallback)
    {
      Sts_E = GL_BofStdParam_X.EmscriptenCallback(_pArg); // GL_BofStdParam_X.pEmscriptenCallbackArg);
      // Don't forget to sync with false to make sure you store it to IndexedDB
      EM_ASM(FS.syncfs(
          false, function(err) {if (err) {} }););
      if (Sts_E != BOF_ERR_NO_ERROR)
      {
        emscripten_cancel_main_loop();
      }
    }
  }
}
#endif

uint32_t Bof_MeasureCpuSpeedInMHz()
{
  uint64_t Start_U64 = Bof_RdTsc(), Rts_U64;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  Rts_U64 = Bof_RdTsc() - Start_U64;
  // Since we slept for 1000 milliseconds, the frequency in MHz is:
  return static_cast<uint32_t>(Rts_U64 / 1.0e6);
}

BOFERR Bof_Initialize(BOFSTDPARAM &_rStdParam_X)
{
  BOFERR Rts_E;
  char pName_c[1024];

  GL_BofStdParam_X = _rStdParam_X;
  GL_BofDbgPrintfStartTime_U32 = Bof_GetMsTickCount();

  if (GL_BofStdParam_X.SignalHandler == nullptr)
  {
    GL_BofStdParam_X.SignalHandler = S_DefaultBofSignalHandler;
  }
  Rts_E = BofSocket::S_InitializeStack();
  /* Set the locale to the POSIX C environment */
  setlocale(LC_ALL, "C");
  _rStdParam_X.ComputerName_S = Bof_GetHostName();;
  _rStdParam_X.CpuFreqInMHz_U32 = Bof_MeasureCpuSpeedInMHz();

#if defined(_WIN32)
  // Register signal handler for Ctrl+C and Ctrl+Break events
  SetConsoleCtrlHandler(S_SignalHandler, TRUE);

  OSVERSIONINFOEX osVersionInfo;
  ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
  osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  _rStdParam_X.OsName_S = "Windows";
  if (GetVersionEx((OSVERSIONINFO *)&osVersionInfo))
  {
    sprintf(pName_c, "Windows %d.%d.%d", osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion, osVersionInfo.dwBuildNumber);
    _rStdParam_X.OsName_S = pName_c;
  }
  SetConsoleOutputCP(65001);

  // You need to enable ansi/vt100 control char under windows 10
  // Set output mode to handle virtual terminal sequences
  HANDLE Out_h = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE In_h = GetStdHandle(STD_INPUT_HANDLE);

  // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
  // #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
  // #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  // #endif
  //  Activate ansi support
  if (BOF_IS_HANDLE_VALID(Out_h))
  {
    if (GetConsoleMode(Out_h, &S_ModeOut_DW))
    {
      if (SetConsoleMode(Out_h, S_ModeOut_DW | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
      {
        // wprintf(L"\x1b[31mThis text has a red foreground using SGR.31.\r\n");
      }
    }
  }
  if (BOF_IS_HANDLE_VALID(In_h))
  {
    if (GetConsoleMode(In_h, &S_ModeIn_DW))
    {
      if (SetConsoleMode(In_h, S_ModeIn_DW | (~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT))))
      {
      }
    }
  }
#else
  signal(SIGINT, S_SignalHandler);
  signal(SIGQUIT, S_SignalHandler);
  signal(SIGTERM, S_SignalHandler);
  signal(SIGKILL, S_SignalHandler);
  signal(SIGABRT, S_SignalHandler);

  struct utsname Si_X;

  _rStdParam_X.OsName_S = "Linux";
  if (uname(&Si_X) != -1)
  {
    sprintf(pName_c, "%s %s %s Cpu %s", Si_X.sysname, Si_X.release, Si_X.version, Si_X.machine);
    _rStdParam_X.OsName_S = pName_c;
  }
  signal(SIGPIPE, SIG_IGN);
  /*
  tcgetattr(STDIN_FILENO, &S_SavedTermIos_X);
  S_NewTermIos_X = S_SavedTermIos_X;

  S_NewTermIos_X.c_lflag &= ~(ICANON | ECHO);
  S_NewTermIos_X.c_cc[VMIN] = 1;
  S_NewTermIos_X.c_cc[VTIME] = 0;
   */
  /*TO BE VALIDATED AS UT repport err on stderr

  "Child aborted" (exit code: 0)

  ------- Stderr: -------
  eJwVxUEOgjAQAMC7r9gXuCGRBA4cMMGb1tQHkFIWXKUt6W5j/L1xLtOLUFbgCPhMgVDJBc/6xanwNvcrRcVPym9siea6WWrnmmqpJo/3nF7kFUtkVRJFSSV7wuA4Hv2+w8aR4NTCVVZ4qIwDdB2czWUcrB1v5p+xhx+4vSxS====


    if (tcsetattr(STDIN_FILENO, TCSANOW, &S_NewTermIos_X) < 0)
    {
      Rts_E=BOF_ERR_NOT_SUPPORTED;
    }
    */
#endif

#if defined(__EMSCRIPTEN__)
    // EM_ASM is a macro to call in-line JavaScript code.
  if (GL_BofStdParam_X.pPersistentRootDir_c)
  {
    EM_ASM({
      Module.fs_is_ready = 0;                // flag to check when data are synchronized
      FS.mkdir(UTF8ToString($0));            // Make a directory other than '/'
      FS.mount(IDBFS, {}, UTF8ToString($0)); // Then mount with IDBFS type

      FS.syncfs( // Then sync with true
          true, function(err) {
            Module.print("End persistent file sync..");
            assert(!err);
            Module.fs_is_ready = 1;
          });
           },
           GL_BofStdParam_X.pPersistentRootDir_c);
  }
  /*
  0 lets the browser decide how often to call your callback, like v-sync. You can pass a positive integer to set a specific frame rate.
  true stops execution at this point, your next code that runs will be the loop callback.
  */
  if (GL_BofStdParam_X.EmscriptenCallback)
  {
    emscripten_set_main_loop_arg(S_BofEmscriptenCallback, GL_BofStdParam_X.pEmscriptenCallbackArg, GL_BofStdParam_X.EmscriptenCallbackFps_U32, false);
  }
#endif

  _rStdParam_X.Version_S = Bof_GetVersion();
  return Rts_E;
}

const char *Bof_ErrorCode(BOFERR _ErrorCode_E)
{
  const char *pRts_c;

  auto It = S_ErrorCodeCollection.find(_ErrorCode_E);
  if (It == S_ErrorCodeCollection.end())
  {
    // Not really thread safe but....
    snprintf(S_pUnknownError_c, sizeof(S_pUnknownError_c), "Unknown error (%d/0x%X)", _ErrorCode_E, _ErrorCode_E);
    pRts_c = S_pUnknownError_c;
  }
  else
  {
    pRts_c = It->second;
  }
  return pRts_c;
}

// For WIN32 or linux system error code
std::string Bof_SystemErrorCode(uint32_t _ErrorCode_U32)
{
  return std::system_category().message(_ErrorCode_U32);
}
BOFERR Bof_Shutdown()
{
  BOFERR Rts_E;
  Rts_E = BofSocket::S_ShutdownStack();

#if defined(_WIN32)
  printf("\x1b[0m");

  // You need to enable ansi/vt100 control char under windows 10
  // Set output mode to handle virtual terminal sequences
  HANDLE Out_h = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE In_h = GetStdHandle(STD_INPUT_HANDLE);
  if (BOF_IS_HANDLE_VALID(Out_h))
  {
    SetConsoleMode(Out_h, S_ModeOut_DW);
  }
  if (BOF_IS_HANDLE_VALID(In_h))
  {
    SetConsoleMode(In_h, S_ModeIn_DW);
  }
#else
  /*TO BE VALIDATED AS UT repport err on stderr

  "Child aborted" (exit code: 0)

  ------- Stderr: -------
  eJwVxUEOgjAQAMC7r9gXuCGRBA4cMMGb1tQHkFIWXKUt6W5j/L1xLtOLUFbgCPhMgVDJBc/6xanwNvcrRcVPym9siea6WWrnmmqpJo/3nF7kFUtkVRJFSSV7wuA4Hv2+w8aR4NTCVVZ4qIwDdB2czWUcrB1v5p+xhx+4vSxS====
*/
  /*
    printf("\x1b[0m");
    tcsetattr(STDIN_FILENO, TCSANOW, &S_SavedTermIos_X);
    */
#endif

  // Give some time to thread/logger to shutdown
  BOF::Bof_MsSleep(1000);
#if defined(__EMSCRIPTEN__)
  if (GL_BofStdParam_X.ExitOnBofShutdown_B)
  {
    emscripten_exit_with_live_runtime();
  }
#endif
  return Rts_E;
}

bool Bof_IsWindows()
{
#if defined(_WIN32)
  return true;
#else
  return false;
#endif
}

bool Bof_AmIRunningInsideDocker()
{
  bool Rts_B;
  bool ItIsADirectory_B;

#if defined(_WIN32)
  Rts_B = (BOF::Bof_IsPathExist("C:\\.dockerenv", ItIsADirectory_B) && (!ItIsADirectory_B));
#else
  Rts_B = (BOF::Bof_IsPathExist("/.dockerenv", ItIsADirectory_B) && (!ItIsADirectory_B));
#endif
  return Rts_B;
}

BofException::BofException(std::string _Header_S, std::string _Context_S, std::string _Where_S, int32_t _ErrorCode_S32)
    : mHeader_S(_Header_S), mContext_S(_Context_S), mWhere_S(_Where_S), mErrorCode_E((BOFERR)_ErrorCode_S32)
{
  std::ostringstream Msg;
  Msg << mHeader_S;

  if (mErrorCode_E)
  {
    Msg << ": " << mErrorCode_E << ": " << BOF::Bof_ErrorCode(mErrorCode_E);
  }
  Msg << " ";

  if (mContext_S != "")
  {
    Msg << '>' << mContext_S << '<';
  }

  if (mWhere_S != "")
  {
    Msg << " @ " << mWhere_S;
  }
  mMessage_S = Msg.str();
}

char const *BofException::what() const noexcept // throw ()
{
  return mMessage_S.c_str();
}

END_BOF_NAMESPACE()