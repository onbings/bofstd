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
 - Change version in C:\pro\github\bofstd\CMakeLists.txt: project(bofstd VERSION 5.2.3.11)
 - Change version in C:\pro\github\bofstd\vcpkg.json:     "version": "5.2.3.11",

 Push all modified files to git and get git push ref:
 get push ref e27ccdd96ffc4abf51046e50a099367c62548745		used also during last phase with the .\update-port.py command at rhe end of the procedure

 - Change REF and SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bofstd\portfile.cmake / C:\pro\evs-vcpkg-registry\ports\bofstd\portfile.cmake
   REF e27ccdd96ffc4abf51046e50a099367c62548745
   SHA512 0

 - Validate BofSrd and bofstd with the C:\pro\github\vcpkg-packaging-env project
   cd C:\bld\a
   del * /S /Q
   cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic -DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env

   [DEBUG] Downloading https:\\github.com\onbings\bofstd\archive\e27ccdd96ffc4abf51046e50a099367c62548745.tar.gz
   Error: Failed to download from mirror set:
   File does not have the expected hash:
              url : [ https:\\github.com\onbings\bofstd\archive\e27ccdd96ffc4abf51046e50a099367c62548745.tar.gz ]
        File path : [ C:\pro\vcpkg\downloads\onbings-bofstd-e27ccdd96ffc4abf51046e50a099367c62548745.tar.gz.20964.part ]
    Expected hash : [ 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 ]
      Actual hash : [ e33ded711b18c07c4aaa20955c17b4bfb02a0449f238f7298ddb9be4f0e504878ddf20f58294c3f8b48059c3e74c101cc53daacc201429e8bc7c1350063315d0 ]

   get the Actual hash  and put it back in SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bofstd\portfile.cmake

   cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic -DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env
   Should be ok, compile it with  cmake --build .

   cd C:\pro\github\onbings-vcpkg-registry / cd c:\pro\evs-vcpkg-registry
   git pull
   python .\update-port.py bofstd 5.2.3.11 e27ccdd96ffc4abf51046e50a099367c62548745
   git push
   git log
   commit message should be "Update bofstd to 5.2.3.11\e27ccdd96ffc4abf51046e50a099367c62548745"

   get the new push git ref 8c8de87e6b5b6fcfe568026faa0bf9f12767dbbc

   update baseline in your vcpkg-configuration of your project which want to use this new lib C:\pro\github\vcpkg-packaging-env\vcpkg-configuration.json

   "baseline": "4eb2199109841185f5075ecad7deed414f2c4f55",

 DLL:
 doxygen:
 cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic C:\pro\github\vcpkg-packaging-env
 cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-static C:\pro\github\vcpkg-packaging-env

 cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic C:\pro\github\vcpkg-packaging-env
 LIB:
 cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-static C:\pro\github\vcpkg-packaging-env
 */
#include <bofstd/bofsocket.h>
#include <map>
#include <locale.h>
#include <bofversioninfo.h>

//Just to check: see std::string Bof_GetVersion()
//#include <libavutil/avutil.h>
//#include <openssl/crypto.h>
//#include <boost/version.hpp>

#if defined(_WIN32)
DWORD  S_ModeIn_DW=0, S_ModeOut_DW=0;
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
//static struct termios S_SavedTermIos_X;
//static struct termios S_NewTermIos_X;

#endif

BOFSTD_EXPORT BOF::BOFSTDPARAM GL_BofStdParam_X;

