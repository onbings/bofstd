/*!
   Copyright (c) 2008, Onbings All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:              BofPerformance.h
   Author:            Bernard HARMEL: onbings@dscloud.me

   Summary:

   General purpose performance monitoring function (Max, min, mean, etc, ...)

   History:
   V 1.00  June 11 2010  BHA : First Release
 */
#include <bofstd/bofperformance.h>
#include <bofstd/bofsystem.h>

#include <cassert>
#include <inttypes.h>
#include <memory>
#include <stdarg.h>
#include <string.h>
#if defined(__linux__)
#else
#include <Windows.h>
#endif

BEGIN_BOF_NAMESPACE()

/*!
Description
  This function returns the current timestamp
  of the per thread clock
*/
TheProfiler::PerThreadClock::time_point TheProfiler::PerThreadClock::now() noexcept
{
#if defined(__linux__)
  struct timespec Time_X;
  // Get the per thread timestamp
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &Time_X);
  return Clock::time_point(std::chrono::duration_cast<Clock::duration>(std::chrono::seconds(Time_X.tv_sec) + std::chrono::nanoseconds(Time_X.tv_nsec)));
#else
  // Not supported
  // return Clock::time_point();
  struct timespec Time_X;
  FILETIME CreationTime, ExitTime, KernelTime, UserTime;
  // SYSTEMTIME   SystemTime_X;

  GetThreadTimes(GetCurrentThread(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
  //((float)UserTime.dwHighDateTime * 65.536 * 6.5536) + ((float)UserTime.dwLowDateTime / 10000000);
  // uint64_t KernelTime_U64 = (Int64ShllMod32(KernelTime.dwHighDateTime, 32) | KernelTime.dwLowDateTime);
  // uint64_t UserTime_U64 = (Int64ShllMod32(UserTime.dwHighDateTime, 32) | UserTime.dwLowDateTime);
  // FileTimeToSystemTime(&UserTime_U64, &SystemTime_X);

  // All times are expressed using FILETIME data structures.Such a structure contains two 32 - bit values that combine to form a 64 - bit count of 100 - nanosecond time units.
  uint64_t UserTime_U64 = (Int64ShllMod32(UserTime.dwHighDateTime, 32) | UserTime.dwLowDateTime);
  const uint32_t Nb100NsInOneSec_U32 = 10000000;
  Time_X.tv_sec = UserTime_U64 / Nb100NsInOneSec_U32;
  Time_X.tv_nsec = UserTime_U64 % Nb100NsInOneSec_U32;
  return Clock::time_point(std::chrono::duration_cast<Clock::duration>(std::chrono::seconds(Time_X.tv_sec) + std::chrono::nanoseconds(Time_X.tv_nsec)));
#endif
}

BofProfiler::BofProfiler(BOF_PROFILER_TYPE _ProfilerType_E, uint32_t _NbItems_U32)
{
  uint32_t i_U32;

  mProfilerType_E = _ProfilerType_E;
  mNbItems_U32 = _NbItems_U32;

  if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
  {
    mpStats_X = new BOF_STAT_VARIABLE<uint64_t>[mNbItems_U32];
    BOF_ASSERT(mpStats_X != nullptr);

    for (i_U32 = 0; i_U32 < _NbItems_U32; i_U32++)
    {
      mpStats_X[i_U32].Reset();
      mpStats_X[i_U32].Min = (uint64_t)-1;
    }
  }
  else
  {
    mpuExtendedProfiler = std::unique_ptr<ExtendedProfiler>(new ExtendedProfiler(_NbItems_U32));
  }
}

BofProfiler::~BofProfiler()
{
  if (mpStats_X != nullptr)
  {
    BOF_SAFE_DELETE_ARRAY(mpStats_X);
  }
}

/*!
   Description
   This function indicates that the specified item enter in its processing zone

   Parameters
   _ItemId_U32 - The ID of the item

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
void BofProfiler::V_EnterBench(uint32_t _ItemId_U32)
{
  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (mpStats_X[_ItemId_U32].LockCount_U64++ == 0)
      {
        mpStats_X[_ItemId_U32].Crt = Bof_GetNsTickCount();
      }
    }
    else
    {
      mpuExtendedProfiler->V_EnterBench(_ItemId_U32);
    }
  }
}

/*!
   Description
   This function indicates that the specified item leave its processing zone

   Parameters
   _ItemId_U32 - The ID of the item

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
void BofProfiler::V_LeaveBench(uint32_t _ItemId_U32)
{
  uint64_t Ticks1_U64;
  uint64_t Ticks2_U64;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (mpStats_X[_ItemId_U32].LockCount_U64)
      {
        if (--mpStats_X[_ItemId_U32].LockCount_U64 == 0)
        {
          Ticks1_U64 = mpStats_X[_ItemId_U32].Crt;
          Ticks2_U64 = Bof_GetNsTickCount();

          mpStats_X[_ItemId_U32].Crt = (uint64_t)(Ticks2_U64 < Ticks1_U64 ? (uint64_t)-1 : 0) + Ticks2_U64 - Ticks1_U64;

          Bof_UpdateStatVar(mpStats_X[_ItemId_U32], mpStats_X[_ItemId_U32].Crt);
        }
      }
    }
    else
    {
      mpuExtendedProfiler->V_LeaveBench(_ItemId_U32);
    }
  }
}

/*!
   Description
   This function retrieves the statistics for the specified item

   Parameters
   _ItemId_U32 - The ID of the item
   _pStats_X   - The pointer where to store the data

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
bool BofProfiler::GetStats(uint32_t _ItemId_U32, BOF_STAT_VARIABLE<uint64_t> *_pStats_X)
{
  bool Rts_B = false;
  BOF_STAT_VARIABLE<uint64_t> Stat_X;

  BOF_ASSERT(_pStats_X != nullptr);

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      memcpy(_pStats_X, &mpStats_X[_ItemId_U32], sizeof(BOF_STAT_VARIABLE<uint64_t>));
    }
    else
    {
      auto TheTiming = mpuExtendedProfiler->GetItem(_ItemId_U32, static_cast<uint32_t>(mProfilerType_E));

      Stat_X.Crt = ToProfileValue(TheTiming.GetLast());
      Stat_X.Min = ToProfileValue(TheTiming.GetMin());
      Stat_X.Mean = ToProfileValue(TheTiming.GetMean());
      Stat_X.Max = ToProfileValue(TheTiming.GetMax());
      Stat_X.NbSample_U64 = static_cast<uint64_t>(TheTiming.GetCount());

      memcpy(_pStats_X, &Stat_X, sizeof(BOF_STAT_VARIABLE<uint64_t>));
    }
    Rts_B = true;
  }

  return Rts_B;
}

/*!
   Description
   This function sets the statistics for the specified item

   Parameters
   _ItemId_U32 - The ID of the item
   _Value_U64  - The current value

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
bool BofProfiler::SetStats(uint32_t _ItemId_U32, uint64_t _Value_U64)
{
  bool Rts_B = false;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      Bof_UpdateStatVar(mpStats_X[_ItemId_U32], _Value_U64);
    }
    else
    {
      mpuExtendedProfiler->AddValue(_ItemId_U32, TicksToUnits(_Value_U64, BOF_PERF_NANOSECOND));
    }
    Rts_B = true;
  }

  return Rts_B;
}

/*!
   Description
   This function reset the statistics for the specified item

   Parameters
   _ItemId_U32 - The ID of the item

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
void BofProfiler::ResetStats(uint32_t _ItemId_U32)
{
  uint32_t LockCount_U64;
  uint64_t Current_U64;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      LockCount_U64 = mpStats_X[_ItemId_U32].LockCount_U64;
      Current_U64 = mpStats_X[_ItemId_U32].Crt;

      mpStats_X[_ItemId_U32].Reset();
      mpStats_X[_ItemId_U32].Min = (uint64_t)-1;

      mpStats_X[_ItemId_U32].LockCount_U64 = LockCount_U64;
      mpStats_X[_ItemId_U32].Crt = Current_U64;
    }
    else
    {
      mpuExtendedProfiler->Reset(_ItemId_U32);
    }
  }
}

/*!
   Description
   This function returns the number of items
   benched by this profiler

   Parameters
   None

   Returns
   The number of items benched by this profiler

   Remarks
   None

   See also
   Nothing
 */
uint32_t BofProfiler::GetCount()
{
  return mNbItems_U32;
}

uint64_t BofProfiler::GetMin(uint32_t _ItemId_U32)
{
  uint64_t Rts_U64 = 0;
  BOF_STAT_VARIABLE<uint64_t> Stats_X;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (GetStats(_ItemId_U32, &Stats_X))
      {
        Rts_U64 = Stats_X.Min;
      }
    }
    else
    {
      Rts_U64 = ToProfileValue(mpuExtendedProfiler->GetItem(_ItemId_U32, static_cast<uint32_t>(mProfilerType_E)).GetMin());
    }
  }
  return Rts_U64;
}

