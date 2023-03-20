/*
 * Copyright (c) 2015-2025, OnBings All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This module defines the zsdr module of the zsdr library
 *
 * Name:        zsdr_bin_serializer.h
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 26 2015  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

BEGIN_BOF_NAMESPACE()

/*!
 * Check Cpu endianness.
 *
 * \param none
 *
 * \return true if the cpu is little endian.
 *
 * \remark Little Endian byte order is our default binary storage order as it is natively used in most modern cpu.
 */
static uint8_t S_EndiannessCache_U8 = 0xFF;

inline bool Bof_IsCpuLittleEndian()
{
//  printf("in:S_EndiannessCache_U8 = %x adr %p\n", S_EndiannessCache_U8, &S_EndiannessCache_U8);
  if (S_EndiannessCache_U8 == 0xFF)
  {
    int EndianVal_i = 0x69;
    S_EndiannessCache_U8 = (*reinterpret_cast<char *> (&EndianVal_i) == 0x69) ? 0x01 : 0x00;
  }
  //printf("in:S_EndiannessCache_U8 = %x adr %p\n", S_EndiannessCache_U8, &S_EndiannessCache_U8);
  return ((S_EndiannessCache_U8 != 0) ? true : false);
}


/*!
 * Swap or not the byte order of a given argument value.
 *
 * \tparam T scalar type such as uint32_t, double, ...
 *
 * \param _Swap_B true if the byte order must be swapped.
 * \param _ScalarValue_T The scalar value to swap or not.
 *
 * \return The value with its byte order swapped or not depending on _Swap_B parameters.
 *
 * \remark Little Endian byte order is our default binary storage order as it is natively used in most modern cpu.
 */
template<typename T>
T EndianScalar(bool _Swap_B, T _ScalarValue_T)
{
  if (_Swap_B)                         // (!Bof_IsCpuLittleEndian( ) )
  {
#if defined (_MSC_VER)
#pragma push_macro("__builtin_bswap16")
#pragma push_macro("__builtin_bswap32")
#pragma push_macro("__builtin_bswap64")
#define __builtin_bswap16    _byteswap_ushort
#define __builtin_bswap32    _byteswap_ulong
#define __builtin_bswap64    _byteswap_uint64

#pragma warning( push )
#pragma warning( disable : 4127)
#endif

    // If you're on the few remaining big endian platforms, we make the bold
    // assumption you're also on gcc/clang, and thus have bswap intrinsics:
    if (sizeof(T) == 1)                // Compile-time if-then's.
    {
      return _ScalarValue_T;
    }
    else if (sizeof(T) == 2)
    {
      uint16_t Rts = __builtin_bswap16(*reinterpret_cast<uint16_t *> (&_ScalarValue_T));
      return *reinterpret_cast<T *> (&Rts);
    }
    else if (sizeof(T) == 4)
    {
      uint32_t Rts = __builtin_bswap32(*reinterpret_cast<uint32_t *> (&_ScalarValue_T));
      return *reinterpret_cast<T *> (&Rts);
    }
    else if (sizeof(T) == 8)
    {
      uint64_t Rts = __builtin_bswap64(*reinterpret_cast<uint64_t *> (&_ScalarValue_T));
      return *reinterpret_cast<T *> (&Rts);
    }
    else
    {
      BOF_ASSERT(0);
    }
#if defined (_MSC_VER)
#pragma pop_macro("__builtin_bswap16")
#pragma pop_macro("__builtin_bswap32")
#pragma pop_macro("__builtin_bswap64")
#pragma warning( pop )
#endif
  }
  else
  {
    return _ScalarValue_T;
  }
}


/*!
 * Read a scalar value from a given address.
 *
 * \tparam T scalar type such as uint32_t, double, ...
 *
 * \param _pValueAddress specifies the value location to read from.
 * \param _rValue_T return the value read.
 *
 * \return The size in byte of the data read.
 *
 * \remark Little Endian byte order is our default binary storage order as it is natively used in most modern cpu.
 */
template<typename T>
uint32_t ReadScalar(const void *_pValueAddress, T &_rValue_T)
{
  _rValue_T = EndianScalar(!Bof_IsCpuLittleEndian(), *reinterpret_cast<const T *> (_pValueAddress));
  return static_cast<uint32_t> (sizeof(T));
}

