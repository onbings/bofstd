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
#include <bofstd/bofstring.h>
#include <bofstd/bofsystem.h>
#include <cstring>

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

#ifdef GUID_ANDROID
#include <cassert>
#include <jni.h>
#endif

BEGIN_BOF_NAMESPACE()

#ifdef GUID_ANDROID
AndroidGuidInfo androidInfo;

AndroidGuidInfo AndroidGuidInfo::fromJniEnv(JNIEnv *env)
{
  AndroidGuidInfo info;
  info.env = env;
  auto localUuidClass = env->FindClass("java/util/UUID");
  info.uuidClass = (jclass)env->NewGlobalRef(localUuidClass);
  env->DeleteLocalRef(localUuidClass);
  info.newGuidMethod = env->GetStaticMethodID(info.uuidClass, "randomUUID", "()Ljava/util/UUID;");
  info.mostSignificantBitsMethod = env->GetMethodID(info.uuidClass, "getMostSignificantBits", "()J");
  info.leastSignificantBitsMethod = env->GetMethodID(info.uuidClass, "getLeastSignificantBits", "()J");
  info.initThreadId = std::this_thread::get_id();
  return info;
}

void initJni(JNIEnv *env)
{
  androidInfo = AndroidGuidInfo::fromJniEnv(env);
}
#endif
// create empty guid
/*
Version 1 (Time-based): Combines the current timestamp with the MAC address of the machine generating the UUID. 
This version has a 60-bit timestamp and a clock sequence that helps manage cases where multiple UUIDs are generated 
in the same timestamp.

Version 4 (Random): Generated using random or pseudo-random numbers. It doesn't rely on the current time or a 
unique identifier from the machine. This version is suitable when a UUID needs to be generated without access to a unique identifier 
or when you want to avoid any potential predictability.

Windows:
GUID guid; wchar_t guidString[40];
Generate a new GUID  if (CoCreateGuid(&guid) == S_OK) {StringFromGUID2(guid, guidString, sizeof(guidString) / sizeof(guidString[0]));

Linux:
uuid_t uuid;char uuidString[37]; // UUIDs are 36 characters long plus null-terminator
uuid_generate(uuid); uuid_unparse(uuid, uuidString);


*/
BofGuid::BofGuid()
{
  std::string Guid_S = Bof_RandomHexa(true, mGuidByteCollection.size(), false);
  FromString(Guid_S);
}

// create a guid from vector of bytes
BofGuid::BofGuid(const std::array<uint8_t, 16> &_rByteCollection) : mGuidByteCollection(_rByteCollection)
{
}

// create a guid from vector of bytes
BofGuid::BofGuid(std::array<uint8_t, 16> &&_rrByteCollection) : mGuidByteCollection(std::move(_rrByteCollection))
{
}
BofGuid::BofGuid(const std::vector<uint8_t> &_rByteCollection)
{
  size_t i;
  for (i = 0; i < mGuidByteCollection.size(); i++)
  {
    mGuidByteCollection[i] = _rByteCollection.data()[i];
  }
}

BofGuid::BofGuid(const uint8_t *_pData_U8)
{
  size_t i;
  for (i = 0; i < mGuidByteCollection.size(); i++)
  {
    mGuidByteCollection[i] = _pData_U8[i];
  }
}

// create a guid from string
BofGuid::BofGuid(const std::string &_rString_S)
{
  char One_c = '\0';
  char Two_c = '\0';
  bool LookingForFirstChar_B = true;
  uint32_t Index_U32 = 0;
  uint8_t Ch_U8;
  std::string String_S = _rString_S;

  String_S = Bof_StringTrim(String_S);
  if ((String_S[0] == '{') && (String_S[String_S.size() - 1] == '}'))
  {
    String_S = String_S.substr(1, String_S.size() - 2);
  }
  for (const char &rIt : String_S)
  {
    if (rIt == '-')
    {
      continue;
    }
    if ((Index_U32 >= 16) || !IsValidHexChar(rIt))
    {
      // Invalid string so bail
      Clear();
      break;
    }

    if (LookingForFirstChar_B)
    {
      One_c = rIt;
      LookingForFirstChar_B = false;
    }
    else
    {
      Two_c = rIt;
      Ch_U8 = HexPairToChar(One_c, Two_c);
      mGuidByteCollection[Index_U32++] = Ch_U8;
      LookingForFirstChar_B = true;
    }
  }

  // if there were fewer than 16 bytes in the string then guid is bad
  if (Index_U32 < 16)
  {
    Clear();
  }
}

