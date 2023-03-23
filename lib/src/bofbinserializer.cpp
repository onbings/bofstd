/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof bin serializer class
 *
 * Name:        bofenum.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include "bofstd/bofbinserializer.h"

BEGIN_BOF_NAMESPACE()

IBofBinSerializable::IBofBinSerializable()
{
}
IBofBinSerializable::~IBofBinSerializable()
{
}
BOF_STREAM_ENDIANNESS IBofBinSerializable::SetStreamEndianness(const BOF_STREAM_ENDIANNESS _Endianness_E)
{
  return mBinSerializer.SetStreamEndianness(_Endianness_E);
}

BOF_STREAM_ENDIANNESS IBofBinSerializable::GetStreamEndianness() const
{
  return mBinSerializer.GetStreamEndianness();
}

BofBinSerializer::BofBinSerializer()
{
  SetStreamEndianness(BOF_STREAM_IS_LITTLE_ENDIAN);
}

BofBinSerializer::BofBinSerializer(const BOF_STREAM_ENDIANNESS _Endianness_E)
{
  SetStreamEndianness(_Endianness_E);
}

BofBinSerializer::~BofBinSerializer()
{
}

BOF_STREAM_ENDIANNESS BofBinSerializer::SetStreamEndianness(const BOF_STREAM_ENDIANNESS _Endianness_E)
{
  BOF_STREAM_ENDIANNESS Rts_E = mEndianness_E;

  BOF_ASSERT(_Endianness_E < BOF_STREAM_ENDIANNESS_IS_UNKNOWN);

  if (_Endianness_E < BOF_STREAM_ENDIANNESS_IS_UNKNOWN)
  {
    mEndianness_E = _Endianness_E;
  }

  return Rts_E;
}

BOF_STREAM_ENDIANNESS BofBinSerializer::GetStreamEndianness() const
{
  return mEndianness_E;
}

bool BofBinSerializer::IsStreamBigEndian() const
{
  bool Rts_B = false;

  Rts_B = (GetStreamEndianness() == BOF_STREAM_IS_BIG_ENDIAN);

  return Rts_B;
}

bool BofBinSerializer::IsStreamLittleEndian() const
{
  bool Rts_B = false;

  Rts_B = (GetStreamEndianness() == BOF_STREAM_IS_LITTLE_ENDIAN);

  return Rts_B;
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_U8   - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(uint8_t _Value_U8, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);

  if (_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(_Value_U8)) <= _Size_U32)
    {
      // Serialize data
      *(uint8_t *)&_pBuffer_U8[_Idx_U32] = _Value_U8;
      Rts_U32 = sizeof(_Value_U8);
    }
  }

  return Rts_U32;
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_S8   - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(int8_t _Value_S8, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((uint8_t)_Value_S8, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_U16  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(uint16_t _Value_U16, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);

  if (_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(_Value_U16)) <= _Size_U32)
    {
      // Serialize data
      *(uint16_t *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? BOF_CPU_TO_BE_16(_Value_U16) : BOF_CPU_TO_LE_16(_Value_U16));
      Rts_U32 = sizeof(_Value_U16);
    }
  }

  return Rts_U32;
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_S16  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(int16_t _Value_S16, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((uint16_t)_Value_S16, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_U32  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(uint32_t _Value_U32, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);

  if (_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(_Value_U32)) <= _Size_U32)
    {
      // Serialize data
      *(uint32_t *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? BOF_CPU_TO_BE_32(_Value_U32) : BOF_CPU_TO_LE_32(_Value_U32));
      Rts_U32 = sizeof(_Value_U32);
    }
  }

  return Rts_U32;
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream

Parameters
  _Value_S32  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer

Returns
  The number of bytes serialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::ToByte(int32_t _Value_S32, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((uint32_t)_Value_S32, _Idx_U32, _pBuffer_U8, _Size_U32);
}

uint32_t BofBinSerializer::ToByte(uint64_t _Value_U64, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);

  if (_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(_Value_U64)) <= _Size_U32)
    {
      // Serialize data
      *(uint64_t *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? BOF_CPU_TO_BE_64(_Value_U64) : BOF_CPU_TO_LE_64(_Value_U64));
      Rts_U32 = sizeof(_Value_U64);
    }
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::ToByte(int64_t _Value_S64, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((uint64_t)_Value_S64, _Idx_U32, _pBuffer_U8, _Size_U32);
}

uint32_t BofBinSerializer::ToByte(char _Value_c, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((int8_t)_Value_c, _Idx_U32, _pBuffer_U8, _Size_U32);
}

uint32_t BofBinSerializer::ToByte(float _Value_f, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  uint32_t Mem_U32;

  memcpy(&Mem_U32, &_Value_f, sizeof(uint32_t));

  return ToByte(Mem_U32, _Idx_U32, _pBuffer_U8, _Size_U32);
}

