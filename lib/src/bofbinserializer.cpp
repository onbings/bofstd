/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof enum class
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

/*** Include files ***********************************************************/

BEGIN_BOF_NAMESPACE()

/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Remarks
  
  Name:			        Serializer.cpp
  Author:		        Nicolas Marique (NMA)

  Summary:
    
    A custom utility class for serializing data

  History:
    V 1.00  September 16 2013  NMA : Original version
*/

/*** Include ***********************************************************************************************************************/
#if defined(G_N_S)
#include <cdxcore/Serializer.h>
#include <cdxcore/endianness.h>
#include <tgedef/xt_wrapper.h>
BEGIN_CDXCORE_NAMESPACE();
#else
#include "Serializer.h"
#include "endianness.h"
#include "Assertion.h"
#include <stdlib.h>
#include <string.h>
#endif

/*** Defines ***********************************************************************************************************************/

#define MAKE_VERSION(MAJOR, MINOR)  (((U16)(MAJOR) << 8) | (U16)(MINOR))

/*** Enums *************************************************************************************************************************/

/*** Structures ********************************************************************************************************************/

/*** Constants *********************************************************************************************************************/

/*** Globals ***********************************************************************************************************************/

/*** Prototypes ********************************************************************************************************************/

/*** Classes ***********************************************************************************************************************/

