/*
 * Copyright (c) 2000-2006, Sci. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines various cryptographic class.
 *    BofCryptoMd5: This one the RFC 1321 from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
 *
 * Name:        BofCrypto.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Revision:    1.0
 *
 * Rem:         https://github.com/vog/sha1/blob/master/sha1.hpp
 *
 * History:
 *
 * V 1.00  Apr 14 2003  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <cstdint>

BEGIN_BOF_NAMESPACE()

/*! MD5 context.
 *
 * These parameters are used to keep track of BofCryptoMd5 object state
 */
struct BOFSTD_EXPORT BOF_MD5_CONTEXT
{
  uint32_t pState_U32[4]; /*! State (ABCD) */
  uint32_t pCount_U32[2]; /*! Number of bits, modulo 2^64 (lsb first) */
  uint8_t pBuffer_U8[64]; /*! Input buffer */
  BOF_MD5_CONTEXT()
  {
    Reset();
  }

  void Reset()
  {
    memset(pState_U32, 0, sizeof(pState_U32));
    memset(pCount_U32, 0, sizeof(pCount_U32));
    memset(pBuffer_U8, 0, sizeof(pBuffer_U8));
  }
};

/*!
 * Class BofCryptoMd5
 *
 * This class defines the interface for the BofCryptoMd5 object.
 */

class BOFSTD_EXPORT BofCryptoMd5
{
private:
  BOF_MD5_CONTEXT mMd5_X;

private:
  void Transform(uint32_t *pState_U32, uint8_t *pBlock_U8);

  void Encode(uint8_t *pOutput_U8, uint32_t *pInput_U32, uint32_t Len_U32);

  void Decode(uint32_t *pOutput_U32, uint8_t *pInput_U8, uint32_t Len_U32);

  void AddData(uint8_t *pInput_U8, uint32_t InputLen_U32);

  void Compute(uint8_t *pDigest_U8, uint32_t *_pNbOut_U32);

public:
  BofCryptoMd5();

  virtual ~BofCryptoMd5();

  bool Hash(uint32_t _NbIn_U32, uint8_t *_pDataIn_U8, uint32_t *_pNbOut_U32, uint8_t *_pDataOut_U8);
};

class BOFSTD_EXPORT BofSha1
{
public:
  BofSha1();
  void Update(const std::string &s);
  void Update(uint32_t _Nb_U32, const void *_pBuffer);
  void Update(std::istream &is);
  std::string Final();
  static std::string S_FromFile(const std::string &_rFilename_S);
  static std::string S_FromBuffer(uint32_t _Nb_U32, const void *_pBuffer);
  static std::string S_FromString(const std::string &_Input_S);

private:
  uint32_t mpDigest_U32[5];
  std::string mBuffer_S;
  uint64_t mTransform_U64;
};
// BOFSTD_EXPORT std::string Bof_ComputeSha1(uint32_t _Nb_U32, const void *_pBuffer);
// BOFSTD_EXPORT std::string Bof_ComputeSha1(const std::string &_Input_S);

END_BOF_NAMESPACE()