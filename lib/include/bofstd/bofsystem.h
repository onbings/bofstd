/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofsystem interface. It wraps os dependent system call
 *
 * Name:        bofsystem.h
 * Author:      Bernard HARMEL: b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Jan 19 2017  BHA : Initial release
 */
#pragma once
#include <bofstd/bofflag.h>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

BEGIN_BOF_NAMESPACE()

#define BOF_MS_TO_S(v) (static_cast<uint32_t>((v) / 1e3))
#define BOF_MS_TO_US(v) (static_cast<uint32_t>((v)*1e3))
#define BOF_NANO_TO_MS(v) (static_cast<uint64_t>((v) / 1e6))
#define BOF_NANO_TO_S(v) (static_cast<uint64_t>((v) / 1e9))

#define BOF_S_TO_MS(v) (static_cast<uint32_t>((v)*1e3))
#define BOF_MS_TO_US(v) (static_cast<uint32_t>((v)*1e3))
#define BOF_MS_TO_NANO(v) (static_cast<uint64_t>((v)*1e6))
#define BOF_S_TO_NANO(v) (static_cast<uint64_t>((v)*1e9))
#define BOF_INFINITE_TIMEOUT ((uint32_t)-1)
#if defined(_WIN32)
#define BOF_IOCTL(fd, req, LenIn, pInBuffer, LenOut, pOutBuffer, Sts)                                     \
  {                                                                                                       \
    DWORD NbRts;                                                                                          \
    Sts = DeviceIoControl((HANDLE)fd, req, pInBuffer, LenIn, pOutBuffer, LenOut, &NbRts, false) ? 0 : -1; \
  }
#else
#define BOF_IOCTL(fd, req, LenIn, pInBuffer, LenOut, pOutBuffer, Sts) \
  {                                                                   \
    Sts = ioctl(fd, req, pInBuffer);                                  \
  }
#endif
enum class BOF_SEEK_METHOD : uint32_t
{
  BOF_SEEK_BEGIN = 0, /*! The starting point is zero or the beginning of the file.*/
  BOF_SEEK_CURRENT,   /*! The starting point is the current value of the file pointer.*/
  BOF_SEEK_END        /*! The starting point is the current end-of-file position*/
};
enum class BOF_BUFFER_ALLOCATE_ZONE : uint32_t
{
  BOF_BUFFER_ALLOCATE_ZONE_RAM = 0,
  BOF_BUFFER_ALLOCATE_ZONE_HUGE_PAGE,
  //	CMA,      ///< Contiguous Memory Allocator
};
struct BOF_BUFFER_ALLOCATE_HEADER
{
  BOF_BUFFER_ALLOCATE_ZONE AllocateZone_E;
  //	uint32_t SizeInByte_U32;
  int Io_i;
  bool Locked_B;
  char pHugePath_c[128];

  BOF_BUFFER_ALLOCATE_HEADER()
  {
    Reset();
  }

  void Reset()
  {
    AllocateZone_E = BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM;
    //			SizeInByte_U32=0;
    Io_i = -1;
    Locked_B = false;
    pHugePath_c[0] = 0;
  }
};
struct BOF_BUFFER
{
  bool MustBeDeleted_B;
  bool MustBeFreeed_B;
  uint64_t Offset_U64; // For seek
  uint64_t Size_U64;
  uint64_t Capacity_U64;
  void *pUser; // Used by Bof_AlignedMemAlloc for example
  uint8_t *pData_U8;
  mutable std::mutex Mtx;

