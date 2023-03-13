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

#include <bofstd/bofstatistics.h>

#include <chrono>
#include <vector>
#include <mutex>
#include <unordered_map>

BEGIN_BOF_NAMESPACE()

#define BOF_ENTER_BENCH(PROFILER, ITEM)    { if ( (PROFILER) != nullptr) { (PROFILER)->V_EnterBench(ITEM); } }
#define BOF_LEAVE_BENCH(PROFILER, ITEM)    { if ( (PROFILER) != nullptr) { (PROFILER)->V_LeaveBench(ITEM); } }

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

typedef enum
{
  BOF_PROFILER_TYPE_NORMAL = 0, //In this order !!!
  BOF_PROFILER_TYPE_OS_AWARE,
  BOF_PROFILER_TYPE_LIGHT,
  BOF_PROFILER_TYPE_MAX
}BOF_PROFILER_TYPE;



namespace TheProfiler
{
  using Clock = std::chrono::steady_clock;

  template<typename duration, typename time_point>
  class Timing
  {
  public:
    Timing() :
      Last_O(duration::zero()),
      Min_O(duration::max()),
      Max_O(duration::min()),
      Acc_O(duration::zero()),
      Count_U32(0)
    {

    }

    ~Timing()
    {

    }

    duration GetLast() const noexcept
    {
      return this->Last_O;
    }

    duration GetMin() const noexcept
    {
      return this->Min_O;
    }

    duration GetMax() const noexcept
    {
      return this->Max_O;
    }

    duration GetMean() const noexcept
    {
      if (this->Count_U32 > 0)
      {
        return this->Acc_O / this->Count_U32;
      }

      return Clock::duration::zero();
    }

    size_t GetCount() const noexcept
    {
      return this->Count_U32;
    }

    void Update(const std::chrono::steady_clock::duration &_Duration_O)
    {
      ++Count_U32;
      Acc_O += _Duration_O;

      this->Last_O = _Duration_O;

      if (_Duration_O < this->Min_O) { this->Min_O = _Duration_O; }
      if (_Duration_O > this->Max_O) { this->Max_O = _Duration_O; }
    }

  protected:

    time_point Time_O;

    duration   Last_O;
    duration   Min_O;
    duration   Max_O;
    duration   Acc_O;
    size_t     Count_U32;
  };

  template<typename Clock>
  class Measurer
  {
  public:

    using _Timing = Timing<typename Clock::duration, typename Clock::time_point>;

    Measurer()
    {

    }

    virtual ~Measurer() {}

    void Start()
    {
      this->GetTimeStampVar() = Clock::now();
    }

    void Stop()
    {
      this->Timing_O.Update(Clock::now() - this->GetTimeStampVar());
    }

    void Update(const std::chrono::steady_clock::duration &_Duration_O)
    {
      this->Timing_O.Update(_Duration_O);
    }

    void Reset()
    {
      this->Timing_O = _Timing();
    }

    const _Timing &GetTiming() const noexcept
    {
      return this->Timing_O;
    }

  private:

    /*!
    Description
      This function returns the time_point for the current thread
      Doing so the profiler can work even for reentrant functions
      e.g. If multiple thread calls the same function

      void f()
      {
        profiler.start();
        // Do things        // T1 is preempted by T2 that calls again f()
        profiler.stop ();
      }
    */
    typename Clock::time_point &GetTimeStampVar()
    {
      // The map is unique for each thread but is static (because of thread_local)
      static thread_local std::unordered_map<Measurer<Clock> *, typename Clock::time_point> TimeStamps_O;

      return TimeStamps_O[this];
    }

    _Timing Timing_O;
  };

  struct PerThreadClock
  {
    using Clock = std::chrono::steady_clock;
    using rep = Clock::rep;
    using period = Clock::period;
    using duration = Clock::duration;
    using time_point = Clock::time_point;
    static constexpr bool is_steady = Clock::is_steady;
    static duration Duration_O;

    static time_point now() noexcept;
  };

  template<typename Clock, typename Mutex>
  class BofExtendedProfiler
  {
  public:

    using _Timing = Timing<typename Clock::duration, typename Clock::time_point>;

    /*!
    Description
      The class constructor

    Parameters
      _NbItems_U32 - The number of items to profile
    */
    BofExtendedProfiler(uint32_t _NbItems_U32)
    {
      this->pItems_O.resize(_NbItems_U32);
    }

    virtual ~BofExtendedProfiler()
    {

    }

    /*!
    Description
      This function starts the measurement for the given item ID

    Parameters
      _ItemId_U32 - The item ID

    Returns
      None
    */
    void V_EnterBench(uint32_t _ItemId_U32)
    {
      if (_ItemId_U32 < this->pItems_O.size())
      {
        std::lock_guard<Mutex> Lock_O(this->Mutex_O);

        this->pItems_O[_ItemId_U32].Absolute_O.Start();
        this->pItems_O[_ItemId_U32].PerThread_O.Start();
      }
    }

    /*!
    Description
    This function stops the measurement for the given item ID

    Parameters
      _ItemId_U32 - The item ID

    Returns
     None
    */
    void V_LeaveBench(uint32_t _ItemId_U32)
    {
      if (_ItemId_U32 < this->pItems_O.size())
      {
        std::lock_guard<Mutex> Lock_O(this->Mutex_O);

        this->pItems_O[_ItemId_U32].PerThread_O.Stop();
        this->pItems_O[_ItemId_U32].Absolute_O.Stop();
      }
    }

    void AddValue(uint32_t _ItemId_U32, uint64_t _Value_U64)
    {
      if (_ItemId_U32 < this->pItems_O.size())
      {
        std::chrono::steady_clock::duration Duration_O(_Value_U64);
        this->pItems_O[_ItemId_U32].Absolute_O.Update(Duration_O);
        this->pItems_O[_ItemId_U32].PerThread_O.Update(Duration_O);
      }
    }