// overload << so that it's easy to convert to a string
std::ostream &operator<<(std::ostream &_rOs, const BofGuid &_rGuid)
{
  std::ios_base::fmtflags Flag(_rOs.flags()); // politely don't leave the ostream in hex mode
  _rOs << std::hex << std::setfill('0') << std::setw(2) << (int)_rGuid.mGuidByteCollection[0] << std::setw(2) << (int)_rGuid.mGuidByteCollection[1] << std::setw(2) << (int)_rGuid.mGuidByteCollection[2] << std::setw(2) << (int)_rGuid.mGuidByteCollection[3]
       << "-" << std::setw(2) << (int)_rGuid.mGuidByteCollection[4] << std::setw(2) << (int)_rGuid.mGuidByteCollection[5] << "-" << std::setw(2) << (int)_rGuid.mGuidByteCollection[6] << std::setw(2) << (int)_rGuid.mGuidByteCollection[7] << "-"
       << std::setw(2) << (int)_rGuid.mGuidByteCollection[8] << std::setw(2) << (int)_rGuid.mGuidByteCollection[9] << "-" << std::setw(2) << (int)_rGuid.mGuidByteCollection[10] << std::setw(2) << (int)_rGuid.mGuidByteCollection[11] << std::setw(2)
       << (int)_rGuid.mGuidByteCollection[12] << std::setw(2) << (int)_rGuid.mGuidByteCollection[13] << std::setw(2) << (int)_rGuid.mGuidByteCollection[14] << std::setw(2) << (int)_rGuid.mGuidByteCollection[15];
  _rOs.flags(Flag);
  return _rOs;
}

bool BofGuid::operator<(const BofGuid &_rOther) const //, const BofGuid &rhs)
{
  return this->Data() < _rOther.Data();
}
bool BofGuid::operator>(const BofGuid &_rOther) const
{
  return this->Data() > _rOther.Data();
}
bool BofGuid::IsValid() const
{
  return IsNull();
}

// convert to string using std::snprintf() and std::string
std::string BofGuid::ToString(bool _Cannonical_B) const
{
  std::string Rts_S;
  char pOne_c[10], pTwo_c[6], pThree_c[6], pFour_c[6], pFive_c[14];

  snprintf(pOne_c, 10, "%02x%02x%02x%02x", mGuidByteCollection[0], mGuidByteCollection[1], mGuidByteCollection[2], mGuidByteCollection[3]);
  snprintf(pTwo_c, 6, "%02x%02x", mGuidByteCollection[4], mGuidByteCollection[5]);
  snprintf(pThree_c, 6, "%02x%02x", mGuidByteCollection[6], mGuidByteCollection[7]);
  snprintf(pFour_c, 6, "%02x%02x", mGuidByteCollection[8], mGuidByteCollection[9]);
  snprintf(pFive_c, 14, "%02x%02x%02x%02x%02x%02x", mGuidByteCollection[10], mGuidByteCollection[11], mGuidByteCollection[12], mGuidByteCollection[13], mGuidByteCollection[14], mGuidByteCollection[15]);
  Rts_S = pOne_c;

  if (_Cannonical_B) // 6ba39f88-ed13-8c48-a765-a8738dba383e
  {
    Rts_S += "-" + std::string(pTwo_c);
    Rts_S += "-" + std::string(pThree_c);
    Rts_S += "-" + std::string(pFour_c);
    Rts_S += "-" + std::string(pFive_c);
  }
  else // 6BA39F88ED138C48A765A8738DBA383E
  {
    Rts_S += pTwo_c;
    Rts_S += pThree_c;
    Rts_S += pFour_c;
    Rts_S += pFive_c;
    Rts_S = Bof_StringToUpper(Rts_S);
  }

  return Rts_S;
}
BofGuid &BofGuid::operator=(BofGuid &&_rrOther)
{
  FromString(_rrOther.ToString(false));
  _rrOther.Clear();
  return *this;
}

