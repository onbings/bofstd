/*
 * Copyright (c) 2000-2020, Onbings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofbuffer interface. 
 *
 * Name:        bofbuffer.h
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
#include <bofstd/bofstd.h>
#include <mutex>

BEGIN_BOF_NAMESPACE()
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
enum BOF_BUFFER_DELETER
{
  BOF_BUFFER_DELETER_NONE = 0,
  BOF_BUFFER_DELETER_FREE,
  BOF_BUFFER_DELETER_ALIGNED_FREE,
  BOF_BUFFER_DELETER_DELETE
};
struct BOF_BUFFER;
BOFSTD_EXPORT BOFERR Bof_AlignedMemFree(BOF::BOF_BUFFER &_rBuffer_X);
struct BOF_BUFFER
{
  BOF_BUFFER_DELETER Deleter_E;
  uint64_t Offset_U64; // For seek
  uint64_t Size_U64;
  uint64_t Capacity_U64;
  void *pUser; // Used by Bof_AlignedMemAlloc for example
  uint8_t *pData_U8;
  mutable std::mutex Mtx;

  BOF_BUFFER()
  {
    // for ReleaseStorage in Reset
    Deleter_E = BOF_BUFFER_DELETER_NONE;
    pUser = nullptr;
    Offset_U64 = 0;
    Size_U64 = 0;
    Capacity_U64 = 0;
    pData_U8 = nullptr;
    // Reset();
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
    Deleter_E = BOF_BUFFER_DELETER_NONE; // Only one deleter  _rOther_X.MustBeDeleted_B;
    pUser = _rOther_X.pUser;
    Offset_U64 = _rOther_X.Offset_U64;
    Size_U64 = _rOther_X.Size_U64;
    Capacity_U64 = _rOther_X.Capacity_U64;
    pData_U8 = _rOther_X.pData_U8;
  }
  BOF_BUFFER &operator=(const BOF_BUFFER &_rOther_X)
  {
    std::lock_guard<std::mutex> Lock(Mtx);
    Deleter_E = BOF_BUFFER_DELETER_NONE; // Only one deleter  _rOther_X.MustBeDeleted_B;
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
    Deleter_E = BOF_BUFFER_DELETER_NONE; // Only one deleter  _rOther_X.MustBeDeleted_B;
    pUser = _rrOther_X.pUser;
    Offset_U64 = _rrOther_X.Offset_U64;
    Size_U64 = _rrOther_X.Size_U64;
    Capacity_U64 = _rrOther_X.Capacity_U64;
    pData_U8 = _rrOther_X.pData_U8;
    return *this;
  }
  void Reset()
  {
    if (pData_U8)
    {
      //printf("WARNING: Resetting an 'active' BOF_BUFFER\n");
    }
    ReleaseStorage();
    std::lock_guard<std::mutex> Lock(Mtx);
    Deleter_E = BOF_BUFFER_DELETER_NONE;
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

    Deleter_E = BOF_BUFFER_DELETER_NONE;
    if (_pData_U8)
    {
      pData_U8 = _pData_U8; // Caller can set Deleter_E if needed
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
  uint8_t *Pos()
  {
    uint8_t *pRts_U8 = nullptr;

    std::lock_guard<std::mutex> Lock(Mtx);
    if (IsValid())
    {
      pRts_U8 = &pData_U8[Size_U64];
    }
    return pRts_U8;
  }
  uint8_t *SeekAbs(uint64_t _Offset_U64, uint64_t &_rRemain_U64)
  {
    uint8_t *pRts_U8 = nullptr;

    std::lock_guard<std::mutex> Lock(Mtx);
    if (_Offset_U64 <= Size_U64)
    {
      Offset_U64 = _Offset_U64;
      _rRemain_U64 = Size_U64 - Offset_U64;
      pRts_U8 = &pData_U8[Offset_U64];
    }
    return pRts_U8;
  }
  uint8_t *SeekRel(int64_t _Amount_S64, uint64_t &_rRemain_U64)
  {
    uint8_t *pRts_U8 = nullptr;
    uint64_t NewOffset_U64;

    std::lock_guard<std::mutex> Lock(Mtx);
    NewOffset_U64 = Offset_U64 + _Amount_S64;
    if (NewOffset_U64 <= Size_U64)
    {
      Offset_U64 = NewOffset_U64;
      _rRemain_U64 = Size_U64 - Offset_U64;
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
  bool Memset(const uint8_t _Val_U8, uint64_t _Size_U64, uint64_t _Offset_U64)
  {
    bool Rts_B = false;

    std::lock_guard<std::mutex> Lock(Mtx);
    if (IsValid()) 
    {
      if ((_Offset_U64 < Capacity_U64) && ((_Size_U64 + _Offset_U64) < Capacity_U64))
      {
        memset(&pData_U8[_Offset_U64], _Val_U8, _Size_U64);
        Rts_B = true;
      }
    }
    return Rts_B;
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
      Deleter_E = BOF_BUFFER_DELETER_DELETE;
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
    switch (Deleter_E)
    {
      default:
      case BOF_BUFFER_DELETER_NONE:
        break;
      case BOF_BUFFER_DELETER_DELETE:
        BOF_SAFE_DELETE_ARRAY(pData_U8);
        break;
      case BOF_BUFFER_DELETER_FREE:
        BOF_SAFE_FREE(pData_U8);
        break;
      case BOF_BUFFER_DELETER_ALIGNED_FREE:
        Bof_AlignedMemFree(*this);
        break;
    }
    Deleter_E = BOF_BUFFER_DELETER_NONE;
    pData_U8 = nullptr;
    Capacity_U64 = 0;
    Offset_U64 = 0;
    Size_U64 = 0;
  }
};

END_BOF_NAMESPACE()