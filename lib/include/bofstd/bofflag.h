/*
* Copyright (c) 2015-2020, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines a general enum flag class. All the enum component can
* be ORed and ANDed
*
* Name:        bofflag.h
* Author:      Bernard HARMEL: onbings@gmail.com
* Web:			    onbings.dscloud.me
* Revision:    1.0
*
* Rem:         http://stackoverflow.com/questions/18803940/how-to-make-enum-class-to-work-with-the-bit-or-feature
*              http://stackoverflow.com/questions/21456262/enum-to-string-in-c11
*
* History:
*
* V 1.00  Dec 26 2013  BHA : Initial release
*/
#pragma once

#include <bofstd/bofstd.h>

#include <type_traits>

BEGIN_BOF_NAMESPACE()

/* Definition:

BEGIN_MY_NAMESPACE()
enum class GENLOCK_STATUS_FLAG : uint32_t
{
		UNKNOWN  = 0x0000,
		NOT_LOCKED      = 0x0001,
		NOT_ALIGNED     = 0x0002,
		NO_REFERENCE    = 0x0004
};
END_MY_NAMESPACE()

BEGIN_BOF_NAMESPACE()
template< >
struct IsItAnEnumBitFLag< MMGW_NAMESPACE::GENLOCK_STATUS_FLAG > : std::true_type {};
END_BOF_NAMESPACE()

Usage:
USE_BOF_NAMESPACE()
VideoGeneratorFlag_E = BOFAV_NAMESPACE::BOFAV_VIDEO_GENERATOR_FLAG::BOFAV_VIDEO_GENERATOR_FLAG_COUNTER | BOFAV_NAMESPACE::BOFAV_VIDEO_GENERATOR_FLAG::BOFAV_VIDEO_GENERATOR_FLAG_MOVING_HORIZONTAL_LINE | BOFAV_NAMESPACE::BOFAV_VIDEO_GENERATOR_FLAG::BOFAV_VIDEO_GENERATOR_FLAG_MOVING_VERTICAL_LINE | BOFAV_NAMESPACE::BOFAV_VIDEO_GENERATOR_FLAG::BOFAV_VIDEO_GENERATOR_FLAG_TIME_CODE;
ReferenceStatus_E = ReferenceStatus_E | GENLOCK_STATUS_FLAG::NOT_LOCKED;
ReferenceStatus_E++;
VideoGeneratorFlag_E++;
if (Bof_IsBitFlagSet(BofAvVideoGeneratorParam_X.VideoGeneratorFlag_E, BOFAV_NAMESPACE::BOFAV_VIDEO_GENERATOR_FLAG::BOFAV_VIDEO_GENERATOR_FLAG_COUNTER))
if (
*/
// Intentionally undefined for non-enum types.
template<typename T, bool = std::is_enum<T>::value>
struct IsItAnEnumBitFLag;

template<typename T>
struct IsItAnEnumBitFLag<T, true> : std::false_type
{
};

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator|(T _Lhs, T _Rhs)
{
	using u_t = typename std::underlying_type<T>::type;
	return static_cast< T > (static_cast< u_t > (_Lhs) | static_cast< u_t > (_Rhs));
}

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T &operator|=(T &_rLhs, T _Rhs)
{
	_rLhs = _rLhs | _Rhs;
	return _rLhs;
}

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator&(T _Lhs, T _Rhs)
{
	using u_t = typename std::underlying_type<T>::type;
	return static_cast< T > (static_cast< u_t > (_Lhs) & static_cast< u_t > (_Rhs));
}

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator^(T _Lhs, T _Rhs)
{
	using u_t = typename std::underlying_type<T>::type;
	return static_cast< T > (static_cast< u_t > (_Lhs) ^ static_cast< u_t > (_Rhs));
}

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator~(T _Flag)
{
	using u_t = typename std::underlying_type<T>::type;
	return static_cast< T > (~static_cast< u_t > (_Flag));
}

//The int is used to distinguish the post from the pre form
// postfix ++
template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator++(T &_rFlag, int)
{
	using u_t = typename std::underlying_type<T>::type;
	T Cpy = _rFlag;
	u_t Val = static_cast<u_t>(_rFlag);
	Val++;
	_rFlag = static_cast<T>(Val);
	return Cpy;
}

// ++prefix
template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator++(T &_rFlag)
{
	using u_t = typename std::underlying_type<T>::type;
	u_t Val = static_cast<u_t>(_rFlag);
	Val++;
	_rFlag = static_cast<T>(Val);
	return _rFlag;
}

//The int is used to distinguish the post from the pre form
// postfix --
template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator--(T &_rFlag, int)
{
	using u_t = typename std::underlying_type<T>::type;
	T Cpy = _rFlag;
	u_t Val = static_cast<u_t>(_rFlag);
	Val--;
	_rFlag = static_cast<T>(Val);
	return Cpy;
}

// --prefix
template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
T operator--(T &_rFlag)
{
	using u_t = typename std::underlying_type<T>::type;
	u_t Val = static_cast<u_t>(_rFlag);
	Val--;
	_rFlag = static_cast<T>(Val);
	return _rFlag;
}

template<typename T, typename std::enable_if<IsItAnEnumBitFLag<T>::value>::type * = nullptr>
bool Bof_IsBitFlagSet(T _Lhs, T _Rhs)
{
	using u_t = typename std::underlying_type<T>::type;
	return ((static_cast< u_t > (_Lhs) & static_cast< u_t > (_Rhs)) ? true : false);
}

END_BOF_NAMESPACE()