// conversion operator for std::string
BofGuid::operator std::string() const
{
  return ToString(true);
}

// Access underlying bytes
const std::array<uint8_t, 16> &BofGuid::Data() const
{
  return mGuidByteCollection;
}

// converts a single hex char to a number (0 - 15)
uint8_t BofGuid::HexDigitToChar(char _Ch_c)
{
  uint8_t Rts_U8 = 0;

  // 0-9
  if ((_Ch_c >= '0') && (_Ch_c <= '9'))
  {
    Rts_U8 = _Ch_c - '0';
  }
  else
  {
    // a-f
    if ((_Ch_c >= 'a') && (_Ch_c <= 'f'))
    {
      Rts_U8 = _Ch_c - 'a' + 10;
    }
    else
    {
      // A-F
      if ((_Ch_c >= 'A') && (_Ch_c <= 'F'))
      {
        Rts_U8 = _Ch_c - 'A' + 10;
      }
    }
  }
  return Rts_U8;
}

bool BofGuid::IsValidHexChar(char _Ch_c)
{
  bool Rts_B = false;

  // 0-9
  if ((_Ch_c >= '0') && (_Ch_c <= '9'))
  {
    Rts_B = true;
  }
  else
  {
    // a-f
    if ((_Ch_c >= 'a') && (_Ch_c <= 'f'))
    {
      Rts_B = true;
    }
    else
    {
      // A-F
      if ((_Ch_c >= 'A') && (_Ch_c <= 'F'))
      {
        Rts_B = true;
      }
    }
  }
  return Rts_B;
}

// converts the two hexadecimal characters to an uint8_t (a byte)
uint8_t BofGuid::HexPairToChar(char _High_c, char _Low_c)
{
  return (HexDigitToChar(_High_c) * 16) + HexDigitToChar(_Low_c);
}

// set all bytes to zero
void BofGuid::Clear()
{
  std::fill(mGuidByteCollection.begin(), mGuidByteCollection.end(), static_cast<uint8_t>(0));
}

bool BofGuid::IsNull() const
{
  bool Rts_B = true;
  size_t i;
  for (i = 0; i < mGuidByteCollection.size(); i++)
  {
    if (mGuidByteCollection[i])
    {
      Rts_B = false;
      break;
    }
  }
  return Rts_B;
}
bool BofGuid::FromString(const std::string &_rString_S)
{
  bool Rts_B = true;
  char One_c = '\0';
  char Two_c = '\0';
  bool LookingForFirstChar_B = true;
  uint32_t Index_U32 = 0;
  uint8_t Ch_U8;

  for (const char &rIt : _rString_S)
  {
    if (rIt == '-')
    {
      continue;
    }
    if ((Index_U32 >= 16) || !IsValidHexChar(rIt))
    {
      // Invalid string so bail
      Rts_B = false;
      break;
    }

    if (LookingForFirstChar_B)
    {
      One_c = rIt;
      LookingForFirstChar_B = false;
    }
    else
    {
      Two_c = rIt;
      Ch_U8 = HexPairToChar(One_c, Two_c);
      mGuidByteCollection[Index_U32++] = Ch_U8;
      LookingForFirstChar_B = true;
    }
  }

  // if there were fewer than 16 bytes in the string then guid is bad
  if (Index_U32 < 16)
  {
    Rts_B = false;
  }

  if (!Rts_B)
  {
    Clear();
  }

  return Rts_B;
}

// overload equality operator
bool BofGuid::operator==(const BofGuid &other) const
{
  return mGuidByteCollection == other.mGuidByteCollection;
}

// overload inequality operator
bool BofGuid::operator!=(const BofGuid &other) const
{
  return !((*this) == other);
}

// member swap function
void BofGuid::swap(BofGuid &other)
{
  mGuidByteCollection.swap(other.mGuidByteCollection);
}

// This is the linux friendly implementation, but it could work on other
// systems that have libuuid available
#ifdef GUID_LIBUUID
Guid newGuid()
{
  std::array<uint8_t, 16> data;
  static_assert(std::is_same<uint8_t[16], uuid_t>::value, "Wrong type!");
  uuid_generate(data.data());
  return Guid{std::move(data)};
}
#endif