  BOF_BUFFER()
  {
    // for ReleaseStorage in Reset
    MustBeDeleted_B = false;
    MustBeFreeed_B = false;
    pUser = nullptr;
    Offset_U64 = 0;
    Size_U64 = 0;
    Capacity_U64 = 0;
    pData_U8 = nullptr;
    Reset();
  }
  /* Use SetStorage
    BOF_BUFFER(uint64_t _Capacity_U64, uint64_t _Size_U64, uint8_t *_pData_U8, bool _MustBeDeleted_B)
    {
      SetStorage(_Capacity_U64, _Size_U64, _pData_U8);
      MustBeDeleted_B = _MustBeDeleted_B;
      MustBeFreeed_B = false;
    }
    */
  ~BOF_BUFFER()
  {
    Reset();
  }
  // Copy constructor as we use mutable std::mutex https://stackoverflow.com/questions/30340029/copy-class-with-stdmutex
  BOF_BUFFER(const BOF_BUFFER &_rOther_X)
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    MustBeDeleted_B = false; // Only one deleter  _rOther_X.MustBeDeleted_B;
    MustBeFreeed_B = false;  // Only one deleter _rOther_X.MustBeFreeed_B;
    pUser = _rOther_X.pUser;
    Offset_U64 = _rOther_X.Offset_U64;
    Size_U64 = _rOther_X.Size_U64;
    Capacity_U64 = _rOther_X.Capacity_U64;
    pData_U8 = _rOther_X.pData_U8;
  }
  BOF_BUFFER &operator=(const BOF_BUFFER &_rOther_X)
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    MustBeDeleted_B = false; // Only one deleter _rOther_X.MustBeDeleted_B;
    MustBeFreeed_B = false;  // Only one deleter _rOther_X.MustBeFreeed_B;
    pUser = _rOther_X.pUser;
    Offset_U64 = _rOther_X.Offset_U64;
    Size_U64 = _rOther_X.Size_U64;
    Capacity_U64 = _rOther_X.Capacity_U64;
    pData_U8 = _rOther_X.pData_U8;
    return *this;
  }
  BOF_BUFFER &operator=(const BOF_BUFFER &&_rrOther_X) noexcept
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    MustBeDeleted_B = false; // Only one deleter _rrOther_X.MustBeDeleted_B;
    MustBeFreeed_B = false;  // Only one deleter _rrOther_X.MustBeFreeed_B;
    pUser = _rrOther_X.pUser;
    Offset_U64 = _rrOther_X.Offset_U64;
    Size_U64 = _rrOther_X.Size_U64;
    Capacity_U64 = _rrOther_X.Capacity_U64;
    pData_U8 = _rrOther_X.pData_U8;
    return *this;
  }
  void Reset()
  {
    ReleaseStorage();
    std::lock_guard<std::mutex> Lock(Mtx);
    MustBeDeleted_B = false;
    MustBeFreeed_B = false;
    pUser = nullptr;
    Offset_U64 = 0;
    Size_U64 = 0;
    Capacity_U64 = 0;
    pData_U8 = nullptr;
  }

  void Clear()
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    Offset_U64 = 0;
    Size_U64 = 0;
  }
  uint8_t *SetStorage(uint64_t _Capacity_U64, uint64_t _Size_U64, uint8_t *_pData_U8)
  {
    BOF_ASSERT(_Capacity_U64 < 0x100000000); // For the moment
    ReleaseStorage();

    MustBeDeleted_B = false;
    MustBeFreeed_B = false;
    if (_pData_U8)
    {
      pData_U8 = _pData_U8; // Caller can set MustBeDeleted_B or MustBeFreeed_B if needed
    }
    else
    {
      pData_U8 = AllocStorage(_Capacity_U64); // Will set MustBeDeleted_B to true
    }
    std::lock_guard<std::mutex> Lock(Mtx);
    Capacity_U64 = _Capacity_U64;

    Offset_U64 = 0;
    if (_Size_U64 <= _Capacity_U64)
    {
      Size_U64 = _Size_U64;
    }
    else
    {
      Size_U64 = 0;
    }
    return pData_U8;
  }

  uint64_t RemainToWrite()
  {
    // Called by read/write std::lock_guard<std::mutex> Lock(Mtx);
    return (Size_U64 <= Capacity_U64) ? Capacity_U64 - Size_U64 : 0;
  }
  uint64_t RemainToRead()
  {
    // Called by read/write std::lock_guard<std::mutex> Lock(Mtx);
    return (Offset_U64 < Size_U64) ? Size_U64 - Offset_U64 : 0;
  }
  uint8_t *Seek(uint64_t _Offset_U64, uint64_t &_rRemain_U64)
  {
    uint8_t *pRts_U8 = nullptr;

    std::lock_guard<std::mutex> Lock(Mtx);
    if (_Offset_U64 <= Size_U64)
    {
      Offset_U64 = _Offset_U64;
      _rRemain_U64 = Size_U64 - _Offset_U64;
      pRts_U8 = &pData_U8[Offset_U64];
    }
    return pRts_U8;
  }
  uint8_t *Read(uint64_t _Size_U64, uint64_t &_rNbRead_U64)
  {
    uint8_t *pRts_U8 = nullptr;
    uint64_t Remain_U64;

    std::lock_guard<std::mutex> Lock(Mtx);
    _rNbRead_U64 = 0;
    if (IsValid())
    {
      Remain_U64 = RemainToRead();
      _rNbRead_U64 = (Remain_U64 < _Size_U64) ? Remain_U64 : _Size_U64;
      if (_rNbRead_U64)
      {
        pRts_U8 = &pData_U8[Offset_U64];
        Offset_U64 += _rNbRead_U64;
      }
    }
    return pRts_U8;
  }
  uint8_t *Write(uint64_t _Size_U64, const uint8_t *_pData_U8, uint64_t &_rNbWritten_U64)
  {
    uint8_t *pRts_U8 = nullptr;
    uint64_t Free_U64;

    std::lock_guard<std::mutex> Lock(Mtx);
    _rNbWritten_U64 = 0;
    if ((IsValid()) && (_pData_U8))
    {
      Free_U64 = RemainToWrite();
      _rNbWritten_U64 = (_Size_U64 < Free_U64) ? _Size_U64 : Free_U64;
      if (_rNbWritten_U64)
      {
        memcpy(&pData_U8[Size_U64], _pData_U8, static_cast<size_t>(_rNbWritten_U64));
        Size_U64 += _rNbWritten_U64;
        pRts_U8 = &pData_U8[Size_U64];
      }
    }
    return pRts_U8;
  }
  bool IsValid()
  {
    bool Rts_B = false;

    // Called by read/write std::lock_guard<std::mutex> Lock(Mtx);
    if ((pData_U8) && (Capacity_U64))
    {
      if ((Size_U64 <= Capacity_U64) && (Offset_U64 <= Size_U64))
      {
        Rts_B = true;
      }
    }
    return Rts_B;
  }

  bool IsNull()
  {
    bool Rts_B = true;

    // Called by read/write std::lock_guard<std::mutex> Lock(Mtx);
    if ((pData_U8) && (Capacity_U64))
    {
      Rts_B = false;
    }
    return Rts_B;
  }

  uint8_t *AllocStorage(uint64_t _Capacity_U64)
  {
    BOF_ASSERT(_Capacity_U64 < 0x100000000); // For the moment
    ReleaseStorage();

    uint8_t *pRts = new uint8_t[static_cast<uint32_t>(_Capacity_U64)];

    if (pRts)
    {
      std::lock_guard<std::mutex> Lock(Mtx);
      MustBeDeleted_B = true;
      Capacity_U64 = _Capacity_U64;
      Offset_U64 = 0;
      Size_U64 = 0;
      pData_U8 = pRts;
    }
    return pRts;
  }
  void ReleaseStorage()
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    if (MustBeDeleted_B)
    {
      BOF_SAFE_DELETE_ARRAY(pData_U8);
    }
    else
    {
      if (MustBeFreeed_B)
      {
        BOF_SAFE_FREE(pData_U8);
      }
    }
    MustBeDeleted_B = false;
    MustBeFreeed_B = false;
    Capacity_U64 = 0;
    Offset_U64 = 0;
    Size_U64 = 0;
  }
};
enum class BOF_ACCESS_SIZE : uint32_t
{
  BOF_ACCESS_SIZE_8 = 0, /*! we access the memory zone using 8 bits byte access.*/
  BOF_ACCESS_SIZE_16,    /*! we access the memory zone using 16 bits word access.*/
  BOF_ACCESS_SIZE_32,    /*! we access the memory zone using 32 bits long access.*/
  BOF_ACCESS_SIZE_64,    /*! we access the memory zone using 64 bits long long access.*/
};