uint64_t BofProfiler::GetMax(uint32_t _ItemId_U32)
{
  uint64_t Rts_U64 = 0;
  BOF_STAT_VARIABLE<uint64_t> Stats_X;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (GetStats(_ItemId_U32, &Stats_X))
      {
        Rts_U64 = Stats_X.Max;
      }
    }
    else
    {
      Rts_U64 = ToProfileValue(mpuExtendedProfiler->GetItem(_ItemId_U32, static_cast<uint32_t>(mProfilerType_E)).GetMax());
    }
  }

  return Rts_U64;
}

uint64_t BofProfiler::GetMean(uint32_t _ItemId_U32)
{
  uint64_t Rts_U64 = 0;
  BOF_STAT_VARIABLE<uint64_t> Stats_X;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (GetStats(_ItemId_U32, &Stats_X))
      {
        Rts_U64 = Stats_X.Mean;
      }
    }
    else
    {
      Rts_U64 = ToProfileValue(mpuExtendedProfiler->GetItem(_ItemId_U32, static_cast<uint32_t>(mProfilerType_E)).GetMean());
    }
  }
  return Rts_U64;
}

uint64_t BofProfiler::GetLast(uint32_t _ItemId_U32)
{
  uint64_t Rts_U64 = 0;
  BOF_STAT_VARIABLE<uint64_t> Stats_X;

  if (_ItemId_U32 < mNbItems_U32)
  {
    if (mProfilerType_E == BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_LIGHT)
    {
      if (GetStats(_ItemId_U32, &Stats_X))
      {
        Rts_U64 = Stats_X.Crt;
      }
    }
    else
    {
      Rts_U64 = ToProfileValue(mpuExtendedProfiler->GetItem(_ItemId_U32, static_cast<uint32_t>(mProfilerType_E)).GetLast());
    }
  }

  return Rts_U64;
}

