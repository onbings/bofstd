/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module provide type safe sprintf like functions with standard C and 'fmtlib' type
 * Fmtlib include also positionnal argument synthax
 *
 * Name:        bofstringformatter.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Based on https://github.com/fmtlib which is included in https://github.com/gabime/spdlog
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include <bofstd/bofstd.h>

#include <fmt/format.h>
#if defined(_WIN32)
#else
#include <fmt/printf.h>
#endif
#include <cstdarg>
 // http://fmtlib.net/latest/syntax.html
 // http://fmtlib.net/Text%20Formatting.html
 // http://fmtlib.net/dev/api.html#printf-formatting

BEGIN_BOF_NAMESPACE()

BOFSTD_EXPORT char *Bof_FastSnprintf(char *_pBuffer_c, uint32_t _MaxBufferSize_U32, const char *_pFormat_c, ...);

///@brief Writes formatted data to a string like the C standard version (sprintf)
///@@param _rFormat_S Specifies the Format-control string.
///@@param _Args Specifies the Optional arguments.
///@return A string containing the formatted data
template<typename ... Args>
std::string Bof_Sprintf(const std::string &_rFormat_S, Args ... _Args)
{
  size_t Size = snprintf(nullptr, 0, _rFormat_S.c_str(), _Args ...) + 1; // Extra space for '\0'
  std::unique_ptr<char[]> puInternalBuffer(new char[Size]);
  snprintf(puInternalBuffer.get(), Size, _rFormat_S.c_str(), _Args ...);
  return std::string(puInternalBuffer.get(), puInternalBuffer.get() + Size - 1);            // We don't want the '\0' inside
}
#if 0
template<typename ... Args>
std::string Bof_SafeSprintf(const std::string &_rFmt_S, const Args &... _Args)
{
  return fmt::sprintf(_rFmt_S, _Args ...);
}

template<typename ... Args>
std::wstring Bof_SafeSprintf(const std::wstring &_rFmt_WS, const Args &... _Args)
{
  return fmt::sprintf(_rFmt_WS, _Args ...);
}

template<typename ... Args>
std::string Bof_SafeFmtSprintf(const std::string &_rFmt_S, const Args &... _Args)
{
  return fmt::format(_rFmt_S, _Args ...);
}
template<typename ... Args>
std::wstring Bof_SafeFmtSprintf(const std::wstring &_rFmt_WS, const Args &... _Args)
{
  return fmt::format(_rFmt_WS, _Args ...);
}
#endif
END_BOF_NAMESPACE()