///@return The ascii printable version of the memory zone.
///@remarks For example calling DumpMemoryZone(60, pMemoryZone_U8, 16, ' ', true, 0x12345678,true,true) will produce the following output
///@remarks Virtual  <-------------- Binary Data ------------------> <--Ascii Data-->
///@remarks 12345678 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ????????????????
///@remarks 12345688 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F ????????????????
///@remarks 12345698 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F  !"#$%&'()*+,-./
///@remarks 123456A8 30 31 32 33 34 35 36 37 38 39 3A 3B             0123456789:;
struct BOF_DUMP_MEMORY_ZONE_PARAM
{
  uint32_t NbItemToDump_U32;        ///< Specifies the number of item (uint8_t, uint16_t, uint32_t, uint64_t to dump.
  const volatile void *pMemoryZone; ///< Specifies the address of the first byte of the memory zone to dump
  uint32_t NbItemPerLine_U32;       ///< Specifies the number of data item to dump per line. (max MAX_NBBYTEPERLINE=1024/sizeof(item))
  char Separator_c;                 ///< Specifies the character to use between each binary data
  bool ShowHexaPrefix_B;            ///< Specifies if c hexa prefix must be displayed in front of each binary data (virtual offset and binary data value)
  bool GenerateVirtualOffset;       ///< Specifies if the virtual offset (below) must be generated.
  int64_t VirtualOffset_S64;        ///< Specifies a pseudo starting counter value which will be displayed as "the address" of each dumped row. if -1 use the pMemoryZone_U8 value
  bool GenerateBinaryData_B;        ///< Specifies if the binary data must be generated.
  bool GenerateAsciiData_B;         ///< Specifies if the ascii data must be generated.
  bool ReverseEndianness_B;         ///< If true reverse byte ordering on an AccessSize_E (below) boundary.
  BOF_ACCESS_SIZE AccessSize_E;     ///< Specifies how the memory zone must be read
  BOF_DUMP_MEMORY_ZONE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    NbItemToDump_U32 = 0;
    pMemoryZone = nullptr;
    NbItemPerLine_U32 = 16;
    Separator_c = ' ';
    ShowHexaPrefix_B = false;
    GenerateVirtualOffset = true;
    VirtualOffset_S64 = -1;
    GenerateBinaryData_B = true;
    GenerateAsciiData_B = true;
    ReverseEndianness_B = false;
    AccessSize_E = BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8;
  }
};
const uint32_t BOF_MUTEX_MAGIC = 0x01D654AC;

struct BOF_MUTEX
{
  uint32_t Magic_U32;
  std::string Name_S;
  bool Recursive_B;
  std::recursive_mutex RecursiveMtx;
  std::mutex Mtx;

  BOF_MUTEX()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
    Recursive_B = false;
  }

  bool IsValid()
  {
    return (Magic_U32 == BOF_MUTEX_MAGIC);
  }
};

const uint32_t BOF_EVENT_MAGIC = 0x1F564864;

struct BOF_EVENT
{
  uint32_t Magic_U32;
  std::string Name_S;
  std::mutex Mtx;
  std::condition_variable Cv;
  //		bool Canceled_B;
  uint32_t MaxNumberToNotify_U32;
  uint64_t SignaledBitmask_U64;
  //		bool NotifyAll_B;
  bool WaitKeepSignaled_B;

  BOF_EVENT()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
    //			Canceled_B = false;
    MaxNumberToNotify_U32 = 0;
    SignaledBitmask_U64 = 0;
    //			NotifyAll_B = false;
    WaitKeepSignaled_B = false;
  }
  bool IsValid()
  {
    return (Magic_U32 == BOF_EVENT_MAGIC);
  }
};

const uint32_t BOF_SEMAPHORE_MAGIC = 0xABFF8974;

struct BOF_SEMAPHORE
{
  uint32_t Magic_U32;
  std::string Name_S;
  std::mutex Mtx;
  std::condition_variable Cv;
  std::atomic<int32_t> Cpt_S32;
  int32_t Max_S32;

