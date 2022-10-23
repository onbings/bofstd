/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bofguid lass
 *
 * Name:        bofguid.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         None
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include <bofstd/bofguid.h>
#include <czmq.h>

BEGIN_BOF_NAMESPACE()
// Opaque pointer design pattern: defined privately here all private data and functions: all of these can now change without recompiling callers ...
class BofGuid::BofGuidImplementation
{
public:
  zuuid_t *mpUuid;
  bool mGuidValid_B;

  BofGuidImplementation()
  {
    mpUuid = zuuid_new();
    mGuidValid_B = true;
  }

  BofGuidImplementation(const uint8_t *_pData_U8)
  {
    mpUuid = zuuid_new();
    if (_pData_U8)
    {
      zuuid_set(mpUuid, _pData_U8);
    }
    mGuidValid_B = true;
  }

  BofGuidImplementation(const std::vector<uint8_t> &_rData)
  {
    mpUuid = zuuid_new();
    mGuidValid_B = (_rData.size() >= Size());
    zuuid_set(mpUuid, _rData.data());
  }

  BofGuidImplementation(const std::string &_rUuidStr_S)  //_rUuidStr_S connonical or not
  {
    int Sts_i;

    mpUuid = zuuid_new();
    Sts_i = zuuid_set_str(mpUuid, _rUuidStr_S.c_str());
    mGuidValid_B = (Sts_i == 0);
  }

  ~BofGuidImplementation()
  {
    zuuid_destroy(&mpUuid);
  }

  size_t Size() const
  {
    return mpUuid ? zuuid_size(mpUuid) : 0;
  }

  const std::vector<uint8_t> Data() const
  {
    std::vector<uint8_t> Rts;

    if (mpUuid)
    {
      const uint8_t *pData_U8 = zuuid_data(mpUuid);
      Rts = std::vector<uint8_t>(pData_U8, pData_U8 + Size());
    }
    return Rts;
  }

  const uint8_t *Data(size_t &_rSize) const
  {
    const uint8_t *pRts_U8 = mpUuid ? zuuid_data(mpUuid) : nullptr;
    _rSize = Size();
    return pRts_U8;
  }

  std::string ToString(bool _Cannonical_B) const
  {
    std::string Rts_S;

    if (mpUuid)
    {
      if (_Cannonical_B)
      {
        Rts_S = zuuid_str_canonical(mpUuid);    //6ba39f88-ed13-8c48-a765-a8738dba383e
      }
      else
      {
        Rts_S = zuuid_str(mpUuid);          //6BA39F88ED138C48A765A8738DBA383E
      }
    }
    return Rts_S;
  }

  void OperatorAssign(const BofGuidImplementation *_pOther)
  {
    if (_pOther)
    {
      zuuid_destroy(&mpUuid);
      mpUuid = zuuid_dup(_pOther->mpUuid);
      //  mpUuid = mpUuid ? zuuid_dup(_pOther->mpUuid) : nullptr;
    }
  }

  void OperatorAssign(const char *_pUuidStr_c)
  {
    mpUuid ? zuuid_set_str(mpUuid, _pUuidStr_c) : -1;
  }

  bool OperatorEqual(const BofGuidImplementation *_pOther) const
  {
    bool Rts_B = false;
    size_t Size;
    if ((_pOther) && (mpUuid))
    {
      const uint8_t *pData_U8 = _pOther->Data(Size);
      Rts_B = zuuid_eq(mpUuid, pData_U8);
    }
    return Rts_B;
  }

  bool OperatorLower(const BofGuidImplementation *_pOther) const
  {
    bool Rts_B = false;

    if (_pOther)
    {
      Rts_B = ToString(true) < _pOther->ToString(true);
    }
    return Rts_B;
  }

  bool IsValid() const
  {
    return mGuidValid_B;
  }