    /*!
    Description
      This function resets the measurement for the given item ID

    Parameters
      _ItemId_U32 - The item ID

    Returns
      None
    */
    void Reset(uint32_t _ItemId_U32)
    {
      if (_ItemId_U32 < this->pItems_O.size())
      {
        std::lock_guard<Mutex> Lock_O(this->Mutex_O);

        this->pItems_O[_ItemId_U32].Absolute_O.Reset();
        this->pItems_O[_ItemId_U32].PerThread_O.Reset();
      }
    }

    /*!
    Description
      This function resets the measurement
      for all items

    Parameters
      None

    Returns
      None
    */
    void ResetAll()
    {
      std::lock_guard<Mutex> Lock_O(this->Mutex_O);

      for (uint32_t i = 0; i < this->pItems_O.size(); i++)
      {
        this->pItems_O[i].Absolute_O.Reset();
        this->pItems_O[i].PerThread_O.Reset();
      }
    }

    /*!
    Description
      This function returns the profiler timing for the specified item and category

    Parameters
      _ItemId_U32 - The item ID
      _Idx        - The profiler category

    Returns
     The associated timing
    */
    _Timing GetItem(uint32_t _Id_U32, size_t _Idx) const
    {
      std::lock_guard<Mutex> Lock_O(this->Mutex_O);

      if (_Id_U32 < this->pItems_O.size())
      {
        if (_Idx == 0)
        {
          return this->pItems_O[_Id_U32].Absolute_O.GetTiming();
        }
        if (_Idx == 1)
        {
          return this->pItems_O[_Id_U32].PerThread_O.GetTiming();
        }
      }

      return _Timing();
    }

  private:

    struct Item
    {
      Measurer<Clock>          Absolute_O;
      Measurer<PerThreadClock> PerThread_O;
    };

    mutable Mutex     Mutex_O;
    std::vector<Item> pItems_O;
  };
}

class BOFSTD_EXPORT BofProfiler
{
public:

  BofProfiler(BOF_PROFILER_TYPE _ProfilerType_E, uint32_t _NbItems_U32);
  virtual ~BofProfiler();

  virtual void V_EnterBench(uint32_t _ItemId_U32);
  virtual void V_LeaveBench(uint32_t _ItemId_U32);

  bool SetStats(uint32_t _ItemId_U32, uint64_t _Value_U64);
  bool GetStats(uint32_t _ItemId_U32, BOF_STAT_VARIABLE<uint64_t> *_pStats_X);
  void ResetStats(uint32_t _ItemId_U32);

  static  uint64_t ToProfileValue(std::chrono::steady_clock::duration _Val_O);
  static uint64_t TicksToUnits(uint64_t _Ticks_U64, PERF_UNITS _Units_U8);
  static uint64_t UnitsToTicks(uint64_t _Value_U64, PERF_UNITS _Units_U8);

  uint64_t GetMin(uint32_t _ItemId_U32);
  uint64_t GetMax(uint32_t _ItemId_U32);
  uint64_t GetMean(uint32_t _ItemId_U32);
  uint64_t GetLast(uint32_t _ItemId_U32);

  uint32_t GetCount();

protected:

  using ExtendedProfiler = TheProfiler::BofExtendedProfiler<std::chrono::steady_clock, std::mutex>; 
  std::unique_ptr<ExtendedProfiler>  mpuExtendedProfiler = nullptr;  //Used by BOF_PROFILER_TYPE_NORMAL, BOF_PROFILER_TYPE_OS_AWARE
  uint32_t                   mNbItems_U32 = 0;
  BOF_STAT_VARIABLE<uint64_t> *mpStats_X=nullptr; //used by BOF_PROFILER_TYPE_LIGHT

private:
  BOF_PROFILER_TYPE mProfilerType_E = BOF_PROFILER_TYPE::BOF_PROFILER_TYPE_MAX;
};

#define BOF_PERF_POINT_MAGIC_NUMBER 0x23AF1468
#define BOF_MAX_PERF_POINT 128
#define BOF_PERF_POINT_MGR_NAME_MAX_CHAR 64
#define BOF_PERF_POINT_NAME_MAX_CHAR 256

#define BOF_PERF_POINT_ADD_FUNCTION_LINE(perfpointmgr)   {BofPerfPointAdd(perfpointmgr, "%s at %d\n",__func__, __LINE__ );}

struct BOFSTD_EXPORT BOF_PERF_POINT_ENTRY
{
  uint64_t TimeStampInNs_U64;
  uint64_t Delta_U64;
  char     pPointName_c[BOF_PERF_POINT_NAME_MAX_CHAR];
};

struct BOFSTD_EXPORT BOF_PERF_POINT_MGR
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
};

BOFSTD_EXPORT BOF_PERF_POINT_MGR *BofPerfPointOpen(const char *_pName_c, uint32_t _MaxEntry_U32, uint64_t _MaxTimeInNs_U64);
BOFSTD_EXPORT BOFERR BofPerfPointStart(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, bool _ResetTrigger_B);
BOFSTD_EXPORT BOFERR BofPerfPointResetTrigger(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);
BOFSTD_EXPORT BOFERR BofPerfPointAdd(BOF_PERF_POINT_MGR *_pPerfPointMgr_X, const char *_pPointName_c, ...);
BOFSTD_EXPORT BOFERR BofPerfPointStop(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);
BOFSTD_EXPORT BOFERR BofPerfPointClose(BOF_PERF_POINT_MGR *_pPerfPointMgr_X);

END_BOF_NAMESPACE()