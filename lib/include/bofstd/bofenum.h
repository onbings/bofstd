/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof enum class
 *
 * Name:        bofenum.h
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

#include <bofstd/bofstd.h>

#include <map>

BEGIN_BOF_NAMESPACE()

template<typename T>
constexpr typename std::underlying_type<T>::type Bof_EnumToNativeValue(T t)
{
  return static_cast<typename std::underlying_type<T>::type>(t);
}

/*
static BofEnum<MUSE_FILE_SYSTEM_MEDIA_TYPE> S_MuseFileSystemMediaTypeEnumConverter({
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_UNKNOWN, "Unknown" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_STILL, "Still" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_CLIP, "Clip" },
  { MUSE_FILE_SYSTEM_MEDIA_TYPE::MUSE_FILE_SYSTEM_MEDIA_TYPE_MAX, "Max" }
                                                                          });
  std::string Tp_S = S_MuseFileSystemMediaTypeEnumConverter.ToString(S_MuseFileSystemMediaStillJson_X.General_X.MediaType_E);
  MUSE_FILE_SYSTEM_MEDIA_TYPE Tp_E = S_MuseFileSystemMediaTypeEnumConverter.ToEnum(Tp_S);

*/

//https://stackoverflow.com/questions/14727313/c-how-to-reference-templated-functions-using-stdbind-stdfunction
//https://stackoverflow.com/questions/24874478/use-stdbind-with-overloaded-functions
///@brief This class represent a double mapping between unique instances of @c T and strings.
template<typename T>
class BOFSTD_EXPORT BofEnum
{
public:
  ///@brief Constructor.
  ///@param list A list of pairs of @c T - @c std::string associations.
  BofEnum(std::initializer_list<std::pair<const T, const std::string> > list) : mTypeToStringCollection(list)
  {
    for (const auto &kv : mTypeToStringCollection)
    {
      mStringToTypeCollection.insert(std::pair<const std::string *, const T>(&kv.second, kv.first));
    }
    mFallbackValue = mTypeToStringCollection.begin()->first;
    mFallbackValue_B = false;
  }

  ///@brief Constructor.
  ///@param list A list of pairs of @c T - @c std::string associations.
  ///@param fbValue Fall-back value of @c T in cases where no decision can be made. This value shall be an item of @p list.
  BofEnum(std::initializer_list<std::pair<const T, const std::string> > list, const T &fbValue) : mTypeToStringCollection(list)
  {
    for (const auto &kv : mTypeToStringCollection)
    {
      mStringToTypeCollection.insert(std::pair<const std::string *, const T>(&kv.second, kv.first));
    }
    mFallbackValue = fbValue;
    mFallbackValue_B = true;
  }

  ///@brief Returns the string associated to @p val.
  ///@param val A value.
  ///@returns The string associated to @p val, or, if @p val is unknown, the string associated to the fall-back value, if defined.
  ///@throws std::out_of_range Thrown if @p val is unknown, and no fall-back value has been set.
  const std::string &ToString(const T &_rVal) const
  {
    auto It = mTypeToStringCollection.find(_rVal);
    if (It != mTypeToStringCollection.end())
    {
      return It->second;
    }
    if (mFallbackValue_B)
    {
      return mTypeToStringCollection.at(mFallbackValue);
    }
    throw std::out_of_range("Unknown value");
  }
  const std::string &ToStringFromInt(int _EnumVal_i) const
  {
    return ToString(static_cast<T>(_EnumVal_i));
  }

  ///@brief Returns the value associated to @p val.
  ///@param val A string.
  ///@returns The value associated to @p val, or, if @p val is unknown, the the fall-back value, if defined.
  ///@throws std::out_of_range Thrown if @p val is unknown, and no fall-back value has been set.
  const T &ToEnum(const std::string &_rVal_S) const
  {
    auto It = mStringToTypeCollection.find(&_rVal_S);
    if (mFallbackValue_B)
    {
      return (It == mStringToTypeCollection.end()) ? mFallbackValue : It->second;
    }
    else if (It == mStringToTypeCollection.end())
    {
      throw std::out_of_range("Unknown value : " + _rVal_S);
    }
    else
    {
      return It->second;
    }
  }
  int FromStringToInt(const std::string &_rVal_S) const
  {
    return static_cast<int>(ToEnum(_rVal_S));
  }
  typename std::underlying_type<T>::type ToBinary(T e)
  {
    return static_cast<typename std::underlying_type<T>::type>(e);
  }

  T FromBinary(typename std::underlying_type<T>::type Val)
  {
    return static_cast<T>(Val);
  }

private:
  using ItemToStringMap = std::map<const T, const std::string>;
  using StringToItemMap = std::map<const std::string *, const T, std::function<bool(const std::string *, const std::string *)> >;

  ItemToStringMap mTypeToStringCollection;
  StringToItemMap mStringToTypeCollection{ [](const std::string *a, const std::string *b) { return a->compare(*b) < 0; } };
  T mFallbackValue;
  bool mFallbackValue_B;
};

END_BOF_NAMESPACE()