/*!
 * Read a Binary Large Object from a given address in native format
 *
 * \tparam T Binary data data, ...
 *
 * \param _pValueAddress specifies the value location to write to.
 * \param _rValue_T specifies the value to write.
 *
 * \return The size in byte of the written data.
 *
 * \remark Data is stored in cpu native endianess.
 */
template<typename T>
uint32_t ReadNativeBlob(const void *_pValueAddress, T &_rValue_T)
{
  uint32_t Rts_U32 = static_cast<uint32_t> (sizeof(T));

  memcpy(&_rValue_T, _pValueAddress, Rts_U32);
  return Rts_U32;
}

/*!
 * Write a scalar value at a given address.
 *
 * \tparam T scalar type such as uint32_t, double, ...
 *
 * \param _pValueAddress specifies the value location to write to.
 * \param _NewValue_T specifies the value to write.
 *
 * \return The size in byte of the written data.
 *
 * \remark Little Endian byte order is our default binary storage order as it is natively used in most modern cpu.
 */
template<typename T>
uint32_t WriteScalar(void *_pValueAddress, T _NewValue_T)
{
  *reinterpret_cast<T *> (_pValueAddress) = EndianScalar(!Bof_IsCpuLittleEndian(), _NewValue_T);
  return static_cast<uint32_t> (sizeof(T));
}

/*!
 * Write a Binary Large Object at a given address in native format
 *
 * \tparam T Binary data data, ...
 *
 * \param _pValueAddress specifies the value location to write to.
 * \param _NewValue_T specifies the value to write.
 *
 * \return The size in byte of the written data.
 *
 * \remark Data is stored in cpu native endianess.
 */
template<typename T>
uint32_t WriteNativeBlob(void *_pValueAddress, T _NewValue_T)
{
  uint32_t Rts_U32 = static_cast<uint32_t> (sizeof(T));
  memcpy(_pValueAddress, &_NewValue_T, Rts_U32);
  return Rts_U32;
}

/*!
 * Skip byte count associated with this data type (which can be different from a scalr)
 *
 * \tparam T object type such as uint32_t, double, ot std:function...
 *
 * \return The size in byte of the data.
 *
 * \remark No data is written in the serialization buffer.
 */
template<typename T>
uint32_t SkipField()
{
  return static_cast<uint32_t> (sizeof(T));
}

/*!
 * Return the alignment, in bytes, of the specified type.
 *
 * \tparam T scalar type such as uint32_t, double, ...
 *
 * \return the alignment, in bytes, of the specified type.
 *
 */
template<typename T>
size_t AlignOf()
{
#ifdef _MSC_VER
  return __alignof(T);

#else
#ifndef alignof
  return __alignof__(T);

#else
  return alignof(T);
#endif
#endif
}

#define BOF_CPU_TO_LE_64(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint64_t>(CPU_VAL))
#define BOF_CPU_TO_LE_32(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint32_t>(CPU_VAL)) 
#define BOF_CPU_TO_LE_16(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint16_t>(CPU_VAL))

#define BOF_CPU_TO_BE_64(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint64_t>(CPU_VAL))
#define BOF_CPU_TO_BE_32(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint32_t>(CPU_VAL)) 
#define BOF_CPU_TO_BE_16(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint16_t>(CPU_VAL))

#define BOF_LE_TO_CPU_64(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint64_t>(CPU_VAL))
#define BOF_LE_TO_CPU_32(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint32_t>(CPU_VAL)) 
#define BOF_LE_TO_CPU_16(CPU_VAL)        BOF::EndianScalar(!BOF::Bof_IsCpuLittleEndian(), static_cast<uint16_t>(CPU_VAL))

#define BOF_BE_TO_CPU_64(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint64_t>(CPU_VAL))
#define BOF_BE_TO_CPU_32(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint32_t>(CPU_VAL)) 
#define BOF_BE_TO_CPU_16(CPU_VAL)        BOF::EndianScalar( BOF::Bof_IsCpuLittleEndian(), static_cast<uint16_t>(CPU_VAL))

END_BOF_NAMESPACE()