/*!
Description
  The class constructor
  
Parameters
  None
  
Returns
  Nothing
   
Remarks
  None
*/
ICSerializable::ICSerializable()
{

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
*/
ICSerializable::~ICSerializable()
{

}

/*!
Description
  This function specifies the endianness
  of the underlying stream
  
Parameters
  _Endianness_E - The endianness of the underlying stream
  
Returns
  The previously configured value
   
Remarks
  None
*/
STREAM_ENDIANNESS ICSerializable::SetStreamEndianness(const STREAM_ENDIANNESS _Endianness_E)
{
  return mSerializer_O.SetStreamEndianness(_Endianness_E);
}

/*!
Description
  This function retrieves the endianness
  of the underlying stream
  
Parameters
  None
  
Returns
  The configured value of the endianness
  of the underlying stream
   
Remarks
  None
*/
STREAM_ENDIANNESS ICSerializable::GetStreamEndianness() const
{
  return mSerializer_O.GetStreamEndianness();
}

/*!
Description
  The class constructor
  
Parameters
  None
  
Returns
  Nothing
   
Remarks
  None
*/
CSerializer::CSerializer()
{
  SetStreamEndianness(STREAM_IS_LITTLE_ENDIAN);
}

/*!
Description
  The class constructor
  
Parameters
  _Endianness_E - The endianness of the underlying stream
  
Returns
  Nothing
   
Remarks
  None
*/
CSerializer::CSerializer(const STREAM_ENDIANNESS _Endianness_E)
{
  SetStreamEndianness(_Endianness_E);
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
*/
CSerializer::~CSerializer()
{

}

/*!
Description
  This function specifies the endianness
  of the underlying stream
  
Parameters
  _Endianness_E - The endianness of the underlying stream
  
Returns
  The previously configured value
   
Remarks
  None
*/
STREAM_ENDIANNESS CSerializer::SetStreamEndianness(const STREAM_ENDIANNESS _Endianness_E)
{
  STREAM_ENDIANNESS Ret_E = mEndianness_E;

  ASSERTION(_Endianness_E < STREAM_ENDIANNESS_IS_UNKNOWN);

  if(_Endianness_E < STREAM_ENDIANNESS_IS_UNKNOWN)
  {
    mEndianness_E = _Endianness_E;
  }

  return Ret_E;
}

/*!
Description
  This function retrieves the endianness
  of the underlying stream
  
Parameters
  None
  
Returns
  The configured value of the endianness
  of the underlying stream
   
Remarks
  None
*/
STREAM_ENDIANNESS CSerializer::GetStreamEndianness() const
{
  return mEndianness_E;
}

/*!
Description
  This function indicates if 
  the stream is big endian
  
Parameters
  None
  
Returns
  TRUE  - The stream is big endian
  FALSE - The stream is not big endian
   
Remarks
  None
*/

bool CSerializer::IsStreamBigEndian() const
{
  bool Ret_B = false;

  Ret_B = (GetStreamEndianness() == STREAM_IS_BIG_ENDIAN);

  return Ret_B;
}

/*!
Description
  This function indicates if 
  the stream is little endian
  
Parameters
  None
  
Returns
  TRUE  - The stream is little endian
  FALSE - The stream is not little endian
   
Remarks
  None
*/
bool CSerializer::IsStreamLittleEndian() const
{
  bool Ret_B = false;

  Ret_B = (GetStreamEndianness() == STREAM_IS_LITTLE_ENDIAN);

  return Ret_B;
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
U32 CSerializer::ToByte(U8 _Value_U8, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  
  if(_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(_Value_U8)) <= _Size_U32)
    {
      // Serialize data
      *(U8 *)&_pBuffer_U8[_Idx_U32] =        _Value_U8;
      Ret_U32                       = sizeof(_Value_U8);
    }
  }
  
  return Ret_U32;
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
U32 CSerializer::ToByte(S8 _Value_S8, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((U8)_Value_S8, _Idx_U32, _pBuffer_U8, _Size_U32);
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
U32 CSerializer::ToByte(U16 _Value_U16, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  
  if(_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(_Value_U16)) <= _Size_U32)
    {
      // Serialize data
      *(U16 *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? CPU_TO_BE16(_Value_U16) : CPU_TO_LE16(_Value_U16));
      Ret_U32                        = sizeof(_Value_U16);
    }
  }
  
  return Ret_U32;
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
U32 CSerializer::ToByte(S16 _Value_S16, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((U16)_Value_S16, _Idx_U32, _pBuffer_U8, _Size_U32);
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
U32 CSerializer::ToByte(U32 _Value_U32, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  
  if(_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(_Value_U32)) <= _Size_U32)
    {
      // Serialize data
      *(U32 *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? CPU_TO_BE32(_Value_U32) : CPU_TO_LE32(_Value_U32));
      Ret_U32                        = sizeof(_Value_U32);
    }
  }
  
  return Ret_U32;
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
U32 CSerializer::ToByte(S32 _Value_S32, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((U32)_Value_S32, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream
  
Parameters
  _Value_U64  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer
  
Returns
  The number of bytes serialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::ToByte(U64 _Value_U64, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  
  if(_pBuffer_U8 != NULL)
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(_Value_U64)) <= _Size_U32)
    {
      // Serialize data
      *(U64 *)&_pBuffer_U8[_Idx_U32] = (IsStreamBigEndian() ? CPU_TO_BE64(_Value_U64) : CPU_TO_LE64(_Value_U64));
      Ret_U32                        = sizeof(_Value_U64);
    }
  }
  
  return Ret_U32;
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream
  
Parameters
  _Value_S64  - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer
  
Returns
  The number of bytes serialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::ToByte(S64 _Value_S64, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((U64)_Value_S64, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream
  
Parameters
  _Value_c    - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer
  
Returns
  The number of bytes serialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::ToByte(char _Value_c,   U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((S8)_Value_c, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream
  
Parameters
  _Value_f    - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer
  
Returns
  The number of bytes serialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::ToByte(float _Value_f, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  U32 Mem_U32;

  memcpy(&Mem_U32, &_Value_f, sizeof(U32));

  return ToByte(Mem_U32, _Idx_U32, _pBuffer_U8, _Size_U32);
}

/*!
Description
  This function serializes the specified
  data value to the given buffer stream
  
Parameters
  _Value_B    - The data to serialize
  _Idx_U32    - The index in the buffer where to start serializing
  _pBuffer_U8 - The pointer to the buffer
  _Size_U32   - The size in bytes of the buffer
  
Returns
  The number of bytes serialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::ToByte(bool _Value_B, U32 _Idx_U32, U8 * _pBuffer_U8, U32 _Size_U32) const
{
  return ToByte((U8)(_Value_B ? 1 : 0), _Idx_U32, _pBuffer_U8, _Size_U32);
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
U32 CSerializer::FromByte(U8 * _pValue_U8,  U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 /*_Version_U16*/)
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  ASSERTION(_pValue_U8  != NULL);
  
  if((_pBuffer_U8 != NULL) && (_pValue_U8 != NULL))
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(*_pValue_U8)) <= _Size_U32)
    {
      *_pValue_U8 = *(U8 *)&_pBuffer_U8[_Idx_U32];
      Ret_U32     = sizeof(*_pValue_U8);
    }
  }
  
  return Ret_U32;
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_S8   - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/ 
U32 CSerializer::FromByte(S8 * _pValue_S8,  U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  return FromByte((U8 *)_pValue_S8, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_U16  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/ 
U32 CSerializer::FromByte(U16 * _pValue_U16, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 /*_Version_U16*/)
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  ASSERTION(_pValue_U16 != NULL);
  
  if((_pBuffer_U8 != NULL) && (_pValue_U16 != NULL))
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(*_pValue_U16)) <= _Size_U32)
    {
      *_pValue_U16 = (IsStreamBigEndian() ? BE_TO_CPU16(*(U16 *)&_pBuffer_U8[_Idx_U32]) : LE_TO_CPU16(*(U16 *)&_pBuffer_U8[_Idx_U32]));
      Ret_U32      = sizeof(*_pValue_U16);
    }
  }
  
  return Ret_U32;
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_S16  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/ 
U32 CSerializer::FromByte(S16 * _pValue_S16, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  return FromByte((U16 *)_pValue_S16, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_U32  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/ 
U32 CSerializer::FromByte(U32 * _pValue_U32, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 /*_Version_U16*/)
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  ASSERTION(_pValue_U32 != NULL);
  
  if((_pBuffer_U8 != NULL) && (_pValue_U32 != NULL))
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(*_pValue_U32)) <= _Size_U32)
    {
      *_pValue_U32 = (IsStreamBigEndian() ? BE_TO_CPU32(*(U32 *)&_pBuffer_U8[_Idx_U32]) : LE_TO_CPU32(*(U32 *)&_pBuffer_U8[_Idx_U32]));
      Ret_U32      = sizeof(*_pValue_U32);
    }
  }
  
  return Ret_U32;
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_S32  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/ 
U32 CSerializer::FromByte(S32 * _pValue_S32, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  return FromByte((U32 *)_pValue_S32, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_U64  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::FromByte(U64 * _pValue_U64, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 /*_Version_U16*/)
{
  U32 Ret_U32 = 0;
  
  ASSERTION(_pBuffer_U8 != NULL);
  ASSERTION(_pValue_U64 != NULL);
  
  if((_pBuffer_U8 != NULL) && (_pValue_U64 != NULL))
  {
    // Avoid buffer overflow
    if((_Idx_U32 + sizeof(*_pValue_U64)) <= _Size_U32)
    {
      *_pValue_U64 = (IsStreamBigEndian() ? BE_TO_CPU64(*(U64 *)&_pBuffer_U8[_Idx_U32]) : LE_TO_CPU64(*(U64 *)&_pBuffer_U8[_Idx_U32]));
      Ret_U32      = sizeof(*_pValue_U64);
    }
  }
  
  return Ret_U32;
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_S64  - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::FromByte(S64 * _pValue_S64, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  return FromByte((U64 *)_pValue_S64, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_c    - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::FromByte(char * _pValue_c, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  return FromByte((S8 *)_pValue_c, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16); 
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_f    - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::FromByte(float * _pValue_f, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  U32 Mem_U32;
  U32 Ret_U32;

  Ret_U32 = FromByte(&Mem_U32, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16); 
  
  if(Ret_U32 == sizeof(U32))
  {
    memcpy(_pValue_f, &Mem_U32, sizeof(U32));
  }

  return Ret_U32;  
}

/*!
Description
  This function deserializes the specified
  data value from the given buffer stream
  
Parameters
  _pValue_i    - The pointer to where to store the data
  _Idx_U32     - The index in the buffer where to start deserializing
  _pBuffer_U8  - The pointer to the buffer
  _Size_U32    - The size in bytes of the buffer
  _Version_U16 - The version of the serialized data
  
Returns
  The number of bytes deserialized
   
  0 means an error occurred
   
Remarks
  None
*/
U32 CSerializer::FromByte(bool * _pValue_B, U32 _Idx_U32, const U8 * _pBuffer_U8, U32 _Size_U32, U16 _Version_U16)
{
  U8  Value_U8  = 0;
  U32 Ret_U32   = 0;

  if(_pValue_B != NULL)
  {
    Ret_U32    = FromByte(&Value_U8, _Idx_U32, _pBuffer_U8, _Size_U32, _Version_U16);  
    *_pValue_B = (Value_U8 != 0);
  }

  return Ret_U32;
}

#if defined(G_N_S)
END_CDXCORE_NAMESPACE();
#else
#endif

END_BOF_NAMESPACE()