  void Clear()
  {
    //		size_t t = Size();	//16
    uint8_t pData_U8[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    zuuid_set(mpUuid, pData_U8);
  }

  bool IsNull() const
  {
    size_t i, Len;
    const uint8_t *pData_U8 = Data(Len);
    for (i = 0; i < Len; i++)
    {
      if (pData_U8[i])
      {
        break;
      }
    }
    return (i == Len);
  }
};

// Opaque pointer design pattern: ... set Implementation values ...
BofGuid::BofGuid()
  : mpuBofGuidImplementation(new BofGuidImplementation())
{}

BofGuid::BofGuid(const uint8_t *_pData_U8)
  : mpuBofGuidImplementation(new BofGuidImplementation(_pData_U8))
{}

BofGuid::BofGuid(const std::vector<uint8_t> &_rData)
  : mpuBofGuidImplementation(new BofGuidImplementation(_rData))
{}

BofGuid::BofGuid(const std::string &_rUuidStr_S)
  : mpuBofGuidImplementation(new BofGuidImplementation(_rUuidStr_S))
{}

BofGuid::BofGuid(const BofGuid &_rOther)
  : mpuBofGuidImplementation(new BofGuidImplementation(_rOther.Data()))
{}

BofGuid::BofGuid(BofGuid &&_rrOther)
  : mpuBofGuidImplementation(new BofGuidImplementation(_rrOther.Data()))
{}

BofGuid::~BofGuid()
{}

const std::vector<uint8_t> BofGuid::Data() const
{
  return mpuBofGuidImplementation->Data();
}

const uint8_t *BofGuid::Data(size_t &_rSize) const
{
  return mpuBofGuidImplementation->Data(_rSize);
}

std::string BofGuid::ToString(bool _Cannonical_B) const
{
  return mpuBofGuidImplementation->ToString(_Cannonical_B);
}

BofGuid &BofGuid::operator=(const BofGuid &_rOther)
{
  mpuBofGuidImplementation->OperatorAssign(_rOther.mpuBofGuidImplementation.get());
  return *this;
}

BofGuid &BofGuid::operator=(BofGuid &&_rrOther)
{
  if (this != &_rrOther)
  {
    mpuBofGuidImplementation->OperatorAssign(_rrOther.mpuBofGuidImplementation.get());
    zuuid_destroy(&_rrOther.mpuBofGuidImplementation->mpUuid);
    //		BOF_SAFE_DELETE(_rrOther.mpuBofGuidImplementation);
  }
  return *this;
}

BofGuid &BofGuid::operator=(const char *_pUuidStr_c)
{
  mpuBofGuidImplementation->OperatorAssign(_pUuidStr_c);
  return *this;
}

BofGuid &BofGuid::operator=(const std::string &_rUuidStr_S)
{
  mpuBofGuidImplementation->OperatorAssign(_rUuidStr_S.c_str());
  return *this;
}

bool BofGuid::operator<(const BofGuid &_rOther) const
{

  bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
  if (Rts_B)
  {
    Rts_B = false;
  }
  else
  {
    Rts_B = mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
  }
  //  printf("%s < %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::operator<=(const BofGuid &_rOther) const
{
  bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
  if (!Rts_B)
  {
    Rts_B = mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
  }
  //  printf("%s <= %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::operator==(const BofGuid &_rOther) const
{
  bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
  //  printf("%s == %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::operator!=(const BofGuid &_rOther) const
{
  bool Rts_B = !(*this == _rOther);
  //  printf("%s != %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::operator>(const BofGuid &_rOther) const
{
  bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
  if (Rts_B)
  {
    Rts_B = false;
  }
  else
  {
    Rts_B = !mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
  }
  //  printf("%s > %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::operator>=(const BofGuid &_rOther) const
{
  bool Rts_B = mpuBofGuidImplementation->OperatorEqual(_rOther.mpuBofGuidImplementation.get());
  if (!Rts_B)
  {
    Rts_B = !mpuBofGuidImplementation->OperatorLower(_rOther.mpuBofGuidImplementation.get());
  }
  //  printf("%s >= %s %d\n", this->ToString(true).c_str(), _rOther.ToString(true).c_str(), Rts_B);
  return Rts_B;
}

bool BofGuid::IsValid() const
{
  return mpuBofGuidImplementation->IsValid();
}

void BofGuid::Clear()
{
  mpuBofGuidImplementation->Clear();
}

bool BofGuid::IsNull() const
{
  return (mpuBofGuidImplementation->IsNull());
}
END_BOF_NAMESPACE()