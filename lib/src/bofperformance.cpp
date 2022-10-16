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
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

BEGIN_BOF_NAMESPACE()

/*!
   Description
   The class constructor

   Parameters
   _NbItems_U32 - The number of items to profile

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
  BofProfiler::BofProfiler(uint32_t _NbItems_U32)
{
  uint32_t I_U32;

  mNbItems_U32 = _NbItems_U32;
  mpStats_X = new BOF_STAT_VARIABLE<uint64_t>[mNbItems_U32];
  BOF_ASSERT(mpStats_X != nullptr);

  for (I_U32 = 0; I_U32 < _NbItems_U32; I_U32++)
  {
    mpStats_X[I_U32].Reset();
    mpStats_X[I_U32].Min = (uint64_t)-1;
  }
}

/*!
   Description
   The class destructor

   Parameters
   None

   Returns
   Nothing

   Remarks
   None

   See also
   Nothing
 */
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
    if (mpStats_X[_ItemId_U32].LockCount_U32++ == 0)
    {
      mpStats_X[_ItemId_U32].Crt = Bof_GetNsTickCount();
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
    if (mpStats_X[_ItemId_U32].LockCount_U32)
    {
      if (--mpStats_X[_ItemId_U32].LockCount_U32 == 0)
      {
        Ticks1_U64 = mpStats_X[_ItemId_U32].Crt;
        Ticks2_U64 = Bof_GetNsTickCount();

        mpStats_X[_ItemId_U32].Crt = (uint64_t)(Ticks2_U64 < Ticks1_U64 ? (uint64_t)-1 : 0) + Ticks2_U64 - Ticks1_U64;

        Bof_UpdateStatVar(mpStats_X[_ItemId_U32], mpStats_X[_ItemId_U32].Crt);
      }
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
  bool Ret_B = false;

  BOF_ASSERT(_pStats_X != nullptr);

  if (_ItemId_U32 < mNbItems_U32)
  {
    memcpy(_pStats_X, &mpStats_X[_ItemId_U32], sizeof(BOF_STAT_VARIABLE<uint64_t>));

    Ret_B = true;
  }

  return Ret_B;
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
  bool Ret_B = false;

  if (_ItemId_U32 < mNbItems_U32)
  {
    Bof_UpdateStatVar(mpStats_X[_ItemId_U32], _Value_U64);

    Ret_B = true;
  }

  return Ret_B;
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
  uint32_t LockCount_U32;
  uint64_t Current_U64;

  if (_ItemId_U32 < mNbItems_U32)
  {
    LockCount_U32 = mpStats_X[_ItemId_U32].LockCount_U32;
    Current_U64 = mpStats_X[_ItemId_U32].Crt;

    mpStats_X[_ItemId_U32].Reset();
    mpStats_X[_ItemId_U32].Min = (uint64_t)-1;

    mpStats_X[_ItemId_U32].LockCount_U32 = LockCount_U32;
    mpStats_X[_ItemId_U32].Crt = Current_U64;
  }
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
uint64_t BofProfiler::TicksToUnits(uint64_t _Ticks_U64, PERF_UNITS _Units_U8)
{
  uint64_t Ret_U64 = (uint64_t)-1;
  uint64_t Scale_U64;

  switch (_Units_U8)
  {
    case PERF_SECOND:
    {
      Scale_U64 = 1;
      break;
    }
    case PERF_MILLISECOND:
    {
      Scale_U64 = 1000;
      break;
    }
    case PERF_MICROSECOND:
    {
      Scale_U64 = 1000000;
      break;
    }
    case PERF_NANOSECOND:
    {
      Scale_U64 = 1000000000;
      break;
    }

    default:
    {
      Scale_U64 = (uint64_t)-1;
      break;
    }
  }

  if (Scale_U64 != ((uint64_t)-1))   // && (CpuFreq_U64 != 0))
  {
    // Ret_U64 = ((_Ticks_U64 * Scale_U64) / CpuFreq_U64);
    Ret_U64 = (_Ticks_U64 * Scale_U64);
  }

  return Ret_U64;
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
uint64_t BofProfiler::UnitsToTicks(uint64_t _Value_U64, PERF_UNITS _Units_U8)
{
  uint64_t Ret_U64 = (uint64_t)-1;
  uint64_t Scale_U64;

  switch (_Units_U8)
  {
    case PERF_SECOND:
    {
      Scale_U64 = 1;
      break;
    }
    case PERF_MILLISECOND:
    {
      Scale_U64 = 1000;
      break;
    }
    case PERF_MICROSECOND:
    {
      Scale_U64 = 1000000;
      break;
    }
    case PERF_NANOSECOND:
    {
      Scale_U64 = 1000000000;
      break;
    }

    default:
    {
      Scale_U64 = (uint64_t)-1;
      break;
    }
  }

  if (Scale_U64 != ((uint64_t)-1))   // && (CpuFreq_U64 != 0))
  {
    // Ret_U64 = ((_Value_U64 * CpuFreq_U64) / Scale_U64);
    Ret_U64 = (_Value_U64 / Scale_U64);
  }

  return Ret_U64;
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

/*!
   Description
   This function retrieves the memory used by this object

   Parameters
   None

   Returns
   The size in bytes of the memory used by this object

   Remarks
   None

   See also
   Nothing
 */
uint64_t BofProfiler::GetMemoryUsage()
{
  uint64_t Ret_U64 = 0;

  Ret_U64 += sizeof(BofProfiler);

  if (mpStats_X != nullptr)
  {
    Ret_U64 += (sizeof(BOF_STAT_VARIABLE<uint64_t>) * mNbItems_U32);
  }

  return Ret_U64;
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
      Rts_E = BOF_ERR_ECANCELED; //not started
    }
  }
  return Rts_E;
}
BOFERR BofPerfPointAdd(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, const char *_pPointName_c, ...)
{
  BOFERR                  Rts_E = BOF_ERR_EINVAL;
  int Sts_i;
  BOF_PERF_POINT_ENTRY *pEntry_X;
  va_list              Arg;
  char                 pLine_c[BOF_PERF_POINT_NAME_MAX_CHAR + 0x100];

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
          Rts_E = BOF_ERR_EOVERFLOW; //Too big
        }
      }
      else
      {
        Rts_E = BOF_ERR_EXFULL; //full
      }
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED; //not started
    }
  }
  return Rts_E;
}

