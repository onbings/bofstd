/*!
   Copyright (c) 2008, EVS. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   Remarks

   Name:              BofStatistics.h
   Author:            Nicolas Marique

   Summary:

   General purpose statistics functions and macros (Max, min, mean, etc, ...)

   History:
   V 1.00  Octobre    21 2008  NMA : First Release
 */

/*** Include ***********************************************************************************************************************/

#include <bofstd/bofstatistics.h>

BEGIN_BOF_NAMESPACE()

/*** Globals ***********************************************************************************************************************/

/*** Defines ***********************************************************************************************************************/

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Constants *********************************************************************************************************************/

/*** UpdateMean ********************************************************************************************************************/
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
void Bof_UpdateStatMean(BOF_STAT_VARIABLE *_pStatVar_X)
{
	uint64_t TempAccumulator_U64;

	if (_pStatVar_X)
	{
		if (_pStatVar_X->Current_U64 != 0)
		{
			// On accumule la valeur dans une variable temporaire
			TempAccumulator_U64 = _pStatVar_X->MeanAccumulator_U64 + _pStatVar_X->Current_U64;

			// Il n'y aura pas de "roll-over"
			if (TempAccumulator_U64 >= _pStatVar_X->MeanAccumulator_U64)
			{
				_pStatVar_X->MeanAccumulator_U64 = TempAccumulator_U64;
				_pStatVar_X->NbItemsAccumulated_U64++;
			}
				// Si on ajoute cette valeur on va provoquer
				// un "roll-over" de notre accumulateur
			else
			{
				// On relance l'accumulateur � la valeur de la moyenne actuelle
				_pStatVar_X->MeanAccumulator_U64 = _pStatVar_X->Mean_U64;
				_pStatVar_X->NbItemsAccumulated_U64 = 1;
			}

			// On met � jour la moyenne
			_pStatVar_X->Mean_U64 = (_pStatVar_X->MeanAccumulator_U64 / _pStatVar_X->NbItemsAccumulated_U64);
		}
	}
}

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
void Bof_InitStatVar(BOF_STAT_VARIABLE *_pStatVar_X)
{
	if (_pStatVar_X != 0)
	{
		_pStatVar_X->Reset();
		_pStatVar_X->Min_U64 = (uint64_t) -1;
	}
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
void Bof_UpdateStatVar(uint64_t _Val_U64, BOF_STAT_VARIABLE *_pStatVar_X)
{
	if (_pStatVar_X != 0)
	{
		_pStatVar_X->Current_U64 = _Val_U64;
		_pStatVar_X->NbTotal_U64 += 1;

		BOF_SET_NEW_STAT_MIN(_pStatVar_X->Current_U64, _pStatVar_X->Min_U64);
		BOF_SET_NEW_STAT_MAX(_pStatVar_X->Current_U64, _pStatVar_X->Max_U64);
		Bof_UpdateStatMean(_pStatVar_X);
	}
}
END_BOF_NAMESPACE()