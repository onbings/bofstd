/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bit manipulation functions
 *
 * Name:        bofbit.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         https://graphics.stanford.edu/~seander/bithacks.html
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */

/*** Include files ***********************************************************/
#include <bofstd/bofbit.h>

BEGIN_BOF_NAMESPACE()
//https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
  static const unsigned char S_pLogTable256_U8[256] =
                               {
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
                                 0xFF, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
                                 LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
                                 LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
                               };

  uint8_t Bof_MostSignificantBitSetPosition(uint64_t _Val_U64) // 32-bit word to find the log of
  {
    uint8_t  Rts_U8;     // Rts_U8 will be log(_Val_U32)
    uint32_t Val_U32, Temp1_U32, Temp2_U32; // temporaries

    if (_Val_U64 == 0)
    {
      Rts_U8 = 0xFF;
    }
    else if (_Val_U64 == 1)
    {
      Rts_U8 = 0;
    }
    else
    {
      Rts_U8  = 0;
      Val_U32 = static_cast<uint32_t>(_Val_U64 >> 32);
      if (Val_U32)  //In upper part ?
      {
        Temp2_U32 = Val_U32 >> 16;
        if (Temp2_U32)
        {
          Temp1_U32 = Temp2_U32 >> 8;
          Rts_U8    = (Temp1_U32) ? static_cast<uint8_t>(56 + S_pLogTable256_U8[Temp1_U32]) : static_cast<uint8_t>(48 + S_pLogTable256_U8[Temp2_U32]);
        }
        else
        {
          Temp1_U32 = Val_U32 >> 8;
          Rts_U8    = (Temp1_U32) ? static_cast<uint8_t>(40 + S_pLogTable256_U8[Temp1_U32]) : static_cast<uint8_t>(32 + S_pLogTable256_U8[Val_U32]);
        }
      }
      else
      {
        Val_U32   = static_cast<uint32_t>(_Val_U64);
        Temp2_U32 = Val_U32 >> 16;
        if (Temp2_U32)
        {
          Temp1_U32 = Temp2_U32 >> 8;
          Rts_U8    = (Temp1_U32) ? static_cast<uint8_t>(24 + S_pLogTable256_U8[Temp1_U32]) : static_cast<uint8_t>(16 + S_pLogTable256_U8[Temp2_U32]);
        }
        else
        {
          Temp1_U32 = Val_U32 >> 8;
          Rts_U8    = (Temp1_U32) ? static_cast<uint8_t>(8 + S_pLogTable256_U8[Temp1_U32]) : static_cast<uint8_t>(S_pLogTable256_U8[Val_U32]);
        }
      }
    }
    return Rts_U8;
  }

  uint8_t Bof_LessSignificantBitSetPosition(uint64_t _Val_U64) // 32-bit word to find the log of
  {
    return Bof_MostSignificantBitSetPosition(_Val_U64 & (~(_Val_U64 - 1)));
  }

  uint8_t Bof_Log2(uint64_t _Val_U64)
  {
    return Bof_MostSignificantBitSetPosition(_Val_U64);
  }

  uint8_t Bof_FindNumberOfBitSet(uint64_t _Val_U64)
  {
    uint8_t Rts_U8;
    for (Rts_U8 = 0; _Val_U64; Rts_U8++)
    {
      _Val_U64 &= (_Val_U64 - 1); // clear the least significant bit set
    }
    return Rts_U8;
  }

  bool Bof_IsAPowerOf2(uint64_t _Val_U64)
  {
    return (_Val_U64 && !(_Val_U64 & (_Val_U64 - 1)));
  }

  uint64_t Bof_NextHighestPowerOf2(uint64_t _Val_U64)
  {
#if 0
    //_Val_U32--;
    _Val_U32 |= _Val_U32 >> 1;
    _Val_U32 |= _Val_U32 >> 2;
    _Val_U32 |= _Val_U32 >> 4;
    _Val_U32 |= _Val_U32 >> 8;
    _Val_U32 |= _Val_U32 >> 16;
    _Val_U32++;
    return _Val_U32;
#endif

    // decrement n (to handle the case when n itself is a power of 2)
    _Val_U64 = _Val_U64 - 1;

    // do till only one bit is left
    while (_Val_U64 & (_Val_U64 - 1))
    {
      _Val_U64 = _Val_U64 & (_Val_U64 - 1);  // unset rightmost bit
    }
    // _Val_U64 is now a power of two (less than _Val_U64)
    // return next power of 2
    return _Val_U64 << 1;

  }
  uint64_t Bof_RoundUpOnMultipleOf(uint64_t _Val_U64, uint64_t _Multiple_U64)
  {
    uint64_t Rts_U64, Remain_U64;

		Remain_U64 = (_Val_U64 % _Multiple_U64);
		if (Remain_U64)
    {
      Rts_U64 = _Val_U64 + (_Multiple_U64 - Remain_U64);
 //  Rts_U64=((_Val_U64 / _Multiple_U64) * _Multiple_U64) + _Multiple_U64;
    }
    else
    {
      Rts_U64 = _Val_U64;
    }
    return Rts_U64;
  }

  bool Bof_IsAligned(uint64_t _Align_U64, uint64_t _Val_U64)
  {
    bool Rts_B;

    if (_Align_U64 == 0)
    {
      Rts_B = true;
    }
    else
    {
      Rts_B = ((_Val_U64 % _Align_U64) == 0) ? true : false;
    }
    return Rts_B;
  }
  bool Bof_IsAligned(uint64_t _Align_U64, void *_pData)
  {
    bool Rts_B;

    if (_Align_U64 == 0)
    {
      Rts_B = true;
    }
    else
    {
      Rts_B = (((uint64_t) _pData % _Align_U64) == 0) ? true : false;
    }
    return Rts_B;
  }

  std::string Bof_BitToString(uint32_t _Value_U32, uint32_t _InsertASpaceEvery_U32)
  {
    char     pToString_c[128];
    uint32_t i_U32, Index_U32, Cpt_U32;

    Cpt_U32=0;
    Index_U32=0;
    for (i_U32 = 0; i_U32 < 32; i_U32++)
    {
      pToString_c[Index_U32++]=(_Value_U32 & 0x80000000) ? '1':'0';
      _Value_U32 <<= 1;
      Cpt_U32++;
      if (Cpt_U32 >= _InsertASpaceEvery_U32)
      {
        Cpt_U32=0;
        pToString_c[Index_U32++]=' ';
      }
    }
    pToString_c[Index_U32]=0;

    return pToString_c;
  }
END_BOF_NAMESPACE()