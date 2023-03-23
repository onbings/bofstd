/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofguid class
 *
 * Name:        bofbit.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Based on https://github.com/graeme-hill/crossguid
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>
/*
* c++ lib open source
https://github.com/graeme-hill/crossguid
https://think-async.com/Asio/Documentation.html
https://itecnote.com/tecnote/enumerating-ipv4-and-ipv6-address-of-the-cards-using-boost-asio/
https://stackoverflow.com/questions/6327210/enumerating-ipv4-and-ipv6-address-of-my-cards-using-boost-asio
https://stackoverflow.com/questions/20890703/how-to-use-a-specfic-networkinterface-ip-with-boost-asio-sockets
*/
#if 0
#include <memory>
#include <vector>

BEGIN_BOF_NAMESPACE()

class BOFSTD_EXPORT BofGuid
{
  // Opaque pointer design pattern: all public and protected stuff goes here ...
public:
  BofGuid();

  BofGuid(const uint8_t *_pData_U8);

  BofGuid(const std::vector<uint8_t> &_rData);

  BofGuid(const std::string &_rUuidStr_S);  //_rUuidStr_S connonical or not
  virtual ~BofGuid();

  bool IsValid() const;

  void Clear();

  bool IsNull() const;

  const std::vector<uint8_t> Data() const;

  const uint8_t *Data(size_t &_rSize) const;

  std::string ToString(bool _Cannonical_B) const;

  BofGuid(const BofGuid &_rOther_O);

  BofGuid(BofGuid &&_rrOther_O);

  BofGuid &operator=(const BofGuid &_rOther_O);

  BofGuid &operator=(BofGuid &&_rrOther_O);

  BofGuid &operator=(const char *_pUuidStr_c);

  BofGuid &operator=(const std::string &_rUuidStr_S);

  bool operator<(const BofGuid &_rOther) const;

  bool operator<=(const BofGuid &_rOther) const;

  bool operator==(const BofGuid &_rOther) const;

  bool operator!=(const BofGuid &_rOther) const;

  bool operator>(const BofGuid &_rOther) const;

  bool operator>=(const BofGuid &_rOther) const;

  // Opaque pointer design pattern: opaque type here
private:
  class BofGuidImplementation;

  std::unique_ptr<BofGuidImplementation> mpuBofGuidImplementation;
};

END_BOF_NAMESPACE()

#else

#pragma once

#ifdef GUID_ANDROID
#include <jni.h>
#include <thread>
#endif

#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

BEGIN_BOF_NAMESPACE()

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
class BOFSTD_EXPORT BofGuid
{
public:
  BofGuid();
  explicit BofGuid(const std::array<uint8_t, 16> &_rByteCollection);
  explicit BofGuid(std::array<uint8_t, 16> &&_rrByteCollection);
  explicit BofGuid(const std::vector<uint8_t> &_rByteCollection);
  explicit BofGuid(const uint8_t *_pData_U8);
  explicit BofGuid(const std::string &_rString_S);

  BofGuid(const BofGuid &_rOther) = default;
  BofGuid &operator=(const BofGuid &_rOther) = default;
  BofGuid(BofGuid &&_rOther) = default;
  BofGuid &operator=(BofGuid &&_rrOther);

  bool operator==(const BofGuid &_rOther) const;
  bool operator!=(const BofGuid &_rOther) const;
  bool operator<(const BofGuid &_rOther) const; //, const BofGuid &rhs);
  bool operator>(const BofGuid &_rOther) const;

  std::string ToString(bool _Cannonical_B) const;
  bool FromString(const std::string &_rString_S);
  operator std::string() const;
  const std::array<uint8_t, 16> &Data() const;
  void swap(BofGuid &other);
  bool IsValid() const;
  void Clear();
  bool IsNull() const;

private:
  // actual data
  std::array<uint8_t, 16> mGuidByteCollection;
  uint8_t HexDigitToChar(char _Ch);
  bool IsValidHexChar(char _Ch_c);
  uint8_t HexPairToChar(char _High_c, char _Low_c);
  // make the << operator a friend so it can access _bytes
  friend std::ostream &operator<<(std::ostream &s, const BofGuid &guid);
};

BofGuid newGuid();

#ifdef GUID_ANDROID
struct AndroidGuidInfo
{
  static AndroidGuidInfo fromJniEnv(JNIEnv *env);

  JNIEnv *env;
  jclass uuidClass;
  jmethodID newGuidMethod;
  jmethodID mostSignificantBitsMethod;
  jmethodID leastSignificantBitsMethod;
  std::thread::id initThreadId;
};

extern AndroidGuidInfo androidInfo;

void initJni(JNIEnv *env);

// overloading for multi-threaded calls
Guid newGuid(JNIEnv *env);
#endif

namespace details
{
template <typename...> struct hash;

template <typename T> struct hash<T> : public std::hash<T>
{
  using std::hash<T>::hash;
};

template <typename T, typename... Rest> struct hash<T, Rest...>
{
  inline std::size_t operator()(const T &v, const Rest &...rest)
  {
    std::size_t seed = hash<Rest...>{}(rest...);
    seed ^= hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};
} // namespace details

END_BOF_NAMESPACE()

namespace std
{
// Template specialization for std::swap<Guid>() --
// See guid.cpp for the function definition
template <> void swap(BOF::BofGuid &_rGuid1, BOF::BofGuid &_rGuid2);

// Specialization for std::hash<Guid> -- this implementation
// uses std::hash<std::string> on the stringification of the guid
// to calculate the hash
template <> struct hash<BOF::BofGuid>
{
  std::size_t operator()(BOF::BofGuid const &_rGuid) const
  {
    const uint64_t *p_U64 = reinterpret_cast<const uint64_t *>(_rGuid.Data().data());
    return BOF::details::hash<uint64_t, uint64_t>{}(p_U64[0], p_U64[1]);
  }
};
} // namespace std
#endif