  BOF_SEMAPHORE()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
    Cpt_S32 = 0;
    Max_S32 = 0;
  }
  bool IsValid()
  {
    return (Magic_U32 == BOF_SEMAPHORE_MAGIC);
  }
};

const uint32_t BOF_CONDITIONAL_VARIABLE_MAGIC = 0xCBFDE456;

struct BOF_CONDITIONAL_VARIABLE
{
  uint32_t Magic_U32;
  std::string Name_S;
  std::mutex Mtx;
  std::condition_variable Cv;
  bool NotifyAll_B;

  BOF_CONDITIONAL_VARIABLE()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
    NotifyAll_B = false;
  }
  bool IsValid()
  {
    return (Magic_U32 == BOF_CONDITIONAL_VARIABLE_MAGIC);
  }
};

const uint32_t BOF_FILEMAPPING_MAGIC = 0x165464DE;
enum class BOF_ACCESS_TYPE : uint32_t
{
  BOF_ACCESS_NONE = 0x00000000,

  BOF_ACCESS_READ = 0x00000001,
  BOF_ACCESS_WRITE = 0x00000002,
};
template <>
struct IsItAnEnumBitFLag<BOF_ACCESS_TYPE> : std::true_type
{
};

struct BOF_SHARED_MEMORY
{
  uint32_t Magic_U32;
  std::string Name_S;
#if defined(_WIN32)
  void *pHandle;
#else
#endif
  uint32_t SizeInByte_U32;
  void *pBaseAddress;

  BOF_SHARED_MEMORY()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
#if defined(_WIN32)
    pHandle = nullptr;
#else
#endif
    SizeInByte_U32 = 0;
    pBaseAddress = nullptr;
  }
  bool IsValid()
  {
    return (Magic_U32 == BOF_FILEMAPPING_MAGIC);
  }
};

enum BOF_THREAD_SCHEDULER_POLICY
{
  BOF_THREAD_SCHEDULER_POLICY_OTHER = 0, // cf linux sched.h SCHED_OTHER/SCHED_FIFO/SCHED_RR
  BOF_THREAD_SCHEDULER_POLICY_FIFO = 1,
  BOF_THREAD_SCHEDULER_POLICY_ROUND_ROBIN = 2,
  BOF_THREAD_SCHEDULER_POLICY_MAX
};
enum BOF_THREAD_PRIORITY
{
  //  BOF_THREAD_PRIORITY_IDLE = 1,
  //  BOF_THREAD_PRIORITY_TIME_CRITICAL = 99,
  //  BOF_THREAD_DEFAULT_PRIORITY = 0x7FFFFFFF,
  //  BOF_THREAD_NONE = 0x7FFFFFFE,

  BOF_THREAD_PRIORITY_000 = 0,
  BOF_THREAD_PRIORITY_001,
  BOF_THREAD_PRIORITY_002,
  BOF_THREAD_PRIORITY_003,
  BOF_THREAD_PRIORITY_004,
  BOF_THREAD_PRIORITY_005,
  BOF_THREAD_PRIORITY_006,
  BOF_THREAD_PRIORITY_007,
  BOF_THREAD_PRIORITY_008,
  BOF_THREAD_PRIORITY_009,
  BOF_THREAD_PRIORITY_010,
  BOF_THREAD_PRIORITY_011,
  BOF_THREAD_PRIORITY_012,
  BOF_THREAD_PRIORITY_013,
  BOF_THREAD_PRIORITY_014,
  BOF_THREAD_PRIORITY_015,
  BOF_THREAD_PRIORITY_016,
  BOF_THREAD_PRIORITY_017,
  BOF_THREAD_PRIORITY_018,
  BOF_THREAD_PRIORITY_019,
  BOF_THREAD_PRIORITY_020,
  BOF_THREAD_PRIORITY_021,
  BOF_THREAD_PRIORITY_022,
  BOF_THREAD_PRIORITY_023,
  BOF_THREAD_PRIORITY_024,
  BOF_THREAD_PRIORITY_025,
  BOF_THREAD_PRIORITY_026,
  BOF_THREAD_PRIORITY_027,
  BOF_THREAD_PRIORITY_028,
  BOF_THREAD_PRIORITY_029,
  BOF_THREAD_PRIORITY_030,
  BOF_THREAD_PRIORITY_031,
  BOF_THREAD_PRIORITY_032,
  BOF_THREAD_PRIORITY_033,
  BOF_THREAD_PRIORITY_034,
  BOF_THREAD_PRIORITY_035,
  BOF_THREAD_PRIORITY_036,
  BOF_THREAD_PRIORITY_037,
  BOF_THREAD_PRIORITY_038,
  BOF_THREAD_PRIORITY_039,
  BOF_THREAD_PRIORITY_040,
  BOF_THREAD_PRIORITY_041,
  BOF_THREAD_PRIORITY_042,
  BOF_THREAD_PRIORITY_043,
  BOF_THREAD_PRIORITY_044,
  BOF_THREAD_PRIORITY_045,
  BOF_THREAD_PRIORITY_046,
  BOF_THREAD_PRIORITY_047,
  BOF_THREAD_PRIORITY_048,
  BOF_THREAD_PRIORITY_049,
  BOF_THREAD_PRIORITY_050,
  BOF_THREAD_PRIORITY_051,
  BOF_THREAD_PRIORITY_052,
  BOF_THREAD_PRIORITY_053,
  BOF_THREAD_PRIORITY_054,
  BOF_THREAD_PRIORITY_055,
  BOF_THREAD_PRIORITY_056,
  BOF_THREAD_PRIORITY_057,
  BOF_THREAD_PRIORITY_058,
  BOF_THREAD_PRIORITY_059,
  BOF_THREAD_PRIORITY_060,
  BOF_THREAD_PRIORITY_061,
  BOF_THREAD_PRIORITY_062,
  BOF_THREAD_PRIORITY_063,
  BOF_THREAD_PRIORITY_064,
  BOF_THREAD_PRIORITY_065,
  BOF_THREAD_PRIORITY_066,
  BOF_THREAD_PRIORITY_067,
  BOF_THREAD_PRIORITY_068,
  BOF_THREAD_PRIORITY_069,
  BOF_THREAD_PRIORITY_070,
  BOF_THREAD_PRIORITY_071,
  BOF_THREAD_PRIORITY_072,
  BOF_THREAD_PRIORITY_073,
  BOF_THREAD_PRIORITY_074,
  BOF_THREAD_PRIORITY_075,
  BOF_THREAD_PRIORITY_076,
  BOF_THREAD_PRIORITY_077,
  BOF_THREAD_PRIORITY_078,
  BOF_THREAD_PRIORITY_079,
  BOF_THREAD_PRIORITY_080,
  BOF_THREAD_PRIORITY_081,
  BOF_THREAD_PRIORITY_082,
  BOF_THREAD_PRIORITY_083,
  BOF_THREAD_PRIORITY_084,
  BOF_THREAD_PRIORITY_085,
  BOF_THREAD_PRIORITY_086,
  BOF_THREAD_PRIORITY_087,
  BOF_THREAD_PRIORITY_088,
  BOF_THREAD_PRIORITY_089,
  BOF_THREAD_PRIORITY_090,
  BOF_THREAD_PRIORITY_091,
  BOF_THREAD_PRIORITY_092,
  BOF_THREAD_PRIORITY_093,
  BOF_THREAD_PRIORITY_094,
  BOF_THREAD_PRIORITY_095,
  BOF_THREAD_PRIORITY_096,
  BOF_THREAD_PRIORITY_097,
  BOF_THREAD_PRIORITY_098,
  BOF_THREAD_PRIORITY_099,
};

