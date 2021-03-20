/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bit manipulation functions
 *
 * Name:        bofbit.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

#pragma once

/*** Include files ***********************************************************/
#include <bofstd/bofstd.h>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/

uint8_t Bof_MostSignificantBitSetPosition(uint64_t _Val_U64);
uint8_t Bof_LessSignificantBitSetPosition(uint64_t _Val_U64);
uint8_t Bof_Log2(uint64_t _Val_U64);
uint8_t Bof_FindNumberOfBitSet(uint64_t _Val_U64);
bool Bof_IsAPowerOf2(uint64_t _Val_U64);
uint64_t Bof_NextHighestPowerOf2(uint64_t _Val_U64);
uint64_t Bof_RoundUpOnMultipleOf(uint64_t _Val_U64, uint64_t _Multiple_U64);
bool Bof_IsAligned(uint64_t _Align_U64, uint64_t _Val_U64);
bool Bof_IsAligned(uint64_t _Align_U64, void *_pData);
std::string Bof_BitToString(uint32_t _Value_U32, uint32_t _InsertASpaceEvery_U32);

END_BOF_NAMESPACE()