/*!
   Description
   This function converts ticks to the specified unit value

   Parameters
   _Ticks_U64 - The ticks count
   _Units_U8  - The value type in which to convert

   Returns
   The converted ticks expressed in the specified unit

   0xFFFFFFFFFFFFFFFF means an error occurred.

   Remarks
   None

   See also
   Nothing
 */
uint64_t BofProfiler::TicksToUnits(uint64_t _Ticks_U64, BOF_PERF_UNITS _Units_U8)
{
  uint64_t Rts_U64 = (uint64_t)-1;
  uint64_t Scale_U64;
  uint64_t CpuFreq_U64 = 1; // GL_NmaTimer_O.GetCpuFrequency();

  switch (_Units_U8)
  {
  case BOF_PERF_SECOND: {
    Scale_U64 = 1;
    break;
  }
  case BOF_PERF_MILLISECOND: {
    Scale_U64 = 1000;
    break;
  }
  case BOF_PERF_MICROSECOND: {
    Scale_U64 = 1000000;
    break;
  }
  case BOF_PERF_NANOSECOND: {
    Scale_U64 = 1000000000;
    break;
  }

  default: {
    Scale_U64 = (uint64_t)-1;
    break;
  }
  }

  if ((Scale_U64 != ((uint64_t)-1)) && (CpuFreq_U64 != 0))
  {
    Rts_U64 = ((_Ticks_U64 * Scale_U64) / CpuFreq_U64);
  }

  return Rts_U64;
}