using BofThreadFunction = std::function<void *(const std::atomic<bool> &_rIsThreadLoopMustExit_B, void *_pContext)>;
const uint32_t BOF_THREAD_MAGIC = 0xCBE89448;

struct BOF_THREAD
{
  uint32_t Magic_U32;
  std::string Name_S;
  uint32_t StartStopTimeoutInMs_U32;
  uint32_t StackSize_U32;
  uint32_t ThreadCpuCoreAffinity_U32;
  BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF_THREAD_PRIORITY ThreadPriority_E;
  BofThreadFunction ThreadFunction;
  void *pUserContext;
  void *pThreadExitCode;

#if defined(_WIN32)
  void *pThread;     /*! Thread windows handle*/
  uint32_t ThreadId; /*! Thread windows Id*/
#else
  pthread_t ThreadId;
#endif
  std::atomic<bool> ThreadLoopMustExit_B;
  std::atomic<bool> ThreadRunning_B;

  BOF_THREAD()
  {
    Reset();
  }

  void Reset()
  {
    Magic_U32 = 0;
    Name_S = "";
    StartStopTimeoutInMs_U32 = 1000;
    StackSize_U32 = 0;
    ThreadCpuCoreAffinity_U32 = 0;
    ThreadSchedulerPolicy_E = BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPriority_E = BOF_THREAD_PRIORITY_050;
    ThreadFunction = nullptr;
    pUserContext = nullptr;

    ThreadLoopMustExit_B = false;
    ThreadRunning_B = false;
#if defined(_WIN32)
    pThread = nullptr;
    ThreadId = 0;
#else
    ThreadId = 0;
#endif
    pThreadExitCode = nullptr;
  }
  bool IsValid()
  {
    return (Magic_U32 == BOF_THREAD_MAGIC);
  }
};

struct BOF_SYSTEM_USAGE_INFO
{
  struct
  {
    float UserCpuUsedInSec_f;   /* user CPU time used */
    float SystemCpuUsedInSec_f; /* system CPU time used */

    uint64_t UpTimeInSec_U64; /* Seconds since boot */
  } TIME;

  struct
  {
    uint64_t NbSoftPageFault_U64;            /* page reclaims (soft page faults) */
    uint64_t NbHardPageFault_U64;            /* page faults (hard page faults) */
    uint64_t NbBlkInputOp_U64;               /* block input operations */
    uint64_t NbBlkOutputOp_U64;              /* block output operations */
    uint64_t NbVoluntaryContextSwitch_U64;   /* voluntary context switches */
    uint64_t NbInvoluntaryContextSwitch_U64; /* involuntary context switches */

    float pLoad_f[3];       /* 1, 5, and 15 minute load averages */
    uint64_t NbProcess_U64; /* Number of current processes */
  } OS;

  struct
  {
    uint64_t MaxRssInKB_U64; /* maximum resident set size */

