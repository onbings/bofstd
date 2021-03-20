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

/*** Include ***********************************************************************************************************************/

#include <cstdint>
#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()

#define BOF_SET_NEW_STAT_MIN(newval, themin)			{ if ( (newval) < (themin) )  { (themin) = (newval);  } }
#define BOF_SET_NEW_STAT_MAX(newval, themax)      { if ( (newval) > (themax) )  { (themax) = (newval);  } }

template<typename T>
struct BOF_STAT_VARIABLE
{
		T Crt;             /*! Crt value */
		T Min;                 /*! Minimum value */
		T Max;                 /*! Maximum value */
		T Mean;                /*! Mean value */
		T MeanAcc;     /*! Accumulator needed for computing the mean */
		uint32_t LockCount_U32;           /*! +1 for each call to V_EnterBench for example */
		uint32_t NbSample_U32;  /*! Number of items accumulated */
		BOF_STAT_VARIABLE()
		{
			Reset();
		}

		void Reset()
		{
			Crt = 0;
			Min = 0;
			Max = 0;
			Mean = 0;
			MeanAcc = 0;
			LockCount_U32= 0;
			NbSample_U32 = 0;
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
template<typename T>
BOFERR Bof_ResetStatVar(BOF_STAT_VARIABLE<T> &_rStatVar_X)
{
	BOFERR Rts_E;

	Rts_E = BOF_ERR_NO_ERROR;
	_rStatVar_X.Reset();
	//Done in Bof_UpdateStatVar function below	_rStatVar_X.Min = (T(-1) < T(0)) ? -1 : 0;	// static_cast<T>(-1);
	//not so easy as we have sihned or unsigned, float or int,... _rStatVar_X.Min = static_cast<T>(-1);
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
template<typename T>
BOFERR Bof_UpdateStatVar(BOF_STAT_VARIABLE<T> &_rStatVar_X, T _Val)
{
	BOFERR Rts_E;

	Rts_E = BOF_ERR_NO_ERROR;

	_rStatVar_X.Crt = _Val;
	if (_rStatVar_X.NbSample_U32 == 0)
	{
		_rStatVar_X.Min = _Val;
		_rStatVar_X.Max = _Val;
	}
	else
	{
		BOF_SET_NEW_STAT_MIN(_rStatVar_X.Crt, _rStatVar_X.Min);
		BOF_SET_NEW_STAT_MAX(_rStatVar_X.Crt, _rStatVar_X.Max);
	}
	Bof_UpdateStatMean(_rStatVar_X);
	
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
template<typename T>
BOFERR Bof_UpdateStatMean(BOF_STAT_VARIABLE<T> &_rStatVar_X)
{
	BOFERR Rts_E;
	T TempAccumulator;
	bool RollOver_B;

	Rts_E = BOF_ERR_EMPTY;
	if (_rStatVar_X.Crt != 0)
	{
		Rts_E = BOF_ERR_NO_ERROR;

		// On accumule la valeur dans une variable temporaire
		TempAccumulator = _rStatVar_X.MeanAcc + _rStatVar_X.Crt;
		// Il n'y aura pas de "roll-over"
		RollOver_B = (TempAccumulator < _rStatVar_X.MeanAcc);
		if (!RollOver_B)
		{
			_rStatVar_X.MeanAcc = TempAccumulator;
			_rStatVar_X.NbSample_U32++;
			RollOver_B = (_rStatVar_X.NbSample_U32 == 0);
		}
		if (RollOver_B)
		{
			// Si on ajoute cette valeur on va provoquer
			// un "roll-over" de notre accumulateur
			// On relance l'accumulateur a la valeur de la moyenne actuelle
			_rStatVar_X.MeanAcc = _rStatVar_X.Mean;
			_rStatVar_X.NbSample_U32 = 1;
		}

		BOF_ASSERT(_rStatVar_X.NbSample_U32 != 0);
		// On met a jour la moyenne
		_rStatVar_X.Mean = static_cast<T>((_rStatVar_X.MeanAcc / _rStatVar_X.NbSample_U32));
	}
	return Rts_E;
}
END_BOF_NAMESPACE()