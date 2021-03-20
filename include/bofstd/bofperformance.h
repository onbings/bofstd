/*!
   Copyright (c) 2008, Onbings All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:              Performance.h
   Author:            Bernard HARMEL: onbings@dscloud.me

   Summary:

   General purpose performance monitoring function (Max, min, mean, etc, ...)

   History:
   V 1.00  June 11 2010  BHA : First Release
 */
#pragma once

/*** Include ***********************************************************************************************************************/

#include <cstdint>
#include <bofstd/bofstatistics.h>

BEGIN_BOF_NAMESPACE()

/*** Globals ***********************************************************************************************************************/

/*** Defines ***********************************************************************************************************************/

// #if !defined(DEBUG)
//
// #define BOF_ENTER_BENCH(PROFILER, ITEM)
// #define BOF_LEAVE_BENCH(PROFILER, ITEM)
//
// #else

#define BOF_ENTER_BENCH(PROFILER, ITEM)    { if ( (PROFILER) != nullptr) { (PROFILER)->V_EnterBench(ITEM); } }
#define BOF_LEAVE_BENCH(PROFILER, ITEM)    { if ( (PROFILER) != nullptr) { (PROFILER)->V_LeaveBench(ITEM); } }

// #endif

/*** Enums *************************************************************************************************************************/

/*!
   Summary
   The various units
 */
typedef enum
{
		PERF_SECOND,                         /*!  s */
		PERF_MILLISECOND,                    /*! ms */
		PERF_MICROSECOND,                    /*! �s */
		PERF_NANOSECOND                      /*! ns */
} PERF_UNITS;

/*** Structures ********************************************************************************************************************/

/*** Constants *********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

class BofProfiler
{
public:

		BofProfiler(uint32_t _NbItems_U32);

		virtual ~BofProfiler();

		virtual void V_EnterBench(uint32_t _ItemId_U32);

		virtual void V_LeaveBench(uint32_t _ItemId_U32);

		bool SetStats(uint32_t _ItemId_U32, uint64_t _Value_U64);

		bool GetStats(uint32_t _ItemId_U32, BOF_STAT_VARIABLE<uint64_t> *_pStats_X);

		void ResetStats(uint32_t _ItemId_U32);

		virtual uint64_t GetMemoryUsage();

		static uint64_t TicksToUnits(uint64_t _Ticks_U64, PERF_UNITS _Units_U8);

		static uint64_t UnitsToTicks(uint64_t _Value_U64, PERF_UNITS _Units_U8);

		uint32_t GetCount();

protected:

		uint32_t mNbItems_U32;
		BOF_STAT_VARIABLE<uint64_t> *mpStats_X;

private:
};



#define BOF_PERF_POINT_MAGIC_NUMBER 0x23AF1468
#define BOF_MAX_PERF_POINT 128
#define BOF_PERF_POINT_MGR_NAME_MAX_CHAR 64
#define BOF_PERF_POINT_NAME_MAX_CHAR 256

#define BOF_PERF_POINT_ADD_FUNCTION_LINE(perfpointmgr)   {BofPerfPointAdd(perfpointmgr, "%s at %d\n",__FUNCTION__, __LINE__ );}

typedef struct
{
  uint64_t TimeStampInNs_U64;
  uint64_t Delta_U64;
  char     pPointName_c[BOF_PERF_POINT_NAME_MAX_CHAR];
} BOF_PERF_POINT_ENTRY;

typedef struct
{
  uint32_t             MgrMagicNumber_U32;
  char                 pMgrName_c[BOF_PERF_POINT_MGR_NAME_MAX_CHAR];
  uint64_t             StartTimeStampInNs_U64;
  uint64_t             OverTimeInNs_U64;
  uint32_t             MaxEntry_U32;
  uint32_t             NbEntry_U32;
  uint64_t             MinInNs_U64;
  uint64_t             MaxInNs_U64;
  bool                 Started_B;
  bool                 Triggered_B;
  BOF_PERF_POINT_ENTRY pEntry_X[BOF_MAX_PERF_POINT];
} BOF_PERF_POINT_MGR;

BOF_PERF_POINT_MGR *BofPerfPointOpen(const char *_pName_c, uint32_t _MaxEntry_U32, uint64_t _MaxTimeInNs_U64);
BOFERR BofPerfPointStart(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, bool _ResetTrigger_B);
BOFERR BofPerfPointResetTrigger(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);
BOFERR BofPerfPointAdd(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, const char *_pPointName_c, ...);
BOFERR BofPerfPointStop(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);
BOFERR BofPerfPointClose(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);

END_BOF_NAMESPACE()