    uint64_t TotalRamInKB_U64;  /* Total usable main memory size */
    uint64_t FreeRamInKB_U64;   /* Available memory size */
    uint64_t SharedRamInKB_U64; /* Amount of shared memory */
    uint64_t BufferRamInKB_U64; /* Memory used by buffers */
    uint64_t TotalSwapInKB_U64; /* Total swap space size */
    uint64_t FreeSwapInKB_U64;  /* Swap space still available */
    uint64_t TotaHighInKB_U64;  /* Total high memory size */
    uint64_t FreeHighInKB_U64;  /* Available high memory size */
  } MEMORY;

  struct
  {
    uint32_t SectorSizeInByte_U32;    /* Size of a sector on disk device (specified in Bof_SystemUsageInfo call)*/
    uint32_t BlockSizeInByte_U32;     /* Size of a block on disk device (specified in Bof_SystemUsageInfo call)*/
    uint64_t CapacityInByte_U64;      /* Factory capacity of the device (specified in Bof_SystemUsageInfo call)*/
    uint64_t RemainingSizeInByte_U64; /* Available disk space (specified in Bof_SystemUsageInfo call)*/
  } DISK;
};
// !! http://h30499.www3.hp.com/t5/Languages-and-Scripting/migration-to-64-bit-mode-semctl/td-p/3204127#.VUCj4XhV2zl !!!!
typedef union semsetgetval {
  int val; /* Value for SETVAL */
} BOF_SEM_SETGETVAL;

BOFSTD_EXPORT BOFERR Bof_OpenSharedMemory(const std::string &_rName_S, uint32_t _SizeInByte_U32, BOF_ACCESS_TYPE _AccessType_E, BOF_SHARED_MEMORY &_rSharedMemory_X);
BOFSTD_EXPORT bool Bof_IsSharedMemoryValid(BOF_SHARED_MEMORY &_rSharedMemory_X);
BOFSTD_EXPORT BOFERR Bof_CloseSharedMemory(BOF_SHARED_MEMORY &_rSharedMemory_X, bool _RemoveIt_B); //__linux__ Need to call this at least once with _RemoveIt_B=true

BOFSTD_EXPORT BOFERR Bof_CreateSemaphore(const std::string &_rName_S, int32_t _InitialCount_S32, BOF_SEMAPHORE &_rSem_X);
BOFSTD_EXPORT bool Bof_IsSemaphoreValid(BOF_SEMAPHORE &_rSem_X);
BOFSTD_EXPORT BOFERR Bof_SignalSemaphore(BOF_SEMAPHORE &_rSem_X);
BOFSTD_EXPORT BOFERR Bof_WaitForSemaphore(BOF_SEMAPHORE &_rSem_X, uint32_t _TimeoutInMs_U32);
BOFSTD_EXPORT BOFERR Bof_DestroySemaphore(BOF_SEMAPHORE &_rSem_X);

/** @brief A simple scoped lock type.
 *
 * A lock_guard controls mutex ownership within a scope, releasing
 * ownership in the destructor.
 */
// Mimic std::lock_gard with an on demand Lock op to use with BOF_LOCK_GUARD_MUTEX_ENTER
template <typename T>
class BofLockGuard
{
public:
  explicit BofLockGuard(T &_rMtx)
      : mrMtx(_rMtx), mLockDone_B(false)
  {
  }

  void Lock()
  {
    if (!mLockDone_B)
    {
      mrMtx.lock();
      mLockDone_B = true;
    }
  }

  ~BofLockGuard()
  {
    if (mLockDone_B)
    {
      mrMtx.unlock();
    }
  }

  BofLockGuard(const BofLockGuard &) = delete;
  BofLockGuard &operator=(const BofLockGuard &) = delete;

private:
  std::atomic<bool> mLockDone_B;
  T &mrMtx;
};
BOFSTD_EXPORT BOFERR Bof_CreateMutex(const std::string &_rName_S, bool _Recursive_B, bool _PriorityInversionAware_B, BOF_MUTEX &_rMtx_X);
BOFSTD_EXPORT bool Bof_IsMutexValid(BOF_MUTEX &_rMtx_X);
BOFSTD_EXPORT BOFERR Bof_LockMutex(BOF_MUTEX &_rMtx_X);
BOFSTD_EXPORT BOFERR Bof_UnlockMutex(BOF_MUTEX &_rMtx_X);
BOFSTD_EXPORT BOFERR Bof_DestroyMutex(BOF_MUTEX &_rMtx_X);

#define BOF_GUARD_MUTEX_ENTER(MultiThreadAware, BofMutex, Sts)                      \
  {                                                                                 \
    Sts = BOF_ERR_INIT;                                                             \
    if ((!MultiThreadAware) || (BofMutex.Magic_U32 == BOF_MUTEX_MAGIC))             \
    {                                                                               \
      Sts = BOF_ERR_NO_ERROR;                                                       \
      BOF::BofLockGuard<std::mutex> Lock(BofMutex.Mtx);                             \
      BOF::BofLockGuard<std::recursive_mutex> RecursiveLock(BofMutex.RecursiveMtx); \
      if (MultiThreadAware)                                                         \
      {                                                                             \
        BofMutex.Recursive_B ? RecursiveLock.Lock() : Lock.Lock();                  \
      }

#define BOF_GUARD_MUTEX_LEAVE() \
  }                             \
  }