// this is the mac and ios version
#ifdef GUID_CFUUID
Guid newGuid()
{
  auto newId = CFUUIDCreate(nullptr);
  auto bytes = CFUUIDGetUUIDBytes(newId);
  CFRelease(newId);

  std::array<uint8_t, 16> byteArray = {{bytes.byte0, bytes.byte1, bytes.byte2, bytes.byte3, bytes.byte4, bytes.byte5, bytes.byte6, bytes.byte7, bytes.byte8, bytes.byte9, bytes.byte10, bytes.byte11, bytes.byte12, bytes.byte13, bytes.byte14, bytes.byte15}};
  return Guid{std::move(byteArray)};
}
#endif

// obviously this is the windows version
#ifdef GUID_WINDOWS
Guid newGuid()
{
  GUID newId;
  CoCreateGuid(&newId);

  std::array<uint8_t, 16> bytes = {(uint8_t)((newId.Data1 >> 24) & 0xFF),
                                   (uint8_t)((newId.Data1 >> 16) & 0xFF),
                                   (uint8_t)((newId.Data1 >> 8) & 0xFF),
                                   (uint8_t)((newId.Data1) & 0xff),

                                   (uint8_t)((newId.Data2 >> 8) & 0xFF),
                                   (uint8_t)((newId.Data2) & 0xff),

                                   (uint8_t)((newId.Data3 >> 8) & 0xFF),
                                   (uint8_t)((newId.Data3) & 0xFF),

                                   (uint8_t)newId.Data4[0],
                                   (uint8_t)newId.Data4[1],
                                   (uint8_t)newId.Data4[2],
                                   (uint8_t)newId.Data4[3],
                                   (uint8_t)newId.Data4[4],
                                   (uint8_t)newId.Data4[5],
                                   (uint8_t)newId.Data4[6],
                                   (uint8_t)newId.Data4[7]};

  return Guid{std::move(bytes)};
}
#endif

// android version that uses a call to a java api
#ifdef GUID_ANDROID
Guid newGuid(JNIEnv *env)
{
  assert(env != androidInfo.env || std::this_thread::get_id() == androidInfo.initThreadId);

  jobject javaUuid = env->CallStaticObjectMethod(androidInfo.uuidClass, androidInfo.newGuidMethod);
  jlong mostSignificant = env->CallLongMethod(javaUuid, androidInfo.mostSignificantBitsMethod);
  jlong leastSignificant = env->CallLongMethod(javaUuid, androidInfo.leastSignificantBitsMethod);

  std::array<uint8_t, 16> bytes = {(uint8_t)((mostSignificant >> 56) & 0xFF),  (uint8_t)((mostSignificant >> 48) & 0xFF),  (uint8_t)((mostSignificant >> 40) & 0xFF),  (uint8_t)((mostSignificant >> 32) & 0xFF),
                                   (uint8_t)((mostSignificant >> 24) & 0xFF),  (uint8_t)((mostSignificant >> 16) & 0xFF),  (uint8_t)((mostSignificant >> 8) & 0xFF),   (uint8_t)((mostSignificant)&0xFF),
                                   (uint8_t)((leastSignificant >> 56) & 0xFF), (uint8_t)((leastSignificant >> 48) & 0xFF), (uint8_t)((leastSignificant >> 40) & 0xFF), (uint8_t)((leastSignificant >> 32) & 0xFF),
                                   (uint8_t)((leastSignificant >> 24) & 0xFF), (uint8_t)((leastSignificant >> 16) & 0xFF), (uint8_t)((leastSignificant >> 8) & 0xFF),  (uint8_t)((leastSignificant)&0xFF)};

  env->DeleteLocalRef(javaUuid);

  return Guid{std::move(bytes)};
}

Guid newGuid()
{
  return newGuid(androidInfo.env);
}
#endif

END_BOF_NAMESPACE()

// Specialization for std::swap<Guid>() --
// call member swap function of lhs, passing rhs
namespace std
{
template <> void swap(BOF::BofGuid &lhs, BOF::BofGuid &rhs)
{
  lhs.swap(rhs);
}
} // namespace std