/*!
   Description
   This function converts the value expressed in the specified unit into its value in ticks

   Parameters
   _Value_U64 - The value
   _Units_U8  - The value units

   Returns
   The converted value in ticks unit

   0xFFFFFFFFFFFFFFFF means an error occurred.

   Remarks
   None

   See also
   Nothing
 */
uint64_t BofProfiler::UnitsToTicks(uint64_t _Value_U64, BOF_PERF_UNITS _Units_U8)
{
  uint64_t Rts_U64 = (uint64_t)-1;
  uint64_t Scale_U64;
  uint64_t CpuFreq_U64 = 1; // GL_NmaTimer_O.GetCpuFrequency();

  switch (_Units_U8)
  {
  case BOF_PERF_SECOND: {
    Scale_U64 = 1;
    break;
  }
  case BOF_PERF_MILLISECOND: {
    Scale_U64 = 1000;
    break;
  }
  case BOF_PERF_MICROSECOND: {
    Scale_U64 = 1000000;
    break;
  }
  case BOF_PERF_NANOSECOND: {
    Scale_U64 = 1000000000;
    break;
  }

  default: {
    Scale_U64 = (uint64_t)-1;
    break;
  }
  }

  if ((Scale_U64 != ((uint64_t)-1)) && (CpuFreq_U64 != 0))
  {
    Rts_U64 = ((_Value_U64 * CpuFreq_U64) / Scale_U64);
  }

  return Rts_U64;
}

uint64_t BofProfiler::ToProfileValue(std::chrono::steady_clock::duration _Val)
{
  uint64_t Value_U64 = std::chrono::duration_cast<std::chrono::microseconds>(_Val).count();

  return UnitsToTicks(Value_U64, BOF_PERF_MICROSECOND);
}

