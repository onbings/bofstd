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

/*** Include files ***********************************************************/
#include <bofstd/bofstd.h>
#include <map>

BEGIN_BOF_NAMESPACE()

/*** Global variables ********************************************************/

/*** Definitions *************************************************************/

/*** Class *******************************************************************/
template<typename T>
constexpr typename std::underlying_type<T>::type Bof_EnumToNativeValue(T t)
{ return static_cast<typename std::underlying_type<T>::type>(t); }

/*

enum class ROOT_TREE_NODE :uint32_t
{
TRANSMITTER=0,
RECEIVER,
MANAGEMENT,
RPC,
ROOT_TREE_NODE_MAX,
UNDEF,
};

using RootTreeNodeEnumConverter = BOF::BofEnum<ROOT_TREE_NODE>;
static RootTreeNodeEnumConverter &S_RootTreeNodeConverter()
{
  static RootTreeNodeEnumConverter  S_TheRootTreeNodeConverter
  {
    {
      { ROOT_TREE_NODE::TRANSMITTER, "SND" },
      { ROOT_TREE_NODE::RECEIVER, "RCV" },
      { ROOT_TREE_NODE::MANAGEMENT, "MGT" },
      { ROOT_TREE_NODE::RPC, "?" },
      { ROOT_TREE_NODE::UNDEF, "?" }
    },
    ROOT_TREE_NODE::UNDEF
  };
  return S_TheRootTreeNodeConverter;
};
	std::string g;
	g=S_RootTreeNodeConverter().ToString(ROOT_TREE_NODE::MANAGEMENT);
	uint32_t f=S_RootTreeNodeConverter().ToBinary(ROOT_TREE_NODE::MANAGEMENT);
*/
///@brief This class represent a double mapping between unique instances of @c T and strings.
template<typename T>
class BOFSTD_EXPORT BofEnum
{
public:
		///@brief Constructor.
		///@param list A list of pairs of @c T - @c std::string associations.
		BofEnum(std::initializer_list<std::pair<const T, const std::string> > list) : typeToString(list)
		{
			for (const auto &kv : typeToString)
			{
				stringToType.insert(std::pair<const std::string *, const T>(&kv.second, kv.first));
			}
			fallbackValue = typeToString.begin()->first;
			hasFallbackValue = false;
		}

		///@brief Constructor.
		///@param list A list of pairs of @c T - @c std::string associations.
		///@param fbValue Fall-back value of @c T in cases where no decision can be made. This value shall be an item of @p list.
		BofEnum(std::initializer_list<std::pair<const T, const std::string> > list, const T &fbValue) : typeToString(list)
		{
			for (const auto &kv : typeToString)
			{
				stringToType.insert(std::pair<const std::string *, const T>(&kv.second, kv.first));
			}
			fallbackValue = fbValue;
			hasFallbackValue = true;
		}

		///@brief Returns the string associated to @p val.
		///@param val A value.
		///@returns The string associated to @p val, or, if @p val is unknown, the string associated to the fall-back value, if defined.
		///@throws std::out_of_range Thrown if @p val is unknown, and no fall-back value has been set.
		const std::string &ToString(const T &val) const
		{
			auto it = typeToString.find(val);
			if (it != typeToString.end())
			{
				return it->second;
			}
			if (hasFallbackValue)
			{
				return typeToString.at(fallbackValue);
			}
			throw std::out_of_range("Unknown value");
		}

		///@brief Returns the value associated to @p val.
		///@param val A string.
		///@returns The value associated to @p val, or, if @p val is unknown, the the fall-back value, if defined.
		///@throws std::out_of_range Thrown if @p val is unknown, and no fall-back value has been set.
		const T &ToEnum(const std::string &val) const
		{
			auto it = stringToType.find(&val);
			if (hasFallbackValue)
			{
				return (it == stringToType.end()) ? fallbackValue : it->second;
			}
			else if (it == stringToType.end())
			{
				throw std::out_of_range("Unknown value : " + val);
			}
			else
			{
				return it->second;
			}
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

		ItemToStringMap typeToString;

		StringToItemMap stringToType{[](const std::string *a, const std::string *b) { return a->compare(*b) < 0; }};
		T fallbackValue;
		bool hasFallbackValue;
};

END_BOF_NAMESPACE()