BEGIN_BOF_NAMESPACE()
static char S_pUnknownError_c[128];
static std::map<BOFERR, const char *> S_ErrorCodeCollection{
	{ BOF_ERR_NO_ERROR,          "No error"},
  { BOF_ERR_EPERM,		 	        "Operation not permitted"},
  { BOF_ERR_ENOENT,		 	      "No such file or directory"},
  { BOF_ERR_ESRCH,		 	        "No such process"},
  { BOF_ERR_EINTR,		 	        "Interrupted system call"},
  { BOF_ERR_EIO,		 	          "I/O error"},
  { BOF_ERR_ENXIO,		 	        "No such device or address"},
  { BOF_ERR_E2BIG,		 	        "Argument list too long"},
  { BOF_ERR_ENOEXEC,		 	      "Exec format error"},
  { BOF_ERR_EBADF,		 	        "Bad file number"},
  { BOF_ERR_ECHILD,			      "No child processes"},  //10
  { BOF_ERR_EAGAIN,			      "Try again"},
  { BOF_ERR_ENOMEM,			      "Out of memory"},
  { BOF_ERR_EACCES,			      "Permission denied"},
  { BOF_ERR_EFAULT,			      "Bad address"},
  { BOF_ERR_ENOTBLK,			      "Block device required"},
  { BOF_ERR_EBUSY,			        "Device or resource busy"},
  { BOF_ERR_EEXIST,			      "File exists"},
  { BOF_ERR_EXDEV,			        "Cross-device link"},
  { BOF_ERR_ENODEV,			      "No such device"},
  { BOF_ERR_ENOTDIR,			      "Not a directory"}, //20
  { BOF_ERR_EISDIR,			      "Is a directory"},
  { BOF_ERR_EINVAL,			      "Invalid argument"},
  { BOF_ERR_ENFILE,			      "File table overflow"},
  { BOF_ERR_EMFILE,			      "Too many open files"},
  { BOF_ERR_ENOTTY,			      "Not a typewriter"},
  { BOF_ERR_ETXTBSY,			      "Text file busy"},
  { BOF_ERR_EFBIG,			        "File too large"},
  { BOF_ERR_ENOSPC,			      "No space left on device"},
  { BOF_ERR_ESPIPE,			      "Illegal seek"},
  { BOF_ERR_EROFS,			        "Read-only file system"}, //30
  { BOF_ERR_EMLINK,			      "Too many links"},
  { BOF_ERR_EPIPE,			        "Broken pipe"},
  { BOF_ERR_EDOM,			        "Math argument out of domain of func"},
  { BOF_ERR_ERANGE,			      "Math result not representable"}, //34
  { BOF_ERR_EDEADLK,			      "Resource deadlock would occur"}, //35
  { BOF_ERR_ENAMETOOLONG,		  "File name too long"},
  { BOF_ERR_ENOLCK,			      "No record locks available"},
  { BOF_ERR_ENOSYS,			      "Invalid system call number"},
  { BOF_ERR_ENOTEMPTY,		      "Directory not empty"},
  { BOF_ERR_ELOOP,			        "Too many symbolic links encountered"}, //40
  { BOF_ERR_ENOMSG,	          "No message of desired type"},
  { BOF_ERR_EIDRM,	            "Identifier removed"},
  { BOF_ERR_ECHRNG,	          "Channel number out of range"},
  { BOF_ERR_EL2NSYNC,	        "Level 2 not synchronized"},
  { BOF_ERR_EL3HLT,	          "Level 3 halted"},
  { BOF_ERR_EL3RST,	          "Level 3 reset"},
  { BOF_ERR_ELNRNG,	          "Link number out of range"},
  { BOF_ERR_EUNATCH,	          "Protocol driver not attached"},
  { BOF_ERR_ENOCSI,	          "No CSI structure available"},  //50
  { BOF_ERR_EL2HLT,	          "Level 2 halted"},  
  { BOF_ERR_EBADE,	            "Invalid exchange"},
  { BOF_ERR_EBADR,	            "Invalid request descriptor"},
  { BOF_ERR_EXFULL,	          "Exchange full"},
  { BOF_ERR_ENOANO,	          "No anode"},
  { BOF_ERR_EBADRQC,	          "Invalid request code"},
  { BOF_ERR_EBADSLT,	          "Invalid slot"},
  { BOF_ERR_EBFONT,	          "Bad font file format"},
  { BOF_ERR_ENOSTR,	          "Device not a stream"}, //60
  { BOF_ERR_ENODATA,	          "No data available"},
  { BOF_ERR_ETIME,	            "Timer expired"},
  { BOF_ERR_ENOSR,	            "Out of streams resources"},
  { BOF_ERR_ENONET,	          "Machine is not on the network"},
  { BOF_ERR_ENOPKG,	          "Package not installed"},
  { BOF_ERR_EREMOTE,	          "Object is remote"},
  { BOF_ERR_ENOLINK,	          "Link has been severed"},
  { BOF_ERR_EADV,	            "Advertise error"},
  { BOF_ERR_ESRMNT,	          "Srmount error"},
  { BOF_ERR_ECOMM,	            "Communication error on send"}, //70
  { BOF_ERR_EPROTO,	          "Protocol error"},
  { BOF_ERR_EMULTIHOP,	        "Multihop attempted"},
  { BOF_ERR_EDOTDOT,	          "RFS specific error"},
  { BOF_ERR_EBADMSG,	          "Not a data message"},
  { BOF_ERR_EOVERFLOW,	        "Value too large for defined data type"},
  { BOF_ERR_ENOTUNIQ	,	        "Name not unique on network"},
  { BOF_ERR_EBADFD,	          "File descriptor in bad state"},
  { BOF_ERR_EREMCHG,	          "Remote address changed"},
  { BOF_ERR_ELIBACC,	          "Can not access a needed shared library"},
  { BOF_ERR_ELIBBAD,	          "Accessing a corrupted shared library"},  //80
  { BOF_ERR_ELIBSCN,	          ".lib section in a.out corrupted"},
  { BOF_ERR_ELIBMAX,	          "Attempting to link in too many shared libraries"},
  { BOF_ERR_ELIBEXEC,	        "Cannot exec a shared library directly"},
  { BOF_ERR_EILSEQ,	          "Illegal byte sequence"},
  { BOF_ERR_ERESTART,	        "Interrupted system call should be restarted"},
  { BOF_ERR_ESTRPIPE,	        "Streams pipe error"},
  { BOF_ERR_EUSERS,	          "Too many users"},
  { BOF_ERR_ENOTSOCK,	        "Socket operation on non-socket"},
  { BOF_ERR_EDESTADDRREQ,	    "Destination address required"},
  { BOF_ERR_EMSGSIZE,	        "Message too long"},  //90
  { BOF_ERR_EPROTOTYPE,	      "Protocol wrong type for socket"},
  { BOF_ERR_ENOPROTOOPT,	      "Protocol not available"},
  { BOF_ERR_EPROTONOSUPPORT,	  "Protocol not supported"},
  { BOF_ERR_ESOCKTNOSUPPORT,	  "Socket type not supported"},
  { BOF_ERR_EOPNOTSUPP,	      "Operation not supported on transport endpoint"},
  { BOF_ERR_EPFNOSUPPORT,	    "Protocol family not supported"},
  { BOF_ERR_EAFNOSUPPORT,	    "Address family not supported by protocol"},
  { BOF_ERR_EADDRINUSE,	      "Address already in use"},
  { BOF_ERR_EADDRNOTAVAIL,	    "Cannot assign requested address"},
  { BOF_ERR_ENETDOWN,	        "Network is down"}, //100
  { BOF_ERR_ENETUNREACH,	      "Network is unreachable"},
  { BOF_ERR_ENETRESET,	        "Network dropped connection because of reset"},
  { BOF_ERR_ECONNABORTED	,	    "Software caused connection abort"},
  { BOF_ERR_ECONNRESET,	      "Connection reset by peer"},
  { BOF_ERR_ENOBUFS,	          "No buffer space available"},
  { BOF_ERR_EISCONN,	          "Transport endpoint is already connected"},
  { BOF_ERR_ENOTCONN,	        "Transport endpoint is not connected"},
  { BOF_ERR_ESHUTDOWN,	        "Cannot send after transport endpoint shutdown"},
  { BOF_ERR_ETOOMANYREFS,	    "Too many references: cannot splice"},
  { BOF_ERR_ETIMEDOUT,	        "Connection timed out"},  //110
  { BOF_ERR_ECONNREFUSED,	    "Connection refused"},
  { BOF_ERR_EHOSTDOWN,	        "Host is down"},
  { BOF_ERR_EHOSTUNREACH,	    "No route to host"},
  { BOF_ERR_EALREADY,	        "Operation already in progress"},
  { BOF_ERR_EINPROGRESS,	      "Operation now in progress"},
  { BOF_ERR_ESTALE,	          "Stale file handle"},
  { BOF_ERR_EUCLEAN,	          "Structure needs cleaning"},
  { BOF_ERR_ENOTNAM,         	"Not a XENIX named type file"},
  { BOF_ERR_ENAVAIL,	          "No XENIX semaphores available"},
  { BOF_ERR_EISNAM,	          "Is a named type file"},  //120
  { BOF_ERR_EREMOTEIO,	        "Remote I/O error"},
  { BOF_ERR_EDQUOT,	          "Quota exceeded"},
  { BOF_ERR_ENOMEDIUM,	        "No medium found"},
  { BOF_ERR_EMEDIUMTYPE,	      "Wrong medium type"},
  { BOF_ERR_ECANCELED,	        "Operation Canceled"},
  { BOF_ERR_ENOKEY,	          "Required key not available"},
  { BOF_ERR_EKEYEXPIRED,	      "Key has expired"},
  { BOF_ERR_EKEYREVOKED,	      "Key has been revoked"},
  { BOF_ERR_EKEYREJECTED,	    "Key was rejected by service"},
  { BOF_ERR_EOWNERDEAD,	      "Owner died"},  //130
  { BOF_ERR_ENOTRECOVERABLE,	  "State not recoverable"},
  { BOF_ERR_ERFKILL,	          "Operation not possible due to RF-kill"},
  { BOF_ERR_EHWPOISON,	        "Memory page has hardware error"},

    //Not errno compatble error code
  { BOF_ERR_SPECIFIC,          "Specific" },
  { BOF_ERR_ALREADY_OPENED,    "Already opened" },
  { BOF_ERR_NOT_OPENED,        "Not opened" },
  { BOF_ERR_CREATE,            "Error during create access" },
  { BOF_ERR_READ,              "Error during read access" },
  { BOF_ERR_WRITE,             "Error during write access" },
  { BOF_ERR_PURGE,             "Error during purge access" },
  { BOF_ERR_SEEK,              "Error during seek access" },
  { BOF_ERR_BIND,              "Error during bind access" },
  { BOF_ERR_CLOSE,             "Error during close access" },
  { BOF_ERR_INTERNAL,          "Internal error" },
  { BOF_ERR_INIT,              "Init error" },
  { BOF_ERR_NOT_INIT,          "Not init error" },
  { BOF_ERR_NO_MORE,           "No more handle/connection available" },
  { BOF_ERR_TOO_SMALL,         "Buffer/value provided is too small" },
  { BOF_ERR_TOO_BIG,         "Buffer/value provided is too large" },
  { BOF_ERR_WRONG_SIZE,        "Bad buffer/value size" },
  { BOF_ERR_FOUND,             "Resource found" },
  { BOF_ERR_NOT_FOUND,         "Resource not found" },
  { BOF_ERR_FREE,              "Object is free" },
  { BOF_ERR_FULL,              "Object is full" },
  { BOF_ERR_EMPTY,             "Object is empty" },
  { BOF_ERR_EOF,               "End of File" },
  { BOF_ERR_OPERATION_FAILED,  "Operation could not be completed" },
  { BOF_ERR_OVERRUN,           "Overrun" },
  { BOF_ERR_UNDERRUN,          "Underrun" },
  { BOF_ERR_STARTED,           "Started" },
  { BOF_ERR_STOPPED,           "Already stopped" },
  { BOF_ERR_CANNOT_START,      "Cannot start requested operation" },
  { BOF_ERR_CANNOT_STOP,       "Cannot stop requested operation" },
  { BOF_ERR_INTERFACE,         "Bad interface" },
  { BOF_ERR_PROTOCOL,          "Bad protocol" },
  { BOF_ERR_ADDRESS,           "Bad address" },
  { BOF_ERR_FORMAT,            "Bad format" },
  { BOF_ERR_WRONG_MODE,        "Wrong mode" },
  { BOF_ERR_NOT_AVAILABLE,     "Not available" },
  { BOF_ERR_INVALID_COMMAND,   "Invalid command" },
  { BOF_ERR_INVALID_ANSWER,    "Invalid answer" },
  { BOF_ERR_INVALID_CHECKSUM,  "Invalid checksum" },
  { BOF_ERR_INVALID_CHECKSUM,  "Invalid checksum" },
  { BOF_ERR_BAD_TYPE,          "Bad type" },
  { BOF_ERR_NOT_SUPPORTED,     "Not supported" },
  { BOF_ERR_PARSER,            "Parser error" },
  { BOF_ERR_INVALID_STATE,     "Invalid state" },
  { BOF_ERR_YES,               "Yes" },
  { BOF_ERR_NO,                "No" },
  { BOF_ERR_CANCEL,            "Cancel" },
  { BOF_ERR_INDEX,             "Index error" },
  { BOF_ERR_CURSOR,            "Cursor error" },
  { BOF_ERR_DUPLICATE,         "Duplicate value" },
  { BOF_ERR_SET,               "Set error" },
  { BOF_ERR_GET,               "Get error" },
  { BOF_ERR_PENDING,           "Pending" },
  { BOF_ERR_RUNNING,           "Running" },
  { BOF_ERR_NOT_RUNNING,       "Not running" },
  { BOF_ERR_LOCK,              "Lock error" },
  { BOF_ERR_UNLOCK,            "Unlock error" },
  { BOF_ERR_FINISHED,          "Finished" },
  { BOF_ERR_SERIALIZE,         "Serialize" },
  { BOF_ERR_DESERIALIZE,       "Deserialize" },
  { BOF_ERR_INVALID_SRC,       "Invalid source" },
  { BOF_ERR_INVALID_DST,       "Invalid destination" },
  { BOF_ERR_BAD_STATUS,        "Bad status" },
  { BOF_ERR_BAD_ID,            "Bad id" },
  { BOF_ERR_INPUT,             "Input" },
  { BOF_ERR_OUTPUT,            "Output" },
  { BOF_ERR_CODEC,             "Codec" },
	{ BOF_ERR_INVALID_HANDLE,		"Invalid handle value" },
	{ BOF_ERR_EXIST,						"Exist" },
	{ BOF_ERR_DONT_EXIST,			  "Don't exist" },
	{	BOF_ERR_OUT_OF_RANGE,			"Out of range"},
  { BOF_ERR_EXCEPTION,			    "Exception" },
};