BOF_PERF_POINT_MGR *BofPerfPointOpen(const char *_pName_c, uint32_t _MaxEntry_U32, uint64_t _MaxTimeInNs_U64)
{
  BOF_PERF_POINT_MGR *pRts_X = NULL;

  if ((_pName_c) && (strlen(_pName_c) < BOF_PERF_POINT_MGR_NAME_MAX_CHAR) && (_MaxEntry_U32 <= BOF_MAX_PERF_POINT))
  {
    pRts_X = new BOF_PERF_POINT_MGR;
    if (pRts_X)
    {
      memset(pRts_X, 0, sizeof(BOF_PERF_POINT_MGR));
      pRts_X->MgrMagicNumber_U32 = BOF_PERF_POINT_MAGIC_NUMBER;
      strcpy(pRts_X->pMgrName_c, _pName_c);
      pRts_X->OverTimeInNs_U64 = _MaxTimeInNs_U64;
      pRts_X->MaxEntry_U32 = _MaxEntry_U32;
    }
  }
  return pRts_X;
}
BOFERR BofPerfPointStart(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, bool _ResetTrigger_B)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_pPerfPointMgr_X) && (_pPerfPointMgr_X->MgrMagicNumber_U32 == BOF_PERF_POINT_MAGIC_NUMBER))
  {
    if (!_pPerfPointMgr_X->Started_B)
    {
      if (_ResetTrigger_B)
      {
        _pPerfPointMgr_X->Triggered_B = false;
      }
      if (!_pPerfPointMgr_X->Triggered_B)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        _pPerfPointMgr_X->NbEntry_U32 = 0;
        _pPerfPointMgr_X->Started_B = true;
        _pPerfPointMgr_X->MinInNs_U64 = (uint64_t)-1;
        _pPerfPointMgr_X->MaxInNs_U64 = 0;
        _pPerfPointMgr_X->StartTimeStampInNs_U64 = Bof_GetNsTickCount();
      }
      else
      {
        Rts_E = BOF_ERR_ENOKEY;
      }
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED;
    }
  }
  return Rts_E;
}
BOFERR BofPerfPointResetTrigger(BOF_PERF_POINT_MGR *_pPerfPointMgr_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_pPerfPointMgr_X) && (_pPerfPointMgr_X->MgrMagicNumber_U32 == BOF_PERF_POINT_MAGIC_NUMBER))
  {
    if (_pPerfPointMgr_X->Started_B)
    {
      _pPerfPointMgr_X->Triggered_B = false;
      _pPerfPointMgr_X->StartTimeStampInNs_U64 = Bof_GetNsTickCount();
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED; // not started
    }
  }
  return Rts_E;
}
BOFERR BofPerfPointAdd(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, const char *_pPointName_c, ...)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  int Sts_i;
  BOF_PERF_POINT_ENTRY *pEntry_X;
  va_list Arg;
  char pLine_c[BOF_PERF_POINT_NAME_MAX_CHAR + 0x100];

  if ((_pPerfPointMgr_X) && (_pPerfPointMgr_X->MgrMagicNumber_U32 == BOF_PERF_POINT_MAGIC_NUMBER) && (_pPointName_c) && (strlen(_pPointName_c) < BOF_PERF_POINT_NAME_MAX_CHAR))
  {
    if (_pPerfPointMgr_X->Started_B)
    {
      if (_pPerfPointMgr_X->NbEntry_U32 < _pPerfPointMgr_X->MaxEntry_U32)
      {
        va_start(Arg, _pPointName_c);
        Sts_i = vsprintf(pLine_c, _pPointName_c, Arg);
        va_end(Arg);

        if ((Sts_i >= 0) && (Sts_i < BOF_PERF_POINT_NAME_MAX_CHAR))
        {
          pEntry_X = &_pPerfPointMgr_X->pEntry_X[_pPerfPointMgr_X->NbEntry_U32];
          pEntry_X->TimeStampInNs_U64 = Bof_GetNsTickCount();
          strcpy(pEntry_X->pPointName_c, pLine_c);
          if (_pPerfPointMgr_X->NbEntry_U32 == 0)
          {
            pEntry_X->Delta_U64 = 0;
          }
          else
          {
            pEntry_X->Delta_U64 = Bof_ElapsedNsTime(_pPerfPointMgr_X->StartTimeStampInNs_U64);
            if (pEntry_X->Delta_U64 > _pPerfPointMgr_X->MaxInNs_U64)
            {
              _pPerfPointMgr_X->MaxInNs_U64 = pEntry_X->Delta_U64;
            }
            if (pEntry_X->Delta_U64 < _pPerfPointMgr_X->MinInNs_U64)
            {
              _pPerfPointMgr_X->MinInNs_U64 = pEntry_X->Delta_U64;
            }
            if (pEntry_X->Delta_U64 > _pPerfPointMgr_X->OverTimeInNs_U64)
            {
              _pPerfPointMgr_X->Triggered_B = true;
            }
          }
          pEntry_X->TimeStampInNs_U64 -= _pPerfPointMgr_X->StartTimeStampInNs_U64;
          _pPerfPointMgr_X->NbEntry_U32++;
          Rts_E = BOF_ERR_NO_ERROR;
        }
        else
        {
          Rts_E = BOF_ERR_EOVERFLOW; // Too big
        }
      }
      else
      {
        Rts_E = BOF_ERR_EXFULL; // full
      }
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED; // not started
    }
  }
  return Rts_E;
}