BOFERR BofPerfPointStop(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, bool _ForceTrigger_B)
{
  BOFERR                  Rts_E = BOF_ERR_EINVAL;
  BOF_PERF_POINT_ENTRY *pEntry_X;
  uint32_t             i_U32;
  uint64_t             Delta_U64;

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
          printf("%s: %d entries, Min %" PRId64 " Max %" PRId64 " >>Overtime detected<< (%" PRId64 ")\n", _pPerfPointMgr_X->pMgrName_c, _pPerfPointMgr_X->NbEntry_U32, _pPerfPointMgr_X->MinInNs_U64, _pPerfPointMgr_X->MaxInNs_U64, _pPerfPointMgr_X->OverTimeInNs_U64);
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
          printf("[%03d] Ts %-14" PRId64 " ns Abs %-10" PRId64 " ns %-4" PRId64 " ms Prv %-10" PRId64 " ns %-4" PRId64 " ms Step %s:%s", i_U32, pEntry_X->TimeStampInNs_U64, pEntry_X->Delta_U64, pEntry_X->Delta_U64 / 1000000, Delta_U64, Delta_U64 / 1000000, _pPerfPointMgr_X->pMgrName_c, pEntry_X->pPointName_c);
          pEntry_X++;
        }
        Rts_E = BOF_ERR_EL2NSYNC;
      }
      else
      {
        Rts_E = BOF_ERR_NO_ERROR;
        //       DBG_OUT(LOGLEVEL_DEBUG, LOG_MASK_MISC, DRIVER_MSG_HEADER, "%s: %d entries with no over time (%lld), Min %lld Max %lld\n", _pPerfPointMgr_X->pMgrName_c, _pPerfPointMgr_X->NbEntry_U32, _pPerfPointMgr_X->OverTimeInNs_U64, _pPerfPointMgr_X->MinInNs_U64, _pPerfPointMgr_X->MaxInNs_U64);
      }
    }
    else
    {
      Rts_E = BOF_ERR_ECANCELED; //not started
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