uint32_t BofBinSerializer::ToByte(bool _Value_B, uint32_t _Idx_U32, uint8_t *_pBuffer_U8, uint32_t _Size_U32) const
{
  return ToByte((uint8_t)(_Value_B ? 1 : 0), _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream

Parameters
  _pValue_U8    - The pointer to where to store the data
  _Idx_U32      - The index in the buffer where to start deserializing
  _pBuffer_U8   - The pointer to the buffer
  _Size_U32     - The size in bytes of the buffer
  _Version_U16  - The version of the serialized data

Returns
  The number of bytes deserialized

  0 means an error occurred

Remarks
  None
*/
uint32_t BofBinSerializer::FromByte(uint8_t *_pValue_U8, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t /*_Version_U16*/)
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);
  BOF_ASSERT(_pValue_U8 != NULL);

  if ((_pBuffer_U8 != NULL) && (_pValue_U8 != NULL))
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(*_pValue_U8)) <= _Size_U32)
    {
      *_pValue_U8 = *(uint8_t *)&_pBuffer_U8[_Idx_U32];
      Rts_U32 = sizeof(*_pValue_U8);
    }
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::FromByte(int8_t *_pValue_S8, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  return FromByte((uint8_t *)_pValue_S8, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

uint32_t BofBinSerializer::FromByte(uint16_t *_pValue_U16, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t /*_Version_U16*/)
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);
  BOF_ASSERT(_pValue_U16 != NULL);

  if ((_pBuffer_U8 != NULL) && (_pValue_U16 != NULL))
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(*_pValue_U16)) <= _Size_U32)
    {
      *_pValue_U16 = (IsStreamBigEndian() ? BOF_BE_TO_CPU_16(*(uint16_t *)&_pBuffer_U8[_Idx_U32]) : BOF_LE_TO_CPU_16(*(uint16_t *)&_pBuffer_U8[_Idx_U32]));
      Rts_U32 = sizeof(*_pValue_U16);
    }
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::FromByte(int16_t *_pValue_S16, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  return FromByte((uint16_t *)_pValue_S16, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

uint32_t BofBinSerializer::FromByte(uint32_t *_pValue_U32, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t /*_Version_U16*/)
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);
  BOF_ASSERT(_pValue_U32 != NULL);

  if ((_pBuffer_U8 != NULL) && (_pValue_U32 != NULL))
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(*_pValue_U32)) <= _Size_U32)
    {
      *_pValue_U32 = (IsStreamBigEndian() ? BOF_BE_TO_CPU_32(*(uint32_t *)&_pBuffer_U8[_Idx_U32]) : BOF_LE_TO_CPU_32(*(uint32_t *)&_pBuffer_U8[_Idx_U32]));
      Rts_U32 = sizeof(*_pValue_U32);
    }
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::FromByte(int32_t *_pValue_S32, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  return FromByte((uint32_t *)_pValue_S32, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

uint32_t BofBinSerializer::FromByte(uint64_t *_pValue_U64, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t /*_Version_U16*/)
{
  uint32_t Rts_U32 = 0;

  BOF_ASSERT(_pBuffer_U8 != NULL);
  BOF_ASSERT(_pValue_U64 != NULL);

  if ((_pBuffer_U8 != NULL) && (_pValue_U64 != NULL))
  {
    // Avoid buffer overflow
    if ((_Idx_U32 + sizeof(*_pValue_U64)) <= _Size_U32)
    {
      *_pValue_U64 = (IsStreamBigEndian() ? BOF_BE_TO_CPU_64(*(uint64_t *)&_pBuffer_U8[_Idx_U32]) : BOF_LE_TO_CPU_64(*(uint64_t *)&_pBuffer_U8[_Idx_U32]));
      Rts_U32 = sizeof(*_pValue_U64);
    }
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::FromByte(int64_t *_pValue_S64, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  return FromByte((uint64_t *)_pValue_S64, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

uint32_t BofBinSerializer::FromByte(char *_pValue_c, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  return FromByte((int8_t *)_pValue_c, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

uint32_t BofBinSerializer::FromByte(float *_pValue_f, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  uint32_t Mem_U32;
  uint32_t Rts_U32;

  Rts_U32 = FromByte(&Mem_U32, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);

  if (Rts_U32 == sizeof(uint32_t))
  {
    memcpy(_pValue_f, &Mem_U32, sizeof(uint32_t));
  }

  return Rts_U32;
}

uint32_t BofBinSerializer::FromByte(bool *_pValue_B, uint32_t _Idx_U32, const uint8_t *_pBuffer_U8, uint32_t _Size_U32, uint16_t _Version_U16)
{
  uint8_t Value_U8 = 0;
  uint32_t Rts_U32 = 0;

  if (_pValue_B != NULL)
  {
    Rts_U32 = FromByte(&Value_U8, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
    *_pValue_B = (Value_U8 != 0);
  }

  return Rts_U32;
}

END_BOF_NAMESPACE()