BOFERR BofPerfPointStop(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, bool _ForceTrigger_B)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_PERF_POINT_ENTRY *pEntry_X;
  uint32_t i_U32;
  uint64_t Delta_U64;

  if ((_pPerfPointMgr_X) && (_pPerfPointMgr_X->MgrMagicNumber_U32 == BOF_PERF_POINT_MAGIC_NUMBER))
  {
    if ((_pPerfPointMgr_X->Started_B) || (_ForceTrigger_B))
    {
      _pPerfPointMgr_X->Started_B = false;
      if ((_pPerfPointMgr_X->Triggered_B) || (_ForceTrigger_B))
      {
        if (_ForceTrigger_B)
        {
          printf("%s: %d entries, Min %" PRId64 " Max %" PRId64 " ( %" PRId64 ")\n", _pPerfPointMgr_X->pMgrName_c, _pPerfPointMgr_X->NbEntry_U32, _pPerfPointMgr_X->MinInNs_U64, _pPerfPointMgr_X->MaxInNs_U64, _pPerfPointMgr_X->OverTimeInNs_U64);
        }
        else
        {
          printf("%s: %d entries, Min %" PRId64 " Max %" PRId64 " >>Overtime detected<< (%" PRId64 ")\n", _pPerfPointMgr_X->pMgrName_c, _pPerfPointMgr_X->NbEntry_U32, _pPerfPointMgr_X->MinInNs_U64, _pPerfPointMgr_X->MaxInNs_U64,
                 _pPerfPointMgr_X->OverTimeInNs_U64);
        }
        pEntry_X = _pPerfPointMgr_X->pEntry_X;
        for (i_U32 = 0; i_U32 < _pPerfPointMgr_X->NbEntry_U32; i_U32++)
        {
          if (i_U32)
          {
            BOF_COMPUTE_DELTA(pEntry_X[-1].TimeStampInNs_U64, pEntry_X->TimeStampInNs_U64, Delta_U64);
          }
          else
          {
            Delta_U64 = 0;
          }
          printf("[%03d] Ts %-14" PRId64 " ns Abs %-10" PRId64 " ns %-4" PRId64 " ms Prv %-10" PRId64 " ns %-4" PRId64 " ms Step %s:%s", i_U32, pEntry_X->TimeStampInNs_U64, pEntry_X->Delta_U64, pEntry_X->Delta_U64 / 1000000, Delta_U64, Delta_U64 / 1000000,
                 _pPerfPointMgr_X->pMgrName_c, pEntry_X->pPointName_c);
          pEntry_X++;
        }
        Rts_E = BOF_ERR_EL2NSYNC;
      }
      else
      {
        Rts_E = BOF_ERR_NO_ERROR;
        //       DBG_OUT(LOGLEVEL_DEBUG, LOG_MASK_MISC, DRIVER_MSG_HEADER, "%s: %d entries with no over time (%lld), Min %lld Max %lld\n", _pPerfPointMgr_X->pMgrName_c, _pPerfPointMgr_X->NbEntry_U32, _pPerfPointMgr_X->OverTimeInNs_U64,
        //       _pPerfPointMgr_X->MinInNs_U64, _pPerfPointMgr_X->MaxInNs_U64);
      }
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED; // not started
    }
  }
  return Rts_E;
}
BOFERR BofPerfPointClose(BOF_PERF_POINT_MGR *_pPerfPointMgr_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_pPerfPointMgr_X) && (_pPerfPointMgr_X->MgrMagicNumber_U32 == BOF_PERF_POINT_MAGIC_NUMBER))
  {
    _pPerfPointMgr_X->MgrMagicNumber_U32 = ~BOF_PERF_POINT_MAGIC_NUMBER;
    BOF_SAFE_DELETE_ARRAY(_pPerfPointMgr_X);
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return Rts_E;
}

END_BOF_NAMESPACE()