BOFSTD_EXPORT BOFERR Bof_CreateEvent(const std::string &_rName_S, bool _InitialState_B, /*bool _NotifyAll_B*/ uint32_t _MaxNumberToNotify_U32, bool _WaitKeepSignaled_B, BOF_EVENT &_rEvent_X);
BOFSTD_EXPORT bool Bof_IsEventValid(BOF_EVENT &_rEvent_X);
BOFSTD_EXPORT BOFERR Bof_SignalEvent(BOF_EVENT &_rEvent_X, /*bool _CancelIt_B*/ uint32_t _Instance_U32);
BOFSTD_EXPORT BOFERR Bof_ResetEvent(BOF_EVENT &_rEvent_X, uint32_t _Instance_U32);
BOFSTD_EXPORT bool Bof_IsEventSignaled(BOF_EVENT &_rEvent_X, uint32_t _Instance_U32);
BOFSTD_EXPORT BOFERR Bof_WaitForEvent(BOF_EVENT &_rEvent_X, uint32_t _TimeoutInMs_U32, uint32_t _Instance_U32);
BOFSTD_EXPORT BOFERR Bof_DestroyEvent(BOF_EVENT &_rEvent_X);

BOFSTD_EXPORT int32_t Bof_PriorityValueFromThreadPriority(BOF_THREAD_PRIORITY _Priority_E);
BOFSTD_EXPORT BOF_THREAD_PRIORITY Bof_ThreadPriorityFromPriorityValue(int32_t _Priority_S32);
BOFSTD_EXPORT BOFERR Bof_GetThreadPriorityRange(BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY &_rMin_E, BOF_THREAD_PRIORITY &_rMax_E);
BOFSTD_EXPORT BOFERR Bof_GetThreadPriorityLevel(BOF_THREAD &_rThread_X, BOF_THREAD_SCHEDULER_POLICY &_rPolicy_E, BOF_THREAD_PRIORITY &_rPriority_E);
BOFSTD_EXPORT BOFERR Bof_SetThreadPriorityLevel(BOF_THREAD &_rThread_X, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E);
BOFSTD_EXPORT BOFERR Bof_GetThreadExitCode(BOF_THREAD &_rThread_X, void **_ppRetCode);
BOFSTD_EXPORT uint32_t Bof_CurrentThreadId();
BOFSTD_EXPORT BOFERR Bof_SetCurrentThreadPriorityLevel(BOF_THREAD_SCHEDULER_POLICY _Policy_E, BOF_THREAD_PRIORITY _Priority_E);
BOFSTD_EXPORT BOFERR Bof_CreateThread(const std::string &_rName_S, BofThreadFunction _ThreadFunction, void *_pUserContext, BOF_THREAD &_rThread_X);
BOFSTD_EXPORT bool Bof_IsThreadValid(BOF_THREAD &_rThread_X);
BOFSTD_EXPORT BOFERR Bof_LaunchThread(BOF_THREAD &_rThread_X, uint32_t _StackSize_U32, uint32_t _ThreadCpuCoreAffinity_U32, BOF_THREAD_SCHEDULER_POLICY _ThreadSchedulerPolicy_E, BOF_THREAD_PRIORITY _ThreadPriority_E, uint32_t _StartStopTimeoutInMs_U32);
BOFSTD_EXPORT BOFERR Bof_DestroyThread(BOF_THREAD &_rThread_X);
BOFSTD_EXPORT BOFERR Bof_GetMemoryState(uint64_t &_rAvailableFreeMemory_U64, uint64_t &_rTotalMemorySize_U64);
BOFSTD_EXPORT uint32_t Bof_InterlockedCompareExchange(volatile uint32_t *_pDestination_U32, uint32_t _ValueToSetIfEqual_U32, uint32_t _CheckIfEqualToThis_U32);
BOFSTD_EXPORT bool Bof_IsPidRunning(uint32_t _Pid_U32);
BOFSTD_EXPORT uint32_t Bof_GetCurrentPid();
BOFSTD_EXPORT BOFERR Bof_GetLastError(bool _NetError_B, int32_t *_pNativeErrorCode_S32 = nullptr);
BOFSTD_EXPORT bool Bof_PatternCompare(const char *_pString_c, const char *_pPattern_c);
BOFSTD_EXPORT BOFERR Bof_Exec(const std::string &_rCommand_S, std::string *_pCapturedOutput_S, int32_t &_rExitCode_S32);
BOFSTD_EXPORT const char *Bof_GetEnvVar(const char *_pName_c);
BOFSTD_EXPORT int Bof_SetEnvVar(const char *_pName_c, const char *_pValue_c, int _Overwrite_i);
//_OsAdvice_i mainly for MADV_DONTFORK option in madvise
BOFSTD_EXPORT BOFERR Bof_LockMem(int _OsAdvice_i, uint64_t _SizeInByte_U64, void *_pData);
BOFSTD_EXPORT BOFERR Bof_UnlockMem(uint64_t _SizeInByte_U64, void *_pData);
BOFSTD_EXPORT bool Bof_AlignedMemCpy8(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32);
BOFSTD_EXPORT bool Bof_AlignedMemCpy16(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32);
BOFSTD_EXPORT bool Bof_AlignedMemCpy32(volatile void *_pDst, const volatile void *_pSrc, uint32_t _SizeInByte_U32);
BOFSTD_EXPORT BOFERR Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE _AllocateZone_E, uint32_t _AligmentInByte_U32, uint32_t _SizeInByte_U32, bool _LockIt_B, int _OsAdvice_i, bool _ClearIt_B, BOF_BUFFER &_rAllocatedBuffer_X);
BOFSTD_EXPORT BOFERR Bof_AlignedMemFree(BOF::BOF_BUFFER &_rBuffer_X);
BOFSTD_EXPORT std::string Bof_DumpMemoryZone(const BOF_DUMP_MEMORY_ZONE_PARAM &_rDumpMemoryZoneParam_X);
BOFSTD_EXPORT int32_t Bof_Random(bool _Reset_B, int32_t _MinValue_S32, int32_t _MaxValue_S32);
BOFSTD_EXPORT std::string Bof_Random(bool _Reset_B, uint32_t _Size_U32, char _MinValue_c, char _MaxValue_c);
BOFSTD_EXPORT std::string Bof_RandomHexa(bool _Reset_B, uint32_t _Size_U32, bool _Upper_B);
BOFSTD_EXPORT BOFERR Bof_ReEvaluateTimeout(uint32_t _Start_U32, uint32_t &_rNewTimeOut_U32);
BOFSTD_EXPORT void Bof_MsSleep(uint32_t _Ms_U32);
BOFSTD_EXPORT void Bof_UsSleep(uint32_t _Us_U32);
BOFSTD_EXPORT uint32_t Bof_GetMsTickCount();
BOFSTD_EXPORT uint64_t Bof_GetUsTickCount();
BOFSTD_EXPORT uint64_t Bof_GetNsTickCount();
BOFSTD_EXPORT uint32_t Bof_ElapsedMsTime(uint32_t _StartInMs_U32);
BOFSTD_EXPORT uint64_t Bof_ElapsedUsTime(uint64_t _StartInUs_U64);
BOFSTD_EXPORT uint64_t Bof_ElapsedNsTime(uint64_t _StartInNs_U64);
BOFSTD_EXPORT bool Bof_IsElapsedTimeInMs(uint32_t _Start_U32, uint32_t _TimeoutInMs_U32);

