/*!
   Copyright (c) 2008, Onbings All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:              BofStatistics.h
   Author:            Bernard HARMEL: onbings@dscloud.me

   Summary:

   General purpose statistics functions and macros (Max, min, mean, etc, ...)

   History:
   V 1.00  Octobre    21 2008  BHA : First Release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <cstdint>
#include <limits>

BEGIN_BOF_NAMESPACE()

template <typename T>
struct BOF_STAT_MAX
{
  uint64_t MaxIndex_U64;
  T Max;
  BOF_STAT_MAX()
  {
    Reset();
  }
  void Reset()
  {
    MaxIndex_U64 = 0xFFFFFFFF;
    Max = std::numeric_limits<T>::min();
  }
};

constexpr uint32_t BOF_STAT_KEEP_LAST_NB_MAX_VAL = 16;
template <typename T>
struct BOF_STAT_VARIABLE
{
  T Crt;                  /*! Crt value */
  T Min;                  /*! Minimum value */
  T Max;                  /*! Maximum value */
  T Mean;                 /*! Mean value */
  T MeanAcc;              /*! Accumulator needed for computing the mean */
  uint64_t LockCount_U64; /*! +1 for each call to EnterBench -1 for each LeaveBench */
  uint64_t NbSample_U64;  /*! Number of items accumulated */
  uint32_t NbMax_U32;
  BOF_STAT_MAX<T> pMax_X[BOF_STAT_KEEP_LAST_NB_MAX_VAL];
  BOF_STAT_VARIABLE()
  {
    Reset();
  }

  void Reset()
  {
    uint32_t i_U32;
    Crt = 0;
    Min = std::numeric_limits<T>::max();
    Max = std::numeric_limits<T>::min();
    Mean = 0;
    MeanAcc = 0;
    LockCount_U64 = 0;
    NbSample_U64 = 0;
    NbMax_U32 = 0;
    for (i_U32 = 0; i_U32 < BOF_STAT_KEEP_LAST_NB_MAX_VAL; i_U32++)
    {
      pMax_X[i_U32].Reset();
    }
  }
};

/*!
Description
Function that initializes the statistical element

Parameters
_pStatVar_X - Pointer on the structure that contains the data of the statistical element

Returns
Nothing

Remarks
Aucune
*/
template <typename T>
BOFERR Bof_ResetStatVar(BOF_STAT_VARIABLE<T> &_rStatVar_X)
{
  BOFERR Rts_E;

  Rts_E = BOF_ERR_NO_ERROR;
  _rStatVar_X.Reset();
  // Done in Bof_UpdateStatVar function below	_rStatVar_X.Min = (T(-1) < T(0)) ? -1 : 0;	// static_cast<T>(-1);
  // not so easy as we have sihned or unsigned, float or int,... _rStatVar_X.Min = static_cast<T>(-1);
  //_rStatVar_X.Max = (T(-1) < T(0)) ? -1 : 0;	// static_cast<T>(-1);

  return Rts_E;
}

/*!
Description
Function that updates the statistical element based on the current value specified

Parameters
_Val_U64    - The current value of the statistical element
_pStatVar_X - Pointer on the structure that contains the data of the statistical element

Returns
Nothing

Remarks
Aucune
*/
template <typename T>
BOFERR Bof_UpdateStatVar(bool _IgnoreFirstSample_B, BOF_STAT_VARIABLE<T> &_rStatVar_X, T _Val)
{
  BOFERR Rts_E;

  Rts_E = BOF_ERR_NO_ERROR;

  _rStatVar_X.Crt = _Val;
  if (_rStatVar_X.NbSample_U64 == 0)
  {
    if (!_IgnoreFirstSample_B)
    {
      _rStatVar_X.Min = _Val;
      _rStatVar_X.Max = _Val;
      _rStatVar_X.pMax_X[0].MaxIndex_U64 = 0;
      _rStatVar_X.pMax_X[0].Max = _Val;
      _rStatVar_X.NbMax_U32 = 1;
    }
  }
  else
  {
    if (_rStatVar_X.Crt < _rStatVar_X.Min)
    {
      _rStatVar_X.Min = _rStatVar_X.Crt;
    }
    if (_rStatVar_X.Crt > _rStatVar_X.Max)
    {
      _rStatVar_X.Max = _rStatVar_X.Crt;
      if (_rStatVar_X.NbMax_U32 < BOF_STAT_KEEP_LAST_NB_MAX_VAL)
      {
        _rStatVar_X.pMax_X[_rStatVar_X.NbMax_U32].MaxIndex_U64 = _rStatVar_X.NbSample_U64;
        _rStatVar_X.pMax_X[_rStatVar_X.NbMax_U32].Max = _rStatVar_X.Crt;
        _rStatVar_X.NbMax_U32++;
      }
    }
  }
  Bof_UpdateStatMean(_IgnoreFirstSample_B, _rStatVar_X);

  return Rts_E;
}
/*!
Description
Function that allows to update the mean of a statistical element

Parameters
_pTimingStat_X - Pointer on the structure that contains the data of the statistical element

Returns
Nothing

Remarks
Aucune
*/
template <typename T>
BOFERR Bof_UpdateStatMean(bool _IgnoreFirstSample_B, BOF_STAT_VARIABLE<T> &_rStatVar_X)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  T TempAccumulator;
  bool RollOver_B;

  // printf("InNb %zd Crt %zd Mean %zd MeanAcc %zd\n", _rStatVar_X.NbSample_U64, _rStatVar_X.Crt, _rStatVar_X.Mean, _rStatVar_X.MeanAcc);
  if (_IgnoreFirstSample_B)
  {
    if (_rStatVar_X.NbSample_U64 == 0)
    {
      _rStatVar_X.NbSample_U64 = 1;
      _rStatVar_X.MeanAcc = 0;
      return Rts_E;
    }
    if (_rStatVar_X.NbSample_U64 == 1)
    {
      _rStatVar_X.MeanAcc = _rStatVar_X.Crt;
    }
  }
  // On accumule la valeur dans une variable temporaire
  TempAccumulator = _rStatVar_X.MeanAcc + _rStatVar_X.Crt;
  // Il n'y aura pas de "roll-over"
  RollOver_B = (TempAccumulator < _rStatVar_X.MeanAcc);
  if (!RollOver_B)
  {
    _rStatVar_X.MeanAcc = TempAccumulator;
    _rStatVar_X.NbSample_U64++;
    RollOver_B = (_rStatVar_X.NbSample_U64 == 0);
  }
  if (RollOver_B)
  {
    // Si on ajoute cette valeur on va provoquer
    // un "roll-over" de notre accumulateur
    // On relance l'accumulateur a la valeur de la moyenne actuelle
    _rStatVar_X.MeanAcc = _rStatVar_X.Mean;
    _rStatVar_X.NbSample_U64 = 1;
  }

  BOF_ASSERT(_rStatVar_X.NbSample_U64 != 0);
  // On met a jour la moyenne
  _rStatVar_X.Mean = static_cast<T>((_rStatVar_X.MeanAcc / _rStatVar_X.NbSample_U64));

  // printf("OutNb %zd Crt %zd Mean %zd MeanAcc %zd\n", _rStatVar_X.NbSample_U64, _rStatVar_X.Crt, _rStatVar_X.Mean, _rStatVar_X.MeanAcc);
  return Rts_E;
}
END_BOF_NAMESPACE()