/*** Defines *****************************************************************/

/*** Global variables ********************************************************/

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
//bool  GL_BofLoggerHasBeenDeleted_B = false;


std::string Bof_GetVersion()
{
  //Just to check
  //const char *pFfmpegVersion_c = av_version_info();
  //const char *pOpenSslVersion_c = OpenSSL_version(OPENSSL_VERSION); 
  //const char *pBoostVersion_c = BOOST_LIB_VERSION;
    
	return std::to_string(BOFSTD_VERSION_MAJOR ) + "." + std::to_string(BOFSTD_VERSION_MINOR) + "." + std::to_string(BOFSTD_VERSION_PATCH) + "." + std::to_string(BOFSTD_VERSION_BUILD);
}

BOFERR Bof_Initialize(const BOFSTDPARAM &_rStdParam_X)
{
	BOFERR Rts_E;

	GL_BofStdParam_X = _rStdParam_X;
  GL_BofDbgPrintfStartTime_U32 = Bof_GetMsTickCount();

	Rts_E = BofSocket::S_InitializeStack();
	/* Set the locale to the POSIX C environment */
	setlocale(LC_ALL, "C");
#if defined(_WIN32)
	SetConsoleOutputCP(65001);

	// You need to enable ansi/vt100 control char under windows 10
	// Set output mode to handle virtual terminal sequences
	HANDLE Out_h = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE In_h = GetStdHandle(STD_INPUT_HANDLE);

	//https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING    0x0004
#endif

//Activate ansi support
	if (Out_h != BOF_INVALID_HANDLE_VALUE)
	{
		if (GetConsoleMode(Out_h, &S_ModeOut_DW))
		{
			S_ModeOut_DW |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			if (SetConsoleMode(Out_h, S_ModeOut_DW))
			{
				// wprintf(L"\x1b[31mThis text has a red foreground using SGR.31.\r\n");
			}
		}
	}
	if (In_h != BOF_INVALID_HANDLE_VALUE)
	{
		if (GetConsoleMode(In_h, &S_ModeIn_DW))
		{
			S_ModeIn_DW = static_cast<DWORD>(~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
			if (SetConsoleMode(In_h, S_ModeIn_DW))
			{
			}
	}
}


#else
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
	return Rts_E;
}

const char *Bof_ErrorCode(BOFERR _ErrorCode_E)
{
	const char *pRts_c;

	auto It = S_ErrorCodeCollection.find(_ErrorCode_E);
	if (It == S_ErrorCodeCollection.end())
	{
//Not really thread safe but....
		snprintf(S_pUnknownError_c, sizeof(S_pUnknownError_c), "Unknown error (%d/0x%X)", _ErrorCode_E, _ErrorCode_E);
		pRts_c = S_pUnknownError_c;
	}
	else
	{
		pRts_c = It->second;
	}
	return pRts_c;
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
	if (Out_h != BOF_INVALID_HANDLE_VALUE)
	{
		SetConsoleMode(Out_h, S_ModeOut_DW);
	}
	if (In_h != BOF_INVALID_HANDLE_VALUE)
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
//Give some time to thread/logger to shutdown
  BOF::Bof_MsSleep(1000);

	return Rts_E;
}


/*!
 * Check Cpu endianness.
 *
 * \param none
 *
 * \return true if the cpu is little endian.
 *
 * \remark Little Endian byte order is our default binary storage order as it is natively used in most modern cpu.
 */
static uint8_t S_EndiannessCache_U8 = 0xFF;

bool Bof_IsCpuLittleEndian()
{
	if (S_EndiannessCache_U8 == 0xFF)
	{
		int EndianVal_i = 0x69;
		S_EndiannessCache_U8 = (*reinterpret_cast< char * > (&EndianVal_i) == 0x69) ? 0x01 : 0x00;
	}
	return ((S_EndiannessCache_U8 != 0) ? true : false);
}

bool Bof_IsWindows()
{
#if defined(_WIN32)
  return true;
#else
  return false;
#endif
}
BofException::BofException(std::string _Header_S, std::string _Context_S, std::string _Where_S, int32_t _ErrorCode_S32) : mHeader_S(_Header_S), mContext_S(_Context_S), mWhere_S(_Where_S), mErrorCode_E((BOFERR)_ErrorCode_S32) 
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

char const *BofException::what() const noexcept //throw ()
{
  return mMessage_S.c_str();
}

END_BOF_NAMESPACE()