#if defined(_WIN32)
inline const char *Bof_Eol()
{
  return "\r\n";
}
inline char Bof_FilenameSeparator()
{
  return '\\';
}
#else

inline const char *Bof_Eol()
{
  return "\n";
}

inline char Bof_FilenameSeparator()
{
  return '/';
}
void Bof_LockRam_ShowNewPagefaultCount(const char *logtext, const char *allowed_maj, const char *allowed_min);
BOFERR Bof_LockRam(uint32_t _StackSizeInByte_U32, uint64_t _ReserveProcessMemoryInByte_U64);
#endif

template <typename Container, typename SearchFunc>
auto Bof_EraseWhere(Container &_Container, SearchFunc &&_Func) -> decltype(_Container.end())
{
  return _Container.erase(std::remove_if(_Container.begin(), _Container.end(), std::forward<SearchFunc>(_Func)), _Container.end());
}

template <typename... Args>
using BofCvPredicateAndReset = std::function<bool(Args...)>;
template <typename... Args>
using BofCvSetter = std::function<void(Args...)>;

BOFSTD_EXPORT BOFERR Bof_CreateConditionalVariable(const std::string &_rName_S, bool _NotifyAll_B, BOF_CONDITIONAL_VARIABLE &_rCv_X);

template <typename... Args>
BOFERR Bof_SignalConditionalVariable(BOF_CONDITIONAL_VARIABLE &_rCv_X, BofCvSetter<Args...> _CvSetter, const Args &..._Args)
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (_rCv_X.Magic_U32 == BOF_CONDITIONAL_VARIABLE_MAGIC)
  {
    std::unique_lock<std::mutex> WaitLock_O(_rCv_X.Mtx);
    _CvSetter(_Args...);
    _rCv_X.NotifyAll_B ? _rCv_X.Cv.notify_all() : _rCv_X.Cv.notify_one();
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

template <typename... Args>
BOFERR Bof_WaitForConditionalVariable(BOF_CONDITIONAL_VARIABLE &_rCv_X, uint32_t _TimeoutInMs_U32, BofCvPredicateAndReset<Args...> _CvPredicateAndReset, const Args &..._Args)
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (_rCv_X.Magic_U32 == BOF_CONDITIONAL_VARIABLE_MAGIC)
  {
    std::unique_lock<std::mutex> WaitLock_O(_rCv_X.Mtx);
    // if (_rCv_X.Cv.wait_for(WaitLock_O, std::chrono::milliseconds(_TimeoutInMs_U32), _CvPredicate(_Args...)))
    Rts_E = BOF_ERR_NO_ERROR;
    std::chrono::system_clock::time_point End = std::chrono::system_clock::now() + std::chrono::milliseconds(_TimeoutInMs_U32);
    while (!_CvPredicateAndReset(_Args...))
    {
      if (_rCv_X.Cv.wait_until(WaitLock_O, End) == std::cv_status::timeout)
      {
        Rts_E = _CvPredicateAndReset(_Args...) ? BOF_ERR_NO_ERROR : BOF_ERR_ETIMEDOUT;
        break;
      }
    }
  }
  return Rts_E;
}

BOFSTD_EXPORT BOFERR Bof_DestroyConditionalVariable(BOF_CONDITIONAL_VARIABLE &_rCv_X);
BOFSTD_EXPORT BOFERR Bof_SystemUsageInfo(const char *_pDiskName_c, BOF_SYSTEM_USAGE_INFO &_rSystemUsageInfo_X);

END_BOF_NAMESPACE()